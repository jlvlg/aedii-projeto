#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "avl.h"
#include "types.h"

void print_node(AVL node) {
    printf("%d:%d ", *(int*) ((Tree) node)->item.data, node->bf);
}

void print(AVL node) {
    tree.preorder(node, print_node);
    printf("\n");
}

int main(int argc, char *argv[]) {
    AVL root = NULL;
    for (int i = 1; i < argc;) {
        switch (atoi(argv[i])) {
            case 1:
                if (strcmp(argv[i+1], "int") == 0) {
                    int change;
                    root = avl.insert(root, avl.create(Int(atoi(argv[i+2]))), &change);
                }
                i += 3;
                break;
            case 2:
                if (strcmp(argv[i+1], "int") == 0) {
                    int change;
                    root = avl.remove(root, Int(atoi(argv[i+2])), &change);
                }
                i += 3;
                break;
            case 3:
                print(root);
                i++;
                break;
        }
    }
    avl.trim(root, root);
}