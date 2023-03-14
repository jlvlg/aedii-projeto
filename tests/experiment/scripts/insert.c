#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "rb.h"
#include "avl.h"
#include "bst.h"
#include "types.h"

void measure(clock_t t, Tree root, int last) {
    printf("%g,%d%s", ((double) clock() - t) / CLOCKS_PER_SEC, tree.height(root), last ? "" : ",");
}

int main() {
    BST bst_root = NULL;
    AVL avl_root = NULL;
    RB rb_root = NULL;
    Item item;
    char type[5];
    int changes, error;
    clock_t t;
    printf("BST elapsed time,BST height,AVL elapsed time,AVL height,RB elapsed time,RB height\n");
    while (1) {
        fgets(type, 5, stdin);
        type[strcspn(type, "\n")] = '\0';
        if (strcmp(type, "out")) {
            if (!strcmp(type, "int")) {
                char data[100];
                fgets(data, 100, stdin);
                data[strcspn(data, "\n")] = '\0';
                int num = atoi(data);
                if (!num)
                    continue;
                item = types.Int(num);
            } else if (!strcmp(type, "str")) {
                char data[10000];
                fgets(data, 10000, stdin);
                data[strcspn(data, "\n")] = '\0';
                item = types.String(data);
            }
            t = clock();
            bst_root = bst.insert(bst_root, bst.create(types.copy(item)), &error, types.copy);
            measure(t, bst_root, 0);
            t = clock();
            avl_root = avl.insert(avl_root, avl.create(types.copy(item)), &changes, &error, types.copy);
            measure(t, avl_root, 0);
            t = clock();
            rb.insert(&rb_root, rb.create(types.copy(item)), &error, types.copy);
            measure(t, rb_root, 1);
            printf("\n");
            types.destroy(item);
        } else {
            break;
        }
    }
    tree.clear(bst_root);
    tree.clear(avl_root);
    tree.clear(rb_root);
}