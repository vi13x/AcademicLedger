#pragma once
#include <QDialog>

class QComboBox;
class QLineEdit;
class QLabel;
class QPushButton;

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
