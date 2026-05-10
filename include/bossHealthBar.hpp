#pragma once
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPen>
#include <QColor>
#include <QGraphicsDropShadowEffect>
#include <QCoreApplication>

class BossHealthBar : public QWidget {
    Q_OBJECT

private:
    int maxHP;
    int currentHP;
    bool halfHealthTriggered;
    QPixmap bossHead;

public:
    BossHealthBar(QWidget* parent = nullptr)
        : QWidget(parent), maxHP(1500), currentHP(1500), halfHealthTriggered(false) {

        setFixedSize(720, 70);
        setAttribute(Qt::WA_TransparentForMouseEvents);

        bossHead = QPixmap(QCoreApplication::applicationDirPath() + "/assets/boss_head.png");

        QGraphicsDropShadowEffect* glow = new QGraphicsDropShadowEffect(this);
        glow->setBlurRadius(35);
        glow->setColor(QColor(0, 220, 255, 220));
        glow->setOffset(0, 0);
        setGraphicsEffect(glow);
    }

    void setHP(int hp) {
        currentHP = hp;

        if (currentHP < 0) {
            currentHP = 0;
        }

        if (currentHP > maxHP) {
            currentHP = maxHP;
        }

        checkHalfHealth();
        update();
    }

    void decreaseHP(int value) {
        currentHP -= value;

        if (currentHP < 0) {
            currentHP = 0;
        }

        checkHalfHealth();
        update();
    }

    int getHP() {
        return currentHP;
    }

    int getMaxHP() {
        return maxHP;
    }

signals:
    void halfHealthBoss();
    void bossDead();

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        double ratio = (double) currentHP / maxHP;

        int iconSize = 60;
        int iconX = 0;
        int iconY = 5;

        int barX = 70;
        int barY = 18;
        int barW = width() - barX - 10;
        int barH = 34;

        int fillWidth = barW * ratio;

        QRect iconRect(iconX, iconY, iconSize, iconSize);
        QRect bgRect(barX, barY, barW, barH);
        QRect fillRect(barX + 4, barY + 4, fillWidth - 8, barH - 8);

        painter.setPen(QPen(QColor(0, 220, 255), 3));
        painter.setBrush(QColor(5, 12, 30, 240));
        painter.drawEllipse(iconRect);

        if (!bossHead.isNull()) {
            QPixmap head = bossHead.scaled(52, 52, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap(iconX + 4, iconY + 4, head);
        }

        painter.setPen(QPen(QColor(0, 230, 255), 3));
        painter.setBrush(QColor(4, 10, 28, 235));
        painter.drawRoundedRect(bgRect, 13, 13);

        painter.setPen(QPen(QColor(0, 255, 255, 90), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(bgRect.adjusted(-4, -4, 4, 4), 16, 16);

        if (fillWidth > 10) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(0, 190, 255));
            painter.drawRoundedRect(fillRect, 9, 9);

            painter.setBrush(QColor(180, 255, 255, 90));
            painter.drawRoundedRect(QRect(barX + 7, barY + 7, fillWidth - 14, 7), 4, 4);
        }

        int middleX = barX + barW / 2;
        painter.setPen(QPen(QColor(255, 255, 255, 220), 3));
        painter.drawLine(middleX, barY + 4, middleX, barY + barH - 4);
    }


private:
    void checkHalfHealth() {
        if (!halfHealthTriggered && currentHP <= maxHP / 2) {
            halfHealthTriggered = true;
            emit halfHealthBoss();
        }

        if (currentHP <= 0) {
            emit bossDead();
        }
    }
};