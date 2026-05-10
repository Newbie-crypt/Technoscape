#pragma once
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QColor>
#include <QFont>
#include <QString>
#include <QPixmap>

class BossHealthBar : public QWidget {
    Q_OBJECT

private:
    int maxHP;
    int currentHP;
    bool halfHealthTriggered;
    QPixmap bossHead;

public:
    BossHealthBar(QWidget* parent = nullptr)
        : QWidget(parent), maxHP(1700), currentHP(1700), halfHealthTriggered(false) {

        setFixedSize(620, 54);
        setAttribute(Qt::WA_TransparentForMouseEvents);

        bossHead = QPixmap("assets/boss_head.png");
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

        double ratio = (double) currentHP / maxHP;

        if (!bossHead.isNull()) {
            painter.drawPixmap(
                0,
                5,
                bossHead.scaled(45, 45, Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
        }

        int barX = 55;
        int barY = 15;
        int barW = 540;
        int barH = 25;

        painter.setPen(QPen(QColor(0, 220, 255), 3));
        painter.setBrush(QColor(5, 12, 30));
        painter.drawRect(barX, barY, barW, barH);

        int fillWidth = barW * ratio;

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 190, 255));
        painter.drawRect(barX + 2, barY + 2, fillWidth - 4, barH - 4);

        painter.setPen(QPen(Qt::white, 3));
        painter.drawLine(barX + barW / 2, barY, barX + barW / 2, barY + barH);
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