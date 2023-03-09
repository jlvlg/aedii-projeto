#include "table.h"

int main(void) {
    struct Table table;
    Book book1 = {
        .authors = "UUAAAA",
        .isbn = 12421214,
        .num_pages = 232,
        .publisher = "AOWA",
        .title = "adasda"
    };
    Book book2 = {
        .authors = "aaa",
        .isbn = 12,
        .num_pages = 12312,
        .publisher = "ZZZZ",
        .title = "B"
    };
    Book book3 = {
        .authors = "ZZZZ",
        .isbn = 15,
        .num_pages = 1,
        .publisher = "a",
        .title = "a"
    };
    initialize_table(&table, "data.dat", "authors.json", "isbn.json", "title.json", "junk.json");
    add_book(&table, book1);
    add_book(&table, book2);
    add_book(&table, book3);
    close_table(&table, "authors.json", "isbn.json", "title.json", "junk.json");

    return 0;
}