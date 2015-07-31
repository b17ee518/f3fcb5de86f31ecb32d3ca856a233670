/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "lyricframe.h"
#include "qdirectslider.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QFrame *controlBarFrame;
    QHBoxLayout *horizontalLayout;
    QPushButton *listButton;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *playPauseButton;
    QPushButton *stopButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *previousButton;
    QPushButton *nextButton;
    QSpacerItem *horizontalSpacer_2;
    QDirectSlider *positionSlider;
    QSpacerItem *horizontalSpacer_6;
    QLabel *timeLabel;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *loopSwitchButton;
    QSpacerItem *horizontalSpacer_4;
    QDirectSlider *volumeSlider;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *closeButton;
    LyricFrame *lyricFrame;
    QVBoxLayout *verticalLayout_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(800, 200);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        controlBarFrame = new QFrame(centralWidget);
        controlBarFrame->setObjectName(QStringLiteral("controlBarFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(controlBarFrame->sizePolicy().hasHeightForWidth());
        controlBarFrame->setSizePolicy(sizePolicy);
        controlBarFrame->setMaximumSize(QSize(16777215, 24));
        controlBarFrame->setFrameShape(QFrame::StyledPanel);
        controlBarFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(controlBarFrame);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        listButton = new QPushButton(controlBarFrame);
        listButton->setObjectName(QStringLiteral("listButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(listButton->sizePolicy().hasHeightForWidth());
        listButton->setSizePolicy(sizePolicy1);
        listButton->setMinimumSize(QSize(48, 24));
        listButton->setMaximumSize(QSize(48, 24));
        listButton->setCheckable(true);

        horizontalLayout->addWidget(listButton);

        horizontalSpacer_3 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        playPauseButton = new QPushButton(controlBarFrame);
        playPauseButton->setObjectName(QStringLiteral("playPauseButton"));
        sizePolicy1.setHeightForWidth(playPauseButton->sizePolicy().hasHeightForWidth());
        playPauseButton->setSizePolicy(sizePolicy1);
        playPauseButton->setMinimumSize(QSize(24, 24));
        playPauseButton->setMaximumSize(QSize(24, 24));
        playPauseButton->setCheckable(true);

        horizontalLayout->addWidget(playPauseButton);

        stopButton = new QPushButton(controlBarFrame);
        stopButton->setObjectName(QStringLiteral("stopButton"));
        sizePolicy1.setHeightForWidth(stopButton->sizePolicy().hasHeightForWidth());
        stopButton->setSizePolicy(sizePolicy1);
        stopButton->setMinimumSize(QSize(24, 24));
        stopButton->setMaximumSize(QSize(24, 24));

        horizontalLayout->addWidget(stopButton);

        horizontalSpacer = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        previousButton = new QPushButton(controlBarFrame);
        previousButton->setObjectName(QStringLiteral("previousButton"));
        sizePolicy1.setHeightForWidth(previousButton->sizePolicy().hasHeightForWidth());
        previousButton->setSizePolicy(sizePolicy1);
        previousButton->setMinimumSize(QSize(24, 24));
        previousButton->setMaximumSize(QSize(24, 24));

        horizontalLayout->addWidget(previousButton);

        nextButton = new QPushButton(controlBarFrame);
        nextButton->setObjectName(QStringLiteral("nextButton"));
        sizePolicy1.setHeightForWidth(nextButton->sizePolicy().hasHeightForWidth());
        nextButton->setSizePolicy(sizePolicy1);
        nextButton->setMinimumSize(QSize(24, 24));
        nextButton->setMaximumSize(QSize(24, 24));

        horizontalLayout->addWidget(nextButton);

        horizontalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        positionSlider = new QDirectSlider(controlBarFrame);
        positionSlider->setObjectName(QStringLiteral("positionSlider"));
        positionSlider->setMinimumSize(QSize(0, 24));
        positionSlider->setMaximumSize(QSize(16777215, 24));
        positionSlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(positionSlider);

        horizontalSpacer_6 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_6);

        timeLabel = new QLabel(controlBarFrame);
        timeLabel->setObjectName(QStringLiteral("timeLabel"));
        sizePolicy.setHeightForWidth(timeLabel->sizePolicy().hasHeightForWidth());
        timeLabel->setSizePolicy(sizePolicy);
        timeLabel->setMinimumSize(QSize(0, 24));
        timeLabel->setMaximumSize(QSize(16777215, 24));

        horizontalLayout->addWidget(timeLabel);

        horizontalSpacer_5 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);

        loopSwitchButton = new QPushButton(controlBarFrame);
        loopSwitchButton->setObjectName(QStringLiteral("loopSwitchButton"));
        sizePolicy1.setHeightForWidth(loopSwitchButton->sizePolicy().hasHeightForWidth());
        loopSwitchButton->setSizePolicy(sizePolicy1);
        loopSwitchButton->setMinimumSize(QSize(36, 24));
        loopSwitchButton->setMaximumSize(QSize(36, 24));

        horizontalLayout->addWidget(loopSwitchButton);

        horizontalSpacer_4 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        volumeSlider = new QDirectSlider(controlBarFrame);
        volumeSlider->setObjectName(QStringLiteral("volumeSlider"));
        sizePolicy1.setHeightForWidth(volumeSlider->sizePolicy().hasHeightForWidth());
        volumeSlider->setSizePolicy(sizePolicy1);
        volumeSlider->setMinimumSize(QSize(50, 24));
        volumeSlider->setMaximumSize(QSize(50, 24));
        volumeSlider->setMaximum(100);
        volumeSlider->setValue(10);
        volumeSlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(volumeSlider);

        horizontalSpacer_7 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_7);

        closeButton = new QPushButton(controlBarFrame);
        closeButton->setObjectName(QStringLiteral("closeButton"));
        sizePolicy1.setHeightForWidth(closeButton->sizePolicy().hasHeightForWidth());
        closeButton->setSizePolicy(sizePolicy1);
        closeButton->setMinimumSize(QSize(24, 24));
        closeButton->setMaximumSize(QSize(24, 24));

        horizontalLayout->addWidget(closeButton);


        verticalLayout->addWidget(controlBarFrame);

        lyricFrame = new LyricFrame(centralWidget);
        lyricFrame->setObjectName(QStringLiteral("lyricFrame"));
        lyricFrame->setFrameShape(QFrame::StyledPanel);
        lyricFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(lyricFrame);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);

        verticalLayout->addWidget(lyricFrame);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);
        QObject::connect(closeButton, SIGNAL(clicked()), MainWindow, SLOT(slotOnCloseButtonClicked()));
        QObject::connect(volumeSlider, SIGNAL(valueChanged(int)), MainWindow, SLOT(slotOnVolumeSliderValueChanged(int)));
        QObject::connect(loopSwitchButton, SIGNAL(clicked()), MainWindow, SLOT(slotOnLoopSwitchButtonClicked()));
        QObject::connect(positionSlider, SIGNAL(valueChanged(int)), MainWindow, SLOT(slotOnPositionSliderValueChanged(int)));
        QObject::connect(nextButton, SIGNAL(clicked()), MainWindow, SLOT(slotOnNextButtonClicked()));
        QObject::connect(previousButton, SIGNAL(clicked()), MainWindow, SLOT(slotOnPreviousButtonClicked()));
        QObject::connect(stopButton, SIGNAL(clicked()), MainWindow, SLOT(slotOnStopButtonClicked()));
        QObject::connect(playPauseButton, SIGNAL(toggled(bool)), MainWindow, SLOT(slotOnPlayStopButtonToggled(bool)));
        QObject::connect(listButton, SIGNAL(toggled(bool)), MainWindow, SLOT(slotOnListButtonToggled(bool)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        listButton->setText(QApplication::translate("MainWindow", "List", 0));
        playPauseButton->setText(QApplication::translate("MainWindow", "\342\226\266", 0));
        stopButton->setText(QApplication::translate("MainWindow", "\342\226\240", 0));
        previousButton->setText(QApplication::translate("MainWindow", "<", 0));
        nextButton->setText(QApplication::translate("MainWindow", ">", 0));
        timeLabel->setText(QApplication::translate("MainWindow", "--:--/--:--", 0));
        loopSwitchButton->setText(QApplication::translate("MainWindow", "\342\227\213", 0));
        closeButton->setText(QApplication::translate("MainWindow", "X", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
