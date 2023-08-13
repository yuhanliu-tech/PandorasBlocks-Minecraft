/********************************************************************************
** Form generated from reading UI file 'playerinfo.ui'
**
** Created by: Qt User Interface Compiler version 6.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYERINFO_H
#define UI_PLAYERINFO_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlayerInfo
{
public:
    QFrame *line;
    QLabel *label;
    QLabel *label_2;
    QLabel *posLabel;
    QLabel *label_4;
    QLabel *velLabel;
    QLabel *label_6;
    QLabel *accLabel;
    QLabel *lookLabel;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *chunkLabel;
    QLabel *label_11;
    QLabel *zoneLabel;

    void setupUi(QWidget *PlayerInfo)
    {
        if (PlayerInfo->objectName().isEmpty())
            PlayerInfo->setObjectName("PlayerInfo");
        PlayerInfo->resize(403, 344);
        line = new QFrame(PlayerInfo);
        line->setObjectName("line");
        line->setGeometry(QRect(10, 20, 381, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label = new QLabel(PlayerInfo);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 0, 381, 31));
        QFont font;
        font.setPointSize(12);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(PlayerInfo);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 60, 71, 31));
        QFont font1;
        font1.setPointSize(10);
        label_2->setFont(font1);
        posLabel = new QLabel(PlayerInfo);
        posLabel->setObjectName("posLabel");
        posLabel->setGeometry(QRect(120, 60, 271, 31));
        posLabel->setFont(font1);
        label_4 = new QLabel(PlayerInfo);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(20, 100, 71, 31));
        label_4->setFont(font1);
        velLabel = new QLabel(PlayerInfo);
        velLabel->setObjectName("velLabel");
        velLabel->setGeometry(QRect(120, 100, 271, 31));
        velLabel->setFont(font1);
        label_6 = new QLabel(PlayerInfo);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(20, 140, 81, 31));
        label_6->setFont(font1);
        accLabel = new QLabel(PlayerInfo);
        accLabel->setObjectName("accLabel");
        accLabel->setGeometry(QRect(120, 140, 271, 31));
        accLabel->setFont(font1);
        lookLabel = new QLabel(PlayerInfo);
        lookLabel->setObjectName("lookLabel");
        lookLabel->setGeometry(QRect(120, 180, 271, 31));
        lookLabel->setFont(font1);
        label_9 = new QLabel(PlayerInfo);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(20, 180, 81, 31));
        label_9->setFont(font1);
        label_10 = new QLabel(PlayerInfo);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(20, 220, 81, 31));
        label_10->setFont(font1);
        chunkLabel = new QLabel(PlayerInfo);
        chunkLabel->setObjectName("chunkLabel");
        chunkLabel->setGeometry(QRect(120, 220, 271, 31));
        chunkLabel->setFont(font1);
        label_11 = new QLabel(PlayerInfo);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(20, 260, 91, 31));
        label_11->setFont(font1);
        zoneLabel = new QLabel(PlayerInfo);
        zoneLabel->setObjectName("zoneLabel");
        zoneLabel->setGeometry(QRect(120, 260, 271, 31));
        zoneLabel->setFont(font1);

        retranslateUi(PlayerInfo);

        QMetaObject::connectSlotsByName(PlayerInfo);
    } // setupUi

    void retranslateUi(QWidget *PlayerInfo)
    {
        PlayerInfo->setWindowTitle(QCoreApplication::translate("PlayerInfo", "Form", nullptr));
        label->setText(QCoreApplication::translate("PlayerInfo", "Player Information", nullptr));
        label_2->setText(QCoreApplication::translate("PlayerInfo", "Position:", nullptr));
        posLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        label_4->setText(QCoreApplication::translate("PlayerInfo", "Velocity:", nullptr));
        velLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        label_6->setText(QCoreApplication::translate("PlayerInfo", "Acceleration:", nullptr));
        accLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        lookLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        label_9->setText(QCoreApplication::translate("PlayerInfo", "Look vector:", nullptr));
        label_10->setText(QCoreApplication::translate("PlayerInfo", "Chunk:", nullptr));
        chunkLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
        label_11->setText(QCoreApplication::translate("PlayerInfo", "Terrain Zone:", nullptr));
        zoneLabel->setText(QCoreApplication::translate("PlayerInfo", "UNK", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlayerInfo: public Ui_PlayerInfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYERINFO_H
