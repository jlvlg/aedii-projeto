#ifndef INDEX_H
#define INDEX_H

#include "types.h"
#include "bst.h"
#include "avl.h"
#include "rb.h"

/// @file
/// @brief File indexing methods

struct Index {
    struct item item;
    int pos;
};

typedef struct Index* Index;

/// Used to mark sections of the data file as deleted or writable
typedef struct Junk {
    int size;
    int pos;
    struct Junk *next;
} JunkNode;

/// Pointer to JunkNode
typedef JunkNode* Junk;

/// Encapsulates functions into the idx namespace
struct index_methods {
    Index (*index)(Item item, int pos);
    Index (*copy)(Index index);

    /// @brief Saves a tree into a JSON file
    /// @param root Tree root
    /// @param filename File name
    /// @return -1 on failure
    int (*save_tree)(Tree root, char* filename);

    /// @brief Saves a Junk list into a JSON file
    /// @param junk List starting node
    /// @param filename File name
    /// @return -1 on failure
    int (*save_junk)(Junk junk, char* filename);

    /// @brief Loads a BST tree from a JSON file
    /// @param filename File name
    /// @return Loaded tree
    BST (*retrieve_bst)(char* filename);

    /// @brief Loads an AVL tree from a JSON file
    /// @param filename File name
    /// @return Loaded tree
    AVL (*retrieve_avl)(char* filename);

    /// @brief Loads a RB tree from a JSON file
    /// @param filename File name
    /// @return Loaded tree
    RB (*retrieve_rb)(char* filename);

    /// @brief Loads a Junk list from a JSON file
    /// @param filename File name
    /// @return Loaded list
    Junk (*retrieve_junk)(char* filename);

    /// @brief Marks a section of the data file as rewritable junk
    /// @param start Previous junk data list
    /// @param size Section size in bytes
    /// @param pos Section start in bytes
    /// @return Updated junk data list
    Junk (*dump)(Junk start, int size, int pos);

    /// @brief Retrieves the best fit position from the junk data list, updating it in the proccess
    /// @param start Pointer to junk data list
    /// @param size Size to be inserted into data file
    /// @return Position of the section in bytes
    int (*recycle)(Junk *start, int size);

    /// @brief Removes a node from the junk data list
    /// @param start Previous junk data list
    /// @param pos Position held by the node to be removed
    Junk (*remove_junk)(Junk start, int pos);

    /// @brief Restores a mistakingly recycled section of the junk data list
    /// @param start Pointer to junk data list
    /// @param size Size that was passed to recycle()
    /// @param pos Position returned by recycle()
    void (*restore_junk)(Junk *start, int size, int pos);
};

extern const struct index_methods idx;

#endif