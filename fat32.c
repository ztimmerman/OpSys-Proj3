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

struct FAT32BootBlock{
	unsigned short sector_size;
	unsigned short reserved_sectors;
	unsigned short root_dir_entries;
	unsigned short total_sectors_short;	//if zero, later field is used
	unsigned short fat_size_sectors;
	unsigned short secotrs_per_track;
	unsigned short numner_of_heads;
	unsigned short bdp_extflags;
	unsigned short bdp_fsver;
	unsigned short boot_sector_signature;

	unsigned int hidden_sectors;
	unsigned int total_sectors_long;
	unsigned int bdp_FATz32;
	unsigned int bdp_rootcluster;

	unsigned char jmp[3];
	unsigned char sectors_per_cluster;
	unsigned char number_of_fats;
	unsigned char media_descriptors;

	char oem[8];
	char volume_label[11];
	char fs_type[8];
	char boot_code[436];
}__attribute((packed));


/*******************************GLOBAL*****************************/
FILE *file;
char *fatName;
struct FAT32BootBlock bdp;


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
