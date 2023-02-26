#ifndef RB_H
#define RB_H

#include "types.h"
#include "tree.h"

/// @file
/// @brief Red-black implementation of the abstract tree

/// Red-black implementation of the abstract tree
typedef struct rb_node {
    Node node;
    int is_black;
    struct rb_node *parent;
} RB_Node;

/// Pointer to an RB_Node
typedef RB_Node* RB;

struct rb_methods {
    /// @brief Creates a new node containing an Item
    /// @param item Item to be sowed
    /// @return Node containing item
    RB (*create)(Item item);

    /// @brief Inserts a node into a tree
    /// @param root Tree to be inserted into
    /// @param leaf Node to be inserted
    /// @param changes Tracks changes in the tree's height
    /// @return Updated tree
    void (*insert)(RB *root, RB leaf);

    /// @brief Searches a tree for a node containing an item and removes it
    /// @param root Tree to be removed from
    /// @param item Item to be removed
    /// @param growth Tracks changes in the tree's height
    /// @return Updated tree
    void (*remove)(RB *root, Item item);

    /// @brief Removes an entire branch from a tree
    /// @param root Root of the tree
    /// @param branch Branch to be removed
    /// @return NULL pointer
    void (*trim)(RB *root, RB branch);
};

extern const struct rb_methods rb;

#endif