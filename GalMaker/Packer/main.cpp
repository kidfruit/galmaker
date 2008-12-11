/*
	NOTE: You must specify your 'resource' directory via:
		Project->Settings->Debug->Program Arguments
*/

#include <windows.h>
#include <stdio.h>							// printf
#include <sys/stat.h>
#include <direct.h>							// _getcwd chdir - MSDN
#include "dirent.h"							// http://www.synesis.com.au/software/
#include <fcntl.h>							// Used for O_WRONLY | O_EXCL | O_CREAT, _S_IREAD
#include <io.h>								// Open Write Close Read Lseek

#define PATH_MAX        1024				// http://www.zmailer.org/mhalist/1996/msg00000.html

#ifndef S_ISDIR								// http://cpp.snippets.org/snip_lister.php?fname=stat.h
#define S_ISDIR(x) ((x) & _S_IFDIR )
#define S_ISREG(x) ((x) & _S_IFREG )
#endif

// S_IRUSR ==> _S_IREAD						// Compared value of S_IRUSR (0000400) to all in Stat.h 

//Function prototypes
int getfilesize(char *filename);
int countfiles(char *path);
void packfile(char *filename, int fd);
void findfiles(char *path, int fd);

int currentfile = 1;	//This integer indicates what file we're currently adding to the resource.  The 1st?  The 5th?
int currentloc = 0;	//This integer references the current write-location within the resource file

int main(int argc, char *argv[]) {	
	char pathname[PATH_MAX + 1];	//This character array will hold the application's working directory path
	int filecount;			//How many files are we adding to the resource?
	int fd;					//The file descriptor for the new resource

	//Store the current path
	_getcwd(pathname, sizeof(pathname));

	//How many files are there?
	filecount = countfiles(argv[1]);
	printf("NUMBER OF FILES: %i\n", filecount);

	//Go back to the original path
	chdir(pathname);

	//How many arguments did the user pass?
	if (argc < 3)
	{
		//The user didn't specify a resource file name, go with the default
		fd = open("resource.dat", O_WRONLY | O_EXCL | O_CREAT, _S_IREAD);
	}
	else
	{
		//Use the filename specified by the user
		fd = open(argv[2], O_WRONLY | O_EXCL | O_CREAT, _S_IREAD);
	}
	//Did we get a valid file descriptor?
	if (fd < 0) 
	{
		//Can't create the file for some reason (possibly because the file already exists)
		perror("Cannot create resource file");
		exit(1);
	}

	//Write the total number of files as the first integer
	write(fd, &filecount, sizeof(int));

	//Set the current conditions
	currentfile = 1;					//Start of by storing the first file, obviously!
	currentloc = (sizeof(int) * filecount) + sizeof(int);	//Leave space at the begining of the resource file for the header info

	//Use the findfiles routine to pack in all the files
	findfiles(argv[1], fd);

	//Close the file
	close(fd);

	return 0;
}

// *****************************************************************
//
// This routine recurses through all subdirectories of a given path,
// and counts all of the files it encounters.
//
// *****************************************************************
int countfiles(char *path) {	
	int count = 0;			//This integer will count up all the files we encounter
	struct dirent *entry;		//This structure will hold file information
	struct stat file_status;	//This structure will be used to query file status
	DIR *dir = opendir(path);	//This pointer references the directory stream

	//Make sure we have a directory stream pointer
	if (!dir) {
		perror("opendir failure");
		exit(1);
	}
	
	//Change directory to the given path
	chdir(path);
	
	//Loop through all files and directories
	while ( (entry = readdir(dir)) != NULL) {
		//Don't bother with the .. and . directories
		if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
			//Get the status info for the current file
			if (stat(entry->d_name, &file_status) == 0) {
				//Is this a directory, or a file?
				if (S_ISDIR(file_status.st_mode)) {
					//Call countfiles again (recursion) and add the result to the count total
					count += countfiles(entry->d_name);
					chdir("..");
				}
				else {
					//We've found a file, increment the count
					count++;
				}
			}
		}
	}
	
	//Make sure we close the directory stream
	if (closedir(dir) == -1) {
		perror("closedir failure");
		exit(1);
	}
	
	//Return the file count
	return count;	
}

// *****************************************************************
//
// This routine recurses through all subdirectories of a given path,
// and calls the packfile function on any files it encounters.
//
// *****************************************************************
void findfiles(char *path, int fd) {
	
	struct dirent *entry;		//This structure will hold file information
	struct stat file_status;	//This structure will be used to query file status
	DIR *dir = opendir(path);	//This pointer references the directory stream
		
	//Make sure we have a directory stream pointer
	if (!dir) {
		perror("opendir failure");
		exit(1);
	}
	
	//Change directory to the given path
	chdir(path);
	
	//Loop through all files and directories
	while ( (entry = readdir(dir)) != NULL) {
		//Don't bother with the .. and . directories
		if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
			//Get the status info for the current file
			if (stat(entry->d_name, &file_status) == 0) {
				//Is this a directory, or a file?
				if (S_ISDIR(file_status.st_mode)) {
					//Call findfiles again (recursion), passing the new directory's path
					findfiles(entry->d_name, fd);
					chdir("..");
				}
				else {
					//We've found a file, pack it into the resource file
					packfile(entry->d_name, fd);
				}
			}
		}
	}
	
	//Make sure we close the directory stream
	if (closedir(dir) == -1) {
		perror("closedir failure");
		exit(1);
	}

	return;	
}

// *****************************************************************
//
// This routine accepts a file path and a file descriptor as 
// parameters.  The file specified by the path is opened, and its
// contents are written to the file specified by the descriptor.
//
// *****************************************************************
void packfile(char *filename, int fd) {
	
	int totalsize = 0;	//This integer will be used to track the total number of bytes written to file
	
	//Handy little output
	printf("PACKING: '%s' SIZE: %i\n", filename, getfilesize(filename));
	
	//In the 'header' area of the resource, write the location of the file about to be added
	lseek(fd, currentfile * sizeof(int), SEEK_SET);
	write(fd, &currentloc, sizeof(int));
	
	//Seek to the location where we'll be storing this new file info
	lseek(fd, currentloc, SEEK_SET);
	
	//Write the size of the file
	int filesize = getfilesize(filename);
	write(fd, &filesize, sizeof(filesize));
	totalsize += sizeof(int);
	
	//Write the LENGTH of the NAME of the file
	int filenamelen = strlen(filename);
	write(fd, &filenamelen, sizeof(int));
	totalsize += sizeof(int);
	
	//Write the name of the file
	write(fd, filename, strlen(filename));
	totalsize += strlen(filename);
	
	//Write the file contents
	int fd_read = open(filename, O_RDONLY);		//Open the file
	char *buffer = (char *) malloc(filesize);	//Create a buffer for its contents
	read(fd_read, buffer, filesize);		//Read the contents into the buffer
	write(fd, buffer, filesize);			//Write the buffer to the resource file
	close(fd_read);					//Close the file
	free(buffer);					//Free the buffer
	totalsize += filesize;				//Add the file size to the total number of bytes written
	
	//Increment the currentloc and current file values
	currentfile++;
	currentloc += totalsize;
}

// *****************************************************************
//
// This routine determines the size of a file specified by a given 
// path.
//
// *****************************************************************
int getfilesize(char *filename) {
	
	struct stat file;	//This structure will be used to query file status
	
	//Extract the file status info
	if(!stat(filename, &file))
	{
		//Return the file size
		return file.st_size;
	}
	   
	//ERROR! Couldn't get the filesize.
	printf("getfilesize:  Couldn't get filesize of '%s'.", filename);
	exit(1);
}