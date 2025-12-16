#ifndef ADDTEACHERDIALOG_H
#define ADDTEACHERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

class AddTeacherDialog : public QDialog {
Q_OBJECT
public:
AddTeacherDialog(QWidget *parent = nullptr);
QString getFullName() const;
QString getLogin() const;
QString getPassword() const;
QString getSubject() const;
private:
QLineEdit *fullNameEdit;
QLineEdit *loginEdit;
QLineEdit *passwordEdit;
QComboBox *subjectCombo;
};

#endif // ADDTEACHERDIALOG_H

