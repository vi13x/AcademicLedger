#include "exceptionhandler.h"
#include <QMessageBox>
#include <QDebug>
#include <QApplication>

// ОБРАБОТКА СТАНДАРТНЫХ ИСКЛЮЧЕНИЙ C++

void ExceptionHandler::handleBadAlloc(const std::bad_alloc& e) {
QString message = "Критическая ошибка: Недостаточно памяти!\n\n"
"Приложение не может выделить необходимую память.\n"
"Попробуйте:\n"
"- Закрыть другие приложения\n"
"- Освободить память\n"
"- Перезапустить приложение";

qCritical() << "BadAlloc:" << e.what();

if (QApplication::instance()) {
QMessageBox::critical(nullptr, "Ошибка памяти", message);
}
}

void ExceptionHandler::handleBadCast(const std::bad_cast& e) {
QString message = "Ошибка приведения типа!\n\n"
"Попытка неверного приведения типа данных.\n"
"Проверьте корректность операций с типами.";

qCritical() << "BadCast:" << e.what();

if (QApplication::instance()) {
QMessageBox::warning(nullptr, "Ошибка приведения типа", message);
}
}

void ExceptionHandler::handleBadTypeId(const std::bad_typeid& e) {
QString message = "Ошибка получения информации о типе!\n\n"
"Не удалось получить информацию о типе объекта.\n"
"Возможно, объект не инициализирован.";

qCritical() << "BadTypeId:" << e.what();

if (QApplication::instance()) {
QMessageBox::warning(nullptr, "Ошибка типа", message);
}
}

// ОБРАБОТКА ЛОГИЧЕСКИХ ОШИБОК

void ExceptionHandler::handleDomainError(const std::domain_error& e) {
QString message = QString("Ошибка области определения!\n\n%1")
.arg(QString::fromStdString(e.what()));

qWarning() << "DomainError:" << e.what();

if (QApplication::instance()) {
QMessageBox::warning(nullptr, "Ошибка области определения", message);
}
}

void ExceptionHandler::handleInvalidArgument(const std::invalid_argument& e) {
QString message = QString("Неверный аргумент!\n\n%1")
.arg(QString::fromStdString(e.what()));

qWarning() << "InvalidArgument:" << e.what();

if (QApplication::instance()) {
QMessageBox::warning(nullptr, "Неверный аргумент", message);
}
}

void ExceptionHandler::handleLengthError(const std::length_error& e) {
QString message = QString("Ошибка длины!\n\n%1")
.arg(QString::fromStdString(e.what()));

qWarning() << "LengthError:" << e.what();

if (QApplication::instance()) {
QMessageBox::warning(nullptr, "Ошибка длины", message);
}
}

void ExceptionHandler::handleOutOfRange(const std::out_of_range& e) {
QString message = QString("Выход за границы диапазона!\n\n%1\n\n"
"Проверьте индексы массивов и контейнеров.")
.arg(QString::fromStdString(e.what()));

qWarning() << "OutOfRange:" << e.what();

if (QApplication::instance()) {
QMessageBox::warning(nullptr, "Выход за границы", message);
}
}

// ОБРАБОТКА ОШИБОК ПЕРЕПОЛНЕНИЯ

void ExceptionHandler::handleOverflowError(const std::overflow_error& e) {
QString message = QString("Переполнение значения!\n\n%1\n\n"
"Значение превышает максимально допустимое.")
.arg(QString::fromStdString(e.what()));

qWarning() << "OverflowError:" << e.what();

if (QApplication::instance()) {
QMessageBox::warning(nullptr, "Переполнение", message);
}
}

void ExceptionHandler::handleUnderflowError(const std::underflow_error& e) {
QString message = QString("Потеря точности значения!\n\n%1\n\n"
"Значение слишком мало для точного представления.")
.arg(QString::fromStdString(e.what()));

qWarning() << "UnderflowError:" << e.what();

if (QApplication::instance()) {
QMessageBox::warning(nullptr, "Потеря точности", message);
}
}

// ОБРАБОТКА ИСКЛЮЧЕНИЙ ЯЗЫКОВОЙ ПОДДЕРЖКИ

void ExceptionHandler::handleLocaleException() {
QString message = "Ошибка языковой поддержки!\n\n"
"Не удалось использовать локаль или фасет.\n"
"Проверьте настройки локализации системы.";

qWarning() << "LocaleException: Ошибка языковой поддержки";

if (QApplication::instance()) {
QMessageBox::warning(nullptr, "Ошибка локали", message);
}
}

// ОБРАБОТКА СИСТЕМНЫХ ОШИБОК

void ExceptionHandler::handleSystemError(const std::system_error& e) {
QString message = QString("Системная ошибка!\n\n%1\n\nКод ошибки: %2")
.arg(QString::fromStdString(e.what()))
.arg(e.code().value());

qCritical() << "SystemError:" << e.what() << "Code:" << e.code().value();

if (QApplication::instance()) {
QMessageBox::critical(nullptr, "Системная ошибка", message);
}
}

// УНИВЕРСАЛЬНАЯ ОБРАБОТКА

void ExceptionHandler::handleException(const std::exception& e) {
QString message = QString("Произошла ошибка!\n\n%1")
.arg(QString::fromStdString(e.what()));

qCritical() << "Exception:" << e.what();

if (QApplication::instance()) {
QMessageBox::critical(nullptr, "Ошибка", message);
}
}

void ExceptionHandler::handleUnknownException() {
QString message = "Произошла неизвестная ошибка!\n\n"
"Приложение столкнулось с непредвиденной ситуацией.\n"
"Попробуйте повторить операцию или перезапустить приложение.";

qCritical() << "UnknownException: Неизвестная ошибка";

if (QApplication::instance()) {
QMessageBox::critical(nullptr, "Неизвестная ошибка", message);
}
}

// ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ

void ExceptionHandler::validateArgument(bool condition, const QString& message) {
if (!condition) {
throw InvalidArgumentException(message);
}
}

void ExceptionHandler::checkOverflow(long long value, long long max) {
if (value > max) {
throw OverflowErrorException(
QString("Значение %1 превышает максимальное %2").arg(value).arg(max)
);
}
}

void ExceptionHandler::checkUnderflow(long long value, long long min) {
if (value < min) {
throw UnderflowErrorException(
QString("Значение %1 меньше минимального %2").arg(value).arg(min)
);
}
}

