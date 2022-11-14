//
// Created by yanjs on 2022/11/13.
//
#pragma once

#include <QtTest/QtTest>
#include <QObject>
#include "../statement/Text.h"

class Test : public QObject
{
Q_OBJECT
public:
    Test();

private slots:

    void testCase1();

    void testCase2();

    void testCase3();

    void testCase4();

//    void testCase5();


};
