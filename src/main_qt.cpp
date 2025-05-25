/**
 * @file main_qt.cpp
 * @brief Entry point for the Qt6 GUI application (MiniSocial Qt).
 * 
 * Initializes the social graph, performs user login, and sets up
 * the main window with menus and friend suggestion functionality.
 */
#include <QtWidgets/QApplication>
//#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QRegularExpression>
#include <QPixmap>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFormLayout>
#include <QLabel>
#include "graph.h"
#include "suggester.h"
#include "user.h"
#include "LoginDialog.h"
#include "RegisterDialog.h"
#include "TimelineWidget.h"
#include <QtWidgets/qinputdialog.h>
#include <QtWidgets/qinputdialog.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <QListWidgetItem>
using namespace std;

/**
 * @brief Launches the Qt application, shows a login dialog,
 *        and displays the main window for friend suggestions.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Application exit code.
 */
int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // Load graph data from CSV files
    Graph g;
    g.loadCSV("../data/sample_edges.csv");
    g.loadUsersCSV("../data/users.csv");
    Suggester s(&g);

    // Prompt user with dedicated LoginDialog (username + password)
    // LoginDialog dlg;
    // dlg.setModal(true);
    // if (dlg.exec() != QDialog::Accepted)
    //     return 0;
    // QString uname = dlg.userName();
    // QString pwd = dlg.password();  // Captured password (to validate later)
    // int currentUser = -1;
    // for (uint64_t id : g.getUserIds()) {
    //     User* u = g.getUser(id);
    //     if (u && QString::fromStdString(u->name) == uname) {
    //         currentUser = id;
    //         break;
    //     }
    // }
    // if (currentUser == -1) {
    //     QMessageBox::critical(nullptr, "Login fallido", "Usuario no encontrado");
    //     return 0;
    // }

    // // Verify password
    // {
    //     User* u = g.getUser(currentUser);
    //     if (!u->verifyPassword(pwd.toStdString())) {
    //         QMessageBox::critical(nullptr, "Login fallido", "Contraseña incorrecta");
    //         return 0;
    //     }
    // }

    // Configure the main application window
    /*QWidget window;
    window.setWindowTitle("MiniSocial Qt");
    window.resize(400, 600); */
    QMainWindow window;
    window.setWindowTitle("MiniSocial Qt");
    window.resize(400, 600);

    // Track current logged-in user ID
    int64_t currentUser = -1;

    // Create stacked pages: Start, Home, Profile
    QStackedWidget* stacked = new QStackedWidget(&window);

    // Set up the widget to display friend suggestions
    QListWidget* list = new QListWidget(&window);
    list->setGeometry(10, 100, 380, 480);

    // Start page with Login and Register buttons
    QWidget* startPage = new QWidget(&window);
    QVBoxLayout* startLayout = new QVBoxLayout(startPage);
    startLayout->addStretch();
    QPushButton* btnLoginStart = new QPushButton("Iniciar sesión", startPage);
    QPushButton* btnRegisterStart = new QPushButton("Registrarse", startPage);
    startLayout->addWidget(btnLoginStart);
    startLayout->addWidget(btnRegisterStart);
    startLayout->addStretch();

    // Home page for suggestions
    QWidget* homePage = new QWidget(&window);
    QVBoxLayout* homeLayout = new QVBoxLayout(homePage);
    homeLayout->addWidget(list);

    // Follow user panel
    QHBoxLayout* followLayout = new QHBoxLayout();
    QLineEdit* followEdit = new QLineEdit(homePage);
    followEdit->setPlaceholderText("Enter username or ID to follow");
    QPushButton* followBtn = new QPushButton("Follow", homePage);
    followLayout->addWidget(followEdit);
    followLayout->addWidget(followBtn);
    homeLayout->addLayout(followLayout);
        // Lista de resultados de búsqueda bajo el input de seguir
    QListWidget* searchResultsList = new QListWidget(homePage);
    homeLayout->addWidget(searchResultsList);


    // Create scrollable feed area
    QScrollArea* feedArea = new QScrollArea(homePage);
    feedArea->setWidgetResizable(true);
    QWidget* feedContainer = new QWidget();
    QVBoxLayout* feedLayout = new QVBoxLayout(feedContainer);
    feedArea->setWidget(feedContainer);
    homeLayout->addWidget(feedArea);

    // Profile page (reuse or create as before)
    QWidget* profilePage = new QWidget(&window);
    QFormLayout* profileLayout = new QFormLayout(profilePage);
    QLabel* nameLabel = new QLabel(profilePage);
    QLabel* ageLabel = new QLabel(profilePage);
    QLabel* cityLabel = new QLabel(profilePage);
    QLabel* tagsLabel = new QLabel(profilePage);
    profileLayout->addRow("Name:", nameLabel);
    profileLayout->addRow("Age:", ageLabel);
    profileLayout->addRow("City:", cityLabel);
    profileLayout->addRow("Tags:", tagsLabel);
    // Profile picture placeholder (created once)
    QLabel* picLabel = new QLabel(profilePage);
    profileLayout->addRow("Foto:", picLabel);

    // Followers count and list
    QLabel* followersCountLabel = new QLabel(profilePage);
    QListWidget* followersList = new QListWidget(profilePage);
    profileLayout->addRow("Seguidores:", followersCountLabel);
    profileLayout->addRow("Lista Seguidores:", followersList);

    // Following count and list (optional)
    QLabel* followingCountLabel = new QLabel(profilePage);
    QListWidget* followingList = new QListWidget(profilePage);
    profileLayout->addRow("Siguiendo:", followingCountLabel);
    profileLayout->addRow("Lista Siguiendo:", followingList);

    // Add pages to stacked widget
    stacked->addWidget(startPage);
    stacked->addWidget(homePage);
    stacked->addWidget(profilePage);

    // Set stacked widget as central widget
    QWidget* central = new QWidget(&window);
    QVBoxLayout* centralLayout = new QVBoxLayout(central);
    centralLayout->addWidget(stacked);
    window.setCentralWidget(central);

    // Show start page initially
    stacked->setCurrentWidget(startPage);

    // Build the menu bar with Home, Profile, File, Options, and Account menus
    QMenuBar* menuBar = window.menuBar();
    // Mostrar menú dentro de la ventana en macOS
    menuBar->setNativeMenuBar(false);

    // Home
    QMenu* homeMenu = menuBar->addMenu("Home");
    QAction* homeAction = homeMenu->addAction("Home");
    QObject::connect(homeAction, &QAction::triggered, [&]() {
        // Volver a la vista principal: limpia la lista y muestra la página Home
        list->clear();
        stacked->setCurrentWidget(homePage);
    });

    // Profile
    QMenu* profileMenu = menuBar->addMenu("Profile");
    QAction* viewProfileAction = profileMenu->addAction("View Profile");
    QObject::connect(viewProfileAction, &QAction::triggered, [&]() {
        User* cur = g.getUser(currentUser);
        if (cur) {
            // Update profile picture
            QPixmap pix;
            if (!cur->profilePic.empty() && pix.load(QString::fromStdString(cur->profilePic))) {
                picLabel->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                picLabel->setText("No Image");
            }

            // Display other fields
            nameLabel->setText(QString::fromStdString(cur->name));
            ageLabel->setText(QString::number(cur->age));
            cityLabel->setText(QString::fromStdString(cur->city));
            // Display tags
            QStringList tagList;
            for (const auto& t : cur->tags) {
                tagList << QString::fromStdString(t);
            }
            tagsLabel->setText(tagList.join(";"));

            // Update followers display
            auto followerIds = g.getFollowers(currentUser);
            followersCountLabel->setText(QString::number(followerIds.size()));
            followersList->clear();
            for (auto fid : followerIds) {
                if (User* u = g.getUser(fid)) {
                    followersList->addItem(QString("%1 (%2)")
                        .arg(QString::fromStdString(u->name))
                        .arg(u->id));
                } else {
                    followersList->addItem(QString("ID %1").arg(fid));
                }
            }

            // Update following display
            auto followingIds = g.getFollowing(currentUser);
            followingCountLabel->setText(QString::number(followingIds.size()));
            followingList->clear();
            for (auto fid : followingIds) {
                if (User* u = g.getUser(fid)) {
                    followingList->addItem(QString("%1 (%2)")
                        .arg(QString::fromStdString(u->name))
                        .arg(u->id));
                } else {
                    followingList->addItem(QString("ID %1").arg(fid));
                }
            }

            stacked->setCurrentWidget(profilePage);
        }
    });

    // File (Save / Load JSON)
    QMenu* fileMenu = menuBar->addMenu("File");
    QAction* saveJsonAction = fileMenu->addAction("Save JSON");
    QObject::connect(saveJsonAction, &QAction::triggered, [&]() {
        QMessageBox::information(&window, "Guardar", "Función Save JSON aún no implementada");
    });
    QAction* loadJsonAction = fileMenu->addAction("Load JSON");
    QObject::connect(loadJsonAction, &QAction::triggered, [&]() {
        QMessageBox::information(&window, "Cargar", "Función Load JSON aún no implementada");
    });

    // Options (Stats)
    QMenu* optionsMenu = menuBar->addMenu("Options");
    QAction* statsAction = optionsMenu->addAction("Stats");
    QObject::connect(statsAction, &QAction::triggered, [&]() {
        double avgDeg = g.averageDegree();
        int diam      = g.approximateDiameter();
        double cc     = g.averageClusteringCoefficient();
        QMessageBox::information(&window, "Métricas Globales",
            QString("Grado promedio: %1\nDiámetro aprox.: %2\nClustering medio: %3")
                .arg(avgDeg)
                .arg(diam)
                .arg(cc));
    });

    // Account (Logout)
    QMenu* accountMenu = menuBar->addMenu("Account");
    QAction* logoutAction = accountMenu->addAction("Logout");
    QObject::connect(logoutAction, &QAction::triggered, [&]() {
        QApplication::quit();
    });

    // Lambda to refresh the suggestion list for given parameters
    auto refresh = [&](int user, int k, int radius){
        list->clear();
        for (int v : s.suggest(user, k, radius)) {
            User* u = g.getUser(v);
            if (u) {
                list->addItem(QString("%1 (%2) — %3 años, %4")
                    .arg(QString::fromStdString(u->name))
                    .arg(u->id)
                    .arg(u->age)
                    .arg(QString::fromStdString(u->city)));
            } else {
                list->addItem(QString("ID %1").arg(v));
            }
        }
    };

    // Connect follow button with proper brace matching and refresh call
    QObject::connect(followBtn, &QPushButton::clicked, [&]() {
        QString text = followEdit->text().trimmed();
        if (text.isEmpty()) return;
        bool ok;
        uint64_t targetId = text.toULongLong(&ok);
        if (ok) {
            g.follow(currentUser, targetId);
        } else {
            auto matches = g.findUsersByName(text.toStdString());
            if (matches.empty()) {
                QMessageBox::information(&window, "Follow", "No matching users found.");
                return;
            } else if (matches.size() == 1) {
                g.follow(currentUser, matches[0].first);
            } else {
                QStringList names;
                for (auto& p : matches)
                    names << QString("%1 (%2)").arg(QString::fromStdString(p.second)).arg(p.first);
                bool ok2;
                QString chosen = QInputDialog::getItem(&window, "Select user", "Users:", names, 0, false, &ok2);
                if (ok2 && !chosen.isEmpty()) {
                    QRegularExpression rx("\\((\\d+)\\)");
                    auto match = rx.match(chosen);
                    if (match.hasMatch()) {
                        uint64_t id = match.capturedTexts()[1].toULongLong();
                        g.follow(currentUser, id);
                    }
                }
            }
        }
        // Refresh suggestions after follow
        refresh(currentUser, /*use previous k*/5, /*previous radius*/3);
    });

    refresh(currentUser, /*use previous k*/5, /*previous radius*/3);

    // Populate search results as user types
    QObject::connect(followEdit, &QLineEdit::textChanged, [&](const QString& text) {
        searchResultsList->clear();
        QString query = text.trimmed();
        if (query.isEmpty()) return;
        auto matches = g.findUsersByName(query.toStdString());
        for (auto& p : matches) {
            QString entry = QString("%1 (%2)")
                .arg(QString::fromStdString(p.second))
                .arg(p.first);
            searchResultsList->addItem(entry);
        }
    });

    // Double-click a search result to follow that user
    QObject::connect(searchResultsList, &QListWidget::itemDoubleClicked,
                     [&](QListWidgetItem* item) {
        QRegularExpression rx("\\((\\d+)\\)");
        auto match = rx.match(item->text());
        if (match.hasMatch()) {
            uint64_t id = match.capturedTexts()[1].toULongLong();
            g.follow(currentUser, id);
            QMessageBox::information(&window, "Follow",
                                     QString("Has seguido a %1.").arg(item->text()));
            // Refresh suggestions after follow
            refresh(currentUser, /*k*/5, /*radius*/3);
        }
    });

    // Create the button to trigger friend suggestion computation
    auto* btn = new QPushButton("Run Suggest", &window);
    btn->setGeometry(150, 50, 100, 30);
    QObject::connect(btn, &QPushButton::clicked, [&]() {
        bool ok;
        int user = QInputDialog::getInt(&window, "User ID", "User ID:", 1, 1, g.numVertices(), 1, &ok);
        if (!ok) return;
        int k = QInputDialog::getInt(&window, "Top k", "Top k:", 5, 1, 20, 1, &ok);
        if (!ok) return;
        int radius = QInputDialog::getInt(&window, "Radius", "Radius:", 3, 1, 10, 1, &ok);
        if (!ok) return;
        refresh(user, k, radius);
    });

    QObject::connect(btnLoginStart, &QPushButton::clicked, [&]() {
        LoginDialog dlg(&window);
        if (dlg.exec() == QDialog::Accepted) {
            QString uname = dlg.userName();
            QString pwd = dlg.password();  // Captured password (to validate later)
            currentUser = -1;
            for (uint64_t id : g.getUserIds()) {
                User* u = g.getUser(id);
                if (u) {
                    QString qName = QString::fromStdString(u->name);
                    QString qEmail = QString::fromStdString(u->email);
                    if (qName == uname || qEmail == uname) {
                        currentUser = id;
                        break;
                    }
                }
            }
            if (currentUser == -1) {
                QMessageBox::critical(&window, "Login fallido", "Usuario no encontrado");
                return;
            }

            // Verify password
            {
                User* u = g.getUser(currentUser);
                if (!u->verifyPassword(pwd.toStdString())) {
                    QMessageBox::critical(&window, "Login fallido", "Contraseña incorrecta");
                    return;
                }
            }

            // On success, switch to home page
            stacked->setCurrentWidget(homePage);
            // Show timeline for the logged-in user
            {
                TimelineWidget* tw = new TimelineWidget(g, currentUser, feedContainer);
                feedLayout->addWidget(tw);
            }
        }
    });
    QObject::connect(btnRegisterStart, &QPushButton::clicked, [&]() {
        RegisterDialog dlg(&window);
        if (dlg.exec() == QDialog::Accepted) {
            // Collect data
            std::string uname = dlg.userName().toStdString();
            std::string pwd   = dlg.password().toStdString();
            int age           = dlg.age();
            std::string city  = dlg.city().toStdString();
            std::string email = dlg.email().toStdString();
            // Split tags by ';'
            std::vector<std::string> tags;
            QStringList tagList = dlg.tags().split(';', Qt::SkipEmptyParts);
            for (const QString& t : tagList) {
                tags.push_back(t.toStdString());
            }
            // Check username uniqueness
            if (g.usernameExists(uname)) {
                QMessageBox::warning(&window, "Registro", "El nombre de usuario ya existe.");
                return;
            }
            // Create and add user
            uint64_t newId = g.nextUserId();
            QString picPath = dlg.profilePic();
            User* newUser = new User(newId, uname, age, city, tags,
                                     email, pwd, picPath.toStdString());
            g.addUser(*newUser);
            // Persist updated graph to JSON
            {
                // Serialize graph to JSON
                nlohmann::json j = g.toJson();
                // Write to file
                std::ofstream ofs("../data/mi_red.json");
                if (!ofs) {
                    QMessageBox::warning(&window, "Guardar JSON", "No se pudo abrir el archivo mi_red.json para escritura.");
                } else {
                    ofs << j.dump(2);
                }
            }
            QMessageBox::information(&window, "Registro", "Usuario registrado con éxito.");
            // Switch to home page
            stacked->setCurrentWidget(homePage);
        }
    });

    // Show the main window and start the event loop
    window.show();
    return app.exec();
}
