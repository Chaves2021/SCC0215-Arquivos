#ifndef _BTREE_
#define _BTREE_

typedef struct btree_header BTREE_HEADER;
typedef struct btree_page BTREE_PAGE;

BTREE_HEADER *btree_header_create();
int btree_index_create(BTREE_HEADER *, BTREE_PAGE **, FILE *);

#endif
