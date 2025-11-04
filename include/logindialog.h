#pragma once
#include <QDialog>

class QLineEdit;
class QLabel;
class QPushButton;

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    enum class Role { Student, TeacherOrAdmin };
    LoginDialog(Role r, QWidget *parent = nullptr);

    QString username() const;
    QString password() const;

private:
    QLineEdit *userEdit;
    QLineEdit *passEdit;
    Role role;
};
