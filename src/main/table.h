#ifndef TABLES_H
#define TABLES_H

#include <stdio.h>
#include "index.h"

typedef struct {
    FILE* file;
    BST_Index isbn_index;
    AVL_Index bookID_index;
    RB_Index title_index;
} Table;

#endif