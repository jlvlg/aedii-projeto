#include <ctype.h>
#include <eti.h>
#include <stdlib.h>
#include <curses.h>
#include <panel.h>
#include <form.h>
#include <regex.h>
#include <menu.h>
#include <string.h>
#include "data.h"
#include "table.h"
#include "util.h"

void debug(Table *t) {
    table.add_employee(t, (Employee) {
        1,
        "000000000",
        "Jorge",
        "jorge@email.com",
        "0000000000"
    });
    table.add_employee(t, (Employee) {
        2,
        "000000002",
        "Mateus",
        "mateus@email.com",
        "0000000002"
    });
    table.add_employee(t, (Employee) {
        3,
        "000000001",
        "Joelma",
        "joelma@email.com",
        "0000000001"
    });
}

PANEL *border_panel(WINDOW **window, int height, int width, int y, int x) {
    *window = newwin(height, width, y, x);
    PANEL *panel = new_panel(*window);
    box(*window, 0, 0);
    return panel;
}

MENU *menu_panel(WINDOW **window, PANEL **panel, int height, int width, int y, int x, ITEM **items, void **ptrs) {
    *window = newwin(height, width, y, x);
    *panel = new_panel(*window);
    MENU *menu = new_menu(items);
    for (int i = 0; items[i] != NULL; i++)
        set_item_userptr(items[i], ptrs[i]);
    set_menu_win(menu, *window);
    menu_opts_off(menu, O_SHOWDESC);
    set_menu_mark(menu, NULL);
    return menu;
}

FORM *form_panel(WINDOW **window, PANEL **panel, int height, int width, int y, int x, FIELD **fields) {
    *window = newwin(height, width, y, x);
    *panel = new_panel(*window);
    FORM *form = new_form(fields);
    for (int i = 0; fields[i] != NULL; i++)
        field_opts_off(fields[i], O_AUTOSKIP | O_STATIC);
    set_form_win(form, *window);
    return form;
}

void show_employee(PANEL *panel, Employee *employee) {
    WINDOW *win = panel_window(panel);
    char id[snprintf(NULL, 0, "%d", employee->id)];
    sprintf(id, "%d", employee->id);
    show_panel(panel);
    top_panel(panel);
    for (int i = 2; i <= 10; i += 2) {
        wmove(win, i, 1);
        wclrtoeol(win);
    }
    mvwvline(win, 1, 63, ACS_VLINE, 15);
    mvwprintw(win, 2, 2, "ID:     %.51s", id);
    mvwprintw(win, 4, 2, "SSN:    %.51s", employee->ssn);
    mvwprintw(win, 6, 2, "Name:   %.51s", employee->name);
    mvwprintw(win, 8, 2, "E-mail: %.51s", employee->email);
    mvwprintw(win, 10, 2, "Phone:  %.51s", employee->phone);
}

void list_employees(Table t, MENU *filters, MENU *list, ITEM ***employee_items, Employee **employees, char ***briefs, int *n_employees) {
    ITEM **items = menu_items(filters), **old_items = *employee_items, *cur;
    menu_driver(filters, KEY_MOUSE);
    unpost_menu(list);
    if (*employees != NULL)
        *employees = data.clear_array(*employees, *n_employees);
    cur = current_item(filters);
    if (cur == items[0]) {
        *employees = table.list_employees_by_ssn(t, 0, n_employees);
    } else if (cur == items[1]) {
        *employees = table.list_employees_by_email(t, 0, n_employees);
    } else if (cur == items[2]) {
        *employees = table.list_employees_by_phone(t, 0, n_employees);
    }
    *employee_items = menu_items(list);
    for (int i = 0; *briefs != NULL && (*briefs)[i] != NULL; i++)
        free((*briefs)[i]);
    if (*briefs != NULL)
        free(*briefs);
    for (int i = 0; *employee_items != NULL && (*employee_items)[i] != NULL; i++)
        free_item((*employee_items)[i]);
    *employee_items = malloc(sizeof(ITEM*) * (*n_employees + 1));
    *briefs = malloc(sizeof(char*) * (*n_employees + 1));
    for (int i = 0; i < *n_employees; i++) {
        (*briefs)[i] = malloc(sizeof(char) * 68);
        char id[snprintf(NULL, 0, "%d", (*employees)[i].id)];
        sprintf(id, "%d", (*employees)[i].id);
        sprintf((*briefs)[i], "%-4.4s   %-9.9s   %-12.12s   %-20.20s   %-10.10s", id, (*employees)[i].ssn, (*employees)[i].name, (*employees)[i].email, (*employees)[i].phone);
        (*employee_items)[i] = new_item((*briefs)[i], "");
        set_item_userptr((*employee_items)[i], &(*employees)[i]);
    }
    (*briefs)[*n_employees] = NULL;
    (*employee_items)[*n_employees] = NULL;
    set_menu_items(list, (*employee_items)[0] == NULL ? NULL : *employee_items);
    post_menu(list);
    free(old_items);
}

void clear_form(FORM *form) {
    FIELD **fields = form_fields(form);
    for (int i = 0; fields != NULL && fields[i] != NULL; i++) {
        set_current_field(form, fields[i]);
        form_driver(form, REQ_CLR_FIELD);
    }
    if (fields != NULL)
        set_current_field(form, fields[0]);
}

void print_centered(WINDOW *win, int y, char* format, char* text) {
    int length = snprintf(NULL, 0, format, text);
    mvwprintw(win, y, (getmaxx(win) - length) / 2, format, text);
}

void show_warning(PANEL **panels, PANEL *panel, char* format, char* message) {
    WINDOW *win = panel_window(panel);
    curs_set(0);
    top_panel(panel);
    wmove(win, 2, 1);
    wclrtoeol(win);
    print_centered(win, 2, format, message);
    mvwaddch(win, 2, 42, ACS_VLINE);
    update_panels();
    doupdate();
    while (getch() == KEY_MOUSE);
    if (panel_below(panel) == panels[4])
        curs_set(1);
    top_panel(panel_below(panel));
}

char* get_string(FIELD *field) {
    int i;
    char *buffer = util.init_string(field_buffer(field, 0));
    for (i = strlen(buffer); i > 0 && isspace(buffer[i-1]); i--);
    buffer[i] = '\0';
    return buffer;
}

int main() {
    int ch, n_windows, n_panels, n_menus, n_forms, n_employees;
    MEVENT event;
    Table t;
    /* MAIN, NAV, LIST, EMPLOYEE_INFO, SEARCH, WARNING, CREATE_EMPLOYEE */
    WINDOW *windows[7], *cur_win;
    /* MAIN, NAV, LIST, EMPLOYEE_INFO, SEARCH, WARNING, CREATE_EMPLOYEE */
    PANEL *panels[7], *cur_panel;
    /* NAV, LIST_FILTERS, LIST_ITEMS, EMPLOYEE_INFO, CREATE_EMPLOYEE*/
    MENU *menus[5], *cur_menu;
    /* SEARCH, CREATE_EMPLOYEE */
    FORM *forms[2], *cur_form;
    ITEM **employee_items = NULL;
    Employee *employees = NULL, *selected = NULL, found;
    char **briefs = NULL;

    table.initialize_table(&t, "employees.dat", "ssn.dat", "email.dat", "phone.dat", "recycle.dat");
    n_windows = n_panels = n_menus = n_forms = n_employees = 0;

    debug(&t);

    initscr();
    cbreak();
    noecho();
    start_color();
    curs_set(0);
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);


    /* MAIN WINDOW */
    cur_panel = border_panel(&cur_win, 24, 80, 0, 0);
    panels[n_panels++] = cur_panel;
    windows[n_windows++] = cur_win;
    mvwaddch(cur_win, 0, 9, ACS_TTEE);
    mvwvline(cur_win, 1, 9, ACS_VLINE, 22);
    mvwaddch(cur_win, 23, 9, ACS_BTEE);

    mvwprintw(cur_win, 2, 19, " /$$   /$$ /$$$$$$$$ /$$   /$$ /$$$$$$$   /$$$$$$ ");
    mvwprintw(cur_win, 3, 19, "| $$$ | $$| $$_____/| $$  | $$| $$__  $$ /$$__  $$");
    mvwprintw(cur_win, 4, 19, "| $$$$| $$| $$      | $$  | $$| $$  \\ $$| $$  \\ $$");
    mvwprintw(cur_win, 5, 19, "| $$ $$ $$| $$$$$   | $$  | $$| $$$$$$$/| $$$$$$$$");
    mvwprintw(cur_win, 6, 19, "| $$  $$$$| $$__/   | $$  | $$| $$__  $$| $$__  $$");
    mvwprintw(cur_win, 7, 19, "| $$\\  $$$| $$      | $$  | $$| $$  \\ $$| $$  | $$");
    mvwprintw(cur_win, 8, 19, "| $$ \\  $$| $$$$$$$$|  $$$$$$/| $$  | $$| $$  | $$");
    mvwprintw(cur_win, 9, 19, "|__/  \\__/|________/ \\______/ |__/  |__/|__/  |__/");
    mvwprintw(cur_win, 10, 19, "        /$$$$$$   /$$$$$$  /$$$$$$$  /$$$$$$$     ");
    mvwprintw(cur_win, 11, 19, "       /$$__  $$ /$$__  $$| $$__  $$| $$__  $$    ");
    mvwprintw(cur_win, 12, 19, "      | $$  \\__/| $$  \\ $$| $$  \\ $$| $$  \\ $$    ");
    mvwprintw(cur_win, 13, 19, "      | $$      | $$  | $$| $$$$$$$/| $$$$$$$/    ");
    mvwprintw(cur_win, 14, 19, "      | $$      | $$  | $$| $$__  $$| $$____/     ");
    mvwprintw(cur_win, 15, 19, "      | $$    $$| $$  | $$| $$  \\ $$| $$          ");
    mvwprintw(cur_win, 16, 19, "      |  $$$$$$/|  $$$$$$/| $$  | $$| $$          ");
    mvwprintw(cur_win, 17, 19, "       \\______/  \\______/ |__/  |__/|__/          ");
    mvwprintw(cur_win, 19, 40, "EMPLOYEE");
    mvwprintw(cur_win, 20, 39, "MANAGEMENT");
    mvwprintw(cur_win, 21, 41, "SYSTEM");


    /* NAVIGATION MENU */
    cur_menu = menu_panel(&cur_win, &cur_panel, 6, 8, 1, 1,
        (ITEM*[]) {
            new_item("Create", ""),
            new_item("List", ""),
            new_item("Search", ""),
            new_item("Exit", ""),
            NULL,
        },
        (void*[]) {
            NULL,
            NULL,
            NULL,
            NULL,
        }
    );
    menus[n_menus++] = cur_menu;
    panels[n_panels++] = cur_panel;
    windows[n_windows++] = cur_win;
    box(cur_win, 0 ,0);
    set_menu_sub(cur_menu, derwin(cur_win, 4, 6, 1, 1));
    post_menu(cur_menu);

    /* LIST FILTERS */
    ITEM *menu1items[4];
    {
        char ssn[20], email[20], phone[20];
        sprintf(ssn, "%*s%s%*s", 8, "", "ssn", 8, "");
        sprintf(email, "%*s%s%*s", 7, "", "email", 7, "");
        sprintf(phone, "%*s%s%*s", 7, "", "phone", 7, "");
        menu1items[0] = new_item(ssn, "");
        menu1items[1] = new_item(email, "");
        menu1items[2] = new_item(phone, "");
        menu1items[3] = NULL;
    }
    cur_menu = menu_panel(&cur_win, &cur_panel, 22, 69, 1, 10, 
        menu1items,
        (void*[]) {
            NULL,
            NULL,
            NULL,
        }
    );
    menus[n_menus++] = cur_menu;
    windows[n_windows++] = cur_win;
    panels[n_panels++] = cur_panel;
    box(cur_win, 0, 0);
    set_menu_sub(cur_menu, derwin(cur_win, 2, 59, 1, 5));
    set_menu_format(cur_menu, 1, 5);
    post_menu(cur_menu);
    mvwhline(cur_win, 2, 1, ACS_HLINE, 67);
    mvwaddch(cur_win, 2, 0, ACS_LTEE);
    mvwaddch(cur_win, 2, 68, ACS_RTEE);
    wrefresh(cur_win);
    hide_panel(cur_panel);

    /* LIST ITEMS */
    mvwprintw(windows[2], 3, 1, " id       ssn          name              e-mail             phone  ");
    cur_menu = new_menu((ITEM*[]) {
        NULL,
    });
    menus[n_menus++] = cur_menu;
    set_menu_win(cur_menu, windows[2]);
    set_menu_sub(cur_menu, derwin(windows[2], 17, 67, 4, 1));
    set_menu_mark(cur_menu, NULL);
    set_menu_format(cur_menu, 17, 1);
    menu_opts_off(cur_menu, O_SHOWDESC);
    post_menu(cur_menu);

    /* EMPLOYEE INFO */
    cur_menu = menu_panel(&cur_win, &cur_panel, 16, 64, 4, 12, 
        (ITEM*[]) {
            new_item("  Back ", ""),
            new_item(" Delete ", ""),
            NULL,
        },
        (void*[]) {
            NULL,
            NULL,
        }
    );
    menus[n_menus++] = cur_menu;
    panels[n_panels++] = cur_panel;
    windows[n_windows++] = cur_win;
    set_menu_sub(cur_menu, derwin(cur_win, 1, 17, 14, 24));
    box(cur_win, 0, 0);
    set_menu_format(cur_menu, 1, 2);
    post_menu(cur_menu);
    hide_panel(cur_panel);

    /* SEARCH */
    cur_form = form_panel(&cur_win, &cur_panel, 22, 69, 1, 10, 
        (FIELD*[]) {
            new_field(1, 39, 2, 11, 0, 0),
            new_field(1, 39, 7, 11, 0, 0),
            new_field(1, 39, 12, 11, 0, 0),
            NULL
        }
    );
    forms[n_forms++] = cur_form;
    panels[n_panels++] = cur_panel;
    windows[n_windows++] = cur_win;
    set_form_sub(cur_form, derwin(cur_win, 15, 53, 3, 8));
    form_opts_off(cur_form, O_BS_OVERLOAD);
    post_form(cur_form);
    hide_panel(cur_panel);
    box(cur_win, 0, 0);
    box(form_sub(cur_form),0,0);
    {
        char *labels[] = {
            "SSN:    ",
            "E-mail: ",
            "Phone:  "
        };
        for (int i = 1; i <= 3; i++) {
            WINDOW *win = form_sub(cur_form);
            mvwaddch(win, i*5-4, 10, ACS_ULCORNER);
            mvwhline(win, i*5-4, 11, ACS_HLINE, 39);
            mvwaddch(win, i*5-4, 50, ACS_URCORNER);
            mvwaddch(win, i*5-3, 10, ACS_VLINE);
            mvwprintw(win, i*5-3, 2, "%s", labels[i-1]);
            mvwaddch(win, i*5-3, 50, ACS_VLINE);
            mvwaddch(win, i*5-2, 10, ACS_LLCORNER);
            mvwhline(win, i*5-2, 11, ACS_HLINE, 39);
            mvwaddch(win, i*5-2, 50, ACS_LRCORNER);
        }
    }

    /* WARNING */
    cur_panel = border_panel(&cur_win, 9, 43, 7, 23);
    panels[n_panels++] = cur_panel;
    windows[n_windows++] = cur_win;
    box(cur_win, 0, 0);
    print_centered(cur_win, 6, "%s", "Press any key to return");
    hide_panel(cur_panel);

    /* CREATE EMPLOYEE */
    FIELD *menu1fields[] = {
        new_field(1, 55, 2, 11, 0, 0),
        new_field(1, 55, 5, 11, 0, 0),
        new_field(1, 55, 8, 11, 0, 0),
        new_field(1, 55, 11, 11, 0, 0),
        new_field(1, 55, 14, 11, 0, 0),
        NULL
    };
    cur_form = form_panel(&cur_win, &cur_panel, 22, 69, 1, 10, menu1fields);
    cur_menu = new_menu((ITEM*[]) {new_item("Save", ""), NULL});
    forms[n_forms++] = cur_form;
    panels[n_panels++] = cur_panel;
    windows[n_windows++] = cur_win;
    menus[n_menus++] = cur_menu;
    set_form_sub(cur_form, derwin(cur_win, 16, 69, 0, 0));
    set_menu_mark(cur_menu, NULL);
    menu_opts_off(cur_menu, O_SHOWDESC);
    set_menu_win(cur_menu, cur_win);
    set_menu_sub(cur_menu, derwin(cur_win, 1, 4, 18, 32));
    form_opts_off(cur_form, O_BS_OVERLOAD);
    post_menu(cur_menu);
    post_form(cur_form);
    hide_panel(cur_panel);
    box(cur_win, 0, 0);
    {
        char *labels[] = {
            "ID:     ",
            "SSN:    ",
            "Name:   ",
            "E-mail: ",
            "Phone:  "
        };
        for (int i = 1; i <= 5; i++) {
            WINDOW *win = form_sub(cur_form);
            mvwaddch(win, i*3-2, 10, ACS_ULCORNER);
            mvwhline(win, i*3-2, 11, ACS_HLINE, 55);
            mvwaddch(win, i*3-2, 66, ACS_URCORNER);
            mvwaddch(win, i*3-1, 10, ACS_VLINE);
            mvwprintw(win, i*3-1, 2, "%s", labels[i-1]);
            mvwaddch(win, i*3-1, 66, ACS_VLINE);
            mvwaddch(win, i*3, 10, ACS_LLCORNER);
            mvwhline(win, i*3, 11, ACS_HLINE, 55);
            mvwaddch(win, i*3, 66, ACS_LRCORNER);
        }
    }

    /* MAINLOOP */
    update_panels();
    doupdate();
    while ((ch = getch()) != KEY_F(1)) {
        switch (ch) {
            case KEY_MOUSE:
                if (!getmouse(&event)) {
                    if ((event.bstate & BUTTON1_CLICKED || event.bstate & BUTTON1_DOUBLE_CLICKED)) {
                        if (wenclose(menu_sub(menus[0]), event.y, event.x)) {
                            /* NAVIGATION MENU INTERACTION */
                            ITEM **items = menu_items(menus[0]), *cur;
                            ungetmouse(&event);
                            menu_driver(menus[0], KEY_MOUSE);
                            cur = current_item(menus[0]);
                            if (cur == items[0]) {
                                /* CREATE */
                                curs_set(1);
                                clear_form(forms[1]);
                                show_panel(panels[6]);
                                top_panel(panels[6]);
                            } else if (cur == items[1]) {
                                /* LIST */
                                curs_set(0);
                                list_employees(t, menus[1], menus[2], &employee_items, &employees, &briefs, &n_employees);
                                show_panel(panels[2]);
                                top_panel(panels[2]);
                            } else if (cur == items[2]) {
                                /* SEARCH */
                                curs_set(1);
                                clear_form(forms[0]);
                                show_panel(panels[4]);
                                top_panel(panels[4]);
                            } else if (cur == items[3]) {
                                /* EXIT */
                                goto end;
                            }
                        } else if (panel_below(NULL) == panels[2] && wenclose(menu_sub(menus[1]), event.y, event.x)) {
                            /* LIST FILTERS */
                            ungetmouse(&event);
                            list_employees(t, menus[1], menus[2], &employee_items, &employees, &briefs, &n_employees);
                        } else if (panel_below(NULL) == panels[2] && wenclose(menu_sub(menus[2]), event.y, event.x)) {
                            /*EMPLOYEE LIST*/
                            ungetmouse(&event);
                            menu_driver(menus[2], KEY_MOUSE);
                            ITEM **items = menu_items(menus[2]), *cur;
                            cur = current_item(menus[2]);
                            selected = item_userptr(cur);
                            show_employee(panels[3], selected);
                        } else if (panel_below(NULL) == panels[3] && wenclose(menu_sub(menus[3]), event.y, event.x)) {
                            /*EMPLOYEE INFO*/
                            ungetmouse(&event);
                            menu_driver(menus[3], KEY_MOUSE);
                            ITEM **items = menu_items(menus[3]), *cur;
                            cur = current_item(menus[3]);
                            if (cur == items[1]) {
                                table.delete_employee(&t, selected);
                            }
                            /*BACK*/
                            if (panel_below(panels[3]) == panels[2]) {
                                list_employees(t, menus[1], menus[2], &employee_items, &employees, &briefs, &n_employees);
                            } else if (panel_below(panels[3]) == panels[4]) {
                                curs_set(1);
                                data.destroy(selected);
                            }
                            selected = NULL;
                            top_panel(panel_below(panels[3]));
                        } else if (panel_below(NULL) == panels[4] && wenclose(form_sub(forms[0]), event.y, event.x)) {
                            curs_set(1);
                            ungetmouse(&event);
                            form_driver(forms[0], KEY_MOUSE);
                        } else if (panel_below(NULL) == panels[6] && wenclose(menu_sub(menus[4]), event.y, event.x)) {
                            int valid = 1;
                            regex_t regex;
                            char *buffers[5], *labels[] = {
                                "ID",
                                "SSN",
                                "name",
                                "e-mail",
                                "phone"
                            };
                            FIELD **fields = form_fields(forms[1]);
                            ungetmouse(&event);
                            for (int i = 0; i < 5; i++) {
                                set_current_field(forms[1], fields[i]);
                                form_driver(forms[1], REQ_VALIDATION);
                                buffers[i] = get_string(fields[i]);
                                if (buffers[i][0] == '\0') {
                                    valid = 0;
                                    show_warning(panels, panels[5], "Field %s cannot be empty", labels[i]);
                                }
                            }
                            set_current_field(forms[1], fields[0]);
                            if (valid) {
                                regcomp(&regex, "^[0-9]*$", REG_EXTENDED);
                                if (regexec(&regex, buffers[0], 0, NULL, 0)) {
                                    valid = 0;
                                    show_warning(panels, panels[5], "%s", "ID must be an integer");
                                }
                                regfree(&regex);
                                if (strlen(buffers[0]) > 9) {
                                    valid = 0;
                                    show_warning(panels, panels[5], "%s", "Max ID value is 999999999");
                                }
                                regcomp(&regex, "^[0-9]{9}$", REG_EXTENDED);
                                if (regexec(&regex, buffers[1], 0, NULL, 0)) {
                                    valid = 0;
                                    show_warning(panels, panels[5], "%s", "SSN must be a 9 digit integer");
                                }
                                regfree(&regex);
                                regcomp(&regex, "^[a-zA-Z ]*$", REG_EXTENDED);
                                if (regexec(&regex, buffers[2], 0, NULL, 0)) {
                                    valid = 0;
                                    show_warning(panels, panels[5], "%s", "Name must not contain special characters");
                                }
                                regfree(&regex);
                                regcomp(&regex, "^[0-9]{10}$", REG_EXTENDED);
                                if (regexec(&regex, buffers[4], 0, NULL, 0)) {
                                    valid = 0;
                                    show_warning(panels, panels[5], "%s", "Phone must be a 10 digit integer");
                                }
                                regfree(&regex);
                                if (valid) {
                                    int res;
                                    Employee employee = data.create(atoi(buffers[0]), buffers[1], buffers[2], buffers[3], buffers[4]);
                                    res = table.add_employee(&t, employee);
                                    data.destroy(&employee);
                                    switch (res) {
                                        case 0:
                                            show_warning(panels, panels[5], "%s", "Employee successfully registered");
                                            break;
                                        case -1:
                                            show_warning(panels, panels[5], "%s", "ID already registered");
                                            break;
                                        case -2:
                                            show_warning(panels, panels[5], "%s", "E-mail already registered");
                                            break;
                                        case -3:
                                            show_warning(panels, panels[5], "%s", "Phone already registered");
                                            break;
                                        case -4:
                                            show_warning(panels, panels[5], "%s", "Unexpected error while writing");
                                    }
                                }
                            }
                            curs_set(1);
                            for (int i = 0; i < 5; i++)
                                free(buffers[i]);
                        }
                    } else if (event.bstate & BUTTON4_PRESSED || event.bstate & BUTTON5_PRESSED) {
                        if (panel_below(NULL) == panels[2] && wenclose(menu_sub(menus[2]), event.y, event.x)) {
                            menu_driver(menus[2], event.bstate & BUTTON4_PRESSED ? REQ_PREV_ITEM : REQ_NEXT_ITEM);
                        }
                    }
                }
                break;
            default:
                if (panel_below(NULL) == panels[4]) {
                    /* SEARCH */
                    switch (ch) {
                        case '\n': {
                            int i, res;
                            char *buffer;
                            FIELD **fields = form_fields(forms[0]), *cur = current_field(forms[0]);
                            form_driver(forms[0], REQ_VALIDATION);
                            buffer = get_string(cur);
                            curs_set(0);
                            if (buffer[0] == '\0')
                                show_warning(panels, panels[5], "%s", "Search field cannot be empty");
                            else {
                                selected = &found;
                                if (cur == fields[0])
                                    res = table.find_employee_by_ssn(t, selected, buffer);
                                else if (cur == fields[1])
                                    res = table.find_employee_by_email(t, selected, buffer);
                                else
                                    res = table.find_employee_by_phone(t, selected, buffer);
                                if (!res)
                                    show_employee(panels[3], selected);
                                else
                                    show_warning(panels, panels[5], "%s", "Employee not found");
                                clear_form(forms[0]);
                            }
                            free(buffer);
                        }
                        break;
                        case KEY_UP:
                            form_driver(forms[0], REQ_PREV_FIELD);
                            break;
                        case KEY_DOWN:
                            form_driver(forms[0], REQ_NEXT_FIELD);
                            break;
                        case KEY_LEFT:
                            form_driver(forms[0], REQ_LEFT_CHAR);
                            break;
                        case KEY_RIGHT:
                            form_driver(forms[0], REQ_RIGHT_CHAR);
                            break;
                        case KEY_BACKSPACE:
                            form_driver(forms[0], REQ_DEL_PREV);
                            break;
                        case KEY_DC:
                            form_driver(forms[0], REQ_DEL_CHAR);
                            break;
                        default:
                            form_driver(forms[0], ch);
                    }
                } else if (panel_below(NULL) == panels[6]) {
                    switch (ch) {
                        case KEY_UP:
                            form_driver(forms[1], REQ_PREV_FIELD);
                            break;
                        case '\t':
                        case KEY_DOWN:
                            form_driver(forms[1], REQ_NEXT_FIELD);
                            break;
                        case KEY_LEFT:
                            form_driver(forms[1], REQ_LEFT_CHAR);
                            break;
                        case KEY_RIGHT:
                            form_driver(forms[1], REQ_RIGHT_CHAR);
                            break;
                        case KEY_BACKSPACE:
                            form_driver(forms[1], REQ_DEL_PREV);
                            break;
                        case KEY_DC:
                            form_driver(forms[1], REQ_DEL_CHAR);
                            break;
                        default:
                            form_driver(forms[1], ch);
                    }
                }
        }
        update_panels();
        doupdate();
    }

    end:
    employees = data.clear_array(employees, n_employees);
    for (int i = 0; briefs != NULL && briefs[i] != NULL; i++)
        free(briefs[i]);
    free(briefs);
    for (int i = 0; i < n_panels; i++)
        del_panel(panels[i]);
    for (int i = 0; i < n_menus; i++) {
        ITEM **items = menu_items(menus[i]);
        unpost_menu(menus[i]);
        for (int j = 0; items != NULL && items[j] != NULL; j++)
            free_item(items[j]);
        free_menu(menus[i]);
    }
    for (int i = 0; i < n_forms; i++) {
        FIELD **fields = form_fields(forms[i]);
        unpost_form(forms[i]);
        for (int j = 0; fields != NULL && fields[j] != NULL; j++)
            free_field(fields[j]);
        free_form(forms[i]);
    }
    for (int i = 0; i < n_windows; i++) 
        delwin(windows[i]);
    endwin();
    table.close_table(&t);
    exit(0);
}