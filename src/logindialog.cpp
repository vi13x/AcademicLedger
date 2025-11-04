#include "../include/logindialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

LoginDialog::LoginDialog(Role r, QWidget *parent) : QDialog(parent), role(r) {
    setWindowTitle(role==Role::Student ? "Вход (Ученик)" : "Вход (Учитель/Админ)");
    setModal(true);
    QVBoxLayout *v = new QVBoxLayout(this);
    v->addWidget(new QLabel("Имя пользователя:"));
    userEdit = new QLineEdit(this);
    v->addWidget(userEdit);

    v->addWidget(new QLabel("Пароль:"));
    passEdit = new QLineEdit(this);
    passEdit->setEchoMode(QLineEdit::Password);
    v->addWidget(passEdit);

    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *ok = new QPushButton("Войти", this);
    QPushButton *cancel = new QPushButton("Отмена", this);
    h->addWidget(ok);
    h->addWidget(cancel);
    v->addLayout(h);

    connect(ok, &QPushButton::clicked, this, &LoginDialog::accept);
    connect(cancel, &QPushButton::clicked, this, &LoginDialog::reject);
}

QString LoginDialog::username() const { return userEdit->text().trimmed(); }
QString LoginDialog::password() const { return passEdit->text(); }
