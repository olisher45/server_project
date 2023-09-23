#include "database.h"

DataBase *DataBase::getInstance()
{
    if (!p_instance)
        p_instance = new DataBase();
    return p_instance;
}

bool DataBase::db_created()
{
    return this->db.tables() == QStringList{"Users", "Tasks", "UserGroups"};
}

void DataBase::create_db()
{
    QSqlQuery query(this->db);
    /* Создаёт две таблицы:
     *
     * Users
     * ------------------------------
     * login | password | type | key
     * ------------------------------
     *       |          |      |
     *       |          |      |
     *       |          |      |
     * ------------------------------
     *
     * type: 0 - студент, 1 - преподаватель
     *
     * Tasks
     * ----------------------------------------------------
     * task_id | login | task_number | answer | is_correct
     * ----------------------------------------------------
     *         |       |             |        |
     *         |       |             |        |
     *         |       |             |        |
     * ----------------------------------------------------
     * Tasks.login <-> Users.login
     *
     * UserGroups
     * ------------------------------
     * teacher_login | student_login
     * ------------------------------
     *               |
     *               |
     *               |
     * ------------------------------
     *
     */
    query.exec("CREATE TABLE Users("
               "login VARCHAR(64) PRIMARY KEY, "
               "password VARCHAR(64) NOT NULL, "
               "type INTEGER, "
               "key INTEGER DEFAULT NULL "
               ")");
    query.exec("CREATE TABLE Tasks("
               "task_id INTEGER NOT NULL, "
               "login VARCHAR(64) NOT NULL, "
               "task_number INTEGER NOT NULL, "
               "answer TEXT, "
               "is_correct BOOLEAN NOT NULL"
               ")");
    query.exec("CREATE TABLE UserGroups("
               "teacher_login VARCHAR(64) NOT NULL, "
               "student_login VARCHAR(64) NOT NULL"
               ")");
}

DataBase::DataBase()
{
    // Инициализируем базу данных из файла database.sqlite
    this->db = QSqlDatabase::addDatabase("QSQLITE");
    this->db.setDatabaseName("database.sqlite");

    // Выводим ошибки, если они есть, в консоль
    if (!(this->db.open()))
        qDebug()<<db.lastError().text();

    // Если нет таблиц Users, Tasks, UserGroups, то создаем их
    if (!this->db_created())
        this->create_db();
}

DataBase::~DataBase()
{
    // Закрываем базу данных
    db.close();
}

QVector<QMap<QString, QString>> DataBase::db_request(QString request)
{
    QSqlQuery query(this->db);
    query.exec(request);            // исполняем запрос

    QVector<QMap<QString, QString>> result; // таблица с запрашиваемыми данными

    if (query.lastError().isValid()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
            return result;
    }

    QSqlRecord record = query.record();
    QVector<QString> columns;       // хранит столбцы возвращаемой таблицы

    for (int i = 0; i < record.count(); i++) {
        QString columnName = record.fieldName(i);
        columns.append(columnName);
    }

    while (query.next()) {
        QMap<QString, QString> row;
        for (int i = 0; i < columns.count(); i++) {
            // строки таблицы заполняются, каждая строка - хэшмап, ключ - название столбца, значение - то, что в ячейке
            QString columnName = query.record().fieldName(i);
            QString value = query.value(i).toString();
            row.insert(columnName, value);
        }
        result.append(row);
    }
    return result;
}

void DataBase::printTable(const QList<QMap<QString, QString>>& table)
{
    const int columnWidth = 15;

    // выводим горизонтальную линию таблицы
    qDebug().noquote() << QString("-").repeated(columnWidth * table.first().size() + table.first().size() + 1);

    // выводим заголовки столбцов
    QString header;
    for (auto& column : table.first().keys()) {
        header += QString("| %1 ").arg(column.leftJustified(columnWidth, ' '), Qt::AlignHCenter);
    }
    qDebug().noquote() << header << "|";

    // выводим горизонтальную линию таблицы
    qDebug().noquote() << QString("-").repeated(columnWidth * table.first().size() + table.first().size() + 1);

    // выводим строки таблицы
    for (auto& row : table) {
        QString rowStr;
        for (auto& value : row.values()) {
            rowStr += QString("| %1 ").arg(value.leftJustified(columnWidth, ' '), Qt::AlignHCenter);
        }
        qDebug().noquote() << rowStr << "|";
    }

    // выводим горизонтальную линию таблицы
    qDebug().noquote() << QString("-").repeated(columnWidth * table.first().size() + table.first().size() + 1);
}

void DataBase::db_clear()
{
    // очищаются таблицы Users и Tasks, но не удаляются
    QSqlQuery query(this->db);
    query.exec("DELETE FROM TABLE Users");
    query.exec("DELETE FROM TABLE Tasks");
    query.exec("DELETE FROM TABLE UserGroups");
}
