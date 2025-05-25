/****
 * @file user.h
 * @brief Defines the User struct representing a user profile within the social network.
 */
#ifndef USER_H
#define USER_H

#include <string>
#include <cstdint>
#include <vector>
#include <sstream>

/**
 * @struct User
 * @brief Represents a user profile with ID, name, age, city, and interest tags.
 */
struct User {
    uint64_t id;    ///< Unique identifier for the user.
    std::string name;    ///< The user's name.
    int age;    ///< The user's age.
    std::string city;    ///< The user's city of residence.
    std::vector<std::string> tags;    ///< List of interest tags for the user.
    std::string email;       ///< User account email address.
    std::string profilePic;  ///< Path to user profile picture.
    std::string password;    ///< User account password (plain text for demo purposes).

    /**
     * @brief Constructs a default User with empty fields.
     */
    User() : id(0ULL), age(0), password(""), email(""), profilePic("") {}
    /**
     * @brief Constructs a User with specified details.
     * @param _id Unique user identifier.
     * @param _name User name.
     * @param _age User age.
     * @param _city User city.
     * @param _tags Vector of interest tags.
     * @param _email User account email address.
     * @param _password User account password.
     * @param _profilePic Optional path to profile picture.
     */
    User(uint64_t _id, const std::string& _name, int _age,
         const std::string& _city, const std::vector<std::string>& _tags,
         const std::string& _email, const std::string& _password,
         const std::string& _profilePic = "")
        : id(_id),
          name(_name),
          age(_age),
          city(_city),
          tags(_tags),
          email(_email),
          profilePic(_profilePic),
          password(_password) {}

    /**
     * @brief Verifies the given password against this user's password.
     * @param pwd Password string to check.
     * @return true if the password matches, false otherwise.
     */
    bool verifyPassword(const std::string& pwd) const {
        return password == pwd;
    }

    /**
     * @brief Splits a semicolon-separated string of tags into individual tags.
     * @param tagStr A string containing tags separated by semicolons.
     * @return A vector of parsed tag strings.
     */
    static std::vector<std::string> splitTags(const std::string& tagStr) {
        std::vector<std::string> v;
        std::stringstream ss(tagStr);
        std::string tok;
        while (std::getline(ss, tok, ';')) {
            if (!tok.empty()) v.push_back(tok);
        }
        return v;
    }
};

#endif // USER_H