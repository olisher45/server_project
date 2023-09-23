#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QString>


class TaskManager
{
private:
    TaskManager();
    TaskManager(const TaskManager& ) = delete;
    TaskManager& operator = (TaskManager &) = delete;
    static inline TaskManager *p_instance = nullptr;
    ~TaskManager();
public:
    static TaskManager* getInstance();

    // Генерирует одно из пяти заданий
    QString create_task(int taskNumber);

    // Проверяет задание
    bool check_task(int taskNumber, int taskKey, QString answer);
private:

    bool check_task1(int taskKey, QString answer);

    bool check_task2(int taskKey, QString answer);

    bool check_task3(int taskKey, QString answer);

    bool check_task4(int taskKey, QString answer);

    bool check_task5(int taskKey, QString answer);

    QString create_task1(int taskKey = 0);

    QString create_task2(int taskKey = 0);

    QString create_task3(int taskKey = 0);

    QString create_task4(int taskKey = 0);

    QString create_task5(int taskKey = 0);
};

#endif // TASKMANAGER_H
