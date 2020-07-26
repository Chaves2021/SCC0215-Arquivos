#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <btree.h>
#include <defines.h>

struct btree_header
{
	char status;
	int noRaiz;
	int nroNiveis;
	int proxRRN;
	int nroChaves;
};

struct btree_page
{
	int nivel;
	int n;
	int key[ORDER - 1];
	int rrn[ORDER - 1];
	int child[ORDER];
};

BTREE_HEADER *btree_header_create()
{
	BTREE_HEADER *b_header;

	b_header = (BTREE_HEADER *) malloc(1 * sizeof(BTREE_HEADER));
	b_header->status = '1';
	b_header->noRaiz = -1;
	b_header->nroNiveis = 0;
	b_header->proxRRN = 0;
	b_header->nroChaves = 0;

	return b_header;
}

BTREE_PAGE *btree_page_create()
{
	int i;
	BTREE_PAGE *b_page;
	b_page = (BTREE_PAGE *) calloc(1, sizeof(BTREE_PAGE));
	for(i = 0; i < ORDER - 1; i++)
	{
		b_page->key[i] = -1;
		b_page->rrn[i] = -1;
		b_page->child[i] = -1;
	}
	b_page->child[ORDER - 1] = -1;

	return b_page;
}

int btree_header_write(FILE *b_file, BTREE_HEADER *header)
{
	fseek(b_file, 0, SEEK_SET);
	int len = 55;
	fwrite(&header->status, sizeof(char), 1, b_file);
	fwrite(&header->noRaiz, sizeof(int), 1, b_file);
	fwrite(&header->nroNiveis, sizeof(int), 1, b_file);
	fwrite(&header->proxRRN, sizeof(int), 1, b_file);
	fwrite(&header->nroChaves, sizeof(int), 1, b_file);
	while(len--) fwrite("$", sizeof(char), 1, b_file);

	return SUCCESS;
}

int btree_page_write(FILE *b_file, BTREE_PAGE *page, int rrn)
{
	int i;
	fseek(b_file, PAGE_SIZE * (rrn + 1), SEEK_SET);
	fwrite(&page->nivel, sizeof(int), 1, b_file);
	fwrite(&page->n, sizeof(int), 1, b_file);
	for(i = 0; i < ORDER - 1; i++)
	{
		fwrite(&page->key[i], sizeof(int), 1, b_file);
		fwrite(&page->rrn[i], sizeof(int), 1, b_file);
	}
	for(i = 0; i < ORDER; i++) fwrite(&page->child[i], sizeof(int), 1, b_file);

	return SUCCESS;
}

BTREE_PAGE *btree_page_read(FILE *file, int rrn)
{
	fseek(file, PAGE_SIZE * (rrn + 1), SEEK_SET);

	int i;
	BTREE_PAGE *page = (BTREE_PAGE *) malloc(1 * sizeof(BTREE_PAGE));
	fread(&page->nivel, sizeof(int), 1, file);
	fread(&page->n, sizeof(int), 1, file);
	for(i = 0; i < ORDER - 1; i++)
	{
		fread(&page->key[i], sizeof(int), 1, file);
		fread(&page->rrn[i], sizeof(int), 1, file);
	}
	for(i = 0; i < ORDER; i++) fread(&page->child[i], sizeof(int), 1, file);

	return page;
}

BTREE_PAGE *btree_split(BTREE_PAGE *page_left, BTREE_PAGE *page_right, int key, int register_rrn)
{
			int i;
			int aux_key[ORDER], aux_rrn[ORDER], aux_child[ORDER + 1];
			for(i = 0; i < ORDER - 1; i++)
			{
				aux_key[i] = page_left->key[i];
				aux_rrn[i] = page_left->rrn[i];
				aux_child[i] = page_left->child[i];
			}
			//Fill the last array elements of each array aux
			//Except of aux_child, because the last 2 array elems are not filled after the for loop
			aux_key[i] = -1;
			aux_rrn[i] = -1;
			aux_child[i] = page_left->child[i];
			aux_child[i + 1] = -1;

			for(i = ORDER - 1; i > 0 && page_left->key[i - 1] > key; i--)
			{
				aux_key[i] = page_left->key[i];
				aux_rrn[i] = page_left->rrn[i];
				aux_child[i + 1] = page_left->child[i];
			}
			aux_key[i] = key;
			aux_rrn[i] = register_rrn;
			aux_child[i + 1] = -1;

			page_left->n = ORDER / 2;
			//Update the left page
			for(i = 0; i < ORDER - 1; i++)
			{
				if(i < page_left->n)
				{
					page_left->key[i] = aux_key[i];
					page_left->rrn[i] = aux_rrn[i];
					page_left->child[i] = aux_child[i];
				}
				else
				{
					page_left->key[i] = -1;
					page_left->rrn[i] = -1;
					page_left->child[i] = -1;
				}
			}
			page_left->child[i] = -1;

			//Update the right page
			page_right->n = 2;
			page_right->nivel = page_left->nivel;
			for(i = 0; i < ORDER - 1; i++)
			{
				if(i < page_right->n)
				{
					page_right->key[i] = aux_key[i + page_left->n + 1];
					page_right->rrn[i] = aux_rrn[i + page_left->n + 1];
					page_right->child[i] = aux_child[i + page_left->n + 1];
				}
				else
				{
					page_right->key[i] = -1;
					page_right->rrn[i] = -1;
					page_right->child[i] = -1;
				}
			}
			page_right->child[i] = -1;

			BTREE_PAGE *promote = btree_page_create();
			promote->nivel = page_left->nivel + 1;
			promote->n = 1;
			promote->key[0] = aux_key[ORDER / 2];
			promote->rrn[0] = aux_rrn[ORDER / 2];

			return promote;
}

int btree_insert(BTREE_HEADER *header, BTREE_PAGE *page, int key, FILE *b_file, int cur_rrn, int register_rrn)
{
	int i;
	for(i = page->n; i > 0 && page->key[i - 1] > key; i--)
	{
		page->key[i] = page->key[i - 1];
		page->rrn[i] = page->rrn[i - 1];
	}
	page->key[i] = key;
	page->rrn[i] = register_rrn ;
	page->n++;
	header->nroChaves++;
	btree_page_write(b_file, page, cur_rrn);
	return SUCCESS;
}

int debug(BTREE_HEADER *header, BTREE_PAGE *page, int key)
{
	printf("KEY: %d\n", key);

	printf("HEADER\nstatus: %c\nnoRaiz: %d\nnroNiveis: %d\nproxRRN: %d\nnroChaves: %d\n\n",
		header->status, header->noRaiz, header->nroNiveis, header->proxRRN, header->nroChaves);
	printf("PAGE\nnivel: %d\nn: %d\n", page->nivel, page->n);
	for(int i = 0; i < ORDER - 1; i++) printf("chave: %d\nrrn: %d\n", page->key[i], page->rrn[i]);
	for(int i = 0; i < ORDER; i++) printf("child: %d\n", page->child[i]);
	printf("-------------------------------------------------------------\n");
	return SUCCESS;
}

int btree_promote_insert(BTREE_HEADER *header, BTREE_PAGE *page, BTREE_PAGE *promote, FILE *b_file, int cur_rrn)
{
	int i;
	for(i = page->n; i > 0 && page->key[i - 1] > promote->key[0]; i--)
	{
		page->key[i] = page->key[i - 1];
		page->rrn[i] = page->rrn[i - 1];
		page->child[i + 1] = page->child[i];
	}
	page->key[i] = promote->key[0];
	page->rrn[i] = promote->rrn[0];
	page->child[i + 1] = header->proxRRN - 1;
	page->n++;
	btree_page_write(b_file, page, cur_rrn);

	return SUCCESS;
}

BTREE_PAGE *btree_index_insert(BTREE_HEADER *header, BTREE_PAGE *page, int key, FILE *b_file, int cur_rrn, int register_rrn)
{
	if(header->noRaiz == -1)
	{
		page = btree_page_create();
		page->key[0] = key;
		page->rrn[0] = register_rrn;
		page->nivel = 1;
		page->n = 1;
		cur_rrn = 0;
		btree_page_write(b_file, page, cur_rrn);

		header->noRaiz = 0;
		header->nroNiveis++;
		header->nroChaves++;
		header->proxRRN = 1;
		btree_header_write(b_file, header);


		return NULL;
	}
	else
	{

		page = btree_page_read(b_file, cur_rrn);
		debug(header, page, key);

		//If the page is a leaf and there is space
		if(page->nivel == 1 && page->n < ORDER - 1) 
		{
			btree_insert(header, page, key, b_file, cur_rrn, register_rrn);
		}
		//If is a leaf, but there is no space
		else if(page->nivel == 1)
		{
			BTREE_PAGE *page_right = btree_page_create();
			BTREE_PAGE *promote_page = btree_split(page, page_right, key, register_rrn);
			btree_page_write(b_file, page, cur_rrn);
			btree_page_write(b_file, page_right, header->proxRRN);

			header->proxRRN++;

			header->nroChaves++;
			return promote_page;
		}
		//Search for the right place to put the key
		else
		{
			int i;
			for(i = 0; i < page->n && page->key[i] < key ; i++);
			BTREE_PAGE *promote = btree_index_insert(header, page, key, b_file, page->child[i], register_rrn);
			//This case the father node already exists
			if(promote)
			{
				if(page->n < ORDER - 1) 
				{
					btree_promote_insert(header, page, promote, b_file, cur_rrn);
				}
				else
				{
					BTREE_PAGE *page_right = btree_page_create();
					BTREE_PAGE *promote_page = btree_split(page, page_right, promote->key[0], promote->rrn[0]);
					for(i = 0; page->child[i] != -1; i++);
					page->child[i] = header->proxRRN - 4;
					btree_page_write(b_file, page, cur_rrn);
					for(i = 0; page_right->child[i] != -1; i++);
					page_right->child[i] = header->proxRRN - 1;
					btree_page_write(b_file, page_right, header->proxRRN);
					header->proxRRN++;

					return promote_page;
				}
			}
		}
	}
	return NULL;
}

int btree_index_create(char *bin_filename, char *b_filename)
{
	FILE *bin_file = fopen(bin_filename, "rb");
	if(!bin_file) return FILE_BROKEN;
	FILE *b_file = fopen(b_filename, "w+b");
	if(!b_file) 
	{
		fclose(bin_file);
		return FILE_BROKEN;
	}

	BTREE_HEADER *btree_header = btree_header_create();
	BTREE_PAGE *btree_page = (BTREE_PAGE *) malloc(1 * sizeof(BTREE_PAGE));
	HEADER *bin_header = header_read(bin_file);
	REGISTRO *reg;
	int register_rrn;

	btree_header->status = INCONSISTENTE;
	btree_header_write(b_file, btree_header);
	register_rrn = 0;
	while(bin_header->numeroRegistrosInseridos--)
	{
		reg = register_read(bin_file);
		if(reg) 
		{
			BTREE_PAGE *promote = btree_index_insert(btree_header, btree_page, reg->idNascimento, b_file, btree_header->noRaiz, register_rrn);
			if(promote) 
			{
				//This case the father node does not exists
				promote->child[0] = btree_header->noRaiz;
				promote->child[1] = btree_header->proxRRN - 1;
				btree_header->noRaiz = btree_header->proxRRN++;
				btree_header->nroNiveis++;
				btree_page_write(b_file, promote, btree_header->noRaiz);
				btree_page = promote;
			}
		}
		register_rrn++;
	}

	btree_header->status = OK;
	btree_header_write(b_file, btree_header);
	fclose(bin_file);
	fclose(b_file);


	return SUCCESS;
}
