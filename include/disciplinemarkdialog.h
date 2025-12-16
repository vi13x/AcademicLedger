#ifndef DISCIPLINEMARKDIALOG_H
#define DISCIPLINEMARKDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QDate>

class DisciplineMarkDialog : public QDialog {
Q_OBJECT
public:
DisciplineMarkDialog(const QString &studentName, QWidget *parent = nullptr);
QString getMark() const; // Оценка от 1 до 10
QString getSubject() const;
QDate getDate() const;

private:
QComboBox *subjectCombo;
QComboBox *markCombo;
QDateEdit *dateEdit;
};

#endif // DISCIPLINEMARKDIALOG_H

