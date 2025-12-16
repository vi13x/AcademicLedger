#ifndef GRADEDIALOG_H
#define GRADEDIALOG_H

#include <QDialog>
#include <QComboBox>

class GradeDialog : public QDialog {
Q_OBJECT
public:
GradeDialog(QWidget *parent = nullptr);
QString getGrade() const; // Возвращает "н" или "1"-"10"
bool isEmpty() const; // true если выбрано "Очистить"
void setCurrentGrade(const QString &grade); // Установить текущую отметку
private:
QComboBox *gradeCombo;
};

#endif // GRADEDIALOG_H

