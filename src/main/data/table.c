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

static int initialize_table(Table *t, char* data_file, char* ssn_index, char* email_index, char* phone_index, char* garbage_index) {
    t->ssn_index = idx.retrieve_avl(t->ssn_path = util.init_string(ssn_index));
    t->email_index = idx.retrieve_bst(t->email_path = util.init_string(email_index));
    t->phone_index = idx.retrieve_rb(t->phone_path = util.init_string(phone_index));
    t->garbage_index = idx.retrieve_garbage(t->garbage_path = util.init_string(garbage_index));
    t->count = tree.count((Tree) t->ssn_index);
    t->file = fopen(t->file_path = util.init_string(data_file), "a");
    fclose(t->file);
    t->file = fopen(t->file_path, "r+");
    return t->file == NULL;
}

static int close_table(Table *t) {
    int res = 0;
    res = fclose(t->file);

    res = idx.save_tree((Tree) t->ssn_index, t->ssn_path);
    res = idx.save_tree(t->email_index, t->email_path);
    res = idx.save_tree((Tree) t->phone_index, t->phone_path);
    res = idx.save_garbage(t->garbage_index, t->garbage_path);

    tree.clear((Tree) t->ssn_index);
    tree.clear((Tree) t->phone_index);
    tree.clear(t->email_index);

    free(t->ssn_path);
    free(t->email_path);
    free(t->phone_path);
    free(t->garbage_path);

    return res;
}

static int error_handler(Table *t, Index *ssn, Index *email, Index *phone, json_object *json, int *size, int *free_section) {
    int changes, error = -1;
    if (ssn != NULL) {
        t->ssn_index = avl.remove(t->ssn_index, (Item) *ssn, &changes, (Item (*)(Item)) idx.copy);
        error--;
    }
    if (email != NULL) {
        t->email_index = bst.remove(t->email_index, (Item) *email, (Item (*)(Item)) idx.copy);
        error--;
    }
    if (phone != NULL) {
        rb.remove(&t->phone_index, (Item) *phone, (Item (*)(Item)) idx.copy);
        error--;
    }
    if (free_section != NULL && *free_section >= 0 && size != NULL)
        idx.dump(&t->garbage_index, *size, *free_section);
    json_object_put(json);
    return error;
}

static int add_employee(Table *t, Employee employee) {
    json_object* json_obj;
    int pos, changes, error = 0;
    if (t->file != NULL) {
        json_obj = data.employee_to_json(employee);
        char* json = (char*) json_object_to_json_string(json_obj);
        int free_section = idx.recycle(&t->garbage_index, sizeof(char) * (strlen(json) + 1));
        int size = sizeof(char) * (strlen(json) + 1);
        Item ssn = types.String(employee.ssn);
        Item email = types.String(employee.email);
        Item phone = types.String(employee.phone);
        Index ssn_i, email_i, phone_i;

        if (free_section >= 0)
            fseek(t->file, free_section, SEEK_SET);
        else 
            fseek(t->file, 0L, SEEK_END);
        pos = ftell(t->file);

        ssn_i = idx.index(&ssn, pos);
        email_i = idx.index(&email, pos);
        phone_i = idx.index(&phone, pos);

        t->ssn_index = avl.insert(t->ssn_index, avl.create((Item) ssn_i), &changes, &error);
        if (error)
            return error_handler(t, NULL, NULL, NULL, json_obj, &size, &free_section);

        t->email_index = bst.insert(t->email_index, bst.create((Item) email_i), &error);
        if (error) 
            return error_handler(t, &ssn_i, NULL, NULL, json_obj, &size, &free_section);

        rb.insert(&t->phone_index, rb.create((Item) phone_i), &error);
        if (error) 
            return error_handler(t, &ssn_i, &email_i, NULL, json_obj, &size, &free_section);

        if ((fwrite(json, sizeof(char), strlen(json), t->file)) && (fwrite("\n", sizeof(char), 1, t->file))) {
            t->count++;
        } else {
            return error_handler(t, &ssn_i, &email_i, &phone_i, json_obj, &size, &free_section);
        }
        
        json_object_put(json_obj);
        return 0;
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
            fgets(buffer, buffsize, t.file);
            
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

static int find_employee(Table t, Employee *employee, char *key, Tree index) {
    Item itemkey = types.String(key);
    Tree found = tree.search(index, itemkey);

    if (found == NULL) {
        types.destroy(itemkey);
        return -1;
    }

    retrieve_employee(t, employee, ((Index) found->item)->pos);

    types.destroy(itemkey);

    return 0;
}

static Employee* list_employees(Table t, int reversed, int *out_count, Tree index) {
    Employee *employees = util.safe_malloc(sizeof(Employee) * t.count);
    Tree *indexes = tree.to_array(index, NULL, reversed, &t.count);

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
    Tree index = tree.search((Tree) t->ssn_index, ssn);
    int changes;
    
    if (index == NULL) {
        types.destroy(ssn);
        return -1;
    }

    email = types.String(employee->email);
    phone = types.String(employee->phone);

    json_obj = data.employee_to_json(*employee);
    json = (char*) json_object_to_json_string(json_obj);

    idx.dump(&t->garbage_index, sizeof(char) * (strlen(json) + 1), ((Index) index->item)->pos);
    t->ssn_index = avl.remove(t->ssn_index, ssn, &changes, (Item (*)(Item)) idx.copy);
    t->email_index = bst.remove(t->email_index, email, (Item (*)(Item)) idx.copy);
    rb.remove(&t->phone_index, phone, (Item (*)(Item)) idx.copy);

    data.destroy(employee);
    types.destroy(ssn);
    types.destroy(email);
    types.destroy(phone);
    json_object_put(json_obj);
    t->count--;
    return 0;
}

static int delete_employee_by_index(Table *t, char *key, Tree index) {
    Employee employee;

    if (find_employee(*t, &employee, key, index))
        return -1;

    delete_employee(t, &employee);

    return 0;
}

const struct table_methods table = {
    .initialize_table = initialize_table,
    .close_table = close_table,
    .add_employee = add_employee,
    .find_employee = find_employee,
    .list_employees = list_employees,
    .delete_employee = delete_employee
};