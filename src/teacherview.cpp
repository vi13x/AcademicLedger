#include "teacherview.h"
#include "datastore.h"
#include "gradedialog.h"
#include "homeworkdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QComboBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QDialog>
#include <QMessageBox>
#include <QDate>
#include <QColor>
#include <QFont>
#include <QTimer>
#include <QAbstractItemView>
#include <QTabWidget>
#include <QWidget>
#include <QVector>
#include <QMap>
#include <QString>
#include <QStringList>

TeacherView::TeacherView(const QString &user, QWidget *parent)
: QWidget(parent), username(user)
{
setWindowTitle("Кабинет учителя");
resize(1400, 800);
QVBoxLayout *main = new QVBoxLayout(this);
main->setSpacing(10);
main->setContentsMargins(15, 15, 15, 15);

// Заголовок
// Верхняя панель с заголовком и кнопкой выхода
QHBoxLayout *headerLayout = new QHBoxLayout();
QLabel *title = new QLabel("Кабинет учителя - Классный журнал");
title->setStyleSheet("font-size: 32px; font-weight: bold; color: #e0e0e0; margin-bottom: 10px;");
headerLayout->addWidget(title);
headerLayout->addStretch();
logoutBtn = new QPushButton("Выйти");
logoutBtn->setMinimumHeight(40);
logoutBtn->setMinimumWidth(120);
logoutBtn->setStyleSheet("QPushButton { background-color: #D63031; color: white; font-size: 14px; font-weight: bold; border-radius: 0px; min-height: 40px; padding: 10px 20px; } QPushButton:hover { background-color: #E74C3C; } QPushButton:pressed { background-color: #C0392B; }");
connect(logoutBtn, &QPushButton::clicked, this, &TeacherView::onLogout);
headerLayout->addWidget(logoutBtn);
main->addLayout(headerLayout);

// Верхняя панель с кнопками
QHBoxLayout *topLayout = new QHBoxLayout();
topicsBtn = new QPushButton("Темы уроков и задания");
connect(topicsBtn, &QPushButton::clicked, this, &TeacherView::onTopicsClicked);
topLayout->addWidget(topicsBtn);

// Получаем ФИО учителя
QString teacherFullName = DataStore().getTeacherFullName(username);
QLabel *teacherInfoLabel = new QLabel("УЧИТЕЛЬ: " + teacherFullName);
teacherInfoLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #e0e0e0;");
topLayout->addStretch();
topLayout->addWidget(teacherInfoLabel);
main->addLayout(topLayout);

// Панель выбора параметров
QHBoxLayout *selectLayout = new QHBoxLayout();
selectLayout->setSpacing(16);
selectLayout->addWidget(new QLabel("Класс:"));
classCombo = new QComboBox();
classCombo->setMinimumHeight(40);
auto classes = DataStore().getClasses();
if (!classes.isEmpty()) {
classCombo->addItems(classes);
currentClass = classCombo->itemText(0);
} else {
// Если классов нет, добавляем дефолтный
classCombo->addItem("1а");
currentClass = "1а";
}
connect(classCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
this, &TeacherView::onClassChanged);
selectLayout->addWidget(classCombo);

selectLayout->addWidget(new QLabel("Предмет:"));
subjectCombo = new QComboBox();
subjectCombo->setMinimumHeight(40);
// Полный список школьных дисциплин
QStringList allSubjects = {
"Математика", "Русский язык", "Русская литература", "Английский язык", 
"Физика", "Химия", "Биология", "География", "История", "История Беларуси",
"Всемирная история", "Обществознание", "Информатика", "Физкультура и здоровье",
"Искусство", "Музыка", "ИЗО", "Трудовое обучение", "ЧЗС", "Окружающий мир",
"Белорусский язык", "Белорусская литература", "Чтение", "Классный час"
};
subjectCombo->addItems(allSubjects);
subjectCombo->setEditable(true);
subjectCombo->setInsertPolicy(QComboBox::NoInsert);
if (subjectCombo->count() > 0) {
currentSubject = subjectCombo->itemText(0);
}
connect(subjectCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
this, &TeacherView::onSubjectChanged);
connect(subjectCombo, &QComboBox::currentTextChanged,
this, &TeacherView::onSubjectChanged);
selectLayout->addWidget(subjectCombo);

selectLayout->addWidget(new QLabel("Месяц:"));
monthDateEdit = new QDateEdit();
monthDateEdit->setMinimumHeight(40);
QDate currentDate = QDate::currentDate();
// Устанавливаем первый день текущего месяца
QDate firstDayOfMonth(currentDate.year(), currentDate.month(), 1);
monthDateEdit->setDate(firstDayOfMonth);
monthDateEdit->setCalendarPopup(true);
monthDateEdit->setDisplayFormat("MMMM yyyy");
monthDateEdit->setDateRange(QDate(2020, 1, 1), QDate(2030, 12, 31));
// Устанавливаем минимальный размер для календаря
monthDateEdit->setMinimumWidth(150);
currentMonth = firstDayOfMonth;
connect(monthDateEdit, &QDateEdit::dateChanged, this, &TeacherView::onMonthDateChanged);
selectLayout->addWidget(monthDateEdit);

QPushButton *addTopicBtn = new QPushButton("📝 Добавить тему урока");
addTopicBtn->setStyleSheet("QPushButton { background: #3D7BFF; color: white; padding: 12px 20px; border-radius: 0px; font-weight: bold; font-size: 14px; min-height: 48px; min-width: 180px; } QPushButton:hover { background: #5A8FFF; }");
connect(addTopicBtn, &QPushButton::clicked, this, &TeacherView::onAddTopicClicked);
selectLayout->addWidget(addTopicBtn);

selectLayout->addStretch();
main->addLayout(selectLayout);


// Таблица журнала
journalTable = new QTableWidget();
journalTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Убираем прямое редактирование
journalTable->setSelectionBehavior(QAbstractItemView::SelectItems);
journalTable->setStyleSheet(R"(
QTableWidget {
background: #1E1E1E;
border: 1px solid #333333;
gridline-color: #2a2a2a;
alternate-background-color: #232323;
}
QTableWidget::item {
padding: 6px;
border: 1px solid #2a2a2a;
color: #e0e0e0;
}
QTableWidget::item:hover {
background-color: #333333;
}
QTableWidget::item:selected {
background: #3D7BFF;
color: white;
}
QHeaderView::section {
background: #2D2D2D;
color: #e0e0e0;
padding: 10px 6px;
border: 1px solid #333333;
font-weight: bold;
font-size: 11px;
}
)");
// Обработчик клика на ячейку для выбора отметки
connect(journalTable, &QTableWidget::cellClicked, this, &TeacherView::onCellClicked);
// Обработчик двойного клика на заголовок для редактирования темы урока
connect(journalTable->horizontalHeader(), &QHeaderView::sectionDoubleClicked, 
this, &TeacherView::onHeaderDoubleClicked);

// Добавляем вкладки для журнала и расписания
tabs = new QTabWidget();

// Вкладка журнала
QWidget *journalTab = new QWidget();
QVBoxLayout *journalTabLayout = new QVBoxLayout(journalTab);
journalTabLayout->setContentsMargins(0, 0, 0, 0);
journalTabLayout->addWidget(journalTable);
tabs->addTab(journalTab, "Журнал");

// Вкладка расписания
QWidget *scheduleTab = new QWidget();
QVBoxLayout *scheduleTabLayout = new QVBoxLayout(scheduleTab);

QLabel *scheduleLabel = new QLabel("Расписание класса: " + (classCombo->count() > 0 ? classCombo->currentText() : ""));
scheduleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin-bottom: 10px; color: #e0e0e0;");
scheduleTabLayout->addWidget(scheduleLabel);

QTableWidget *scheduleTable = new QTableWidget();
scheduleTable->setColumnCount(5);
QStringList scheduleHeaders = {"Время", "Урок", "Кабинет", "ДЗ", ""};
scheduleTable->setHorizontalHeaderLabels(scheduleHeaders);
if (scheduleTable->horizontalHeader()) {
scheduleTable->horizontalHeader()->setStretchLastSection(true);
}
scheduleTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

// Обработчик изменения ДЗ (подключаем до загрузки данных)
connect(scheduleTable, &QTableWidget::cellChanged, [=](int row, int col) {
if (col == 3) { // Колонка ДЗ
QTableWidgetItem *item = scheduleTable->item(row, col);
if (!item) return;

QString homework = item->text();
QString lessonName = "";
QTableWidgetItem *lessonItem = scheduleTable->item(row, 1);
if (lessonItem) {
lessonName = lessonItem->text().trimmed();
}

if (!lessonName.isEmpty() && lessonName != "-" && classCombo && classCombo->count() > 0) {
// Используем текущую дату для ДЗ
QDate date = QDate::currentDate();
DataStore().addHomework(classCombo->currentText(), lessonName, homework, date.toString("yyyy-MM-dd"));
}
}
});

scheduleTabLayout->addWidget(scheduleTable);
tabs->addTab(scheduleTab, "Расписание");

// Вкладка оценок класса
QWidget *gradesTab = new QWidget();
QVBoxLayout *gradesTabLayout = new QVBoxLayout(gradesTab);
gradesTabLayout->setContentsMargins(0, 0, 0, 0);

gradesTable = new QTableWidget();
gradesTable->setColumnCount(5);
QStringList gradesHeaders = {"Фамилия Имя", "Предмет", "Оценка", "Дата", "Действие"};
gradesTable->setHorizontalHeaderLabels(gradesHeaders);
gradesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
gradesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
gradesTable->setStyleSheet(R"(
QTableWidget {
background: #1E1E1E;
border: 1px solid #333333;
gridline-color: #2a2a2a;
alternate-background-color: #232323;
}
QTableWidget::item {
padding: 6px;
border: 1px solid #2a2a2a;
color: #e0e0e0;
}
QTableWidget::item:hover {
background-color: #333333;
}
QTableWidget::item:selected {
background: #3D7BFF;
color: white;
}
QHeaderView::section {
background: #2D2D2D;
color: #e0e0e0;
padding: 10px 6px;
border: 1px solid #333333;
font-weight: bold;
font-size: 11px;
}
)");
if (gradesTable->horizontalHeader()) {
gradesTable->horizontalHeader()->setStretchLastSection(true);
}
gradesTabLayout->addWidget(gradesTable);
tabs->addTab(gradesTab, "Оценки класса");

main->addWidget(tabs);

// Загружаем журнал после создания всех виджетов
if (!currentClass.isEmpty() && !currentSubject.isEmpty() && currentMonth.isValid()) {
loadJournal();
}

// Загружаем оценки класса после создания таблицы
if (!currentClass.isEmpty() && gradesTable) {
loadGradesForClass(currentClass);
}

// Загружаем и отображаем расписание (после подключения сигналов и создания всех виджетов)
// Используем QTimer::singleShot для отложенного вызова, чтобы убедиться, что все виджеты созданы
QTimer::singleShot(100, [=]() {
if (!scheduleTable) return;
QString initialClass = classCombo->count() > 0 ? classCombo->currentText() : "";
if (!initialClass.isEmpty()) {
updateScheduleTable(scheduleTable, initialClass);
}
});

// Обновляем расписание при изменении класса
connect(classCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int) {
if (!scheduleTable) return;
QString selectedClass = classCombo->currentText();
if (!selectedClass.isEmpty()) {
updateScheduleTable(scheduleTable, selectedClass);
if (scheduleLabel) {
scheduleLabel->setText("Расписание класса: " + selectedClass);
}
}
});

// Кнопка "Назад" внизу
QHBoxLayout *btnLayout = new QHBoxLayout();
backBtn = new QPushButton("Назад");
backBtn->setMinimumHeight(40);
backBtn->setMinimumWidth(120);
backBtn->setStyleSheet("QPushButton { background-color: #444444; color: white; font-size: 14px; font-weight: bold; border-radius: 0px; padding: 10px 16px; } QPushButton:hover { background-color: #555555; } QPushButton:pressed { background-color: #333333; }");
connect(backBtn, &QPushButton::clicked, this, &TeacherView::onBack);
btnLayout->addWidget(backBtn);
btnLayout->addStretch();
main->addLayout(btnLayout);
}

void TeacherView::onClassChanged() {
currentClass = classCombo->currentText();
// Обновляем список учеников
loadJournal();
// Обновляем таблицу оценок класса
if (gradesTable) {
loadGradesForClass(currentClass);
}
}

void TeacherView::onSubjectChanged() {
currentSubject = subjectCombo->currentText();
loadJournal();
}

void TeacherView::onMonthChanged() {
// Старый метод для совместимости
if (monthDateEdit) {
currentMonth = monthDateEdit->date();
loadJournal();
}
}

void TeacherView::onMonthDateChanged(const QDate &date) {
// Устанавливаем первый день выбранного месяца
QDate firstDayOfMonth(date.year(), date.month(), 1);
currentMonth = firstDayOfMonth;
loadJournal();
}

void TeacherView::loadJournal() {
if (currentClass.isEmpty() || currentSubject.isEmpty() || !currentMonth.isValid()) {
return;
}

updateJournalTable();
}

void TeacherView::updateJournalTable() {
if (!journalTable) return;

// Блокируем сигналы, чтобы избежать бесконечного цикла
journalTable->blockSignals(true);
journalTable->clear();

// Получаем список учеников класса
auto students = DataStore().getStudentsForClass(currentClass);
if (students.isEmpty()) {
journalTable->setRowCount(0);
journalTable->setColumnCount(0);
return;
}

// Получаем даты для месяца
QVector<QDate> dates = getDatesForMonth(currentMonth);

// Настройка таблицы
int dateCols = dates.size();
journalTable->setColumnCount(1 + dateCols + 2); // Ученики + даты + средняя + за период
journalTable->setRowCount(students.size());

QStringList headers;
headers << "№" << "Фамилия Имя";
QVector<QString> dayTopics; // Сохраняем темы для каждого дня
for (int day = 1; day <= dates.size(); day++) {
QDate checkDate = dates[day - 1];
QString dateStr = checkDate.toString("yyyy-MM-dd");

// Ищем тему урока для этого дня и предмета
QString topic = DataStore().getLessonTopic(currentClass, currentSubject, dateStr);

// Формируем заголовок: число + тема (если есть)
QString header = QString::number(day);
if (!topic.isEmpty()) {
header += "\n" + topic;
}
headers << header;
dayTopics.push_back(topic);
}
headers << "Средняя оценка" << "Оценка за период";
journalTable->setHorizontalHeaderLabels(headers);

// Улучшаем стиль заголовков для отображения тем
journalTable->horizontalHeader()->setDefaultSectionSize(80);
journalTable->horizontalHeader()->setMinimumSectionSize(50);
for (int col = 2; col < headers.size() - 2; ++col) {
journalTable->setColumnWidth(col, 80);
}

// Загружаем оценки (в файле хранится логин ученика)
auto allGrades = DataStore().loadGrades();
QMap<QString, QMap<QDate, QString>> studentGradesByLogin; // логин -> дата -> оценка

for (const QString &line : allGrades) {
QStringList parts = line.split('|');
if (parts.size() == 5 && parts[0] == currentClass && parts[2] == currentSubject) {
QString studentLogin = parts[1]; // В файле хранится логин
QDate gradeDate = QDate::fromString(parts[3], "yyyy-MM-dd");
QString grade = parts[4];
studentGradesByLogin[studentLogin][gradeDate] = grade;
}
}

// Загружаем отметки за дисциплину (формат: student|subject|mark|date)
auto disciplineMarks = DataStore().loadDisciplineMarks();
QMap<QString, QMap<QDate, QString>> studentDisciplineMarks; // логин -> дата -> отметка

for (const QString &line : disciplineMarks) {
QStringList parts = line.split('|');
// Поддерживаем старый формат (3 части) и новый (4 части)
if (parts.size() == 3) {
// Старый формат: student|mark|date
QString studentLogin = parts[0];
QString mark = parts[1];
QDate markDate = QDate::fromString(parts[2], "yyyy-MM-dd");
studentDisciplineMarks[studentLogin][markDate] = mark;
} else if (parts.size() == 4) {
// Новый формат: student|subject|mark|date
QString studentLogin = parts[0];
QString subject = parts[1];
QString mark = parts[2];
QDate markDate = QDate::fromString(parts[3], "yyyy-MM-dd");
// Показываем отметку только если предмет совпадает с текущим
if (subject == currentSubject) {
studentDisciplineMarks[studentLogin][markDate] = mark;
}
}
}

// Заполняем таблицу
for (int row = 0; row < students.size(); row++) {
QString studentName = students[row];
// Извлекаем только ФИО без логина в скобках
QString displayName = studentName;
QString studentLogin = "";
if (studentName.contains('(') && studentName.contains(')')) {
displayName = studentName.left(studentName.indexOf('(')).trimmed();
int start = studentName.indexOf('(') + 1;
int end = studentName.indexOf(')');
studentLogin = studentName.mid(start, end - start);
}

// №
QTableWidgetItem *numItem = new QTableWidgetItem(QString::number(row + 1));
numItem->setTextAlignment(Qt::AlignCenter);
numItem->setFlags(numItem->flags() & ~Qt::ItemIsEditable);
QFont numFont = numItem->font();
numFont.setBold(true);
numItem->setFont(numFont);
journalTable->setItem(row, 0, numItem);

// Имя ученика
QTableWidgetItem *nameItem = new QTableWidgetItem(displayName);
nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
journalTable->setItem(row, 1, nameItem);

// Оценки по датам
QVector<int> validGrades;
for (int col = 0; col < dates.size(); col++) {
QDate date = dates[col];
QString grade = "";

// Ищем оценку по логину
if (!studentLogin.isEmpty()) {
grade = studentGradesByLogin[studentLogin].value(date, "");
}

// Проверяем отметку за дисциплину для этой даты
if (grade.isEmpty() && !studentLogin.isEmpty()) {
QString disciplineMark = studentDisciplineMarks[studentLogin].value(date, "");
if (!disciplineMark.isEmpty()) {
grade = disciplineMark; // 1, 2, ..., 10 (без префикса Д:)
}
}

QTableWidgetItem *gradeItem = new QTableWidgetItem(grade);
if (grade.isEmpty()) {
gradeItem->setText("");
}
// Устанавливаем tooltip с датой
gradeItem->setToolTip(date.toString("dd.MM.yyyy"));
// Делаем ячейку нередактируемой напрямую (редактирование через диалог)
gradeItem->setFlags(gradeItem->flags() & ~Qt::ItemIsEditable);
journalTable->setItem(row, col + 2, gradeItem); // +2 потому что теперь есть колонка №

if (!grade.isEmpty() && grade != "н") {
bool ok;
int g = grade.toInt(&ok);
if (ok && g >= 1 && g <= 10) {
validGrades.append(g);
}
}
}

// Средняя оценка
double avg = 0;
if (!validGrades.isEmpty()) {
for (int g : validGrades) avg += g;
avg /= validGrades.size();
}
QTableWidgetItem *avgItem = new QTableWidgetItem(avg > 0 ? QString::number(avg, 'f', 2) : "");
avgItem->setFlags(avgItem->flags() & ~Qt::ItemIsEditable);
QFont avgFont = avgItem->font();
avgFont.setBold(true);
avgItem->setFont(avgFont);
journalTable->setItem(row, dateCols + 2, avgItem); // +2 потому что есть колонка №

// Оценка за период
int periodGrade = calculatePeriodGrade(avg);
QTableWidgetItem *periodItem = new QTableWidgetItem(periodGrade > 0 ? QString::number(periodGrade) : "");
periodItem->setFlags(periodItem->flags() & ~Qt::ItemIsEditable);
QFont periodFont = periodItem->font();
periodFont.setBold(true);
periodFont.setPointSize(14);
periodItem->setFont(periodFont);
journalTable->setItem(row, dateCols + 3, periodItem); // +3 потому что есть колонка №
}

journalTable->resizeColumnsToContents();
if (journalTable->horizontalHeader()) {
journalTable->horizontalHeader()->setStretchLastSection(true);
}
// Разблокируем сигналы
journalTable->blockSignals(false);
}

QVector<QDate> TeacherView::getDatesForMonth(const QDate &monthDate) {
QVector<QDate> dates;
if (!monthDate.isValid()) {
return dates;
}

int year = monthDate.year();
int month = monthDate.month();

// Проверяем валидность года и месяца
if (year < 2000 || year > 2100 || month < 1 || month > 12) {
return dates;
}

QDate firstDay(year, month, 1);
if (!firstDay.isValid()) {
return dates;
}

int daysInMonth = firstDay.daysInMonth();

// Ограничиваем количество дней (максимум 31)
if (daysInMonth > 31) {
daysInMonth = 31;
}

// Генерируем даты от 1 до последнего дня месяца
for (int day = 1; day <= daysInMonth; day++) {
QDate date(year, month, day);
if (date.isValid()) {
dates.append(date);
}
}

return dates;
}

double TeacherView::calculateAverageForStudent(const QString &studentName, const QString &subject) {
auto allGrades = DataStore().loadGrades();
QVector<int> grades;

for (const QString &line : allGrades) {
QStringList parts = line.split('|');
if (parts.size() == 5 && parts[1] == studentName && parts[2] == subject) {
bool ok;
int grade = parts[4].toInt(&ok);
if (ok && grade >= 0 && grade <= 10) {
grades.append(grade);
}
}
}

if (grades.isEmpty()) return 0;
double sum = 0;
for (int g : grades) sum += g;
return sum / grades.size();
}

int TeacherView::calculatePeriodGrade(double average) {
if (average == 0) return 0;
// Для 10-балльной системы
if (average >= 9.5) return 10;
if (average >= 8.5) return 9;
if (average >= 7.5) return 8;
if (average >= 6.5) return 7;
if (average >= 5.5) return 6;
if (average >= 4.5) return 5;
if (average >= 3.5) return 4;
if (average >= 2.5) return 3;
if (average >= 1.5) return 2;
return 1;
}

void TeacherView::onCellClicked(int row, int col) {
// Игнорируем клики на №, имя, среднюю и период
if (col <= 1 || col >= journalTable->columnCount() - 2) return;

QTableWidgetItem *item = journalTable->item(row, col);
if (!item) return;

QString studentName = journalTable->item(row, 1)->text(); // Имя теперь в колонке 1

// Находим полное имя с логином
auto students = DataStore().getStudentsForClass(currentClass);
QString fullStudentName = studentName;
for (const QString &s : students) {
if (s.contains(studentName)) {
fullStudentName = s;
break;
}
}

// Извлекаем логин
QString studentLogin = fullStudentName;
if (fullStudentName.contains('(') && fullStudentName.contains(')')) {
int start = fullStudentName.indexOf('(') + 1;
int end = fullStudentName.indexOf(')');
studentLogin = fullStudentName.mid(start, end - start);
}

// Получаем дату для этой колонки
QVector<QDate> dates = getDatesForMonth(currentMonth);
if (col - 2 >= dates.size()) return; // -2 потому что есть колонки № и ФИО

QDate date = dates[col - 2];
QString dateStr = date.toString("yyyy-MM-dd");

// Открываем диалог выбора отметки
GradeDialog dlg(this);
// Устанавливаем текущую отметку, если она есть
QString currentGrade = item->text().trimmed();
dlg.setCurrentGrade(currentGrade);

if (dlg.exec() == QDialog::Accepted) {
QString grade = "";
if (!dlg.isEmpty()) {
grade = dlg.getGrade();
}

// Сохраняем оценку в файл
if (DataStore().addGrade(currentClass, studentLogin, currentSubject, dateStr, grade)) {
// Обновляем таблицу, чтобы оценка сразу отобразилась
updateJournalTable();
}
}
}

void TeacherView::onHeaderDoubleClicked(int col) {
// Колонки 0 и 1 - это № и ФИО, их не редактируем
if (col < 2) return;

if (currentClass.isEmpty() || currentSubject.isEmpty()) {
QMessageBox::information(this, "Ошибка", "Выберите класс и предмет");
return;
}

QVector<QDate> dates = getDatesForMonth(currentMonth);
int day = col - 2; // -2 потому что есть колонки № и ФИО
if (day < 0 || day >= dates.size()) return;

QDate checkDate = dates[day];
QString dateStr = checkDate.toString("yyyy-MM-dd");

// Ищем существующую тему
QString existingTopic = DataStore().getLessonTopic(currentClass, currentSubject, dateStr);

// Диалог редактирования темы
QDialog topicDlg(this);
topicDlg.setWindowTitle("Тема урока");
topicDlg.resize(500, 300);
QVBoxLayout *vTopic = new QVBoxLayout;

QLabel *lblDate = new QLabel(QString("Дата: %1").arg(checkDate.toString("dd.MM.yyyy (dddd)")));
lblDate->setStyleSheet("font-weight: bold; font-size: 13px; color: #3D7BFF; padding: 5px;");

QLabel *lblTopic = new QLabel("Тема урока:");
QTextEdit *teTopic = new QTextEdit;
teTopic->setPlainText(existingTopic);
teTopic->setMaximumHeight(120);

QHBoxLayout *hTopic = new QHBoxLayout;
QPushButton *btnOk = new QPushButton("Сохранить");
QPushButton *btnDelete = new QPushButton("Удалить");
QPushButton *btnCancel = new QPushButton("Отмена");
btnOk->setStyleSheet("QPushButton { background: #3D7BFF; color: white; padding: 12px 20px; font-weight: bold; border-radius: 0px; min-height: 40px; } QPushButton:hover { background: #5A8FFF; }");
btnDelete->setStyleSheet("QPushButton { background: #f44336; color: white; padding: 8px; font-weight: bold; } QPushButton:hover { background: #e57373; }");
btnCancel->setStyleSheet("QPushButton { background: #424242; color: white; padding: 8px; } QPushButton:hover { background: #616161; }");
hTopic->addStretch();
hTopic->addWidget(btnOk);
hTopic->addWidget(btnDelete);
hTopic->addWidget(btnCancel);

vTopic->addWidget(lblDate);
vTopic->addWidget(lblTopic);
vTopic->addWidget(teTopic);
vTopic->addLayout(hTopic);
topicDlg.setLayout(vTopic);

connect(btnOk, &QPushButton::clicked, &topicDlg, [&]() {
QString topic = teTopic->toPlainText().trimmed();
if (topic.isEmpty()) {
DataStore().deleteLessonTopic(currentClass, currentSubject, dateStr);
} else {
DataStore().updateLessonTopic(currentClass, currentSubject, dateStr, topic, username);
}
updateJournalTable();
topicDlg.accept();
});

connect(btnDelete, &QPushButton::clicked, &topicDlg, [&]() {
DataStore().deleteLessonTopic(currentClass, currentSubject, dateStr);
updateJournalTable();
topicDlg.accept();
});

connect(btnCancel, &QPushButton::clicked, &topicDlg, &QDialog::reject);
topicDlg.exec();
}

void TeacherView::onAddTopicClicked() {
if (currentClass.isEmpty() || currentSubject.isEmpty()) {
QMessageBox::information(this, "Ошибка", "Выберите класс и предмет");
return;
}

QDialog topicDlg(this);
topicDlg.setWindowTitle("Добавить тему урока");
topicDlg.resize(500, 300);
QVBoxLayout *vTopic = new QVBoxLayout;

QLabel *lblDate = new QLabel("Дата урока:");
QComboBox *cbDate = new QComboBox;
QVector<QDate> dates = getDatesForMonth(currentMonth);
for (const QDate &date : dates) {
cbDate->addItem(date.toString("dd.MM.yyyy (dddd)"), date.toString("yyyy-MM-dd"));
}

QLabel *lblTopic = new QLabel("Тема урока:");
QTextEdit *teTopic = new QTextEdit;
teTopic->setPlainText("");
teTopic->setMaximumHeight(100);

QHBoxLayout *hTopic = new QHBoxLayout;
QPushButton *btnOk = new QPushButton("Сохранить");
QPushButton *btnCancel = new QPushButton("Отмена");
btnOk->setStyleSheet("QPushButton { background: #3D7BFF; color: white; padding: 12px 20px; font-weight: bold; border-radius: 0px; min-height: 40px; } QPushButton:hover { background: #5A8FFF; }");
btnCancel->setStyleSheet("QPushButton { background: #424242; color: white; padding: 8px; } QPushButton:hover { background: #616161; }");
hTopic->addStretch();
hTopic->addWidget(btnOk);
hTopic->addWidget(btnCancel);

vTopic->addWidget(lblDate);
vTopic->addWidget(cbDate);
vTopic->addWidget(lblTopic);
vTopic->addWidget(teTopic);
vTopic->addLayout(hTopic);
topicDlg.setLayout(vTopic);

connect(btnOk, &QPushButton::clicked, &topicDlg, [&]() {
QString dateStr = cbDate->currentData().toString();
QString topic = teTopic->toPlainText().trimmed();
if (topic.isEmpty()) {
QMessageBox::warning(&topicDlg, "Ошибка", "Тема урока не может быть пустой");
return;
}
DataStore().updateLessonTopic(currentClass, currentSubject, dateStr, topic, username);
updateJournalTable();
topicDlg.accept();
});

connect(btnCancel, &QPushButton::clicked, &topicDlg, &QDialog::reject);
topicDlg.exec();
}

void TeacherView::onTopicsClicked() {
HomeworkDialog dlg(currentClass, this);
if (dlg.exec() == QDialog::Accepted) {
QString subject = dlg.getSubject();
QString homework = dlg.getHomework();
QString dueDate = dlg.getDueDate();

if (!subject.isEmpty() && !homework.isEmpty()) {
DataStore().addHomework(currentClass, subject, homework, dueDate);
QMessageBox::information(this, "Успех", "Домашнее задание добавлено.");
}
}
}


void TeacherView::onLogout() { 
close(); 
}

void TeacherView::onBack() {
if (!navigationStack.isEmpty()) {
QWidget *prevWidget = navigationStack.pop();
prevWidget->showMaximized();
close();
} else {
close();
}
}


void TeacherView::onAddHomework() {
// Реализация добавления домашнего задания
if (currentClass.isEmpty() || currentSubject.isEmpty()) {
QMessageBox::information(this, "Ошибка", "Выберите класс и предмет");
return;
}

QDialog hwDlg(this);
hwDlg.setWindowTitle("Добавить домашнее задание");
hwDlg.resize(500, 300);
QVBoxLayout *vLayout = new QVBoxLayout(&hwDlg);

QLabel *lblText = new QLabel("Текст задания:");
QTextEdit *textEdit = new QTextEdit();
textEdit->setPlainText("");
textEdit->setMaximumHeight(120);

QHBoxLayout *hLayout = new QHBoxLayout();
QPushButton *btnOk = new QPushButton("Добавить");
QPushButton *btnCancel = new QPushButton("Отмена");
hLayout->addStretch();
hLayout->addWidget(btnOk);
hLayout->addWidget(btnCancel);

vLayout->addWidget(lblText);
vLayout->addWidget(textEdit);
vLayout->addLayout(hLayout);

connect(btnOk, &QPushButton::clicked, &hwDlg, &QDialog::accept);
connect(btnCancel, &QPushButton::clicked, &hwDlg, &QDialog::reject);

if (hwDlg.exec() == QDialog::Accepted) {
QString text = textEdit->toPlainText().trimmed();
if (text.isEmpty()) {
QMessageBox::warning(this, "Ошибка", "Текст задания не может быть пустым");
return;
}

QDate date = QDate::currentDate();
if (DataStore().addHomework(currentClass, currentSubject, 
date.toString("yyyy-MM-dd"), text)) {
QMessageBox::information(this, "Успех", "Домашнее задание добавлено.");
} else {
QMessageBox::warning(this, "Ошибка", "Не удалось добавить задание.");
}
}
}

void TeacherView::onPeriodChanged() {
// Реализация изменения периода (можно использовать onMonthChanged)
onMonthChanged();
}

void TeacherView::updateScheduleTable(QTableWidget *table, const QString &className) {
if (!table) return;

table->blockSignals(true); // Блокируем сигналы во время обновления
table->clear();
table->setRowCount(0);
table->setColumnCount(5);
QStringList scheduleHeaders = {"Время", "Урок", "Кабинет", "ДЗ", ""};
table->setHorizontalHeaderLabels(scheduleHeaders);

if (className.isEmpty()) {
table->blockSignals(false);
return;
}

// Загружаем домашние задания
auto homeworkList = DataStore().loadHomework();
QMap<QString, QString> homeworkMap; // предмет -> ДЗ
for (const QString &line : homeworkList) {
QStringList parts = line.split('|');
if (parts.size() >= 4 && parts[0] == className) {
QString subject = parts[1];
QString homework = parts[2];
homeworkMap[subject] = homework;
}
}

// Загружаем расписание для класса
auto scheduleLines = DataStore().loadSchedule(className);

// Создаем структуру расписания по дням недели
QStringList dayNames = {"Понедельник", "Вторник", "Среда", "Четверг", "Пятница"};
QStringList dayNamesUpper = {"ПОНЕДЕЛЬНИК", "ВТОРНИК", "СРЕДА", "ЧЕТВЕРГ", "ПЯТНИЦА"};
QStringList dayColors = {"#3D7BFF", "#1e1e1e", "#5A8FFF", "#1e1e1e", "#5A8FFF"};
QStringList dayTextColors = {"#ffffff", "#e0e0e0", "#ffffff", "#e0e0e0", "#ffffff"};

int totalRows = 0;
for (const QString &day : dayNamesUpper) {
totalRows += 7; // 6 уроков + заголовок дня
}
table->setRowCount(totalRows);

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
dayHeader->setTextAlignment(Qt::AlignCenter);
table->setItem(currentRow, 0, dayHeader);
table->setSpan(currentRow, 0, 1, 5);
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
QTableWidgetItem *timeItem = new QTableWidgetItem(times[lesson]);
timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
timeItem->setTextAlignment(Qt::AlignCenter);
table->setItem(currentRow, 0, timeItem);

QString lessonText = lesson < lessons.size() ? lessons[lesson] : "";
QString classroom = lesson < classrooms.size() ? classrooms[lesson] : "";

if (lessonText.isEmpty()) {
lessonText = "-";
}

QTableWidgetItem *lessonItem = new QTableWidgetItem(lessonText);
lessonItem->setFlags(lessonItem->flags() & ~Qt::ItemIsEditable);
lessonItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
table->setItem(currentRow, 1, lessonItem);

QTableWidgetItem *classroomItem = new QTableWidgetItem(classroom);
classroomItem->setFlags(classroomItem->flags() & ~Qt::ItemIsEditable);
classroomItem->setTextAlignment(Qt::AlignCenter);
table->setItem(currentRow, 2, classroomItem);

// Колонка ДЗ (редактируемая)
QString homeworkText = homeworkMap.value(lessonText, "");
QTableWidgetItem *homeworkItem = new QTableWidgetItem(homeworkText);
homeworkItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
homeworkItem->setToolTip("Двойной клик для редактирования ДЗ");
table->setItem(currentRow, 3, homeworkItem);

currentRow++;
}
}

table->resizeColumnsToContents();
table->setAlternatingRowColors(true);
if (table->verticalHeader()) {
table->verticalHeader()->setVisible(false);
}
if (table->horizontalHeader()) {
table->horizontalHeader()->setStretchLastSection(true);
}
table->setStyleSheet(R"(
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
QHeaderView::section {
background-color: #2D2D2D;
padding: 12px;
border: 1px solid #333333;
font-weight: bold;
font-size: 13px;
color: #e0e0e0;
}
)");
table->blockSignals(false); // Разблокируем сигналы
}

void TeacherView::updateStudentsList() {
// Not used in current implementation, but declared
}

void TeacherView::loadGradesForClass(const QString &className) {
// Шаг 1. Начало
// Шаг 2. Очистить таблицу оценок
if (!gradesTable) return;
gradesTable->setRowCount(0);

// Шаг 3. Создать объект класса DataStore
DataStore ds;

// Шаг 4. Вызвать метод ds.loadGrades() и получить список строк с оценками
List<QString> grades = ds.loadGrades();

// Шаг 5. Начать цикл перебора всех строк списка grades
int row = 0;
for (int i = 0; i < grades.size(); i++) {
QString line = grades[i];

// Шаг 6. Разделить текущую строку на части по разделителю
QStringList parts;
if (line.contains('|')) {
parts = line.split('|');
} else if (line.contains(',')) {
parts = line.split(',');
} else {
parts = line.split(' ');
}

// Проверяем формат: класс|ученик|предмет|дата|оценка или класс,ученик,предмет,дата,оценка
if (parts.size() < 5) continue;

QString gradeClass = parts[0].trimmed();
QString studentName = parts[1].trimmed();
QString subject = parts[2].trimmed();
QString date = parts[3].trimmed();
QString grade = parts[4].trimmed();

// Шаг 7. Сравнить класс в строке с параметром className
// Шаг 8. Если класс не совпадает — перейти к следующему элементу цикла
if (gradeClass != className) {
continue;
}

// Шаг 9. Вставить новую строку в таблицу
gradesTable->insertRow(row);

// Шаг 10. Добавить в таблицу фамилию и имя ученика
QTableWidgetItem *nameItem = new QTableWidgetItem(studentName);
nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
gradesTable->setItem(row, 0, nameItem);

// Шаг 11. Добавить предмет
QTableWidgetItem *subjectItem = new QTableWidgetItem(subject);
subjectItem->setFlags(subjectItem->flags() & ~Qt::ItemIsEditable);
gradesTable->setItem(row, 1, subjectItem);

// Шаг 12. Добавить оценку
QTableWidgetItem *gradeItem = new QTableWidgetItem(grade);
gradeItem->setFlags(gradeItem->flags() & ~Qt::ItemIsEditable);
gradeItem->setTextAlignment(Qt::AlignCenter);
gradesTable->setItem(row, 2, gradeItem);

// Шаг 13. Добавить дату
QTableWidgetItem *dateItem = new QTableWidgetItem(date);
dateItem->setFlags(dateItem->flags() & ~Qt::ItemIsEditable);
gradesTable->setItem(row, 3, dateItem);

// Шаг 14. Создать кнопку «Редактировать» и вставить её в последний столбец
QPushButton *editBtn = new QPushButton("Редактировать");
editBtn->setStyleSheet("QPushButton { background-color: #3D7BFF; color: white; padding: 6px 12px; border-radius: 0px; font-size: 12px; } QPushButton:hover { background-color: #5A8FFF; }");
gradesTable->setCellWidget(row, 4, editBtn);

// Подключаем обработчик кнопки редактирования
connect(editBtn, &QPushButton::clicked, [=]() {
// Открываем диалог редактирования оценки
GradeDialog dlg(this);
dlg.setCurrentGrade(grade);
if (dlg.exec() == QDialog::Accepted) {
QString newGrade = dlg.isEmpty() ? "" : dlg.getGrade();
// Сохраняем изменённую оценку
if (ds.addGrade(className, studentName, subject, date, newGrade)) {
// Обновляем таблицу
loadGradesForClass(className);
}
}
});

row++;
// Шаг 15. Конец цикла по строкам
}

// Шаг 16. Обновить таблицу оценок
gradesTable->resizeColumnsToContents();
if (gradesTable->horizontalHeader()) {
gradesTable->horizontalHeader()->setStretchLastSection(true);
}
// Шаг 17. Конец
}

