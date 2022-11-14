//
// Created by yanjs on 2022/11/13.
//

#include "Test.h"

Test::Test()
{

}

void Test::testCase1()
{
    QString expr = QString("3**2\n");
    Statement stmt(expr);
    QVERIFY(stmt.calculateExp() == 9);
}

void Test::testCase2()
{
    QString expr = "3+(5*(2+3)+7)";
    Statement stmt(expr);
    QVERIFY(stmt.calculateExp() == 35);
}

void Test::testCase3()
{
    QString expr = "3+(-1)+(-10)";
    Statement stmt(expr);
    QCOMPARE(stmt.calculateExp(), -8);

}

void Test::testCase4()
{
    Text text;
    auto &lexer = Text::lexer;
    std::string content = "100 REM Program to print the Fibonacci sequence.\n"
                          "110 LET max = 10000\n"
                          "120 LET n1 = 0\n"
                          "130 LET n2 = 1\n"
                          "140 IF n1 > max THEN 190\n"
                          "145 PRINT n1\n"
                          "150 LET n3 = n1 + n2\n"
                          "160 LET n1 = n2\n"
                          "170 LET n2 = n3\n"
                          "180 GOTO 140\n"
                          "190 END\n"
                          "QUIT\n"
                          "LIST";
    std::string str1 = "100 REM Program to print the Fibonacci sequence.\n";
    text.input(content);
    QVERIFY(1 == 1);
}

//void Test::testCase5()
//{
//
//}
QTEST_MAIN(Test)

//#include "test.moc"