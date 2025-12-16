#ifndef TEACHERVIEW_H
#define TEACHERVIEW_H

#include <QWidget>
#include <QString>
#include <QStack>
#include <QTabWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QListWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDate>
#include <QVector>
#include <QMap>

#include "datastore.h" // For DataStore access

class TeacherView : public QWidget {
Q_OBJECT
public:
TeacherView(const QString &username, QWidget *parent = nullptr);
QStack<QWidget*> navigationStack;

private slots:
void onLogout();
void onBack();
void onAddHomework();
void onPeriodChanged();
void onMonthChanged();
void onMonthDateChanged(const QDate &date);
void onHeaderDoubleClicked(int section);
void onAddTopicClicked();
void onTopicsClicked();
void onClassChanged();
void onSubjectChanged();
void onCellClicked(int row, int col);

private:
void updateJournalTable();
void updateStudentsList();
void loadJournal();
void updateScheduleTable(QTableWidget *table, const QString &className);
void loadGradesForClass(const QString &className);
QVector<QDate> getDatesForMonth(int year, int month);
QVector<QDate> getDatesForMonth(const QDate &monthDate);
double calculateAverageForStudent(const QString &studentName, const QString &subject);
int calculatePeriodGrade(double average);
QString username;
QTabWidget *tabs;
QPushButton *logoutBtn;
QPushButton *backBtn;
QPushButton *topicsBtn;
QTableWidget *journalTable;
QTableWidget *gradesTable;
QListWidget *studentsList;
QComboBox *classCombo;
QComboBox *subjectCombo;
QDateEdit *monthDateEdit;
QComboBox *yearCombo;
QString currentClass;
QString currentSubject;
QDate currentMonth;
int currentYear;
};

#endif // TEACHERVIEW_H

