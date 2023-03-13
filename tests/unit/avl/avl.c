#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "avl.h"
#include "types.h"

void print_node(AVL node) {
    Tree tree_node = node;
    switch (tree_node->item.type) {
        case INT:
            printf("%d", *(int*) tree_node->item.data);
            break;
        case CHAR:
            printf("%c", *(char*) tree_node->item.data);
            break;
        case STRING:
            printf("%s", (char*) tree_node->item.data);
    }
    printf(":%d ", node->bf);
}

void print(AVL node) {
    tree.preorder(node, print_node, 0);
    printf("\n");
}

int main(int argc, char *argv[]) {
    AVL root = NULL;
    int changes, error;
    for (int i = 1; i < argc;) {
        switch (atoi(argv[i])) {
            case 1:
                if (!strcmp(argv[i+1], "int")) {
                    root = avl.insert(root, avl.create(types.Int(atoi(argv[i+2]))), &changes, &error);
                } else if (!strcmp(argv[i+1], "str")) {
                    root = avl.insert(root, avl.create(types.String(argv[i+2])), &changes, &error);
                } else if (!strcmp(argv[i+1], "char")) {
                    root = avl.insert(root, avl.create(types.Char(argv[i+2][0])), &changes, &error);
                }
                i += 3;
                break;
            case 2:
                if (!strcmp(argv[i+1], "int")) {
                    root = avl.remove(root, types.Int(atoi(argv[i+2])), &changes);
                } else if (!strcmp(argv[i+1], "str")) {
                    root = avl.remove(root, types.String(argv[i+2]), &changes);
                } else if (!strcmp(argv[i+1], "char")) {
                    root = avl.remove(root, types.Char(argv[i+2][0]), &changes);
                }
                i += 3;
                break;
            case 3:
                print(root);
                i++;
                break;
            case 4:
                printf("%d\n", tree.height(root));
                i++;
        }
    }
    tree.clear(root);
}