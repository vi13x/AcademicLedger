#include "../include/adminview.h"
#include "../include/datastore.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFrame>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QTabWidget>
#include <QSplitter>
#include <QDate>
#include <QRegularExpression>

AdminView::AdminView(const QString &username, QWidget *parent) : QWidget(parent), username(username) {
    setWindowTitle(QString("АДМИНИСТРАТОР: %1").arg(username));
    resize(1200, 800);
    setMinimumSize(1000, 700);

    // Устанавливаем брутальные стили
    setStyleSheet(
        "QWidget { background-color: #1a1a1a; color: white; } "
        "QListWidget { background-color: #2a2a2a; border: 2px solid #404040; } "
        "QListWidget::item { padding: 12px; border-bottom: 1px solid #404040; font-weight: bold; } "
        "QListWidget::item:selected { background-color: #505050; } "
        "QListWidget::item:hover { background-color: #404040; } "
        "QTextEdit { background-color: #2a2a2a; border: 2px solid #404040; color: white; font-family: monospace; } "
        "QPushButton { background-color: #404040; color: white; border: 2px solid #606060; padding: 12px 24px; font-weight: bold; letter-spacing: 1px; } "
        "QPushButton:hover { background-color: #505050; border: 2px solid #707070; } "
        "QPushButton:pressed { background-color: #303030; border: 2px solid #505050; } "
        "QLabel { color: white; font-weight: bold; } "
    );

    QVBoxLayout *main = new QVBoxLayout(this);
    main->setSpacing(20);
    main->setContentsMargins(20, 20, 20, 20);

    // Заголовок в брутальном стиле
    QFrame *headerFrame = new QFrame(this);
    headerFrame->setStyleSheet("QFrame { background-color: #1a1a1a; border: 2px solid #404040; padding: 15px; }");
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    QLabel *userLbl = new QLabel(QString("АДМИНИСТРАТОР: %1").arg(username.toUpper()), this);
    userLbl->setStyleSheet("QLabel { color: white; font-size: 24px; font-weight: bold; letter-spacing: 2px; }");
    headerLayout->addWidget(userLbl);
    headerLayout->addStretch();
    
    QLabel *dateLabel = new QLabel(QDate::currentDate().toString("dd.MM.yyyy"), this);
    dateLabel->setStyleSheet("QLabel { color: #b0b0b0; font-size: 16px; font-weight: bold; }");
    headerLayout->addWidget(dateLabel);
    
    main->addWidget(headerFrame);

    // Основной контент с вкладками
    QTabWidget *mainTabs = new QTabWidget(this);
    mainTabs->setStyleSheet(
        "QTabWidget::pane { border: 2px solid #404040; background-color: #2a2a2a; } "
        "QTabBar::tab { background-color: #404040; color: white; padding: 12px 24px; margin-right: 2px; border: 1px solid #606060; font-weight: bold; letter-spacing: 1px; } "
        "QTabBar::tab:selected { background-color: #505050; border: 2px solid #707070; } "
        "QTabBar::tab:hover { background-color: #454545; } "
    );

    // Вкладка управления пользователями
    QWidget *usersTab = new QWidget();
    QHBoxLayout *usersLayout = new QHBoxLayout(usersTab);

    // Учителя
    QVBoxLayout *tcol = new QVBoxLayout();
    QLabel *teachersLabel = new QLabel("УЧИТЕЛЯ:", this);
    teachersLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; margin: 10px 0; letter-spacing: 1px; }");
    tcol->addWidget(teachersLabel);
    
    teachersList = new QListWidget(this);
    // load from file (Teachers.txt with tab-separated columns)
    {
        QFile tf(QDir::currentPath() + "/data/Teachers.txt");
        if (tf.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&tf);
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (line.isEmpty()) continue;
                QStringList parts = line.split('\t');
                if (parts.size() < 5) parts = line.split(QRegularExpression("\\s+"));
                if (parts.size() >= 3) {
                    teachersList->addItem(QString("%1 %2 %3").arg(parts[0].trimmed(), parts[1].trimmed(), parts[2].trimmed()));
                }
            }
        }
    }
    tcol->addWidget(teachersList);
    QPushButton *addT = new QPushButton("ДОБАВИТЬ УЧИТЕЛЯ", this);
    QPushButton *remT = new QPushButton("УДАЛИТЬ УЧИТЕЛЯ", this);
    tcol->addWidget(addT);
    tcol->addWidget(remT);

    usersLayout->addLayout(tcol);

    // Администраторы
    QVBoxLayout *acol = new QVBoxLayout();
    QLabel *adminsLabel = new QLabel("АДМИНИСТРАТОРЫ:", this);
    adminsLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; margin: 10px 0; letter-spacing: 1px; }");
    acol->addWidget(adminsLabel);
    
    adminsList = new QListWidget(this);
    {
        QFile af(QDir::currentPath() + "/data/Admins.txt");
        if (af.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&af);
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (line.isEmpty()) continue;
                QStringList parts = line.split('\t');
                if (parts.size() < 5) parts = line.split(QRegularExpression("\\s+"));
                if (parts.size() >= 3) {
                    adminsList->addItem(QString("%1 %2 %3").arg(parts[0].trimmed(), parts[1].trimmed(), parts[2].trimmed()));
                }
            }
        }
    }
    acol->addWidget(adminsList);
    QPushButton *addA = new QPushButton("ДОБАВИТЬ АДМИНА", this);
    QPushButton *remA = new QPushButton("УДАЛИТЬ АДМИНА", this);
    acol->addWidget(addA);
    acol->addWidget(remA);

    usersLayout->addLayout(acol);
    mainTabs->addTab(usersTab, "УПРАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯМИ");

    // Вкладка расписания
    QWidget *scheduleTab = new QWidget();
    QVBoxLayout *scheduleLayout = new QVBoxLayout(scheduleTab);
    
    QLabel *scheduleLabel = new QLabel("РАСПИСАНИЕ (КАЖДАЯ СТРОКА — ЗАПИСЬ):", this);
    scheduleLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; margin: 10px 0; letter-spacing: 1px; }");
    scheduleLayout->addWidget(scheduleLabel);
    
    scheduleEdit = new QTextEdit(this);
    // load schedule
    DataStore ds;
    auto lines = ds.loadSchedule();
    for (auto &l : lines) {
        scheduleEdit->append(l);
    }
    scheduleLayout->addWidget(scheduleEdit);
    
    QHBoxLayout *scheduleButtons = new QHBoxLayout();
    QPushButton *loadS = new QPushButton("ЗАГРУЗИТЬ (ОБНОВИТЬ)", this);
    QPushButton *saveS = new QPushButton("СОХРАНИТЬ РАСПИСАНИЕ", this);
    QPushButton *generateS = new QPushButton("СГЕНЕРИРОВАТЬ БАЗОВОЕ РАСПИСАНИЕ", this);
    scheduleButtons->addWidget(loadS);
    scheduleButtons->addWidget(saveS);
    scheduleButtons->addWidget(generateS);
    scheduleLayout->addLayout(scheduleButtons);

    mainTabs->addTab(scheduleTab, "РАСПИСАНИЕ");

    main->addWidget(mainTabs);
    
    // Сохраняем ссылку на mainTabs
    this->mainTabs = mainTabs;

    // Кнопки в брутальном стиле
    QHBoxLayout *bot = new QHBoxLayout();
    QPushButton *viewGradesBtn = new QPushButton("ПРОСМОТРЕТЬ ОЦЕНКИ", this);
    QPushButton *clearGradesBtn = new QPushButton("ОЧИСТИТЬ ВСЕ ОЦЕНКИ", this);
    QPushButton *logoutBtn = new QPushButton("ВЫЙТИ", this);

    bot->addWidget(viewGradesBtn);
    bot->addWidget(clearGradesBtn);
    bot->addStretch();
    bot->addWidget(logoutBtn);

    main->addLayout(bot);

    connect(addT, &QPushButton::clicked, this, &AdminView::onAddTeacher);
    connect(remT, &QPushButton::clicked, this, &AdminView::onRemoveTeacher);
    connect(addA, &QPushButton::clicked, this, &AdminView::onAddAdmin);
    connect(remA, &QPushButton::clicked, this, &AdminView::onRemoveAdmin);
    connect(loadS, &QPushButton::clicked, this, &AdminView::onLoadSchedule);
    connect(saveS, &QPushButton::clicked, this, &AdminView::onSaveSchedule);
    connect(generateS, &QPushButton::clicked, this, &AdminView::onGenerateSchedule);
    connect(viewGradesBtn, &QPushButton::clicked, this, &AdminView::onViewGrades);
    connect(clearGradesBtn, &QPushButton::clicked, this, &AdminView::onClearGrades);
    connect(logoutBtn, &QPushButton::clicked, this, &AdminView::onLogout);
}

void AdminView::onLogout() { close(); }

void AdminView::onAddTeacher() {
    bool ok;
    QString surname = QInputDialog::getText(this, "Добавить учителя", "Фамилия:", QLineEdit::Normal, "", &ok); if (!ok || surname.trimmed().isEmpty()) return;
    QString name = QInputDialog::getText(this, "Добавить учителя", "Имя:", QLineEdit::Normal, "", &ok); if (!ok || name.trimmed().isEmpty()) return;
    QString patronymic = QInputDialog::getText(this, "Добавить учителя", "Отчество:", QLineEdit::Normal, "", &ok); if (!ok || patronymic.trimmed().isEmpty()) return;
    QString login = QInputDialog::getText(this, "Добавить учителя", "Логин:", QLineEdit::Normal, "", &ok); if (!ok || login.trimmed().isEmpty()) return;
    QString pwd = QInputDialog::getText(this, "Добавить учителя", "Пароль:", QLineEdit::Password, "", &ok); if (!ok) return;
    QString subject = QInputDialog::getText(this, "Добавить учителя", "Дисциплина:", QLineEdit::Normal, "", &ok); if (!ok || subject.trimmed().isEmpty()) return;
    QFile f(QDir::currentPath() + "/data/Teachers.txt");
    if (f.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&f);
        out << surname.trimmed() << "\t" << name.trimmed() << "\t" << patronymic.trimmed() << "\t" << login.trimmed() << "\t" << pwd << "\t" << subject.trimmed() << "\n";
        teachersList->addItem(QString("%1 %2 %3").arg(surname.trimmed(), name.trimmed(), patronymic.trimmed()));
    }
}

void AdminView::onRemoveTeacher() {
    if (!teachersList->currentItem()) return;
    QString fio = teachersList->currentItem()->text();
    // Удаляем по логину, спросим логин для точности
    bool ok;
    QString login = QInputDialog::getText(this, "Удалить учителя", "Логин учителя:", QLineEdit::Normal, "", &ok);
    if (!ok || login.trimmed().isEmpty()) return;
    DataStore ds;
    ds.removeTeacher(login.trimmed());
    delete teachersList->takeItem(teachersList->currentRow());
}

void AdminView::onAddAdmin() {
    bool ok;
    QString surname = QInputDialog::getText(this, "Добавить администратора", "Фамилия:", QLineEdit::Normal, "", &ok); if (!ok || surname.trimmed().isEmpty()) return;
    QString name = QInputDialog::getText(this, "Добавить администратора", "Имя:", QLineEdit::Normal, "", &ok); if (!ok || name.trimmed().isEmpty()) return;
    QString patronymic = QInputDialog::getText(this, "Добавить администратора", "Отчество:", QLineEdit::Normal, "", &ok); if (!ok || patronymic.trimmed().isEmpty()) return;
    QString login = QInputDialog::getText(this, "Добавить администратора", "Логин:", QLineEdit::Normal, "", &ok); if (!ok || login.trimmed().isEmpty()) return;
    QString pwd = QInputDialog::getText(this, "Добавить администратора", "Пароль:", QLineEdit::Password, "", &ok); if (!ok) return;
    QFile f(QDir::currentPath() + "/data/Admins.txt");
    if (f.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&f);
        out << surname.trimmed() << "\t" << name.trimmed() << "\t" << patronymic.trimmed() << "\t" << login.trimmed() << "\t" << pwd << "\n";
        adminsList->addItem(QString("%1 %2 %3").arg(surname.trimmed(), name.trimmed(), patronymic.trimmed()));
    }
}

void AdminView::onRemoveAdmin() {
    if (!adminsList->currentItem()) return;
    // Удаляем по логину
    bool ok;
    QString login = QInputDialog::getText(this, "Удалить администратора", "Логин администратора:", QLineEdit::Normal, "", &ok);
    if (!ok || login.trimmed().isEmpty()) return;
    DataStore ds;
    ds.removeAdmin(login.trimmed());
    delete adminsList->takeItem(adminsList->currentRow());
}

void AdminView::onLoadSchedule() {
    DataStore ds;
    auto lines = ds.loadSchedule();
    scheduleEdit->clear();
    for (auto &l : lines) scheduleEdit->append(l);
}

void AdminView::onSaveSchedule() {
    QString all = scheduleEdit->toPlainText();
    QVector<QString> lines = all.split('\n').toVector();
    DataStore ds;
    if (ds.saveSchedule(lines)) QMessageBox::information(this, "OK", "Расписание сохранено.");
    else QMessageBox::warning(this, "Ошибка", "Не удалось сохранить расписание.");
}

void AdminView::onGenerateSchedule() {
    DataStore ds;
    ds.generateDefaultSchedule();
    onLoadSchedule(); // Обновляем отображение
    QMessageBox::information(this, "УСПЕШНО", "Базовое расписание сгенерировано и загружено.");
}

void AdminView::onViewGrades() {
    DataStore ds;
    auto grades = ds.loadGrades();
    QString txt;
    for (auto &g : grades) txt += g + "\n";
    QMessageBox::information(this, "Оценки", txt.isEmpty() ? "Нет оценок" : txt);
}

void AdminView::onClearGrades() {
    if (QMessageBox::question(this, "Подтвердите", "Удалить все оценки?") != QMessageBox::Yes) return;
    DataStore ds;
    ds.overwriteGrades({});
    QMessageBox::information(this, "OK", "Оценки удалены.");
}
