//
// Created by yanjs on 2022/11/13.
//

#include "Test.h"

Test::Test()
{

}

void Test::testCase1()
{
    QString expr = QString("3**2**3\n");
    Statement stmt(expr);
    QCOMPARE(calculateExp(stmt.splitLine), pow(3, pow(2, 3)));
}

void Test::testCase2()
{
    QString expr = "3+(5*(2+3)+7)";
    Statement stmt(expr);
    QCOMPARE(calculateExp(stmt.splitLine), 35);
}

void Test::testCase3()
{
    QString expr = "3+(-1)+(-10)";
    Statement stmt(expr);
    QCOMPARE(calculateExp(stmt.splitLine), -8);

}

void Test::testCase4()
{
    QString filePath = "../basicSrc/case2";
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    Text text;
    auto allText = file.readAll().toStdString();
    text.input(allText);
    text.executeProgram();
    QCOMPARE(1, 1);
}

//void Test::testCase5()
//{
//
//}
QTEST_MAIN(Test)

//#include "test.moc"