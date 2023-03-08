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
    /// @brief Initializes an already allocated memory space
    /// @param leaf Pointer to allocated memory 
    /// @param item Item to be sowed
    /// @return Node containing item
    AVL (*init)(AVL leaf, Item item);

    /// @brief Allocates memory and initializes
    /// @param item Item to be wrapped
    AVL (*create)(Item item);

    /// @brief Inserts a node into a tree
    /// @param root Tree to be inserted into
    /// @param leaf Node to be inserted
    /// @param changes Tracks changes in the tree's height
    /// @return Updated tree
    AVL (*insert)(AVL root, AVL leaf, int* changes);

    /// @brief Searches a tree for a node containing an item and removes it
    /// @param root Tree to be removed from
    /// @param item Item to be removed
    /// @param changes Tracks changes in the tree's height
    /// @return Updated tree
    AVL (*remove)(AVL root, Item item, int* changes);

    /// @brief Removes an entire branch from a tree
    /// @param root Root of the tree
    /// @param branch Branch to be removed
    /// @return NULL pointer
    AVL (*trim)(AVL root, AVL branch);
};

extern const struct avl_methods avl;

#endif