#ifndef MENU_GAMESCENE_HPP
#define MENU_GAMESCENE_HPP

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QFrame>
#include <QResizeEvent>
#include <QLinearGradient>
#include <QPainterPath>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QCoreApplication>
#include <QEnterEvent>
#include <QUrl>
#include <QSoundEffect>
#include "../include/players.hpp"
#include "../include/wall.hpp"
#include "../include/furniture.hpp"
#include "../include/door.hpp"
#include "../include/trap.hpp"
#include "../include/machine.hpp"
#include "../include/classes.hpp"
#include "../include/keyitem.hpp"


// Alright, so now we have implemented the very basics of the game. Let's now make this a real game ;)


// Classes to read about:
// QGraphicsItem
// QPainter
// QPixmap
// QRectF
// QBrush


// GLOBAL MUSIC
QMediaPlayer* music;
QAudioOutput* audio;

// GLOBAL PAUSE STATE
bool paused = false;

class MenuWindow;
void showMainMenu(QGraphicsView* currentView);

class HoverSoundFilter : public QObject {
public:
    HoverSoundFilter(QSoundEffect* player, QObject* parent = nullptr)
        : QObject(parent), hoverPlayer(player) {}

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::Enter) {
            if (hoverPlayer) {
                hoverPlayer->stop();
                hoverPlayer->play();
            }
        }
        return QObject::eventFilter(watched, event);
    }

private:
    QSoundEffect* hoverPlayer;
};

QGraphicsView* createGameView() {

    QGraphicsScene* scene = new QGraphicsScene;

    QPixmap levelBg("assets/level1_closed.png");
    if (levelBg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level1_closed.png";
    }

    QGraphicsPixmapItem* background = scene->addPixmap(levelBg);
    background->setZValue(-100);

    scene->setSceneRect(0, 0, 800, 600);

    auto addWall = [&](int x, int y, int w, int h) {
        scene->addItem(new Wall(x, y, w, h));
    };

    auto addBox = [&](int x, int y, int w, int h) {
        scene->addItem(new Furniture(x, y, w, h));
    };

    auto addTrap = [&](int x, int y, int w, int h) {
        scene->addItem(new Trap(x, y, w, h));
    };

        auto spawnAccessKey = [&](QPointF pos) {
        KeyItem* worldKey = new KeyItem(
            QCoreApplication::applicationDirPath() + "/assets/key.gif",
            60, 90
        );

        worldKey->setPos(pos.x(), pos.y());
        worldKey->setZValue(300);
        scene->addItem(worldKey);
    };

Player* player = new Player(0, 0);
player->setPos(568, 400);
scene->addItem(player);

// TEMP hidden health bar, only so damage/death works until teammate merges HUD
HealthBar* hiddenHealthBar = new HealthBar();
hiddenHealthBar->setVisible(false);
scene->addItem(hiddenHealthBar);
player->setHealthBar(hiddenHealthBar);

// HUD KEY (hidden until collected)
KeyItem* hudKey = new KeyItem(
    QCoreApplication::applicationDirPath() + "/assets/key.gif",
    90,140
);
hudKey->setPos(729, 488);
hudKey->setZValue(1500);
hudKey->hide();
scene->addItem(hudKey);

player->setHudKey(hudKey);

scene->setFocusItem(player);
player->setFocus();

    // TEMP TEST: spawn key in scene
    spawnAccessKey(QPointF(500, 300));

    // WALLS
    addWall(48, 0, 723, 46);
    addWall(0, 0, 46, 346);
    addWall(0, 344, 74, 149);
    addWall(0, 494, 800, 106);
    addWall(771, 0, 29, 199);
    addWall(766, 198, 34, 296);
    addWall(301, 276, 34, 64);
    addWall(335, 276, 292, 24);
    addWall(304, 404, 31, 89);
    addWall(45, 327, 32, 15);
    addWall(505, 390, 26, 24);

    // FURNITURE
    addBox(48, 48, 126, 26);
    addBox(303, 50, 462, 19);
    addBox(579, 68, 59, 98);
    addBox(403, 72, 175, 29);
    addBox(615, 170, 27, 31);
    addBox(51, 83, 27, 98);
    addBox(91, 383, 96, 71);
    addBox(307, 132, 24, 25);
    addBox(403, 179, 61, 10);
    addBox(339, 153, 57, 20);
    addBox(418, 189, 40, 12);
    addBox(353, 141, 23, 11);
    addBox(397, 165, 56, 12);
    addBox(504, 120, 28, 21);

    // MACHINE
    addBox(164, 186, 29, 22);
    addBox(196, 200, 85, 7);
    addBox(211, 208, 39, 7);
    addBox(195, 185, 85, 15);
    addBox(183, 131, 101, 54);
    addBox(193, 116, 83, 16);
    addBox(206, 106, 54, 10);
    //addBox(226, 88, 8, 18);
    //addBox(234, 102, 10, 4);
    addBox(216, 102, 10, 4);
    addBox(187, 124, 6, 7);
    addBox(199, 112, 6, 4);
    addBox(284, 134, 23, 20);
    addBox(170, 121, 12, 32);
    addBox(176, 111, 17, 8);
    addBox(196, 101, 5, 10);
    addBox(261, 101, 4, 9);
    addBox(286, 117, 5, 17);
    addBox(271, 110, 14, 6);
    addBox(284, 166, 5, 21);
    addBox(280, 186, 6, 9);

    // DOOR
    Door* door = new Door(658, 155, 100, 25);
    scene->addItem(door);

    // TRAP
    addTrap(176, 47, 124, 14);

    QGraphicsView* view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(0);
    view->setBackgroundBrush(Qt::black);
    view->setAlignment(Qt::AlignCenter);

    view->showFullScreen();
    QApplication::processEvents();
    view->setFocus();
    scene->setFocusItem(player);
    player->setFocus();

    auto fitScene = [view, scene]() {
        view->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
    };

    fitScene();
    QTimer::singleShot(0, [fitScene]() { fitScene(); });
    QTimer::singleShot(50, [fitScene]() { fitScene(); });

    // ===== WORKING PAUSE UI ON TOP OF GAME =====
    QPushButton* pauseButton = new QPushButton(view->viewport());
    pauseButton->setGeometry(20, 20, 56, 56);
    pauseButton->setText("");
    pauseButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(0,0,0,120);"
        "   border: 2px solid rgba(255,255,255,180);"
        "   border-radius: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255,255,255,40);"
        "   border: 2px solid white;"
        "}"
    );
    pauseButton->raise();
    pauseButton->show();

    QFrame* leftBar = new QFrame(pauseButton);
    leftBar->setGeometry(16, 12, 8, 32);
    leftBar->setStyleSheet(
        "background-color: white;"
        "border-radius: 3px;"
    );
    leftBar->show();

    QFrame* rightBar = new QFrame(pauseButton);
    rightBar->setGeometry(32, 12, 8, 32);
    rightBar->setStyleSheet(
        "background-color: white;"
        "border-radius: 3px;"
    );
    rightBar->show();

QWidget* pauseOverlay = new QWidget(view->viewport());
pauseOverlay->setGeometry(view->viewport()->rect());
pauseOverlay->setStyleSheet("background-color: rgba(0,0,0,140);");
pauseOverlay->hide();
pauseOverlay->raise();

QVBoxLayout* overlayLayout = new QVBoxLayout(pauseOverlay);
overlayLayout->setContentsMargins(0, 0, 0, 0);
overlayLayout->setAlignment(Qt::AlignCenter);

QFrame* pausePanel = new QFrame;
pausePanel->setFixedSize(420, 260);
overlayLayout->addWidget(pausePanel, 0, Qt::AlignCenter);

    pausePanel->setStyleSheet(
        "QFrame {"
        "   background-color: rgba(10, 20, 45, 230);"
        "   border: 3px solid #36e0ff;"
        "   border-radius: 18px;"
        "}"
    );

    QVBoxLayout* pauseLayout = new QVBoxLayout(pausePanel);
    pauseLayout->setSpacing(18);
    pauseLayout->setContentsMargins(35, 30, 35, 30);

    QLabel* pauseTitle = new QLabel("PAUSED");
    pauseTitle->setAlignment(Qt::AlignCenter);
    pauseTitle->setStyleSheet(
        "color: white;"
        "font-size: 30px;"
        "font-weight: bold;"
    );

    QPushButton* continueButton = new QPushButton("CONTINUE");
    QPushButton* leaveButton = new QPushButton("LEAVE GAME");

    continueButton->setMinimumHeight(70);
    leaveButton->setMinimumHeight(70);
    QString pauseBtnStyle =
        "QPushButton {"
        "   background-color: rgba(0,0,0,155);"
        "   color: white;"
        "   border: 3px solid #36e0ff;"
        "   border-radius: 12px;"
        "   font-size: 22px;"
        "   font-weight: bold;"
        "   padding: 16px;"
        "}"
        "QPushButton:hover {"
        "   color: #7ef9ff;"
        "   background-color: rgba(20,40,90,210);"
        "}";

    continueButton->setStyleSheet(pauseBtnStyle);
    leaveButton->setStyleSheet(pauseBtnStyle);

    pauseLayout->addWidget(pauseTitle);
    pauseLayout->addWidget(continueButton);
    pauseLayout->addWidget(leaveButton);

QObject::connect(pauseButton, &QPushButton::clicked, [=]() {
    if (player->isDead()) {
        return;
    }

    paused = true;
    pauseOverlay->setGeometry(view->viewport()->rect());
    pauseOverlay->show();
    pauseOverlay->raise();
});

    QObject::connect(continueButton, &QPushButton::clicked, [=]() {
        paused = false;
        pauseOverlay->hide();
        view->setFocus();
        scene->setFocusItem(player);
        player->setFocus();
    });

    QObject::connect(leaveButton, &QPushButton::clicked, [=]() {
        paused = false;
        showMainMenu(view);
    });
    // ===== DEATH FADE OVERLAY =====
    QWidget* deathFadeOverlay = new QWidget(view->viewport());
    deathFadeOverlay->setGeometry(view->viewport()->rect());
    deathFadeOverlay->setStyleSheet("background-color: rgba(0,0,0,0);");
    deathFadeOverlay->hide();
    deathFadeOverlay->raise();
       
    // ===== GAME OVER UI =====
    QWidget* gameOverOverlay = new QWidget(view->viewport());
    gameOverOverlay->setGeometry(view->viewport()->rect());
    gameOverOverlay->setStyleSheet("background-color: rgba(0,0,0,255);");
    gameOverOverlay->hide();
    gameOverOverlay->raise();

    QVBoxLayout* gameOverLayout = new QVBoxLayout(gameOverOverlay);
    gameOverLayout->setContentsMargins(0, 0, 0, 0);
    gameOverLayout->setAlignment(Qt::AlignCenter);

    // Main container
    QWidget* gameOverContainer = new QWidget(gameOverOverlay);
    gameOverContainer->setStyleSheet("background: transparent;");
    QVBoxLayout* gameOverContainerLayout = new QVBoxLayout(gameOverContainer);
    gameOverContainerLayout->setAlignment(Qt::AlignCenter);
    gameOverContainerLayout->setSpacing(20);
    gameOverContainerLayout->setContentsMargins(0, 0, 0, 0);

    // ===== GLITCH TITLE =====
    QWidget* titleWrap = new QWidget;
    titleWrap->setFixedSize(720, 230);
    titleWrap->setStyleSheet("background: transparent;");

    QLabel* cyanText = new QLabel("GAME\nOVER", titleWrap);
    cyanText->setAlignment(Qt::AlignCenter);
    cyanText->setGeometry(0, 0, 720, 230);
    cyanText->move(6, 0);
    cyanText->setStyleSheet(
        "color: rgb(0,255,255);"
        "background: transparent;"
        "font-size: 78px;"
        "font-weight: 900;"
        "font-family: Impact, Arial Black, sans-serif;"
        "letter-spacing: 4px;"
    );

    QLabel* magentaText = new QLabel("GAME\nOVER", titleWrap);
    magentaText->setAlignment(Qt::AlignCenter);
    magentaText->setGeometry(0, 0, 720, 230);
    magentaText->move(-6, 0);
    magentaText->setStyleSheet(
        "color: rgb(255,0,200);"
        "background: transparent;"
        "font-size: 78px;"
        "font-weight: 900;"
        "font-family: Impact, Arial Black, sans-serif;"
        "letter-spacing: 4px;"
    );

    QLabel* mainText = new QLabel("GAME\nOVER", titleWrap);
    mainText->setAlignment(Qt::AlignCenter);
    mainText->setGeometry(0, 0, 720, 230);
    mainText->setStyleSheet(
        "color: white;"
        "background: transparent;"
        "font-size: 78px;"
        "font-weight: 900;"
        "font-family: Impact, Arial Black, sans-serif;"
        "letter-spacing: 4px;"
    );

    // ===== SMALL GLITCH LINES =====
    QFrame* glitch1 = new QFrame(gameOverOverlay);
    glitch1->setGeometry(130, 120, 180, 4);
    glitch1->setStyleSheet("background-color: rgba(0,255,255,180); border: none;");

    QFrame* glitch2 = new QFrame(gameOverOverlay);
    glitch2->setGeometry(900, 180, 140, 4);
    glitch2->setStyleSheet("background-color: rgba(255,0,200,180); border: none;");

    QFrame* glitch3 = new QFrame(gameOverOverlay);
    glitch3->setGeometry(260, 300, 220, 3);
    glitch3->setStyleSheet("background-color: rgba(255,255,255,120); border: none;");

    QFrame* glitch4 = new QFrame(gameOverOverlay);
    glitch4->setGeometry(760, 330, 170, 3);
    glitch4->setStyleSheet("background-color: rgba(0,255,255,150); border: none;");

    // ===== BUTTONS =====
    QPushButton* tryAgainButton = new QPushButton("TRY AGAIN");
    QPushButton* gameOverMenuButton = new QPushButton("MAIN MENU");

    tryAgainButton->setFixedSize(320, 72);
    gameOverMenuButton->setFixedSize(320, 72);

    QString gameOverBtnStyle =
        "QPushButton {"
        "   background-color: rgba(0,0,0,210);"
        "   color: white;"
        "   border: 3px solid white;"
        "   border-radius: 0px;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   padding: 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(20,20,20,255);"
        "   border: 3px solid rgb(0,255,255);"
        "   color: rgb(0,255,255);"
        "}";

    tryAgainButton->setStyleSheet(gameOverBtnStyle);
    gameOverMenuButton->setStyleSheet(gameOverBtnStyle);

    gameOverContainerLayout->addWidget(titleWrap, 0, Qt::AlignCenter);
    gameOverContainerLayout->addSpacing(25);
    gameOverContainerLayout->addWidget(tryAgainButton, 0, Qt::AlignCenter);
    gameOverContainerLayout->addWidget(gameOverMenuButton, 0, Qt::AlignCenter);

    gameOverLayout->addWidget(gameOverContainer, 0, Qt::AlignCenter);

    // Simple glitch animation
    QTimer* glitchTimer = new QTimer(gameOverOverlay);

    QObject::connect(glitchTimer, &QTimer::timeout, [=]() {
        static bool flip = false;
        flip = !flip;

        if (flip) {
            cyanText->move(8, 0);
            magentaText->move(-8, 0);
            glitch1->setGeometry(110, 118, 210, 4);
            glitch2->setGeometry(920, 184, 120, 4);
            glitch3->setGeometry(240, 296, 250, 3);
            glitch4->setGeometry(740, 334, 200, 3);
        } else {
            cyanText->move(4, 0);
            magentaText->move(-4, 0);
            glitch1->setGeometry(130, 120, 180, 4);
            glitch2->setGeometry(900, 180, 140, 4);
            glitch3->setGeometry(260, 300, 220, 3);
            glitch4->setGeometry(760, 330, 170, 3);
        }
    });

    QObject::connect(player, &Player::died, [=]() {
    paused = true;

    // stop player focus
    view->clearFocus();

    // make sure overlays fill full screen
    deathFadeOverlay->setGeometry(view->viewport()->rect());
    gameOverOverlay->setGeometry(view->viewport()->rect());

    // ===== SCREEN SHAKE =====
    QTransform baseTransform = view->transform();

QTimer* shakeTimer = new QTimer(view);
int* shakeStep = new int(0);

QObject::connect(shakeTimer, &QTimer::timeout, [=]() mutable {
    (*shakeStep)++;

    int dx = 0;
    int dy = 0;

    switch ((*shakeStep) % 6) {
        case 0: dx = -10; dy = 0; break;
        case 1: dx = 10; dy = 0; break;
        case 2: dx = 0; dy = -8; break;
        case 3: dx = 0; dy = 8; break;
        case 4: dx = -6; dy = -6; break;
        case 5: dx = 6; dy = 6; break;
    }

    view->setTransform(baseTransform);
    view->translate(dx, dy);

    if (*shakeStep >= 12) {
        shakeTimer->stop();
        view->setTransform(baseTransform);
        delete shakeStep;
        shakeTimer->deleteLater();
    }
});

shakeTimer->start(30);

    // ===== FADE TO BLACK =====
    deathFadeOverlay->show();
    deathFadeOverlay->raise();

    QTimer* fadeTimer = new QTimer(deathFadeOverlay);
    int* alpha = new int(0);

    QObject::connect(fadeTimer, &QTimer::timeout, [=]() mutable {
        *alpha += 20;
        if (*alpha > 255) *alpha = 255;

        deathFadeOverlay->setStyleSheet(
            QString("background-color: rgba(0,0,0,%1);").arg(*alpha)
        );

        if (*alpha >= 255) {
            fadeTimer->stop();
            delete alpha;
            fadeTimer->deleteLater();

            gameOverOverlay->show();
            gameOverOverlay->raise();
            glitchTimer->start(120);
        }
    });

    fadeTimer->start(35);
});

    QObject::connect(tryAgainButton, &QPushButton::clicked, [=]() {
    glitchTimer->stop();
    paused = false;

    deathFadeOverlay->hide();
    deathFadeOverlay->setStyleSheet("background-color: rgba(0,0,0,0);");

    QGraphicsView* newGameView = createGameView();
    newGameView->show();
    view->hide();
    view->close();
});

    QObject::connect(gameOverMenuButton, &QPushButton::clicked, [=]() {
    glitchTimer->stop();
    paused = false;

    deathFadeOverlay->hide();
    deathFadeOverlay->setStyleSheet("background-color: rgba(0,0,0,0);");

    showMainMenu(view);
});

    return view;
}

class TitleWidget : public QWidget {
public:
    TitleWidget(QWidget* parent = nullptr) : QWidget(parent) {
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

        QFont font("Arial Black", 52, QFont::Bold);
        font.setLetterSpacing(QFont::AbsoluteSpacing, 3);

        QFontMetrics fm(font);

        QString text = "TECHNOSCAPE";

        int textWidth = fm.horizontalAdvance(text);
        int textX = (width() - textWidth) / 2;
        int textBaseline = height() / 2 + 20;

        QPainterPath path;
        path.addText(textX, textBaseline, font, text);

        painter.setPen(Qt::NoPen);

        painter.setBrush(QColor(0, 50, 110, 220));
        painter.save();
        painter.translate(0, 12);
        painter.drawPath(path);
        painter.restore();

        painter.setBrush(QColor(0, 120, 220, 180));
        painter.save();
        painter.translate(0, 6);
        painter.drawPath(path);
        painter.restore();

        painter.setPen(QPen(QColor(120, 255, 255, 190), 4));
        painter.drawPath(path);

        QLinearGradient gradient(0, 20, 0, height());
        gradient.setColorAt(0.0, QColor(235, 255, 255));
        gradient.setColorAt(0.25, QColor(170, 255, 255));
        gradient.setColorAt(0.55, QColor(80, 235, 255));
        gradient.setColorAt(1.0, QColor(0, 150, 255));

        painter.setPen(QPen(QColor(230, 255, 255), 2));
        painter.setBrush(gradient);
        painter.drawPath(path);
    }
};

class MenuWindow : public QWidget {
public:
    QLabel* background;
    TitleWidget* title;
    QFrame* panel;

    MenuWindow() {
        setWindowTitle("Technoscape");

        background = new QLabel(this);
        QPixmap bg("assets/menu_bg.png");
        if (bg.isNull()) {
            //qDebug() << "ERROR: IMAGE NOT FOUND: assets/menu_bg.png";
        }
        background->setPixmap(bg);
        background->setScaledContents(true);
        background->setGeometry(0, 0, width(), height());
        background->lower();


        panel = new QFrame(this);
        panel->setGeometry(0, 0, 450, 430);
        panel->setStyleSheet(
            "QFrame {"
            "   background-color: rgba(10, 20, 45, 145);"
            "   border: 2px solid #36e0ff;"
            "   border-radius: 16px;"
            "}"
        );

        title = new TitleWidget(this);
        title->setGeometry(0, 0, 630, 229);

        QGraphicsDropShadowEffect* titleGlow = new QGraphicsDropShadowEffect;
        titleGlow->setBlurRadius(100);
        titleGlow->setColor(QColor(0, 255, 255, 220));
        titleGlow->setOffset(0, 0);
        title->setGraphicsEffect(titleGlow);

        QVBoxLayout* panelLayout = new QVBoxLayout(panel);
        panelLayout->setSpacing(28);
        panelLayout->setContentsMargins(32, 32, 32, 32);

        QMediaPlayer* clickPlayer = new QMediaPlayer(this);
QAudioOutput* clickAudio = new QAudioOutput(this);
clickPlayer->setAudioOutput(clickAudio);
clickPlayer->setSource(QUrl::fromLocalFile(
    QCoreApplication::applicationDirPath() + "/assets/sounds/click.wav"
));
clickAudio->setVolume(1.0);

QSoundEffect* hoverPlayer = new QSoundEffect(this);
hoverPlayer->setSource(QUrl::fromLocalFile(
    QCoreApplication::applicationDirPath() + "/assets/sounds/houver.wav"
));
hoverPlayer->setVolume(1.0);

        QPushButton* startButton = new QPushButton("START GAME");
        QPushButton* settingsButton = new QPushButton("SETTINGS");
        QPushButton* exitButton = new QPushButton("EXIT");
        HoverSoundFilter* hoverFilter = new HoverSoundFilter(hoverPlayer, this);
startButton->installEventFilter(hoverFilter);
settingsButton->installEventFilter(hoverFilter);
exitButton->installEventFilter(hoverFilter);

QObject::connect(startButton, &QPushButton::clicked, [=]() {
    clickPlayer->stop();
    clickPlayer->setPosition(0);
    clickPlayer->play();
});

QObject::connect(settingsButton, &QPushButton::clicked, [=]() {
    clickPlayer->stop();
    clickPlayer->setPosition(0);
    clickPlayer->play();
});

QObject::connect(exitButton, &QPushButton::clicked, [=]() {
    clickPlayer->stop();
    clickPlayer->setPosition(0);
    clickPlayer->play();
});

        QString btnStyle =
            "QPushButton {"
            "   background-color: rgba(0,0,0,155);"
            "   color: white;"
            "   border: 3px solid #36e0ff;"
            "   border-radius: 12px;"
            "   font-size: 24px;"
            "   font-weight: bold;"
            "   padding: 18px;"
            "}"
            "QPushButton:hover {"
            "   border: 3px solid #36e0ff;"
            "   color: #7ef9ff;"
            "   background-color: rgba(20,40,90,210);"
            "}";

        startButton->setStyleSheet(btnStyle);
        settingsButton->setStyleSheet(btnStyle);
        exitButton->setStyleSheet(btnStyle);

        startButton->setMinimumHeight(90);
        settingsButton->setMinimumHeight(90);
        exitButton->setMinimumHeight(90);

        QGraphicsDropShadowEffect* startGlow = new QGraphicsDropShadowEffect;
        startGlow->setBlurRadius(40);
        startGlow->setColor(QColor(0, 255, 255));
        startGlow->setOffset(0, 0);
        startButton->setGraphicsEffect(startGlow);

        QGraphicsDropShadowEffect* settingsGlow = new QGraphicsDropShadowEffect;
        settingsGlow->setBlurRadius(40);
        settingsGlow->setColor(QColor(0, 255, 255));
        settingsGlow->setOffset(0, 0);
        settingsButton->setGraphicsEffect(settingsGlow);

        QGraphicsDropShadowEffect* exitGlow = new QGraphicsDropShadowEffect;
        exitGlow->setBlurRadius(40);
        exitGlow->setColor(QColor(0, 255, 255));
        exitGlow->setOffset(0, 0);
        exitButton->setGraphicsEffect(exitGlow);

        panelLayout->addWidget(startButton);
        panelLayout->addWidget(settingsButton);
        panelLayout->addWidget(exitButton);

        QObject::connect(startButton, &QPushButton::clicked, [this]() {
            QTimer::singleShot(120, [this]() {
                paused = false;
                audio->setVolume(0.02);
                QGraphicsView* gameView = createGameView();
                gameView->show();
                this->close();
            });
        });

        QObject::connect(exitButton, &QPushButton::clicked, []() {
            QTimer::singleShot(120, []() {
                QApplication::quit();
            });
        });

        showFullScreen();
    }

protected:
    void resizeEvent(QResizeEvent* event) override {
        QWidget::resizeEvent(event);

        if (background) {
            background->setGeometry(0, 0, width(), height());
        }

        if (panel) {
            int panelW = 450;
            int panelH = 430;

            int panelX = (width() - panelW) / 2;
            int panelY = (height() - panelH) / 2 + 80;

            panel->setGeometry(panelX, panelY, panelW, panelH);

            if (title) {
                title->setGeometry(panelX - 90, panelY - 270, panelW + 180, 229);
            }
        }
    }
};

void showMainMenu(QGraphicsView* currentView) {
    MenuWindow* menu = new MenuWindow();
    menu->showFullScreen();
    QApplication::processEvents();
    currentView->hide();
    currentView->close();
}

#endif // MENU_GAMESCENE_HPP