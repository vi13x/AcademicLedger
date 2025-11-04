#include "../include/mainwindow.h"
#include "../include/logindialog.h"
#include "../include/studentview.h"
#include "../include/teacherview.h"
#include "../include/adminview.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QFrame>
#include <QFont>
#include <QPalette>
#include <QApplication>
#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("📚 Электронный журнал");
    resize(800, 600);
    setMinimumSize(600, 500);

    // Устанавливаем брутальный стиль
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    
    // Создаем палитру для брутальной темы
    QPalette brutalPalette;
    brutalPalette.setColor(QPalette::Window, QColor(30, 30, 30));
    brutalPalette.setColor(QPalette::WindowText, Qt::white);
    brutalPalette.setColor(QPalette::Base, QColor(20, 20, 20));
    brutalPalette.setColor(QPalette::AlternateBase, QColor(40, 40, 40));
    brutalPalette.setColor(QPalette::ToolTipBase, QColor(50, 50, 50));
    brutalPalette.setColor(QPalette::ToolTipText, Qt::white);
    brutalPalette.setColor(QPalette::Text, Qt::white);
    brutalPalette.setColor(QPalette::Button, QColor(50, 50, 50));
    brutalPalette.setColor(QPalette::ButtonText, Qt::white);
    brutalPalette.setColor(QPalette::BrightText, Qt::white);
    brutalPalette.setColor(QPalette::Link, QColor(150, 150, 150));
    brutalPalette.setColor(QPalette::Highlight, QColor(80, 80, 80));
    brutalPalette.setColor(QPalette::HighlightedText, Qt::white);
    QApplication::setPalette(brutalPalette);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    
    // Основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Заголовок в брутальном стиле
    QFrame *headerFrame = new QFrame(this);
    headerFrame->setFrameStyle(QFrame::Box);
    headerFrame->setStyleSheet("QFrame { background-color: #1a1a1a; border: 2px solid #404040; padding: 20px; }");
    
    QVBoxLayout *headerLayout = new QVBoxLayout(headerFrame);
    headerLayout->setAlignment(Qt::AlignCenter);
    
    welcomeLabel = new QLabel("ЭЛЕКТРОННЫЙ ЖУРНАЛ", this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet("QLabel { color: white; font-size: 28px; font-weight: bold; margin: 5px; letter-spacing: 1px; }");
    
    QLabel *subtitleLabel = new QLabel("ДОБРО ПОЖАЛОВАТЬ В СИСТЕМУ УПРАВЛЕНИЯ ОБРАЗОВАНИЕМ", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("QLabel { color: #b0b0b0; font-size: 14px; margin: 5px; }");
    
    headerLayout->addWidget(welcomeLabel);
    headerLayout->addWidget(subtitleLabel);
    mainLayout->addWidget(headerFrame);

    // Кнопки в брутальном стиле
    QFrame *buttonsFrame = new QFrame(this);
    buttonsFrame->setStyleSheet("QFrame { background-color: #1a1a1a; border: 1px solid #404040; padding: 20px; }");
    
    QVBoxLayout *buttonsLayout = new QVBoxLayout(buttonsFrame);
    buttonsLayout->setSpacing(15);

    loginBtn = new QPushButton("ВХОД ДЛЯ УЧЕНИКА", this);
    loginBtn->setStyleSheet(
        "QPushButton { "
        "background-color: #404040; "
        "border: 2px solid #606060; "
        "color: white; "
        "font-size: 16px; "
        "font-weight: bold; "
        "padding: 15px; "
        "min-height: 20px; "
        "letter-spacing: 1px; "
        "} "
        "QPushButton:hover { "
        "background-color: #505050; "
        "border: 2px solid #707070; "
        "} "
        "QPushButton:pressed { "
        "background-color: #303030; "
        "border: 2px solid #505050; "
        "}"
    );

    teacherAdminBtn = new QPushButton("ВХОД ДЛЯ УЧИТЕЛЯ / АДМИНИСТРАТОРА", this);
    teacherAdminBtn->setStyleSheet(
        "QPushButton { "
        "background-color: #404040; "
        "border: 2px solid #606060; "
        "color: white; "
        "font-size: 16px; "
        "font-weight: bold; "
        "padding: 15px; "
        "min-height: 20px; "
        "letter-spacing: 1px; "
        "} "
        "QPushButton:hover { "
        "background-color: #505050; "
        "border: 2px solid #707070; "
        "} "
        "QPushButton:pressed { "
        "background-color: #303030; "
        "border: 2px solid #505050; "
        "}"
    );

    buttonsLayout->addWidget(loginBtn);
    buttonsLayout->addWidget(teacherAdminBtn);
    
    mainLayout->addWidget(buttonsFrame);
    mainLayout->addStretch();

    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::onLogin);
    connect(teacherAdminBtn, &QPushButton::clicked, this, &MainWindow::onTeacherAdminLogin);
}

void MainWindow::onLogin() {
    LoginDialog dlg(LoginDialog::Role::Student, this);
    if (dlg.exec() == QDialog::Accepted) {
        QString username = dlg.username();
        QString password = dlg.password();
        if (ds.checkStudentCredentials(username, password)) {
            StudentView *sv = new StudentView(username);
            sv->setAttribute(Qt::WA_DeleteOnClose);
            sv->show();
        } else {
            QMessageBox::warning(this, "Ошибка", "Неверные учётные данные ученика.");
        }
    }
}

void MainWindow::onTeacherAdminLogin() {
    LoginDialog dlg(LoginDialog::Role::TeacherOrAdmin, this);
    if (dlg.exec() == QDialog::Accepted) {
        QString username = dlg.username();
        QString password = dlg.password();
        if (ds.checkTeacherCredentials(username, password)) {
            TeacherView *tv = new TeacherView(username);
            tv->setAttribute(Qt::WA_DeleteOnClose);
            tv->show();
        } else if (ds.checkAdminCredentials(username, password)) {
            AdminView *av = new AdminView(username);
            av->setAttribute(Qt::WA_DeleteOnClose);
            av->show();
        } else {
            QMessageBox::warning(this, "Ошибка", "Неверные учётные данные (учитель/админ).");
        }
    }
}
