#pragma once
#include <QWidget>
#include <QString>

class QListWidget;
class QTabWidget;
class QPushButton;

class StudentView : public QWidget {
    Q_OBJECT
public:
    StudentView(const QString &username, QWidget *parent = nullptr);
private slots:
    void onLogout();
    void onBack();
private:
    QString username;
    QTabWidget *tabs;
    QPushButton *logoutBtn;
    QPushButton *backBtn;
};
