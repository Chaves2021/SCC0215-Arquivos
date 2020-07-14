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
		fread(&page->key[i], sizeof(int), 1, b_file);
		fread(&page->rrn[i], sizeof(int), 1, b_file);
	}
	for(i = 0; i < ORDER; i++) fread(&page->child[i], sizeof(int), 1, b_file);

	return page;
}

BTREE_PAGE *btree_index_insert(BTREE_HEADER *header, BTREE_PAGE **page, int key, FILE *b_file, int cur_rrn)
{
	if(header->noRaiz == -1)
	{
		*page = btree_page_create();
		*page->key[0] = key;
		*page->nivel = 1;
		*page->n = 1;
		btree_page_write(b_file, *page, cur_rrn);

		header->noRaiz = 0;
		header->nroNiveis++;
		header->nroChaves++;
		header->proxRRN = 1;
		btree_header_write(b_file, header);

		return NULL;
	}
	else
	{

		*page = btree_page_read(b_file, cur_rrn);

		//If the page is a leaf and there is space
		if(*page->nivel == 1 && *page->n < ORDER - 1)
		{
			int i;
			for(i = page->n; i > 0 && (*page)->key[i - 1] > key; i--)
			{
				*page->key[index] = *page->key[index - 1];
				*page->rrn[index] = *page->rrn[index - 1];
			}
			*page->key[index] = key;
			*page->rrn[index] = /*sla*/ ;
			*page->n++;
			btree_page_write(b_file, *page, cur_rrn);
			return NULL;
		}
		//TODO
		//If is a leaf, but there is no space
		else if(*page->nivel == 1)
		{
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

	btree_header->status = '0';
	btree_header_write(b_file, btree_header);
	while(bin_header->numeroRegistrosInseridos--)
	{
		reg = register_read(bin_file);
		if(reg) btree_index_insert(btree_header, &btree_page, reg->idNascimento, b_file, btree_header->noRaiz);
	}

	return SUCCESS;
}
