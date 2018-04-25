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
	unsigned char BS_jmBootp[3];
	unsigned char BS_OEMName[8];
	unsigned short BPB_BytesPerSec;
	unsigned char BPB_SecPerClus;
	unsigned short BPB_RsvdSecCnt;
	unsigned char BPB_NumFATs;
	unsigned short BPB_RootEntCnt;
	unsigned short BPB_TotSec16;
	unsigned char BPB_Media;
	unsigned short BPB_FatSz16;
	unsigned short BPB_SecPerTrk;
	unsigned short BPB_NumHeads;
	unsigned int BPB_HiddSec;
	unsigned int BPB_TotSec32;
	unsigned int BPB_FATSz32;
	unsigned short BPB_FSVer;
	unsigned int BPB_RootClus;
	unsigned short BPB_FSInfo;
	unsigned short BPB_BkBootSec;
	unsigned char BPB_Reserved[12];
	unsigned char BS_Drvnum;
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
