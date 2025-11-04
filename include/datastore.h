#pragma once
#include "user.h"
#include <QString>
#include <QVector>
#include <QMap>

class DataStore {
public:
    DataStore();

    // users
    QVector<User> loadStudents();
    bool addStudent(const User &u);
    bool checkStudentCredentials(const QString &username, const QString &password);

    bool checkTeacherCredentials(const QString &username, const QString &password);
    bool checkAdminCredentials(const QString &username, const QString &password);

    // teacher/admin files are plain: username:password per line
    bool addTeacher(const QString &username, const QString &password);
    bool removeTeacher(const QString &username);

    bool addAdmin(const QString &username, const QString &password);
    bool removeAdmin(const QString &username);

    // schedule
    QVector<QString> loadSchedule();
    bool saveSchedule(const QVector<QString> &lines);

    // grades
    // grades file format: class|student|subject|date|grade
    QVector<QString> loadGrades();
    bool addGrade(const QString &className, const QString &student, const QString &subject, const QString &date, const QString &grade);
    bool overwriteGrades(const QVector<QString> &lines);

    // classes and students management
    QVector<QString> getClasses();
    QVector<QString> getStudentsForClass(const QString &className);
    bool addStudentToClass(const QString &className, const QString &studentName);
    bool removeStudentFromClass(const QString &className, const QString &studentName);
    
    // schedule generation
    void generateDefaultSchedule();
    QVector<QString> getSubjects();
    QVector<QString> getTeachers();

private:
    QString studentsFile();
    QString teachersFile();
    QString adminsFile();
    QString scheduleFile();
    QString gradesFile();
    QString classesFile();
};
