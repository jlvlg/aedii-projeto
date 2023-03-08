#include <stdlib.h>
#include "util.h"
#include "types.h"
#include "tree.h"
#include "bst.h"

static BST init(BST leaf, Item item) {
    return tree.init(leaf, item);
}

static BST create(Item item) {
    return init(util.safe_malloc(sizeof(BST_Node)), item);
}

static BST insert(BST root, BST leaf) {
    if (leaf == NULL)
        return root;
    if (root == NULL) 
        return leaf;

    if (types.cmp(leaf->item, root->item) > 0)
        root->r = insert(root->r, leaf);
    else
        root->l = insert(root->l, leaf);
        
    return root;
}

static BST remove(BST root, Item item) {
    if (root == NULL)
        return NULL;
    switch (types.cmp(item, root->item)) {
        case 0: 
            switch (tree.children(root)) {
                case 0:
                    return tree.kill(root);
                case 1: {
                    BST child = root->l != NULL ? root->l : root->r;
                    tree.kill(root);
                    return child;
                }
                case 2:
                    types.destroy(root->item);
                    root->item = types.copy(tree.max(root->l)->item);
                    root->l = remove(root->l, root->item);
            } break;
        case -1:
            root->l = remove(root->l, item);
            break;
        case 1:
            root->r = remove(root->r, item);
    }
    return root;
}

// static BST trim(BST root, BST branch) {
//     if (root != NULL) {
//         if (branch != NULL) {
//             trim(root, branch->l);
//             trim(root, branch->r);
//             remove(root, branch->item);
//         }
//     }
//     return NULL;
// }

static BST trim(BST root, BST branch) {
    int count;
    Tree* array = tree.to_array(branch, &count);
    for (int i = 0; i < count; i++) {
        root = remove(root, array[i]->item);
    }
    return root;
}

const struct bst_methods bst = {
    .init = init,
    .create = create,
    .insert = insert,
    .remove = remove,
    .trim = trim
};