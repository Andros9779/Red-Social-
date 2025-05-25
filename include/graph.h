/**
 * @file graph.h
 * @brief Defines the Graph class for managing a social network of users.
 */
#ifndef GRAPH_H
#define GRAPH_H



#include "hash_table.h"
#include "user.h"
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <cstdint>

#include <nlohmann/json.hpp>

/**
 * @struct Post
 * @brief Represents a user's status update or post.
 */
struct Post {
    uint64_t userId;           ///< ID of the user who created the post.
    std::string text;          ///< Content of the post.
    std::time_t timestamp;     ///< Time when the post was created.
    int likes = 0;             ///< Number of likes on the post.
    std::vector<std::pair<std::string, std::time_t>> comments;  ///< Comments (text, timestamp).
};

/**
 * @class Graph
 * @brief Represents a social network graph with user profiles and friendships.
 */
class Graph {
private:
    std::unordered_map<uint64_t, User*> users;   // perfiles
    std::unordered_set<std::string> usernames;   // para verificar unicidad de nombre
    HashTable adj;              // userID → lista de vecinos (amistades)
    int edges;                  // cantidad de aristas no dirigidas
    uint64_t nextId;   // siguiente ID a asignar
    std::vector<Post> posts;   ///< All posts in the network.
    // Dirección de seguidores y seguidos
    std::unordered_map<uint64_t, std::vector<uint64_t>> followersMap_;  ///< Map of user → list of followers
    std::unordered_map<uint64_t, std::vector<uint64_t>> followingMap_;  ///< Map of user → list of users they follow
public:
    /**
     * @brief Constructs an empty Graph.
     */
    Graph();

    /**
     * @brief Adds an undirected friendship connection between two users.
     * @param u First user ID.
     * @param v Second user ID.
     */
    void addEdge(int u, int v);           // u ↔ v

    /**
     * @brief Checks if two users are directly connected (friends).
     * @param u First user ID.
     * @param v Second user ID.
     * @return true if users are friends, false otherwise.
     */
    bool areFriends(int u, int v) const;

    /**
     * @brief Returns the number of friends (degree) of a user.
     * @param u User ID.
     * @return Number of adjacent vertices.
     */
    int degree(int u) const;

    /**
     * @brief Retrieves the list of neighbor IDs for a user.
     * @param u User ID.
     * @return Pointer to a LinkedList of neighbor IDs.
     */
    LinkedList* neighbors(int u) const;

    /**
     * @brief Loads user profiles from a CSV file.
     * @param path Path to a CSV file with lines id,name,age,city,tags.
     */
    void loadUsersCSV(const std::string& path);

    /**
     * @brief Retrieves a user profile by ID.
     * @param id The user ID.
     * @return Pointer to the User object, or nullptr if not found.
     */
    User* getUser(int id) const;

    /**
     * @brief Adds a new user to the graph.
     * @param u The User object to add.
     * @return true if added successfully, false if username exists.
     */
    bool addUser(const User& u);

    /**
     * @brief Checks if a username is already taken.
     * @param name The username to check.
     * @return true if the username exists, false otherwise.
     */
    bool usernameExists(const std::string& name) const;

    /**
     * @brief Generates the next available user ID.
     * @return The next unused user ID.
     */
    uint64_t nextUserId();

    /**
     * @brief Serializes the graph (users and edges) to JSON.
     * @return JSON object representing the graph.
     */
    nlohmann::json toJson() const;

    /**
     * @brief Deserializes a graph from JSON.
     * @param j JSON object containing users and edges.
     * @return A Graph constructed from the JSON data.
     */
    static Graph fromJson(const nlohmann::json& j);

    /**
     * @brief Loads friendship edges from a CSV file.
     * @param path Path to a CSV file with lines u,v for each edge.
     */
    void loadCSV(const std::string& path);

    /**
     * @brief Gets the number of vertices (users) in the graph.
     * @return Number of vertices.
     */
    int numVertices() const { return adj.size(); }

    /**
     * @brief Gets the number of edges (friendships) in the graph.
     * @return Number of undirected edges.
     */
    int numEdges() const { return edges; }

    /**
     * @brief Counts connected components using BFS.
     * @return Number of connected components.
     */
    int bfsComponentCount() const;

    /**
     * @brief Computes shortest path length between two users.
     * @param src Source user ID.
     * @param dst Destination user ID.
     * @return Number of hops in shortest path or -1 if unreachable.
     */
    int shortestPath(int src, int dst) const;

    /**
     * @brief Calculates average degree (2E/V) of the graph.
     * @return Average number of friends per user.
     */
    double averageDegree() const;

    /**
     * @brief Approximates the graph diameter by sampling.
     * @param samples Number of nodes to sample.
     * @return Maximum shortest path found from samples.
     */
    int approximateDiameter(int samples = 5) const;

    /**
     * @brief Computes the average clustering coefficient.
     * @return Mean clustering coefficient across vertices.
     */
    double averageClusteringCoefficient() const;

    /**
     * @brief Retrieves all user IDs in the graph.
     * @return A vector of user IDs.
     */
    std::vector<uint64_t> getUserIds() const;

    /**
     * @brief Adds a new post for a user.
     * @param userId The ID of the user making the post.
     * @param text The content of the post.
     */
    void addPost(uint64_t userId, const std::string& text);

    /**
     * @brief Retrieves all posts made by a specific user.
     * @param userId The user ID whose posts to retrieve.
     * @return Vector of Post objects for the user.
     */
    std::vector<Post> getPostsForUser(uint64_t userId) const;

    /**
     * @brief Increments like count for a specific post.
     * @param userId ID of the user who made the post.
     * @param timestamp Timestamp of the post to like.
     */
    void likePost(uint64_t userId, std::time_t timestamp);

    /**
     * @brief Adds a comment to a specific post.
     * @param userId ID of the user who made the post.
     * @param timestamp Timestamp of the post to comment on.
     * @param text The comment text.
     */
    void addComment(uint64_t userId, std::time_t timestamp, const std::string& text);

    /**
     * @brief Retrieves the feed posts for a user (own posts and those of friends).
     * @param userId The ID of the user whose feed to get.
     * @return Vector of Post objects for the feed.
     */
    std::vector<Post> getFeed(uint64_t userId) const;

/**
 * @brief Hacer que un usuario siga a otro.
 * @param followerId ID del seguidor.
 * @param followeeId ID del usuario a seguir.
 */
void follow(uint64_t followerId, uint64_t followeeId);

/**
 * @brief Dejar de seguir a un usuario.
 * @param followerId ID del seguidor.
 * @param followeeId ID del usuario a dejar de seguir.
 */
void unfollow(uint64_t followerId, uint64_t followeeId);

/**
 * @brief Obtiene la lista de IDs de usuarios que siguen a un usuario dado.
 * @param userId El ID del usuario.
 * @return Vector de IDs de seguidores.
 */
std::vector<uint64_t> getFollowers(uint64_t userId) const;

/**
 * @brief Obtiene la lista de IDs de usuarios a los que un usuario dado sigue.
 * @param userId El ID del usuario.
 * @return Vector de IDs de usuarios seguidos.
 */
std::vector<uint64_t> getFollowing(uint64_t userId) const;

/**
 * @brief Buscar usuarios por nombre.
 * @param name El nombre parcial o completo a buscar.
 * @return Vector de pares (userId, nombre) que coinciden.
 */
std::vector<std::pair<uint64_t, std::string>> findUsersByName(const std::string& name) const;

};
#endif // GRAPH_H 