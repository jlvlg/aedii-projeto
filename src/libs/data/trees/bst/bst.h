#ifndef BST_H
#define BST_H

#include "types.h"
#include "tree.h"

/// @file
/// @brief BST implemantation of the abstract tree

/// BST implementation of the abstract Tree node
typedef Node BST_Node;

/// Pointer to a BST_Node
typedef BST_Node* BST;

/// Encapsulates functions into the bst namespace
struct bst_methods {
    /// @brief Initializes an already allocated memory space
    /// @param leaf Pointer to allocated memory 
    /// @param item Item to be sowed
    /// @return Node containing item
    BST (*init)(BST leaf, Item item);

    /// @brief Allocates memory and initializes
    /// @param item Item to be wrapped
    BST (*create)(Item item);

    /// @brief Inserts a node into a tree
    /// @param root Tree to be inserted into
    /// @param leaf Node to be inserted
    /// @param error Is set to 0 if the operation succeded else 1
    /// @return Updated tree
    BST (*insert)(BST root, BST leaf, int* error);

    /// @brief Searches a tree for a node containing an item and removes it
    /// @param root Tree to be removed from
    /// @param item Item to be removed
    /// @param copyfun Function that duplicates an item
    /// @return Updated tree
    BST (*remove)(BST root, Item item, Item copyfun(Item));
};

extern const struct bst_methods bst;

#endif