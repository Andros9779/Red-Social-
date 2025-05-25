/**
 * @file hash_table.h
 * @brief Implements a hash table mapping integer keys to LinkedList values via chaining.
 */
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "linked_list.h"
#include <climits>
#include <vector>

/**
 * @class HashTable
 * @brief A hash table storing integer keys with associated LinkedList* values.
 */
class HashTable {
private:
    int* keys;               // claves (userID)
    LinkedList** values;     // punteros a listas de vecinos
    bool* used;              // celda ocupada
    int capacity;            // tamaño del arreglo
    int sz;                  // pares almacenados
    float maxLoad;
    static const int EMPTY = INT_MIN;

    int hash(int key) const;             // hash base‑37 sencillo
    void rehash();                       // duplica capacidad
    int probe(int key) const;            // encuentra índice para clave (inserción/búsqueda)
public:
    /**
     * @brief Constructs a HashTable.
     * @param cap Initial capacity (number of buckets), default is 101.
     * @param load Maximum load factor before rehashing, default is 0.7.
     */
    explicit HashTable(int cap = 101, float load = 0.7f);
    /**
     * @brief Destroys the HashTable and frees all allocated resources.
     */
    ~HashTable();

    /**
     * @brief Retrieves the LinkedList associated with a key.
     * @param key The key to look up.
     * @return Pointer to the LinkedList of values, or nullptr if key is absent.
     */
    LinkedList* get(int key) const;
    /**
     * @brief Inserts or updates the mapping for a key.
     * @param key The key to insert.
     * @param value Pointer to the LinkedList of values for this key.
     */
    void put(int key, LinkedList* value);
    /**
     * @brief Checks existence of a key in the table.
     * @param key The key to check.
     * @return True if the key exists, false otherwise.
     */
    bool containsKey(int key) const;
    /**
     * @brief Returns the number of stored key-value pairs.
     * @return Current number of elements in the table.
     */
    int size() const { return sz; }
    /**
     * @brief Returns the current bucket array capacity.
     * @return Size of the underlying array.
     */
    int getCapacity() const { return capacity; }

    /**
     * @brief Returns all keys stored in the table.
     * @return A vector containing every stored key.
     */
    std::vector<int> keySet() const;
};

#endif // HASH_TABLE_H