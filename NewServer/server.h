#ifndef SERVER_H
#define SERVER_H

#include <QByteArray>

class Server
{
private:
    Server();
    Server(const Server& );
    Server& operator = (Server &);
    static inline Server *p_instance = nullptr;

private:
    //Вызывается при неверных сообщениях от пользователя
    QByteArray wrong_user_message();

    //Авторизует подключение пользователя
    QByteArray auth(QString login, QString password);

    // Регистрирует пользователя (добавляет информацию о нём в базу данных)
    QByteArray reg(QString login, QString password, int userType, QString loginTeacher);

    //Проверяет, авторизировано ли данное устройство
    bool check_auth(int userKey);

    //Отдаёт задание на проверку и возвращает ответ
    QByteArray task_is_done(int userKey, int taskNumber, int taskKey, QString answer);

    //Сбрасывает подключения для зарегистрированных пользователей
    QByteArray reset_connections();

    //Генерирует задание, передаёт клиенту условие задания и его уникальный номер
    QByteArray get_task(int taskNumber);

    //Проверяет задание, генерируя условие задания по номеру задания и его уникальному ключу,
    bool check_task(int taskNumber, int taskKey, QString answer);

    //Удаляет подключения для пользователя с данными логином и паролем.
    QByteArray user_logout(QString login, QString password);

    //Удаляет группу заданного преподавателя.
    QByteArray del_group(int userKey);

    //Проверка на аккаунт преподавателя
    bool is_it_a_teacher(int userKey);

    //Если запрос идёт от преподавателя, вернуть список логинов его студентов
    QByteArray get_students_list(int userKey);

    //Если запрос идёт от преподавателя, возвращает статистику по заданию ученика
    QByteArray get_statistics(int userKey, QString studentLogin, int taskNumber);

public:
    static Server* getInstance();

    //Принимает сообщение пользователя и возвращает ответ подходящей функции
    QByteArray parse(QString userMessage);
};

#endif // SERVER_H
