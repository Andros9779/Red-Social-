/**
 * @file avl_tree.h
 * @brief Defines the AVLTree class and AVLNode structure for maintaining a balanced binary search tree of user scores.
 */
// === include/avl_tree.h ===
#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <vector>

/**
 * @struct AVLNode
 * @brief Represents a node in an AVL tree, storing a user ID and score with balance information.
 */
struct AVLNode {
    int user;    ///< Candidate user ID.
    int score;   ///< Score value (e.g., number of mutual friends).
    int height;  ///< Height of the subtree rooted at this node.
    AVLNode* left;  ///< Pointer to the left child.
    AVLNode* right; ///< Pointer to the right child.
    /**
     * @brief Constructs an AVLNode with the given user ID and score.
     * @param u User ID.
     * @param s Initial score for the user.
     */
    AVLNode(int u, int s)
        : user(u), score(s), height(1), left(nullptr), right(nullptr) {}
};

/**
 * @class AVLTree
 * @brief Implements a balanced AVL tree to store user scores and retrieve the top-k highest scores.
 */
class AVLTree {
private:
    AVLNode* root;

    // utilidades internas
    int h(AVLNode* n) const { return n ? n->height : 0; }
    int balance(AVLNode* n) const { return n ? h(n->left) - h(n->right) : 0; }

    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);

    bool isLess(int s1, int u1, int s2, int u2) const; // orden ascendente por score, luego ID

    AVLNode* insert(AVLNode* node, int user, int score);

    void inorderDesc(AVLNode* node, int k, int& count, std::vector<int>& out) const;
    void destroy(AVLNode* node);
public:
    /**
     * @brief Constructs an empty AVLTree.
     */
    AVLTree();
    /**
     * @brief Destroys the AVLTree and frees all nodes.
     */
    ~AVLTree();

    /**
     * @brief Inserts or updates a user and its score into the AVL tree.
     * @param user The user ID.
     * @param score The score associated with the user.
     */
    void insert(int user, int score);
    /**
     * @brief Returns the top k users with highest scores in descending order.
     * @param k Number of top users to retrieve.
     * @return Vector of user IDs sorted by score from highest to lowest.
     */
    std::vector<int> topK(int k) const;           // k mayores puntajes
};

#endif // AVL_TREE_H