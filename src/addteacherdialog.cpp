#include "addteacherdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

AddTeacherDialog::AddTeacherDialog(QWidget *parent)
: QDialog(parent)
{
setWindowTitle("Добавить учителя");
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

lay->addWidget(new QLabel("Дисциплина:"));
subjectCombo = new QComboBox();
// Полный список школьных дисциплин
QStringList allSubjects = {
"Математика", "Русский язык", "Русская литература", "Английский язык", 
"Физика", "Химия", "Биология", "География", "История", "История Беларуси",
"Всемирная история", "Обществознание", "Информатика", "Физкультура и здоровье",
"Искусство", "Музыка", "ИЗО", "Трудовое обучение", "ЧЗС", "Окружающий мир",
"Белорусский язык", "Белорусская литература", "Чтение", "Классный час"
};
subjectCombo->addItems(allSubjects);
subjectCombo->setEditable(true);
subjectCombo->setInsertPolicy(QComboBox::NoInsert);
lay->addWidget(subjectCombo);

QPushButton *ok = new QPushButton("Добавить");
QPushButton *cancel = new QPushButton("Отмена");
connect(ok, &QPushButton::clicked, this, &AddTeacherDialog::accept);
connect(cancel, &QPushButton::clicked, this, &AddTeacherDialog::reject);

QHBoxLayout *btns = new QHBoxLayout();
btns->addStretch();
btns->addWidget(ok);
btns->addWidget(cancel);
lay->addLayout(btns);
}

QString AddTeacherDialog::getFullName() const { return fullNameEdit->text().trimmed(); }
QString AddTeacherDialog::getLogin() const { return loginEdit->text().trimmed(); }
QString AddTeacherDialog::getPassword() const { return passwordEdit->text(); }
QString AddTeacherDialog::getSubject() const { return subjectCombo->currentText().trimmed(); }

