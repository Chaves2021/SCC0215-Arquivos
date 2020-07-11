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
	char lixo[56]
};

btree_page
{
	int nivel;
	int n;
	int key[ORDER - 1];
	int child[ORDER];
	int rrn[ORDER - 1];
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
	BTREE_PAGE b_page;
	b_page = (BTREE_PAGE *) calloc(1, sizeof(BTREE_PAGE));

	return b_page;
}

//TODO
//remember you need to write in a file in every insert
//first thing to do when return ;)
int btree_index_create(BTREE_HEADER *btree_header, BTREE_PAGE **btree_page, FILE *bin_file)
{
	HEADER *bin_header = HEADER_READ(bin_file);
	REGISTER *reg;
	while(bin_header->numeroRegistrosInseridos--)
	{
		reg = register_read(bin_file);
		btree_index_insert(btree_header, btree_page, reg);
	}
}
