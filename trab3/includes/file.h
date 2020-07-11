#ifndef _FILE_
#define _FILE_

typedef struct registro REGISTRO;
typedef struct header HEADER;
typedef struct combined_search_elem COMBINED_ELEM;
typedef struct combined_search_header COMBINED_HEADER;

//Function to read a csv file
FILE *csv_open(char *);
//Function to create a reader of a file
HEADER *header_create();
//Function to convert a file .csv to a binary
int csv2binary(FILE *, HEADER *, char *);
//Function to print the binary registers
int bin_print(char *bin_filename);
//Function to do the combined search and print the binary registers
int bin_search_print(char *);
//Function to do the combined search and print the binary registers based on RRN
int bin_search_rrn(char *);
//Function to remove registers based on combined search results
int bin_remove(char *);
//Function to insert new registers at the end of the file
int bin_insert(char *);
//Function to update registers
int bin_update(char *);

#endif
