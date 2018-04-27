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

/*******************************GLOBAL*****************************/
FILE *file;
char *fatName;
struct FAT32BootBlock bpb;
struct DirectoryEntry dir;
struct LongDirectoryEntry ldir;
unsigned int currClus;
unsigned int fstDataSec;
unsigned int fstSecClus;
unsigned char name[11];
unsigned int parent;


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
    if(parent==c){
    }
    else{
      c=parent;
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
      printf("%s\n",fname);
      offset+=64;	//increments to next entry
  }
  else{offset+=64;}
    }//end while
  }
  else{
    printf("Directory not found\n");
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
      parent=currClus;

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

	  while((getchar())!='\n');
	}
	else if(strcmp(cmd,"ls")==0){
	  ls();
	  while((getchar())!='\n');
	}

	else{
	  printf("Command not found.\n");
	  printf("List of commands:\nexit\ninfo\nls\ncd\nsize\ncreat\nmkdir\nrm\nrmdir\nopen\nclose\nread\nwrite\n");
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
