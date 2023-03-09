#include <stdio.h>
#include "table.h"
#include "data.h"
#include "types.h"
#include "bst.h"
#include "avl.h"
#include "rb.h"
#include "json_object.h"
#include "string.h"

int initialize_table(Table table, char* data_file, char* authors_index, char* isbn_index, char* title_index, char* junk_index) {
    table->isbn_index = idx.retrieve_avl(isbn_index);
    table->authors_index = idx.retrieve_rb(authors_index);
    table->title_index = idx.retrieve_bst(title_index);
    table->junk_index = idx.retrieve_junk(junk_index);
    table->file = fopen(data_file, "a+");
    return table->file != NULL;
}

void close_table(Table table, char* authors_index, char* isbn_index, char* title_index, char* junk_index) {
    fclose(table->file);
    idx.save_avl(table->isbn_index, isbn_index);
    idx.save_rb(table->authors_index, authors_index);
    idx.save_bst(table->title_index, title_index);
    idx.save_junk(table->junk_index, junk_index);
}

json_object* book_to_json(Book book) {
    json_object* json = json_object_new_object();
    json_object_object_add(json, "isbn", json_object_new_int(book.isbn));
    json_object_object_add(json, "num_pages", json_object_new_int(book.num_pages));
    json_object_object_add(json, "title", json_object_new_string(book.title));
    json_object_object_add(json, "authors", json_object_new_string(book.authors));
    json_object_object_add(json, "publisher", json_object_new_string(book.publisher));
    return json;
}

void add_book(Table table, Book book) {
    int pos, changes;
    if (table->file != NULL) {
        json_object* json_obj = book_to_json(book);
        char* json = json_object_to_json_string(json_obj);
        int free_section = idx.recycle(&table->junk_index, sizeof(char) * strlen(json));
        fseek(table->file, 0L, free_section >= 0 ? free_section : SEEK_END);
        pos = ftell(table->file);

        table->title_index = bst.insert(table->title_index, idx.create_bst(types.String(book.title), pos));
        table->isbn_index = avl.insert(table->isbn_index, idx.create_avl(types.Int(book.isbn), pos), &changes);
        rb.insert(&table->authors_index, idx.create_rb(types.String(book.authors), pos));

        fwrite(json, sizeof(char), strlen(json), table->file);
        json_object_put(json_obj);
    }
}