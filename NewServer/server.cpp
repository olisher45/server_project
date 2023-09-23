#include "server.h"
#include "database.h"
#include "taskmanager.h"
#include <QString>
#include <QDebug>
#include <ctime>

Server::Server()
{

}

QByteArray Server::wrong_user_message() {
    return QByteArray(QString("invalid_arguments").toUtf8());
}

QByteArray Server::auth(QString login, QString password)
{
    QVector<QMap<QString, QString>> request = DataBase::getInstance()->db_request(
        QString("select * from Users "
                "where login = '%1' and password = '%2'").arg(login, password));
    //qDebug() << login << " " << password << " " << request.size();
    if (request.size() == 1){
        srand(time(0));
        int key = 1 + rand()%1000;
        DataBase::getInstance()->db_request(
            QString("update Users "
                    "set key = %1 "
                    "where login = '%2' and password = '%3'").arg(QString::number(key), login, password));
        return QByteArray(QString(QString("auth$") + QString::number(key)).toUtf8());
    } else {
        return QByteArray(QString(QString("auth$") + QString::number(0)).toUtf8());
    }
}

QByteArray Server::reg(QString login, QString password, int userType, QString loginTeacher)
{
    bool regSuccess;
    QVector<QMap<QString, QString>> answer = DataBase::getInstance()->db_request(
        QString("select * from Users "
                "where login = '%1'").arg(login));
    // Если данный логин найден в базе данных, значит он занят.
    if (answer.size() != 0) {
        regSuccess = false;
    // Добавляем нового пользователя в базу данных.
    } else {
        DataBase::getInstance()->db_request(QString("insert into Users(type, login, password) "
                                                    "values (%1, '%2', '%3')").arg(QString::number(userType), login, password));
        QVector<QMap<QString, QString>> current_user =
            DataBase::getInstance()->db_request(QString("select * from Users "
                                                        "where login = '%1' and password = '%2'").arg(login, password));
        if (userType == 0) {
            DataBase::getInstance()->db_request(QString("insert into UserGroups "
                                                        "values('%1', '%2')").arg(loginTeacher, login));
        }
        regSuccess = true;
    }
    return QByteArray((QString("reg$")+QString::number(regSuccess)).toUtf8());
}

bool Server::check_auth(int userKey)
{
    bool checkAuthSucces;
    // Ищем авторизированных пользователей с данным подключением.
    QVector<QMap<QString, QString>> answer = DataBase::getInstance()->db_request(
        QString("select * from Users "
                "where key = %1").arg(QString::number(userKey)));
    // Если таких пользователей нет, то авторизация не пройдена.
    if (answer.size() == 0) {
        checkAuthSucces = false;
    // Если таких пользователей (строк в базе данных) больше одного, то что-то пошло не так.
    } else if (answer.size() > 1) {
        qDebug() << "Ошибка. Пользователей с данным подключением больше одного." << '\n';
        checkAuthSucces = false;
    // Если такой пользователь один, то авторизация успешна.
    } else {
        checkAuthSucces = true;
    }
    return checkAuthSucces;
}

QByteArray Server::task_is_done(int userKey, int taskNumber, int taskKey, QString answer)
{
    bool isAnswerCorrect = check_task(taskNumber, taskKey, answer);
    QVector<QMap<QString, QString>> answer_temp = DataBase::getInstance()->db_request(
        QString("select login from Users where key = %1").arg(QString::number(userKey)));
    if (answer_temp.size() == 0) {
        qDebug() << "Ошибка. Пользователь с данным идентификатором не найден.";
        return QByteArray();
    }
    QString userLogin = answer_temp[0]["login"];
    DataBase::getInstance()->db_request(QString("insert into Tasks(login, task_id, task_number, is_correct, answer) "
                                                "values('%1', %2, %3, %4, '%5')").arg
                                        (userLogin, QString::number(taskKey),
                                         QString::number(taskNumber), QString::number(isAnswerCorrect), answer));
    return QByteArray((QString("task_is_done$")+QString::number(isAnswerCorrect)).toUtf8());
}

QByteArray Server::reset_connections()
{
    // Удаляет дескрипторы соединений для всех пользователей.
    DataBase::getInstance()->db_request(QString("update Users "
                                                "set key = null"));
    return QByteArray(QString("reset_connections").toUtf8());
}

QByteArray Server::user_logout(QString login, QString password)
{
    bool checkLogoutSuccess;
    QVector<QMap<QString, QString>> answer = DataBase::getInstance()->db_request(
        QString("select * "
                "from Users "
                "where login = '%1' and password = '%2'").arg(login, password));
    qDebug() << login << " " << password << '\n';
    if (answer.size() == 0) {
        qDebug() << "Пользователь не найден." << '\n';
        checkLogoutSuccess = false;
    } else {
        DataBase::getInstance()->db_request(
            QString("update Users "
                    "set key = null "
                    "where login = '%1' and password = '%2'").arg(login, password));
        checkLogoutSuccess = true;
    }
    return QByteArray(QString(QString("user_logout$") + QString::number(checkLogoutSuccess)).toUtf8());
}

QByteArray Server::del_group(int userKey)
{
    if (is_it_a_teacher(userKey)) {
        QString teacherLogin = DataBase::getInstance()->db_request(QString("select login "
                                                                          "from Users where key = %1").arg(userKey))[0]["login"];
        DataBase::getInstance()->db_request(QString("delete from UserGroups "
                                                    "where teacher_login = '%1'").arg(teacherLogin));
        return QByteArray(QString(QString("del_group$") + QString(teacherLogin)).toUtf8());
    } else {
        return QByteArray(QString(QString("del_group$") + QString("")).toUtf8());
    }

}

bool Server::is_it_a_teacher(int userKey)
{
    bool checkTeacher;
    QVector<QMap<QString, QString>> current_user =
        DataBase::getInstance()->db_request(
            QString("select type from Users "
                    "where key = %1").arg(QString::number(userKey)));
    if (current_user.size() == 0) {
        qDebug() << "Данный пользователь не найден." << '\n';
        checkTeacher = false;
    } else if (current_user[0]["type"] == "0") {
        checkTeacher = false;
    } else {
        checkTeacher = true;
    }
    return checkTeacher;
}

QByteArray Server::get_students_list(int userKey)
{
    bool checkTeacher = is_it_a_teacher(userKey);
    QString answer;
    if (checkTeacher == false) {
        qDebug() << "Данный пользователь не является преподавателем." << '\n';
        return QByteArray(answer.toUtf8());
    } else {
        QStringList studentList;
        QVector<QMap<QString, QString>> studentList_temp =
            DataBase::getInstance()->db_request(
                QString("select login from "
                        "Users as s1 "
                        "left join "
                        "UserGroups as s2 "
                        "on s1.login = s2.student_login "
                        "where s2.teacher_login = "
                        "(select login from Users where key = %1)").arg(QString::number(userKey)));
        for (int i = 0; i < studentList_temp.size();i++) {
            studentList.push_back(studentList_temp[i]["login"]);
        }
        for (int i = 0; i < studentList.size(); i++) {
            answer += studentList[i];
            if (i != studentList.size()-1) {
                answer += "$";
            }
        }
        return QByteArray((QString("get_student_list$") + answer).toUtf8());
    }
}

QByteArray Server::get_task(int taskNumber)
{
    return QByteArray(QString("get_task$").toUtf8()+TaskManager::getInstance()->create_task(taskNumber).toUtf8());
}

bool Server::check_task(int taskNumber, int taskKey, QString answer)
{
    return TaskManager::getInstance()->check_task(taskNumber, taskKey, answer);
}


QByteArray Server::get_statistics(int userKey, QString studentLogin, int taskNumber)
{
    bool checkAccess = is_it_a_teacher(userKey);
    QString answer;
    if (checkAccess == false) {
        qDebug() << "Доступ запрещён." << '\n';
        return QByteArray("");
    } else {
        QVector<QMap<QString, QString>> studentTasks =
            DataBase::getInstance()->db_request(
                QString("select s1.task_id, s1.is_correct from "
                        "Tasks as s1 "
                        "left join "
                        "Users as s2 "
                        "on s1.login = s2.login "
                        "where s2.login = '%1' and "
                        "s1.task_number = %2").arg(studentLogin, QString::number(taskNumber)));
        int correctAnswers = 0;
        int wrongAnswers = 0;
        QStringList wrongAnswerTasks;
        for (int i = 0; i < studentTasks.size(); i++) {
            if (studentTasks[i]["is_correct"] == "1") {
                correctAnswers++;
            } else {
                wrongAnswers++;
            }
        }
        for (int i = 0; i < studentTasks.size(); i++) {
            if (studentTasks[i]["is_correct"] == "0") {
                wrongAnswerTasks.push_back(studentTasks[i]["task_id"]);
            }
        }
        answer = QString::number(correctAnswers) + "$" + QString::number(wrongAnswers);
        for (int i = 0; i < wrongAnswerTasks.size(); i++) {
            answer += "$";
            answer += wrongAnswerTasks[i];
        }
    }
    return QByteArray(answer.toUtf8());
}

/*
 * Формат команды
 *
 * [Дескриптор подключения] [Команда] [arg1] [arg2]...
 *
 *
 */

QByteArray Server::parse(QString userMessage) {
    userMessage.chop(2);
    QStringList data = userMessage.split(' ');
    for (int i = 0; i < data.size(); i++) {
        qDebug() << data[i] << " ";
    }
    qDebug() << '\n';
    if (data[0] == "reg") {
        return reg(data[1], data[2], data[3].toInt(), data[4]);
    } else if (data[0] == "auth") {
        return auth(data[1], data[2]);
    } else if (data[0] ==  "check_auth") {
        return QByteArray(QString("check auth$" + QString::number(check_auth(data[1].toInt()))).toUtf8());
    } else if (data[0] == "reset_connections") {
        return reset_connections();
    } else if (data[0] ==  "task") {
        return task_is_done(data[1].toInt(), data[2].toInt(), data[3].toInt(), data[4]);

    } else if (data[0] ==  "user_logout") {
        return user_logout(data[1], data[2]);
    } else if (data[0] == "get_task") {
        return get_task(data[1].toInt());
    } else if (data[0] == "del_group") {
        return del_group(data[1].toInt());
    } else if (data[0] == "is_it_a_teacher") {
        return QByteArray(QString("is it a teacher$" + QString::number(is_it_a_teacher(data[1].toInt()))).toUtf8());
    } else if (data[0] == "get_students_list") {
        return get_students_list(data[1].toInt());
    } else if (data[0] == "get_statistics") {
        return get_statistics(data[1].toInt(),data[2],data[3].toInt());
    } else {
        return wrong_user_message();
    }
}

Server *Server::getInstance()
{
    if (!p_instance)
        p_instance = new Server();
    return p_instance;
}
