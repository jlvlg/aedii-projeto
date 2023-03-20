#ifndef TREE_H
#define TREE_H

#include "types.h"

/// @file
/// Implements basic functions to be used with all inherited tree structs
/// @brief Abstract tree implementation.

/// Abstract tree node, prefer usage of inherited structs
typedef struct node {
    Item item;
    struct node *l, *r;
} Node;

/// Pointer to tree node
typedef Node* Tree;

/// Encapsulates functions into the tree namespace
struct tree_methods {
    /// @brief Initializes an already allocated memory space
    /// @param leaf Pointer to allocated memory 
    /// @param item Item to be sowed
    /// @return Node containing item
    Tree (*init)(Tree leaf, Item item);

    /// @brief Deletes a node and its contents
    /// @param leaf Node to be deleted
    /// @return NULL pointer
    Tree (*kill)(Tree leaf);

    /// @brief Completely frees the memory occupied by a tree
    /// @param root Tree to be cleared
    /// @return NULL pointer
    Tree (*clear)(Tree root);

    /// @brief Prints all values of a tree
    /// @param root Tree to be printed
    void (*print)(Tree root);

    /// Order of execution:
    ///     -# Calls function on current node
    ///     -# Tries to access left node if possible
    ///     -# When no longer possible, resumes from previous node
    ///     -# Tries to access right node if possible
    ///     -# When no longer possible, resumes from previous node
    ///
    /// May be used to recreate a tree if needed
    /// @brief Transverses the tree and calls a function as it goes
    /// @param root Tree to be transversed
    /// @param function A function that takes a @ref Tree "tree" as a parameter and returns void
    /// @param reversed Reverse the order of execution
    void (*preorder)(Tree root, void function(Tree), int reversed);

    /// Order of execution:
    ///     -# Tries to access left node if possible
    ///     -# When no longer possible, calls function on current node
    ///     -# Tries to access right node if possible
    ///     -# When no longer possible, resumes from previous node
    ///
    /// May be used to obtain the values of a tree in order in O(n)
    /// @brief Transverses the tree and calls a function as it goes
    /// @param root Tree to be transversed
    /// @param function A function that takes a @ref Tree "tree" as a parameter and returns void
    /// @param reversed Reverse the order of execution
    void (*inorder)(Tree root, void function(Tree), int reversed);

    /// Order of execution:
    ///     -# Tries to access left node if possible
    ///     -# When no longer possible, tries to access right node
    ///     -# When no longer possible, calls function on current node
    ///     -# Resumes from previous node
    ///
    /// May be used if you need to access a node's children values before the node's own
    /// @brief Transverses the tree and calls a function as it goes
    /// @param root Tree to be transversed
    /// @param function A function that takes a @ref Tree "tree" as a parameter and returns void
    /// @param reversed Reverse the order of execution
    void (*postorder)(Tree root, void function(Tree), int reversed);

    /// @brief Searchs a tree for a node containing an @ref Item
    /// @param root Tree to be searched in
    /// @param item Item to be searched for
    /// @return Tree node containing item
    Tree (*search)(Tree root, Item item);

    /// @brief Calculates a tree's height
    /// @param root Tree to be measured
    int (*height)(Tree root);

    /// @brief Calculates number of elements in tree
    /// @param root Tree to be measured
    int (*count)(Tree root);

    /// @brief Calculates number of elements matching item
    /// @param root Tree to be measured
    /// @param item Item to be filtered out
    int (*count_filtered)(Tree root, Item item);

    /// @brief Converts a branch into an ordered array
    /// @param branch Branch to be converted
    /// @param out_count Pointer to int to be filled with number of elements in array
    /// @param reversed Reverses the order of the elements
    /// @param in_count Used when you already calculated the number of elements in tree, pass NULL to recalculate
    /// @return Array of pointers
    Tree* (*to_array)(Tree branch, int *out_count, int reversed, int *in_count);

    /// @brief Converts a branch into an array filtering by item
    /// @param branch Branch to be converted
    /// @param item Item to be filtered out
    /// @param out_count Pointer to int to be filled with number of elements in array
    /// @param in_count Used when you already calculated the number of elements in tree, pass NULL to recalculate
    /// @return Array of pointers
    Tree* (*to_array_filtered)(Tree branch, Item item, int *out_count, int *in_count);

    /// @brief Searches tree for its greatest value node
    /// @param root Tree to be searched
    /// @return Node containing the tree's greatest value
    Tree (*max)(Tree root);

    /// @brief Searches tree for its lowest value node
    /// @param root Tree to be searched
    /// @return Node containing the tree's lowest value
    Tree (*min)(Tree root);

    /// @brief Returns number of children
    /// @param node Node to be measured
    /// @return Number of children
    int (*children)(Tree node);

    /// @brief Rotates the tree structure to the left
    /// @param p Node to be rotated
    /// @return Updated tree
    Tree (*simple_rotation_left)(Tree p);

    /// @brief Rotates the tree structure to the right
    /// @param p Node to be rotated
    /// @return Updated tree
    Tree (*simple_rotation_right)(Tree p);

    /// @brief Rotates the right child to the right and then the node to the left
    /// @param p Node to be rotated
    /// @return Updated tree
    Tree (*double_rotation_left)(Tree p);

    /// @brief Rotates the left child to the left and then the node to the right
    /// @param p Node to be rotated
    /// @return Updated tree
    Tree (*double_rotation_right)(Tree p);
};

extern const struct tree_methods tree;

#endif