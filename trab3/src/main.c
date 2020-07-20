//Caio Marcos Chaves Viana - 11208217


#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <binarioNaTela.h>
#include <btree.h>
#include <defines.h>

int main(void)
{
	HEADER *header;
	char csv_filename[50];
	char bin_filename[50];
	char b_filename[50];
	FILE *csv_file;
	int opt; //Variable to store the option by the user
	int ret;
	//Reads the option
	scanf("%d", &opt);
	switch(opt)
	{
		case 1:
			scanf(" %s %s", csv_filename, bin_filename);
			header = header_create();
			csv_file = csv_open(csv_filename);;
			//Execute the function and test if there are errors
			if(csv2binary(csv_file, header, bin_filename) != SUCCESS) printf("Falha no carregamento do arquivo.\n");
			else{
				fclose(csv_file);
				binarioNaTela(bin_filename);
			}
			break;
		case 2:
			scanf(" %s", bin_filename);
			if(bin_print(bin_filename) != SUCCESS) printf("Falha no processamento do arquivo.\n");
			break;
		case 3:
			scanf(" %s", bin_filename);
			ret = bin_search_print(bin_filename);
			if(ret == FILE_BROKEN) printf("Falha no processamento do arquivo.\n");
			else if(ret == NO_REGISTER) printf("Registro Inexistente.\n");
			break;
		case 4:
			scanf(" %s", bin_filename);
			ret = bin_search_rrn(bin_filename);
			if(ret == FILE_BROKEN) printf("Falha no processamento do arquivo.\n");
			else if(ret == NO_REGISTER) printf("Registro Inexistente.\n");
			break;
		case 5:
			scanf(" %s", bin_filename);
			if(bin_remove(bin_filename) == SUCCESS) binarioNaTela(bin_filename);
			else printf("Falha no processamento do arquivo.\n");
			break;
		case 6:
			scanf(" %s", bin_filename);
			ret = bin_insert(bin_filename);
			if(ret != SUCCESS) printf("Falha no processamento do arquivo.\n");
			else binarioNaTela(bin_filename);
			break;
		case 7:
			scanf(" %s", bin_filename);
			ret = bin_update(bin_filename);
			if(ret != SUCCESS) printf("Falha no processamento do arquivo.\n");
			else binarioNaTela(bin_filename);
			break;
		case 8:
			scanf(" %s %s", bin_filename, b_filename);
			ret = btree_index_create(bin_filename, b_filename);
			if(ret != SUCCESS) printf("Falha no processamento do arquivo.\n");
			//else binarioNaTela(b_filename);
		default:
			break;
	}

	return SUCCESS;
}
