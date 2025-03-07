// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <QWidget>
#include <DLineEdit>

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace bluetooth {
class TitleEdit : public QWidget
{
    Q_OBJECT
public:
    explicit TitleEdit(QWidget *parent = nullptr);

Q_SIGNALS:
    void requestSetBluetoothName(const QString &name);

public Q_SLOTS:
    void setName();
    void setEdit();
    void setTitle(const QString &title);

private:
    QLabel *m_name;
    DLineEdit *m_lineEdit;
};
}
}
