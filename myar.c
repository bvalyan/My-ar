#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <utime.h>
//#include <ec.h>
#include <unistd.h>
#include <utime.h>
#include <dirent.h>

/*
THIS CODE IS MY OWN WORK. IT WAS WRITTEN WITHOUT CONSULTING CODE WRITTEN BY OTHER STUDENTS OR MATERIALS OTHER THAN THIS SEMESTER'S COURSE MATERIALS. Brandon Valyan-Clark
*/

#define  ARMAG   "!<arch>\n"	 /* magic string */
#define  SARMAG   8		 /* length of magic string */
#define  ARFMAG   "`\n"	

// q - read and a write. copying a file to an archive

void quickPend(char* archiveName, char* fileName);
void extractor(char* archiveName, char* fileName);
void findFile(char* filename, int fd);
void tablePrinter( char* archiveName, char* fileName);
void appender(char*);




typedef struct header1{
	char filename[16];
	char timestamp[12];
	char uid[6];
	char gid[6];
	char mode[8];
	char filesize[10];
	char filemagic[2];
} header;

//printf("%llu", head->timestamp);


/*struct headerBuilder{
		char* filename;
		char* fileModTime;
		char* ownerID;
		char* groupID;
		char* fileMode;
		char* filesize;
		char* filemag;
	} headerBuilder;*/

int checkLegality(char* inputString, int length){
	int i = 0;
	printf(inputString);
	while (i < length){
		if(strpbrk(&inputString[i], "qxtcdA") == NULL)
		{ //if not one of our options, exit the program with an error
			//printf("ERROR: %s\n", strerror(errno));
		 	printf("invalid letter");
			//exit program
			exit(0);
		}
		i +=1;		
	}

}

/*size_t getFilesize(const char* filename) {
    struct stat st;
    if(stat(filename, &st) != 0) {
    	printf("ERROR: %s\n", strerror(errno));
        return 0;
    }
    return st.st_size;   
}*/

void truncFlags(char* inputString, char* archiveName, char* fileName){
	int stringCount = 0;
	while (inputString[stringCount] !='\0'){ // /0 is the end string???
		switch(inputString[stringCount])
			{
			 // once the flags have been check for legality. Determine required process(es)
		     case 'q': quickPend(archiveName, fileName);
			 stringCount +=1;
			 break;
			 case 'x': extractor(archiveName ,fileName);
			 stringCount +=1;
			 break;
			 case 't': tablePrinter(archiveName, fileName);
			 stringCount +=1;
			 break;
			 case 'A': appender(archiveName);
			 stringCount +=1;
			}
		}
	}

/*struct tm* getTime(struct stat* getTime){
	struct tm* time;//for getting time later
   
    time = localtime(&getTime->st_mtime);
    return time;
}*/

void quickPend(char* archiveName, char* fileName){
	int fd, fdfrom;
	//int nread, nwrite, n;
	int headerInfo ;
	char buffer[1048577];//Adjusted number to fit blocksize
	char intro[8] = ARMAG;
	struct stat buf;
	struct stat buf2;
	int headerInfo2;
	if(access(archiveName, F_OK) != -1){
		 fd = open(archiveName, O_RDWR | O_CREAT | O_APPEND, 0666); //opens the file for appending with read/write permissions. Does not truncate. Creates file if not there
    		}
	
	else{
    fd = open(archiveName, O_RDWR | O_CREAT | O_APPEND, 0666); //opens the file for appending with read/write permissions. Does not truncate. Creates file if not there
    if(write(fd, &intro, sizeof(intro)) == -1){// Write ARMAG
    			printf("ERROR 4: %s\n", strerror(errno));
    			exit(0);
    	}
    }
    fdfrom = open(fileName, O_RDONLY , 0666);
    headerInfo = fstat(fdfrom, &buf); //run fstat to get all the header information for opened file
    //stat(fileName, &buf2); //run fstat to get all the header information for opened file
    int blocksize = buf.st_blksize;
    int filesize = buf.st_size;
    int fileTime = buf.st_mtime;
    struct utimbuf* timeFind;
    timeFind = malloc(sizeof(struct utimbuf*));
    timeFind->actime = fileTime;
    timeFind->modtime = fileTime;
    utime(fileName,timeFind);
    fstat(fdfrom, &buf);
    headerInfo = fstat(fdfrom, &buf);
    char headerBuffer[60];

    

    sprintf(headerBuffer, "%-16s%-12li%-6u%-6u%-8hu%-10lld%-2s", fileName, buf.st_mtime, buf.st_uid, buf.st_gid, buf.st_mode, buf.st_size, ARFMAG ); //format name for buffer
	
    int counting = strlen(headerBuffer);
    if(write(fd, headerBuffer, strlen(headerBuffer)) == -1){
	printf("%d", fd);
    	printf("ERROR 2: %s\n", strerror(errno));
	exit(0);	
    } //write 60 bytes of header to header! If write returns an error, report this and exit the program.
    else{printf("%d", fd );}    

    int size = buf.st_size;
    int x;
    
    size = buf.st_size;
    while ((x=read(fdfrom, buffer,blocksize))>0){
    	if(write(fd, buffer,x) == -1){
		printf("ERROR 4: %s\n", strerror(errno));
	};
}
 
    if (size%2>0) //if wrote odd byte, add newline
    	write(fd,"\n",1);


}
/*
void removeSpaces(char* stringUsed){
	char* a = stringUsed;
	char* b = stringUsed;
	while(*b != 0){
		*a = *b++;
		if(*a != '\\')
			a++;
	}
	*a = 0;
}*/


header* getHeader(off_t skip, int fd){

	lseek(fd, skip, SEEK_CUR);
	
	header* head = malloc(sizeof(header)); //allocate memory for a header
	char buf4[60];
	read(fd,buf4,sizeof(buf4));
	sscanf(buf4, "%16s%12s%6s%6s%8s%10s%2s", head->filename, head->timestamp, head->uid, head->gid, head->mode, head->filesize, head->filemagic); //fill the header with info
	return head;

}


void grabFile(header *target, int fd){
	struct stat buf3;
	int newStat;
	int blockSize2;
	int newFD;
	int filesize2 = atoi(target->filesize); 
	char* realFile = target->filename;
	newStat = stat(realFile, &buf3); //do a normal stat this time since we have a filename
	blockSize2 = 1048577; //already found as optimal block size
	char buffer[blockSize2];
	if((newFD=open(realFile, O_CREAT | O_RDWR, 0666)) == -1){ //open our new file to write to
		printf("ERROR : %s\n", strerror(errno));
	}
	unsigned long targetSize = atoi(target->filesize);
	int takeBuf  = 0;
	while ((takeBuf=read(fd, buffer,blockSize2))>0 && takeBuf > blockSize2){ // read in from the archive. The optimal amount as defined by the target file's header
		if(takeBuf = write(newFD, buffer,takeBuf) == -1){
			printf("ERROR : %s\n", strerror(errno));
		}//write this amount to new file 
	}

	if (takeBuf%2>0) //if wrote odd byte, add newline
    	write(newFD,"\n",1);

//////////////have to adjust time
	struct utimbuf* timeAdjust;
	timeAdjust = malloc(sizeof(struct utimbuf*));
	timeAdjust->actime = atoi(target->timestamp);
	timeAdjust->modtime = atoi(target->timestamp);

	utime(realFile, timeAdjust);//Actual setting of time
	int uid = atoi(target->uid); //get uid to change ownership
	int gid = atoi(target->gid); //get gid to change ownership
	chown(target->filename, uid, gid); // change ownership to target's UID and GID
	int mode = atoi(target->mode);
	chmod(target->filename, mode); //change permissions to target's permisions
}	

void findFile(char* filename, int fd){
	struct stat buf2;
	int headInfo;
	headInfo = fstat(fd, &buf2);
	int size2 = buf2.st_size;
	int skip = 0;
	int skip2;
	lseek(fd, SARMAG, SEEK_SET); //Skip over the ARMAG part of the file. Begins header
	int dataLeft = (lseek(fd, 0, SEEK_CUR));
	while(dataLeft < size2){//while we are not at the end of the file keep searching 
			header* ptr = getHeader(skip, fd);
			char* newfilename = ptr->filename; //get file name for comparison to requested filename
			if(!strcmp(newfilename, filename)){ //file found
				grabFile(ptr, fd);//file found so grab it!
				return;
			}
			else{skip2 = atoi(ptr->filesize);// file not found :( keep looking!
				if(skip2 % 2 != 0){ //check if we are adding odd bytes
					skip2 += 1;
				}
				skip = skip2;
				dataLeft = (lseek(fd, 0, SEEK_CUR) + skip);
			}
	}
}



void extractor(char* archiveName, char* fileName){
	int fd;
	if(access(archiveName, F_OK) != -1){//if archive exists open it
		fd = open(archiveName, O_RDWR | O_CREAT | O_APPEND, 0666); //opens the file for appending with read/write permissions. Does not truncate. Creates file if not there
		 
    		}
    else{printf("ERROR: Archive does not exist!");//Can't extract from a nonexistant archive! Exit!
    	 exit(0);
			}
	findFile(fileName, fd);//Assuming we have an archive, we can find our files!

}

 

void tablePrinter(char* archiveName, char* fileName){
	int skip = 0;
	struct stat buf5;
	int fd = open(archiveName, O_RDWR | O_CREAT, 0666);//open the archive in quetion 
	fstat(fd, &buf5);
	lseek(fd, SARMAG, SEEK_SET); //Skip over the ARMAG part of the file. Begins header
	int size2 = buf5.st_size; //get the total size of the directory
	while(lseek(fd, 0, SEEK_CUR) + skip < size2){ //check if our current position in the file is less than the archive's size
		header* ptr = getHeader(skip, fd);
		char* name2;
		name2 = ptr->filename;//get the filename from the header we are currently on
		printf("%s \n", name2);
		int skip2 = atoi(ptr->filesize);
		if(skip2 % 2 != 0){ //check if we are skipping to odd bytes
				skip2 += 1;
				}
				skip = skip2;
	}
}

void appender(char* archiveName){
	DIR* current;
	char * dir_name = ".";

	//cwd = getcwd( cwd, sizeof(cwd)); //get the current working directory name
	current = opendir(dir_name); //open the directory associated with the current working directory
	
	while(readdir(current) != NULL){// While next directory entry is not null
		struct dirent* file;
		file = readdir(current);
		if (file->d_type == DT_REG && (strcmp(file->d_name, "." ) != 0) && (strcmp(file->d_name, ".." ) != 0)){ //if the file is a regular file and not the directory itself append it
			char* fileName2 = file->d_name;
			quickPend(archiveName, file->d_name);
		}
	}
	closedir(current);


}

int main(int argc, char * argv[])
{	
   checkLegality(argv[1], strlen(argv[1]));
   truncFlags(argv[1], argv[2], argv[3]);

}
