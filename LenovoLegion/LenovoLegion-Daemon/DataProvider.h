// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "Core/ExceptionBuilder.h"
#include <QObject>



namespace LenovoLegionDaemon {


class DataProvider : public QObject
{
    Q_OBJECT
public:
    DEFINE_EXCEPTION(DataProvider)
public:

    enum ERROR_CODES : int {
        INVALID_DATA                        = -1,
        SERIALIZE_ERROR                     = -2
    };

public:

    DataProvider(QObject* parent,quint8  dataType);

    virtual ~DataProvider() = default;

    virtual QByteArray serializeAndGetData()                      const = 0;
    virtual QByteArray deserializeAndSetData(const QByteArray&)         = 0;

    virtual void init()  {};
    virtual void clean() {};

private:


public:

   /*
    * Data type identifier
    */
   const quint8  m_dataType;
};

}
