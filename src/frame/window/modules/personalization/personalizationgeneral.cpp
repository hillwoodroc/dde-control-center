// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "personalizationgeneral.h"
#include "perssonalizationthemewidget.h"
#include "roundcolorwidget.h"
#include "window/utils.h"
#include "widgets/dccslider.h"
#include "widgets/titledslideritem.h"
#include "widgets/settingsitem.h"
#include "widgets/titlelabel.h"
#include "widgets/comboxwidget.h"
#include "window/gsettingwatcher.h"
#include "window/dconfigwatcher.h"

#include <DStyle>
#include <DSwitchButton>
#include <DPalette>
#include <DFontSizeManager>
#include <DApplicationHelper>
#include <DPlatformTheme>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColor>
#include <QRect>
#include <QPalette>
#include <QSettings>
#include <QComboBox>
#include <QGraphicsDropShadowEffect>
#include <QPainterPath>
#include <QScrollArea>
#include <QResizeEvent>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

const QList<QString> ACTIVE_COLORS = {
    "#D8316C",
    "#FF5D00",
    "#F8CB00",
    "#23C400",
    "#00A48A",
    "#0081FF",
    "#3C02FF",
    "#8C00D4",
    "#4D4D4D"
};

struct ColorStru {
    int r = 0;
    int g = 0;
    int b = 0;
    bool operator==(const ColorStru b) const
    {
        return (b.r == this->r && b.g == this->g && b.b == this->b);
    }
};

const QList<ColorStru> ACTIVE_COLORST = {
    {216, 49, 108},
    {255, 93, 0},
    {248, 203, 0},
    {35, 196, 0},
    {0, 164, 138},
    {0, 129, 255},
    {60, 2, 215},
    {140, 0, 212},
    {77, 77, 77},
};

const QList<ColorStru> Dark_ACTIVE_COLORST = {
    {179, 33, 87},
    {204, 71, 0},
    {198, 162, 0},
    {28, 156, 0},
    {0, 130, 108},
    {0, 105, 204},
    {44, 0, 206},
    {109, 0, 168},
    {61, 61, 61},
};

PersonalizationGeneral::PersonalizationGeneral(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout())
    , m_wmSwitch(nullptr)
    , m_transparentSlider(nullptr)
    , m_cmbMiniEffect(new ComboxWidget)
    , m_windowMovedSwitch(nullptr)
    , m_windowMovedLabel(nullptr)
    , m_displayData("")
    , m_winRoundSlider(nullptr)
    , m_Themes(new PerssonalizationThemeWidget())
    , m_bgWidget(new RingColorWidget)
    , m_switchWidget(new QWidget)
    , m_isWayland(qEnvironmentVariable("XDG_SESSION_TYPE").contains("wayland"))
    , m_movedWinSwitchItem(nullptr)
{
    m_centralLayout->setMargin(0);
    m_centralLayout->setContentsMargins(ThirdPageContentsMargins);
    //appearance
    //~ contents_path /personalization/General
    //~ child_page General
    TitleLabel *themeL = new TitleLabel(tr("Theme"));
    m_centralLayout->addWidget(themeL);
    //pictures and types
    m_Themes->setMainLayout(new QHBoxLayout(), true);
    m_centralLayout->addWidget(m_Themes);

    m_themeType = DGuiApplicationHelper::instance()->themeType();

    //active colors
    //~ contents_path /personalization/General
    //~ child_page General
    TitleLabel *activeL = new TitleLabel(tr("Accent Color"));
    m_centralLayout->addWidget(activeL);

    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->setAlignment(Qt::AlignLeft);
    m_bgWidget->setLayout(colorLayout);
    m_bgWidget->setFixedHeight(40);
    colorLayout->setContentsMargins(10, 0, 10, 0);
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, this);
    int totalSpace = borderWidth + borderSpacing + RoundColorWidget::EXTRA; //2px extra space to avoid line cutted off

    for (ColorStru aColor : (m_themeType == DGuiApplicationHelper::ColorType::LightType ? ACTIVE_COLORST : Dark_ACTIVE_COLORST)) {
        QColor color;
        color.setRgb(aColor.r, aColor.g, aColor.b);
        RoundColorWidget *colorItem = new RoundColorWidget(color, this);
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
        effect->setBlurRadius(17);        // 阴影圆角的大小

        color.setAlpha(68);
        effect->setColor(color);      //阴影的颜色
        effect->setOffset(0,5);
        colorItem->setGraphicsEffect(effect);

        if (m_themeType == DGuiApplicationHelper::ColorType::LightType) {
            colorItem->setAccessibleName(ACTIVE_COLORS[ACTIVE_COLORST.indexOf(aColor)]);
        } else {
            colorItem->setAccessibleName(ACTIVE_COLORS[Dark_ACTIVE_COLORST.indexOf(aColor)]);
        }

        DPalette pa = colorItem->palette();
        pa.setBrush(DPalette::Base, color);
        colorItem->setPalette(pa);
        colorItem->setFixedSize(20 + 2 * totalSpace, 40);
        colorLayout->addWidget(colorItem);
        connect(colorItem, &RoundColorWidget::clicked, this, &PersonalizationGeneral::onActiveColorClicked);
        m_activeColorsList.append(colorItem);
    }

    m_centralLayout->addWidget(m_bgWidget);

    m_bSystemIsServer = IsServerSystem;

    if (!m_bSystemIsServer) {
        //sw switch
        QVBoxLayout *winEffectVLayout = new QVBoxLayout();
        winEffectVLayout->addSpacing(20);
        m_switchWidget->setLayout(winEffectVLayout);
        m_switchWidget->setAccessibleName("switchWidget");

        m_wmSwitch = new DSwitchButton();
        m_wmSwitch->setAccessibleName("switchbutton");
        QHBoxLayout *swswitchLayout = new QHBoxLayout();
        SettingsItem *switem = new dcc::widgets::SettingsItem;
        switem->setFixedHeight(SwitchWidgetHeight);
        switem->addBackground();
        switem->setLayout(swswitchLayout);
        if (m_isWayland) {
            switem->setVisible(false);
        }

        //~ contents_path /personalization/General
        //~ child_page General
        swswitchLayout->addWidget(new QLabel(tr("Window Effect")));
        swswitchLayout->addStretch();
        swswitchLayout->addWidget(m_wmSwitch);
        swswitchLayout->setContentsMargins(10, 0, 10, 0);
        winEffectVLayout->addWidget(switem);
        winEffectVLayout->setContentsMargins(0, 0, 0, 0);
        winEffectVLayout->addSpacing(10);

        //~ contents_path /personalization/General
        //~ child_page General
        m_transparentSlider = new dcc::widgets::TitledSliderItem(tr("Transparency"));
        //transparancy switch
        m_transparentSlider->addBackground();
        m_transparentSlider->slider()->setOrientation(Qt::Horizontal);
        m_transparentSlider->setObjectName("Transparency");

        //设计效果图变更：增加左右图标
        m_transparentSlider->slider()->setLeftIcon(QIcon::fromTheme("dcc_transparency_low"));
        m_transparentSlider->slider()->setRightIcon(QIcon::fromTheme("dcc_transparency_high"));
        m_transparentSlider->slider()->setIconSize(QSize(24, 24));
        dcc::widgets::DCCSlider *slider = m_transparentSlider->slider();
        slider->setAccessibleName("transparency");
        slider->setRange(0, 100);
        slider->setType(dcc::widgets::DCCSlider::Vernier);
        slider->setTickPosition(QSlider::TicksBelow);
        slider->setTickInterval(1);
        slider->setPageStep(1);
        winEffectVLayout->addWidget(m_transparentSlider);
        winEffectVLayout->addSpacing(10);

        //~ contents_path /personalization/General
        //~ child_page General
        m_cmbMiniEffect->setTitle(tr("Window Minimize Effect"));
        m_cmbMiniEffect->addBackground();
        QStringList options;
        options << tr("Scale") << tr("Magic Lamp");
        m_cmbMiniEffect->setComboxOption(options);
        winEffectVLayout->addWidget(m_cmbMiniEffect);

        //窗口移动时启用透明特效
        winEffectVLayout->addSpacing(10);
        m_windowMovedSwitch = new DSwitchButton();
        QHBoxLayout *movedWinSwitchLayout = new QHBoxLayout();
        m_movedWinSwitchItem = new dcc::widgets::SettingsItem;
        m_movedWinSwitchItem->setFixedHeight(MovedWindowWidgetHeight);
        m_movedWinSwitchItem->addBackground();
        m_movedWinSwitchItem->setLayout(movedWinSwitchLayout);

        //用于记录原始内容长度，解决内容过长显示不全的问题
        //~ contents_path /personalization/General
        //~ child_page General
        m_displayData = tr("Show transparency effects when a window is moved");
        m_windowMovedLabel = new QLabel(m_displayData);
        m_windowMovedLabel->setText(QFontMetrics(m_windowMovedLabel->font()).elidedText(m_displayData, Qt::ElideRight, 280));//显示省略号的字符串

        movedWinSwitchLayout->addWidget(m_windowMovedLabel);
        movedWinSwitchLayout->addStretch();
        movedWinSwitchLayout->addWidget(m_windowMovedSwitch);
        movedWinSwitchLayout->setContentsMargins(10, 0, 10, 0);
        winEffectVLayout->addWidget(m_movedWinSwitchItem);
        winEffectVLayout->setContentsMargins(0, 0, 0, 0);
        DConfigWatcher::instance()->bind(DConfigWatcher::personalization, "effectMovewindowTranslucency", m_movedWinSwitchItem);
        m_movedWinSwitchItem->setVisible(DConfigWatcher::instance()->getStatus(DConfigWatcher::personalization, "effectMovewindowTranslucency") != "Hidden");

        connect(m_transparentSlider->slider(), &dcc::widgets::DCCSlider::valueChanged, this,
                &PersonalizationGeneral::requestSetOpacity);
        connect(m_transparentSlider->slider(), &dcc::widgets::DCCSlider::sliderMoved, this,
                &PersonalizationGeneral::requestSetOpacity);
        connect(m_cmbMiniEffect, &dcc::widgets::ComboxWidget::onIndexChanged, this,
                &PersonalizationGeneral::requestSetMiniEffect);

        connect(m_wmSwitch, &DTK_WIDGET_NAMESPACE::DSwitchButton::clicked, this, [this](bool checked) {
                if (!m_model) {
                    return;
                }
                qDebug() << "DSwitchButton::clicked:" << checked << ",m_model->is3DWm():" << m_model->is3DWm();
                m_wmSwitch->setChecked(m_model->is3DWm());
                Q_EMIT requestWindowSwitchWM(checked);
                Q_EMIT requestSetMiniEffect(m_cmbMiniEffect->comboBox()->currentIndex());
        });

        connect(m_windowMovedSwitch,  &DTK_WIDGET_NAMESPACE::DSwitchButton::clicked, this, [this](bool checked) {
            if (!m_model) {
                return;
            }
            qDebug() << "DSwitchButton::clicked:" << checked << ",m_model->isMoveWindow():" << m_model->isMoveWindow();
            m_windowMovedSwitch->setChecked(m_model->isMoveWindow());
            Q_EMIT requestMovedWindowSwitchWM(checked);
        });

        connect(this, &PersonalizationGeneral::windowMovedVisibleChanged, this, [this](bool value) {
            if (!m_model) {
                return;
            }
            if (m_movedWinSwitchItem) {
                m_movedWinSwitchItem->setVisible(m_model->is3DWm() && value && DConfigWatcher::instance()->getStatus(DConfigWatcher::personalization, "effectMovewindowTranslucency")  != "Hidden");
            }
        });

        winEffectVLayout->addSpacing(10);
        m_winRoundSlider = new dcc::widgets::TitledSliderItem(tr("Rounded Corner"));
        m_winRoundSlider->addBackground();
        m_winRoundSlider->slider()->setOrientation(Qt::Horizontal);
        m_winRoundSlider->setObjectName("m_winRoundSlider");

        dcc::widgets::DCCSlider *sliderRound = m_winRoundSlider->slider();
        QStringList list;
        list<<tr("Small")<<tr("Medium")<<tr("Large");
        sliderRound->setAnnotations(list);
        sliderRound->setType(dcc::widgets::DCCSlider::Vernier);
        sliderRound->setTickPosition(QSlider::TicksBelow);
        sliderRound->setRange(0, 2);
        sliderRound->setTickInterval(1);
        sliderRound->setPageStep(1);
        winEffectVLayout->addWidget(m_winRoundSlider);
        winEffectVLayout->addStretch(20);

        connect(m_winRoundSlider->slider(), &dcc::widgets::DCCSlider::valueChanged, this, [=](int value){
            int val = value;
            if (value == 0) {
                val = 0;
            } else if (value == 1) {
                val = 8;
            } else if (value == 2) {
                val = 18;
            }

            Q_EMIT windowRadiusChanged(val);
        });
        update();
    }

    m_centralLayout->addWidget(m_switchWidget);
    m_centralLayout->addStretch(20);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setAccessibleName("PersonalizationGeneral_scrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->setBackgroundRole(QPalette::Base);

    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    setLayout(mainContentLayout);
    mainContentLayout->addWidget(scrollArea);

    auto tw = new QWidget();
    tw->setAccessibleName("centralLayoutWidget");
    tw->setLayout(m_centralLayout);
    m_centralLayout->setContentsMargins(0, 0, 0, 0);
    scrollArea->setWidget(tw);
}

PersonalizationGeneral::~PersonalizationGeneral()
{
    GSettingWatcher::instance()->erase("perssonalGeneralThemes", m_Themes);
    GSettingWatcher::instance()->erase("perssonalGeneralEffects", m_switchWidget);
    DConfigWatcher::instance()->erase(DConfigWatcher::personalization, "effectMovewindowTranslucency", m_movedWinSwitchItem);
}

void PersonalizationGeneral::setModel(dcc::personalization::PersonalizationModel *model)
{
    m_model = model;
    m_Themes->setModel(model->getWindowModel());
    GSettingWatcher::instance()->bind("perssonalGeneralThemes", m_Themes);

    if (!m_bSystemIsServer) {
        connect(model, &dcc::personalization::PersonalizationModel::wmChanged, this,
                [this](bool checked) {
            m_wmSwitch->blockSignals(true);
            updateWMSwitcher(checked);
            m_wmSwitch->blockSignals(false);
        });

        connect(model, &dcc::personalization::PersonalizationModel::moveWindowChanged, this,
                [this](bool checked) {
            m_windowMovedSwitch->blockSignals(true);
            m_windowMovedSwitch->setChecked(checked);
            m_windowMovedSwitch->blockSignals(false);
        });
        if (m_windowMovedSwitch) {
            m_windowMovedSwitch->setChecked(m_model->isMoveWindow());
        }

        updateWMSwitcher(model->is3DWm());
        connect(model, &dcc::personalization::PersonalizationModel::onOpacityChanged, this,
                &PersonalizationGeneral::onOpacityChanged);

        onOpacityChanged(model->opacity());
        connect(model, &dcc::personalization::PersonalizationModel::onMiniEffectChanged, this,
                &PersonalizationGeneral::onMiniEffectChanged);
        onMiniEffectChanged(model->miniEffect());
    }

    connect(model, &dcc::personalization::PersonalizationModel::onActiveColorChanged, this,
            &PersonalizationGeneral::onActiveColorChanged);
    onActiveColorChanged(model->getActiveColor());

    connect(m_model, &dcc::personalization::PersonalizationModel::onCompositingAllowSwitch, this,
            &PersonalizationGeneral::onCompositingAllowSwitchChanged);
    onCompositingAllowSwitchChanged(m_model->getAllowSwitch());

    connect(m_model, &dcc::personalization::PersonalizationModel::onWindowRadiusChanged, this,
            &PersonalizationGeneral::onWindowRadiusChanged);
    onWindowRadiusChanged(m_model->windowRadius());

}

void PersonalizationGeneral::updateThemeColors(DGuiApplicationHelper::ColorType type)
{
    int count = 0;
    for (auto item : m_activeColorsList) {
        ColorStru acolor = type == DGuiApplicationHelper::ColorType::LightType ? ACTIVE_COLORST[count] : Dark_ACTIVE_COLORST[count];
        count++;
        QColor color;
        color.setRgb(acolor.r, acolor.g, acolor.b);
        item->setColor(color);
        DPalette pa = item->palette();
        pa.setBrush(DPalette::Base, color);
        item->setPalette(pa);
        if (dynamic_cast<QGraphicsDropShadowEffect *>(item->graphicsEffect())) {
            color.setAlpha(68);
            dynamic_cast<QGraphicsDropShadowEffect *>(item->graphicsEffect())->setColor(color);
        }
    }
}

void PersonalizationGeneral::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (m_themeType != DGuiApplicationHelper::instance()->themeType()) {
        m_themeType = DGuiApplicationHelper::instance()->themeType();
        updateThemeColors(m_themeType);
    }
}

void PersonalizationGeneral::resizeEvent(QResizeEvent *event)
{
    m_windowMovedLabel->setText(QFontMetrics(m_windowMovedLabel->font()).elidedText(m_displayData, Qt::ElideRight, event->size().width() >= 430 ? 500 : 280));
    QWidget::resizeEvent(event);
}

void PersonalizationGeneral::updateActiveColors(RoundColorWidget *selectedWidget)
{
    for (RoundColorWidget *item : m_activeColorsList) {
        if (item == selectedWidget) {
            m_bgWidget->setSelectedItem(item);
            item->setSelected(true);
        } else {
            item->setSelected(false);
        }
    }
}

void PersonalizationGeneral::updateWMSwitcher(bool checked)
{
    if (m_wmSwitch) {
        m_wmSwitch->setChecked(m_model->is3DWm());
    }

    if (m_transparentSlider) {
        m_transparentSlider->setVisible(checked || m_isWayland);
        m_cmbMiniEffect->setVisible(checked || m_isWayland);
    }

    if (m_winRoundSlider) {
        m_winRoundSlider->setVisible(checked || m_isWayland);
    }

    if (m_windowMovedSwitch) {
        m_windowMovedSwitch->setChecked(m_model->isMoveWindow());
    }

    Q_EMIT windowMovedVisibleChanged(checked);
}

void PersonalizationGeneral::onCompositingAllowSwitchChanged(bool value)
{
    if (QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive) || (!m_bSystemIsServer && value)) {
        m_switchWidget->setVisible(true);
        GSettingWatcher::instance()->bind("perssonalGeneralEffects", m_switchWidget);
    } else {
        m_switchWidget->setVisible(false);
    }
}

void PersonalizationGeneral::onWindowRadiusChanged(int radius)
{
    /* 虽然自定义圆角值保留了三档: 直角 0px/ 小圆角 8px/ 大圆角 18px
     * 但是实际是允许用户用命令自定义任何 >= 0 的窗口圆角, 但是在滑动条上依旧显示大概范围的值
     */

    m_windowRadius = radius;

    if (m_winRoundSlider) {
        if (m_windowRadius <= 0) {
            m_winRoundSlider->slider()->setValue(0);
        } else if (0 < m_windowRadius && m_windowRadius <= 8) {
            m_winRoundSlider->slider()->setValue(1);
        } else {
            m_winRoundSlider->slider()->setValue(2);
        }
    }
}

void PersonalizationGeneral::onOpacityChanged(std::pair<int, double> value)
{
    if (m_transparentSlider) {
        m_transparentSlider->slider()->blockSignals(true);
        m_transparentSlider->slider()->setValue(value.first);
        m_transparentSlider->setValueLiteral(QString::number(value.second));
        m_transparentSlider->slider()->blockSignals(false);
    }
}

void PersonalizationGeneral::onMiniEffectChanged(int index)
{
    if(index < m_cmbMiniEffect->comboBox()->count())
        m_cmbMiniEffect->comboBox()->setCurrentIndex(index);
}

void PersonalizationGeneral::onActiveColorChanged(const QString &newColor)
{
    int colorIndex = ACTIVE_COLORS.indexOf(newColor.trimmed());

    if (colorIndex >= 0)
        updateActiveColors(m_activeColorsList[colorIndex]);
}

void PersonalizationGeneral::onActiveColorClicked()
{
    RoundColorWidget *activeColor = qobject_cast<RoundColorWidget *>(sender());

    RoundColorWidget* pItem = dynamic_cast<RoundColorWidget*>(sender());
    if(nullptr != pItem)
        m_bgWidget->setSelectedItem(pItem);
    //设置active color
    QString strColor = ACTIVE_COLORS[m_activeColorsList.indexOf(activeColor)];
    qDebug() << Q_FUNC_INFO << " strColor : " << strColor;
    Q_EMIT requestSetActiveColor(strColor);
}

void RingColorWidget::paintEvent(QPaintEvent *event)
{
    const DPalette &dp = DApplicationHelper::instance()->palette(this);
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(dp.brush(DPalette::ItemBackground));
    painter.drawRoundedRect(rect(), 18, 18);

    QWidget::paintEvent(event);
    if(nullptr == m_selectedItem)
        return;

    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);

    //draw select circle
    QPen pen;
    pen.setBrush(palette().highlight());
    pen.setWidth(borderWidth);  //pen width
    painter.setPen(pen);
    QRect rc = m_selectedItem->geometry();
    painter.drawEllipse(QRect(rc.center().x() - 14, rc.center().y() - 14, 30, 30));
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
}
