#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defines.h>

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

FILE *csv_open(char *file_name)
{
	FILE *file;
	file = fopen(filename, "rb");
	return file;
}

HEADER *create_header()
{
	HEADER *header;
	//Set as 0 all the fields of the struct
	header = (HEADER *) calloc(1, sizeof(HEADER));
	memset(header->lixo, '$', 110);
	header->lixo[111] = '\0';
	return header;
}

//Len is the length that the buffer is going through
//Each time it's increased to reach the next field of the line
//It's increased +1 because it's need to pass the "," that limits the field
REGISTRO *store_struct(char *buffer)
{
	REGISTRO *reg = (REGISTRO *) malloc(sizeof(REGISTRO));
	char str[MAX_LEN]; 

	//Reads cidadeMae, and store its size
	int len = 0;
	memset(str, '\0', MAX_LEN);
	sscanf(buffer + len, " %[^,]", str);
	strcpy(reg->cidadeMae, str);
	reg->tamanhoCidadeMae = strlen(str);
	len += strlen(str) + 1;
	//Reads cidadeBebe, and store its size
	memset(str, '\0', MAX_LEN);
	sscanf(buffer + len, " %[^,]", str);
	strcpy(reg->cidadeBebe, str);
	reg->tamanhoCidadeBebe = strlen(str);
	len += strlen(str) + 1;
	//Reads idNascimento, and store it if exists, or store -1 if not
	memset(str, '\0', MAX_LEN);
	sscanf(buffer + len, " %[^,]", str);
	if(strlen(str) > 0) reg->idNascimento = atoi(str);
	else reg->idNascimento = -1;
	len += strlen(str) + 1;
	//Reads idadeMae, and store it if exists, or store -1 if not
	memset(str, '\0', MAX_LEN);
	sscanf(buffer + len, " %[^,]", str);
	if(strlen(str) > 0) reg->idadeMae = atoi(str);
	else reg->idadeMae = -1;
	len += strlen(str) + 1;
	//Reads dataNascimento, and store it, does not needs to check :)
	memset(str, '\0', MAX_LEN);
	sscanf(buffer + len, " %[^,]", str);
	strcpy(reg->dataNascimento, str);
	len += strlen(str) + 1;
	//Reads sexoBebe, and store it if determinated, or 0 if is inderterminate
	memset(str, '\0', MAX_LEN);
	sscanf(buffer + len, " %[^,]", str);
	if(strlen(str) > 0) reg->sexoBebe = str[0];
	else reg->sexoBebe = '0';
	len += strlen(str) + 1;
	//Reads estadoMae, and store it
	memset(str, '\0', MAX_LEN);
	sscanf(buffer + len, " %[^,]", str);
	strcpy(reg->estadoMae, str);
	len += strlen(str) + 1;
	//Reads estadoBebe, and store it
	memset(str, '\0', MAX_LEN);
	sscanf(buffer + len, " %s", str);
	strcpy(reg->estadoBebe, str);
	len += strlen(str) + 1;

	return reg;
}

int write_binary(REGISTRO *reg, char *bin_filename)
{
	//Calculates what was not ocuppied to write "$"
	int len = 105 - (8 + reg->tamanhoCidadeMae + reg->tamanhoCidadeBebe);
	FILE *file;
	file = fopen(filename, "ab");

	fwrite(&reg->tamanhoCidadeMae, sizeof(int), 1, file);
	fwrite(&reg->tamanhoCidadeBebe, sizeof(int), 1, file);
	fwrite(reg->cidadeMae, sizeof(char), reg->tamanhoCidadeMae, file);
	fwrite(reg->cidadeBebe, sizeof(char), reg->tamanhoCidadeBebe, file);
	fwrite("$", sizeof(char), len, file);
	fwrite(&reg->idNascimento, sizeof(int), 1, file);
	fwrite(&reg->idadeMae, sizeof(int), 1, file);
	fwrite(reg->dataNascimento, sizeof(char), 10, file);
	fwrite(&reg->sexoBebe, sizeof(char), 1, file);
	fwrite(reg->estadoMae, sizeof(char), 2, file);
	fwrite(reg->estadoBebe, sizeof(char), 2, file);
	
	return SUCCESS;
}

int csv2binary(FILE *file, HEADER *header, char *bin_filename)
{
	if(!file) return FILE_BROKEN;
	REGISTRO *registro;
	char buffer[MAX_LEN];
	//While there is something to read
	while(fgets(buffer, MAX_LEN, file))
	{
		registro = store_struct(buffer);
		write_binary(registro, bin_filename);
		header->numeroRegistrosInseridos++;
	}
	header->status = OK;
	header->RRNproxRegistro = header->numeroRegistrosInseridos;
	return SUCCESS;
}
