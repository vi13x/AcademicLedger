#ifndef ADMINVIEW_H
#define ADMINVIEW_H

#include <QWidget>
#include <QString>
#include <QStack>
#include <QTabWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QListWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDate>
#include <QVector>
#include <QMap>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QAbstractItemView>

#include "list.hpp"
#include "datastore.h"
#include "addteacherdialog.h"
#include "addstudentdialog.h"
#include "disciplinemarkdialog.h"

class AdminView : public QWidget {
Q_OBJECT
public:
AdminView(const QString &username, QWidget *parent = nullptr);
QStack<QWidget*> navigationStack;

private slots:
void onLogout();
void onBack();
void onClassDoubleClicked(QListWidgetItem *item);
void onStudentDoubleClicked();
void onRefreshClasses();
void onRefreshStudents();
void onAddTeacher();
void onRemoveTeacher();
void onAddAdmin();
void onRemoveAdmin();
void onLoadSchedule();
void onSaveSchedule();
void onGenerateSchedule();
void onViewGrades();
void onClearGrades();
void onRefreshAdmins();
void onExportData();
void onSearchTeachers();
void onSearchAdmins();

private:
void updateClassesList();
void updateStudentsList();
QString username;
QTabWidget *tabs;
QTabWidget *mainTabs;
QPushButton *logoutBtn;
QPushButton *backBtn;
QListWidget *classesList;
QListWidget *studentsList;
QListWidget *studentsListForDiscipline;
QListWidget *teachersList;
QListWidget *adminsList;
QTableWidget *scheduleTable;
QComboBox *scheduleClassCombo;
QLineEdit *teacherSearchEdit;
QLineEdit *adminSearchEdit;
QTableWidget *allGradesTable;
};

#endif // ADMINVIEW_H

