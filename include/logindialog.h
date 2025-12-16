#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>

class LoginDialog : public QDialog {
Q_OBJECT
public:
enum Role { Student, TeacherOrAdmin };
LoginDialog(Role role, QWidget *parent = nullptr);
QString username() const;
QString password() const;

private:
Role role;
QLineEdit *userEdit;
QLineEdit *passEdit;
};

#endif // LOGINDIALOG_H

