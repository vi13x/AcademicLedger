cmake_minimum_required(VERSION 3.16)
project(ElectronicJournal VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Включение автоматической обработки MOC
set(CMAKE_AUTOMOC ON)

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/studentview.cpp
    src/teacherview.cpp
    src/adminview.cpp
    src/datastore.cpp
    src/logindialog.cpp
    src/markdialog.cpp
)

# Заголовочные файлы
set(HEADERS
    include/mainwindow.h
    include/studentview.h
    include/teacherview.h
    include/adminview.h
    include/datastore.h
    include/logindialog.h
    include/markdialog.h
    include/user.h
)

# UI файлы (если есть)
set(UI_FILES
    ui/mainwindow.ui
)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES})

# Подключение Qt модулей
target_link_libraries(${PROJECT_NAME} 
    Qt6::Core 
    Qt6::Widgets
)

# Копирование стилей в папку сборки
file(COPY styles/ DESTINATION ${CMAKE_BINARY_DIR}/styles/)

# Копирование данных в папку сборки
file(COPY data/ DESTINATION ${CMAKE_BINARY_DIR}/data/)

# Настройка для Windows
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Настройка для отладки
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()

# Установка
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY styles/
    DESTINATION bin/styles
)

install(DIRECTORY data/
    DESTINATION bin/data
)