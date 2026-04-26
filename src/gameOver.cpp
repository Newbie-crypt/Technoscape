#include "gameOver.hpp"
extern bool paused;
// This constructor is responsible for designing the "Game Over" screen that pops up after the player dies.
gameOver::gameOver(QGraphicsView* inputView, gameLevel* inputLevel) {
    deathFadeOverlay = new QWidget(inputView->viewport());
    deathFadeOverlay->setGeometry(inputView->viewport()->rect());
    deathFadeOverlay->setStyleSheet("background-color: rgba(0,0,0,0);");
    deathFadeOverlay->hide();
    deathFadeOverlay->raise();
       
    // UI of the Game over screen.
    gameOverOverlay = new QWidget(inputView->viewport());
    gameOverOverlay->setGeometry(inputView->viewport()->rect());
    gameOverOverlay->setStyleSheet("background-color: rgba(0,0,0,255);");
    gameOverOverlay->hide();
    gameOverOverlay->raise();

    gameOverLayout = new QVBoxLayout(gameOverOverlay);
    gameOverLayout->setContentsMargins(0, 0, 0, 0);
    gameOverLayout->setAlignment(Qt::AlignCenter);

    // Main container
    gameOverContainer = new QWidget(gameOverOverlay);
    gameOverContainer->setStyleSheet("background: transparent;");
    gameOverContainerLayout = new QVBoxLayout(gameOverContainer);
    gameOverContainerLayout->setAlignment(Qt::AlignCenter);
    gameOverContainerLayout->setSpacing(20);
    gameOverContainerLayout->setContentsMargins(0, 0, 0, 0);

    // Game over Text
    titleWrap = new QWidget;
    titleWrap->setFixedSize(720, 230);
    titleWrap->setStyleSheet("background: transparent;");

    cyanText = new QLabel("GAME\nOVER", titleWrap);
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

    magentaText = new QLabel("GAME\nOVER", titleWrap);
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

    mainText = new QLabel("GAME\nOVER", titleWrap);
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

    // Small Glitch Lines
    glitch1 = new QFrame(gameOverOverlay);
    glitch1->setGeometry(130, 120, 180, 4);
    glitch1->setStyleSheet("background-color: rgba(0,255,255,180); border: none;");

    glitch2 = new QFrame(gameOverOverlay);
    glitch2->setGeometry(900, 180, 140, 4);
    glitch2->setStyleSheet("background-color: rgba(255,0,200,180); border: none;");

    glitch3 = new QFrame(gameOverOverlay);
    glitch3->setGeometry(260, 300, 220, 3);
    glitch3->setStyleSheet("background-color: rgba(255,255,255,120); border: none;");

    glitch4 = new QFrame(gameOverOverlay);
    glitch4->setGeometry(760, 330, 170, 3);
    glitch4->setStyleSheet("background-color: rgba(0,255,255,150); border: none;");

    // Buttons
    tryAgainButton = new QPushButton("TRY AGAIN");
    gameOverMenuButton = new QPushButton("MAIN MENU");

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
    glitchTimer = new QTimer(gameOverOverlay);

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
    
    QObject::connect(inputLevel, &gameLevel::playerDied, [=]() {
            paused = true;

            // stop player focus
            inputView->clearFocus();

            // make sure overlays fill full screen
            deathFadeOverlay->setGeometry(inputView->viewport()->rect());
            gameOverOverlay->setGeometry(inputView->viewport()->rect());

            // Screen shake
            QTransform baseTransform = inputView->transform();

            QTimer* shakeTimer = new QTimer(inputView);
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

                inputView->setTransform(baseTransform);
                inputView->translate(dx, dy);

                if (*shakeStep >= 12) {
                    shakeTimer->stop();
                    inputView->setTransform(baseTransform);
                    delete shakeStep;
                    shakeTimer->deleteLater();
                }
            });

            shakeTimer->start(30);

            // Fade to black
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
            gameOverOverlay->hide();

            emit tryAgainRequested();

            inputView->hide();
    });

    QObject::connect(gameOverMenuButton, &QPushButton::clicked, [=]() {
        glitchTimer->stop();
        paused = false;

        deathFadeOverlay->hide();
        deathFadeOverlay->setStyleSheet("background-color: rgba(0,0,0,0);");

        emit mainMenuRequested();
    });

}

