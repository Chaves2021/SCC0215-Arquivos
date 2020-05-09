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
	//Set as 0 all the camps of the struct
	header = (HEADER *) calloc(1, sizeof(HEADER));
	memset(header->lixo, '$', 111);
	header->lixo[111] = '\0'
	return header;
}

REGISTRO *csv_read(FILE *file, HEADER *header)
{
	//+1 to add '\0'
	char buffer[MAX_LEN + 1];
	//Field thats is going go be stored;
	char *str;
	//Saveptr
	char *rest = str;
	while(fgets(buffer, MAX_LEN, file))
	{
		while((token = strtok_r(rest, ",", &rest)))
		{
		}
	}
}
