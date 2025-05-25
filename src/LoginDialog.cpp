#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent),
      nameEdit(new QLineEdit(this)),
      okButton(new QPushButton("OK", this)),
      cancelButton(new QPushButton("Cancel", this)) {
    // Placeholder indicates both username or email
    nameEdit->setPlaceholderText("Usuario o email");
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    // Set up the layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    mainLayout->addWidget(nameEdit);
    mainLayout->addWidget(passwordEdit);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Connect signals to slots
    connect(okButton, &QPushButton::clicked, this, &LoginDialog::onOk);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::reject);

    setLayout(mainLayout);
    setWindowTitle("Login");
}

/**
 * @brief Devuelve el nombre ingresado.
 * @return Nombre de usuario.
 */
QString LoginDialog::userName() const {
    return nameEdit->text();
}

/**
 * @brief Devuelve la contraseña ingresada.
 * @return Cadena con la contraseña.
 */
QString LoginDialog::password() const {
    return passwordEdit->text();
}

void LoginDialog::onOk() {
    if (!nameEdit->text().isEmpty()) {
        accept(); // Close the dialog and return QDialog::Accepted
    } else {
        nameEdit->setPlaceholderText("Por favor ingresa usuario o email");
    }
}

/**
 * @brief Destructor.
 */
LoginDialog::~LoginDialog() { }

/**
 * @brief Override de reject para manejar “Cancelar”.
 */
void LoginDialog::reject() {
    QDialog::reject();
}