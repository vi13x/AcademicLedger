#include "disciplinemarkdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDate>


DisciplineMarkDialog::DisciplineMarkDialog(const QString &studentName, QWidget *parent)
: QDialog(parent)
{
setWindowTitle("Отметка за дисциплину: " + studentName);
resize(400, 250);

QVBoxLayout *lay = new QVBoxLayout(this);

lay->addWidget(new QLabel("Ученик: " + studentName));

lay->addWidget(new QLabel("Предмет:"));
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

lay->addWidget(new QLabel("Оценка за дисциплину (1-10 или н - пропуск):"));
markCombo = new QComboBox();
QStringList marks;
marks << "н"; // Пропуск
for (int i = 1; i <= 10; i++) {
marks << QString::number(i);
}
markCombo->addItems(marks);
lay->addWidget(markCombo);

lay->addWidget(new QLabel("Дата:"));
dateEdit = new QDateEdit();
dateEdit->setDate(QDate::currentDate());
dateEdit->setCalendarPopup(true);
dateEdit->setDisplayFormat("dd.MM.yyyy");
lay->addWidget(dateEdit);

QPushButton *ok = new QPushButton("Поставить");
QPushButton *cancel = new QPushButton("Отмена");
connect(ok, &QPushButton::clicked, this, &DisciplineMarkDialog::accept);
connect(cancel, &QPushButton::clicked, this, &DisciplineMarkDialog::reject);

QHBoxLayout *btns = new QHBoxLayout();
btns->addWidget(ok);
btns->addWidget(cancel);
lay->addLayout(btns);
}

QString DisciplineMarkDialog::getMark() const { return markCombo->currentText(); }
QString DisciplineMarkDialog::getSubject() const { return subjectCombo->currentText().trimmed(); }
QDate DisciplineMarkDialog::getDate() const { return dateEdit->date(); }

