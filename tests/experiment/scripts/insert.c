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
    char type[4];
    int changes;
    clock_t t;
    printf("BST elapsed time,BST height,AVL elapsed time,AVL height,RB elapsed time,RB height\n");
    while (1) {
        scanf("%3[^\n]%*c", type);
        if (strcmp(type, "out")) {
            if (!strcmp(type, "int")) {
                char data[100];
                scanf("%99[^\n]%*c", data);
                item = types.Int(atoi(data));
            } else if (!strcmp(type, "str")) {
                char data[10000];
                scanf("%9999[^\n]%*c", data);
                item = types.String(data);
            }
            t = clock();
            bst_root = bst.insert(bst_root, bst.create(types.copy(item)));
            measure(t, bst_root, 0);
            t = clock();
            avl_root = avl.insert(avl_root, avl.create(types.copy(item)), &changes);
            measure(t, avl_root, 0);
            t = clock();
            rb.insert(&rb_root, rb.create(types.copy(item)));
            measure(t, rb_root, 1);
            printf("\n");
            types.destroy(item);
        } else {
            break;
        }
    }
    bst.trim(bst_root, bst_root);
    avl.trim(avl_root, avl_root);
    rb.trim(&rb_root, rb_root);
}