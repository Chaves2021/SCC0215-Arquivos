#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defines.h>
#include <file.h>

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

FILE *csv_open(char *filename)
{
	FILE *file;
	file = fopen(filename, "r");
	return file;
}

HEADER *header_create()
{
	HEADER *header;
	//Set as 0 all the fields of the struct
	header = (HEADER *) calloc(1, sizeof(HEADER));
	memset(header->lixo, '$', 111);
	header->lixo[111] = '\0';
	return header;
}

//Len is the length that the buffer is going through
//Each time it's increased to reach the next field of the line
//It's increased +1 because it's need to pass the "," that limits the field
REGISTRO *store_struct(char *buffer)
{
	REGISTRO *reg = (REGISTRO *) malloc(1*sizeof(REGISTRO));
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
	memset(reg->dataNascimento, '$', 10);
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
	memset(reg->estadoMae, '$', 3);
	strcpy(reg->estadoMae, str);
	len += strlen(str) + 1;
	//Reads estadoBebe, and store it
	memset(str, '\0', MAX_LEN);
	sscanf(buffer + len, " %s", str);
	memset(reg->estadoBebe, '$', 3);
	strcpy(reg->estadoBebe, str);
	return reg;
}

FILE *write_binary(REGISTRO *reg, FILE *file)
{
	//Calculates what was not ocuppied to write "$"
	int len = 105 - (8 + reg->tamanhoCidadeMae + reg->tamanhoCidadeBebe);

	fwrite(&reg->tamanhoCidadeMae, sizeof(int), 1, file);
	fwrite(&reg->tamanhoCidadeBebe, sizeof(int), 1, file);
	fwrite(reg->cidadeMae, sizeof(char), reg->tamanhoCidadeMae, file);
	fwrite(reg->cidadeBebe, sizeof(char), reg->tamanhoCidadeBebe, file);
	while(len--) fwrite("$", sizeof(char), 1, file);
	fwrite(&reg->idNascimento, sizeof(int), 1, file);
	fwrite(&reg->idadeMae, sizeof(int), 1, file);
	fwrite(reg->dataNascimento, sizeof(char), 10, file);
	fwrite(&reg->sexoBebe, sizeof(char), 1, file);
	fwrite(reg->estadoMae, sizeof(char), 2, file);
	fwrite(reg->estadoBebe, sizeof(char), 2, file);
	return SUCCESS;
}

//Write the header of the binary file
int write_binary_header(HEADER *header, FILE *file)
{
	//Go to the start of the file to write the header
	fseek(file, 0, SEEK_SET);

	fwrite(&header->status, sizeof(char), 1, file);
	fwrite(&header->RRNproxRegistro, sizeof(int), 1, file);
	fwrite(&header->numeroRegistrosInseridos, sizeof(int), 1, file);
	fwrite(&header->numeroRegistrosRemovidos, sizeof(int), 1, file);
	fwrite(&header->numeroRegistrosAtualizados, sizeof(int), 1, file);
	fwrite(header->lixo, sizeof(char), 111, file);
	return SUCCESS;
}

int csv2binary(FILE *csv_file, HEADER *header, char *bin_filename)
{
	if(!csv_file) return FILE_BROKEN;
	FILE *bin_file;
	bin_file = fopen(bin_filename, "wb");
	if(!bin_file) return FILE_BROKEN;

	REGISTRO *registro;
	char buffer[MAX_LEN];
	//Ignores the first line
	fgets(buffer, MAX_LEN, csv_file);
	//Pass the first 128 bytes to write the header after
	fseek(bin_file, 128, SEEK_SET);
	//While there is something to read
	while(fgets(buffer, MAX_LEN, csv_file))
	{
		registro = store_struct(buffer);
		write_binary(registro, bin_file);
		header->numeroRegistrosInseridos++;
	}
	header->status = OK;
	header->RRNproxRegistro = header->numeroRegistrosInseridos;
	fseek(bin_file, 0, SEEK_SET);
	write_binary_header(header, bin_file);

	fclose(bin_file);
	return SUCCESS;
}

//Reads the header from the binary and store it in the ram
HEADER *header_read(FILE *file)
{
	HEADER *header = header_create();

	fread(&header->status, sizeof(char), 1, file);
	fread(&header->RRNproxRegistro, sizeof(int), 1, file);
	fread(&header->numeroRegistrosInseridos, sizeof(int), 1, file);
	fread(&header->numeroRegistrosRemovidos, sizeof(int), 1, file);
	fread(&header->numeroRegistrosAtualizados, sizeof(int), 1, file);
	fread(&header->numeroRegistrosAtualizados, sizeof(int), 1, file);
	fread(header->lixo, sizeof(char), 111, file);
	header->lixo[111] = '\0';
	return header;

}

//Reads the register from the binary and store it in the ram
REGISTRO *register_read(FILE *file)
{
	REGISTRO *reg;
	reg = (REGISTRO *) malloc(sizeof(REGISTRO));

	fread(&reg->tamanhoCidadeMae, sizeof(int), 1, file);
	fread(&reg->tamanhoCidadeBebe, sizeof(int), 1, file);
	memset(reg->cidadeMae, '\0', 105);
	fread(reg->cidadeMae, reg->tamanhoCidadeMae, 1, file);
	memset(reg->cidadeBebe, '\0', 105);
	fread(reg->cidadeBebe, reg->tamanhoCidadeBebe, 1, file);
	fread(&reg->idNascimento, sizeof(int), 1, file);
	fread(&reg->idadeMae, sizeof(int), 1, file);
	memset(reg->dataNascimento, '\0', 11);
	fread(reg->dataNascimento, sizeof(char), 10, file);
	fread(&reg->sexoBebe, sizeof(char), 1, file);
	memset(reg->estadoMae, '\0', 3);
	fread(reg->estadoMae, sizeof(char), 2, file);
	memset(reg->estadoBebe, '\0', 3);
	fread(reg->estadoBebe, sizeof(char), 2, file);
	return reg;
}

//Format the output
int register_print(REGISTRO *reg)
{
	printf("Nasceu em ");
	if(reg->tamanhoCidadeBebe > 0) printf("%s/ ", reg->cidadeBebe);
	else printf("-/");

	if(strlen(reg->estadoBebe) > 0) printf("%s,", reg->estadoBebe);
	else printf("-, ");

	printf("em ");
	if(strlen(reg->dataNascimento) > 0) printf("%s,", reg->dataNascimento);
	else printf("-, ");

	printf("um bebê de sexo ");
	if(reg->sexoBebe == '0') printf("IGNORADO");
	else if(reg->sexoBebe == '1') printf("MASCULINO");
	else if(reg->sexoBebe == '2') printf("FEMININO");

	printf(".\n");

	return SUCCESS;
}

int bin_print(char *bin_filename)
{
	char status;
	FILE *bin_file = fopen(bin_filename, "rb");
	if(!bin_file) return FILE_BROKEN;
	fread(&status, sizeof(char), 1, bin_file);
	if(status == '0') return FILE_BROKEN;
	//Put the pointer back to the start of the file
	fseek(bin_file, 0, SEEK_SET);

	HEADER *header = header_read(bin_file);
	REGISTRO *reg;

	//While there is registers to print
	while(header->numeroRegistrosInseridos--)
	{
		reg = register_read(bin_file);
		register_print(reg);
	}

	fclose(bin_file);
	return SUCCESS;
}
