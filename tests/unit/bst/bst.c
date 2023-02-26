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
                if (!strcmp(argv[i+1], "int")) {
                    root = bst.insert(root, bst.create(types.Int(atoi(argv[i+2]))));
                } else if (!strcmp(argv[i+1], "str")) {
                    root = bst.insert(root, bst.create(types.String(argv[i+2])));
                } else if (!strcmp(argv[i+1], "char")) {
                    root = bst.insert(root, bst.create(types.Char(argv[i+2][0])));
                }
                i += 3;
                break;
            case 2:
                if (!strcmp(argv[i+1], "int")) {
                    root = bst.remove(root, types.Int(atoi(argv[i+2])));
                } else if (!strcmp(argv[i+1], "str")) {
                    root = bst.remove(root, types.String(argv[i+2]));
                } else if (!strcmp(argv[i+1], "char")) {
                    root = bst.remove(root, types.Char(argv[i+2][0]));
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