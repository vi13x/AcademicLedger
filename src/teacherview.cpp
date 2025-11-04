#include "../include/teacherview.h"
#include "../include/markdialog.h"
#include "../include/datastore.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QDate>
#include <QFrame>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QSplitter>
TeacherView::TeacherView(const QString &username, QWidget *parent) : QWidget(parent), username(username) {
    setWindowTitle(QString("👨‍🏫 Учитель: %1").arg(username));
    resize(1200, 800);
    setMinimumSize(1000, 700);

    // Устанавливаем брутальные стили
    setStyleSheet(
        "QWidget { background-color: #1a1a1a; color: white; } "
        "QListWidget { background-color: #2a2a2a; border: 2px solid #404040; } "
        "QListWidget::item { padding: 12px; border-bottom: 1px solid #404040; font-weight: bold; } "
        "QListWidget::item:selected { background-color: #505050; } "
        "QListWidget::item:hover { background-color: #404040; } "
        "QTableWidget { background-color: #2a2a2a; gridline-color: #404040; border: 1px solid #404040; } "
        "QTableWidget::item { padding: 10px; border-bottom: 1px solid #404040; } "
        "QTableWidget::item:selected { background-color: #505050; } "
        "QHeaderView::section { background-color: #404040; color: white; padding: 12px; border: 1px solid #606060; font-weight: bold; } "
        "QPushButton { background-color: #404040; color: white; border: 2px solid #606060; padding: 12px 24px; font-weight: bold; letter-spacing: 1px; } "
        "QPushButton:hover { background-color: #505050; border: 2px solid #707070; } "
        "QPushButton:pressed { background-color: #303030; border: 2px solid #505050; } "
        "QPushButton#gradeBtn { background-color: #505050; border: 2px solid #707070; } "
        "QPushButton#gradeBtn:hover { background-color: #606060; border: 2px solid #808080; } "
    );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Заголовок в брутальном стиле
    QFrame *headerFrame = new QFrame(this);
    headerFrame->setStyleSheet("QFrame { background-color: #1a1a1a; border: 2px solid #404040; padding: 15px; }");
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    QLabel *userLbl = new QLabel(QString("УЧИТЕЛЬ: %1").arg(username.toUpper()), this);
    userLbl->setStyleSheet("QLabel { color: white; font-size: 24px; font-weight: bold; letter-spacing: 2px; }");
    headerLayout->addWidget(userLbl);
    headerLayout->addStretch();
    
    QLabel *dateLabel = new QLabel(QDate::currentDate().toString("dd.MM.yyyy"), this);
    dateLabel->setStyleSheet("QLabel { color: #b0b0b0; font-size: 16px; font-weight: bold; }");
    headerLayout->addWidget(dateLabel);
    
    mainLayout->addWidget(headerFrame);

    // Основной контент
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Левая панель - классы и студенты
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    
    QLabel *classesLabel = new QLabel("КЛАССЫ:", this);
    classesLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; margin: 10px 0; letter-spacing: 1px; }");
    leftLayout->addWidget(classesLabel);
    
    classList = new QListWidget(this);
    // Загружаем классы из DataStore
    DataStore ds;
    auto classes = ds.getClasses();
    for (const QString &cls : classes) {
        classList->addItem(cls);
    }
    leftLayout->addWidget(classList);
    
    QLabel *studentsLabel = new QLabel("СТУДЕНТЫ:", this);
    studentsLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; margin: 10px 0; letter-spacing: 1px; }");
    leftLayout->addWidget(studentsLabel);
    
    studentList = new QListWidget(this);
    studentList->addItem("ВЫБЕРИТЕ КЛАСС ДЛЯ ПРОСМОТРА СТУДЕНТОВ");
    leftLayout->addWidget(studentList);
    
    QPushButton *gradeBtn = new QPushButton("ПОСТАВИТЬ ОЦЕНКУ", this);
    gradeBtn->setObjectName("gradeBtn");
    gradeBtn->setEnabled(false);
    leftLayout->addWidget(gradeBtn);
    
    mainSplitter->addWidget(leftPanel);
    
    // Правая панель - таблица оценок
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    QLabel *gradesLabel = new QLabel("ОЦЕНКИ КЛАССА:", this);
    gradesLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; margin: 10px 0; letter-spacing: 1px; }");
    rightLayout->addWidget(gradesLabel);
    
    gradesTable = new QTableWidget(0, 5, this);
    gradesTable->setHorizontalHeaderLabels(QStringList() << "СТУДЕНТ" << "ПРЕДМЕТ" << "ОЦЕНКА" << "ДАТА" << "ДЕЙСТВИЯ");
    gradesTable->verticalHeader()->setVisible(false);
    gradesTable->horizontalHeader()->setStretchLastSection(true);
    gradesTable->setAlternatingRowColors(true);
    gradesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    rightLayout->addWidget(gradesTable);
    
    mainSplitter->addWidget(rightPanel);
    mainSplitter->setSizes({400, 800});
    
    mainLayout->addWidget(mainSplitter);

    // Кнопки в брутальном стиле
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    logoutBtn = new QPushButton("ВЫЙТИ", this);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(logoutBtn);
    mainLayout->addLayout(buttonsLayout);

    connect(classList, &QListWidget::itemClicked, this, &TeacherView::onClassSelected);
    connect(studentList, &QListWidget::itemClicked, this, &TeacherView::onStudentClicked);
    connect(gradeBtn, &QPushButton::clicked, this, &TeacherView::onStudentClicked);
    connect(logoutBtn, &QPushButton::clicked, this, &TeacherView::onLogout);
    
    // Сохраняем ссылки на виджеты для использования в слотах
    this->gradeBtn = gradeBtn;
}

void TeacherView::onClassSelected() {
    QString cls = classList->currentItem()->text();
    studentList->clear();
    gradeBtn->setEnabled(false);
    
    // Загружаем студентов для выбранного класса из DataStore
    DataStore ds;
    auto students = ds.getStudentsForClass(cls);
    for (const QString &student : students) {
        studentList->addItem(student);
    }
    
    // Загружаем оценки для выбранного класса
    loadGradesForClass(cls);
}

void TeacherView::loadGradesForClass(const QString &className) {
    gradesTable->setRowCount(0);
    
    DataStore ds;
    auto grades = ds.loadGrades();
    
    for (const QString &gradeLine : grades) {
        QStringList parts = gradeLine.split(',');
        if (parts.size() >= 5 && parts[0] == className) {
            int row = gradesTable->rowCount();
            gradesTable->insertRow(row);
            
            gradesTable->setItem(row, 0, new QTableWidgetItem(parts[1])); // Студент
            gradesTable->setItem(row, 1, new QTableWidgetItem(parts[2])); // Предмет
            gradesTable->setItem(row, 2, new QTableWidgetItem(parts[4])); // Оценка
            gradesTable->setItem(row, 3, new QTableWidgetItem(parts[3])); // Дата
            
            QPushButton *editBtn = new QPushButton("РЕДАКТИРОВАТЬ");
            editBtn->setStyleSheet("QPushButton { background-color: #505050; color: white; border: 1px solid #707070; padding: 5px; font-weight: bold; }");
            gradesTable->setCellWidget(row, 4, editBtn);
        }
    }
}

void TeacherView::onStudentClicked() {
    if (!classList->currentItem()) return;
    
    QString cls = classList->currentItem()->text();
    QString student;
    
    // Проверяем, выбрал ли пользователь студента или нажал кнопку
    if (studentList->currentItem() && studentList->currentItem()->text() != "Выберите класс для просмотра студентов") {
        student = studentList->currentItem()->text();
    } else {
        // Если студент не выбран, показываем сообщение
        QMessageBox::information(this, "Информация", "Пожалуйста, выберите студента из списка");
        return;
    }

    // Открываем диалог для постановки оценки
    MarkDialog dlg(cls, student, this);
    if (dlg.exec() == QDialog::Accepted) {
        QString subject = dlg.subject();
        QString grade = dlg.grade();
        DataStore ds;
        ds.addGrade(cls, student, subject, QDate::currentDate().toString(Qt::ISODate), grade);
        
        // Обновляем таблицу оценок
        loadGradesForClass(cls);
        
        QMessageBox::information(this, "✅ Успешно", 
            QString("Поставлена оценка <b>%1</b> ученику <b>%2</b> по предмету <b>%3</b>")
            .arg(grade, student, subject));
    }
}

void TeacherView::onLogout() {
    close();
}
