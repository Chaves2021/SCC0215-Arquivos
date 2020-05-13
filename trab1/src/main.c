//Caio Marcos Chaves Viana - 11208217


#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <binarioNaTela.h>
#include <defines.h>

int main(void)
{
	HEADER *header;
	char csv_filename[50];
	char bin_filename[50];
	FILE *csv_file;
	int opt; //Variable to store the option by the user
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
		default:
			break;
	}

	return SUCCESS;
}
