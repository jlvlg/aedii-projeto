#ifndef TABLES_H
#define TABLES_H

#include <stdio.h>
#include "index.h"
#include "data.h"

typedef struct Table {
    FILE* file;
    int count;
    AVL ssn_index;
    BST email_index;
    RB phone_index;
    Garbage garbage_index;
    char *file_path, *ssn_path, *email_path, *phone_path, *garbage_path;
} Table;

struct table_methods {
    int (*initialize_table)(Table *t, char* data_file, char* ssn_index, char* email_index, char* phone_index, char* garbage_index);
    int (*close_table)(Table *t);
    int (*add_employee)(Table *t, Employee employee);
    int (*find_employee)(Table t, Employee* employee, char *key, Tree index);
    Employee* (*list_employees)(Table t, int reversed, int *out_count, Tree index);
    int (*delete_employee)(Table *t, Employee *employee);
    int (*delete_employee_by_index)(Table *t, char *key, Tree index);
};

extern const struct table_methods table;

#endif