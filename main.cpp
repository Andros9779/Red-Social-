/**
 * @file main.cpp
 * @brief Entry point for the CLI version of MiniSocial: loads data, processes commands, and outputs friend suggestions.
 */
#include <iostream>
#include <string>
#include "include/graph.h"
#include "include/suggester.h"
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <fstream>

/**
 * @brief Parses command-line arguments, initializes the social graph, and enters the user command loop.
 * @param argc Number of command-line arguments.
 * @param argv Array of argument strings.
 * @return Exit code (0 on success, non-zero on error).
 */
int main(int argc, char* argv[]) {
    // --- Parse CSV path, top-k and radius arguments ---
    std::string csvPath = "data/sample_edges.csv";
    if (argc >= 2) csvPath = argv[1];

    // k (top suggestions) y radius (saltos máximos) opcionales
    int k = 5;
    int radius = 3;
    if (argc >= 3) k = std::stoi(argv[2]);
    if (argc >= 4) radius = std::stoi(argv[3]);

    // --- Load graph and user profiles from CSV files ---
    Graph g;
    try {
        g.loadCSV(csvPath);
        // Cargar perfiles de usuarios
        try {
            g.loadUsersCSV("../data/users.csv");
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "No se pudo abrir CSV (" << csvPath << "): " << ex.what() << std::endl;
        return 1;
    }

    // --- Display graph summary and command help ---
    std::cout << "Vertices: " << g.numVertices()
              << ", aristas: " << g.numEdges()
              << "  [Archivo: " << csvPath << "]"
              << "  |  top-k=" << k << "  radius=" << radius << "\n";

    Suggester s(&g);
    std::cout << "Comandos: k <valor>, radius <valor>, weights m t d, profile <id>, register, savejson <ruta>, loadjson <ruta>, export <uid> [ruta], stats (Ctrl+D para salir)\n";
    std::cout << "(Inicial k=" << k << ", radius=" << radius << ")\n";

    // --- Main command processing loop ---
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;   // EOF = salir

        // --- Command: stats (show global metrics) ---
        if (line == "stats") {
            std::cout << "Vértices: " << g.numVertices()
                      << ", Aristas: " << g.numEdges() << "\n";
            std::cout << "Grado promedio: " << g.averageDegree() << "\n";
            std::cout << "Diámetro aprox.: " << g.approximateDiameter() << "\n";
            std::cout << "Clustering medio: " << g.averageClusteringCoefficient() << "\n";
            continue;
        }

        // --- Command: savejson <path> (export graph to JSON) ---
        if (line.rfind("savejson ", 0) == 0) {
            std::string path = line.substr(9);
            try {
                auto j = g.toJson();
                std::ofstream ofs(path);
                if (!ofs) throw std::runtime_error("No se pudo abrir: " + path);
                ofs << j.dump(4);
                std::cout << "Guardado JSON en \"" << path << "\"\n";
            } catch (const std::exception& e) {
                std::cout << "Error al guardar JSON: " << e.what() << "\n";
            }
            continue;
        }

        // --- Command: loadjson <path> (import graph from JSON) ---
        if (line.rfind("loadjson ", 0) == 0) {
            std::string path = line.substr(9);
            try {
                std::ifstream ifs(path);
                if (!ifs) throw std::runtime_error("No se pudo abrir: " + path);
                nlohmann::json j;
                ifs >> j;
                g = Graph::fromJson(j);
                std::cout << "Cargado red desde JSON \"" << path << "\"\n";
            } catch (const std::exception& e) {
                std::cout << "Error al cargar JSON: " << e.what() << "\n";
            }
            continue;
        }

        // --- Command: export <uid> [path] (export suggestions to CSV) ---
        if (line.rfind("export ", 0) == 0) {
            std::stringstream ss(line.substr(7));
            int uid;
            std::string outPath;
            ss >> uid;
            if (!(ss >> outPath)) {
                outPath = "suggestions_" + std::to_string(uid) + ".csv";
            }
            auto recs = s.suggest(uid, k, radius);
            std::ofstream ofs(outPath);
            if (!ofs) {
                std::cout << "No se pudo abrir archivo: " << outPath << "\n";
            } else {
                ofs << "recommendation_id,name,age,city\n";
                for (int v : recs) {
                    User* prof = g.getUser(v);
                    std::string vName = prof ? prof->name : std::to_string(v);
                    int vAge = prof ? prof->age : 0;
                    std::string vCity = prof ? prof->city : "";
                    ofs << v << "," << vName << "," << vAge << "," << vCity << "\n";
                }
                std::cout << "Sugerencias exportadas a " << outPath << "\n";
            }
            continue;
        }

        // --- Command: radius <value> (set suggestion search radius) ---
        if (line.rfind("radius ", 0) == 0) {        // cambia radio
            try {
                radius = std::stoi(line.substr(7));
                std::cout << "Nuevo radius = " << radius << "\n";
            } catch (...) { std::cout << "Valor radius inválido\n"; }
            continue;
        }
        // --- Command: k <value> (set number of suggestions) ---
        if (line.rfind("k ", 0) == 0) {             // cambia k
            try {
                k = std::stoi(line.substr(2));
                std::cout << "Nuevo k = " << k << "\n";
            } catch (...) { std::cout << "Valor k inválido\n"; }
            continue;
        }

        // --- Command: weights <mutual> <tags> <dist> (set scoring weights) ---
        if (line.rfind("weights ", 0) == 0) {   // cambia pesos: mutuos tags dist
            std::stringstream ss(line.substr(8));
            int wm, wt, wd;
            if (ss >> wm >> wt >> wd) {
                s.setWeights(wm, wt, wd);
                std::cout << "Pesos actualizados: mutuos=" << wm
                          << ", tags=" << wt
                          << ", dist=" << wd << "\n";
            } else {
                std::cout << "Uso: weights <mutuos> <tags> <dist>\n";
            }
            continue;
        }

        // --- Command: profile <id> (show user profile) ---
        if (line.rfind("profile ", 0) == 0) {   // muestra perfil
            try {
                int pid = std::stoi(line.substr(8));
                User* uProf = g.getUser(pid);
                if (!uProf) {
                    std::cout << "Usuario " << pid << " no encontrado.\n";
                } else {
                    std::cout << "Perfil de " << uProf->name
                              << " (" << uProf->id << ")\n  Edad: "
                              << uProf->age << "\n  Ciudad: "
                              << uProf->city << "\n  Tags: ";
                    for (size_t i = 0; i < uProf->tags.size(); ++i) {
                        if (i) std::cout << ", ";
                        std::cout << uProf->tags[i];
                    }
                    std::cout << "\n";
                }
            } catch (...) {
                std::cout << "Uso: profile <id>\n";
            }
            continue;
        }

        // --- Command: register (interactive user registration) ---
        if (line == "register") {
            // --- Solicitar nombre ---
            std::string uname;
            while (true) {
                std::cout << "Nombre de usuario: ";
                if (!std::getline(std::cin, uname)) break;
                if (uname.empty()) { std::cout << "Debe ingresar un nombre.\n"; continue; }
                if (g.usernameExists(uname)) {
                    std::cout << "Ese nombre ya existe, intenta otro.\n";
                    continue;
                }
                break;
            }
            if (uname.empty()) continue;  // cancelado

            // --- Edad ---
            std::string sAge;
            int age = 0;
            while (true) {
                std::cout << "Edad: ";
                if (!std::getline(std::cin, sAge)) break;
                try { age = std::stoi(sAge); } catch (...) { age = 0; }
                if (age <= 0) { std::cout << "Edad inválida.\n"; continue; }
                break;
            }
            if (age <= 0) continue;       // cancelado

            // --- Ciudad ---
            std::string city;
            std::cout << "Ciudad: ";
            std::getline(std::cin, city);

            // --- Tags ---
            std::string tagLine;
            std::cout << "Tags (sep. por ';'): ";
            std::getline(std::cin, tagLine);
            std::vector<std::string> tags = User::splitTags(tagLine);

            // --- Contraseña ---
            std::string pwd;
            std::cout << "Contraseña: ";
            std::getline(std::cin, pwd);
            if (pwd.empty()) { std::cout << "Contraseña inválida.\n"; continue; }

            // --- Crear usuario ---
            int newId = g.nextUserId();
            // --- Email para registro ---
            std::string email;
            std::cout << "Email: ";
            std::getline(std::cin, email);
            if (email.empty()) { 
                std::cout << "Email inválido.\n"; 
                continue; 
            }
            User newUser(newId, uname, age, city, tags, email, pwd);
            if (!g.addUser(newUser)) {
                std::cout << "Error: nombre duplicado.\n";
                continue;
            }
            // Append a la base CSV
            std::ofstream f("../data/users.csv", std::ios::app);
            if (f.is_open())
                f << newId << ',' << uname << ',' << age << ',' << city  << ',' << tagLine << ',' << email << ',' << pwd << '\n';

            std::cout << "Usuario \"" << uname << "\" registrado con id " << newId << "\n";
            continue;
        }

        int uid;
        try { uid = std::stoi(line); }
        catch (...) { std::cout << "Comando desconocido\n"; continue; }

        auto recs = s.suggest(uid, k, radius);
        User* current = g.getUser(uid);
        std::string curName = current ? current->name : std::to_string(uid);

        if (recs.empty()) {
            std::cout << "No hay sugerencias para " << curName << "\n";
        } else {
            std::cout << "Sugerencias para " << curName << ":\n";
            for (int v : recs) {
                User* prof = g.getUser(v);
                std::string vName = prof ? prof->name : std::to_string(v);
                if (prof) {
                    std::cout << "  - " << vName << " (" << v << ", "
                              << prof->age << ", " << prof->city << ")\n";
                } else {
                    std::cout << "  - " << vName << " (" << v << ")\n";
                }
            }
        }
        std::cout << "---\n";
    }
    return 0;
}
