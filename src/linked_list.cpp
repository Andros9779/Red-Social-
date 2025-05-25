/**
 * @file linked_list.cpp
 * @brief Implements the LinkedList class methods for singly linked list operations.
 */
// === src/linked_list.cpp ===
#include "../include/linked_list.h"
#include <iostream>

/**
 * @brief Constructs an empty LinkedList.
 */
LinkedList::LinkedList() : head(nullptr), len(0) {}

/**
 * @brief Destroys the LinkedList and deallocates all nodes.
 */
LinkedList::~LinkedList() {
    Node* cur = head;
    while (cur) {
        Node* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
}

/**
 * @brief Checks if the list contains the specified key.
 * @param key The key value to search for.
 * @return true if the key is present, false otherwise.
 */
bool LinkedList::contains(int key) const {
    for (Node* p = head; p; p = p->next)
        if (p->key == key) return true;
    return false;
}

/**
 * @brief Inserts a new key at the beginning of the list if not already present.
 * @param key The key value to insert.
 */
void LinkedList::insert(int key) {
    if (!contains(key)) {
        head = new Node(key, head);
        ++len;
    }
}

/**
 * @brief Removes the first occurrence of the specified key from the list.
 * @param key The key value to remove.
 */
void LinkedList::remove(int key) {
    Node* prev = nullptr;
    Node* cur = head;
    while (cur) {
        if (cur->key == key) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            delete cur;
            --len;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}