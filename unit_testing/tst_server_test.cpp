#include <QtTest>

#include "../NewServer/server.h"

class server_test : public QObject
{
    Q_OBJECT

public:
    server_test();
    ~server_test();

private slots:
    void test_case1();
    void test_case2();
};

server_test::server_test()
{

}

server_test::~server_test()
{

}

void server_test::test_case1()
{
    QVERIFY(Server::getInstance()->parse("123") == QByteArray(QString("invalid_arguments").toUtf8()));
}


void server_test::test_case2()
{
    QVERIFY(Server::getInstance()->parse("12345") == QByteArray(QString("invalid_arguments").toUtf8()));
}


QTEST_APPLESS_MAIN(server_test)

#include "tst_server_test.moc"
