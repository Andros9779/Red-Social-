#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QWidget>

/**
 * @file RegisterDialog.h
 * @brief Defines a dialog for new user registration.
 */

/**
 * @class RegisterDialog
 * @brief Modal dialog prompting the user to create a new account.
 */
class RegisterDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructs the RegisterDialog.
     * @param parent Parent widget (default nullptr).
     */
    explicit RegisterDialog(QWidget* parent = nullptr);

    /**
     * @brief Destroys the RegisterDialog.
     */
    ~RegisterDialog() override;

    /**
     * @brief Returns the entered username.
     * @return Username string.
     */
    QString userName() const;

    /**
     * @brief Returns the entered password.
     * @return Password string.
     */
    QString password() const;

    /**
     * @brief Returns the entered age.
     * @return Age value.
     */
    int age() const;

    /**
     * @brief Returns the entered city.
     * @return City string.
     */
    QString city() const;

    /**
     * @brief Returns the entered tags (semicolon-separated).
     * @return Tags string.
     */
    QString tags() const;

    /**
     * @brief Returns the entered email address.
     * @return Email string.
     */
    QString email() const;

    /**
     * @brief Returns the selected profile picture path.
     * @return File path of the profile picture.
     */
    QString profilePic() const;

private slots:
    /**
     * @brief Slot called when OK button is clicked.
     */
    void onOk();

private:
    QLineEdit* nameEdit;       ///< Field for username.
    QLineEdit* emailEdit;     ///< Field for email address.
    QLineEdit* passwordEdit;   ///< Field for password.
    QLineEdit* confirmEdit;    ///< Field to confirm password.
    QSpinBox* ageSpin;         ///< Field for age.
    QLineEdit* cityEdit;       ///< Field for city.
    QLineEdit* tagsEdit;       ///< Field for tags (semicolon-separated).
    QPushButton* photoButton;    ///< Button to select a profile picture.
    QString selectedPicPath;     ///< Holds the path of the chosen profile picture.
    QPushButton* okButton;     ///< Button to accept registration.
    QPushButton* cancelButton; ///< Button to cancel.
};

#endif // REGISTERDIALOG_H
