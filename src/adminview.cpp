#include "adminview.h"
#include "datastore.h"
#include "addteacherdialog.h"
#include "addstudentdialog.h"
#include "disciplinemarkdialog.h"
#include "user.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QColor>
#include <QFont>
#include <QAbstractItemView>
#include <QTabWidget>
#include <QWidget>
#include <QVector>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QIODevice>
#include <QDialog>


AdminView::AdminView(const QString &user, QWidget *parent)
: QWidget(parent), username(user)
{
setWindowTitle("Кабинет администратора");
resize(1400, 800);
QVBoxLayout *main = new QVBoxLayout(this);

// Верхняя панель с заголовком и кнопкой выхода
QHBoxLayout *headerLayout = new QHBoxLayout();
QLabel *title = new QLabel("Кабинет администратора: " + username);
title->setStyleSheet("font-size: 26px; font-weight: bold; color: #e0e0e0;");
headerLayout->addWidget(title);
headerLayout->addStretch();
QPushButton *logout = new QPushButton("Выйти");
logout->setMinimumHeight(40);
logout->setMinimumWidth(120);
logout->setStyleSheet("QPushButton { background-color: #D63031; color: white; font-size: 14px; font-weight: bold; border-radius: 0px; min-height: 40px; padding: 10px 20px; } QPushButton:hover { background-color: #E74C3C; } QPushButton:pressed { background-color: #C0392B; }");
connect(logout, &QPushButton::clicked, this, &AdminView::onLogout);
headerLayout->addWidget(logout);
main->addLayout(headerLayout);

mainTabs = new QTabWidget();

// ---- Teachers tab ----
QWidget *teachTab = new QWidget();
QVBoxLayout *tLay = new QVBoxLayout(teachTab);

// Поиск учителей
QHBoxLayout *teacherSearchLayout = new QHBoxLayout();
teacherSearchEdit = new QLineEdit();
teacherSearchEdit->setPlaceholderText("Поиск учителей...");
connect(teacherSearchEdit, &QLineEdit::textChanged, this, &AdminView::onSearchTeachers);
teacherSearchLayout->addWidget(new QLabel("Поиск:"));
teacherSearchLayout->addWidget(teacherSearchEdit);
tLay->addLayout(teacherSearchLayout);

teachersList = new QListWidget();
teachersList->addItems(DataStore().getTeachers());
tLay->addWidget(teachersList);

QPushButton *addT  = new QPushButton("Добавить учителя");
QPushButton *remT  = new QPushButton("Удалить учителя");

connect(addT, &QPushButton::clicked, this, &AdminView::onAddTeacher);
connect(remT, &QPushButton::clicked, this, &AdminView::onRemoveTeacher);

tLay->addWidget(addT);
tLay->addWidget(remT);

mainTabs->addTab(teachTab, "Учителя");

// ---- Admins tab ----
QWidget *admTab = new QWidget();
QVBoxLayout *aLay = new QVBoxLayout(admTab);

// Поиск админов
QHBoxLayout *adminSearchLayout = new QHBoxLayout();
adminSearchEdit = new QLineEdit();
adminSearchEdit->setPlaceholderText("Поиск администраторов...");
connect(adminSearchEdit, &QLineEdit::textChanged, this, &AdminView::onSearchAdmins);
adminSearchLayout->addWidget(new QLabel("Поиск:"));
adminSearchLayout->addWidget(adminSearchEdit);
aLay->addLayout(adminSearchLayout);

adminsList = new QListWidget();
adminsList->addItems(DataStore().getAdmins());
aLay->addWidget(adminsList);

QPushButton *addA = new QPushButton("Добавить админа");
QPushButton *remA = new QPushButton("Удалить админа");
QPushButton *refreshA = new QPushButton("Обновить список");

connect(addA, &QPushButton::clicked, this, &AdminView::onAddAdmin);
connect(remA, &QPushButton::clicked, this, &AdminView::onRemoveAdmin);
connect(refreshA, &QPushButton::clicked, this, &AdminView::onRefreshAdmins);

aLay->addWidget(addA);
aLay->addWidget(remA);
aLay->addWidget(refreshA);

mainTabs->addTab(admTab, "Администраторы");

// ---- Schedule ----
QWidget *schTab = new QWidget();
QVBoxLayout *sLay = new QVBoxLayout(schTab);

// Выбор класса для расписания
QHBoxLayout *scheduleClassLayout = new QHBoxLayout();
scheduleClassLayout->addWidget(new QLabel("Класс:"));
scheduleClassCombo = new QComboBox();
scheduleClassCombo->addItems(DataStore().getClasses());
scheduleClassLayout->addWidget(scheduleClassCombo);
scheduleClassLayout->addStretch();
sLay->addLayout(scheduleClassLayout);

// Создаем таблицу расписания
scheduleTable = new QTableWidget();
scheduleTable->setColumnCount(4);
// Убираем горизонтальные заголовки
scheduleTable->horizontalHeader()->setVisible(false);
scheduleTable->horizontalHeader()->setStretchLastSection(true);
scheduleTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
scheduleTable->verticalHeader()->setVisible(false);
// Настраиваем скролл только вертикальный
scheduleTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
scheduleTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
// Устанавливаем фиксированную ширину колонок для предотвращения горизонтального скролла
scheduleTable->setColumnWidth(0, 120); // Время
scheduleTable->setColumnWidth(1, 250); // Урок
scheduleTable->setColumnWidth(2, 100); // Кабинет
// ДЗ - растягивается

// Функция для обновления таблицы расписания
auto updateScheduleTable = [=](const QString &className) {
scheduleTable->blockSignals(true);
scheduleTable->clear();
scheduleTable->setRowCount(0);

// Загружаем расписание для выбранного класса
// Убеждаемся, что имя класса передается правильно
QString actualClassName = className.trimmed();
auto scheduleLines = DataStore().loadSchedule(actualClassName);

// Если расписание пустое, возможно файл не найден
if (scheduleLines.isEmpty() && !actualClassName.isEmpty()) {
// Пробуем загрузить еще раз с проверкой
scheduleLines = DataStore().loadSchedule(actualClassName);
}

// Загружаем домашние задания
auto homeworkList = DataStore().loadHomework();
QMap<QString, QString> homeworkMap;
for (const QString &line : homeworkList) {
QStringList parts = line.split('|');
if (parts.size() >= 4 && parts[0] == className) {
QString subject = parts[1];
QString homework = parts[2];
homeworkMap[subject] = homework;
}
}

// Создаем структуру расписания по дням недели
QStringList dayNames = {"Понедельник", "Вторник", "Среда", "Четверг", "Пятница"};
QStringList dayNamesUpper = {"ПОНЕДЕЛЬНИК", "ВТОРНИК", "СРЕДА", "ЧЕТВЕРГ", "ПЯТНИЦА"};
QStringList dayColors = {"#000000", "#000000", "#000000", "#000000", "#000000"};
QStringList dayTextColors = {"#000000", "#000000", "#000000", "#000000", "#000000"};

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
dayFont.setPointSize(12);
dayHeader->setFont(dayFont);
dayHeader->setBackground(QColor(dayColors[dayIdx]));
dayHeader->setForeground(QColor(dayTextColors[dayIdx]));
dayHeader->setFlags(dayHeader->flags() & ~Qt::ItemIsEditable);
dayHeader->setTextAlignment(Qt::AlignCenter);
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
scheduleTable->setItem(currentRow, 0, timeItem);

QString lessonText = lesson < lessons.size() ? lessons[lesson] : "";
QString classroom = lesson < classrooms.size() ? classrooms[lesson] : "";

if (lessonText.isEmpty()) {
lessonText = "-";
}

QTableWidgetItem *lessonItem = new QTableWidgetItem(lessonText);
lessonItem->setFlags(lessonItem->flags() & ~Qt::ItemIsEditable);
lessonItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
scheduleTable->setItem(currentRow, 1, lessonItem);

QTableWidgetItem *classroomItem = new QTableWidgetItem(classroom);
classroomItem->setFlags(classroomItem->flags() & ~Qt::ItemIsEditable);
classroomItem->setTextAlignment(Qt::AlignCenter);
scheduleTable->setItem(currentRow, 2, classroomItem);

// Колонка ДЗ (редактируемая)
QString homeworkText = homeworkMap.value(lessonText, "");
QTableWidgetItem *homeworkItem = new QTableWidgetItem(homeworkText);
homeworkItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
homeworkItem->setFlags(homeworkItem->flags() | Qt::ItemIsEditable);
scheduleTable->setItem(currentRow, 3, homeworkItem);

currentRow++;
}
}

// Устанавливаем фиксированные ширины колонок для предотвращения горизонтального скролла
scheduleTable->setColumnWidth(0, 120); // Время
scheduleTable->setColumnWidth(1, 250); // Урок
scheduleTable->setColumnWidth(2, 100); // Кабинет
// ДЗ - растягивается автоматически
scheduleTable->setAlternatingRowColors(true);
scheduleTable->horizontalHeader()->setStretchLastSection(true);
scheduleTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
scheduleTable->setStyleSheet(R"(
QTableWidget { 
background-color: #1E1E1E; 
border: 2px solid #333333;
gridline-color: #2a2a2a;
alternate-background-color: #232323;
} 
QTableWidget::item { 
padding: 8px; 
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
border: 1px solid #d0d0d0;
font-weight: bold;
}
)");

scheduleTable->blockSignals(false);
};

// Инициализируем таблицу с общим расписанием
updateScheduleTable("");

// Убеждаемся, что при выборе класса загружается его расписание
// Функция updateScheduleTable уже использует DataStore().loadSchedule(className),
// которая загружает расписание из файла Schedule_<className>.txt для каждого класса

// Обработчик изменения ДЗ
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

if (!lessonName.isEmpty() && lessonName != "-" && scheduleClassCombo && scheduleClassCombo->count() > 0) {
QString className = scheduleClassCombo->itemData(scheduleClassCombo->currentIndex()).toString();
QDate date = QDate::currentDate();
DataStore().addHomework(className, lessonName, homework, date.toString("yyyy-MM-dd"));
}
}
});

sLay->addWidget(scheduleTable);

QPushButton *saveSch  = new QPushButton("Сохранить");
QPushButton *loadSch  = new QPushButton("Загрузить");
QPushButton *genSch   = new QPushButton("Создать базовое");

connect(saveSch, &QPushButton::clicked, this, &AdminView::onSaveSchedule);
connect(loadSch, &QPushButton::clicked, this, &AdminView::onLoadSchedule);
connect(genSch, &QPushButton::clicked, this, &AdminView::onGenerateSchedule);

// Обновляем расписание при выборе класса
connect(scheduleClassCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
QString className = scheduleClassCombo->itemText(index).trimmed();
// Обновляем таблицу расписания для выбранного класса
updateScheduleTable(className);
});

sLay->addWidget(saveSch);
sLay->addWidget(loadSch);
sLay->addWidget(genSch);

mainTabs->addTab(schTab, "Расписание");

// ---- Grades ----
QWidget *grTab = new QWidget();
QVBoxLayout *gLay = new QVBoxLayout(grTab);

allGradesTable = new QTableWidget();
allGradesTable->setColumnCount(5);
QStringList gradeHeaders = {"Класс", "Ученик", "Предмет", "Дата", "Оценка"};
allGradesTable->setHorizontalHeaderLabels(gradeHeaders);
allGradesTable->horizontalHeader()->setStretchLastSection(true);

// Загружаем все оценки
DataStore ds;
auto allGrades = ds.loadGrades();
int row = 0;
for (int i = 0; i < allGrades.size(); i++) {
QStringList p = allGrades[i].split('|');
if (p.size() == 5) {
QString grade = p[4].trimmed();
// Показываем только строки с непустой оценкой
if (!grade.isEmpty()) {
allGradesTable->insertRow(row);
for (int j = 0; j < 5; j++) {
allGradesTable->setItem(row, j, new QTableWidgetItem(p[j].trimmed()));
}
row++;
}
}
}

// Загружаем отметки за дисциплину
auto disciplineMarks = ds.loadDisciplineMarks();
// Получаем список всех учеников для получения их классов и ФИО
auto students = ds.loadStudents();
QMap<QString, QString> studentToClass; // логин -> класс
QMap<QString, QString> studentToFullName; // логин -> ФИО
for (const User &u : students) {
studentToClass[u.username] = u.className;
studentToFullName[u.username] = u.fullName;
}

for (const QString &line : disciplineMarks) {
QStringList parts = line.split('|');
if (parts.size() >= 4) {
QString studentLogin = parts[0].trimmed();
QString subject = parts[1].trimmed();
QString mark = parts[2].trimmed();
QString date = parts[3].trimmed();

// Получаем класс и ФИО ученика
QString className = studentToClass.value(studentLogin, "");
QString studentFullName = studentToFullName.value(studentLogin, studentLogin);

// Добавляем строку в таблицу
allGradesTable->insertRow(row);
allGradesTable->setItem(row, 0, new QTableWidgetItem(className));
allGradesTable->setItem(row, 1, new QTableWidgetItem(studentFullName));
allGradesTable->setItem(row, 2, new QTableWidgetItem(subject + " (Дисциплина)"));
allGradesTable->setItem(row, 3, new QTableWidgetItem(date));
allGradesTable->setItem(row, 4, new QTableWidgetItem(mark));
row++;
}
}
gLay->addWidget(allGradesTable);

QPushButton *viewG = new QPushButton("Обновить оценки");
QPushButton *clrG  = new QPushButton("Очистить оценки");
QPushButton *exportG = new QPushButton("Экспорт оценок");

connect(viewG, &QPushButton::clicked, this, &AdminView::onViewGrades);
connect(clrG,  &QPushButton::clicked, this, &AdminView::onClearGrades);
connect(exportG, &QPushButton::clicked, this, &AdminView::onExportData);

QHBoxLayout *gradeBtns = new QHBoxLayout();
gradeBtns->addWidget(viewG);
gradeBtns->addWidget(clrG);
gradeBtns->addWidget(exportG);
gLay->addLayout(gradeBtns);

mainTabs->addTab(grTab, "Оценки");

// ---- Classes & Students ----
QWidget *classTab = new QWidget();
QVBoxLayout *cLay = new QVBoxLayout(classTab);

QHBoxLayout *classH = new QHBoxLayout();
classesList = new QListWidget();
// Автоматически загружаем классы из файла
classesList->addItems(DataStore().getClasses());
QListWidget *studentsInClassList = new QListWidget();
// Автоматически загружаем учеников для первого класса, если есть
if (classesList->count() > 0) {
QString firstClass = classesList->item(0)->text();
studentsInClassList->addItems(DataStore().getStudentsForClass(firstClass));
}

classH->addWidget(new QLabel("Классы:"));
classH->addWidget(classesList);
classH->addWidget(new QLabel("Ученики в классе:"));
classH->addWidget(studentsInClassList);
cLay->addLayout(classH);

QPushButton *addClassBtn = new QPushButton("Добавить класс");
QPushButton *remClassBtn = new QPushButton("Удалить класс");
QPushButton *addStudentBtn = new QPushButton("Добавить ученика в класс");
QPushButton *remStudentBtn = new QPushButton("Удалить ученика из класса");
QPushButton *refreshClassBtn = new QPushButton("Обновить");

// Удален двойной клик на класс - функционал редактирования класса удален

connect(classesList, &QListWidget::itemClicked, [=](QListWidgetItem *item) {
QString cls = item->text();
studentsInClassList->clear();
studentsInClassList->addItems(DataStore().getStudentsForClass(cls));
});

// Сохраняем указатель на список учеников для доступа из метода
studentsListForDiscipline = studentsInClassList;
connect(studentsInClassList, &QListWidget::itemDoubleClicked, this, &AdminView::onStudentDoubleClicked);

connect(refreshClassBtn, &QPushButton::clicked, this, &AdminView::onRefreshClasses);

connect(addClassBtn, &QPushButton::clicked, [=]() {
bool ok;
QString className = QInputDialog::getText(this, "Новый класс", "Название класса:", 
QLineEdit::Normal, "", &ok);
if (ok && !className.isEmpty()) {
QFile f(DataStore().classesFile());
if (f.open(QIODevice::Append | QIODevice::Text)) {
QTextStream out(&f);
out << className << "\n";
}
classesList->addItem(className);
}
});

connect(remClassBtn, &QPushButton::clicked, [=]() {
if (!classesList->currentItem()) return;
QString cls = classesList->currentItem()->text();
auto allClasses = DataStore().getClasses();
allClasses.removeAll(cls);
QFile f(DataStore().classesFile());
if (f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
QTextStream out(&f);
for (auto &c : allClasses) out << c << "\n";
}
delete classesList->currentItem();
studentsInClassList->clear();
});

connect(addStudentBtn, &QPushButton::clicked, [=]() {
if (!classesList->currentItem()) {
QMessageBox::warning(this, "Ошибка", "Выберите класс.");
return;
}

QString selectedClass = classesList->currentItem()->text();

AddStudentDialog dlg(this);
// Устанавливаем выбранный класс в диалоге
if (dlg.classCombo) {
int index = dlg.classCombo->findText(selectedClass);
if (index >= 0) {
dlg.classCombo->setCurrentIndex(index);
}
}

if (dlg.exec() == QDialog::Accepted) {
QString fullName = dlg.getFullName();
QString login = dlg.getLogin();
QString password = dlg.getPassword();
QString className = dlg.getClassName();

if (fullName.isEmpty() || login.isEmpty() || password.isEmpty() || className.isEmpty()) {
QMessageBox::warning(this, "Ошибка", "Заполните все поля.");
return;
}

// Проверяем, что класс существует в списке классов
bool classExists = false;
for (int i = 0; i < classesList->count(); i++) {
if (classesList->item(i)->text() == className) {
classExists = true;
break;
}
}

// Если класса нет в списке, добавляем его
if (!classExists) {
QFile f(DataStore().classesFile());
if (f.open(QIODevice::Append | QIODevice::Text)) {
QTextStream out(&f);
out.setEncoding(QStringConverter::Utf8);
out << className << "\n";
f.close();
classesList->addItem(className);
}
}

if (DataStore().addStudentToClass(className, fullName, login, password)) {
// Обновляем список учеников для выбранного класса
QString currentSelectedClass = classesList->currentItem() ? classesList->currentItem()->text() : className;
studentsInClassList->clear();
studentsInClassList->addItems(DataStore().getStudentsForClass(currentSelectedClass));
// Если выбранный класс отличается от добавленного, переключаемся на добавленный
if (currentSelectedClass != className && classesList->currentItem()) {
for (int i = 0; i < classesList->count(); i++) {
if (classesList->item(i)->text() == className) {
classesList->setCurrentRow(i);
studentsInClassList->clear();
studentsInClassList->addItems(DataStore().getStudentsForClass(className));
break;
}
}
}
QMessageBox::information(this, "Успех", "Ученик добавлен в класс " + className + ".");
} else {
QMessageBox::warning(this, "Ошибка", "Не удалось добавить ученика. Возможно, ученик с таким логином уже существует.");
}
}
});

connect(remStudentBtn, &QPushButton::clicked, [=]() {
if (!classesList->currentItem() || !studentsInClassList->currentItem()) {
QMessageBox::warning(this, "Ошибка", "Выберите класс и ученика.");
return;
}
QString cls = classesList->currentItem()->text();
QString student = studentsInClassList->currentItem()->text();
if (DataStore().removeStudentFromClass(cls, student)) {
// Обновляем список учеников после удаления
studentsInClassList->clear();
studentsInClassList->addItems(DataStore().getStudentsForClass(cls));
QMessageBox::information(this, "Успех", "Ученик удален из класса.");
} else {
QMessageBox::warning(this, "Ошибка", "Не удалось удалить ученика.");
}
});

connect(refreshClassBtn, &QPushButton::clicked, [=]() {
classesList->clear();
classesList->addItems(DataStore().getClasses());
if (classesList->currentItem()) {
QString cls = classesList->currentItem()->text();
studentsInClassList->clear();
studentsInClassList->addItems(DataStore().getStudentsForClass(cls));
} else if (classesList->count() > 0) {
// Если нет выбранного, но есть классы, выбираем первый
classesList->setCurrentRow(0);
QString cls = classesList->item(0)->text();
studentsInClassList->clear();
studentsInClassList->addItems(DataStore().getStudentsForClass(cls));
}
});

QHBoxLayout *classBtns1 = new QHBoxLayout();
classBtns1->addWidget(addClassBtn);
classBtns1->addWidget(remClassBtn);
cLay->addLayout(classBtns1);

QHBoxLayout *classBtns2 = new QHBoxLayout();
classBtns2->addWidget(addStudentBtn);
classBtns2->addWidget(remStudentBtn);
classBtns2->addWidget(refreshClassBtn);
cLay->addLayout(classBtns2);

mainTabs->addTab(classTab, "Классы и ученики");

main->addWidget(mainTabs);

// Кнопка "Назад" внизу
QHBoxLayout *btnLayout = new QHBoxLayout();
backBtn = new QPushButton("Назад");
backBtn->setMinimumHeight(40);
backBtn->setMinimumWidth(120);
backBtn->setStyleSheet("QPushButton { background-color: #444444; color: white; font-size: 14px; font-weight: bold; border-radius: 0px; padding: 10px 16px; } QPushButton:hover { background-color: #555555; } QPushButton:pressed { background-color: #333333; }");
connect(backBtn, &QPushButton::clicked, this, &AdminView::onBack);
btnLayout->addWidget(backBtn);
btnLayout->addStretch();
main->addLayout(btnLayout);
}

// ------ AdminView logic ------
void AdminView::onLogout() { 
close(); 
}

void AdminView::onBack() {
if (!navigationStack.isEmpty()) {
QWidget *prevWidget = navigationStack.pop();
prevWidget->showMaximized();
close();
} else {
close();
}
}

void AdminView::onAddTeacher() {
AddTeacherDialog dlg(this);
if (dlg.exec() == QDialog::Accepted) {
QString fullName = dlg.getFullName();
QString login = dlg.getLogin();
QString password = dlg.getPassword();
QString subject = dlg.getSubject();

if (fullName.isEmpty() || login.isEmpty() || password.isEmpty() || subject.isEmpty()) {
QMessageBox::warning(this, "Ошибка", "Заполните все поля.");
return;
}

if (DataStore().addTeacher(fullName, login, password, subject)) {
teachersList->clear();
teachersList->addItems(DataStore().getTeachers());
QMessageBox::information(this, "Успех", "Учитель добавлен.");
} else {
QMessageBox::warning(this, "Ошибка", "Не удалось добавить учителя.");
}
}
}

void AdminView::onRemoveTeacher() {
if (!teachersList->currentItem()) return;

QString itemText = teachersList->currentItem()->text();
// Извлекаем логин из строки вида "логин (Дисциплина)"
// Формат: "логин (Дисциплина)" - логин до первой пробела или скобки
QString login = itemText;
if (itemText.contains(' ')) {
login = itemText.left(itemText.indexOf(' '));
}
if (login.contains('(')) {
login = login.left(login.indexOf('('));
}
login = login.trimmed();

if (login.isEmpty()) {
QMessageBox::warning(this, "Ошибка", "Не удалось определить логин учителя.");
return;
}

// Подтверждение удаления
int ret = QMessageBox::question(this, "Подтверждение", 
"Вы уверены, что хотите удалить учителя с логином \"" + login + "\"?",
QMessageBox::Yes | QMessageBox::No);
if (ret != QMessageBox::Yes) return;

if (DataStore().removeTeacher(login)) {
teachersList->clear();
teachersList->addItems(DataStore().getTeachers());
QMessageBox::information(this, "Успех", "Учитель удален.");
} else {
QMessageBox::warning(this, "Ошибка", "Не удалось удалить учителя.");
}
}

void AdminView::onAddAdmin() {
bool ok;
QString login = QInputDialog::getText(this, "Админ", "Логин:", QLineEdit::Normal, "", &ok);
if (!ok || login.isEmpty()) return;

QString pass = QInputDialog::getText(this, "Админ", "Пароль:", QLineEdit::Normal, "", &ok);
if (!ok || pass.isEmpty()) return;

DataStore().addAdmin(login, pass);
adminsList->clear();
adminsList->addItems(DataStore().getAdmins());
}

void AdminView::onRemoveAdmin() {
if (!adminsList->currentItem()) return;

QString login = adminsList->currentItem()->text();
if (login == "admin") {
QMessageBox::warning(this, "Ошибка", "Нельзя удалить главного администратора.");
return;
}
DataStore().removeAdmin(login);
adminsList->clear();
adminsList->addItems(DataStore().getAdmins());
}

void AdminView::onRefreshAdmins() {
adminsList->clear();
adminsList->addItems(DataStore().getAdmins());
}

void AdminView::onLoadSchedule() {
QString className = "";
if (scheduleClassCombo && scheduleClassCombo->currentIndex() >= 0) {
className = scheduleClassCombo->itemData(scheduleClassCombo->currentIndex()).toString();
}
// Обновляем таблицу расписания
if (scheduleTable) {
scheduleTable->blockSignals(true);
scheduleTable->clear();
scheduleTable->setRowCount(0);

auto scheduleLines = DataStore().loadSchedule(className);
auto homeworkList = DataStore().loadHomework();
QMap<QString, QString> homeworkMap;
for (const QString &line : homeworkList) {
QStringList parts = line.split('|');
if (parts.size() >= 4 && parts[0] == className) {
QString subject = parts[1];
QString homework = parts[2];
homeworkMap[subject] = homework;
}
}

QStringList dayNames = {"Понедельник", "Вторник", "Среда", "Четверг", "Пятница"};
QStringList dayNamesUpper = {"ПОНЕДЕЛЬНИК", "ВТОРНИК", "СРЕДА", "ЧЕТВЕРГ", "ПЯТНИЦА"};
QStringList dayColors = {"#3D7BFF", "#1e1e1e", "#5A8FFF", "#1e1e1e", "#5A8FFF"};
QStringList dayTextColors = {"#ffffff", "#e0e0e0", "#ffffff", "#e0e0e0", "#ffffff"};

int totalRows = 0;
for (const QString &day : dayNamesUpper) {
totalRows += 7;
}
scheduleTable->setRowCount(totalRows);

int currentRow = 0;
for (int dayIdx = 0; dayIdx < dayNames.size(); dayIdx++) {
QString dayName = dayNames[dayIdx];
QString dayUpper = dayNamesUpper[dayIdx];

QTableWidgetItem *dayHeader = new QTableWidgetItem(dayUpper);
QFont dayFont = dayHeader->font();
dayFont.setBold(true);
dayFont.setPointSize(12);
dayHeader->setFont(dayFont);
dayHeader->setBackground(QColor(dayColors[dayIdx]));
dayHeader->setForeground(QColor(dayTextColors[dayIdx]));
dayHeader->setFlags(dayHeader->flags() & ~Qt::ItemIsEditable);
dayHeader->setTextAlignment(Qt::AlignCenter);
scheduleTable->setItem(currentRow, 0, dayHeader);
scheduleTable->setSpan(currentRow, 0, 1, 4);
currentRow++;

QStringList times = {"14:00-14:45", "15:00-15:45", "16:00-16:45", 
"16:55-17:40", "17:50-18:35", "18:45-19:30"};

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
scheduleTable->setItem(currentRow, 0, timeItem);

QString lessonText = lesson < lessons.size() ? lessons[lesson] : "";
QString classroom = lesson < classrooms.size() ? classrooms[lesson] : "";

if (lessonText.isEmpty()) {
lessonText = "-";
}

QTableWidgetItem *lessonItem = new QTableWidgetItem(lessonText);
lessonItem->setFlags(lessonItem->flags() & ~Qt::ItemIsEditable);
lessonItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
scheduleTable->setItem(currentRow, 1, lessonItem);

QTableWidgetItem *classroomItem = new QTableWidgetItem(classroom);
classroomItem->setFlags(classroomItem->flags() & ~Qt::ItemIsEditable);
classroomItem->setTextAlignment(Qt::AlignCenter);
scheduleTable->setItem(currentRow, 2, classroomItem);

QString homeworkText = homeworkMap.value(lessonText, "");
QTableWidgetItem *homeworkItem = new QTableWidgetItem(homeworkText);
homeworkItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
homeworkItem->setFlags(homeworkItem->flags() | Qt::ItemIsEditable);
scheduleTable->setItem(currentRow, 3, homeworkItem);

currentRow++;
}
}

scheduleTable->resizeColumnsToContents();
scheduleTable->horizontalHeader()->setStretchLastSection(true);
scheduleTable->blockSignals(false);
}
}

void AdminView::onSaveSchedule() {
// Сохранение расписания из таблицы в файл
QString className = "";
if (scheduleClassCombo && scheduleClassCombo->currentIndex() >= 0) {
className = scheduleClassCombo->itemData(scheduleClassCombo->currentIndex()).toString();
}

// Собираем расписание из таблицы обратно в текстовый формат
List<QString> scheduleLines;
QStringList dayNames = {"Понедельник", "Вторник", "Среда", "Четверг", "Пятница"};

if (scheduleTable) {
int row = 0;
for (int dayIdx = 0; dayIdx < dayNames.size(); dayIdx++) {
QString dayName = dayNames[dayIdx];
QString scheduleLine = dayName + ": ";
QStringList lessons;

row++; // Пропускаем заголовок дня

for (int lesson = 0; lesson < 6; lesson++) {
if (row >= scheduleTable->rowCount()) break;

QTableWidgetItem *lessonItem = scheduleTable->item(row, 1);
QTableWidgetItem *classroomItem = scheduleTable->item(row, 2);

if (lessonItem && lessonItem->text() != "-") {
QString lessonText = lessonItem->text();
QString classroom = classroomItem ? classroomItem->text() : "";
if (!classroom.isEmpty()) {
lessons.append(lessonText + " (" + classroom + ")");
} else {
lessons.append(lessonText);
}
}
row++;
}

if (!lessons.isEmpty()) {
scheduleLine += lessons.join(", ");
} else {
scheduleLine += "Выходной";
}
scheduleLines.append(scheduleLine);
}
}

DataStore().saveSchedule(scheduleLines, className);
QMessageBox::information(this, "OK", "Сохранено для класса: " + (className.isEmpty() ? "Общее расписание" : className));
}

void AdminView::onGenerateSchedule() {
DataStore().generateDefaultSchedule();
// Обновляем таблицу расписания
onLoadSchedule();
}

void AdminView::onViewGrades() {
// Обновляем таблицу оценок
allGradesTable->clearContents();
allGradesTable->setRowCount(0);

DataStore ds;

// Загружаем обычные оценки
auto allGrades = ds.loadGrades();
int row = 0;
for (int i = 0; i < allGrades.size(); i++) {
QStringList p = allGrades[i].split('|');
if (p.size() == 5) {
QString grade = p[4].trimmed();
// Показываем только строки с непустой оценкой
if (!grade.isEmpty()) {
allGradesTable->insertRow(row);
for (int j = 0; j < 5; j++) {
allGradesTable->setItem(row, j, new QTableWidgetItem(p[j].trimmed()));
}
row++;
}
}
}

// Загружаем отметки за дисциплину
auto disciplineMarks = ds.loadDisciplineMarks();
// Получаем список всех учеников для получения их классов и ФИО
auto students = ds.loadStudents();
QMap<QString, QString> studentToClass; // логин -> класс
QMap<QString, QString> studentToFullName; // логин -> ФИО
for (const User &u : students) {
studentToClass[u.username] = u.className;
studentToFullName[u.username] = u.fullName;
}

for (const QString &line : disciplineMarks) {
QStringList parts = line.split('|');
if (parts.size() >= 4) {
QString studentLogin = parts[0].trimmed();
QString subject = parts[1].trimmed();
QString mark = parts[2].trimmed();
QString date = parts[3].trimmed();

// Получаем класс и ФИО ученика
QString className = studentToClass.value(studentLogin, "");
QString studentFullName = studentToFullName.value(studentLogin, studentLogin);

// Добавляем строку в таблицу
allGradesTable->insertRow(row);
allGradesTable->setItem(row, 0, new QTableWidgetItem(className));
allGradesTable->setItem(row, 1, new QTableWidgetItem(studentFullName));
allGradesTable->setItem(row, 2, new QTableWidgetItem(subject + " (Дисциплина)"));
allGradesTable->setItem(row, 3, new QTableWidgetItem(date));
allGradesTable->setItem(row, 4, new QTableWidgetItem(mark));
row++;
}
}
QMessageBox::information(this, "OK", "Оценки обновлены.");
}

void AdminView::onSearchTeachers() {
QString searchText = teacherSearchEdit->text().toLower();
teachersList->clear();

auto allTeachers = DataStore().getTeachers();
for (const QString &teacher : allTeachers) {
if (searchText.isEmpty() || teacher.toLower().contains(searchText)) {
teachersList->addItem(teacher);
}
}
}

void AdminView::onSearchAdmins() {
QString searchText = adminSearchEdit->text().toLower();
adminsList->clear();

auto allAdmins = DataStore().getAdmins();
for (const QString &admin : allAdmins) {
if (searchText.isEmpty() || admin.toLower().contains(searchText)) {
adminsList->addItem(admin);
}
}
}

void AdminView::onExportData() {
QString fileName = QFileDialog::getSaveFileName(this, "Экспорт данных", 
"journal_export.txt", 
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
out << "=== ЭКСПОРТ ДАННЫХ ЭЛЕКТРОННОГО ЖУРНАЛА ===\n";
out << "Дата экспорта: " << QDate::currentDate().toString("dd.MM.yyyy") << "\n";
out << "Администратор: " << username << "\n\n";

DataStore ds;

// Экспорт оценок
out << "=== ОЦЕНКИ ===\n";
out << "Класс\tУченик\tПредмет\tДата\tОценка\n";
auto allGrades = ds.loadGrades();
for (const QString &line : allGrades) {
QStringList parts = line.split('|');
if (parts.size() == 5) {
out << parts[0] << "\t" << parts[1] << "\t" << parts[2] 
<< "\t" << parts[3] << "\t" << parts[4] << "\n";
}
}
out << "\n";

// Экспорт классов
out << "=== КЛАССЫ ===\n";
auto classes = ds.getClasses();
for (const QString &cls : classes) {
out << cls << "\n";
}
out << "\n";

// Экспорт учителей
out << "=== УЧИТЕЛЯ ===\n";
auto teachers = ds.getTeachers();
for (const QString &teacher : teachers) {
out << teacher << "\n";
}
out << "\n";

// Экспорт администраторов
out << "=== АДМИНИСТРАТОРЫ ===\n";
auto admins = ds.getAdmins();
for (const QString &admin : admins) {
out << admin << "\n";
}
out << "\n";

// Экспорт расписания
out << "=== РАСПИСАНИЕ ===\n";
auto schedule = ds.loadSchedule("");
for (const QString &line : schedule) {
out << line << "\n";
}

QMessageBox::information(this, "Успех", "Данные экспортированы в файл.");
}

void AdminView::onStudentDoubleClicked() {
if (!studentsListForDiscipline || !studentsListForDiscipline->currentItem()) return;

QString studentName = studentsListForDiscipline->currentItem()->text();
// Извлекаем только ФИО без логина
QString displayName = studentName;
if (studentName.contains('(')) {
displayName = studentName.left(studentName.indexOf('(')).trimmed();
}

DisciplineMarkDialog dlg(displayName, this);
if (dlg.exec() == QDialog::Accepted) {
QString subject = dlg.getSubject();
QString mark = dlg.getMark();
QDate date = dlg.getDate();

if (subject.isEmpty()) {
QMessageBox::warning(this, "Ошибка", "Выберите предмет.");
return;
}

// Извлекаем логин ученика
QString studentLogin = studentName;
if (studentName.contains('(') && studentName.contains(')')) {
int start = studentName.indexOf('(') + 1;
int end = studentName.indexOf(')');
studentLogin = studentName.mid(start, end - start);
}

if (DataStore().addDisciplineMark(studentLogin, subject, mark, date)) {
QMessageBox::information(this, "Успех", "Отметка за дисциплину поставлена.");
} else {
QMessageBox::warning(this, "Ошибка", "Не удалось поставить отметку.");
}
}
}

void AdminView::onClearGrades() {
DataStore().overwriteGrades({});
QMessageBox::information(this, "OK", "Оценки очищены.");
}

void AdminView::onClassDoubleClicked(QListWidgetItem *item) {
// Функционал редактирования класса удален
// Двойной клик больше не открывает диалог редактирования
}

void AdminView::onRefreshClasses() {
// Обновление списка классов
if (classesList) {
classesList->clear();
classesList->addItems(DataStore().getClasses());
}
}

void AdminView::onRefreshStudents() {
// Обновление списка учеников для выбранного класса
if (classesList && classesList->currentItem() && studentsListForDiscipline) {
QString className = classesList->currentItem()->text();
studentsListForDiscipline->clear();
studentsListForDiscipline->addItems(DataStore().getStudentsForClass(className));
}
}

void AdminView::updateClassesList() {
// Not used in current implementation, but declared
}

void AdminView::updateStudentsList() {
// Not used in current implementation, but declared
}

