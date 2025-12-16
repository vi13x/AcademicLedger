#include "addstudentdialog.h"
#include "datastore.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

AddStudentDialog::AddStudentDialog(QWidget *parent)
: QDialog(parent)
{
setWindowTitle("Добавить ученика");
resize(400, 250);

QVBoxLayout *lay = new QVBoxLayout(this);

lay->addWidget(new QLabel("ФИО:"));
fullNameEdit = new QLineEdit();
fullNameEdit->setPlaceholderText("Фамилия Имя Отчество");
lay->addWidget(fullNameEdit);

lay->addWidget(new QLabel("Логин:"));
loginEdit = new QLineEdit();
lay->addWidget(loginEdit);

lay->addWidget(new QLabel("Пароль:"));
passwordEdit = new QLineEdit();
passwordEdit->setEchoMode(QLineEdit::Password);
lay->addWidget(passwordEdit);

lay->addWidget(new QLabel("Класс:"));
classCombo = new QComboBox();
// Загружаем существующие классы
auto classes = DataStore().getClasses();
classCombo->addItems(classes);
if (classes.isEmpty()) {
classCombo->setPlaceholderText("Нет доступных классов");
}
lay->addWidget(classCombo);

QPushButton *ok = new QPushButton("Добавить");
QPushButton *cancel = new QPushButton("Отмена");
connect(ok, &QPushButton::clicked, this, &AddStudentDialog::accept);
connect(cancel, &QPushButton::clicked, this, &AddStudentDialog::reject);

QHBoxLayout *btns = new QHBoxLayout();
btns->addStretch();
btns->addWidget(ok);
btns->addWidget(cancel);
lay->addLayout(btns);
}

QString AddStudentDialog::getFullName() const { return fullNameEdit->text().trimmed(); }
QString AddStudentDialog::getLogin() const { return loginEdit->text().trimmed(); }
QString AddStudentDialog::getPassword() const { return passwordEdit->text(); }
QString AddStudentDialog::getClassName() const { 
return classCombo->currentText().trimmed(); 
}

