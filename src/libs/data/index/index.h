#ifndef INDEX_H
#define INDEX_H

#include "bst.h"
#include "avl.h"
#include "rb.h"

struct BST_Index {
    BST_Node node;
    int pos;
};

struct AVL_Index {
    AVL_Node node;
    int pos;
};

struct RB_Index {
    RB_Node node;
    int pos;
};

typedef struct Junk {
    int size;
    int pos;
    struct Junk *next;
} JunkNode;

typedef struct BST_Index* BST_Index;
typedef struct AVL_Index* AVL_Index;
typedef struct RB_Index* RB_Index;
typedef JunkNode* Junk;

struct index_methods {
    /// @brief Saves a BST_Index tree into a JSON file
    /// @param root Tree root
    /// @param filename File name
    /// @return -1 if failure
    int (*save_bst)(BST_Index root, char* filename);

    /// @brief Saves an AVL_Index tree into a JSON file
    /// @param root Tree root
    /// @param filename File name
    /// @return -1 if failure
    int (*save_avl)(AVL_Index root, char* filename);

    /// @brief Saves a RB_Index tree into a JSON file
    /// @param root Tree root
    /// @param filename File name
    /// @return -1 if failure
    int (*save_rb)(RB_Index root, char* filename);

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