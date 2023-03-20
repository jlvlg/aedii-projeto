#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "util.h"
#include "tree.h"

static Tree init(Tree leaf, Item item) {
    leaf->item = item;
    leaf->l = leaf->r = NULL;
    return leaf;
}

static void preorder(Tree root, void function(Tree), int reversed) {
    if (root != NULL) {
        if (reversed) {
            preorder(root->r, function, reversed);
            preorder(root->l, function, reversed);
            function(root);
        } else {
            function(root);
            preorder(root->l, function, reversed);
            preorder(root->r, function, reversed);
        }
    }
}

static void inorder(Tree root, void function(Tree), int reversed) {
    if (root != NULL) {
        inorder(reversed ? root->r : root->l, function, reversed);
        function(root);
        inorder(reversed ? root->l : root->r, function, reversed);
    }
}
static void postorder(Tree root, void function(Tree), int reversed) {
    if (root != NULL) {
        if (reversed) {
            function(root);
            postorder(root->r, function, reversed);
            postorder(root->l, function, reversed);
        } else {
            postorder(root->l, function, reversed);
            postorder(root->r, function, reversed);
            function(root);
        }
    }
}

static void print_node(Tree root) {
    switch (root->item->type) {
        case INT:
            printf("%d ", *(int*) root->item->data);
            break;
        case FLOAT:
            printf("%f ", *(float*) root->item->data);
            break;
        case DOUBLE:
            printf("%lf ", *(double*) root->item->data);
            break;
        case CHAR:
            printf("%c ", *(char*) root->item->data);
            break;
        case STRING:
            printf("%s ", (char*) root->item->data);
    }
}

static void print(Tree root) {
    preorder(root, print_node, 0);
    printf("\n");
}

static int children(Tree node) {
    if (node == NULL)
        return -1;
    if (node->l == NULL && node->r == NULL)
        return 0;
    if (node->l == NULL || node->r == NULL)
        return 1;
    return 2;
}

static void kill_helper(Tree leaf) {
    types.destroy(leaf->item);
    free(leaf);
}

static Tree kill(Tree leaf) {
    kill_helper(leaf);
    return NULL;
}

static Tree clear(Tree root) {
    postorder(root, kill_helper, 0);
    return NULL;
}

static Tree search(Tree root, Item item) {
    if (root != NULL) {
        switch (types.cmp(item, root->item)) {
            case -1:
                return search(root->l, item);
            case 0:
                return root;
            case 1:
                return search(root->r, item);
        }
    }
    return NULL;
}

static Tree max(Tree root) {
    if (root == NULL || root->r == NULL)
        return root;
    return max(root->r);
}

static Tree min(Tree root) {
    if (root == NULL || root->l == NULL)
        return root;
    return min(root->l);
}

static int height(Tree root) {
    if (root == NULL)
        return 0;
    return util.max(height(root->l), height(root->r)) + 1;
}

static int count(Tree root) {
    if (root == NULL)
        return 0;
    return count(root->l) + count(root->r) + 1;
}

static int count_filtered(Tree root, Item item) {
    if (root == NULL)
        return 0;
    return count_filtered(root->l, item) + count_filtered(root->r, item) + !types.cmp(item, root->item);
}

static void to_array_helper(Tree branch, Tree *array, int *i, int reversed) {
    if (branch != NULL) {
        to_array_helper(reversed ? branch->r : branch->l, array, i, reversed);
        array[(*i)++] = branch;
        to_array_helper(reversed ? branch->l : branch->r, array, i, reversed);
    }
}

static Tree* to_array(Tree branch, int *out_count, int reversed, int *in_count) {
    int i = 0;
    Tree *array = util.safe_malloc(sizeof(Tree) * (in_count != NULL ? *in_count : (*out_count = count(branch))));
    to_array_helper(branch, array, &i, reversed);
    return array;
}

static void to_array_filtered_helper(Tree branch, Tree *array, Item item, int *i) {
    if (branch != NULL) {
        switch (types.cmp(item, branch->item)) {
            case 0:
                array[(*i)++] = branch;
            case -1:
                to_array_filtered_helper(branch->l, array, item, i);
                break;
            case 1:
                to_array_filtered_helper(branch->r, array, item, i);
        }
    }
}

static Tree* to_array_filtered(Tree branch, Item item, int *out_count, int *in_count) {
    int i = 0;
    Tree *array = util.safe_malloc(sizeof(Tree) * (in_count != NULL ? *in_count : (*out_count = count_filtered(branch, item))));
    to_array_filtered_helper(branch, array, item, &i);
    return array;
}

static Tree simple_rotation_left(Tree p) {
    Tree u = p->r;
    p->r = u->l;
    u->l = p;
    return u;
}

static Tree simple_rotation_right(Tree p) {
    Tree u = p->l;
    p->l = u->r;
    u->r = p;
    return u;
}

static Tree double_rotation_left(Tree p) {
    p->r = simple_rotation_right(p->r);
    return simple_rotation_left(p);
}

static Tree double_rotation_right(Tree p) {
    p->l = simple_rotation_left(p->l);
    return simple_rotation_right(p);
}

const struct tree_methods tree = {
    .init = init,
    .print = print,
    .preorder = preorder,
    .inorder = inorder,
    .postorder = postorder,
    .children = children,
    .kill = kill,
    .clear = clear,
    .max = max,
    .min = min,
    .search = search,
    .height = height,
    .count = count,
    .count_filtered = count_filtered,
    .to_array = to_array,
    .to_array_filtered = to_array_filtered,
    .simple_rotation_left = simple_rotation_left,
    .simple_rotation_right = simple_rotation_right,
    .double_rotation_left = double_rotation_left,
    .double_rotation_right = double_rotation_right
};