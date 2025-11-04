#pragma once
#include <QWidget>
#include <QString>

class QListWidget;
class QPushButton;
class QTableWidget;

class TeacherView : public QWidget {
    Q_OBJECT
public:
    TeacherView(const QString &username, QWidget *parent = nullptr);
private slots:
    void onClassSelected();
    void onStudentClicked();
    void onLogout();
private:
    void loadGradesForClass(const QString &className);
    QString username;
    QListWidget *classList;
    QListWidget *studentList;
    QPushButton *logoutBtn;
    QPushButton *gradeBtn;
    QTableWidget *gradesTable;
};
