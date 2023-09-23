#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

class DataBase
{
private:
    DataBase();
    DataBase(const DataBase& ) = delete;
    DataBase& operator = (DataBase &) = delete;
    static inline DataBase *p_instance = nullptr;
    ~DataBase();

    QSqlDatabase db;

    bool db_created();
    void create_db();

public:
    //Отправляет запрос в базу данных
    QVector<QMap<QString, QString>> db_request(QString request);

    //Очищает базу данных
    void db_clear();

    //Возвращает в консоль таблицу, полученную через db_request
    void printTable(const QVector<QMap<QString, QString>>& table);

public:
    static DataBase* getInstance();
};

#endif // DATABASE_H
