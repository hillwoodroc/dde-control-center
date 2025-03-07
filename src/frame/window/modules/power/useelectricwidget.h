// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "modules/power/powermodel.h"

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
class QSlider;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class TitledSliderItem;
class SwitchWidget;
class ComboxWidget;
}

namespace power {
class PowerModel;
class PowerWorker;
}
}

namespace DCC_NAMESPACE {
namespace power {

typedef QMap<int, QString> ActionList;

class UseElectricWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UseElectricWidget(dcc::power::PowerModel *model, QWidget *parent = nullptr, dcc::power::PowerWorker *work = nullptr);
    virtual ~UseElectricWidget();

    void setModel(const dcc::power::PowerModel *model);
    void setLidClose(bool state);

Q_SIGNALS:
    void requestSetScreenBlackDelayOnPower(const int delay) const;
    void requestSetSleepDelayOnPower(const int delay) const;
    void requestSetAutoLockScreenOnPower(const int delay) const;
    void requestSetSleepOnLidOnPowerClosed(const bool sleep) const;
    void requestSetLinePowerPressPowerBtnAction(const int reply) const;//按下电源
    void requestSetLinePowerLidClosedAction(const int reply) const;//合上盖子

public Q_SLOTS:
    void setScreenBlackDelayOnPower(const int delay);
    void setSleepDelayOnPower(const int delay);
    void setAutoLockScreenOnPower(const int delay);
    void setLockScreenAfter(const int delay);
    void setCloseLid(const dcc::power::PowerModel *model, int option);
    void setPowerBtn(const dcc::power::PowerModel *model, int option);
    void updatePowerButtonActionList();

private:
    QString delayToLiteralString(const int delay) const;
    void setComboxOption(dcc::widgets::ComboxWidget *combox, const ActionList options);

private:
    dcc::power::PowerModel *m_model;
    dcc::power::PowerWorker *m_work;
    QVBoxLayout *m_layout = nullptr;
    dcc::widgets::TitledSliderItem *m_monitorSleepOnPower = nullptr;
    dcc::widgets::TitledSliderItem *m_computerSleepOnPower = nullptr;
    dcc::widgets::TitledSliderItem *m_autoLockScreen = nullptr;

    dcc::widgets::ComboxWidget *m_cmbPowerBtn;    //电源按钮
    dcc::widgets::ComboxWidget *m_cmbCloseLid;    //盒盖
};

}// namespace datetime
}// namespace DCC_NAMESPACE
