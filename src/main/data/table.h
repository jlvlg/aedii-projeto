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
    Junk junk_index;
    char *file_path, *ssn_path, *email_path, *phone_path, *junk_path;
} Table;

struct table_methods {
    int (*initialize_table)(Table *t, char* data_file, char* ssn_index, char* email_index, char* phone_index, char* junk_index);
    int (*close_table)(Table *t);
    int (*add_employee)(Table *t, Employee employee);
    int (*find_employee_by_ssn)(Table t, Employee* employee, char *ssn);
    int (*find_employee_by_phone)(Table t, Employee* employee, char *phone);
    int (*find_employee_by_email)(Table t, Employee* employee, char *email);
    Employee* (*list_employees_by_ssn)(Table t, int reversed, int *out_count);
    Employee* (*list_employees_by_phone)(Table t, int reversed, int *out_count);
    Employee* (*list_employees_by_email)(Table t, int reversed, int *out_count);
    int (*delete_employee)(Table *t, Employee *employee);
    int (*delete_employee_by_ssn)(Table *t, char *ssn);
    int (*delete_employee_by_email)(Table *t, char *email);
    int (*delete_employee_by_phone)(Table *t, char *phone);
};

extern const struct table_methods table;

#endif