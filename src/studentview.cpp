#include "../include/studentview.h"
#include "../include/datastore.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFrame>
#include <QHBoxLayout>
#include <QDate>
#include <QMessageBox>

StudentView::StudentView(const QString &username, QWidget *parent) : QWidget(parent), username(username) {
    setWindowTitle(QString("👨‍🎓 Студент: %1").arg(username));
    resize(1000, 700);
    setMinimumSize(800, 600);

    // Устанавливаем брутальные стили
    setStyleSheet(
        "QWidget { background-color: #1a1a1a; color: white; } "
        "QTabWidget::pane { border: 2px solid #404040; background-color: #2a2a2a; } "
        "QTabBar::tab { background-color: #404040; color: white; padding: 12px 24px; margin-right: 2px; border: 1px solid #606060; font-weight: bold; letter-spacing: 1px; } "
        "QTabBar::tab:selected { background-color: #505050; border: 2px solid #707070; } "
        "QTabBar::tab:hover { background-color: #454545; } "
        "QTableWidget { background-color: #2a2a2a; gridline-color: #404040; border: 1px solid #404040; } "
        "QTableWidget::item { padding: 10px; border-bottom: 1px solid #404040; } "
        "QTableWidget::item:selected { background-color: #505050; } "
        "QHeaderView::section { background-color: #404040; color: white; padding: 12px; border: 1px solid #606060; font-weight: bold; } "
        "QPushButton { background-color: #404040; color: white; border: 2px solid #606060; padding: 12px 24px; font-weight: bold; letter-spacing: 1px; } "
        "QPushButton:hover { background-color: #505050; border: 2px solid #707070; } "
        "QPushButton:pressed { background-color: #303030; border: 2px solid #505050; } "
    );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Заголовок в брутальном стиле
    QFrame *headerFrame = new QFrame(this);
    headerFrame->setStyleSheet("QFrame { background-color: #1a1a1a; border: 2px solid #404040; padding: 15px; }");
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    QLabel *userLbl = new QLabel(QString("СТУДЕНТ: %1").arg(username.toUpper()), this);
    userLbl->setStyleSheet("QLabel { color: white; font-size: 24px; font-weight: bold; letter-spacing: 2px; }");
    headerLayout->addWidget(userLbl);
    headerLayout->addStretch();
    
    QLabel *dateLabel = new QLabel(QDate::currentDate().toString("dd.MM.yyyy"), this);
    dateLabel->setStyleSheet("QLabel { color: #b0b0b0; font-size: 16px; font-weight: bold; }");
    headerLayout->addWidget(dateLabel);
    
    mainLayout->addWidget(headerFrame);

    // Вкладки
    tabs = new QTabWidget(this);
    tabs->setTabPosition(QTabWidget::North);

    // Расписание
    QWidget *scheduleWidget = new QWidget();
    QVBoxLayout *scheduleLayout = new QVBoxLayout(scheduleWidget);
    
    QTableWidget *scheduleTable = new QTableWidget(7, 2, this);
    scheduleTable->setHorizontalHeaderLabels(QStringList() << "День недели" << "Предметы");
    scheduleTable->verticalHeader()->setVisible(false);
    scheduleTable->horizontalHeader()->setStretchLastSection(true);
    scheduleTable->setAlternatingRowColors(true);
    scheduleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // Загружаем расписание из DataStore
    DataStore ds;
    auto scheduleLines = ds.loadSchedule();
    
    // Если расписание пустое, генерируем базовое
    if (scheduleLines.isEmpty()) {
        ds.generateDefaultSchedule();
        scheduleLines = ds.loadSchedule();
    }
    
    QStringList days = {"Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота", "Воскресенье"};
    
    for (int i = 0; i < 7; ++i) {
        QTableWidgetItem *dayItem = new QTableWidgetItem(days[i]);
        dayItem->setFlags(dayItem->flags() & ~Qt::ItemIsEditable);
        dayItem->setBackground(QColor(0x4a4a4a));
        scheduleTable->setItem(i, 0, dayItem);
        
        QString subjects = "Выходной";
        if (i < scheduleLines.size() && !scheduleLines[i].isEmpty()) {
            // Извлекаем только предметы из строки расписания
            QString line = scheduleLines[i];
            int colonPos = line.indexOf(':');
            if (colonPos != -1) {
                subjects = line.mid(colonPos + 1).trimmed();
            }
        }
        QTableWidgetItem *subjectItem = new QTableWidgetItem(subjects);
        subjectItem->setFlags(subjectItem->flags() & ~Qt::ItemIsEditable);
        scheduleTable->setItem(i, 1, subjectItem);
    }
    
    scheduleLayout->addWidget(scheduleTable);
    tabs->addTab(scheduleWidget, "РАСПИСАНИЕ");

    // Успеваемость
    QWidget *gradesWidget = new QWidget();
    QVBoxLayout *gradesLayout = new QVBoxLayout(gradesWidget);
    
    QTableWidget *gradesTable = new QTableWidget(0, 4, this);
    gradesTable->setHorizontalHeaderLabels(QStringList() << "ПРЕДМЕТ" << "ОЦЕНКА" << "ДАТА" << "УЧИТЕЛЬ");
    gradesTable->verticalHeader()->setVisible(false);
    gradesTable->horizontalHeader()->setStretchLastSection(true);
    gradesTable->setAlternatingRowColors(true);
    gradesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // Загружаем оценки из DataStore
    auto grades = ds.loadGrades();
    for (const QString &gradeLine : grades) {
        QStringList parts = gradeLine.split(',');
        if (parts.size() >= 4) {
            int row = gradesTable->rowCount();
            gradesTable->insertRow(row);
            
            gradesTable->setItem(row, 0, new QTableWidgetItem(parts[2])); // Предмет
            gradesTable->setItem(row, 1, new QTableWidgetItem(parts[4])); // Оценка
            gradesTable->setItem(row, 2, new QTableWidgetItem(parts[3])); // Дата
            gradesTable->setItem(row, 3, new QTableWidgetItem("УЧИТЕЛЬ")); // Учитель
        }
    }
    
    gradesLayout->addWidget(gradesTable);
    tabs->addTab(gradesWidget, "УСПЕВАЕМОСТЬ");

    // Статистика
    QWidget *statsWidget = new QWidget();
    QVBoxLayout *statsLayout = new QVBoxLayout(statsWidget);
    
    QLabel *statsLabel = new QLabel("СТАТИСТИКА УСПЕВАЕМОСТИ", this);
    statsLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; margin: 10px; letter-spacing: 1px; }");
    statsLayout->addWidget(statsLabel);
    
    // Статистическая информация в брутальном стиле
    QLabel *statsInfo = new QLabel("СРЕДНИЙ БАЛЛ: 8.5\nЛУЧШИЙ ПРЕДМЕТ: МАТЕМАТИКА\nКОЛИЧЕСТВО ОЦЕНОК: 15", this);
    statsInfo->setStyleSheet("QLabel { font-size: 14px; padding: 20px; background-color: #2a2a2a; border: 1px solid #404040; letter-spacing: 1px; }");
    statsLayout->addWidget(statsInfo);
    
    tabs->addTab(statsWidget, "СТАТИСТИКА");

    mainLayout->addWidget(tabs);

    // Кнопки в брутальном стиле
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    backBtn = new QPushButton("НАЗАД", this);
    logoutBtn = new QPushButton("ВЫЙТИ ИЗ АККАУНТА", this);
    
    buttonsLayout->addWidget(backBtn);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(logoutBtn);
    
    mainLayout->addLayout(buttonsLayout);

    connect(logoutBtn, &QPushButton::clicked, this, &StudentView::onLogout);
    connect(backBtn, &QPushButton::clicked, this, &StudentView::onBack);
}

void StudentView::onLogout() {
    close();
}

void StudentView::onBack() {
    // просто закрываем — пользователь вернётся к главному окну
    close();
}
