#ifndef ADDSTUDENTDIALOG_H
#define ADDSTUDENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

class AddStudentDialog : public QDialog {
Q_OBJECT
public:
AddStudentDialog(QWidget *parent = nullptr);
QString getFullName() const;
QString getLogin() const;
QString getPassword() const;
QString getClassName() const; // Полное название класса из списка существующих
QComboBox *classCombo; // Публичный для доступа из AdminView
private:
QLineEdit *fullNameEdit;
QLineEdit *loginEdit;
QLineEdit *passwordEdit;
};

#endif // ADDSTUDENTDIALOG_H

