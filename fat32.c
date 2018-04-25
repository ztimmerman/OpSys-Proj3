//********************************************************************/
//Dillon Prendergast, John Thomas Lascha, Zachary Timmerman
//COP4610
//Operating Systems
//Assignment 3- FAT32 File System
//********************************************************************/

/*****************************LIBRARIES******************************/
#include <stdio.h>	//printf
#include <string.h>	//strcmp

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


/*******************************GLOBAL*****************************/
FILE *file;
char *fatName;
struct FAT32BootBlock bpb;


/****************************FUNCTIONS*****************************/
/***************************INFO**********************************/
void info(){

  fread(&bpb,sizeof(struct FAT32BootBlock),1,file);

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
/********************************MAIN******************************/
int main(int argc, char*argv[]){

  char cmd[10];

  //confimring arguments passed in
  if (argc==2){
    fatName=argv[1];

    if(file=fopen(fatName,"rb+")){
      while(1){
	scanf("%s",cmd);

	if(strcmp(cmd,"exit") ==0){
	  fclose(file);
	  return 0;
	}
	else if(strcmp(cmd,"info")==0){
	  info();
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
