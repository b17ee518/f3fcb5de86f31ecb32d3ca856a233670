/********************************************************************************
** Form generated from reading UI file 'playlistwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYLISTWINDOW_H
#define UI_PLAYLISTWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlaylistWindow
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QListWidget *playlistWidget;
    QListWidget *songlistWidget;

    void setupUi(QMainWindow *PlaylistWindow)
    {
        if (PlaylistWindow->objectName().isEmpty())
            PlaylistWindow->setObjectName(QStringLiteral("PlaylistWindow"));
        PlaylistWindow->resize(384, 265);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PlaylistWindow->sizePolicy().hasHeightForWidth());
        PlaylistWindow->setSizePolicy(sizePolicy);
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        QBrush brush1(QColor(120, 120, 120, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush1);
        PlaylistWindow->setPalette(palette);
        centralWidget = new QWidget(PlaylistWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        playlistWidget = new QListWidget(centralWidget);
        playlistWidget->setObjectName(QStringLiteral("playlistWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(playlistWidget->sizePolicy().hasHeightForWidth());
        playlistWidget->setSizePolicy(sizePolicy1);
        QFont font;
        font.setPointSize(12);
        playlistWidget->setFont(font);
        playlistWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        playlistWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        playlistWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        horizontalLayout->addWidget(playlistWidget);

        songlistWidget = new QListWidget(centralWidget);
        songlistWidget->setObjectName(QStringLiteral("songlistWidget"));
        sizePolicy1.setHeightForWidth(songlistWidget->sizePolicy().hasHeightForWidth());
        songlistWidget->setSizePolicy(sizePolicy1);
        songlistWidget->setFont(font);
        songlistWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        songlistWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        songlistWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        horizontalLayout->addWidget(songlistWidget);

        PlaylistWindow->setCentralWidget(centralWidget);

        retranslateUi(PlaylistWindow);

        QMetaObject::connectSlotsByName(PlaylistWindow);
    } // setupUi

    void retranslateUi(QMainWindow *PlaylistWindow)
    {
        PlaylistWindow->setWindowTitle(QApplication::translate("PlaylistWindow", "PlaylistWindow", 0));
    } // retranslateUi

};

namespace Ui {
    class PlaylistWindow: public Ui_PlaylistWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYLISTWINDOW_H
