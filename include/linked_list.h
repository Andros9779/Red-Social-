/**
 * @file linked_list.h
 * @brief Defines the LinkedList class for managing a singly linked list of Nodes.
 */
// === include/linked_list.h ===
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "node.h"
#include <cstddef>


/**
 * @class LinkedList
 * @brief Implements a singly linked list of integer keys using Node.
 */
class LinkedList {
private:
    Node* head;
    std::size_t len;
public:
    /**
     * @brief Constructs an empty LinkedList.
     */
    LinkedList();
    /**
     * @brief Destroys the LinkedList and frees all nodes.
     */
    ~LinkedList();

    /**
     * @brief Checks if the list contains a key.
     * @param key The key to search for.
     * @return true if the key is present, false otherwise.
     */
    bool contains(int key) const;
    /**
     * @brief Inserts a key at the front if it does not already exist.
     * @param key The key to insert.
     */
    void insert(int key);
    /**
     * @brief Removes the first occurrence of a key from the list.
     * @param key The key to remove.
     */
    void remove(int key);
    /**
     * @brief Returns the number of elements in the list.
     * @return The size of the list.
     */
    std::size_t size() const { return len; }
    /**
     * @brief Returns the head node of the list.
     * @return Pointer to the first node.
     */
    Node* begin() const { return head; }
};

#endif // LINKED_LIST_H