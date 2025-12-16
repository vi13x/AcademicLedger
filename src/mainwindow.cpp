#include "mainwindow.h"

#include <QApplication>
#include <QScrollArea>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDate>
#include <QCoreApplication>
#include <QTableWidget>
#include <QHeaderView>
#include <QSet>
#include <QFileDialog>
#include <QLineEdit>
#include <QGroupBox>
#include <QProgressBar>
#include <QDateEdit>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
{
setWindowTitle("Электронный журнал");
applyStyle();
setMinimumSize(600, 500);
resize(800, 600);
setupUi();
}

void MainWindow::applyStyle() {
QString darkStyle = R"(
QMainWindow, QWidget {
background-color: #121212;
color: #e0e0e0;
}
QPushButton {
background-color: #3D7BFF;
color: white;
border: none;
border-radius: 0px;
padding: 12px 24px;
font-size: 14px;
font-weight: bold;
min-height: 44px;
}
QPushButton:hover {
background-color: #5A8FFF;
}
QPushButton:pressed {
background-color: #2A6AFF;
}
QLabel {
color: #e0e0e0;
}
QLineEdit, QTextEdit {
background-color: #1e1e1e;
color: #e0e0e0;
border: 2px solid #333333;
border-radius: 0px;
padding: 10px;
selection-background-color: #3D7BFF;
selection-color: white;
}
QLineEdit:focus, QTextEdit:focus {
border-color: #3D7BFF;
background-color: #252525;
}
QListWidget, QTableWidget {
background-color: #1E1E1E;
color: #e0e0e0;
border: 2px solid #333333;
border-radius: 0px;
gridline-color: #2a2a2a;
selection-background-color: #3D7BFF;
selection-color: white;
alternate-background-color: #232323;
}
QListWidget::item:hover, QTableWidget::item:hover {
background-color: #333333;
}
QListWidget::item:selected, QTableWidget::item:selected {
background-color: #3D7BFF;
color: white;
}
QHeaderView::section {
background-color: #2D2D2D;
color: #e0e0e0;
padding: 12px;
border: 1px solid #333333;
font-weight: bold;
font-size: 13px;
}
QTabWidget::pane {
background-color: #1e1e1e;
border: 2px solid #333333;
border-radius: 0px;
}
QTabBar::tab {
background-color: #1a1a1a;
color: #b0b0b0;
padding: 12px 24px;
border-top-left-radius: 0px;
border-top-right-radius: 0px;
margin-right: 2px;
}
QTabBar::tab:selected {
background-color: #1e1e1e;
border-bottom: 3px solid #3D7BFF;
color: #3D7BFF;
font-weight: bold;
}
QTabBar::tab:hover {
background-color: #252525;
color: #e0e0e0;
}
QComboBox {
background-color: #1e1e1e;
color: #e0e0e0;
border: 2px solid #333333;
border-radius: 0px;
padding: 8px;
min-width: 150px;
}
QComboBox:hover {
border-color: #3D7BFF;
}
QComboBox::drop-down {
border: none;
}
QComboBox QAbstractItemView {
background-color: #1e1e1e;
color: #e0e0e0;
selection-background-color: #3D7BFF;
selection-color: white;
border: 1px solid #333333;
}
QDialog {
background-color: #121212;
color: #e0e0e0;
}
QMessageBox {
background-color: #121212;
color: #e0e0e0;
}
QGroupBox {
border: 2px solid #333333;
border-radius: 0px;
margin-top: 15px;
padding-top: 15px;
font-weight: bold;
color: #e0e0e0;
}
QGroupBox::title {
subcontrol-origin: margin;
left: 10px;
padding: 0 8px;
}
QScrollBar:vertical {
background-color: #1a1a1a;
width: 14px;
border: none;
}
QScrollBar::handle:vertical {
background-color: #424242;
border-radius: 7px;
min-height: 30px;
}
QScrollBar::handle:vertical:hover {
background-color: #616161;
}
QScrollBar:horizontal {
background-color: #1a1a1a;
height: 14px;
border: none;
}
QScrollBar::handle:horizontal {
background-color: #424242;
border-radius: 7px;
min-width: 30px;
}
QScrollBar::handle:horizontal:hover {
background-color: #616161;
}
)";
qApp->setStyleSheet(darkStyle);
}

void MainWindow::setupUi() {
QWidget *c = new QWidget();
setCentralWidget(c);

QVBoxLayout *lay = new QVBoxLayout(c);
lay->setSpacing(20);
lay->setContentsMargins(50, 50, 50, 50);

welcomeLabel = new QLabel("Электронный журнал");
welcomeLabel->setStyleSheet("font-size: 32px; font-weight: bold; padding: 20px; color: #e0e0e0;");
lay->addWidget(welcomeLabel, 0, Qt::AlignCenter);

// Кнопки входа
QGroupBox *loginGroup = new QGroupBox("Вход в систему");
QVBoxLayout *loginLay = new QVBoxLayout(loginGroup);

loginBtn = new QPushButton("Вход ученика");
loginBtn->setMinimumHeight(80);
loginBtn->setMinimumWidth(300);
loginBtn->setStyleSheet("font-size: 18px; font-weight: bold; padding: 20px;");
loginBtn->setCursor(Qt::PointingHandCursor);
teacherAdminBtn = new QPushButton("Вход учителя/админа");
teacherAdminBtn->setMinimumHeight(80);
teacherAdminBtn->setMinimumWidth(300);
teacherAdminBtn->setStyleSheet("font-size: 18px; font-weight: bold; padding: 20px;");
teacherAdminBtn->setCursor(Qt::PointingHandCursor);

loginLay->addWidget(loginBtn);
loginLay->addWidget(teacherAdminBtn);

connect(loginBtn, &QPushButton::clicked, this, &MainWindow::onLogin);
connect(teacherAdminBtn, &QPushButton::clicked, this, &MainWindow::onTeacherAdminLogin);

lay->addWidget(loginGroup);
lay->addStretch();
}

void MainWindow::onLogin() {
LoginDialog dlg(LoginDialog::Role::Student);
if (dlg.exec() != QDialog::Accepted) return;

QString u = dlg.username();
QString p = dlg.password();

if (!ds.checkStudentCredentials(u, p)) {
QMessageBox::warning(this, "Ошибка", "Неверный логин/пароль.");
return;
}

StudentView *sv = new StudentView(u);
sv->setAttribute(Qt::WA_DeleteOnClose);
sv->navigationStack.push(this);
hide();
sv->showMaximized();
}

void MainWindow::onTeacherAdminLogin() {
LoginDialog dlg(LoginDialog::Role::TeacherOrAdmin);
if (dlg.exec() != QDialog::Accepted) return;

QString u = dlg.username();
QString p = dlg.password();

// admin? (включая admin/admin)
if (ds.checkAdminCredentials(u, p)) {
AdminView *av = new AdminView(u);
av->setAttribute(Qt::WA_DeleteOnClose);
av->navigationStack.push(this);
hide();
av->showMaximized();
return;
}

// teacher?
if (ds.checkTeacherCredentials(u, p)) {
TeacherView *tv = new TeacherView(u);
tv->setAttribute(Qt::WA_DeleteOnClose);
tv->navigationStack.push(this);
hide();
tv->showMaximized();
return;
}

QMessageBox::warning(this, "Ошибка", "Неверный логин/пароль.");
}
