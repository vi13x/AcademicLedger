#include "studentview.h"
#include "datastore.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QFileDialog>
#include <QMessageBox>
#include <QDate>
#include <QColor>
#include <QFont>
#include <QAbstractItemView>


StudentView::StudentView(const QString &user, QWidget *parent)
: QWidget(parent), username(user)
{
setWindowTitle("Личный кабинет ученика");
resize(1400, 800);
QVBoxLayout *lay = new QVBoxLayout(this);

// Получаем ФИО ученика из файла
DataStore ds;
QString studentFullName = username;
QFile f(ds.studentsFile());
if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);
while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (line.isEmpty()) continue;
QStringList p = line.split('\t');
if (p.size() >= 6 && p[3] == username) {
studentFullName = p[0] + " " + p[1] + " " + p[2];
break;
}
}
}

// Верхняя панель с заголовком и кнопкой выхода
QHBoxLayout *headerLayout = new QHBoxLayout();
QLabel *title = new QLabel("Личный кабинет ученика: " + studentFullName);
title->setStyleSheet("font-size: 24px; font-weight: bold; color: #e0e0e0;");
headerLayout->addWidget(title);
headerLayout->addStretch();
logoutBtn = new QPushButton("Выйти");
logoutBtn->setMinimumHeight(40);
logoutBtn->setMinimumWidth(120);
logoutBtn->setStyleSheet("QPushButton { background-color: #D63031; color: white; font-size: 14px; font-weight: bold; border-radius: 0px; min-height: 40px; padding: 10px 20px; } QPushButton:hover { background-color: #E74C3C; } QPushButton:pressed { background-color: #C0392B; }");
connect(logoutBtn, &QPushButton::clicked, this, &StudentView::onLogout);
headerLayout->addWidget(logoutBtn);
lay->addLayout(headerLayout);

tabs = new QTabWidget();

// ---- Расписание ----
QWidget *scheduleTab = new QWidget();
QVBoxLayout *schedLay = new QVBoxLayout(scheduleTab);

// Создаем таблицу расписания как на фото
QTableWidget *scheduleTable = new QTableWidget();
scheduleTable->setColumnCount(4);
QStringList scheduleHeaders = {"Время", "Урок", "Кабинет", "ДЗ"};
scheduleTable->setHorizontalHeaderLabels(scheduleHeaders);
scheduleTable->horizontalHeader()->setStretchLastSection(true);
scheduleTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::AnyKeyPressed);

// Загружаем расписание и парсим его
// Получаем класс ученика из файла (используем уже объявленный ds и f)
QString studentClass = "";
// Закрываем предыдущий файл и открываем заново для поиска класса
f.close();
if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);
while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (line.isEmpty()) continue;
QStringList p = line.split('\t');
if (p.size() >= 6 && p[3] == username) {
studentClass = p[5].trimmed();
break;
}
}
f.close();
}
// Загружаем расписание для класса ученика из файла
auto scheduleLines = ds.loadSchedule(studentClass);

// Загружаем домашние задания
auto homeworkList = ds.loadHomework();
QMap<QString, QString> homeworkMap; // предмет -> ДЗ
for (const QString &line : homeworkList) {
QStringList parts = line.split('|');
if (parts.size() >= 4 && parts[0] == studentClass) {
QString subject = parts[1];
QString homework = parts[2];
homeworkMap[subject] = homework;
}
}

// Создаем структуру расписания по дням недели
QStringList dayNames = {"Понедельник", "Вторник", "Среда", "Четверг", "Пятница"};
QStringList dayNamesUpper = {"ПОНЕДЕЛЬНИК", "ВТОРНИК", "СРЕДА", "ЧЕТВЕРГ", "ПЯТНИЦА"};
QStringList dayColors = {"#3D7BFF", "#1e1e1e", "#5A8FFF", "#1e1e1e", "#5A8FFF"}; // Синий, темный, фиолетовый, темный, фиолетовый
QStringList dayTextColors = {"#ffffff", "#e0e0e0", "#ffffff", "#e0e0e0", "#ffffff"}; // Белый текст для цветных, светлый для темных

int totalRows = 0;
for (const QString &day : dayNamesUpper) {
totalRows += 7; // 6 уроков + заголовок дня
}
scheduleTable->setRowCount(totalRows);

int currentRow = 0;
for (int dayIdx = 0; dayIdx < dayNames.size(); dayIdx++) {
QString dayName = dayNames[dayIdx];
QString dayUpper = dayNamesUpper[dayIdx];

// Заголовок дня
QTableWidgetItem *dayHeader = new QTableWidgetItem(dayUpper);
QFont dayFont = dayHeader->font();
dayFont.setBold(true);
dayFont.setPointSize(14);
dayHeader->setFont(dayFont);
dayHeader->setBackground(QColor(dayColors[dayIdx]));
dayHeader->setForeground(QColor(dayTextColors[dayIdx]));
dayHeader->setFlags(dayHeader->flags() & ~Qt::ItemIsEditable);
dayHeader->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
scheduleTable->setItem(currentRow, 0, dayHeader);
scheduleTable->setSpan(currentRow, 0, 1, 4);
currentRow++;

// Времена уроков
QStringList times = {"14:00-14:45", "15:00-15:45", "16:00-16:45", 
"16:55-17:40", "17:50-18:35", "18:45-19:30"};

// Парсим расписание для этого дня
QString scheduleLine = "";
for (const QString &line : scheduleLines) {
QString lineLower = line.toLower();
if (lineLower.startsWith(dayName.toLower())) {
scheduleLine = line;
break;
}
}

QStringList lessons;
QStringList classrooms;
if (!scheduleLine.isEmpty()) {
int colonPos = scheduleLine.indexOf(':');
if (colonPos >= 0) {
QString afterColon = scheduleLine.mid(colonPos + 1).trimmed();
QStringList items = afterColon.split(',');
for (const QString &item : items) {
QString trimmed = item.trimmed();
// Парсим формат "Предмет (кабинет)" или "Предмет (каб1/каб2)"
if (trimmed.contains('(') && trimmed.contains(')')) {
int start = trimmed.indexOf('(') + 1;
int end = trimmed.indexOf(')');
QString subject = trimmed.left(trimmed.indexOf('(')).trimmed();
QString cab = trimmed.mid(start, end - start);
lessons.append(subject);
classrooms.append(cab);
} else {
lessons.append(trimmed);
classrooms.append("");
}
}
}
}

for (int lesson = 0; lesson < 6; lesson++) {
// Устанавливаем цвет фона для чередующихся строк
QColor rowColor = (lesson % 2 == 0) ? QColor("#1e1e1e") : QColor("#252525");

QTableWidgetItem *timeItem = new QTableWidgetItem(times[lesson]);
timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
timeItem->setTextAlignment(Qt::AlignCenter);
timeItem->setBackground(rowColor);
scheduleTable->setItem(currentRow, 0, timeItem);

QString lessonText = lesson < lessons.size() ? lessons[lesson] : "";
QString classroom = lesson < classrooms.size() ? classrooms[lesson] : "";

if (lessonText.isEmpty()) {
lessonText = "-";
}

QTableWidgetItem *lessonItem = new QTableWidgetItem(lessonText);
lessonItem->setFlags(lessonItem->flags() & ~Qt::ItemIsEditable);
lessonItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
lessonItem->setBackground(rowColor);
scheduleTable->setItem(currentRow, 1, lessonItem);

QTableWidgetItem *classroomItem = new QTableWidgetItem(classroom);
classroomItem->setFlags(classroomItem->flags() & ~Qt::ItemIsEditable);
classroomItem->setTextAlignment(Qt::AlignCenter);
classroomItem->setBackground(rowColor);
scheduleTable->setItem(currentRow, 2, classroomItem);

// Колонка ДЗ (редактируемая)
QString homeworkText = homeworkMap.value(lessonText, "");
QTableWidgetItem *homeworkItem = new QTableWidgetItem(homeworkText);
homeworkItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
homeworkItem->setBackground(rowColor);
// Делаем ячейку редактируемой
homeworkItem->setFlags(homeworkItem->flags() | Qt::ItemIsEditable);
scheduleTable->setItem(currentRow, 3, homeworkItem);

currentRow++;
}
}

// Обработчик изменения ДЗ
connect(scheduleTable, &QTableWidget::cellChanged, [=, &ds](int row, int col) {
if (col == 3) { // Колонка ДЗ
QTableWidgetItem *item = scheduleTable->item(row, col);
if (!item) return;

QString homework = item->text();
QString lessonName = "";
QTableWidgetItem *lessonItem = scheduleTable->item(row, 1);
if (lessonItem) {
lessonName = lessonItem->text().trimmed();
}

if (!lessonName.isEmpty() && lessonName != "-") {
QDate date = QDate::currentDate();
ds.addHomework(studentClass, lessonName, homework, date.toString("yyyy-MM-dd"));
}
}
});

// Настраиваем ширины колонок
scheduleTable->setColumnWidth(0, 150); // Время
scheduleTable->setColumnWidth(1, 300); // Урок
scheduleTable->setColumnWidth(2, 120); // Кабинет
// ДЗ - растягивается автоматически
scheduleTable->setAlternatingRowColors(true);
scheduleTable->verticalHeader()->setVisible(false);
scheduleTable->horizontalHeader()->setStretchLastSection(true);
scheduleTable->setStyleSheet(R"(
QTableWidget { 
background-color: #1E1E1E; 
border: 2px solid #333333;
gridline-color: #2a2a2a;
alternate-background-color: #232323;
} 
QTableWidget::item { 
padding: 10px; 
border: 1px solid #2a2a2a;
color: #e0e0e0;
}
QTableWidget::item:hover {
background-color: #333333;
}
QTableWidget::item:selected {
background-color: #3D7BFF;
color: white;
}
QTableWidget::item:focus {
background-color: #2a2a2a;
border: 2px solid #3D7BFF;
color: #e0e0e0;
}
QHeaderView::section {
background-color: #2D2D2D;
padding: 12px;
border: 1px solid #333333;
font-weight: bold;
font-size: 13px;
color: #e0e0e0;
}
)");

schedLay->addWidget(scheduleTable);
tabs->addTab(scheduleTab, "Расписание");

// ---- Оценки ----
QWidget *gradesTab = new QWidget();
QVBoxLayout *gradesLay = new QVBoxLayout(gradesTab);

// Поиск и статистика
QHBoxLayout *searchLayout = new QHBoxLayout();
searchEdit = new QLineEdit();
searchEdit->setPlaceholderText("Поиск по предмету...");
QPushButton *searchBtn = new QPushButton("Найти");
QPushButton *statsBtn = new QPushButton("Статистика");
QPushButton *exportBtn = new QPushButton("Экспорт");
connect(searchEdit, &QLineEdit::textChanged, this, &StudentView::onSearchGrades);
connect(searchBtn, &QPushButton::clicked, this, &StudentView::onSearchGrades);
connect(statsBtn, &QPushButton::clicked, this, &StudentView::onShowStatistics);
connect(exportBtn, &QPushButton::clicked, this, &StudentView::onExportGrades);
searchLayout->addWidget(new QLabel("Поиск:"));
searchLayout->addWidget(searchEdit);
searchLayout->addWidget(searchBtn);
searchLayout->addWidget(statsBtn);
searchLayout->addWidget(exportBtn);
gradesLay->addLayout(searchLayout);

gradesTable = new QTableWidget();
gradesTable->setColumnCount(4);
QStringList headers = {"Предмет", "Дата", "Оценка", "Класс"};
gradesTable->setHorizontalHeaderLabels(headers);
gradesTable->horizontalHeader()->setStretchLastSection(true);
updateGradesTable();
gradesLay->addWidget(gradesTable);
tabs->addTab(gradesTab, "Оценки");

lay->addWidget(tabs);

QHBoxLayout *btnLayout = new QHBoxLayout();
backBtn = new QPushButton("Назад");
backBtn->setMinimumHeight(40);
backBtn->setMinimumWidth(120);
backBtn->setStyleSheet("QPushButton { background-color: #444444; color: white; font-size: 14px; font-weight: bold; border-radius: 0px; padding: 10px 16px; } QPushButton:hover { background-color: #555555; } QPushButton:pressed { background-color: #333333; }");
connect(backBtn, &QPushButton::clicked, this, &StudentView::onBack);
btnLayout->addWidget(backBtn);
btnLayout->addStretch();
lay->addLayout(btnLayout);
}

void StudentView::onLogout() { 
close(); 
}

void StudentView::onBack() {
if (!navigationStack.isEmpty()) {
QWidget *prevWidget = navigationStack.pop();
prevWidget->showMaximized();
close();
} else {
close();
}
}

void StudentView::updateGradesTable() {
gradesTable->clearContents();
gradesTable->setRowCount(0);

DataStore ds;
auto allGrades = ds.loadGrades();
int row = 0;
QString searchText = searchEdit ? searchEdit->text().toLower() : "";

for (const QString &line : allGrades) {
QStringList parts = line.split('|');
if (parts.size() == 5) {
QString studentName = parts[1];
if (studentName.contains(username) || parts[1] == username) {
// Фильтрация по поиску
if (searchText.isEmpty() || parts[2].toLower().contains(searchText)) {
gradesTable->insertRow(row);
gradesTable->setItem(row, 0, new QTableWidgetItem(parts[2])); // Предмет
gradesTable->setItem(row, 1, new QTableWidgetItem(parts[3])); // Дата
gradesTable->setItem(row, 2, new QTableWidgetItem(parts[4])); // Оценка
gradesTable->setItem(row, 3, new QTableWidgetItem(parts[0])); // Класс
row++;
}
}
}
}
gradesTable->setRowCount(row);
}

void StudentView::onSearchGrades() {
updateGradesTable();
}

void StudentView::onShowStatistics() {
DataStore ds;
auto allGrades = ds.loadGrades();

QMap<QString, QVector<int>> subjectGrades;
int totalGrades = 0;
double sumGrades = 0;

for (const QString &line : allGrades) {
QStringList parts = line.split('|');
if (parts.size() == 5) {
QString studentName = parts[1];
if (studentName.contains(username) || parts[1] == username) {
QString subject = parts[2];
int grade = parts[4].toInt();
if (grade >= 0 && grade <= 10) {
subjectGrades[subject].append(grade);
totalGrades++;
sumGrades += grade;
}
}
}
}

QString stats = "=== Статистика оценок ===\n\n";
stats += QString("Всего оценок: %1\n").arg(totalGrades);
if (totalGrades > 0) {
stats += QString("Средний балл: %1\n\n").arg(QString::number(sumGrades / totalGrades, 'f', 2));
}

stats += "По предметам:\n";
for (auto it = subjectGrades.begin(); it != subjectGrades.end(); ++it) {
QString subject = it.key();
QVector<int> grades = it.value();
double avg = 0;
for (int g : grades) avg += g;
avg /= grades.size();
stats += QString("%1: %2 оценок, средний балл: %3\n")
.arg(subject)
.arg(grades.size())
.arg(QString::number(avg, 'f', 2));
}

QMessageBox::information(this, "Статистика", stats);
}

void StudentView::onExportGrades() {
QString fileName = QFileDialog::getSaveFileName(this, "Экспорт оценок", 
username + "_grades.txt", 
"Текстовые файлы (*.txt)");
if (fileName.isEmpty()) return;

QFile file(fileName);
if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
QMessageBox::warning(this, "Ошибка", "Не удалось создать файл.");
return;
}

QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
out.setCodec("UTF-8");
#endif
out << "Оценки ученика: " << username << "\n";
out << "Дата экспорта: " << QDate::currentDate().toString("dd.MM.yyyy") << "\n\n";
out << "Предмет\tДата\tОценка\tКласс\n";

DataStore ds;
auto allGrades = ds.loadGrades();
for (const QString &line : allGrades) {
QStringList parts = line.split('|');
if (parts.size() == 5) {
QString studentName = parts[1];
if (studentName.contains(username) || parts[1] == username) {
out << parts[2] << "\t" << parts[3] << "\t" << parts[4] << "\t" << parts[0] << "\n";
}
}
}

QMessageBox::information(this, "Успех", "Оценки экспортированы в файл.");
}

