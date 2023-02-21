#ifndef AVL_H
#define AVL_H

#include "types.h"
#include "tree.h"

/// @file
/// @brief AVL implementation of the abstract tree

/// AVL implementation of the abstract tree
typedef struct avl_node {
    Node node;
    int bf;
} AVL_Node;

/// Pointer to an AVL_Node
typedef AVL_Node* AVL;

struct avl_methods {

};

extern const struct avl_methods avl;

#endif