#ifndef INDEX_H
#define INDEX_H

#include "types.h"
#include "bst.h"
#include "avl.h"
#include "rb.h"

/// @file
/// @brief File indexing methods

/// Ties a key (item) to a absolute byte position (pos)
struct Index {
    struct Item item;
    int pos;
};

typedef struct Index* Index;

/// Used to mark sections of the data file as deleted or writable
typedef struct Garbage {
    int start;
    int end;
    struct Garbage *next;
} GarbageNode;

/// Pointer to JunkNode
typedef GarbageNode* Garbage;

/// Encapsulates functions into the idx namespace
struct index_methods {
    Index (*index)(Item *item, int pos);
    Index (*copy)(Index index);

    /// @brief Saves a tree into a JSON file
    /// @param root Tree root
    /// @param filename File name
    /// @return -1 on failure
    int (*save_tree)(Tree root, char* filename);

    /// @brief Saves a Garbage list into a JSON file
    /// @param garbage List starting node
    /// @param filename File name
    /// @return -1 on failure
    int (*save_garbage)(Garbage garbage, char* filename);

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

    /// @brief Loads a Garbage list from a JSON file
    /// @param filename File name
    /// @return Loaded list
    Garbage (*retrieve_garbage)(char* filename);

    /// @brief Marks a section of the data file as rewritable garbage
    /// @param node Previous garbage data list
    /// @param size Section size in bytes
    /// @param pos Section start in bytes
    /// @return Updated garbage data list
    void (*dump)(Garbage *node, int size, int pos);

    /// @brief Retrieves the worst fit position from the garbage data list, updating it in the proccess
    /// @param node Pointer to garbage data list
    /// @param size Size to be inserted into data file
    /// @return Position of the section in bytes
    int (*recycle)(Garbage *node, int size);

    /// @brief Clears a garbage data list
    /// @param node Starting node
    /// @return NULL pointer
    Garbage (*destroy_garbage)(Garbage node);
};

extern const struct index_methods idx;

#endif