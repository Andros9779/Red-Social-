/**
 * @file node.h
 * @brief Defines a node for a singly linked list.
 */
#ifndef NODE_H
#define NODE_H

/**
 * @struct Node
 * @brief Represents a single element in a singly linked list.
 */
struct Node {
    int key; ///< Key value stored in the node (e.g., user ID).
    Node* next; ///< Pointer to the next node in the list.

    /**
     * @brief Constructs a Node with a key and optional next pointer.
     * @param k The key value to store.
     * @param n Optional pointer to the next node (default nullptr).
     */
    explicit Node(int k, Node* n = nullptr)
        : key(k), next(n) {}
};

#endif // NODE_H
