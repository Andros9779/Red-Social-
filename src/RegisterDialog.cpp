#include "RegisterDialog.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>

/**
 * @file RegisterDialog.cpp
 * @brief Implements the RegisterDialog for new user registration.
 */

RegisterDialog::RegisterDialog(QWidget* parent)
    : QDialog(parent),
      nameEdit(new QLineEdit(this)),
      emailEdit(new QLineEdit(this)),
      passwordEdit(new QLineEdit(this)),
      confirmEdit(new QLineEdit(this)),
      ageSpin(new QSpinBox(this)),
      cityEdit(new QLineEdit(this)),
      tagsEdit(new QLineEdit(this)),
      photoButton(new QPushButton("Seleccionar foto...", this)),
      selectedPicPath(""),
      okButton(new QPushButton("OK", this)),
      cancelButton(new QPushButton("Cancel", this)) {
    setWindowTitle("Register");
    emailEdit->setPlaceholderText("email@example.com");
    passwordEdit->setEchoMode(QLineEdit::Password);
    confirmEdit->setEchoMode(QLineEdit::Password);
    ageSpin->setRange(0, 150);

    auto* formLayout = new QFormLayout();
    formLayout->addRow("Username:", nameEdit);
    formLayout->addRow("Email:", emailEdit);
    formLayout->addRow("Password:", passwordEdit);
    formLayout->addRow("Confirm:", confirmEdit);
    formLayout->addRow("Age:", ageSpin);
    formLayout->addRow("City:", cityEdit);
    formLayout->addRow("Tags:", tagsEdit);
    formLayout->addRow("Foto de perfil:", photoButton);

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &RegisterDialog::onOk);
    connect(cancelButton, &QPushButton::clicked, this, &RegisterDialog::reject);
    connect(photoButton, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getOpenFileName(this, "Seleccionar foto de perfil", QString(), "Images (*.png *.jpg *.bmp)");
        if (!path.isEmpty()) {
            selectedPicPath = path;
            photoButton->setText(QFileInfo(path).fileName());
        }
    });
}

RegisterDialog::~RegisterDialog() = default;

QString RegisterDialog::userName() const {
    return nameEdit->text();
}

QString RegisterDialog::password() const {
    return passwordEdit->text();
}

int RegisterDialog::age() const {
    return ageSpin->value();
}

QString RegisterDialog::city() const {
    return cityEdit->text();
}

QString RegisterDialog::tags() const {
    return tagsEdit->text();
}

QString RegisterDialog::email() const {
    return emailEdit->text();
}

void RegisterDialog::onOk() {
    const QString uname = nameEdit->text().trimmed();
    const QString pwd = passwordEdit->text();
    const QString confirm = confirmEdit->text();
    if (uname.isEmpty() || pwd.isEmpty() || confirm.isEmpty()) {
        QMessageBox::warning(this, "Registro", "Todos los campos son obligatorios");
        return;
    }
    if (pwd != confirm) {
        QMessageBox::warning(this, "Registro", "Las contraseñas no coinciden");
        return;
    }
    // Accept only if valid
    accept();
}

QString RegisterDialog::profilePic() const {
    return selectedPicPath;
}
