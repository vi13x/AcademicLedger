#ifndef DATASTORE_H
#define DATASTORE_H

#include <QString>
#include <QDate>
#include "user.h"
#include "list.hpp"

class DataStore {
public:
DataStore();
QString studentsFile();
QString teachersFile();
QString adminsFile();
QString scheduleFile();
QString gradesFile();
QString classesFile();
QString disciplineMarksFile();
QString homeworkFile();
QString lessonTopicsFile();
QString scheduleFileForClass(const QString &className);

List<User> loadStudents();
bool addStudent(const User &u, const QString &fullName = "", const QString &className = "");
List<QString> getTeachers();
QString getTeacherFullName(const QString &username);
List<QString> getSubjects();
List<QString> getStudentsForClass(const QString &className);
List<QString> getClasses();
List<QString> getAdmins();
List<QString> getPeriods();
List<QString> loadSchedule(const QString &className = "");
List<QString> loadGrades();
List<QString> loadHomework();
bool checkStudentCredentials(const QString &username, const QString &password);
bool checkTeacherCredentials(const QString &username, const QString &password);
bool checkAdminCredentials(const QString &username, const QString &password);
Map<QString, List<int>> loadGrades(const QString &studentLogin);
List<QString> loadDisciplineMarks();
bool addDisciplineMark(const QString &studentLogin, const QString &subject, const QString &mark, const QDate &date);
bool addGrade(const QString &cls, const QString &st, const QString &subj, const QString &date, const QString &grade);
bool overwriteGrades(const List<QString> &lines);
bool addHomework(const QString &className, const QString &subject, const QString &date, const QString &text);
bool addLessonTopic(const QString &className, const QString &subject, const QString &date, const QString &topic, const QString &teacherLogin);
bool updateLessonTopic(const QString &className, const QString &subject, const QString &date, const QString &topic, const QString &teacherLogin);
bool deleteLessonTopic(const QString &className, const QString &subject, const QString &date);
QString getLessonTopic(const QString &className, const QString &subject, const QString &date);
bool addTeacher(const QString &fullName, const QString &login, const QString &password, const QString &subject);
bool removeTeacher(const QString &login);
bool addAdmin(const QString &username, const QString &password);
bool removeAdmin(const QString &login);
bool saveSchedule(const List<QString> &lines, const QString &className);
void generateDefaultSchedule();
bool addStudentToClass(const QString &cls, const QString &fullName, const QString &login, const QString &password);
bool removeStudentFromClass(const QString &cls, const QString &name);

private:
QString dataDir;
};

#endif // DATASTORE_H

