#ifndef TABLES_H
#define TABLES_H

#include <stdio.h>
#include "index.h"
#include "data.h"
#include "json_object.h"

struct Table {
    FILE* file;
    AVL_Index isbn_index;
    BST_Index title_index;
    RB_Index authors_index;
    Junk junk_index;
};

typedef struct Table *Table;

int initialize_table(Table table, char* data_file, char* authors_index, char* isbn_index, char* title_index, char* junk_index);
void close_table(Table table, char* authors_index, char* isbn_index, char* title_index, char* junk_index);
json_object* book_to_json(Book book);
void add_book(Table table, Book book);

#endif