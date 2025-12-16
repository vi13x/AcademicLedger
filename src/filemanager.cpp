#include "filemanager.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QVariantList>

FileManager::FileManager(QObject *parent)
: QObject(parent), isSaved(false) {}

void FileManager::setFilename(const QString &name) {
filename = name;
}

QString FileManager::getFilename() const {
return filename;
}

bool FileManager::saved() const {
return isSaved;
}

bool FileManager::saveFile(const QVariant &ring) {
// Если имя файла не задано — спрашиваем у пользователя
if (filename.isEmpty()) {
const QString chosen = QFileDialog::getSaveFileName(nullptr,
tr("Сохранить файл"),
QString(),
tr("Текстовые файлы (*.txt);;Все файлы (*.*)"));
if (chosen.isEmpty()) {
isSaved = false;
return false;
}
filename = chosen;
}

QFile file(filename);
if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
QMessageBox::warning(nullptr, tr("Ошибка"), tr("Не удалось открыть файл для записи:\n%1").arg(filename));
isSaved = false;
return false;
}

QTextStream out(&file);

// Пишем содержимое QVariant максимально дружелюбно
if (ring.canConvert<QStringList>()) {
const QStringList data = ring.toStringList();
for (const QString &line : data) {
out << line << '\n';
}
} else if (ring.canConvert<QVariantList>()) {
const QVariantList data = ring.toList();
for (const QVariant &v : data) {
out << v.toString() << '\n';
}
} else {
out << ring.toString();
}

file.close();
isSaved = true;
return true;
}

