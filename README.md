# Red Social — Proyecto Final

## Project Overview

Red Social is a basic social network simulation implemented in C++ using custom data structures. It models users and their friendships, allowing operations such as loading data from CSV files, searching paths between users, recommending friends based on mutual connections, and efficient data storage using linked lists, hash tables, AVL trees, and graphs.

## Installation

To build the project, you need CMake (version 3.10 or higher) and a C++17 compatible compiler.

```bash
git clone <repository-url>
cd red-social
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

### Command Line Interface (CLI)

The executable `red_social` provides a command line interface to interact with the social network.

Example commands:

- Load data from CSV:
  ```
  load data/sample_edges.csv
  ```
- Find shortest path between users:
  ```
  shortest_path 1 4
  ```
- Get friend suggestions:
  ```
  suggest 1 5
  ```
- Exit:
  ```
  exit
  ```

### Graphical User Interface (GUI)

*Note: GUI support is planned for future releases.*

## File Structure

```
red-social/
├── include/              # Header files (interfaces for structures and classes)
│   ├── node.h            # Generic node for linked lists
│   ├── linked_list.h     # Linked list with iterator
│   ├── hash_table.h      # Hash table with integer keys (base-37 + linear probing)
│   ├── graph.h           # Graph: adjacency lists, CSV loading, BFS, shortest path
│   ├── suggester.h       # Friend suggestion and quicksort ordering
│   ├── avl_tree.h        # AVL tree for balanced suggestions
│   └── user.h            # User class with id, name, age, and city
├── src/                  # Implementation files
│   ├── node.cpp
│   ├── linked_list.cpp
│   ├── hash_table.cpp
│   ├── graph.cpp
│   ├── suggester.cpp
│   └── avl_tree.cpp      # (if applicable, or all in header if template)
├── tests/                # Unit tests (test_hash.cpp, test_graph.cpp)
├── data/
│   └── sample_edges.csv  # Input data in u,v per line format
├── main.cpp              # Entry point, CLI interface
└── CMakeLists.txt        # CMake configuration for building the project
```

## Contributing

Contributions are welcome! Please fork the repository and submit pull requests with clear descriptions. Ensure your code follows the existing style and includes appropriate tests.

## License

This project is licensed under the MIT License. See the LICENSE file for details.


/**
 * @file node.h
 * @brief Generic node for linked lists.
 */

/**
 * @class Node
 * @brief Represents a node in a linked list holding data of type T.
 */
template<class T>
class Node {
public:
    /**
     * @brief Constructs a Node with given data and optional next pointer.
     * @param data Data to store in the node.
     * @param next Pointer to the next node (default nullptr).
     */
    Node(const T& data, Node<T>* next = nullptr);

    /**
     * @brief Gets the data stored in the node.
     * @return Reference to the data.
     */
    T& getData();

    /**
     * @brief Gets the next node pointer.
     * @return Pointer to the next node.
     */
    Node<T>* getNext();

    /**
     * @brief Sets the next node pointer.
     * @param next Pointer to the next node.
     */
    void setNext(Node<T>* next);

private:
    T data;
    Node<T>* next;
};


/**
 * @file linked_list.h
 * @brief Implementation of a singly linked list with iterator support.
 */

/**
 * @class LinkedList
 * @brief Template class for singly linked list.
 */
template<class T>
class LinkedList {
public:
    /**
     * @brief Constructs an empty linked list.
     */
    LinkedList();

    /**
     * @brief Destructor to clean up all nodes.
     */
    ~LinkedList();

    /**
     * @brief Adds an element at the front of the list.
     * @param value Value to add.
     */
    void push_front(const T& value);

    /**
     * @brief Adds an element at the back of the list.
     * @param value Value to add.
     */
    void push_back(const T& value);

    /**
     * @brief Removes the first element of the list.
     */
    void pop_front();

    /**
     * @brief Checks if the list is empty.
     * @return True if empty, false otherwise.
     */
    bool empty() const;

    /**
     * @brief Returns the size of the list.
     * @return Number of elements in the list.
     */
    size_t size() const;

    /**
     * @brief Clears all elements from the list.
     */
    void clear();

    /**
     * @brief Iterator class to traverse the list.
     */
    class Iterator {
    public:
        /**
         * @brief Constructs an iterator starting at given node.
         * @param node Pointer to the starting node.
         */
        Iterator(Node<T>* node);

        /**
         * @brief Dereference operator.
         * @return Reference to the data at current node.
         */
        T& operator*() const;

        /**
         * @brief Pre-increment operator.
         * @return Reference to the iterator after increment.
         */
        Iterator& operator++();

        /**
         * @brief Equality comparison.
         * @param other Another iterator.
         * @return True if both iterators point to the same node.
         */
        bool operator==(const Iterator& other) const;

        /**
         * @brief Inequality comparison.
         * @param other Another iterator.
         * @return True if iterators point to different nodes.
         */
        bool operator!=(const Iterator& other) const;

    private:
        Node<T>* current;
    };

    /**
     * @brief Returns an iterator to the beginning of the list.
     * @return Iterator at first element.
     */
    Iterator begin() const;

    /**
     * @brief Returns an iterator to the end of the list (nullptr).
     * @return Iterator at end.
     */
    Iterator end() const;

private:
    Node<T>* head;
    size_t count;
};


/**
 * @file hash_table.h
 * @brief Hash table implementation using base-37 hashing and linear probing.
 */

/**
 * @class HashTable
 * @brief Template hash table with key type K and value type V.
 */
template<typename K, typename V>
class HashTable {
public:
    /**
     * @brief Constructs a hash table with given initial capacity.
     * @param capacity Initial size of the hash table.
     */
    HashTable(size_t capacity = 101);

    /**
     * @brief Destructor to free allocated memory.
     */
    ~HashTable();

    /**
     * @brief Inserts or updates a key-value pair.
     * @param key Key to insert.
     * @param value Value to associate with the key.
     */
    void insert(const K& key, const V& value);

    /**
     * @brief Removes a key from the table.
     * @param key Key to remove.
     */
    void remove(const K& key);

    /**
     * @brief Finds the value associated with a key.
     * @param key Key to find.
     * @return Pointer to the value if found, nullptr otherwise.
     */
    V* find(const K& key);

    /**
     * @brief Checks if the table contains a key.
     * @param key Key to check.
     * @return True if key exists, false otherwise.
     */
    bool contains(const K& key) const;

    /**
     * @brief Clears all entries in the hash table.
     */
    void clear();

private:
    size_t hashFunction(const K& key) const;
    size_t capacity;
    size_t size;
    struct Entry {
        K key;
        V value;
        bool occupied;
        bool deleted;
    };
    Entry* table;
};


/**
 * @file graph.h
 * @brief Graph data structure representing users and friendships.
 */

/**
 * @class Graph
 * @brief Undirected graph implemented as a hash table of adjacency lists.
 */
class Graph {
public:
    /**
     * @brief Constructs an empty graph.
     */
    Graph();

    /**
     * @brief Adds an edge (friendship) between two users.
     * @param u First user ID.
     * @param v Second user ID.
     */
    void addEdge(int u, int v);

    /**
     * @brief Loads graph edges from a CSV file.
     * @param path Path to CSV file.
     * @return True if loading was successful, false otherwise.
     */
    bool loadCSV(const std::string& path);

    /**
     * @brief Performs breadth-first search starting from given user.
     * @param start User ID to start BFS.
     * @return List of visited user IDs.
     */
    std::vector<int> bfs(int start) const;

    /**
     * @brief Finds the shortest path length between two users.
     * @param src Source user ID.
     * @param dst Destination user ID.
     * @return Number of steps in shortest path or -1 if no path exists.
     */
    int shortestPath(int src, int dst) const;

    /**
     * @brief Counts the number of disconnected components in the graph.
     * @return Number of connected components.
     */
    int bfsComponentCount() const;

private:
    HashTable<int, LinkedList<int>> adjacency;
};


/**
 * @file suggester.h
 * @brief Friend suggestion system based on mutual connections.
 */

/**
 * @class Suggester
 * @brief Provides friend suggestions for users based on neighbors of distance 2.
 */
class Suggester {
public:
    /**
     * @brief Suggests up to k friends for a given user, ordered by popularity.
     * @param user User ID to suggest friends for.
     * @param k Maximum number of suggestions.
     * @return List of suggested user IDs.
     */
    std::vector<int> suggest(int user, int k);
};


/**
 * @file avl_tree.h
 * @brief AVL tree implementation for balanced storage of suggestions.
 */

/**
 * @class AVLTree
 * @brief Template AVL tree for storing elements with automatic balancing.
 */
template<class T>
class AVLTree {
public:
    /**
     * @brief Inserts an element into the AVL tree.
     * @param value Element to insert.
     */
    void insert(const T& value);

    /**
     * @brief Removes an element from the AVL tree.
     * @param value Element to remove.
     */
    void remove(const T& value);

    /**
     * @brief Balances the AVL tree at a given node.
     * @param node Node to balance.
     * @return Pointer to the balanced node.
     */
    Node<T>* balance(Node<T>* node);

private:
    Node<T>* root;
};


/**
 * @file user.h
 * @brief User class representing a social network user.
 */

/**
 * @class User
 * @brief Represents a user with ID, name, age, and city.
 */
class User {
public:
    /**
     * @brief Constructs a User with given parameters.
     * @param id Unique user identifier.
     * @param name User's name.
     * @param age User's age.
     * @param city User's city.
     */
    User(int id, const std::string& name, int age, const std::string& city);

    /**
     * @brief Gets the user ID.
     * @return User ID.
     */
    int getId() const;

    /**
     * @brief Gets the user's name.
     * @return User's name.
     */
    std::string getName() const;

    /**
     * @brief Gets the user's age.
     * @return User's age.
     */
    int getAge() const;

    /**
     * @brief Gets the user's city.
     * @return User's city.
     */
    std::string getCity() const;

private:
    int id;
    std::string name;
    int age;
    std::string city;
};
