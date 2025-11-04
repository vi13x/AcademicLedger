#include "../include/registrationdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

RegistrationDialog::RegistrationDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Регистрация (ученик)");
    QVBoxLayout *v = new QVBoxLayout(this);
    v->addWidget(new QLabel("Имя пользователя:"));
    userEdit = new QLineEdit(this);
    v->addWidget(userEdit);

    v->addWidget(new QLabel("Пароль:"));
    passEdit = new QLineEdit(this);
    passEdit->setEchoMode(QLineEdit::Password);
    v->addWidget(passEdit);

    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *ok = new QPushButton("Зарегистрироваться", this);
    QPushButton *cancel = new QPushButton("Отмена", this);
    h->addWidget(ok);
    h->addWidget(cancel);
    v->addLayout(h);

    connect(ok, &QPushButton::clicked, [this]() {
        if (userEdit->text().trimmed().isEmpty() || passEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Заполните все поля.");
            return;
        }
        accept();
    });
    connect(cancel, &QPushButton::clicked, this, &RegistrationDialog::reject);
}

User RegistrationDialog::user() const {
    User u;
    u.username = userEdit->text().trimmed();
    u.password = passEdit->text();
    u.role = "student";
    return u;
}
