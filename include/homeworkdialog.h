#ifndef HOMEWORKDIALOG_H
#define HOMEWORKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>

class HomeworkDialog : public QDialog {
Q_OBJECT
public:
HomeworkDialog(const QString &className, QWidget *parent = nullptr);
QString getSubject() const;
QString getHomework() const;
QString getDueDate() const;

private:
QLineEdit *subjectEdit;
QTextEdit *homeworkEdit;
QLineEdit *dueDateEdit;
QString className;
};

#endif // HOMEWORKDIALOG_H

