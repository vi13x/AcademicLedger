#include "logindialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>


LoginDialog::LoginDialog(Role r, QWidget *parent)
: QDialog(parent), role(r)
{
setWindowTitle(role == Role::Student ? "Вход ученика" : "Вход учителя/админа");

QVBoxLayout *lay = new QVBoxLayout(this);

lay->addWidget(new QLabel("Логин:"));
userEdit = new QLineEdit();
lay->addWidget(userEdit);

lay->addWidget(new QLabel("Пароль:"));
passEdit = new QLineEdit();
passEdit->setEchoMode(QLineEdit::Password);
lay->addWidget(passEdit);

QPushButton *ok = new QPushButton("Войти");
QPushButton *cancel = new QPushButton("Отмена");

connect(ok, &QPushButton::clicked, this, &LoginDialog::accept);
connect(cancel, &QPushButton::clicked, this, &LoginDialog::reject);

QHBoxLayout *btns = new QHBoxLayout();
btns->addWidget(ok);
btns->addWidget(cancel);

lay->addLayout(btns);
}

QString LoginDialog::username() const { return userEdit->text().trimmed(); }
QString LoginDialog::password() const { return passEdit->text().trimmed(); }

