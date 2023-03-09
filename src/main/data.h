#ifndef DATA_H
#define DATA_H

typedef struct {
    int isbn;
    int num_pages;
    char* title;
    char* authors;
    char* publisher;
} Book;

#endif