//********************************************************************/
//Dillon Prendergast, John Thomas Lascha, Zachary Timmerman
//COP4610
//Operating Systems
//Assignment 3- FAT32 File System
//********************************************************************/

/*****************************LIBRARIES******************************/
#include <stdio.h>	//printf
#include <string.h>	//strcmp
#include <stdbool.h>	//bool
#include <stdlib.h>	//atoi

/*******************************STRUCTS*******************************/

//Order of this matters for reading in info directly from image
struct FAT32BootBlock{
	unsigned char BS_jmpBoot[3];
	unsigned char BS_OEMName[8];
	unsigned short BPB_BytsPerSec;
	unsigned char BPB_SecPerClus;
	unsigned short BPB_RsvdSecCnt;
	unsigned char BPB_NumFATs;
	unsigned short BPB_RootEntCnt;
	unsigned short BPB_TotSec16;
	unsigned char BPB_Media;
	unsigned short BPB_FATSz16;
	unsigned short BPB_SecPerTrk;
	unsigned short BPB_NumHeads;
	unsigned int BPB_HiddSec;
	unsigned int BPB_TotSec32;
	unsigned int BPB_FATSz32;
	unsigned short BPB_ExtFlags;
	unsigned short BPB_FSVer;
	unsigned int BPB_RootClus;
	unsigned short BPB_FSInfo;
	unsigned short BPB_BkBootSec;
	unsigned char BPB_Reserved[12];
	unsigned char BS_DrvNum;
	unsigned char BS_Reserved1;
	unsigned char BS_BootSig;
	unsigned int BS_VolID;
	unsigned char BS_VolLab[11];
	unsigned char BS_FilSysType[8];
}__attribute((packed));

//Long Entry appears first in hex
struct DirectoryEntry{
	unsigned char DIR_Name[11];
	unsigned char DIR_Attr;
	unsigned char DIR_NTRes;
	unsigned char DIR_CrtTimeTenth;
	unsigned short DUR_CrtTime;
	unsigned short DIR_CrtDate;
	unsigned short DIR_LstAccDate;
	unsigned short DIR_FstClusHI;
	unsigned short DIR_WrtTime;
	unsigned short DIR_WrtDate;
	unsigned short DIR_FstClusLO;
	unsigned int DIR_FileSize;
}__attribute((packed));

//Long Entry appears first in hex
struct LongDirectoryEntry{
	unsigned char LDIR_Ord;
	unsigned char LDIR_Name1[10];
	unsigned char LDIR_Attr;
	unsigned char LDIR_Type;
	unsigned char LDIR_Chksum;
	unsigned char LDIR_Name2[12];
	unsigned short LDIR_FstClusLO;
	unsigned char LDIR_Name3[4];
}__attribute((packed));
struct OpenFileEntry{
	unsigned char name[26];
	unsigned char mode[4];
	bool used;
	int cluster;
};
/*******************************GLOBAL*****************************/
FILE *file;
char *fatName;
struct FAT32BootBlock bpb;
struct DirectoryEntry dir;
struct LongDirectoryEntry ldir;
unsigned int currClus;
unsigned int fstDataSec;
unsigned int fstSecClus;
//unsigned char name[11];
unsigned char name[26];
unsigned int depth=0;
unsigned int parent[100];	//system can go 100 directory deep
struct OpenFileEntry openlist[25];

/****************************FUNCTIONS*****************************/
/*************************UTILITIES********************************/
unsigned int SectorOffset(unsigned int N){
	return bpb.BPB_BytsPerSec*N;
}

unsigned int FirstDataSector(){
	return bpb.BPB_RsvdSecCnt+(bpb.BPB_NumFATs*bpb.BPB_FATSz32);
}

unsigned int FirstSectorCluster(unsigned int N){
	return ((N-2)*bpb.BPB_SecPerClus)+FirstDataSector();
}

/***************************OPEN UTILTIES**********************************/
int firstOpen(){
 for(int i = 0; i < 25; i++){
	if(openlist[i].used == false){return i;}
 }
return -1;// return -1 as an error
}
void initOpen(){
 for(int i = 0; i < 25; i++){
	openlist[i].used = false;
 }
}

int openSearch(char * name){
 for(int i = 0; i < 25; i++){
  if(openlist[i].used){
   if(strcmp(openlist[i].name,name)==0){
    return i;
   }
  }
 }
return -1; // if the file isn't open, return -1 as an error
}

/**************************OPEN*******************************************/

void open(){
  unsigned int offset;
  unsigned int c=currClus;
  bool found=0;
  unsigned char mode[4];
  scanf("%s",name);
  scanf("%s",mode);
    if(strcmp(mode,"r")!=0 && strcmp(mode,"w")!=0 && strcmp(mode,"rw")!=0 && strcmp(mode,"wr")!=0){
	//if mode is not "r", "w", "rw", or "wr", it's invalid, so print an error and stop.
	printf("Invalid mode.\n");
	return;
    }

    offset=SectorOffset(FirstSectorCluster(c));
    fseek(file,offset,SEEK_SET);
    unsigned int temp=offset+bpb.BPB_BytsPerSec*bpb.BPB_SecPerClus;
    while(temp>offset){
	//fills our dir struct
      fseek(file,offset,SEEK_SET);
      fread(&ldir,sizeof(struct LongDirectoryEntry),1,file);
      fread(&dir,sizeof(struct DirectoryEntry),1,file);
      unsigned char fname[26]={0};

	//skips the . and .. entries
    if(ldir.LDIR_Ord=='A'){
      for(int i=0,k=0;i<10;i+=2){
  	fname[k]=ldir.LDIR_Name1[i];
	if(fname[k]=='\0')
	  break;
	k++;
	if(i==8){
	  for(int j=0;j<12;j+=2){
	    fname[k]=ldir.LDIR_Name2[j];
	    if(fname[k]=='\0')
	      break;
	    k++;
	    if(j==10){
	      for(int l=0;l<4;l+=2){
	        fname[k]=ldir.LDIR_Name3[l];
	        if(fname[k]=='\0')
	          break;
	        k++;
	      }
	    }
	  }
	}
      }

      if(strcmp(name,fname)==0){
	int oi = firstOpen();	//oi stands for open index
	if(oi == -1){
	  printf("We couldn't open your file because there are too many open files.\n");
	  return;
	}
	if(openSearch(name) == -1){
	 strcpy(openlist[oi].name, name);
	 strcpy(openlist[oi].mode, mode);
//	 openlist[oi].cluster = offset;	// I'm not sure, but I think this is right.
	 openlist[oi].cluster =SectorOffset(FirstSectorCluster(dir.DIR_FstClusHI*0x100+dir.DIR_FstClusLO)); // this is what Dillon suggested
         openlist[oi].cluster -= 32; //for some reason, this is need to let it print

//printf("DIR_FstClusHI: %d, DIR_FstClusLO: %d\n",dir.DIR_FstClusHI, dir.DIR_FstClusLO);//**********************************

	 openlist[oi].used = true;
	 offset = temp;
	 printf("%s is in openlist[%d] at cluster %d\n",openlist[oi].name, oi, openlist[oi].cluster);
	}
	else{
	 printf("This file is already open.\n");
	}
	found = true;
      }
      offset+=64;	//increments to next entry
  }
  else{offset+=64;}
    }//end while
  if(!found){printf("%s was not found.\n",name);}
}

/***************************CLOSE*********************************/
void close(){
  scanf("%s",name);
  int filepos = openSearch(name);
  if(filepos == -1){
	printf("This file isn't open.");
  }
  else{
	openlist[filepos].used = false;
  }
}

/***********************RETURN SIZE*******************************/
int findsize(){
  unsigned int offset;
  unsigned int c=currClus;
  bool found=0;

  //Valid entries are '.', '..' or a string

  if(strcmp(name,"..")==0){
    if(depth==0){
    }
    else{
      c=parent[depth-1];
      found=1;
    }
  }
  else if(strcmp(name,".")==0){
      found=1;
  }
  else{
    //seeks to our current cluster
    offset=SectorOffset(FirstSectorCluster(c));
    fseek(file,offset,SEEK_SET);
    unsigned int temp=offset+bpb.BPB_BytsPerSec*bpb.BPB_SecPerClus;

    while(temp>offset){

	//fills our dir structs
      fread(&ldir,sizeof(struct LongDirectoryEntry),1,file);
      fread(&dir,sizeof(struct DirectoryEntry),1,file);

      unsigned char fname[26]={0};
	//concatenates file names from read data
      for(int i=0,k=0;i<10;i+=2){
  	fname[k]=ldir.LDIR_Name1[i];
	if(fname[k]=='\0')
	  break;
	k++;
	if(i==8){
	  for(int j=0;j<12;j+=2){
	    fname[k]=ldir.LDIR_Name2[j];
	    if(fname[k]=='\0')
	      break;
	    k++;
	    if(j==10){
	      for(int l=0;l<4;l+=2){
	        fname[k]=ldir.LDIR_Name3[l];
	        if(fname[k]=='\0')
	          break;
	        k++;
	      }
	    }
	  }
	}
      }
	//trigger on name match
      if(strcmp(fname,name)==0){
        found=1;
	if(dir.DIR_Attr!=0x10){
	  //printf("Size: %d Bytes\n",dir.DIR_FileSize);
	  return dir.DIR_FileSize;
	}
	else{
	  c=dir.DIR_FstClusHI*0x100+dir.DIR_FstClusLO;
	}
	break;
      }
      offset+=64;	//increments to next entry
    }//end while
  }

	//found matching directory
  if(found){
	//same algorithm as above, but printing filenames
    offset=SectorOffset(FirstSectorCluster(c));
    fseek(file,offset,SEEK_SET);
    unsigned int temp=offset+bpb.BPB_BytsPerSec*bpb.BPB_SecPerClus;
    unsigned int entries=0;
    while(temp>offset){
	//fills our dir struct
      fread(&ldir,sizeof(struct LongDirectoryEntry),1,file);
      fread(&dir,sizeof(struct DirectoryEntry),1,file);
      unsigned char fname[26]={0};

	//skips the . and .. entries
    if(ldir.LDIR_Ord=='A'){
      for(int i=0,k=0;i<10;i+=2){
  	fname[k]=ldir.LDIR_Name1[i];
	if(fname[k]=='\0')
	  break;
	k++;
	if(i==8){
	  for(int j=0;j<12;j+=2){
	    fname[k]=ldir.LDIR_Name2[j];
	    if(fname[k]=='\0')
	      break;
	    k++;
	    if(j==10){
	      for(int l=0;l<4;l+=2){
	        fname[k]=ldir.LDIR_Name3[l];
	        if(fname[k]=='\0')
	          break;
	        k++;
	      }
	    }
	  }
	}
      }
      entries++;
      offset+=64;	//increments to next entry
  }
  else{offset+=64;}
    }//end while
    printf("Entries in directory: %d\n",entries);
  }
  else{
    printf("Directory or file not found\n");
  }
}


/***************************READ**********************************/
void read(){
	// TO DO:
	// Multi-cluster files need to be tested, but I think this should be able to handle them.
	// mode checking for the mode in the actual file system. (I've already tested for the mode the user entered when opening the file).


 char inoffset[11];
 char insize[11];
 int offset;
 int originalOffset;
 int size;
 int realSize;
 scanf("%s",name);
 scanf("%s",inoffset);
 scanf("%s",insize);
 offset = atoi(inoffset);
 size = atoi(insize);
 realSize = findsize(); 
 int n = offset / bpb.BPB_BytsPerSec;
 char contents[bpb.BPB_BytsPerSec];
 int index = openSearch(name);
 if(index == -1){
  printf("This file is not open.\n");
  return;
 }
 
 if(strcmp(openlist[index].mode,"r")!=0 && strcmp(openlist[index].mode,"rw")!=0 && strcmp(openlist[index].mode,"wr")!=0)
 {
  printf("You do not have permission to read from this file.\n");
  return;
 }
 // Do this test again, but for the actual mode in the file system for this file.
 
 if(offset < 0){offset = offset * -1;} // don't let offset be negative.
 if(offset + size > realSize){size = realSize - offset;} // Don't let them read past the end of the file.
 if(size <=0){return;}
 originalOffset = offset;
 int next = 1;
 offset += openlist[index].cluster; 

 while(next != 0){
	fseek (file,offset,SEEK_SET);
	fread(&dir,sizeof(struct DirectoryEntry),1,file);
	fread (&contents, sizeof(contents), 1, file);

	for(int i = 0; i < size; i++){
 	 printf("%c",contents[i]);
 	}
 	printf("\n");

 	fseek (file, offset - originalOffset,SEEK_SET);
 	fread(&dir,sizeof(struct DirectoryEntry),1,file);
 	offset =SectorOffset(FirstSectorCluster(dir.DIR_FstClusHI*0x100+dir.DIR_FstClusLO)) - 32;
 	next = dir.DIR_FstClusHI*0x100 + dir.DIR_FstClusLO;
	originalOffset = 0; // no offset for subsequent clusters
	// printf("Next Cluster: FAT[%d]\n",next);
} // end of while

}

/****************************WRITE********************************/
void write(){
	// TO DO:
	// Multi-cluster files need to be tested, but I think this should be able to handle them.
	// mode checking for the mode in the actual file system. (I've already tested for the mode the user entered when opening the file).


 char inoffset[11];
 char insize[11];
 char instring[1000]={0};
 int offset;
 int originalOffset;
 int size;
 int realSize;
 char *string;
 scanf("%s",name);
 scanf("%s",inoffset);
 scanf("%s",insize);
 scanf("%s",instring);
 offset = atoi(inoffset);
 size = atoi(insize);
 string=calloc(sizeof(char),size+1);
 realSize = findsize(); 
 int n = offset / bpb.BPB_BytsPerSec;
 char contents[bpb.BPB_BytsPerSec];
 int index = openSearch(name);
 if(index == -1){
  printf("This file is not open.\n");
  return;
 }
 
 if(strcmp(openlist[index].mode,"w")!=0 && strcmp(openlist[index].mode,"rw")!=0 && strcmp(openlist[index].mode,"wr")!=0)
 {
  printf("You do not have permission to write this file.\n");
  return;
 }
 // Do this test again, but for the actual mode in the file system for this file.
 
 if(offset < 0){offset = offset * -1;} // don't let offset be negative.
 if(offset + size > realSize){size = realSize - offset;} // Don't let them read past the end of the file.
 if(size <=0){return;}
 originalOffset = offset;
 int next = 1;
 offset += openlist[index].cluster; 

 for(int i=0;i<size;i++){
   if(instring[i+1]=='"'){
     string[i]='\0';
     continue;
   }
   string[i]=instring[i+1];
 }

	fseek (file,offset,SEEK_SET);
	fread(&dir,sizeof(struct DirectoryEntry),1,file);
fwrite(string,size,1,file);
	
}
/***************************INFO**********************************/
void info(){


  printf("BS_jmpBoot: 0x%x%x%x\n",bpb.BS_jmpBoot[0],bpb.BS_jmpBoot[1],bpb.BS_jmpBoot[2]);

  printf("BS_OEMName: ");
	for(int i=0;i<8;i++){
	  printf("%c",bpb.BS_OEMName[i]);
	  if(i==7)
	    printf("\n");
	}
  printf("BPB_BytsPerSec: %d\n",bpb.BPB_BytsPerSec);
  printf("BPB_SecPerClus: %d\n",bpb.BPB_SecPerClus);
  printf("BPB_RsvdSecCnt: %d\n",bpb.BPB_RsvdSecCnt);
  printf("BPB_NumFATs: %d\n",bpb.BPB_NumFATs);
  printf("BPB_RootEntCnt: %d\n",bpb.BPB_RootEntCnt);
  printf("BPB_TotSec16: %d\n",bpb.BPB_TotSec16);
  printf("BPP_Media: 0x%x\n",bpb.BPB_Media);
  printf("BPB_FATSz16: %d\n",bpb.BPB_FATSz16);
  printf("BPB_SecPerTrk: %d\n",bpb.BPB_SecPerTrk);
  printf("BPB_NumHeads: %d\n",bpb.BPB_NumHeads);
  printf("BPB_HiddSec: %d\n",bpb.BPB_HiddSec);
  printf("BPB_TotSec32: %d\n",bpb.BPB_TotSec32);
  printf("BPB_FATSz32: %d\n",bpb.BPB_FATSz32);
  printf("BPB_ExtFlags: %d\n",bpb.BPB_ExtFlags);
  printf("BPB_FSVer: %d\n",bpb.BPB_FSVer);
  printf("BPB_RootClus: %d\n",bpb.BPB_RootClus);
  printf("BPB_FSInfo: %d\n",bpb.BPB_FSInfo);
  printf("BPB_BkBootSec: %d\n",bpb.BPB_BkBootSec);
  printf("BPB_Reserved: 0x");
	for(int i=0;i<12;i++){
	  printf("%x",bpb.BPB_Reserved[i]);
	  if(i==11)
	    printf("\n");
	}
  printf("BS_DrvNum: %d\n",bpb.BS_DrvNum);
  printf("BS_Reserved1: %d\n",bpb.BS_Reserved1);
  printf("BS_BootSig: %d\n",bpb.BS_BootSig);
  printf("BS_VolID: 0x%x\n",bpb.BS_VolID);
  printf("BS_VolLab: 0x");
	for(int i=0;i<11;i++){
	  printf("%x",bpb.BS_VolLab[i]);
	  if(i==10)
	    printf("\n");
	}
  printf("BS_FilSysType: ");
	for(int i=0;i<11;i++){
	  printf("%c",bpb.BS_FilSysType[i]);
	  if(i==10)
	    printf("\n");
	}

}
/*******************************LS********************************/
void ls(){
  unsigned int offset;
  unsigned int c=currClus;
  bool found=0;

  scanf("%s",name);
  
  //Valid entries are '.', '..' or a string

  if(strcmp(name,"..")==0){
    if(depth==0){
    }
    else{
      c=parent[depth-1];
      found=1;
    }
  }
  else if(strcmp(name,".")==0){
      found=1;
  }
  else{
    //seeks to our current cluster
    offset=SectorOffset(FirstSectorCluster(c));
    fseek(file,offset,SEEK_SET);
    unsigned int temp=offset+bpb.BPB_BytsPerSec*bpb.BPB_SecPerClus;

    while(temp>offset){

	//fills our dir structs
      fread(&ldir,sizeof(struct LongDirectoryEntry),1,file);
      fread(&dir,sizeof(struct DirectoryEntry),1,file);

      unsigned char fname[26]={0};
	//concatenates file names from read data
      for(int i=0,k=0;i<10;i+=2){
  	fname[k]=ldir.LDIR_Name1[i];
	if(fname[k]=='\0')
	  break;
	k++;
	if(i==8){
	  for(int j=0;j<12;j+=2){
	    fname[k]=ldir.LDIR_Name2[j];
	    if(fname[k]=='\0')
	      break;
	    k++;
	    if(j==10){
	      for(int l=0;l<4;l+=2){
	        fname[k]=ldir.LDIR_Name3[l];
	        if(fname[k]=='\0')
	          break;
	        k++;
	      }
	    }
	  }
	}
      }
	//trigger on name match
	//ls only works with directories
      if(strcmp(fname,name)==0){
        found=1;
	if(dir.DIR_Attr!=0x10){
	  found=0;
	  break;
	}
	else{
	  c=dir.DIR_FstClusHI*0x100+dir.DIR_FstClusLO;
	}
	break;
      }
      offset+=64;	//increments to next entry
    }//end while
  }

	//found matching directory
  if(found){
    printf("./\n");
    if(depth!=0){
      printf("../\n");
    }
	//same algorithm as above, but printing filenames
    offset=SectorOffset(FirstSectorCluster(c));
    fseek(file,offset,SEEK_SET);
    unsigned int temp=offset+bpb.BPB_BytsPerSec*bpb.BPB_SecPerClus;
    while(temp>offset){
	//fills our dir struct
      fread(&ldir,sizeof(struct LongDirectoryEntry),1,file);
      fread(&dir,sizeof(struct DirectoryEntry),1,file);
      unsigned char fname[26]={0};

	//skips the . and .. entries
    if(ldir.LDIR_Ord=='A'){
      for(int i=0,k=0;i<10;i+=2){
  	fname[k]=ldir.LDIR_Name1[i];
	if(fname[k]=='\0')
	  break;
	k++;
	if(i==8){
	  for(int j=0;j<12;j+=2){
	    fname[k]=ldir.LDIR_Name2[j];
	    if(fname[k]=='\0')
	      break;
	    k++;
	    if(j==10){
	      for(int l=0;l<4;l+=2){
	        fname[k]=ldir.LDIR_Name3[l];
	        if(fname[k]=='\0')
	          break;
	        k++;
	      }
	    }
	  }
	}
      }
      printf("%s",fname);
      if(dir.DIR_Attr==0x10){
	printf("/\n");
      }
      else{
	printf("\n");
      }
      //printf("%s\n",fname);
      offset+=64;	//increments to next entry
  }
  else{offset+=64;}
    }//end while
  }
  else{
    printf("Directory not found\n");
  }
}

/*******************************CD*********************************/
void cd(){
  unsigned int offset;
  unsigned int c=currClus;
  bool found=0;

  scanf("%s",name);

  if(strcmp(name,"..")==0){
    if(depth==0){
      return;
    }
    else{
      c=parent[depth-1];
      depth--;
      currClus=c;
      return;
    }
  }
  else if(strcmp(name,".")==0){
      return;
  }
  else{
    //seeks to our current cluster
    offset=SectorOffset(FirstSectorCluster(c));
    fseek(file,offset,SEEK_SET);
    unsigned int temp=offset+bpb.BPB_BytsPerSec*bpb.BPB_SecPerClus;

    while(temp>offset){

	//fills our dir structs
      fread(&ldir,sizeof(struct LongDirectoryEntry),1,file);
      fread(&dir,sizeof(struct DirectoryEntry),1,file);

      unsigned char fname[26]={0};
	//concatenates file names from read data
      for(int i=0,k=0;i<10;i+=2){
  	fname[k]=ldir.LDIR_Name1[i];
	if(fname[k]=='\0')
	  break;
	k++;
	if(i==8){
	  for(int j=0;j<12;j+=2){
	    fname[k]=ldir.LDIR_Name2[j];
	    if(fname[k]=='\0')
	      break;
	    k++;
	    if(j==10){
	      for(int l=0;l<4;l+=2){
	        fname[k]=ldir.LDIR_Name3[l];
	        if(fname[k]=='\0')
	          break;
	        k++;
	      }
	    }
	  }
	}
      }
	//trigger on name match
	//ls only works with directories
      if(strcmp(fname,name)==0){
        found=1;
	if(dir.DIR_Attr!=0x10){
	  found=0;
	  break;
	}
	else{
	  c=dir.DIR_FstClusHI*0x100+dir.DIR_FstClusLO;
	}
	break;
      }
      offset+=64;	//increments to next entry
    }//end while
  }

	//found matching directory
  if(found){
    depth++;
    parent[depth]=c;
    currClus=c;
  }
  else{
    printf("Directory not found\n");
  }
}

/******************************SIZE********************************/
void size(){
  unsigned int offset;
  unsigned int c=currClus;
  bool found=0;

  scanf("%s",name);
  
  //Valid entries are '.', '..' or a string

  if(strcmp(name,"..")==0){
    if(depth==0){
    }
    else{
      c=parent[depth-1];
      found=1;
    }
  }
  else if(strcmp(name,".")==0){
      found=1;
  }
  else{
    //seeks to our current cluster
    offset=SectorOffset(FirstSectorCluster(c));
    fseek(file,offset,SEEK_SET);
    unsigned int temp=offset+bpb.BPB_BytsPerSec*bpb.BPB_SecPerClus;

    while(temp>offset){

	//fills our dir structs
      fread(&ldir,sizeof(struct LongDirectoryEntry),1,file);
      fread(&dir,sizeof(struct DirectoryEntry),1,file);

      unsigned char fname[26]={0};
	//concatenates file names from read data
      for(int i=0,k=0;i<10;i+=2){
  	fname[k]=ldir.LDIR_Name1[i];
	if(fname[k]=='\0')
	  break;
	k++;
	if(i==8){
	  for(int j=0;j<12;j+=2){
	    fname[k]=ldir.LDIR_Name2[j];
	    if(fname[k]=='\0')
	      break;
	    k++;
	    if(j==10){
	      for(int l=0;l<4;l+=2){
	        fname[k]=ldir.LDIR_Name3[l];
	        if(fname[k]=='\0')
	          break;
	        k++;
	      }
	    }
	  }
	}
      }
	//trigger on name match
      if(strcmp(fname,name)==0){
        found=1;
	if(dir.DIR_Attr!=0x10){
	  printf("Size: %d Bytes\n",dir.DIR_FileSize);
	  return;
	}
	else{
	  c=dir.DIR_FstClusHI*0x100+dir.DIR_FstClusLO;
	}
	break;
      }
      offset+=64;	//increments to next entry
    }//end while
  }

	//found matching directory
  if(found){
	//same algorithm as above, but printing filenames
    offset=SectorOffset(FirstSectorCluster(c));
    fseek(file,offset,SEEK_SET);
    unsigned int temp=offset+bpb.BPB_BytsPerSec*bpb.BPB_SecPerClus;
    unsigned int entries=0;
    while(temp>offset){
	//fills our dir struct
      fread(&ldir,sizeof(struct LongDirectoryEntry),1,file);
      fread(&dir,sizeof(struct DirectoryEntry),1,file);
      unsigned char fname[26]={0};

	//skips the . and .. entries
    if(ldir.LDIR_Ord=='A'){
      for(int i=0,k=0;i<10;i+=2){
  	fname[k]=ldir.LDIR_Name1[i];
	if(fname[k]=='\0')
	  break;
	k++;
	if(i==8){
	  for(int j=0;j<12;j+=2){
	    fname[k]=ldir.LDIR_Name2[j];
	    if(fname[k]=='\0')
	      break;
	    k++;
	    if(j==10){
	      for(int l=0;l<4;l+=2){
	        fname[k]=ldir.LDIR_Name3[l];
	        if(fname[k]=='\0')
	          break;
	        k++;
	      }
	    }
	  }
	}
      }
      entries++;
      offset+=64;	//increments to next entry
  }
  else{offset+=64;}
    }//end while
    printf("Entries in directory: %d\n",entries);
  }
  else{
    printf("Directory or file not found\n");
  }
}

/********************************MAIN******************************/
int main(int argc, char*argv[]){

  char cmd[10];

  //confimring arguments passed in
  if (argc==2){
    fatName=argv[1];

    if(file=fopen(fatName,"rb+")){
      fread(&bpb,sizeof(struct FAT32BootBlock),1,file);	//fills our FAT32 struct

      currClus=bpb.BPB_RootClus;
      parent[depth]=currClus;

	initOpen(); // initiallize each member of openlist so used is false
      while(1){
	printf("\n%s> ",fatName);
	scanf("%s",cmd);

	if(strcmp(cmd,"exit") ==0){
	  fclose(file);
	  return 0;
	}
	else if(strcmp(cmd,"info")==0){
	  info();
	  while((getchar())!='\n');
	}
	else if(strcmp(cmd,"cd")==0){
	  cd();
	  while((getchar())!='\n');
	}
	else if(strcmp(cmd,"ls")==0){
	  ls();
	  while((getchar())!='\n');
	}
	else if(strcmp(cmd,"size")==0){
	  size();
	  while((getchar())!='\n');
	}
	else if(strcmp(cmd,"open")==0){
	  open();
	  while((getchar())!='\n');
	}
	else if(strcmp(cmd,"close")==0){
	  close();
	  while((getchar())!='\n');
	}
	else if(strcmp(cmd,"read")==0){
	  read();
	  while((getchar())!='\n');
	}
	else if(strcmp(cmd,"write")==0){
	  write();
	  while((getchar())!='\n');
	}
	else{
	  printf("Command not found.\n");
	  printf("List of commands:\nexit\ninfo\nls <dir>\ncd <dir>\nsize <dir> or size <file>\ncreat\nmkdir <dir>\nrm\nrmdir <dir>\nopen <file> <mode>\nclose <file>\nread <file> <offset> <size>\nwrite <file> <offset> <size> <string>\n");
	}
      }
    }

    //end program if file not found
    else{
      printf("Could not find FAT32 image.\n");
      return 0;
    }
	
  }

  //end program if no argument passed
  else{
    printf("Incorrect number of arguments.\nUsage: <excutable> <fat.img>\n");
    return 0;
  }

}
