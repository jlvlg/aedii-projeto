#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "bst.h"
#include "types.h"

int main(int argc, char *argv[]) {
    BST root = NULL;
    for (int i = 1; i < argc;) {
        switch (atoi(argv[i])) {
            case 1:
                if (strcmp(argv[i+1], "int") == 0) {
                    root = bst.insert(root, bst.create(Int(atoi(argv[i+2]))));
                }
                i += 3;
                break;
            case 2:
                if (strcmp(argv[i+1], "int") == 0) {
                    root = bst.remove(root, Int(atoi(argv[i+2])));
                }
                i += 3;
                break;
            case 3:
                tree.print(root);
                i++;
                break;
        }
    }
    bst.trim(root, root);
}