#ifndef TYPES_H
#define TYPES_H

/// @file
/// @brief Provides dynamic typing functionality and useful functions

/// Used to infer actual type of the stored data
typedef enum type {
    INT, ///< int
    FLOAT, ///< float
    DOUBLE, ///< double
    CHAR, ///< char
    STRING ///< char *
} Type;

/// Groups data with its actual type
struct Item {
    void* data; ///< void pointer to stored data
    Type type; ///< Actual type of stored data
};

typedef struct Item* Item;

/// Encapsulates functions into the types namespace
struct types_methods {
    /// @brief Compares two @ref Item "items"
    /// @return 0 if item1 == item2 @n>0 if item1 > item2 @n<0 if item1 < item2
    int (*cmp)(Item item1, Item item2);

    /// @brief Frees up the allocated memory
    /// @param Item to be destroyed
    void (*destroy)(Item item);

    /// @brief Duplicates an item
    /// @param Item item to be duplicated
    Item (*copy)(Item item);

    /// @brief Creates an item from an int
    /// @param val Integer to be wrapped
    /// @return Int item
    Item (*Int)(int val);

    /// @brief Creates an item from a float
    /// @param val Float to be wrapped
    /// @return Float item
    Item (*Float)(float val);

    /// @brief Creates an item from a double
    /// @param val Double to be wrapped
    /// @return Double item
    Item (*Double)(double val);

    /// @brief Creates an item from a char
    /// @param val Char to be wrapped
    /// @return Char item
    Item (*Char)(char val);

    /// @brief Creates an item from a string
    /// @param val String to be wrapped
    /// @return String item
    Item (*String)(char val[]);

    /// @brief Converts an item to a string item
    /// @param item Item to be converted
    /// @return Converted item
    Item (*tostring)(Item item);
};

extern const struct types_methods types;

#endif