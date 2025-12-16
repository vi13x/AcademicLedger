#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QVariant>
#include <QString>

// Класс, отвечающий за работу с файлами (сохранение/чтение «кольца»)
class FileManager : public QObject {
Q_OBJECT
public:
explicit FileManager(QObject *parent = nullptr);

// Сохранить данные кольца в файл. Возвращает true при успехе.
bool saveFile(const QVariant &ring);

void setFilename(const QString &name);
QString getFilename() const;
bool saved() const;

signals:
// Передаёт прочитанное из файла кольцо в AnotherWindow
void readRing(QVariant ring);

private:
bool isSaved;
QString filename;
};

#endif // FILEMANAGER_H

