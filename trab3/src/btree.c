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

BTREE_PAGE *btree_split(BTREE_PAGE *left_page, BTREE_PAGE *right_page, int key, int register_rrn)
{
			int i;
			int aux_key[ORDER], aux_rrn[ORDER], aux_child[ORDER + 1];
			for(i = 0; i < ORDER - 1; i++)
			{
				aux_key[i] = left_page->key[i];
				aux_rrn[i] = left_page->rrn[i];
				aux_child[i] = left_page->child[i];
			}
			//Fill the last array elements of each array aux
			//Except of aux_child, because the last 2 array elems are not filled after the for loop
			aux_key[i] = -1;
			aux_rrn[i] = -1;
			aux_child[i] = left_page->child[i];
			aux_child[i + 1] = -1;

			for(i = ORDER; i > 0 && left_page->key[i - 1] > key; i--)
			{
				aux_key[i] = left_page->key[i];
				aux_rrn[i] = left_page->rrn[i];
				aux_child[i + 1] = left_page->child[i];
			}
			aux_key[i] = key;
			aux_rrn[i] = register_rrn;
			aux_child[i + 1] = -1;

			left_page->n = ORDER / 2;
			//Update the left page
			for(i = 0; i < ORDER - 1; i++)
			{
				if(i < left_page->n)
				{
					left_page->key[i] = aux_key[i];
					left_page->rrn[i] = aux_rrn[i];
					left_page->child[i] = aux_child[i];
				}
				else
				{
					left_page->key[i] = -1;
					left_page->rrn[i] = -1;
					left_page->child[i] = -1;
				}
			}
			left_page->child[i] = -1;

			//Update the right page
			right_page->n = 2;
			right_page->nivel = 1;
			for(i = 0; i < ORDER - 1; i++)
			{
				if(i < right_page->n)
				{
					right_page->key[i] = aux_key[i + left_page->n + 1];
					right_page->rrn[i] = aux_rrn[i + left_page->n + 1];
					right_page->child[i] = aux_child[i + left_page->n + 1];
				}
				else
				{
					right_page->key[i] = -1;
					right_page->rrn[i] = -1;
					right_page->child[i] = -1;
				}
			}
			right_page->child[i] = -1;
			//TODO
			//needs to look the header rrn and some other things
}

BTREE_PAGE *btree_index_insert(BTREE_HEADER *header, BTREE_PAGE *page, int key, FILE *b_file, int cur_rrn, int register_rrn)
{
	if(header->noRaiz == -1)
	{
		page = btree_page_create();
		page->key[0] = key;
		page->nivel = 1;
		page->n = 1;
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

		//If the page is a leaf and there is space
		if(page->nivel == 1 && page->n < ORDER - 1)
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
			btree_page_write(b_file, page, cur_rrn);
			return NULL;
		}
		//If is a leaf, but there is no space
		else if(page->nivel == 1)
		{
			BTREE_PAGE *right_page = btree_page_create();
			btree_split(page, right_page, key, register_rrn);
		}
	}
}

int btree_index_create(char *bin_filename, char *b_filename)
{
	FILE *bin_file = fopen("bin_filename", "rb");
	if(!bin_file) return FILE_BROKEN;
	FILE *b_file = fopen("b_filename", "wb");
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

	btree_header->status = '0';
	btree_header_write(b_file, btree_header);
	register_rrn = 1;
	while(bin_header->numeroRegistrosInseridos--)
	{
		reg = register_read(bin_file);
		if(reg) btree_index_insert(btree_header, btree_page, reg->idNascimento, b_file, btree_header->noRaiz, register_rrn);
		register_rrn++;
	}

	return SUCCESS;
}
