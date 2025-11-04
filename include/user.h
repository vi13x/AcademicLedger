#pragma once
#include <QString>

struct User {
    QString username;
    QString password;
    QString role; // "student", "teacher", "admin"
};
