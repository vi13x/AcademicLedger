// Файл для инициализации демо-данных
// Этот файл можно использовать для создания начальных данных при первом запуске

#include "../include/datastore.h"
#include <QApplication>
#include <QMessageBox>

void initializeDemoData() {
    DataStore ds;
    
    // Создаем демо-учителей
    ds.addTeacher("Иванова А.И.", "teacher123");
    ds.addTeacher("Петров П.П.", "teacher123");
    ds.addTeacher("Сидорова С.С.", "teacher123");
    ds.addTeacher("Козлов К.К.", "teacher123");
    ds.addTeacher("Волкова В.В.", "teacher123");
    
    // Создаем демо-администратора
    ds.addAdmin("admin", "admin123");
    
    // Создаем демо-студентов
    ds.addStudent({"Иванов Иван", "student123", "student"});
    ds.addStudent({"Петров Петр", "student123", "student"});
    ds.addStudent({"Сидоров Сидор", "student123", "student"});
    ds.addStudent({"Козлова Анна", "student123", "student"});
    ds.addStudent({"Анна Смирнова", "student123", "student"});
    ds.addStudent({"Ольга Волкова", "student123", "student"});
    ds.addStudent({"Мария Новикова", "student123", "student"});
    
    // Генерируем базовое расписание
    ds.generateDefaultSchedule();
    
    // Добавляем демо-оценки
    ds.addGrade("1A", "Иванов Иван", "Математика", "2024-01-15", "5");
    ds.addGrade("1A", "Иванов Иван", "Русский язык", "2024-01-16", "4");
    ds.addGrade("1A", "Петров Петр", "Математика", "2024-01-15", "4");
    ds.addGrade("1A", "Петров Петр", "Физика", "2024-01-17", "5");
    ds.addGrade("1B", "Анна Смирнова", "История", "2024-01-16", "5");
    ds.addGrade("1B", "Ольга Волкова", "Литература", "2024-01-18", "4");
}
