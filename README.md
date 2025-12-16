# Academic Ledger

Система управления академическими записями.

## Требования

- Qt 5.15+ или Qt 6.0+
- CMake 3.16+
- C++17 совместимый компилятор

## Установка Qt

### Windows

1. Скачайте Qt Online Installer с официального сайта: https://www.qt.io/download-qt-installer
2. Запустите установщик и выберите:
   - Qt 5.15.2 или Qt 6.5+ (рекомендуется)
   - MinGW 64-bit компилятор
   - Qt Creator (опционально)

### Альтернативный способ через vcpkg

```bash
# Установите vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Установите Qt5
.\vcpkg install qt5-base qt5-widgets

# Или Qt6
.\vcpkg install qt6-base qt6-widgets
```

## Сборка проекта

1. Откройте проект в CLion
2. Убедитесь, что Qt найден CMake
3. Нажмите Build или используйте команду:

```bash
cmake -B build -S .
cmake --build build
```

## Настройка CMake в CLion

Если Qt не найден автоматически:

1. Откройте Settings → Build, Execution, Deployment → CMake
2. Добавьте переменную окружения:
   - `CMAKE_PREFIX_PATH` = путь к установке Qt (например, `C:/Qt/6.5.0/mingw_64`)

## Использование

### Демо аккаунты

- **Администратор**: admin / admin123
- **Преподаватель**: teacher1 / teacher123
- **Студент**: student1 / student123

### Функции

- **Студенты**: просмотр своих оценок
- **Преподаватели**: добавление и просмотр оценок студентов
- **Администраторы**: управление пользователями и просмотр всех данных

## Структура проекта


## Решение проблем

### Qt не найден

1. Убедитесь, что Qt установлен
2. Проверьте переменную `CMAKE_PREFIX_PATH`
3. Перезапустите CLion после установки Qt

### Ошибки компиляции

1. Убедитесь, что используется C++17
2. Проверьте, что все заголовочные файлы Qt доступны
3. Очистите кэш CMake: File → Invalidate Caches and Restart

## Лицензия

Проект создан на основе оригинального Academic Ledger с GitHub.


