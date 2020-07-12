#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <btree.h>

btree_header
{
	char status;
	int noRaiz;
	int nroNiveis;
	int proxRRN;
	int nroChaves;
};

btree_page
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
	b_header->status = "1";
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

btree_header_write(FILE *b_file, BTREE_HEADER *header)
{
	fseek(b_file, 0, SEEK_SET);
	int len = 55;
	fwrite(header->status, sizeof(char), 1, b_file);
	fwrite(header->noRaiz, sizeof(int), 1, b_file);
	fwrite(header->nroNiveis, sizeof(int), 1, b_file);
	fwrite(header->proxRRN, sizeof(int), 1, b_file);
	fwrite(header->nroChaves, sizeof(int), 1, b_file);
	while(len--) fwrite("$", sizeof(char), 1, b_file);

	return SUCCESS;
}

int btree_page_write(FILE *b_file, BTREE_PAGE *page)
{
	int i;
	fwrite(page->nivel, sizeof(int), 1, b_file);
	fwrite(page->n, sizeof(int), 1, b_file);
	for(i = 0; i < ORDER - 1; i++)
	{
		fwrite(page->key[i], sizeof(int), 1, b_file);
		fwrite(page->rrn[i], sizeof(int), 1, b_file);
	}
	for(i = 0; i < ORDER; i++) fwrite(page->child[i], sizeof(int), 1, b_file);

	return SUCCESS;
}

int btree_index_insert(BTREE_HEADER *header, BTREE_PAGE ***page, int key, FILE *b_file, int cur_rrn)
{
	if(header->noRaiz == -1)
	{
		(*page)[0] = btree_page_create();
		(*page)[0]->key[0] = key;
		(*page)[0]->nivel = 1;
		(*page)[0]->n = 1;
		fseek(b_file, ORDER, SEEK_SET);
		btree_page_write(b_file, btree_page[0]);

		header->noRaiz = 0;
		header->nroNiveis++;
		header->nroChaves++;
		header->proxRRN = 1;
		btree_header_write(b_file, header);
	}
	else
	{
		//TODO
		//if page is a leaf and there is space
		if((*page)[cur_rrn]->nivel == 1 && (*page)[cur_rrn]->n < ORDER - 1)
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
	BTREE_PAGE **btree_page = (BTREE_PAGE **) realloc(btree_page, 1 * sizeof(BTREE_PAGE *));
	HEADER *bin_header = HEADER_READ(bin_file);
	REGISTER *reg;

	btree_header->status = "0";
	btree_header_write(b_file, header);
	while(bin_header->numeroRegistrosInseridos--)
	{
		reg = register_read(bin_file);
		if(reg) btree_index_insert(btree_header, &btree_page, reg->idNascimento, b_file, header->noRaiz);
	}

	return SUCCESS;
}
