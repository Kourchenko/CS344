/*********************************************************
 * Author: Diego Kourchenko
 * Email: kourched@oregonstate.edu
 * Description:	Generates a series of files.
 *		Each file corresponds to a type of 'room'.
 ********************************************************/
#include <stdio.h>
#include <stdbool.h> 	/* Boolean */
#include <sys/types.h> 	/* getpid() - Process ID */
#include <sys/stat.h> 	/* stat() - check if directory exists */
#include <unistd.h> 	/* getpid() - Process ID */
#include <stdlib.h> 	/* iota() - integer to string */
#include <string.h> 	/*  memset() */
#include <time.h>	/* Used for rand() - generate random int */
#include <dirent.h>	/* readdir() */

/* Room Name Restrictions */
#define START_ROOM 	"START_ROOM"
#define MID_ROOM 	"MID_ROOM"
#define END_ROOM	"END_ROOM"

/* Room Size Restrictions */
#define MAX_NAME_SIZE 	8
#define MAX_TYPE_SIZE	10
#define MAX_CONNECTIONS 7

/* Room Names - 10 Total, not all used */
#define ROOM_BOB 	"BOB"
#define ROOM_MARTHA	"MARTHA"
#define ROOM_KITCHEN	"KITCHEN"
#define ROOM_DUNGEON	"DUNGEON"
#define ROOM_CLOSET	"CLOSET"
#define ROOM_EMPTY	"EMPTY"
#define	ROOM_GUEST	"GUEST"
#define ROOM_GARAGE	"GARAGE"
#define	ROOM_DEN	"DEN"
#define	ROOM_ATTIC	"ATTIC"

/**
 * Room struct used for unified file structure.
 */
typedef struct {
	char *name;
	char *type;
	char *connections[MAX_CONNECTIONS];
	int nConnections;
} Room;

/**
 * Function Declarations before usage.
 */
void CreateDirAndEnter();
void CreateFileName();
void HasFileInDir();
void WriteFileInDir();
bool IsGraphFull();
void AddRandomConnection();
void CreateAllConnections();
Room GetRandomRoom(); /* Function returns struct Room */
bool CanAddConnectionFrom(Room x);
bool ConnectionAlreadyExists(Room x, Room y);
void ConnectRoom(Room x, Room y);
bool IsSameRoom(Room x, Room y);
bool IsFile(const char *fileName);

/* Store current files and rooms in directory in an array, used for creating connections */
char* filesInDir[7];
/* Used for adding connections, add to connections */
Room roomsInDir[7];

/**
 * Create a directory for the rooms and goes into that directory.
 */
void CreateDirAndEnter() {
	/* 21 bytes = length of "kourched.rooms." + processID */
	char dirName[21];
	int processID = getpid();
	
	/* Clear 21 bytes for use */
	memset(dirName, '\0', 21);
	
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
void CreateFile() {
	/* Create pointer array of all file names */
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
	int minSize = 13; /* minimum bytes required */

	bool fileNotCreated = true; /* loop until file created */
	char *randomFile; /* get random file pointer from array */
	char roomName[minSize]; /* room name for file */
	char fileName[minSize]; /* file name */
	FILE *newFile; /* new file to write to */
	int randomInt = rand() % 10; /* get random int from 0 to 9 */
	
	/* Clear memory for room and file names */
	memset(roomName, '\0', minSize);
	memset(fileName, '\0', minSize);
	
	/* Generate file name if it doesn't exist */
	while (fileNotCreated) {
	    randomFile = allFiles[randomInt]; /* random file from array */
	    strcpy(fileName, randomFile); /* copy string ptr to char array */
	    strcpy(roomName, randomFile); /* copy string ptr to char array */
	    strcat(fileName, "_room");

	    /* Buffer to store stat() information about file */
	    struct stat st = {0};
	    
	    /* File doesn't exist, create file */
	    if (stat(fileName, &st) == -1) {
		newFile = fopen(fileName, "wb+"); /* new file, read/write */
	    	fprintf(newFile, "ROOM NAME: %s\n\n", roomName); /* write to file */
		fileNotCreated = false; /* close loop */
	    } else {
	        randomInt = rand() % 10; /* get new random int */
	    }
	}

	/* Close file after writing to it */
	fclose(newFile);	
}


/**
 * Populates the files in the current working directory.
 */
void PopulateFiles() {

	/* Clear memory for files and rooms */
	memset(filesInDir, '\0', 7);
	memset(roomsInDir, '\0', 7);

	/* Create 7 unique rooms */
	int i = 0;
	for (i; i < 7; i++) {
		CreateFile();
	}
	i = 0;
	DIR * dir;
	struct dirent *d;
	dir = opendir(".");
	while ((d = readdir(dir)) != NULL) {
	    if (IsFile(d->d_name)) {
		filesInDir[i++] = d->d_name;
	    }
	}
	
	Room startRoom;

	/* Allocate space for startRoom name and type */
	startRoom.name = malloc(sizeof(char) * 15);
	startRoom.type = malloc(sizeof(char) * 15);

	/* Copy formatted string, with newline, into room name and type */
	snprintf(startRoom.name, sizeof(char) * 15, "%s", filesInDir[0]);
	snprintf(startRoom.type, sizeof(char) * 15, "%s", START_ROOM);
	memset(startRoom.connections, '\0', MAX_CONNECTIONS);
	startRoom.nConnections = 0;

	/* Create End room */	
	Room endRoom;
	
	/** Allocate space for endRoom name and type */
	endRoom.name = malloc(sizeof(char) * 15);
	endRoom.type = malloc(sizeof(char) * 15);

	/* Copy formatted string, with newline, into room name and type */
	snprintf(endRoom.name, sizeof(char) * 15, "%s", filesInDir[6]);
	snprintf(endRoom.type, sizeof(char) * 15, "%s", END_ROOM);
	memset(endRoom.connections, '\0', MAX_CONNECTIONS);
	endRoom.nConnections = 0;

	/* Place startRoom and endRoom into beginning and end of array, respectively */
	roomsInDir[0] = startRoom;
	roomsInDir[6] = endRoom;
	
	/* Reset counter to second position in array */
	i = 1;
	Room midRoom;

	/* Iterate i - 2 times, we already have 2 rooms in array */
	for (i; i < 6; i++) {

	    /* Allocate space for midRoom name and type */
	    midRoom.name = malloc(sizeof(char) * 12);
	    midRoom.type = malloc(sizeof(char) * 12);

	    /* Copy formatted string, with newline, into room name and type, and set nConnections to zero. */
	    snprintf(midRoom.name, sizeof(char) * 12, "%s", filesInDir[i]);
	    snprintf(midRoom.type, sizeof(char) * 12, "%s", MID_ROOM);
	    /* Allocate memory for array, bytes of char * name size * max number of connections) */
	    memset(midRoom.connections, '\0', MAX_CONNECTIONS);
	    midRoom.nConnections = 0;

	    /* Place room in array */
	    roomsInDir[i] = midRoom;
	}
}


/**
 * Returns true if all rooms have 3 to 6 outbound connections, false otherwise
 */
bool IsGraphFull() {
	int i = 0;
	Room x;
	for (i; i < 7; i++) {
	    x = roomsInDir[i];
	    if (x.nConnections < 3 | x.nConnections > 6) {
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
	while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);

	ConnectRoom(A, B); /* TODO: Add this connection to the real variables */
	ConnectRoom(B, A); /* because this A and B will be destroyed with this function terminates */
}

/**
 * Create all connections in graph.
 */
void CreateAllConnections() {
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
bool CanAddConnectionFrom(Room x) {
	if (x.nConnections < 6)  {
	    return true;
	}
	return false;
}

/**
 * Returns true if a connection from Roox x to Room y already exists, false otherwise.
 * Assumes two way connection between Room x and Room y. 
 * Only have to check if one room is in the array of connections of another room.
 */
bool ConnectionAlreadyExists(Room x, Room y) {
	int i = 0;
	
	for(i; i < MAX_CONNECTIONS; i++) {
	    /* Connection exists in the array of connections of Room x that is Room y */
	    if (strcmp(x.connections[i], y.name)) {
	        return true;
	    }	
	}
	return false;
}

/**
 * Connects Rooms x and y together, does not check if this connection is valid.
 */
void ConnectRoom(Room x, Room y) {
	/* Check connection limits */
	if (CanAddConnectionFrom(x) & CanAddConnectionFrom(y)) {
	    /* Add into connections of x, Room y */	
	    x.connections[x.nConnections] = y.name;
	    x.nConnections++;
	    /* Add into connections of y, Room x */
	    y.connections[y.nConnections] = x.name;
	    y.nConnections++;
	}	
}	

/**
 * Returns true if Rooms x and y are the same Room, false otherwise.
 */
bool IsSameRoom(Room x, Room y) {
	if (strcmp(x.name, y.name) == 1) {
	    return true;
	}
	return false;
}

/**
 * Returns true if fileName is a file.
 */
bool IsFile(const char *fileName) {
	struct stat fileStat;
	stat(fileName, &fileStat);
	return S_ISREG(fileStat.st_mode);
}

void main() {
	/* Initialize time to NULL */ 
	srand(time(NULL));
	/* Create Directory and go into it */
	CreateDirAndEnter();

	/* Populate files in directory */
	PopulateFiles();

	/* Create room connections in files */
}	
