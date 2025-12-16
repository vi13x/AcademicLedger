#include "datastore.h"
#include "exceptionhandler.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <vector>
#include <stdexcept>


DataStore::DataStore() {
dataDir = QCoreApplication::applicationDirPath() + "/data";
QDir d(dataDir);
if (!d.exists()) d.mkpath(".");
}

// ---------- FILE PATHS ----------
QString DataStore::studentsFile() { return dataDir + "/Students.txt"; }
QString DataStore::teachersFile() { return dataDir + "/Teachers.txt"; }
QString DataStore::adminsFile()   { return dataDir + "/Admins.txt"; }
QString DataStore::scheduleFile() { return dataDir + "/Schedule.txt"; }
QString DataStore::gradesFile()   { return dataDir + "/Grades.txt"; }
QString DataStore::classesFile()  { return dataDir + "/Classes.txt"; }

// ---------- LOAD STUDENTS ----------
List<User> DataStore::loadStudents() {
List<User> out;

try {
QFile f(studentsFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
throw FileException("Не удалось открыть файл студентов для чтения");
}

QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);

while (!in.atEnd()) {
QString l = in.readLine().trimmed();
if (l.isEmpty()) continue;
QStringList p = l.split('\t');
// Формат: Фам\tИмя\tОтч\tЛогин\tПароль\tКласс
if (p.size() < 5) continue;

// Проверка границ массива
try {
ExceptionHandler::checkBounds(p, 3);
ExceptionHandler::checkBounds(p, 4);

User u;
u.username = p[3];
u.password = p[4];
u.role = "student";
out.push_back(u);
} catch (const OutOfRangeException& e) {
// Пропускаем некорректную строку
continue;
}
}
} catch (const FileException& e) {
QString errorMsg;
ExceptionHandler::safeExecute([&e]() { throw e; }, errorMsg);
return out;
} catch (const std::exception& e) {
ExceptionHandler::handleException(e);
return out;
} catch (...) {
ExceptionHandler::handleUnknownException();
return out;
}

return out;
}

bool DataStore::addStudent(const User &u, const QString &fullName, const QString &className) {
QFile f(studentsFile());
if (!f.open(QIODevice::Append | QIODevice::Text)) return false;
QTextStream out(&f);
// Формат: Фам\tИмя\tОтч\tЛогин\tПароль\tКласс
if (!fullName.isEmpty()) {
QStringList nameParts = fullName.split(' ');
if (nameParts.size() >= 3) {
out << nameParts[0] << "\t" << nameParts[1] << "\t" << nameParts[2] 
<< "\t" << u.username << "\t" << u.password << "\t" << className << "\n";
} else {
out << fullName << "\t\t\t" << u.username << "\t" << u.password << "\t" << className << "\n";
}
} else {
out << "Фам\tИмя\tОтч\t" << u.username << "\t" << u.password << "\t" << className << "\n";
}
return true;
}

// ---------- GET TEACHERS ----------
List<QString> DataStore::getTeachers() {
List<QString> out;
QFile f(teachersFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return out;
QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);

while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (line.isEmpty()) continue;
QStringList p = line.split('\t');
// Формат: Фам\tИмя\tОтч\tЛогин\tПароль\tДисциплина
if (p.size() >= 4) {
QString teacher = p[3].trimmed() + " (" + p.value(5, "Без дисциплины").trimmed() + ")";
out.push_back(teacher);
}
}
return out;
}

QString DataStore::getTeacherFullName(const QString &username) {
QFile f(teachersFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return username;
QTextStream in(&f);

while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (line.isEmpty()) continue;
QStringList p = line.split('\t');
// Формат: Фам\tИмя\tОтч\tЛогин\tПароль\tДисциплина
if (p.size() >= 4 && p[3] == username) {
return p[0] + " " + p[1] + " " + p[2];
}
}
return username;
}

// ---------- GET SUBJECTS ----------
List<QString> DataStore::getSubjects() {
List<QString> subjects;
QFile f(teachersFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
subjects.append("Математика");
subjects.append("Русский язык");
subjects.append("Физика");
subjects.append("История");
subjects.append("Литература");
subjects.append("Химия");
subjects.append("География");
subjects.append("Биология");
subjects.append("Информатика");
subjects.append("Английский язык");
subjects.append("Обществознание");
subjects.append("Физкультура");
subjects.append("Искусство");
return subjects;
}
QTextStream in(&f);
List<QString> uniqueSubjects;

while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (line.isEmpty()) continue;
QStringList p = line.split('\t');
if (p.size() >= 6 && !p[5].isEmpty()) {
if (!uniqueSubjects.contains(p[5])) {
uniqueSubjects.append(p[5]);
}
}
}

if (uniqueSubjects.isEmpty()) {
subjects.append("Математика");
subjects.append("Русский язык");
subjects.append("Физика");
subjects.append("История");
} else {
subjects = uniqueSubjects;
}
return subjects;
}

// ---------- GET ADMINS ----------
List<QString> DataStore::getAdmins() {
List<QString> out;
QFile f(adminsFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
// Добавляем дефолтного админа
out.append("admin");
return out;
}
QTextStream in(&f);

while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (line.isEmpty()) continue;
QStringList p = line.split('\t');
// Формат: Фам\tИмя\tОтч\tЛогин\tПароль
if (p.size() >= 4) {
out.append(p[3]);
}
}
// Добавляем дефолтного админа если его нет
if (!out.contains("admin")) {
out.push_front("admin");
}
return out;
}

// ---------- HOMEWORK ----------
QString DataStore::homeworkFile() { return dataDir + "/Homework.txt"; }

bool DataStore::addHomework(const QString &className, const QString &subject,
const QString &homework, const QString &dueDate) {
QFile f(homeworkFile());
if (!f.open(QIODevice::Append | QIODevice::Text)) return false;
QTextStream out(&f);
out << className << "|" << subject << "|" << homework << "|" << dueDate << "\n";
return true;
}

List<QString> DataStore::loadHomework() {
List<QString> out;
QFile f(homeworkFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return out;
QTextStream in(&f);
while (!in.atEnd()) out.push_back(in.readLine());
return out;
}

// ---------- PERIODS ----------
List<QString> DataStore::getPeriods() {
List<QString> periods;
periods.append("1-я четверть");
periods.append("2-я четверть");
periods.append("3-я четверть");
periods.append("4-я четверть");
return periods;
}

// ---------- LESSON TOPICS (темы уроков) ----------
QString DataStore::lessonTopicsFile() { 
return dataDir + "/LessonTopics.txt"; 
}

bool DataStore::addLessonTopic(const QString &className, const QString &subject, const QString &date, const QString &topic, const QString &teacherLogin) {
QFile f(lessonTopicsFile());
if (!f.open(QIODevice::Append | QIODevice::Text)) return false;
QTextStream out(&f);
out.setEncoding(QStringConverter::Utf8);
out << className << "|" << subject << "|" << date << "|" << topic << "|" << teacherLogin << "\n";
return true;
}

bool DataStore::updateLessonTopic(const QString &className, const QString &subject, const QString &date, const QString &topic, const QString &teacherLogin) {
// Сначала удаляем старую запись, потом добавляем новую
deleteLessonTopic(className, subject, date);
if (!topic.isEmpty()) {
return addLessonTopic(className, subject, date, topic, teacherLogin);
}
return true;
}

bool DataStore::deleteLessonTopic(const QString &className, const QString &subject, const QString &date) {
QFile f(lessonTopicsFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

List<QString> lines;
QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);
while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (line.isEmpty()) continue;
QStringList parts = line.split('|');
if (parts.size() >= 3) {
if (parts[0] == className && parts[1] == subject && parts[2] == date) {
continue; // Пропускаем удаляемую запись
}
}
lines.append(line);
}
f.close();

// Перезаписываем файл
if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
QTextStream out(&f);
out.setEncoding(QStringConverter::Utf8);
for (int i = 0; i < lines.size(); i++) {
out << lines[i] << "\n";
}
return true;
}

QString DataStore::getLessonTopic(const QString &className, const QString &subject, const QString &date) {
QFile f(lessonTopicsFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return "";

QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);
while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (line.isEmpty()) continue;
QStringList parts = line.split('|');
if (parts.size() >= 4) {
if (parts[0] == className && parts[1] == subject && parts[2] == date) {
return parts[3]; // Возвращаем тему
}
}
}
return "";
}

// ---------- DISCIPLINE MARKS ----------
QString DataStore::disciplineMarksFile() { return dataDir + "/DisciplineMarks.txt"; }

bool DataStore::addDisciplineMark(const QString &student, const QString &subject, const QString &mark, const QDate &date) {
QFile f(disciplineMarksFile());
if (!f.open(QIODevice::Append | QIODevice::Text)) return false;
QTextStream out(&f);
out.setEncoding(QStringConverter::Utf8);
out << student << "|" << subject << "|" << mark << "|" << date.toString("yyyy-MM-dd") << "\n";
f.close();
return true;
}

List<QString> DataStore::loadDisciplineMarks() {
List<QString> out;
QFile f(disciplineMarksFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return out;
QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);
while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (!line.isEmpty()) out.push_back(line);
}
return out;
}

// ---------- CHECK CREDS ----------
bool DataStore::checkStudentCredentials(const QString &u, const QString &p) {
// Статичный логин для ученика
if (u == "test" && p == "test") return true;

auto list = loadStudents();
for (auto &x : list)
if (x.username == u && x.password == p) return true;
return false;
}

static bool checkFile(const QString &file, const QString &u, const QString &p) {
QFile f(file);
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
QTextStream in(&f);

while (!in.atEnd()) {
QString line = in.readLine().trimmed();
QStringList parts = line.split('\t');
if (parts.size() >= 5 && parts[3] == u && parts[4] == p)
return true;
}
return false;
}

bool DataStore::checkTeacherCredentials(const QString &u, const QString &p) {
// Статичный логин для учителя
if (u == "test2" && p == "test2") return true;
return checkFile(teachersFile(), u, p);
}

bool DataStore::checkAdminCredentials(const QString &u, const QString &p) {
// Статичный логин для админа
if (u == "test3" && p == "test3") return true;
if (u == "admin" && p == "admin") return true;
return checkFile(adminsFile(), u, p);
}

// ---------- ADD / REMOVE ----------
bool DataStore::addTeacher(const QString &fullName, const QString &login, const QString &password, const QString &subject) {
QFile f(teachersFile());
if (!f.open(QIODevice::Append | QIODevice::Text)) return false;
QTextStream out(&f);
out.setEncoding(QStringConverter::Utf8);
// Формат: Фам\tИмя\tОтч\tЛогин\tПароль\tДисциплина
QStringList nameParts = fullName.split(' ');
QString discipline = subject.isEmpty() ? "Дисциплина" : subject;
if (nameParts.size() >= 3) {
out << nameParts[0] << "\t" << nameParts[1] << "\t" << nameParts[2] 
<< "\t" << login << "\t" << password << "\t" << discipline << "\n";
} else if (nameParts.size() == 2) {
out << nameParts[0] << "\t" << nameParts[1] << "\t" 
<< "\t" << login << "\t" << password << "\t" << discipline << "\n";
} else {
out << fullName << "\t\t\t" << login << "\t" << password << "\t" << discipline << "\n";
}
return true;
}

bool DataStore::removeTeacher(const QString &login) {
QFile f(teachersFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

List<QString> lines;
QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);
while (!in.atEnd()) {
QString l = in.readLine().trimmed();
if (l.isEmpty()) continue; // Пропускаем пустые строки
QStringList p = l.split('\t');
// Формат: Фам\tИмя\tОтч\tЛогин\tПароль\tДисциплина
// Логин находится в позиции 3 (индекс 3)
if (p.size() >= 4 && p[3].trimmed() == login.trimmed()) {
// Пропускаем эту строку (удаляем)
continue;
}
lines.append(l);
}
f.close();

if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
return false;

QTextStream out(&f);
out.setEncoding(QStringConverter::Utf8);
for (int i = 0; i < lines.size(); i++) {
if (!lines[i].isEmpty()) {
out << lines[i] << "\n";
}
}
return true;
}

bool DataStore::addAdmin(const QString &u, const QString &p) {
QFile f(adminsFile());
if (!f.open(QIODevice::Append | QIODevice::Text)) return false;
QTextStream out(&f);
out << "Фам\tИмя\tОтч\t" << u << "\t" << p << "\n";
return true;
}

bool DataStore::removeAdmin(const QString &login) {
QFile f(adminsFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

List<QString> lines;
QTextStream in(&f);
while (!in.atEnd()) {
QString l = in.readLine();
QStringList p = l.split('\t');
if (p.size() < 4 || p[3] != login)
lines.push_back(l);
}
f.close();

if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
return false;

QTextStream out(&f);
for (int i = 0; i < lines.size(); i++) {
out << lines[i] << "\n";
}
return true;
}

// ---------- SCHEDULE ----------
QString DataStore::scheduleFileForClass(const QString &className) {
if (className.isEmpty()) {
return scheduleFile();
}
return dataDir + "/Schedule_" + className + ".txt";
}

List<QString> DataStore::loadSchedule(const QString &className) {
List<QString> out;
QString filePath = scheduleFileForClass(className);

// Проверяем существование файла
if (!QFile::exists(filePath) && !className.isEmpty()) {
// Если файл не найден, пробуем найти его с разными вариантами регистра
QString lowerPath = dataDir + "/Schedule_" + className.toLower() + ".txt";
QString upperPath = dataDir + "/Schedule_" + className.toUpper() + ".txt";

if (QFile::exists(lowerPath)) {
filePath = lowerPath;
} else if (QFile::exists(upperPath)) {
filePath = upperPath;
} else {
// Если нет расписания для класса, используем общее
QFile generalFile(scheduleFile());
if (generalFile.exists()) {
// Копируем общее расписание для класса
if (generalFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
QTextStream in(&generalFile);
in.setEncoding(QStringConverter::Utf8);
List<QString> lines;
while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (!line.isEmpty()) lines.append(line);
}
generalFile.close();
saveSchedule(lines, className);
// Пробуем открыть снова
filePath = scheduleFileForClass(className);
}
} else {
generateDefaultSchedule();
filePath = scheduleFileForClass(className);
}
}
} else if (!QFile::exists(filePath) && className.isEmpty()) {
generateDefaultSchedule();
filePath = scheduleFileForClass(className);
}

// Открываем файл для чтения
QFile f(filePath);
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
// Если не удалось открыть, возвращаем пустой список
return out;
}

QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);
while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (!line.isEmpty()) out.append(line);
}
f.close();
return out;
}

bool DataStore::saveSchedule(const List<QString> &lines, const QString &className) {
QString filePath = scheduleFileForClass(className);
QFile f(filePath);
if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
return false;
QTextStream out(&f);
out.setEncoding(QStringConverter::Utf8);
for (int i = 0; i < lines.size(); i++) {
if (!lines[i].isEmpty()) out << lines[i] << "\n";
}
f.close();
return true;
}

void DataStore::generateDefaultSchedule() {
// Генерируем разные расписания для разных классов
auto classes = getClasses();

// Базовые расписания для разных параллелей
Map<QString, List<QString>> schedules;

// Расписание для 1-4 классов
List<QString> schedule14;
schedule14.append("Понедельник: Рус. яз. (213), Матем. (218), Окр. мир (220), Физ. к. и зд. (C/3), ИЗО (225), Музыка (227)");
schedule14.append("Вторник: Рус. лит. (213), Матем. (218), Англ. яз. (210), Труд. обуч. (123), Чтение (213), Физ. к. и зд. (C/3)");
schedule14.append("Среда: Рус. яз. (213), Матем. (218), Окр. мир (220), Англ. яз. (210), ИЗО (225), Классный час (301)");
schedule14.append("Четверг: Рус. лит. (213), Матем. (218), Англ. яз. (210), Физ. к. и зд. (C/3), Музыка (227), Труд. обуч. (123)");
schedule14.append("Пятница: Рус. яз. (213), Матем. (218), Окр. мир (220), Англ. яз. (210), Физ. к. и зд. (C/3), Чтение (213)");
schedule14.append("Суббота: Выходной");
schedule14.append("Воскресенье: Выходной");
schedules.insert("1-4", schedule14);

// Расписание для 5-9 классов
List<QString> schedule59;
schedule59.append("Понедельник: Рус. лит. (213), Рус. яз. (213), Англ. яз. (210/316), Матем. (218), Физ. к. и зд. (C/3), География (321)");
schedule59.append("Вторник: Информ. (323/324/325), Англ. яз. (223/316), Матем. (318), ЧЗС (C/3), Бел. яз. (301), Бел. лит. (301)");
schedule59.append("Среда: Англ. яз. (223/316), Искусство (225), Рус. яз. (213), Ист. Бел. (224), Матем. (318), Рус. лит. (213)");
schedule59.append("Четверг: Биология (220), Бел. яз. (301), Англ. яз. (218/316), Физ. к. и зд. (C/3), Всемир. ист. (224), Матем. (210)");
schedule59.append("Пятница: Труд. обуч. (123/118), Матем. (209), Рус. яз. (213), Бел. лит. (301), Англ. яз. (223/316), Бел. яз. (301)");
schedule59.append("Суббота: Выходной");
schedule59.append("Воскресенье: Выходной");
schedules.insert("5-9", schedule59);

// Расписание для 10-11 классов
List<QString> schedule1011;
schedule1011.append("Понедельник: Рус. яз. (213), Матем. (318), Физика (315), Химия (314), Англ. яз. (316), Обществознание (224)");
schedule1011.append("Вторник: Рус. лит. (213), Матем. (318), Биология (220), История (224), Англ. яз. (316), Физ. к. и зд. (C/3)");
schedule1011.append("Среда: Матем. (318), Физика (315), Химия (314), Информ. (323), Англ. яз. (316), География (321)");
schedule1011.append("Четверг: Рус. яз. (213), Матем. (318), Биология (220), История (224), Англ. яз. (316), Обществознание (224)");
schedule1011.append("Пятница: Рус. лит. (213), Матем. (318), Физика (315), Химия (314), Англ. яз. (316), Физ. к. и зд. (C/3)");
schedule1011.append("Суббота: Выходной");
schedule1011.append("Воскресенье: Выходной");
schedules.insert("10-11", schedule1011);

// Распределяем расписания по классам
for (int i = 0; i < classes.size(); i++) {
QString cls = classes[i];
QString scheduleKey;
int classNum = 0;
// Извлекаем номер класса
if (cls.length() >= 2) {
QString numStr = cls.left(cls.length() - 1);
bool ok;
classNum = numStr.toInt(&ok);
if (!ok) classNum = 0;
}

if (classNum >= 1 && classNum <= 4) {
scheduleKey = "1-4";
} else if (classNum >= 5 && classNum <= 9) {
scheduleKey = "5-9";
} else if (classNum >= 10 && classNum <= 11) {
scheduleKey = "10-11";
} else {
scheduleKey = "5-9"; // По умолчанию
}

if (schedules.contains(scheduleKey)) {
saveSchedule(schedules[scheduleKey], cls);
}
}

// Сохраняем общее расписание
saveSchedule(schedules.value("5-9"), "");
}

// ---------- GRADES ----------
List<QString> DataStore::loadGrades() {
List<QString> out;
QFile f(gradesFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return out;
QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);
int lineCount = 0;
const int MAX_LINES = 100000; // Ограничение на количество строк
while (!in.atEnd() && lineCount < MAX_LINES) {
QString line = in.readLine().trimmed();
if (!line.isEmpty()) {
// Проверяем, что оценка не пустая (формат: класс|ученик|предмет|дата|оценка)
QStringList parts = line.split('|');
if (parts.size() == 5) {
QString grade = parts[4].trimmed();
// Загружаем только строки с непустой оценкой
if (!grade.isEmpty()) {
out.append(line);
lineCount++;
}
} else {
// Если формат неправильный, все равно добавляем (для совместимости)
out.append(line);
lineCount++;
}
}
}
return out;
}

bool DataStore::addGrade(const QString &cls, const QString &st,
const QString &sub, const QString &date,
const QString &gr) {
// Сначала загружаем все оценки
List<QString> allLines = loadGrades();
List<QString> newLines;
bool found = false;

for (int i = 0; i < allLines.size(); i++) {
QString line = allLines[i];
QStringList parts = line.split('|');
if (parts.size() == 5) {
// Проверяем, совпадает ли класс, ученик, предмет и дата
if (parts[0] == cls && parts[1] == st && parts[2] == sub && parts[3] == date) {
// Нашли существующую оценку
found = true;
// Если gr не пустая, заменяем оценку, иначе удаляем (не добавляем в newLines)
if (!gr.isEmpty()) {
QString newLine = cls + "|" + st + "|" + sub + "|" + date + "|" + gr;
newLines.append(newLine);
}
// Если gr пустая, просто не добавляем строку (удаляем оценку)
} else {
// Оставляем другие оценки без изменений
newLines.append(line);
}
} else {
newLines.append(line);
}
}

// Если оценка не найдена и gr не пустая, добавляем новую
if (!found && !gr.isEmpty()) {
QString newLine = cls + "|" + st + "|" + sub + "|" + date + "|" + gr;
newLines.append(newLine);
}

// Сохраняем все оценки обратно в файл
QFile f(gradesFile());
if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) return false;
QTextStream out(&f);
out.setEncoding(QStringConverter::Utf8);
for (const QString &line : newLines) {
// Проверяем, что строка не пустая и оценка не пустая
if (!line.isEmpty()) {
QStringList parts = line.split('|');
if (parts.size() == 5) {
QString grade = parts[4].trimmed();
// Сохраняем только строки с непустой оценкой
if (!grade.isEmpty()) {
out << line << "\n";
}
} else {
// Если формат неправильный, все равно сохраняем (для совместимости)
out << line << "\n";
}
}
}
return true;
}

bool DataStore::overwriteGrades(const List<QString> &lines) {
QFile f(gradesFile());
if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
return false;
QTextStream out(&f);
for (int i = 0; i < lines.size(); i++) {
out << lines[i] << "\n";
}
return true;
}

// ---------- CLASSES ----------
List<QString> DataStore::getClasses() {
List<QString> out;
QFile f(classesFile());
if (!f.exists()) {
// Если файла нет, создаем базовые классы
out.append("1а");
out.append("1б");
out.append("2а");
out.append("2б");
out.append("3а");
out.append("3б");
out.append("4а");
out.append("4б");
out.append("5а");
out.append("5б");
out.append("6а");
out.append("6б");
out.append("7а");
out.append("7б");
out.append("8а");
out.append("8б");
out.append("9а");
out.append("9б");
out.append("10а");
out.append("10б");
out.append("11а");
out.append("11б");
QFile nf(classesFile());
if (nf.open(QIODevice::WriteOnly | QIODevice::Text)) {
QTextStream o(&nf);
o.setEncoding(QStringConverter::Utf8);
for (int i = 0; i < out.size(); i++) {
o << out[i] << "\n";
}
}
return out;
}
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return out;
QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);
while (!in.atEnd()) {
QString x = in.readLine().trimmed();
if (!x.isEmpty()) out.append(x);
}
return out;
}

List<QString> DataStore::getStudentsForClass(const QString &cls) {
List<QString> out;

// Валидация аргумента
ExceptionHandler::validateArgument(!cls.isEmpty(), "Имя класса не может быть пустым");

try {
QFile f(studentsFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
throw FileException("Не удалось открыть файл студентов для чтения");
}

QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);
while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (line.isEmpty()) continue;
QStringList p = line.split('\t');
// Формат: Фам\tИмя\tОтч\tЛогин\tПароль\tКласс
if (p.size() >= 6) {
// Проверка границ перед доступом к элементам
try {
ExceptionHandler::checkBounds(p, 5);
QString fileClass = p[5].trimmed();
// Сравниваем классы (учитываем возможные различия в регистре)
if (fileClass == cls || fileClass.toLower() == cls.toLower()) {
ExceptionHandler::checkBounds(p, 0);
ExceptionHandler::checkBounds(p, 1);
ExceptionHandler::checkBounds(p, 2);
ExceptionHandler::checkBounds(p, 3);
QString fullName = p[0] + " " + p[1] + " " + p[2] + " (" + p[3] + ")";
out.push_back(fullName);
}
} catch (const OutOfRangeException& e) {
// Пропускаем некорректную строку
continue;
}
}
}
} catch (const InvalidArgumentException& e) {
ExceptionHandler::handleInvalidArgument(std::invalid_argument(e.getMessage().toStdString()));
return out;
} catch (const FileException& e) {
QString errorMsg;
ExceptionHandler::safeExecute([&e]() { throw e; }, errorMsg);
return out;
} catch (const std::exception& e) {
ExceptionHandler::handleException(e);
return out;
} catch (...) {
ExceptionHandler::handleUnknownException();
return out;
}

return out;
}

bool DataStore::addStudentToClass(const QString &cls, const QString &fullName, const QString &login, const QString &password) {
// Проверяем, не существует ли уже ученик с таким логином
QFile checkFile(studentsFile());
if (checkFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
QTextStream checkIn(&checkFile);
checkIn.setEncoding(QStringConverter::Utf8);
while (!checkIn.atEnd()) {
QString line = checkIn.readLine().trimmed();
if (line.isEmpty()) continue;
QStringList p = line.split('\t');
// Формат: Фам\tИмя\tОтч\tЛогин\tПароль\tКласс
if (p.size() >= 4 && p[3].trimmed() == login.trimmed()) {
checkFile.close();
return false; // Ученик с таким логином уже существует
}
}
checkFile.close();
}

QFile f(studentsFile());
if (!f.open(QIODevice::Append | QIODevice::Text)) return false;
QTextStream out(&f);
out.setEncoding(QStringConverter::Utf8);
// Формат: Фам\tИмя\tОтч\tЛогин\tПароль\tКласс
QStringList nameParts = fullName.split(' ');
if (nameParts.size() >= 3) {
out << nameParts[0].trimmed() << "\t" << nameParts[1].trimmed() << "\t" << nameParts[2].trimmed() 
<< "\t" << login.trimmed() << "\t" << password << "\t" << cls.trimmed() << "\n";
} else if (nameParts.size() == 2) {
out << nameParts[0].trimmed() << "\t" << nameParts[1].trimmed() << "\t" 
<< "\t" << login.trimmed() << "\t" << password << "\t" << cls.trimmed() << "\n";
} else {
out << fullName.trimmed() << "\t\t\t" << login.trimmed() << "\t" << password << "\t" << cls.trimmed() << "\n";
}
f.close(); // Явно закрываем файл, чтобы данные записались
return true;
}

bool DataStore::removeStudentFromClass(const QString &cls, const QString &name) {
QFile f(studentsFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

List<QString> lines;
QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);

while (!in.atEnd()) {
QString l = in.readLine();
QStringList p = l.split('\t');
// Формат: Фам\tИмя\tОтч\tЛогин\tПароль\tКласс
if (p.size() < 6 || !(p[5].trimmed() == cls && l.contains(name)))
lines.append(l);
}

f.close();

if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
return false;

QTextStream out(&f);
out.setEncoding(QStringConverter::Utf8);
for (int i = 0; i < lines.size(); i++) {
QString l = lines[i];
out << l;
if (!l.endsWith('\n')) out << "\n";
}
f.close();
return true;
}

Map<QString, List<int>> DataStore::loadGrades(const QString &studentLogin) {
Map<QString, List<int>> out;

// Валидация аргумента
ExceptionHandler::validateArgument(!studentLogin.isEmpty(), "Логин студента не может быть пустым");

try {
QFile f(gradesFile());
if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
throw FileException("Не удалось открыть файл оценок для чтения");
}

QTextStream in(&f);
in.setEncoding(QStringConverter::Utf8);

while (!in.atEnd()) {
QString line = in.readLine().trimmed();
if (line.isEmpty()) continue;
QStringList parts = line.split('|');
if (parts.size() == 5) {
try {
// Проверка границ перед доступом
ExceptionHandler::checkBounds(parts, 1);
ExceptionHandler::checkBounds(parts, 2);
ExceptionHandler::checkBounds(parts, 4);

QString student = parts[1];
QString subject = parts[2];
QString gradeStr = parts[4];

if (student == studentLogin || student.contains(studentLogin)) {
bool ok;
int grade = gradeStr.toInt(&ok);
if (ok) {
// Проверка переполнения и подполнения для оценки
ExceptionHandler::checkOverflow(grade, 10);
ExceptionHandler::checkUnderflow(grade, 1);
if (!out.contains(subject)) {
out.insert(subject, List<int>());
}
out[subject].append(grade);
}
}
} catch (const OutOfRangeException& e) {
// Пропускаем некорректную строку
continue;
} catch (const OverflowErrorException& e) {
// Оценка слишком большая, пропускаем
continue;
} catch (const UnderflowErrorException& e) {
// Оценка слишком маленькая, пропускаем
continue;
}
}
}
} catch (const InvalidArgumentException& e) {
ExceptionHandler::handleInvalidArgument(std::invalid_argument(e.getMessage().toStdString()));
return out;
} catch (const FileException& e) {
QString errorMsg;
ExceptionHandler::safeExecute([&e]() { throw e; }, errorMsg);
return out;
} catch (const std::exception& e) {
ExceptionHandler::handleException(e);
return out;
} catch (...) {
ExceptionHandler::handleUnknownException();
return out;
}

return out;
}

