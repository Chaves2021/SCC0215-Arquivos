#ifndef _FILE_
#define _FILE_

typedef struct registro REGISTRO;
typedef struct header HEADER;

//Function to read a csv file
FILE *csv_open(char *);
//Function to create a reader of a file
HEADER *header_create();
//Function to convert a file .csv to a binary
int csv2binary(FILE *, HEADER *, char *);
//Function to read the binary
int bin_print(char *bin_filename);

#endif
