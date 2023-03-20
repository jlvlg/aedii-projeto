#include <stdlib.h>
#include "util.h"
#include "types.h"
#include "tree.h"
#include "rb.h"

static RB init(RB leaf, Item item) {
    tree.init((Tree) leaf, item);
    leaf->parent = NULL;
    leaf->is_black = 0;
    return leaf;
}

static RB create(Item item) {
    return init(util.safe_malloc(sizeof(RB_Node)), item);
}

static RB parent(RB node) {
    return node != NULL ? node->parent : NULL;
}

static RB grandparent(RB node) {
    return parent(parent(node));
}

static int is_black(RB node) {
    return node == NULL || node->is_black;
}

static int is_red(RB node) {
    return !is_black(node);
}

static int is_root(RB node) {
    return parent(node) == NULL;
}

static int is_left(RB node) {
    return !is_root(node) && ((Tree) node->parent)->l == (Tree) node;
}

static RB sibling(RB node) {
    if (node != NULL && !is_root(node)) {
        Tree tree_parent = (Tree) parent(node);
        return (RB) (is_left(node) ? tree_parent->r : tree_parent->l);
    }
    return NULL;
}

static RB uncle(RB node) {
    return sibling(parent(node));
}

static RB closest_nephew(RB node) {
    Tree sibling_tree = (Tree) sibling(node);
    return (RB) (is_left(node) ? sibling_tree->l : sibling_tree->r);
}

static RB farthest_nephew(RB node) {
    Tree sibling_tree = (Tree) sibling(node);
    return (RB) (is_left(node) ? sibling_tree->r : sibling_tree->l);
}

static void redden(RB node) {
    node->is_black = 0;
}

static void blacken(RB node) {
    node->is_black = 1;
}

static void update_parents(RB *root, RB node, RB new, int left) {
    if (new != NULL) {
        new->parent = node->parent;
    }
    if (is_root(node)) {
        *root = new;
    } else if (left)  {
        ((Tree) node->parent)->l = (Tree) new;
    } else {
        ((Tree) node->parent)->r = (Tree) new;
    }
}

static void rotate(RB *root, RB p, RB u, RB t, RB rotate_function(RB node)) {
    int left = is_left(p);

    u->parent = p->parent;
    p->parent = u;
    if (t != NULL)
        t->parent = p;
    
    p = rotate_function(p);

    update_parents(root, p, p, left);
}

static void simple_rotation_left(RB *root, RB p) {
    RB u = (RB) ((Tree) p)->r;
    RB t = (RB) ((Tree) u)->l;

    rotate(root, p, u, t, (RB(*)(RB)) tree.simple_rotation_left);
}

static void simple_rotation_right(RB *root, RB p) {
    RB u = (RB) ((Tree) p)->l;
    RB t = (RB) ((Tree) u)->r;

    rotate(root, p, u, t, (RB(*)(RB)) tree.simple_rotation_right);
}

static void double_rotation_right(RB *root, RB p) {
    simple_rotation_left(root, (RB) ((Tree) p)->l);
    simple_rotation_right(root, p);
}

static void double_rotation_left(RB *root, RB p) {
    simple_rotation_right(root, (RB) ((Tree) p)->r);
    simple_rotation_left(root, p);
}

static void adjust(RB *root, RB leaf) {
    while (is_red(leaf) && is_red(parent(leaf))) {
        if (is_red(uncle(leaf))) {
            blacken(uncle(leaf));
            blacken(parent(leaf));
            redden(grandparent(leaf));
            leaf = grandparent(leaf);
            continue;
        }
        if (is_left(leaf)) {
            if (is_left(parent(leaf))) {
                simple_rotation_right(root, grandparent(leaf));
                blacken(parent(leaf));
                redden(sibling(leaf));
            } else {
                double_rotation_left(root, grandparent(leaf));
                blacken(leaf);
                redden((RB) ((Tree) leaf)->l);
            }
        } else {
            if (!is_left(parent(leaf))) {
                simple_rotation_left(root, grandparent(leaf));
                blacken(parent(leaf));
                redden(sibling(leaf));
            } else {
                double_rotation_right(root, grandparent(leaf));
                blacken(leaf);
                redden((RB) ((Tree) leaf)->r);
            }
        }
    }
    blacken(*root);
}

static void remove_double_black(RB *root, RB node, int nil) {
    if (nil) {
        update_parents(root, node, NULL, is_left(node));
        tree.kill((Tree) node);
    }
}

static void handle_double_black(RB *root, RB node, int nil) {
    if (is_root(node)) {
        remove_double_black(root, node, nil);
        return;
    }
    int left = is_left(node);
    RB parent_node = parent(node);
    RB grandparent_node = parent(parent_node);
    RB sibling_node = sibling(node);
    RB closest_nephew_node = closest_nephew(node);
    RB farthest_nephew_node = farthest_nephew(node);
    if (is_black(parent_node) &&
        is_red(sibling_node) &&
        is_black(closest_nephew_node) &&
        is_black(farthest_nephew_node)) {
            redden(parent_node);
            blacken(sibling_node);
            if (left) {
                simple_rotation_left(root, parent_node);
            } else {
                simple_rotation_right(root, parent_node);
            }
            handle_double_black(root, node, nil);
            return;
        }
    if (is_black(parent_node) &&
        is_black(sibling_node) &&
        is_black(closest_nephew_node) &&
        is_black(farthest_nephew_node)) {
            remove_double_black(root, node, nil);
            redden(sibling_node);
            handle_double_black(root, parent_node, 0);
            return;
        }
    if (is_red(parent_node) &&
        is_black(sibling_node) &&
        is_black(closest_nephew_node) &&
        is_black(farthest_nephew_node)) {
            blacken(parent_node);
            redden(sibling_node);
            remove_double_black(root, node, nil);
            return;
        }
    if (is_black(sibling_node) &&
        is_red(closest_nephew_node) &&
        is_black(farthest_nephew_node)) {
            redden(sibling_node);
            blacken(closest_nephew_node);
            if (left) {
                simple_rotation_right(root, sibling_node);
            } else {
                simple_rotation_left(root, sibling_node);
            }
            handle_double_black(root, node, nil);
            return;
        }
    if (is_black(sibling_node) &&
        is_red(farthest_nephew_node)) {
            sibling_node->is_black = parent_node->is_black;
            blacken(farthest_nephew_node);
            blacken(parent_node);
            remove_double_black(root, node, nil);
            if (left) {
                simple_rotation_left(root, parent_node);
            } else {
                simple_rotation_right(root, parent_node);
            }
            return;
        }
}

static void insert(RB *root, RB leaf, int *error) {
    int cmp;
    Tree tree_leaf = (Tree) leaf, track = NULL;

    *error = 0;
    
    for (Tree search = (Tree) *root; search != NULL;) {
        track = search;
        cmp = types.cmp(tree_leaf->item, track->item);

        switch (cmp) {
            case 0:
                *error = 1;
                tree.kill((Tree) leaf);
                return;
            case -1:
                search = search->l;
                break;
            case 1:
                search = search->r;
        }
    }

    leaf->parent = (RB) track;

    if (track == NULL) {
        *root = leaf;
    } else {
        switch (cmp) {
            case 0:
                *error = 1;
                tree.kill((Tree) leaf);
                return;
            case -1:
                track->l = (Tree) leaf;
                break;
            case 1:
                track->r = (Tree) leaf;
        }
    }
    
    adjust(root, leaf);
}

static void remove(RB *root, Item item, Item copyfun(Item)) {
    for (Tree search = (Tree) *root; search != NULL;) {
        switch (types.cmp(item, search->item)) {
            case 0:
                switch (tree.children(search)) {
                    case 0:
                        if (is_root((RB) search) || is_red((RB) search)) {
                            update_parents(root, (RB) search, NULL, is_left((RB) search));
                            tree.kill(search);
                        } else {
                            handle_double_black(root, (RB) search, 1);
                        }
                        break;
                    case 1: {
                        RB child = (RB) (search->l != NULL ? search->l : search->r);
                        blacken(child);
                        update_parents(root, (RB) search, child, is_left((RB) search));
                        tree.kill(search);
                    } break;
                    case 2: {
                        Item child_item = copyfun(tree.max(search->l)->item);
                        remove(root, child_item, copyfun);
                        types.destroy(search->item);
                        search->item = child_item;
                    }
                } break;
            case -1:
                search = search->l;
                continue;
            case 1:
                search = search->r;
                continue;
        }
        break;
    }
}

const struct rb_methods rb = {
    .init = init,
    .create = create,
    .insert = insert,
    .remove = remove
};