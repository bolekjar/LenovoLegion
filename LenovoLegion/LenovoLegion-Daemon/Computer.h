// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once


#include <Core/ExceptionBuilder.h>

#include <QObject>




namespace LenovoLegionDaemon {


class SysFsDriverManager;
class Computer : public QObject
{
    Q_OBJECT
public:

    DEFINE_EXCEPTION(Computer)

    enum ERROR_CODES : int {
        INITIALIZATION_ERROR            = 1
    };

public:

    Computer(SysFsDriverManager& sysFsDriverManager,QObject* parent);
    ~Computer() override = default;

    void initComputer();

private:

    SysFsDriverManager& m_sysFsDriverManager;

public:

    static constexpr quint32 LOCKING_VALUE_IN_MW = 999000;
};



}
