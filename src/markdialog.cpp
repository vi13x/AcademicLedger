#include "markdialog.h"
#include "datastore.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>


MarkDialog::MarkDialog(const QString &, const QString &, QWidget *parent)
: QDialog(parent)
{
setWindowTitle("Выставление оценки");

QVBoxLayout *lay = new QVBoxLayout(this);

lay->addWidget(new QLabel("Предмет:"));
subjectEdit = new QLineEdit();
// Добавляем подсказку с доступными предметами
DataStore ds;
auto subjects = ds.getSubjects();
if (!subjects.isEmpty()) {
subjectEdit->setPlaceholderText("Например: " + subjects.first());
}
lay->addWidget(subjectEdit);

lay->addWidget(new QLabel("Оценка:"));
gradeCombo = new QComboBox();
gradeCombo->addItems({"5", "4", "3", "2"});
lay->addWidget(gradeCombo);

QPushButton *ok = new QPushButton("OK");
QPushButton *cancel = new QPushButton("Отмена");

connect(ok, &QPushButton::clicked, this, &MarkDialog::accept);
connect(cancel, &QPushButton::clicked, this, &MarkDialog::reject);

QHBoxLayout *h = new QHBoxLayout();
h->addWidget(ok);
h->addWidget(cancel);

lay->addLayout(h);
}

QString MarkDialog::subject() const { return subjectEdit->text().trimmed(); }
QString MarkDialog::grade() const { return gradeCombo->currentText(); }

