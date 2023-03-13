#include <stdlib.h>
#include <curses.h>
#include <panel.h>
#include <form.h>
#include <menu.h>
#include "data/data.h"
#include "table.h"

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

void show_employee(PANEL *panel, Employee *employee) {
    char id[snprintf(NULL, 0, "%d", employee->id)];
    sprintf(id, "%d", employee->id);
    WINDOW *win = panel_window(panel);
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

int main() {
    int ch, n_windows, n_panels, n_menus, n_employees;
    MEVENT event;
    Table t;
    /* MAIN, NAV, LIST, EMPLOYEE_SHOW*/
    WINDOW *windows[4], *cur_win;
    /* MAIN, NAV, LIST, EMPLOYEE_SHOW*/
    PANEL *panels[4], *cur_panel;
    /* NAV, LIST_FILTERS, LIST_ITEMS, EMPLOYEE_SHOW_BACK*/
    MENU *menus[4], *cur_menu;
    ITEM **employee_items = NULL;
    Employee *employees = NULL, *selected = NULL;
    char **briefs = NULL;

    table.initialize_table(&t, "employees.dat", "ssn.dat", "email.dat", "phone.dat", "recycle.dat");
    n_windows = n_panels = n_menus = 0;

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
    ITEM *items[4];
    {
        char ssn[20], email[20], phone[20];
        sprintf(ssn, "%*s%s%*s", 8, "", "ssn", 8, "");
        sprintf(email, "%*s%s%*s", 7, "", "email", 7, "");
        sprintf(phone, "%*s%s%*s", 7, "", "phone", 7, "");
        items[0] = new_item(ssn, "");
        items[1] = new_item(email, "");
        items[2] = new_item(phone, "");
        items[3] = NULL;
    }
    cur_menu = menu_panel(&cur_win, &cur_panel, 22, 69, 1, 10, 
        items,
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

    /*Show Employee Data*/
    cur_menu = menu_panel(&cur_win, &cur_panel, 17, 64, 5, 12, 
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
    set_menu_sub(cur_menu, derwin(cur_win, 1, 17, 15, 24));
    box(cur_win, 0, 0);
    set_menu_format(cur_menu, 1, 2);
    post_menu(cur_menu);
    hide_panel(cur_panel);

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
                                
                            } else if (cur == items[1]) {
                                /* LIST */
                                show_panel(panels[2]);
                                top_panel(panels[2]);
                            } else if (cur == items[3]) {
                                /* EXIT */
                                goto end;
                            }
                        } else if (panel_below(NULL) == panels[2] && wenclose(menu_sub(menus[1]), event.y, event.x)) {
                            /* LIST FILTERS */
                            ITEM **items = menu_items(menus[1]), *cur;
                            ungetmouse(&event);
                            menu_driver(menus[1], KEY_MOUSE);
                            cur = current_item(menus[1]);
                            if (employee_items != NULL)
                                free(employee_items);
                            employee_items = menu_items(menus[2]);
                            unpost_menu(menus[2]);
                            if (employees != NULL)
                                employees = data.clear_array(employees, n_employees);
                            for (int i = 0; briefs != NULL && briefs[i] != NULL; i++)
                                free(briefs[i]);
                            if (briefs != NULL)
                                free(briefs);
                            for (int i = 0; employee_items != NULL && employee_items[i] != NULL; i++)
                                free_item(employee_items[i]);
                            if (cur == items[0]) {
                                employees = table.list_employees_by_ssn(t, 0, &n_employees);
                            } else if (cur == items[1]) {
                                employees = table.list_employees_by_email(t, 0, &n_employees);
                            } else if (cur == items[2]) {
                                employees = table.list_employees_by_phone(t, 0, &n_employees);
                            }
                            employee_items = malloc(sizeof(ITEM*) * (n_employees + 1));
                            briefs = malloc(sizeof(char*) * (n_employees + 1));
                            for (int i = 0; i < n_employees; i++) {
                                briefs[i] = malloc(sizeof(char) * 68);
                                char id[snprintf(NULL, 0, "%d", employees[i].id)];
                                sprintf(id, "%d", employees[i].id);
                                sprintf(briefs[i], "%-4.4s   %-9.9s   %-12.12s   %-20.20s   %-10.10s", id, employees[i].ssn, employees[i].name, employees[i].email, employees[i].phone);
                                employee_items[i] = new_item(briefs[i], "");
                                set_item_userptr(employee_items[i], &employees[i]);
                            }
                            briefs[n_employees] = NULL;
                            employee_items[n_employees] = NULL;
                            set_menu_items(menus[2], employee_items);
                            post_menu(menus[2]);
                        } else if (panel_below(NULL) == panels[2] && wenclose(menu_sub(menus[2]), event.y, event.x)) {
                            /*EMPLOYEE LIST*/
                            ungetmouse(&event);
                            menu_driver(menus[2], KEY_MOUSE);
                            if (event.bstate & BUTTON1_DOUBLE_CLICKED) {
                            ITEM **items = menu_items(menus[2]), *cur;
                                cur = current_item(menus[2]);
                                set_panel_userptr(panels[3], panels[2]);
                                selected = item_userptr(cur);
                                show_employee(panels[3], selected);
                            }
                        } else if (panel_below(NULL) == panels[3] && wenclose(menu_sub(menus[3]), event.y, event.x)) {
                            /*EMPLOYEE INFO*/
                            ungetmouse(&event);
                            menu_driver(menus[3], KEY_MOUSE);
                            if (event.bstate & BUTTON1_DOUBLE_CLICKED) {
                                ITEM **items = menu_items(menus[3]), *cur;
                                cur = current_item(menus[3]);
                                if (cur == items[1]) {
                                    table.delete_employee(&t, selected);
                                }
                                /*BACK*/
                                selected = NULL;
                                top_panel(panel_userptr(panels[3]));
                            }
                        }
                    } else if (event.bstate & BUTTON4_PRESSED || event.bstate & BUTTON5_PRESSED) {
                        if (panel_below(NULL) == panels[2] && wenclose(menu_sub(menus[2]), event.y, event.x)) {
                            menu_driver(menus[2], event.bstate & BUTTON4_PRESSED ? REQ_PREV_ITEM : REQ_NEXT_ITEM);
                        }
                    }
                }
                update_panels();
                doupdate();
                break;
        }
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
        for (int j = 0; items != NULL && items[j] != NULL; j++) {
            free_item(items[j]);
        }
        free_menu(menus[i]);
    }
    for (int i = 0; i < n_windows; i++) 
        delwin(windows[i]);
    endwin();
    table.close_table(&t);
    exit(0);
}