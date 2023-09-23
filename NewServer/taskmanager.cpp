#include "taskmanager.h"
#include <ctime>
#include <QVector>

TaskManager::TaskManager()
{

}

TaskManager *TaskManager::getInstance()
{
    if (!p_instance)
        p_instance = new TaskManager();
    return p_instance;
}

QString TaskManager::create_task(int taskNumber)
{
    std::srand(std::time(nullptr));
    int taskKey = 1+rand()%10000;
    QString taskText = "123";

    switch (taskNumber) {
    case 1:
        return create_task1(taskKey);
        break;
    default:
        return QString("error");
        break;
    }
}

bool TaskManager::check_task(int taskNumber, int taskKey, QString answer)
{
    switch (taskNumber) {
    case 1:
        return check_task1(taskKey,answer);
    default:
        return false;
    }
}

bool TaskManager::check_task1(int taskKey, QString answer)
{
    srand(taskKey);
    QVector<QVector<int>> matrix(3,QVector<int>(3,0));
    for(int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] = rand()%199-99;
        }
    }
    // Вычисление определителя по методу Лапласа
    int det = matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1])
          - matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0])
          + matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
    if (det == answer.toInt()) {
        return true;
    } else {
        return false;
    }
}

QString TaskManager::create_task1(int taskKey)
{
    srand(taskKey);
    QString data;
    for(int i = 0; i < 9; i++) {
        data.append(QString::number(rand()%199-99) + " ");
    }
    QString taskText = "Дана квадратная матрица размера 3х3. Найти её определитель.";
    return QString("1") + QString("$") + QString::number(taskKey) + QString("$") + taskText + QString("$") + data;
}
