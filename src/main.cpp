#include <QApplication>
#include "mainwindow.h"
#include "exceptionhandler.h"
#include <exception>

int main(int argc, char *argv[])
{
QApplication a(argc, argv);

// Можно при желании задать стиль/палитру здесь

try {
MainWindow w;
w.showMaximized();
return a.exec();
} catch (const std::bad_alloc& e) {
ExceptionHandler::handleBadAlloc(e);
return -1;
} catch (const std::bad_cast& e) {
ExceptionHandler::handleBadCast(e);
return -1;
} catch (const std::bad_typeid& e) {
ExceptionHandler::handleBadTypeId(e);
return -1;
} catch (const std::domain_error& e) {
ExceptionHandler::handleDomainError(e);
return -1;
} catch (const std::invalid_argument& e) {
ExceptionHandler::handleInvalidArgument(e);
return -1;
} catch (const std::length_error& e) {
ExceptionHandler::handleLengthError(e);
return -1;
} catch (const std::out_of_range& e) {
ExceptionHandler::handleOutOfRange(e);
return -1;
} catch (const std::overflow_error& e) {
ExceptionHandler::handleOverflowError(e);
return -1;
} catch (const std::underflow_error& e) {
ExceptionHandler::handleUnderflowError(e);
return -1;
} catch (const std::system_error& e) {
ExceptionHandler::handleSystemError(e);
return -1;
} catch (const std::exception& e) {
ExceptionHandler::handleException(e);
return -1;
} catch (...) {
ExceptionHandler::handleUnknownException();
return -1;
}
}
