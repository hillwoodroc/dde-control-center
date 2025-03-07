// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef MONITORCONTROLWIDGET_H
#define MONITORCONTROLWIDGET_H

#include <QFrame>
#include <QLabel>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
QT_END_NAMESPACE

namespace dcc {

namespace display {

class Monitor;
class DisplayModel;
class MonitorsGround;
class MonitorControlWidget : public QFrame
{
    Q_OBJECT

public:
    explicit MonitorControlWidget(int activateHeight = 240, QWidget *parent = nullptr);

    void setModel(DisplayModel *model, Monitor *moni = nullptr);
    void setScreensMerged(const int mode);
    void setMergeMode(bool val);

Q_SIGNALS:
    void requestRecognize() const;
    void requestGatherWindows(const QPoint cursor) const;
    void requestSetMonitorPosition(QHash<Monitor *, QPair<int, int>> monitorPosition) const;
    void requestShowsecondaryScreen() const;
    void requestMonitorPress(Monitor *mon);
    void requestMonitorRelease(Monitor *mon);

public Q_SLOTS:
    void onGatherEnabled(const bool enable);
    void onSetEffectiveReminderVisible(bool visible, int nEffectiveTime);

private:
    MonitorsGround *m_screensGround;
    QPushButton *m_recognize;
    QPushButton *m_gather;
    //多屏设置生效提示
    QLabel *m_effectiveReminder;
};

} // namespace display

} // namespace dcc

#endif // MONITORCONTROLWIDGET_H
