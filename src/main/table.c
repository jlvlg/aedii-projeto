#include <stdio.h>
#include "table.h"

int initializeTable(Table *table, char* data_file, char* bookId_index, char* isbn_index, char* title_index) {
    table->bookID_index = NULL;
    table->isbn_index = NULL;
    table->title_index = NULL;
    table->file = fopen(data_file, "a+b");
    return table->file != NULL;
}

void closeTable(Table *table) {
    fclose(table->file);
}