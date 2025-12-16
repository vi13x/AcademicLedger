#ifndef EXCEPTIONHANDLER_H
#define EXCEPTIONHANDLER_H

#include <QString>
#include <exception>
#include <stdexcept>
#include <new>
#include <typeinfo>
#include <locale>
#include <system_error>

// БАЗОВЫЙ КЛАСС ИСКЛЮЧЕНИЙ
class BaseException : public std::exception {
public:
BaseException(const QString& message) : m_message(message.toStdString()) {}
virtual ~BaseException() noexcept = default;

const char* what() const noexcept override {
return m_message.c_str();
}

QString getMessage() const {
return QString::fromStdString(m_message);
}

protected:
std::string m_message;
};

// ИСКЛЮЧЕНИЯ СТАНДАРТНОЙ БИБЛИОТЕКИ C++

// Bad allocation (нехватка памяти)
class BadAllocException : public BaseException {
public:
BadAllocException(const QString& message = "Ошибка выделения памяти") 
: BaseException(message) {}
};

// Bad cast (неверное приведение типа)
class BadCastException : public BaseException {
public:
BadCastException(const QString& message = "Ошибка приведения типа") 
: BaseException(message) {}
};

// Bad typeid (ошибка получения информации о типе)
class BadTypeIdException : public BaseException {
public:
BadTypeIdException(const QString& message = "Ошибка получения информации о типе") 
: BaseException(message) {}
};

// ЛОГИЧЕСКИЕ ОШИБКИ

// Domain error (ошибка области определения)
class DomainErrorException : public BaseException {
public:
DomainErrorException(const QString& message = "Ошибка области определения") 
: BaseException(message) {}
};

// Invalid argument (неверный аргумент)
class InvalidArgumentException : public BaseException {
public:
InvalidArgumentException(const QString& message = "Неверный аргумент") 
: BaseException(message) {}
};

// Length error (ошибка длины)
class LengthErrorException : public BaseException {
public:
LengthErrorException(const QString& message = "Ошибка длины") 
: BaseException(message) {}
};

// Out of range (выход за границы)
class OutOfRangeException : public BaseException {
public:
OutOfRangeException(const QString& message = "Выход за границы диапазона") 
: BaseException(message) {}
};

// ОШИБКИ ПЕРЕПОЛНЕНИЯ

// Overflow error (переполнение сверху)
class OverflowErrorException : public BaseException {
public:
OverflowErrorException(const QString& message = "Переполнение значения") 
: BaseException(message) {}
};

// Underflow error (переполнение снизу)
class UnderflowErrorException : public BaseException {
public:
UnderflowErrorException(const QString& message = "Потеря точности значения") 
: BaseException(message) {}
};

// ИСКЛЮЧЕНИЯ ЯЗЫКОВОЙ ПОДДЕРЖКИ (LOCALE)
class LocaleException : public BaseException {
public:
LocaleException(const QString& message = "Ошибка языковой поддержки") 
: BaseException(message) {}
};

// ИСКЛЮЧЕНИЯ ВНЕШНИХ ОШИБОК

// Файловые ошибки
class FileException : public BaseException {
public:
FileException(const QString& message = "Ошибка работы с файлом") 
: BaseException(message) {}
};

// Сетевые ошибки
class NetworkException : public BaseException {
public:
NetworkException(const QString& message = "Ошибка сети") 
: BaseException(message) {}
};

// Ошибки базы данных
class DatabaseException : public BaseException {
public:
DatabaseException(const QString& message = "Ошибка базы данных") 
: BaseException(message) {}
};

// Системные ошибки
class SystemException : public BaseException {
public:
SystemException(const QString& message = "Системная ошибка") 
: BaseException(message) {}
};

// КЛАСС ОБРАБОТКИ ИСКЛЮЧЕНИЙ
class ExceptionHandler {
public:
// Обработка стандартных исключений C++
static void handleBadAlloc(const std::bad_alloc& e);
static void handleBadCast(const std::bad_cast& e);
static void handleBadTypeId(const std::bad_typeid& e);

// Обработка логических ошибок
static void handleDomainError(const std::domain_error& e);
static void handleInvalidArgument(const std::invalid_argument& e);
static void handleLengthError(const std::length_error& e);
static void handleOutOfRange(const std::out_of_range& e);

// Обработка ошибок переполнения
static void handleOverflowError(const std::overflow_error& e);
static void handleUnderflowError(const std::underflow_error& e);

// Обработка исключений языковой поддержки
static void handleLocaleException();

// Обработка системных ошибок
static void handleSystemError(const std::system_error& e);

// Универсальная обработка всех исключений
static void handleException(const std::exception& e);
static void handleUnknownException();

// Безопасное выполнение функции с обработкой исключений
template<typename Func>
static bool safeExecute(Func func, QString& errorMessage);

// Проверка и обработка исключений при выделении памяти
template<typename T>
static T* safeAllocate(size_t count);

// Проверка границ массива
template<typename Container>
static void checkBounds(const Container& container, size_t index);

// Проверка валидности аргумента
static void validateArgument(bool condition, const QString& message);

// Проверка переполнения
static void checkOverflow(long long value, long long max);
static void checkUnderflow(long long value, long long min);
};

// РЕАЛИЗАЦИЯ ШАБЛОННЫХ МЕТОДОВ

template<typename Func>
bool ExceptionHandler::safeExecute(Func func, QString& errorMessage) {
try {
func();
return true;
} catch (const BadAllocException& e) {
errorMessage = "Ошибка выделения памяти: " + e.getMessage();
handleBadAlloc(std::bad_alloc());
return false;
} catch (const BadCastException& e) {
errorMessage = "Ошибка приведения типа: " + e.getMessage();
handleBadCast(std::bad_cast());
return false;
} catch (const DomainErrorException& e) {
errorMessage = "Ошибка области определения: " + e.getMessage();
handleDomainError(std::domain_error(e.getMessage().toStdString()));
return false;
} catch (const InvalidArgumentException& e) {
errorMessage = "Неверный аргумент: " + e.getMessage();
handleInvalidArgument(std::invalid_argument(e.getMessage().toStdString()));
return false;
} catch (const LengthErrorException& e) {
errorMessage = "Ошибка длины: " + e.getMessage();
handleLengthError(std::length_error(e.getMessage().toStdString()));
return false;
} catch (const OutOfRangeException& e) {
errorMessage = "Выход за границы: " + e.getMessage();
handleOutOfRange(std::out_of_range(e.getMessage().toStdString()));
return false;
} catch (const OverflowErrorException& e) {
errorMessage = "Переполнение: " + e.getMessage();
handleOverflowError(std::overflow_error(e.getMessage().toStdString()));
return false;
} catch (const UnderflowErrorException& e) {
errorMessage = "Потеря точности: " + e.getMessage();
handleUnderflowError(std::underflow_error(e.getMessage().toStdString()));
return false;
} catch (const FileException& e) {
errorMessage = "Ошибка файла: " + e.getMessage();
return false;
} catch (const NetworkException& e) {
errorMessage = "Ошибка сети: " + e.getMessage();
return false;
} catch (const DatabaseException& e) {
errorMessage = "Ошибка БД: " + e.getMessage();
return false;
} catch (const SystemException& e) {
errorMessage = "Системная ошибка: " + e.getMessage();
return false;
} catch (const std::bad_alloc& e) {
errorMessage = "Ошибка выделения памяти";
handleBadAlloc(e);
return false;
} catch (const std::bad_cast& e) {
errorMessage = "Ошибка приведения типа";
handleBadCast(e);
return false;
} catch (const std::bad_typeid& e) {
errorMessage = "Ошибка получения информации о типе";
handleBadTypeId(e);
return false;
} catch (const std::domain_error& e) {
errorMessage = QString::fromStdString(e.what());
handleDomainError(e);
return false;
} catch (const std::invalid_argument& e) {
errorMessage = QString::fromStdString(e.what());
handleInvalidArgument(e);
return false;
} catch (const std::length_error& e) {
errorMessage = QString::fromStdString(e.what());
handleLengthError(e);
return false;
} catch (const std::out_of_range& e) {
errorMessage = QString::fromStdString(e.what());
handleOutOfRange(e);
return false;
} catch (const std::overflow_error& e) {
errorMessage = QString::fromStdString(e.what());
handleOverflowError(e);
return false;
} catch (const std::underflow_error& e) {
errorMessage = QString::fromStdString(e.what());
handleUnderflowError(e);
return false;
} catch (const std::system_error& e) {
errorMessage = QString::fromStdString(e.what());
handleSystemError(e);
return false;
} catch (const std::exception& e) {
errorMessage = QString::fromStdString(e.what());
handleException(e);
return false;
} catch (...) {
errorMessage = "Неизвестная ошибка";
handleUnknownException();
return false;
}
}

template<typename T>
T* ExceptionHandler::safeAllocate(size_t count) {
try {
return new(std::nothrow) T[count];
} catch (const std::bad_alloc& e) {
handleBadAlloc(e);
throw BadAllocException("Не удалось выделить память для массива");
}
}

template<typename Container>
void ExceptionHandler::checkBounds(const Container& container, size_t index) {
if (index >= container.size()) {
throw OutOfRangeException(
QString("Индекс %1 выходит за границы контейнера размером %2")
.arg(index).arg(container.size())
);
}
}

#endif // EXCEPTIONHANDLER_H

