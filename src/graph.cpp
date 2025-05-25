/**
 * @file graph.cpp
 * @brief Implements the Graph class methods for managing users and their connections.
 */
#include <nlohmann/json.hpp>
#include "../include/graph.h"
#include <fstream>
#include <sstream>
#include <random>     // for sampling if needed
#include <ctime>

/**
 * @brief Computes the shortest path length (number of hops) between two users.
 * @param src Source user ID.
 * @param dst Destination user ID.
 * @return Number of edges in the shortest path, or -1 if no path exists.
 */
int Graph::shortestPath(int src, int dst) const {
    if (src == dst) return 0;

    LinkedList* listSrc = adj.get(src);
    LinkedList* listDst = adj.get(dst);
    if (!listSrc || !listDst) return -1;        // uno de los dos no existe

    std::unordered_map<int, int> dist;          // nodo → distancia
    std::queue<int> q;

    dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        LinkedList* neigh = adj.get(u);
        if (!neigh) continue;

        for (Node* p = neigh->begin(); p; p = p->next) {
            int v = p->key;
            if (!dist.count(v)) {
                dist[v] = dist[u] + 1;
                if (v == dst) return dist[v];   // encontrado
                q.push(v);
            }
        }
    }
    return -1;  // sin camino
}

/**
 * @brief Constructs an empty Graph with default initial capacity.
 */
Graph::Graph() : adj(101), edges(0) {}

/**
 * @brief Adds an undirected friendship edge between two users.
 * @param u First user ID.
 * @param v Second user ID.
 */
void Graph::addEdge(int u, int v) {
    if (u == v) return;                 // evita bucles
    LinkedList* listU = adj.get(u);
    if (!listU) {
        listU = new LinkedList();
        adj.put(u, listU);
    }
    LinkedList* listV = adj.get(v);
    if (!listV) {
        listV = new LinkedList();
        adj.put(v, listV);
    }
    bool existed = listU->contains(v);  // para no contar 2 veces
    listU->insert(v);
    listV->insert(u);
    if (!existed) ++edges;
}

/**
 * @brief Retrieves the adjacency list of a user.
 * @param u User ID.
 * @return Pointer to a LinkedList of neighbor IDs, or nullptr if none.
 */
LinkedList* Graph::neighbors(int u) const {
    return adj.get(u);
}

/**
 * @brief Checks if two users are directly connected.
 * @param u First user ID.
 * @param v Second user ID.
 * @return true if the users are friends, false otherwise.
 */
bool Graph::areFriends(int u, int v) const {
    LinkedList* n = adj.get(u);
    return n && n->contains(v);
}

/**
 * @brief Returns the number of friends (degree) of a user.
 * @param u User ID.
 * @return Number of adjacent friends.
 */
int Graph::degree(int u) const {
    LinkedList* n = adj.get(u);
    return n ? static_cast<int>(n->size()) : 0;
}

/**
 * @brief Loads friendship edges from a CSV file.
 * @param path Path to the CSV file containing lines "u,v" per edge.
 * @throws runtime_error if file cannot be opened.
 */
void Graph::loadCSV(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("No se pudo abrir CSV: " + path);

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string a, b;
        if (!std::getline(ss, a, ',')) continue;
        if (!std::getline(ss, b, ',')) continue;
        int u = std::stoi(a);
        int v = std::stoi(b);
        addEdge(u, v);
    }
}

/**
 * @brief Counts the number of connected components in the graph via BFS.
 * @return Number of connected components.
 */
int Graph::bfsComponentCount() const {
    std::vector<int> keys = adj.keySet();
    std::unordered_map<int, bool> visited;
    visited.reserve(keys.size() * 2);

    int comps = 0;
    std::queue<int> q;
    for (int start : keys) {
        if (visited[start]) continue;
        ++comps;
        visited[start] = true;
        q.push(start);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            LinkedList* neigh = adj.get(u);
            if (!neigh) continue;
            for (Node* p = neigh->begin(); p; p = p->next) {
                int v = p->key;
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
    }
    return comps;
}

/**
 * @brief Loads user profiles from a CSV file.
 * @param path Path to the CSV file with header and lines "id,name,age,city,tags".
 * @throws runtime_error if file cannot be opened.
 */
void Graph::loadUsersCSV(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("No se pudo abrir CSV usuarios: " + path);

    std::string line;
    // descarta cabecera
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string sId, name, sAge, city, tagStr;
        std::getline(ss, sId, ',');
        std::getline(ss, name, ',');
        std::getline(ss, sAge, ',');
        std::getline(ss, city, ',');
        std::getline(ss, tagStr, ',');
        std::string email;
        std::getline(ss, email, ',');
        std::string password;
        std::getline(ss, password, ',');

        int id  = std::stoi(sId);
        int age = std::stoi(sAge);
        std::vector<std::string> tags = User::splitTags(tagStr);

        users[id] = new User(id, name, age, city, tags, email, password);
        usernames.insert(name);           // registrar nombre para unicidad
    }
    // Inicializar nextId tras cargar usuarios
    {
        uint64_t maxId = 0;
        for (const auto& pair : users)
            if (pair.first > maxId) maxId = pair.first;
        nextId = maxId + 1;
    }
}

/**
 * @brief Retrieves a user profile by ID.
 * @param id The user ID.
 * @return Pointer to the User or nullptr if not found.
 */
User* Graph::getUser(int id) const {
    auto it = users.find(id);
    return (it == users.end()) ? nullptr : it->second;
}

/**
 * @brief Adds a new user to the graph if the username is unique.
 * @param u The User object to add.
 * @return true if added successfully, false otherwise.
 */
bool Graph::addUser(const User& u) {
    if (usernames.count(u.name)) return false;      // nombre ya registrado
    if (users.count(u.id)) return false;            // id ya existe (raro)
    users[u.id] = new User(u);
    usernames.insert(u.name);
    return true;
}

/**
 * @brief Checks if a username is already taken.
 * @param name The username to check.
 * @return true if the name exists, false otherwise.
 */
bool Graph::usernameExists(const std::string& name) const {
    return usernames.count(name) > 0;
}

/**
 * @brief Returns the next available unique user ID.
 * @return A new user ID.
 */
uint64_t Graph::nextUserId() {
    return nextId++;
}

/**
 * @brief Serializes the graph (users and edges) into a JSON object.
 * @return JSON representation of the graph.
 */
nlohmann::json Graph::toJson() const {
    nlohmann::json j;
    // Usuarios
    j["users"] = nlohmann::json::array();
    for (const auto& pair : users) {
        const User* u = pair.second;
        j["users"].push_back({
            {"id", u->id},
            {"name", u->name},
            {"age", u->age},
            {"city", u->city},
            {"tags", u->tags},
            {"email", u->email},
            {"password", u->password}
        });
    }
    // Aristas (solo una vez por par)
    j["edges"] = nlohmann::json::array();
    for (const auto& pair : users) {
        uint64_t u = pair.first;
        LinkedList* neigh = adj.get(static_cast<int>(u));
        if (!neigh) continue;
        for (Node* p = neigh->begin(); p; p = p->next) {
            uint64_t v = p->key;
            if (v > u) {
                j["edges"].push_back({u, v});
            }
        }
    }
    return j;
}

/**
 * @brief Constructs a Graph instance from a JSON representation.
 * @param j JSON object containing users and edges arrays.
 * @return A populated Graph.
 */
Graph Graph::fromJson(const nlohmann::json& j) {
    Graph g;
    // Usuarios
    for (const auto& uj : j.at("users")) {
        uint64_t id   = uj.at("id").get<uint64_t>();
        std::string name = uj.at("name").get<std::string>();
        int age      = uj.at("age").get<int>();
        std::string city = uj.at("city").get<std::string>();
        std::vector<std::string> tags = uj.at("tags").get<std::vector<std::string>>();
        std::string email = uj.at("email").get<std::string>();
        std::string password = uj.at("password").get<std::string>();
        User* u = new User(id, name, age, city, tags, email, password);
        g.users[id] = u;
        g.usernames.insert(name);
    }
    // Inicializar nextId interno
    {
        uint64_t maxId = 0;
        for (const auto& pair : g.users)
            if (pair.first > maxId) maxId = pair.first;
        g.nextId = maxId + 1;
    }
    // Aristas
    for (const auto& ej : j.at("edges")) {
        uint64_t u = ej.at(0).get<uint64_t>();
        uint64_t v = ej.at(1).get<uint64_t>();
        g.addEdge(static_cast<int>(u), static_cast<int>(v));
    }
    return g;
}

/**
 * @brief Calculates the average degree (2E/V) of the graph.
 * @return Average number of edges per vertex.
 */
double Graph::averageDegree() const {
    int V = numVertices();
    if (V == 0) return 0.0;
    return (2.0 * numEdges()) / V;
}

/**
 * @brief Approximates the graph diameter by sampling BFS from a subset of nodes.
 * @param samples Number of vertices to sample.
 * @return Maximum shortest-path length found.
 */
int Graph::approximateDiameter(int samples) const {
    std::vector<int> keys = adj.keySet();
    int n = static_cast<int>(keys.size());
    if (n == 0) return 0;
    int m = std::min(samples, n);
    int diam = 0;
    for (int i = 0; i < m; ++i) {
        int start = keys[i];
        // BFS para distancias desde start
        std::unordered_map<int,int> dist;
        std::queue<int> q;
        dist[start] = 0;
        q.push(start);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            LinkedList* neigh = adj.get(u);
            if (!neigh) continue;
            for (Node* p = neigh->begin(); p; p = p->next) {
                int v = p->key;
                if (!dist.count(v)) {
                    dist[v] = dist[u] + 1;
                    q.push(v);
                    diam = std::max(diam, dist[v]);
                }
            }
        }
    }
    return diam;
}

/**
 * @brief Computes the average clustering coefficient of the graph.
 * @return Mean clustering coefficient across all vertices.
 */
double Graph::averageClusteringCoefficient() const {
    std::vector<int> keys = adj.keySet();
    double sumC = 0.0;
    int count = 0;
    for (int u : keys) {
        LinkedList* neigh = adj.get(u);
        if (!neigh) continue;
        std::vector<int> vs;
        for (Node* p = neigh->begin(); p; p = p->next)
            vs.push_back(p->key);
        int k = static_cast<int>(vs.size());
        if (k < 2) continue;
        int links = 0;
        for (int i = 0; i < k; ++i) {
            for (int j = i + 1; j < k; ++j) {
                if (areFriends(vs[i], vs[j])) ++links;
            }
        }
        double possible = k * (k - 1) / 2.0;
        sumC += (links / possible);
        ++count;
    }
    return (count > 0) ? (sumC / count) : 0.0;
}

#include <vector>

/**
 * @brief Retrieves all user IDs present in the graph.
 * @return A vector of user IDs.
 */
std::vector<uint64_t> Graph::getUserIds() const {
    std::vector<uint64_t> ids;
    ids.reserve(users.size());
    for (const auto& kv : users) {
        ids.push_back(kv.first);
    }
    return ids;
}

/**
 * @brief Adds a new post for a user.
 * @param userId The ID of the user making the post.
 * @param text The content of the post.
 */
void Graph::addPost(uint64_t userId, const std::string& text) {
    Post p;
    p.userId = userId;
    p.text = text;
    p.timestamp = std::time(nullptr);
    posts.push_back(std::move(p));
}

/**
 * @brief Retrieves all posts made by a specific user.
 * @param userId The user ID whose posts to retrieve.
 * @return Vector of Post objects for the user.
 */

std::vector<Post> Graph::getPostsForUser(uint64_t userId) const {
    std::vector<Post> result;
    for (const auto& p : posts) {
        if (p.userId == userId) {
            result.push_back(p);
        }
    }
    return result;
}

/**
 * @brief Increments like count for a specific post.
 * @param userId ID of the user who made the post.
 * @param timestamp Timestamp of the post to like.
 */
void Graph::likePost(uint64_t userId, std::time_t timestamp) {
    for (auto& p : posts) {
        if (p.userId == userId && p.timestamp == timestamp) {
            ++p.likes;
            return;
        }
    }
}

/**
 * @brief Adds a comment to a specific post.
 * @param userId ID of the user who made the post.
 * @param timestamp Timestamp of the post to comment on.
 * @param text The comment text.
 */

void Graph::addComment(uint64_t userId, std::time_t timestamp, const std::string& text) {
    for (auto& p : posts) {
        if (p.userId == userId && p.timestamp == timestamp) {
            p.comments.emplace_back(text, std::time(nullptr));
            return;
        }
    }
}

/**
 * @brief Retrieves the feed posts for a user (own posts and those of friends).
 * @param userId The ID of the user whose feed to get.
 * @return Vector of Post objects for the feed.
 */

std::vector<Post> Graph::getFeed(uint64_t userId) const {
    std::vector<Post> feed;
    // Add own posts
    for (const auto& p : posts) {
        if (p.userId == userId) {
            feed.push_back(p);
        }
    }
    // Add posts from direct friends
    LinkedList* neigh = adj.get(static_cast<int>(userId));
    if (neigh) {
        for (Node* n = neigh->begin(); n; n = n->next) {
            uint64_t friendId = n->key;
            for (const auto& p : posts) {
                if (p.userId == friendId) {
                    feed.push_back(p);
                }
            }
        }
    }
    return feed;
}

// For std::remove, std::find, etc.
#include <algorithm>

/**
 * @brief Makes one user follow another (directed).
 * @param followerId ID of the follower.
 * @param followeeId ID of the user to follow.
 */
void Graph::follow(uint64_t followerId, uint64_t followeeId) {
    if (followerId == followeeId) return;
    // Add to following map if not already present
    auto& followList = followingMap_[followerId];
    if (std::find(followList.begin(), followList.end(), followeeId) == followList.end()) {
        followList.push_back(followeeId);
        // Also update followers map
        followersMap_[followeeId].push_back(followerId);
    }
}

/**
 * @brief Makes one user unfollow another (directed).
 * @param followerId ID of the follower.
 * @param followeeId ID of the user to unfollow.
 */
void Graph::unfollow(uint64_t followerId, uint64_t followeeId) {
    auto itF = followingMap_.find(followerId);
    if (itF != followingMap_.end()) {
        auto& followList = itF->second;
        followList.erase(std::remove(followList.begin(), followList.end(), followeeId), followList.end());
    }
    auto itR = followersMap_.find(followeeId);
    if (itR != followersMap_.end()) {
        auto& revList = itR->second;
        revList.erase(std::remove(revList.begin(), revList.end(), followerId), revList.end());
    }
}

/**
 * @brief Retrieves the list of followers for a user.
 * @param userId User ID whose followers to get.
 * @return Vector of follower user IDs.
 */
std::vector<uint64_t> Graph::getFollowers(uint64_t userId) const {
    auto it = followersMap_.find(userId);
    if (it == followersMap_.end()) return {};
    return it->second;
}

/**
 * @brief Retrieves the list of users a given user is following.
 * @param userId User ID whose followees to get.
 * @return Vector of followed user IDs.
 */
std::vector<uint64_t> Graph::getFollowing(uint64_t userId) const {
    auto it = followingMap_.find(userId);
    if (it == followingMap_.end()) return {};
    return it->second;
}

/**
 * @brief Finds users whose names contain the given substring (case-insensitive).
 * @param name Partial or full username to search.
 * @return Vector of pairs (userId, userName) matching the query.
 */
std::vector<std::pair<uint64_t, std::string>> Graph::findUsersByName(const std::string& name) const {
    std::vector<std::pair<uint64_t, std::string>> results;
    std::string lower = name;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    for (const auto& kv : users) {
        const User* u = kv.second;
        std::string uname = u->name;
        std::string unameLower = uname;
        std::transform(unameLower.begin(), unameLower.end(), unameLower.begin(), ::tolower);
        if (unameLower.find(lower) != std::string::npos) {
            results.emplace_back(u->id, uname);
        }
    }
    return results;
}
