#ifndef MARKDIALOG_H
#define MARKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

class MarkDialog : public QDialog {
Q_OBJECT
public:
MarkDialog(const QString &className, const QString &student, QWidget *parent = nullptr);
QString subject() const;
QString grade() const;

private:
QLineEdit *subjectEdit;
QComboBox *gradeCombo;
};

#endif // MARKDIALOG_H

