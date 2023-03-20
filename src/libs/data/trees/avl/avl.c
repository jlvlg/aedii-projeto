#include <stdlib.h>
#include "util.h"
#include "types.h"
#include "tree.h"
#include "avl.h"

static AVL init(AVL leaf, Item item) {
    tree.init((Tree) leaf, item);
    leaf->bf = 0;
    return leaf;
}

static AVL create(Item item) {
    return init(util.safe_malloc(sizeof(AVL_Node)), item);
}

static AVL rotate(AVL root) {
    Tree tree_root = (Tree) root;
    if (root->bf > 0) {
        if (((AVL) tree_root->r)->bf < 0) {
            root = (AVL) tree.double_rotation_left(tree_root);
        } else {
            root = (AVL) tree.simple_rotation_left(tree_root);

            ((AVL) ((Tree) root)->l)->bf = root->bf == 0 ? 1 : 0;
            root->bf = root->bf == 0 ? -1 : 0;
            
            return root;
        }
    } else {
        if (((AVL) tree_root->l)->bf > 0) {
            root = (AVL) tree.double_rotation_right(tree_root);
        } else {
            root = (AVL) tree.simple_rotation_right(tree_root);

            ((AVL) ((Tree) root)->r)->bf = root->bf == 0 ? -1 : 0;
            root->bf = root->bf == 0 ? 1 : 0;

            return root;
        }
    }
    ((AVL) ((Tree) root)->l)->bf = root->bf == 1 ? -1 : 0;
    ((AVL) ((Tree) root)->r)->bf = root->bf == -1 ? 1 : 0;
    root->bf = 0;
    return root;
}

static AVL balance(AVL root, int *changes, int right, int rem) {
    if (*changes) {
        *changes = abs(rem ^ right ? root->bf++ : root->bf--) == rem;
    }

    if (abs(root->bf) > 1) {
        Tree tree_root = (Tree) root;
        if (rem && !((AVL) (right ? tree_root->l : tree_root->r))->bf)
            *changes = 0;
        return rotate(root);
    }
    return root;
}

static AVL insert(AVL root, AVL leaf, int *changes, int *error) {
    Tree tree_root = (Tree) root, tree_leaf = (Tree) leaf;
    int cmp;

    *error = 0;

    if (leaf == NULL)
        return root;

    if (root == NULL) {
        *changes = 1;
        return leaf;
    }

    cmp = types.cmp(tree_leaf->item, tree_root->item);

    switch (cmp) {
        case 0:
            *error = 1;
            tree.kill((Tree) leaf);
            return root;
        case -1:
            tree_root->l = (Tree) insert((AVL) tree_root->l, leaf, changes, error);
            break;
        case 1:
            tree_root->r = (Tree) insert((AVL) tree_root->r, leaf, changes, error);
    }

    return balance(root, changes, cmp == 1, 0);
}

static AVL remove(AVL root, Item item, int *changes, Item copyfun(Item)) {
    Tree tree_root = (Tree) root;
    int cmp;

    if (root == NULL) {
        *changes = 0;
        return NULL;
    }

    cmp = types.cmp(item, tree_root->item);

    switch (cmp) {
        case 0: 
            *changes = 1;
            switch (tree.children(tree_root)) {
                case 0:
                    return (AVL) tree.kill(tree_root);
                case 1: {
                    AVL child = (AVL) (tree_root->l != NULL ? tree_root->l : tree_root->r);
                    tree.kill(tree_root);
                    return child;
                }
                case 2:
                    types.destroy(tree_root->item);
                    tree_root->item = copyfun(tree.max(tree_root->l)->item);
                    tree_root->l = (Tree) remove((AVL) tree_root->l, tree_root->item, changes, copyfun);
                    return balance(root, changes, 0, 1);
            }
        case -1:
            tree_root->l = (Tree) remove((AVL) tree_root->l, item, changes, copyfun);
            break;
        case 1:
            tree_root->r = (Tree) remove((AVL) tree_root->r, item, changes, copyfun);
    }

    return balance(root, changes, cmp == 1, 1);
}

const struct avl_methods avl = {
    .init = init,
    .create = create,
    .insert = insert,
    .remove = remove
};