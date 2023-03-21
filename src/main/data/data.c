#include <string.h>
#include "data.h"
#include "util.h"
#include "json_object.h"

static Employee create(int id, char *ssn, char *name, char *email, char *phone) {
    Employee employee;
    employee.id = id;
    employee.ssn = util.init_string(ssn);
    employee.name = util.init_string(name);
    employee.email = util.init_string(email);
    employee.phone = util.init_string(phone);
    return employee;
}

static void destroy(Employee *employee) {
    if (employee != NULL) {
        free(employee->ssn);
        free(employee->name);
        free(employee->email);
        free(employee->phone);
        employee->ssn = employee->name = employee->email = employee->phone = NULL;
        employee->id = 0;
    }
}

static void* clear_array(Employee *array, int count) {
    for (int i = 0; i < count; i++) {
        destroy(&array[i]);
    }
    free(array);
    return NULL;
}

static json_object* employee_to_json(Employee employee) {
    json_object* json = json_object_new_object();
    json_object_object_add(json, "id", json_object_new_int(employee.id));
    json_object_object_add(json, "ssn", json_object_new_string(employee.ssn));
    json_object_object_add(json, "name", json_object_new_string(employee.name));
    json_object_object_add(json, "email", json_object_new_string(employee.email));
    json_object_object_add(json, "phone", json_object_new_string(employee.phone));
    return json;
}

static Employee json_to_employee(json_object* json) {
    json_object *id, *ssn, *name, *email, *phone;
    
    json_object_object_get_ex(json, "id", &id);
    json_object_object_get_ex(json, "ssn", &ssn);
    json_object_object_get_ex(json, "name", &name);
    json_object_object_get_ex(json, "email", &email);
    json_object_object_get_ex(json, "phone", &phone);

    return create(
        json_object_get_int(id),
        (char*) json_object_get_string(ssn), 
        (char*) json_object_get_string(name), 
        (char*) json_object_get_string(email), 
        (char*) json_object_get_string(phone)
    );
}


const struct data_methods data = {
    .create = create,
    .destroy = destroy,
    .clear_array = clear_array,
    .employee_to_json = employee_to_json,
    .json_to_employee = json_to_employee
};