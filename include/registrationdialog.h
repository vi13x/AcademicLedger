#pragma once
#include <QDialog>
#include "user.h"

class QLineEdit;

class RegistrationDialog : public QDialog {
    Q_OBJECT
public:
    RegistrationDialog(QWidget *parent = nullptr);
    User user() const;
private:
    QLineEdit *userEdit;
    QLineEdit *passEdit;
};
