#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "rb.h"
#include "types.h"

void print_node(RB node) {
    Tree tree_node = (Tree) node;
    switch (tree_node->item->type) {
        case INT:
            printf("%d", *(int*) tree_node->item->data);
            break;
        case CHAR:
            printf("%c", *(char*) tree_node->item->data);
            break;
        case STRING:
            printf("%s", (char*) tree_node->item->data);
    }
    printf(":%c", node->is_black ? 'b' : 'r');
    printf(":%c:", node->parent != NULL && ((Tree) node->parent)->l == (Tree) node ? 'l' : 'r');
    if (node->parent == NULL) {
        printf("nil ");
    } else {
        switch (((Tree) node->parent)->item->type) {
            case INT:
                printf("%d ", *(int*) ((Tree) node->parent)->item->data);
                break;
            case CHAR:
                printf("%c ", *(char*) ((Tree) node->parent)->item->data);
                break;
            case STRING:
                printf("%s ", (char*) ((Tree) node->parent)->item->data);
        }
    }
}

void print(RB root) {
    tree.preorder((Tree) root, (void(*)(Tree)) print_node, 0);
    printf("\n");
}

int main(int argc, char *argv[]) {
    RB root = NULL;
    int error;
    for (int i = 1; i < argc;) {
        switch (atoi(argv[i])) {
            case 1:
                if (!strcmp(argv[i+1], "int")) {
                    rb.insert(&root, rb.create(types.Int(atoi(argv[i+2]))), &error);
                } else if (!strcmp(argv[i+1], "str")) {
                    rb.insert(&root, rb.create(types.String(argv[i+2])), &error);
                } else if (!strcmp(argv[i+1], "char")) {
                    rb.insert(&root, rb.create(types.Char(argv[i+2][0])), &error);
                }
                i += 3;
                break;
            case 2:
                if (!strcmp(argv[i+1], "int")) {
                    rb.remove(&root, types.Int(atoi(argv[i+2])), types.copy);
                } else if (!strcmp(argv[i+1], "str")) {
                    rb.remove(&root, types.String(argv[i+2]), types.copy);
                } else if (!strcmp(argv[i+1], "char")) {
                    rb.remove(&root, types.Char(argv[i+2][0]), types.copy);
                }
                i += 3;
                break;
            case 3:
                print(root);
                i++;
                break;
        }
    }
    tree.clear((Tree) root);
}