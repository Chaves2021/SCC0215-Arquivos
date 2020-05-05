#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct header
{
	char status;
	int RRNproxRegistro;
	int numeroRegistrosInseridos;
	int numeroRegistrosRemovidos;
	int numeroRegistrosAtualizados;
	char *lixo;
};

struct registro
{
	int tamanhoCidadeMae;
	int tamanhoCidadeBebe;
	char *cidadeMae;
	char *cidadeBebe;
	int idNascimento;
	int idadeMae;
	char dataNascimento[10];
	char sexoBebe;
	char estadoMae[2];
	char estadoBebe[2];
};

FILE *csv_open(char *file_name)
{
	FILE *file;
	file = fopen(filename, "rb");
	return file;
}

HEADER *csv_read(FILE *file)
{
	char *line_buffer;
	line_buffer = (char *) malloc(MAX_LINE, sizeof(char));
	memset(line_buffer, "$", MAX_LINE);
	//TODO
	//Needs to know how to read a single file of .csv
	//fgets(line_buffer, MAX_LINE, file);
	return SUCCESS;
}
