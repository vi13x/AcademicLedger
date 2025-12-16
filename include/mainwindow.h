#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QVector>
#include <QMap>
#include <QDate>
#include <QDateEdit>
#include <QMessageBox>
#include <QComboBox>
#include <QFrame>
#include <QSplitter>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QStack>
#include <memory>
#include <algorithm>
#include <QStringList>
#include <QStringConverter>
#include <QGroupBox>

// Includes for extracted classes
#include "list.hpp"
#include "user.h"
#include "datastore.h"
#include "logindialog.h"
#include "markdialog.h"
#include "gradedialog.h"
#include "addteacherdialog.h"
#include "addstudentdialog.h"
#include "disciplinemarkdialog.h"
#include "homeworkdialog.h"
#include "studentview.h"
#include "teacherview.h"
#include "adminview.h"

class MainWindow : public QMainWindow {
Q_OBJECT
public:
MainWindow(QWidget *parent = nullptr);

private slots:
void onLogin();
void onTeacherAdminLogin();

private:
void applyStyle();
void setupUi();
void setupHeader(QVBoxLayout *mainLayout);
void setupButtons(QVBoxLayout *mainLayout);

DataStore ds;
QLabel *welcomeLabel;
QPushButton *loginBtn;
QPushButton *teacherAdminBtn;
};

#endif
