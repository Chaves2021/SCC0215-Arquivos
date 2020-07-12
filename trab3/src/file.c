#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defines.h>
#include <file.h>
#include <binarioNaTela.h>

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

//Struct element to store the name and value of the field that is going to be used in comparison
struct combined_search_elem
{
	char field_name[105];
	union
	{
		char string_value[105];
		int int_value;
	}value;

};

//Struct header that contains how many fields are going to be used in the combined search, and a list of them as elements
struct combined_search_header
{
	COMBINED_ELEM **elem_list;
	int n_fields;
};

//Function to allocate memory to combined_header vars 
COMBINED_HEADER *combined_search_create(int n_fields)
{
	int i;

	COMBINED_HEADER *ch = (COMBINED_HEADER *) malloc(sizeof(COMBINED_HEADER));
	ch->n_fields = n_fields;
	ch->elem_list = (COMBINED_ELEM **) malloc(n_fields * sizeof(COMBINED_ELEM *));
	for(i = 0; i < n_fields; i++) ch->elem_list[i] = (COMBINED_ELEM *) malloc(1 * sizeof(COMBINED_ELEM));
	return ch;
}

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
	//Reads idNascimento, and store it
	memset(str, '\0', MAX_LEN);
	sscanf(buffer + len, " %[^,]", str);
	reg->idNascimento = atoi(str);
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
	//I don't know why "fwrite("$", sizeof(char), len, file);" was not working
	//It was giving an undefined behavior 
	//This method worked so I used it
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
	//Flag to say if the register points to some memory space or not
	int isAllocated = FALSE;
	//Ignores the first line
	fgets(buffer, MAX_LEN, csv_file);
	//Skips the first 128 bytes to write the header after
	fseek(bin_file, 128, SEEK_SET);
	//If there is something to read, flag receives true
	//If fgets fails at first attempt, flag continues false
	while(fgets(buffer, MAX_LEN, csv_file) && (isAllocated = TRUE))
	{
		registro = store_struct(buffer);
		write_binary(registro, bin_file);
		header->numeroRegistrosInseridos++;
	}
	header->status = OK;
	header->RRNproxRegistro = header->numeroRegistrosInseridos;
	//Return to the beggining of the file to write the header
	fseek(bin_file, 0, SEEK_SET);
	write_binary_header(header, bin_file);

	//The header is always created in main, so i can free it without check
	free(header);
	//If the file is null, register not points to anything, so free gives error
	if(isAllocated) free(registro);
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
	fread(header->lixo, sizeof(char), 111, file);
	header->lixo[111] = '\0';
	return header;

}

//Reads the register from the binary and store it in the ram
REGISTRO *register_read(FILE *file)
{
	REGISTRO *reg;
	reg = (REGISTRO *) malloc(sizeof(REGISTRO));

	char lixo[105];
	memset(lixo, '\0', 105);

	fread(&reg->tamanhoCidadeMae, sizeof(int), 1, file);
	if(reg->tamanhoCidadeMae == REMOVED) 
	{
		fseek(file, 124, SEEK_CUR);
		return NULL;
	}
	fread(&reg->tamanhoCidadeBebe, sizeof(int), 1, file);
	memset(reg->cidadeMae, '\0', 105);
	fread(reg->cidadeMae, reg->tamanhoCidadeMae, 1, file);
	memset(reg->cidadeBebe, '\0', 105);
	fread(reg->cidadeBebe, reg->tamanhoCidadeBebe, 1, file);
	fread(lixo, 105 -(8 + reg->tamanhoCidadeBebe + reg->tamanhoCidadeMae), 1, file);
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
	if(reg->tamanhoCidadeBebe > 0) printf("%s/", reg->cidadeBebe);
	else printf("-/");

	if(strlen(reg->estadoBebe) > 0) printf("%s, ", reg->estadoBebe);
	else printf("-, ");

	printf("em ");
	if(strlen(reg->dataNascimento) > 0) printf("%s, ", reg->dataNascimento);
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
	if(status == INCONSISTENTE) return FILE_BROKEN;
	//Put the pointer back to the start of the file
	fseek(bin_file, 0, SEEK_SET);

	HEADER *header = header_read(bin_file);
	REGISTRO *reg;

	if(header->numeroRegistrosInseridos)
	{

		//While there is registers to print
		while(header->numeroRegistrosInseridos--)
		{
			reg = register_read(bin_file);
			register_print(reg);
		}

		free(header);
		free(reg);
	}
	else printf("Registro Inexistente.\n");

	fclose(bin_file);
	return SUCCESS;
}

//Function to see compare the register field and the field in the search 
int isMatch(COMBINED_ELEM *ce, REGISTRO *reg)
{
	if(!strcmp("cidadeMae", ce->field_name))
	{
		if(!strcmp(ce->value.string_value, reg->cidadeMae))
			return TRUE;
		else
			return FALSE;
	}
	if(!strcmp("cidadeBebe", ce->field_name))
	{
		if(!strcmp(ce->value.string_value, reg->cidadeBebe))
			return TRUE;
		else
			return FALSE;
	}
	if(!strcmp("idNascimento", ce->field_name))
	{
		if(ce->value.int_value == reg->idNascimento)
			return TRUE;
		else
			return FALSE;
	}
	if(!strcmp("idadeMae", ce->field_name))
	{
		if(ce->value.int_value == reg->idadeMae)
			return TRUE;
		else
			return FALSE;
	}
	if(!strcmp("dataNascimento", ce->field_name))
	{
		if(!strcmp(ce->value.string_value, reg->dataNascimento))
			return TRUE;
		else
			return FALSE;
	}
	if(!strcmp("sexoBebe", ce->field_name))
	{
		if(ce->value.string_value[0] == reg->sexoBebe)
			return TRUE;
		else
			return FALSE;
	}
	if(!strcmp("estadoMae", ce->field_name))
	{
		if(!strcmp(ce->value.string_value, reg->estadoMae))
			return TRUE;
		else
			return FALSE;
	}
	if(!strcmp("estadoBebe", ce->field_name))
	{
		if(!strcmp(ce->value.string_value, reg->estadoBebe))
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

//Function to search with ch already filled and returns a register that matches with the conditions
//Or returns NULL to say that the register does not match
REGISTRO *combined_search_register(COMBINED_HEADER *ch, FILE *bin_file)
{
	int found;
	int i;
	REGISTRO *reg;
	reg = register_read(bin_file);
	if(reg)
	{
		found = TRUE;
		for(i  = 0; i < ch->n_fields && found; i++) found = isMatch(ch->elem_list[i], reg);
		if(found)
		{
			return reg;
		}
	}
	return NULL;
}

//Function to store the fields of the combined search in the struct
COMBINED_HEADER *combined_search_fill(COMBINED_HEADER *ch, int n_fields)
{
	int i;
	for(i = 0; i < n_fields; i++)
	{
		scanf(" %s", ch->elem_list[i]->field_name);
		if(!strcmp("idadeMae", ch->elem_list[i]->field_name) || 
				!strcmp("idNascimento", ch->elem_list[i]->field_name))
		{
			scanf("%d", &(ch->elem_list[i]->value.int_value));
		}
		else
		{
			scan_quote_string(ch->elem_list[i]->value.string_value);
		}
	}
	return ch;
}

int combined_search_free(COMBINED_HEADER **ch)
{
	for(int i = 0; i < (*ch)->n_fields; i++) free((*ch)->elem_list[i]);
	free((*ch)->elem_list);
	free(*ch);
	return SUCCESS;
}

int bin_search_print(char *bin_filename)
{
	FILE *bin_file = fopen(bin_filename, "rb");
	//Check if file is broken
	if(!bin_file) return FILE_BROKEN;
	HEADER *header = header_read(bin_file);
	//Check if status is ok
	if(header->status == INCONSISTENTE) return FILE_BROKEN;
	//Check if there is a valid register
	if(!header->numeroRegistrosInseridos) return NO_REGISTER;

	int n_fields;
	//Flag to say if found at least 1 one register that matches
	int found = FALSE;
	int aux = header->numeroRegistrosInseridos;
	COMBINED_HEADER *ch;
	REGISTRO *reg;

	scanf("%d", &n_fields);
	ch = combined_search_create(n_fields);
	ch = combined_search_fill(ch, n_fields);

	while(aux--)
	{
		reg = combined_search_register(ch, bin_file);
		if(reg) 
		{
			found = TRUE;
			register_print(reg);
		}
	}
	if(!found) printf("Registro Inexistente.\n");

	fclose(bin_file);
	free(header);
	free(reg);
	combined_search_free(&ch);
	return SUCCESS;
}

int bin_search_rrn(char *bin_filename)
{
	FILE *bin_file = fopen(bin_filename, "rb");
	//Check if file is broken
	if(!bin_file) return FILE_BROKEN;
	HEADER *header = header_read(bin_file);
	//Check if status is ok
	if(header->status == INCONSISTENTE) return FILE_BROKEN;
	//Check if there is a valid register
	//if(!header->numeroRegistrosInseridos) return NO_REGISTER;

	int rrn;
	REGISTRO *reg;
	scanf("%d", &rrn);

	if(rrn < 0 || rrn >= header->RRNproxRegistro) return NO_REGISTER;
	else
	{
		//Pass the header
		fseek(bin_file, 128 * (rrn + 1), SEEK_SET);
		reg = register_read(bin_file);
		if(!reg) return NO_REGISTER;
		else register_print(reg);
	}
	fclose(bin_file);
	free(header);
	free(reg);
	return SUCCESS;
}

int bin_remove(char *bin_filename)
{
	FILE *bin_file = fopen(bin_filename, "rb+");
	//Check if file is broken
	if(!bin_file) return FILE_BROKEN;
	fseek(bin_file, 0, SEEK_SET);
	HEADER *header = header_read(bin_file);
	//Check if status is ok
	if(header->status == INCONSISTENTE) return FILE_BROKEN;

	header->status = INCONSISTENTE;
	int aux;
	//Change the file while inserting
	fseek(bin_file, 0, SEEK_SET);
	write_binary_header(header, bin_file);

	int i, j;
	//Number removals and number of fields of each removal
	int n_removes, n_fields;
	COMBINED_HEADER *ch = NULL;
	REGISTRO *reg = NULL;
	int removed = -1;

	scanf("%d", &n_removes);
	ch = (COMBINED_HEADER *) malloc(n_removes * sizeof(COMBINED_HEADER ));
	aux = header->numeroRegistrosInseridos;
	for(i = 0; i < n_removes; i++)
	{
		//aux = header->numeroRegistrosInseridos;
		scanf("%d", &n_fields);
		ch = combined_search_create(n_fields);
		ch = combined_search_fill(ch, n_fields);
		fseek(bin_file, 128, SEEK_SET);
		//While there is registers
		for(j = 0; j < aux; j++)
		{
			reg = combined_search_register(ch, bin_file);
			if(reg) 
			{
				fseek(bin_file, -128, SEEK_CUR);
				fwrite(&removed, sizeof(int), 1, bin_file);
				fseek(bin_file, 124, SEEK_CUR);
				header->numeroRegistrosInseridos--;
				header->numeroRegistrosRemovidos++;
			}
		}
	}

	header->status = OK;
	memset(header->lixo, '$', 111);
	//Return to the beggining of the file to write the header
	fseek(bin_file, 0, SEEK_SET);
	write_binary_header(header, bin_file);

	fclose(bin_file);
	free(header);
	if(reg) free(reg);
	combined_search_free(&ch);

	return SUCCESS;
}

//Read the fields of the register in stdin and store it at the struct
REGISTRO *read_fields()
{
	REGISTRO *reg = (REGISTRO *) malloc(sizeof(REGISTRO));
	char str[MAX_LEN];
	int aux;
	//tamanhoCidadeMae
	scan_quote_string(str);
	reg->tamanhoCidadeMae = strlen(str);
	strcpy(reg->cidadeMae, str);
	//tamanhoCidadeBebe
	scan_quote_string(str);
	reg->tamanhoCidadeBebe = strlen(str);
	strcpy(reg->cidadeBebe, str);
	//idNascimento
	scanf("%d", &aux);
	reg->idNascimento = aux;
	//idadeMae
	scanf(" %s", str);
	if(str[0] == 'N') reg->idadeMae = -1;
	else reg->idadeMae = atoi(str);
	//dataNascimento
	memset(reg->dataNascimento, '$', 11);
	scan_quote_string(str);
	strcpy(reg->dataNascimento, str);
	//sexoBebe
	scan_quote_string(str);
	if(!strcmp(str, "")) reg->sexoBebe = '0';
	else reg->sexoBebe = str[0];
	//estadoMae
	memset(reg->estadoMae, '$', 3);
	scan_quote_string(str);
	strcpy(reg->estadoMae, str);
	//estadoBebe
	memset(reg->estadoBebe, '$', 3);
	scan_quote_string(str);
	strcpy(reg->estadoBebe, str);

	return reg;
}

int bin_insert(char *bin_filename)
{
	FILE *bin_file = fopen(bin_filename, "rb+");
	//Check if file is broken
	if(!bin_file) return FILE_BROKEN;
	fseek(bin_file, 0, SEEK_SET);
	HEADER *header = header_read(bin_file);
	//Check if status is ok
	if(header->status == INCONSISTENTE) return FILE_BROKEN;
	//Check if there is a valid register
	//if(!header->numeroRegistrosInseridos) return NO_REGISTER;

	header->status = INCONSISTENTE;
	int n_inserts;
	int i;
	REGISTRO *reg;
	//Change the header to INCONSISTENTE
	fseek(bin_file, 0, SEEK_SET);
	write_binary_header(header, bin_file);
	scanf("%d", &n_inserts);

	fseek(bin_file, 0, SEEK_END);
	for(i = 0; i < n_inserts; i++)
	{
		reg = read_fields();
		write_binary(reg, bin_file);
		header->numeroRegistrosInseridos++;
	}
	header->status = OK;
	header->RRNproxRegistro = header->numeroRegistrosInseridos;
	memset(header->lixo, '$', 111);
	//Return to the beggining of the file to write the header
	fseek(bin_file, 0, SEEK_SET);
	write_binary_header(header, bin_file);

	fclose(bin_file);
	free(header);
	free(reg);

	return SUCCESS;
}

//This function update the register fields comparing field_name with all fields
//and after updating all reg fields, write in the bin file
int register_update(REGISTRO *reg, COMBINED_HEADER *ch, FILE *file)
{
	int i = 0;
	for(i = 0; i < ch->n_fields; i++)
	{
		if(!strcmp("cidadeMae", ch->elem_list[i]->field_name))
		{
			strcpy(reg->cidadeMae, ch->elem_list[i]->value.string_value);
			reg->tamanhoCidadeMae = strlen(reg->cidadeMae);
		}
		else if(!strcmp("cidadeBebe", ch->elem_list[i]->field_name))
		{
			strcpy(reg->cidadeBebe, ch->elem_list[i]->value.string_value);
			reg->tamanhoCidadeBebe = strlen(reg->cidadeBebe);
		}
		else if(!strcmp("idNascimento", ch->elem_list[i]->field_name))
		{
			reg->idNascimento = ch->elem_list[i]->value.int_value;
		}
		else if(!strcmp("idadeMae", ch->elem_list[i]->field_name))
		{
			if(ch->elem_list[i]->value.int_value == 0) reg->idadeMae = -1;
			else reg->idadeMae = ch->elem_list[i]->value.int_value;
		}
		else if(!strcmp("dataNascimento", ch->elem_list[i]->field_name))
		{
			if(strlen(ch->elem_list[i]->value.string_value) > 0) strcpy(reg->dataNascimento, ch->elem_list[i]->value.string_value);
			else reg->dataNascimento[0] = '\0';
		}
		else if(!strcmp("sexoBebe", ch->elem_list[i]->field_name))
		{
			if(strlen(ch->elem_list[i]->value.string_value) > 0) reg->sexoBebe = ch->elem_list[i]->value.string_value[0];
			else reg->sexoBebe = '0';
		}
		else if(!strcmp("estadoMae", ch->elem_list[i]->field_name))
		{
			if(strlen(ch->elem_list[i]->value.string_value) > 0) strcpy(reg->estadoMae, ch->elem_list[i]->value.string_value);
			else reg->estadoMae[0] = '\0';
		}
		else if(!strcmp("estadoBebe", ch->elem_list[i]->field_name))
		{
			if(strlen(ch->elem_list[i]->value.string_value) > 0) strcpy(reg->estadoBebe, ch->elem_list[i]->value.string_value);
			else reg->estadoBebe[0] = '\0';
		}
	}

	int len = 105 - (8 + reg->tamanhoCidadeMae + reg->tamanhoCidadeBebe);
	char aux[MAX_LEN];
	fwrite(&reg->tamanhoCidadeMae, sizeof(int), 1, file);
	fwrite(&reg->tamanhoCidadeBebe, sizeof(int), 1, file);
	fwrite(reg->cidadeMae, sizeof(char), reg->tamanhoCidadeMae, file);
	fwrite(reg->cidadeBebe, sizeof(char), reg->tamanhoCidadeBebe, file);
	while(len--) fread(&aux, sizeof(char), 1, file);
	fwrite(&reg->idNascimento, sizeof(int), 1, file);
	fwrite(&reg->idadeMae, sizeof(int), 1, file);
	fwrite(reg->dataNascimento, sizeof(char), 10, file);
	fwrite(&reg->sexoBebe, sizeof(char), 1, file);
	fwrite(reg->estadoMae, sizeof(char), 2, file);
	fwrite(reg->estadoBebe, sizeof(char), 2, file);

	return SUCCESS;
}

int bin_update(char *bin_filename)
{
	FILE *bin_file = fopen(bin_filename, "rb+");
	//Check if file is broken
	if(!bin_file) return FILE_BROKEN;
	fseek(bin_file, 0, SEEK_SET);
	HEADER *header = header_read(bin_file);
	//Check if status is ok
	if(header->status == INCONSISTENTE) return FILE_BROKEN;
	//Check if there is a valid register
	//if(!header->numeroRegistrosInseridos) return NO_REGISTER;

	header->status = INCONSISTENTE;
	int n_updates, n_fields;
	int i;
	int rrn;
	int flag = FALSE;
	REGISTRO *reg;
	COMBINED_HEADER *ch;
	//Change the header to INCONSISTENTE
	fseek(bin_file, 0, SEEK_SET);
	write_binary_header(header, bin_file);
	scanf("%d", &n_updates);

	for(i = 0; i < n_updates; i++)
	{
		scanf("%d", &rrn);
		if(rrn >= 0 && rrn < header->RRNproxRegistro)
		{
			flag = TRUE;

			scanf("%d", &n_fields);
			ch = combined_search_create(n_fields);
			ch = combined_search_fill(ch, n_fields);

			fseek(bin_file, (rrn + 1) * 128, SEEK_SET);
			reg = register_read(bin_file);
			if(reg && reg->tamanhoCidadeMae != REMOVED)
			{
				fseek(bin_file, -128, SEEK_CUR);
				register_update(reg, ch, bin_file);
				header->numeroRegistrosAtualizados++;
			}
		}

	}

	header->status = OK;
	//Return to the beggining of the file to write the header
	fseek(bin_file, 0, SEEK_SET);
	write_binary_header(header, bin_file);

	fclose(bin_file);
	free(header);
	free(reg);
	if(flag) combined_search_free(&ch);

	return SUCCESS;
}
