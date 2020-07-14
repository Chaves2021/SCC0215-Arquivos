#ifndef _FILE_
#define _FILE_

struct header
{
	char status;
	int RRNproxRegistro;
	int numeroRegistrosInseridos;
	int numeroRegistrosRemovidos;
	int numeroRegistrosAtualizados;
	char lixo[112];
};

struct registro
{
	int tamanhoCidadeMae;
	int tamanhoCidadeBebe;
	char cidadeMae[105];
	char cidadeBebe[105];
	int idNascimento;
	int idadeMae;
	char dataNascimento[11];
	char sexoBebe;
	char estadoMae[3];
	char estadoBebe[3];
};



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
//TODO
HEADER *header_read(FILE *);
//TODO
REGISTRO *register_read(FILE *);

#endif
