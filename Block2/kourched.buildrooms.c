/*********************************************************
 * Author: Diego Kourchenko
 * Email: kourched@oregonstate.edu
 * Description:	Generates a series of files.
 *		Each file corresponds to a type of 'room'.
 ********************************************************/
#include <stdio.h>
#include <sys/types.h> 	/* getpid() - Process ID */
#include <sys/stat.h> 	/* stat() - check if directory exists */
#include <unistd.h> 	/* getpid() - Process ID */
#include <stdlib.h> 	/* iota() - integer to string */
#include <string.h> 	/*  memset() */
#include <time.h>	/* Used for rand() - generate random int */
#include <dirent.h>	/* readdir() */

/* Define Boolean values, C89 doesn't have boolean type */
#define true 1
#define false 0

/* Room Name Restrictions */
#define START_ROOM 	"START_ROOM"
#define MID_ROOM 	"MID_ROOM"
#define END_ROOM	"END_ROOM"

/* Room Size Restrictions */
#define MAX_NAME_SIZE 	21
#define MAX_TYPE_SIZE	21
#define MAX_CONNECTIONS 7

/* Room Names - 10 Total, not all used */
#define ROOM_BOB 	    "BOB"
#define ROOM_MARTHA	    "MARTHA"
#define ROOM_KITCHEN	"KITCHEN"
#define ROOM_DUNGEON	"DUNGEON"
#define ROOM_CLOSET	    "CLOSET"
#define ROOM_EMPTY	    "EMPTY"
#define	ROOM_GUEST	    "GUEST"
#define ROOM_GARAGE	    "GARAGE"
#define	ROOM_DEN	    "DEN"
#define	ROOM_ATTIC	    "ATTIC"

/**
 * Room struct used for unified file structure.
 */
typedef struct {
	char *name;
	char *type;
	char *connections[MAX_CONNECTIONS];
	int nConnections;
} Room;
typedef int bool;

/**
 * Function Declarations before usage.
 */
void CreateDirAndEnter();
void PopulateArrays();
int IsGraphFull();
void AddRandomConnection();
void CreateAllConnections();
Room GetRandomRoom(); /* Function returns struct Room */
int CanAddConnectionFrom(Room x);
int ConnectionAlreadyExists(Room x, Room y);
void ConnectRoom(Room x, Room y);
int IsSameRoom(Room x, Room y);
int IsFile(const char *fileName);

/* Store current files and rooms in directory in an array, used for creating connections */
char *filesInDir[7];
int nFiles = 0;
/* Used for adding connections, add to connections */
Room roomsInDir[7];

/**
 * Create a directory for the rooms and goes into that directory.
 */
void CreateDirAndEnter() {
	/* 21 bytes = length of "kourched.rooms." + processID */
	char dirName[MAX_NAME_SIZE];
	int processID = getpid();
	
	/* Clear 21 bytes for use */
	memset(dirName, '\0', MAX_NAME_SIZE);
	
	/* Copy integer processID into string baseName */
	sprintf(dirName, "kourched.rooms.%d", processID);
	
	/* Buffer for information from stat() */
	struct stat st = {0};
	
	/* Create directory if it doesn't exist */
	if (stat(dirName, &st) == -1) {
	    /* Create directory with READ/WRITE for User and Group */
	    mkdir(dirName, S_IRWXU | S_IRWXG);
	    chdir(dirName);
	}
}

/**
 * Generate file name.
 */
void CreateFiles() {
    FILE *newFile;
    
	/* Close file after writing to it */
	fclose(newFile);	
}

/**
 * Populates an array of file names and array of Rooms.
 */
void PopulateArrays() {
    char *allFiles[10] = {
        ROOM_BOB,
	    ROOM_MARTHA,
	    ROOM_KITCHEN,
	    ROOM_DUNGEON,
    	ROOM_CLOSET,
    	ROOM_EMPTY,
    	ROOM_GUEST,
    	ROOM_GARAGE,
    	ROOM_DEN,
    	ROOM_ATTIC	
    };
    
    /* Clear memory for files */
    int i = 0, j = 0;
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        filesInDir[i] = malloc(sizeof(char) * MAX_NAME_SIZE);
        memset(filesInDir[i], '\0', MAX_NAME_SIZE);
    }
    
    /* Loop until file created */
    int fileNotCreated = true;
    
    /* Get random int from 0 to 9 */
    int randomInt;

    for (i = 0; i < MAX_CONNECTIONS; i++) {
        /* Generate file name while not in list */
         while (fileNotCreated) {
            /* Random name from array */
            randomInt = rand() % 10;
            fileNotCreated = false;
            for (j = 0; j < MAX_CONNECTIONS; j++) {
                /* File exist in file name array */
                if (filesInDir[j] == allFiles[randomInt]) {
                     fileNotCreated = true;
                }
            }
        }
        roomsInDir[i].type = malloc(sizeof(char) * MAX_NAME_SIZE);
        memset(roomsInDir[i].type, '\0', MAX_NAME_SIZE);
        if (nFiles == 0) {
            strcpy(roomsInDir[i].type, START_ROOM);
        } else if (nFiles == 6) {
            strcpy(roomsInDir[i].type, END_ROOM);
        } else {
            strcpy(roomsInDir[i].type, MID_ROOM);
        }
        
        fileNotCreated = true;
 
        roomsInDir[nFiles].name = malloc(sizeof(char) * MAX_NAME_SIZE);
        memset(roomsInDir[nFiles].name, '\0', MAX_NAME_SIZE);
        strcpy(roomsInDir[nFiles].name,allFiles[randomInt]);
        filesInDir[nFiles] =  allFiles[randomInt];
        nFiles++;
    }
}


/**
 * Returns true if all rooms have 3 to 6 outbound connections, false otherwise
 */
int IsGraphFull() {
	int i = 0;
	Room x;
	for (i = 0; i < 7; i++) {
	    x = roomsInDir[i];
	    printf("Room %s has #%d\n", x.name, x.nConnections);
	    /* Room is not between 3 and 6 */
	    if (x.nConnections <= 3 && x.nConnections <= 6) {
		return false;
	    }
	}
	return true;
}

/*
 * Adds a random, valid outbound connection from a Room to another Room.
 */
void AddRandomConnection() {
	Room A;
	Room B;
	while (true) {
		A = GetRandomRoom();
		if (CanAddConnectionFrom(A) == true)
			break;
	}

	do {
		B = GetRandomRoom();
	}
	while((CanAddConnectionFrom(B) == 0) || (IsSameRoom(A, B) == 1) || (ConnectionAlreadyExists(A, B) == 1));
	printf("Connecting %s to %s\n\n", A.name, B.name);
	ConnectRoom(A, B);
}

/**
 * Create all connections in graph.
 */
void CreateAllConnections() {
    printf("CreateAllConnections...\n");
	while (IsGraphFull() == false) {
	    AddRandomConnection();
	}
}

/**
 * Returns a random Room, does NOT validate if connection can be added.
 */
Room GetRandomRoom() {
	/* Get random number from 0 to 6 */
	int randomInt = rand() % 7;
	/* Get room from array */
	Room randomRoom = roomsInDir[randomInt];
	return randomRoom;
}

/**
 * Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise.
 */
int CanAddConnectionFrom(Room x) {
	if (x.nConnections < 6) {
	    return true;
	}
	return false;
}

/**
 * Returns true if a connection from Roox x to Room y already exists, false otherwise.
 * Assumes two way connection between Room x and Room y. 
 * Only have to check if one room is in the array of connections of another room.
 */
int ConnectionAlreadyExists(Room x, Room y) {
	int i = 0;
	for(i; i < MAX_CONNECTIONS; i++) {
	    if (strcmp(x.name, y.name) == 0) { 
		/* Connection exists in the array of connections of Room x that is Room y */
	            return true;
	    }	
	}
	return false;
}

/**
 * Connects Rooms x and y together, does not check if this connection is valid.
 */
void ConnectRoom(Room x, Room y) {
	int i = 0;
	/* Check connection limits */
	if (CanAddConnectionFrom(x) & CanAddConnectionFrom(y)) {
	   for (i = 0; i < MAX_CONNECTIONS; i++) {
		    if (strcmp(roomsInDir[i].name, x.name) == 0) {
		        roomsInDir[i].connections[roomsInDir[i].nConnections] = x.name;
		        roomsInDir[i].nConnections++;
		    }
	    }
	    for (i = 0; i < MAX_CONNECTIONS; i++) {
		    if (strcmp(roomsInDir[i].name, y.name) == 0) {
		        roomsInDir[i].connections[roomsInDir[i].nConnections] =  y.name;
		        roomsInDir[i].nConnections++;
		    }
	    }
	}
}	

/**
 * Returns true if Rooms x and y are the same Room, false otherwise.
 */
int IsSameRoom(Room x, Room y) {
	if (strcmp(x.name, y.name) == 0) {
	    return true;
	}
	return false;
}
	
/**
 * Returns true if fileName is a file.
 */
int IsFile(const char *fileName) {
	struct stat fileStat;
	stat(fileName, &fileStat);
	return S_ISREG(fileStat.st_mode);
}

int main() {
	/* Initialize time to NULL */ 
	srand(time(NULL));
	/* Create Directory and go into it */
	CreateDirAndEnter();

	/* Populate File and Room arrays */
	PopulateArrays();
    int i = 0;
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        printf("FILE: %s\n", roomsInDir[i].name);
        printf("TYPE: %s\n", roomsInDir[i].type);
    }
    printf("returned sucessfully");	
	/* Create room connections in files */
	CreateAllConnections();
    return 0;
}	
