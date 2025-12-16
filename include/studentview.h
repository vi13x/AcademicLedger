#ifndef STUDENTVIEW_H
#define STUDENTVIEW_H

#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QLineEdit>
#include <QStack>

class StudentView : public QWidget {
Q_OBJECT
public:
StudentView(const QString &username, QWidget *parent = nullptr);
QStack<QWidget*> navigationStack;

private slots:
void onLogout();
void onBack();
void onSearchGrades();
void onExportGrades();
void onShowStatistics();

private:
void updateGradesTable();
QString username;
QTabWidget *tabs;
QPushButton *logoutBtn;
QPushButton *backBtn;
QTableWidget *gradesTable;
QLineEdit *searchEdit;
};

#endif // STUDENTVIEW_H

