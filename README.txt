**************************************************
**************************************************
		OPSYS_PROJ3
**************************************************
**************************************************


**************************************************
		    MEMBERS
**************************************************

Member 1: Dillon Prendergast
Member 2: Zachary Timmerman
Member 3: John Thomas Lascha

**************************************************
		DIVISION OF LABOR
**************************************************

Dillon Prendergast
  -project manager
  -README
  -part1- exit
  -part2- info
  -part3- ls
  -part4- cd
  -part5- size
  -part7- mkdir
  -part9- rmdir
  -part13- write

Zachary Timmerman

John Thomas Lascha
  -README
  -part10- open
  -part11- close
  -part12- read
**************************************************
		TAR ARCHIVE CONTENTS
**************************************************

project3_lascha_prendergast_timmerman.tar contents:

README.txt
Makefile
fat32.c

**************************************************
		   COMPILATION
**************************************************

Completed Using:
Linux 4.10.0-38-generic x86_64
gcc version 5.4.0

**************************************************
		   THE MAKEFILE
**************************************************

To make executable:
 Use 'make' or 'make proj3'

To run:
 Use './proj3 <fat32.img>'

To remove executable:
  Use 'make clean'

**************************************************
		      BUGS
**************************************************

-Mkdir truncates names of directories longer than 5 char
-write cuts ending 0's when size>strlen

**************************************************
		UNFINISHED PORTIONS
**************************************************

-read must be tested on multi cluster files

**************************************************
		     COMMENTS
**************************************************
-Discrepancies between description.pdf and recitation
 slides on size command over size DIRNAME vs size FILE,
 so both are included
-When in root directory, most uses of '..' function
 same as '.' Based on the function of 'cd ..' in a Linux root directory
