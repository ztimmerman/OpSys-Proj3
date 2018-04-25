//********************************************************************/
//Dillon Prendergast, John Thomas Lascha, Zachary Timmerman
//COP4610
//Operating Systems
//Assignment 3- FAT32 File System
//********************************************************************/

/*****************************LIBRARIES******************************/
#include <stdio.h>	//printf

/*******************************STRUCTS*******************************/

typedef struct{
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
}__attribute((packed)) FAT32BootBlock;


/********************************MAIN******************************/
int main(int argc, char*argv[]){

	if (argc==2){

	}
	else{
		printf("Incorrect number of arguments.\nUsage: <excutable> <fat.img>\n");
	return 0;
	}

}
