#include "homeworkdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>


HomeworkDialog::HomeworkDialog(const QString &className, QWidget *parent)
: QDialog(parent), className(className)
{
setWindowTitle("Добавить домашнее задание - " + className);
resize(500, 400);

QVBoxLayout *lay = new QVBoxLayout(this);

lay->addWidget(new QLabel("Предмет:"));
subjectEdit = new QLineEdit();
lay->addWidget(subjectEdit);

lay->addWidget(new QLabel("Домашнее задание:"));
homeworkEdit = new QTextEdit();
lay->addWidget(homeworkEdit);

lay->addWidget(new QLabel("Срок сдачи (дд.мм.гггг):"));
dueDateEdit = new QLineEdit();
dueDateEdit->setPlaceholderText("Например: 25.12.2024");
lay->addWidget(dueDateEdit);

QPushButton *ok = new QPushButton("Добавить");
QPushButton *cancel = new QPushButton("Отмена");
connect(ok, &QPushButton::clicked, this, &HomeworkDialog::accept);
connect(cancel, &QPushButton::clicked, this, &HomeworkDialog::reject);

QHBoxLayout *btns = new QHBoxLayout();
btns->addWidget(ok);
btns->addWidget(cancel);
lay->addLayout(btns);
}

QString HomeworkDialog::getSubject() const { return subjectEdit->text().trimmed(); }
QString HomeworkDialog::getHomework() const { return homeworkEdit->toPlainText().trimmed(); }
QString HomeworkDialog::getDueDate() const { return dueDateEdit->text().trimmed(); }

