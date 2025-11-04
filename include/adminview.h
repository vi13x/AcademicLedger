#pragma once
#include <QWidget>
#include <QString>

class QPushButton;
class QListWidget;
class QTextEdit;
class QTabWidget;

class AdminView : public QWidget {
    Q_OBJECT
public:
    AdminView(const QString &username, QWidget *parent = nullptr);
private slots:
    void onLogout();
    void onAddTeacher();
    void onRemoveTeacher();
    void onAddAdmin();
    void onRemoveAdmin();
    void onLoadSchedule();
    void onSaveSchedule();
    void onGenerateSchedule();
    void onViewGrades();
    void onClearGrades();
private:
    QString username;
    QListWidget *teachersList;
    QListWidget *adminsList;
    QTextEdit *scheduleEdit;
    QTabWidget *mainTabs;
};
