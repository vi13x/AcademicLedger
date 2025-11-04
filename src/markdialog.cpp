#include "../include/markdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>

MarkDialog::MarkDialog(const QString &className, const QString &student, QWidget *parent) : QDialog(parent) {
    setWindowTitle(QString("Поставить отметку — %1 / %2").arg(className, student));
    QVBoxLayout *v = new QVBoxLayout(this);
    v->addWidget(new QLabel("Предмет:"));
    subjectEdit = new QLineEdit(this);
    subjectEdit->setPlaceholderText("Например: Математика");
    v->addWidget(subjectEdit);

    v->addWidget(new QLabel("Оценка (2-10) или H:"));
    gradeCombo = new QComboBox(this);
    for (int i=2;i<=10;++i) gradeCombo->addItem(QString::number(i));
    gradeCombo->addItem("H");
    v->addWidget(gradeCombo);

    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *ok = new QPushButton("Поставить", this);
    QPushButton *cancel = new QPushButton("Отмена", this);
    h->addWidget(ok);
    h->addWidget(cancel);
    v->addLayout(h);

    connect(ok, &QPushButton::clicked, this, &MarkDialog::accept);
    connect(cancel, &QPushButton::clicked, this, &MarkDialog::reject);
}

QString MarkDialog::subject() const { return subjectEdit->text().trimmed(); }
QString MarkDialog::grade() const { return gradeCombo->currentText(); }
