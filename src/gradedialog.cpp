#include "gradedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVariant>

GradeDialog::GradeDialog(QWidget *parent)
: QDialog(parent)
{
setWindowTitle("Выбор отметки");
resize(300, 150);

QVBoxLayout *lay = new QVBoxLayout(this);

lay->addWidget(new QLabel("Выберите отметку:"));
gradeCombo = new QComboBox();
gradeCombo->addItem("Очистить", ""); // Опция для удаления отметки
gradeCombo->addItem("н (пропуск)", "н");
for (int i = 1; i <= 10; i++) {
gradeCombo->addItem(QString::number(i), QString::number(i));
}
lay->addWidget(gradeCombo);

QPushButton *ok = new QPushButton("OK");
QPushButton *cancel = new QPushButton("Отмена");
connect(ok, &QPushButton::clicked, this, &GradeDialog::accept);
connect(cancel, &QPushButton::clicked, this, &GradeDialog::reject);

QHBoxLayout *btns = new QHBoxLayout();
btns->addStretch();
btns->addWidget(ok);
btns->addWidget(cancel);
lay->addLayout(btns);
}

QString GradeDialog::getGrade() const {
QVariant data = gradeCombo->currentData();
if (data.isValid() && !data.toString().isEmpty()) {
return data.toString();
}
return gradeCombo->currentText().split(' ').first(); // Возвращаем "н" или число
}

bool GradeDialog::isEmpty() const {
QVariant data = gradeCombo->currentData();
return !data.isValid() || data.toString().isEmpty();
}

void GradeDialog::setCurrentGrade(const QString &grade) {
if (grade.isEmpty()) {
gradeCombo->setCurrentIndex(0); // "Очистить"
} else if (grade == "н" || grade == "Н") {
gradeCombo->setCurrentIndex(1); // "н (пропуск)"
} else {
bool ok;
int gradeNum = grade.toInt(&ok);
if (ok && gradeNum >= 1 && gradeNum <= 10) {
gradeCombo->setCurrentIndex(gradeNum + 1); // +1 потому что есть "Очистить" и "н"
}
}
}

