#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Construye el diálogo de login.
     * @param parent Widget padre (por defecto nullptr).
     */
    explicit LoginDialog(QWidget* parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~LoginDialog() override;

    /**
     * @brief Devuelve el nombre de usuario o email ingresado.
     */
    QString userName() const;

    /**
     * @brief Devuelve la contraseña ingresada.
     * @return Cadena con la contraseña.
     */
    QString password() const;
private slots:
    void onOk();
    void reject() override;

private:
    QLineEdit* nameEdit;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QLineEdit* passwordEdit;   ///< Campo para la contraseña.
};

#endif // LOGINDIALOG_H