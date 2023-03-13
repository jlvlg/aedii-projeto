#ifndef DATA_H
#define DATA_H

#include "json_object.h"

typedef struct {
    int id;
    char* ssn;
    char* name;
    char* email;
    char* phone;
} Employee;

struct data_methods {
    Employee (*create)(int id, char *ssn, char *name, char *email, char* phone);
    void (*destroy)(Employee *employee);
    void* (*clear_array)(Employee *array, int count);
    json_object* (*employee_to_json)(Employee employee);
    Employee (*json_to_employee)(json_object *json);
};

extern const struct data_methods data;

#endif