#ifndef INDEX_H
#define INDEX_H

#include "bst.h"
#include "avl.h"
#include "rb.h"

/// @file
/// @brief File indexing methods

/// BST indexing tree
struct BST_Index {
    BST_Node node;
    int pos;
};

/// AVL indexing tree
struct AVL_Index {
    AVL_Node node;
    int pos;
};

/// RB indexing tree
struct RB_Index {
    RB_Node node;
    int pos;
};

/// Used to mark sections of the data file as deleted or writable
typedef struct Junk {
    int size;
    int pos;
    struct Junk *next;
} JunkNode;

/// Pointer to struct BST_Index
typedef struct BST_Index* BST_Index;
/// Pointer to struct AVL_Index
typedef struct AVL_Index* AVL_Index;
/// Pointer to struct RB_Index
typedef struct RB_Index* RB_Index;
/// Pointer to JunkNode
typedef JunkNode* Junk;

/// Encapsulates functions into the idx namespace
struct index_methods {
    /// @brief Creates and initializes a BST_Index node
    /// @param item Item to be used as key
    /// @param pos Position of registry data in bytes
    /// @return Initialized node
    BST_Index (*create_bst)(Item item, int pos);

    /// @brief Creates and initializes an AVL_Index node
    /// @param item Item to be used as key
    /// @param pos Position of registry data in bytes
    /// @return Initialized node
    AVL_Index (*create_avl)(Item item, int pos);

    /// @brief Creates and initializes a RB_Index node
    /// @param item Item to be used as key
    /// @param pos Position of registry data in bytes
    /// @return Initialized node
    RB_Index (*create_rb)(Item item, int pos);

    /// @brief Saves a BST_Index tree into a JSON file
    /// @param root Tree root
    /// @param filename File name
    /// @return -1 on failure
    int (*save_bst)(BST_Index root, char* filename);

    /// @brief Saves an AVL_Index tree into a JSON file
    /// @param root Tree root
    /// @param filename File name
    /// @return -1 on failure
    int (*save_avl)(AVL_Index root, char* filename);

    /// @brief Saves a RB_Index tree into a JSON file
    /// @param root Tree root
    /// @param filename File name
    /// @return -1 on failure
    int (*save_rb)(RB_Index root, char* filename);

    /// @brief Saves a Junk list into a JSON file
    /// @param junk List starting node
    /// @param filename File name
    /// @return -1 on failure
    int (*save_junk)(Junk junk, char* filename);

    /// @brief Loads a BST_Index tree from a JSON file
    /// @param filename File name
    /// @return Loaded tree
    BST_Index (*retrieve_bst)(char* filename);

    /// @brief Loads an AVL_Index tree from a JSON file
    /// @param filename File name
    /// @return Loaded tree
    AVL_Index (*retrieve_avl)(char* filename);

    /// @brief Loads a RB_Index tree from a JSON file
    /// @param filename File name
    /// @return Loaded tree
    RB_Index (*retrieve_rb)(char* filename);

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
};

extern const struct index_methods idx;

#endif