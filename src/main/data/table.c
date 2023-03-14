#include <stdio.h>
#include <string.h>
#include "table.h"
#include "data.h"
#include "types.h"
#include "util.h"
#include "bst.h"
#include "avl.h"
#include "rb.h"
#include "json_object.h"
#include "json_tokener.h"

static int initialize_table(Table *t, char* data_file, char* ssn_index, char* email_index, char* phone_index, char* junk_index) {
    t->ssn_index = idx.retrieve_avl(t->ssn_path = util.init_string(ssn_index));
    t->email_index = idx.retrieve_bst(t->email_path = util.init_string(email_index));
    t->phone_index = idx.retrieve_rb(t->phone_path = util.init_string(phone_index));
    t->junk_index = idx.retrieve_junk(t->junk_path = util.init_string(junk_index));
    t->count = tree.count(t->ssn_index);
    t->file = fopen(t->file_path = util.init_string(data_file), "a");
    fclose(t->file);
    t->file = fopen(t->file_path, "r+");
    return t->file == NULL;
}

static int close_table(Table *t) {
    int res = 0;
    res = fclose(t->file);

    res = idx.save_tree(t->ssn_index, t->ssn_path);
    res = idx.save_tree(t->email_index, t->email_path);
    res = idx.save_tree(t->phone_index, t->phone_path);
    res = idx.save_junk(t->junk_index, t->junk_path);

    tree.clear(t->ssn_index);
    tree.clear(t->phone_index);
    tree.clear(t->email_index);

    free(t->ssn_path);
    free(t->email_path);
    free(t->phone_path);
    free(t->junk_path);

    return res;
}

static int error_handler(Table *t, Item *ssn, Item *email, Item *phone, json_object *json, int *size, int *free_section) {
    int changes, error = -1;
    if (ssn != NULL) {
        t->ssn_index = avl.remove(t->ssn_index, *ssn, &changes, idx.copy);
        error--;
    }
    if (email != NULL) {
        t->email_index = bst.remove(t->email_index, *email, idx.copy);
        error--;
    }
    if (phone != NULL)
        rb.remove(&t->phone_index, *phone, idx.copy);
    if (free_section != NULL && *free_section >= 0 && size != NULL)
        idx.restore_junk(&t->junk_index, *size, *free_section);
    json_object_put(json);
    return -1;
}

static int add_employee(Table *t, Employee employee) {
    json_object* json_obj;
    int pos, changes, error = 0;
    if (t->file != NULL) {
        json_obj = data.employee_to_json(employee);
        char* json = json_object_to_json_string(json_obj);
        int free_section = idx.recycle(&t->junk_index, sizeof(char) * (strlen(json) + 1));
        int size = sizeof(char) * (strlen(json) + 1);
        Item ssn = types.String(employee.ssn);
        Item email = types.String(employee.email);
        Item phone = types.String(employee.phone);

        if (free_section >= 0)
            fseek(t->file, free_section, SEEK_SET);
        else 
            fseek(t->file, 0L, SEEK_END);
        pos = ftell(t->file);

        t->ssn_index = avl.insert(t->ssn_index, avl.create(idx.index(ssn, pos)), &changes, &error, idx.copy);
        if (error)
            return error_handler(t, NULL, NULL, NULL, json_obj, &size, &free_section);

        t->email_index = bst.insert(t->email_index, bst.create(idx.index(email, pos)), &error, idx.copy);
        if (error) 
            return error_handler(t, &ssn, NULL, NULL, json_obj, &size, &free_section);

        rb.insert(&t->phone_index, rb.create(idx.index(phone, pos)), &error, idx.copy);
        if (error) 
            return error_handler(t, &ssn, &email, NULL, json_obj, &size, &free_section);

        if ((fwrite(json, sizeof(char), strlen(json), t->file)) && (fwrite("\n", sizeof(char), 1, t->file))) {
            t->count++;
        } else {
            return error_handler(t, &ssn, &email, &phone, json_obj, &size, &free_section);
        }

        json_object_put(json_obj);
        return error;
    }
    return -1;
}

static int retrieve_employee(Table t, Employee *employee, int pos) {
    if (t.file != NULL) {
        json_object *json;
        int success = 0;
        int buffsize = 1000;
        char *buffer = util.safe_malloc(sizeof(char) * buffsize);

        while (!success) {
            fseek(t.file, pos, SEEK_SET);
            fgets(buffer, 1000, t.file);
            
            success = strchr(buffer, '\n') != NULL;
            if (!success) {
                buffer = util.safe_realloc(buffer, sizeof(char) * (buffsize *= 2));
            }
        };

        json = json_tokener_parse(buffer);
        if (json == NULL) {
            free(buffer);
            return -1;
        }

        *employee = data.json_to_employee(json);

        free(buffer);
        json_object_put(json);
        return 0;
    }
    return -1;
}

static int find_employee_by_ssn(Table t, Employee *employee, char *ssn) {
    Item key = types.String(ssn);
    Tree index = tree.search(t.ssn_index, key);

    if (index == NULL) {
        types.destroy(key);
        return -1;
    }

    retrieve_employee(t, employee, ((Index) index->item)->pos);

    types.destroy(key);

    return 0;
}

static int find_employee_by_email(Table t, Employee *employee, char *email) {
    Item key = types.String(email);
    Tree index = tree.search(t.email_index, key);

    if (index == NULL) {
        types.destroy(key);
        return -1;
    }

    retrieve_employee(t, employee, ((Index) index->item)->pos);

    types.destroy(key);

    return 0;
}

static int find_employee_by_phone(Table t, Employee *employee, char *phone) {
    Item key = types.String(phone);
    Tree index = tree.search(t.phone_index, key);

    if (index == NULL) {
        types.destroy(key);
        return -1;
    }

    retrieve_employee(t, employee, ((Index) index->item)->pos);

    types.destroy(key);

    return 0;
}

static Employee* list_employees_by_ssn(Table t, int reversed, int *out_count) {
    Employee* employees = util.safe_malloc(sizeof(Employee) * t.count);
    Tree* indexes = tree.to_array(t.ssn_index, NULL, reversed, &t.count);

    if (indexes == NULL) {
        free(indexes);
        return NULL;
    }

    for (int i = 0; i < t.count; i++)
        retrieve_employee(t, &employees[i], ((Index) indexes[i]->item)->pos);

    free(indexes);

    *out_count = t.count;
    return employees;
}

static Employee* list_employees_by_email(Table t, int reversed, int *out_count) {
    Employee* employees = util.safe_malloc(sizeof(Employee) * t.count);
    Tree* indexes = tree.to_array(t.email_index, NULL, reversed, &t.count);

    if (indexes == NULL) {
        free(indexes);
        return NULL;
    }

    for (int i = 0; i < t.count; i++)
        retrieve_employee(t, &employees[i], ((Index) indexes[i]->item)->pos);

    free(indexes);

    *out_count = t.count;
    return employees;
}

static Employee* list_employees_by_phone(Table t, int reversed, int *out_count) {
    Employee* employees = util.safe_malloc(sizeof(Employee) * t.count);
    Tree* indexes = tree.to_array(t.phone_index, NULL, reversed, &t.count);

    if (indexes == NULL) {
        free(indexes);
        return NULL;
    }

    for (int i = 0; i < t.count; i++)
        retrieve_employee(t, &employees[i], ((Index) indexes[i]->item)->pos);

    free(indexes);

    *out_count = t.count;
    return employees;
}

static int delete_employee(Table *t, Employee *employee) {
    json_object *json_obj;
    char* json;
    Item email, phone, ssn = types.String(employee->ssn);
    Tree index = tree.search(t->ssn_index, ssn);
    int changes;
    
    if (index == NULL) {
        types.destroy(ssn);
        return -1;
    }

    email = types.String(employee->email);
    phone = types.String(employee->phone);

    json_obj = data.employee_to_json(*employee);
    json = json_object_to_json_string(json_obj);

    t->junk_index = idx.dump(t->junk_index, sizeof(char) * (strlen(json) + 1), ((Index) index->item)->pos);
    t->ssn_index = avl.remove(t->ssn_index, ssn, &changes, idx.copy);
    t->email_index = bst.remove(t->email_index, email, idx.copy);
    rb.remove(&t->phone_index, phone, idx.copy);

    data.destroy(employee);
    types.destroy(ssn);
    types.destroy(email);
    types.destroy(phone);
    json_object_put(json_obj);
    t->count--;
    return 0;
}

static int delete_employee_by_ssn(Table *t, char *ssn) {
    Employee employee;

    if (find_employee_by_ssn(*t, &employee, ssn))
        return -1;

    delete_employee(t, &employee);

    return 0;
}

static int delete_employee_by_email(Table *t, char *email) {
    Employee employee;

    if (find_employee_by_email(*t, &employee, email))
        return -1;

    delete_employee(t, &employee);

    return 0;
}

static int delete_employee_by_phone(Table *t, char *phone) {
    Employee employee;

    if (find_employee_by_phone(*t, &employee, phone))
        return -1;

    delete_employee(t, &employee);

    return 0;
}

const struct table_methods table = {
    .initialize_table = initialize_table,
    .close_table = close_table,
    .add_employee = add_employee,
    .find_employee_by_ssn = find_employee_by_ssn,
    .find_employee_by_email = find_employee_by_email,
    .find_employee_by_phone = find_employee_by_phone,
    .list_employees_by_ssn = list_employees_by_ssn,
    .list_employees_by_email = list_employees_by_email,
    .list_employees_by_phone = list_employees_by_phone,
    .delete_employee = delete_employee,
    .delete_employee_by_ssn = delete_employee_by_ssn,
    .delete_employee_by_email = delete_employee_by_email,
    .delete_employee_by_phone = delete_employee_by_phone
};