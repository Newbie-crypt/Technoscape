#include "../include/menu_gameScene.hpp"

bool paused = false;
QMediaPlayer* music;
QAudioOutput* audio;

// This event is responsible for drawing the Technoscape logo in the main menu.
void TitleWidget::paintEvent(QPaintEvent*) {
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

MenuWindow::MenuWindow(QGraphicsScene*& scene) : currentScene(scene) {

    // MAIN MENU DESIGN SECTION
    setWindowTitle("Technoscape");

    background = new QLabel(this);
    QPixmap bg("assets/menu_bg.png");

    if (bg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/menu_bg.png";
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

    QAudioDevice out = QMediaDevices::defaultAudioOutput();
    qDebug() << "Menu audio output:" << out.description();
    qDebug() << "Menu output is null?" << out.isNull();

    clickAudio->setDevice(out);
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

    // Adding the buttons..
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

    // END OF MAIN MENU DESIGN SECTION

    // Events when the buttons are pressed
    QObject::connect(startButton, &QPushButton::clicked, [this]() {
        QTimer::singleShot(120, [this]() {
            paused = false;
            audio->setVolume(0.02);
            view = createGameView();
            view->showFullScreen();
            this->hide();

            // This is necessary for us to dynamically allocate the robots in the main program (main.cpp)
            // emit gameStarted();
        });
    });

    QObject::connect(exitButton, &QPushButton::clicked, []() {
        QTimer::singleShot(120, []() {
            QApplication::quit();
        });
    });

    showFullScreen();
}

// The function's purpose is to set up the scene
QGraphicsView* MenuWindow::createGameView() {

    levelOne* L1 = new levelOne;
    currentLevel = L1;
    currentLevel->setupScene();
    L1->spawnEnemies();
    currentLevel->setupSpawnKeyEvent();

    QGraphicsView* view = new QGraphicsView(L1->getScene());
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(0);
    view->setBackgroundBrush(Qt::black);
    view->setAlignment(Qt::AlignCenter);
    view->showFullScreen();
    QTimer::singleShot(0, [L1, view]() {
        view->setFocus();
        L1->getScene()->setFocusItem(L1->getPlayer());
        L1->getPlayer()->setFocus();
    });

    // Preparing level 2 once the player is done with level 1.
    // QObject::connect(player, &Player::level2Requested, [=]() {
    //     paused = true;

    //     scene->setFocusItem(nullptr);
    //     player->clearFocus();
    //     view->clearFocus();

    //     QWidget* transitionOverlay = new QWidget(view->viewport());
    //     transitionOverlay->setGeometry(view->viewport()->rect());
    //     transitionOverlay->setStyleSheet("background-color: rgba(0,0,0,0);");
    //     transitionOverlay->show();
    //     transitionOverlay->raise();

    //     // Title container
    //     QWidget* introContainer = new QWidget(transitionOverlay);
    //     introContainer->setGeometry(0, 0, transitionOverlay->width(), transitionOverlay->height());
    //     introContainer->hide();

    //     QLabel* cyanText = new QLabel("LEVEL 2", introContainer);
    //     cyanText->setAlignment(Qt::AlignCenter);
    //     cyanText->setGeometry(0, 0, introContainer->width(), introContainer->height());
    //     cyanText->move(-4, 0);
    //     cyanText->setStyleSheet(
    //         "color: rgb(0,255,255);"
    //         "background: transparent;"
    //         "font-size: 54px;"
    //         "font-weight: 900;"
    //         "font-family: Impact, Arial Black, sans-serif;"
    //         "letter-spacing: 4px;"
    //     );

    //     QLabel* magentaText = new QLabel("LEVEL 2", introContainer);
    //     magentaText->setAlignment(Qt::AlignCenter);
    //     magentaText->setGeometry(0, 0, introContainer->width(), introContainer->height());
    //     magentaText->move(4, 0);
    //     magentaText->setStyleSheet(
    //         "color: rgb(255,0,200);"
    //         "background: transparent;"
    //         "font-size: 54px;"
    //         "font-weight: 900;"
    //         "font-family: Impact, Arial Black, sans-serif;"
    //         "letter-spacing: 4px;"
    //     );

    //     QLabel* mainText = new QLabel("LEVEL 2", introContainer);
    //     mainText->setAlignment(Qt::AlignCenter);
    //     mainText->setGeometry(0, 0, introContainer->width(), introContainer->height());
    //     mainText->setStyleSheet(
    //         "color: white;"
    //         "background: transparent;"
    //         "font-size: 54px;"
    //         "font-weight: 900;"
    //         "font-family: Impact, Arial Black, sans-serif;"
    //         "letter-spacing: 4px;"
    //     );


    //     // Decorative glitch lines
    //     QVector<QFrame*> glitchLines;
    //     for (int i = 0; i < 18; i++) {
    //         QFrame* line = new QFrame(introContainer);
    //         int x = rand() % 700;
    //         int y = 80 + rand() % 420;
    //         int w = 30 + rand() % 120;
    //         int h = 2 + rand() % 2;
    //         line->setGeometry(x, y, w, h);

    //         if (i % 3 == 0) {
    //             line->setStyleSheet("background-color: rgba(0,255,255,120); border: none;");
    //         } else if (i % 3 == 1) {
    //             line->setStyleSheet("background-color: rgba(255,0,200,120); border: none;");
    //         } else {
    //             line->setStyleSheet("background-color: rgba(255,255,255,60); border: none;");
    //         }

    //         line->hide();
    //         glitchLines.push_back(line);
    //     }

    //     // Fade to black
    //     QTimer* fadeTimer = new QTimer(transitionOverlay);
    //     int* alpha = new int(0);

    //     QObject::connect(fadeTimer, &QTimer::timeout, [=]() mutable {
    //         *alpha += 20;
    //         if (*alpha > 255) *alpha = 255;

    //         transitionOverlay->setStyleSheet(
    //             QString("background-color: rgba(0,0,0,%1);").arg(*alpha)
    //         );

    //         if (*alpha >= 255) {
    //             fadeTimer->stop();
    //             delete alpha;
    //             fadeTimer->deleteLater();

    //             introContainer->show();
    //             introContainer->raise();

    //             for (QFrame* line : glitchLines) {
    //                 line->show();
    //             }

    //             QTimer* glitchTimer = new QTimer(introContainer);
    //             QObject::connect(glitchTimer, &QTimer::timeout, [=]() {
    //                 static bool flip = false;
    //                 flip = !flip;

    //                 if (flip) {
    //                     cyanText->move(-7, 0);
    //                     magentaText->move(7, 0);
    //                 } else {
    //                     cyanText->move(-4, 0);
    //                     magentaText->move(4, 0);
    //                 }
    //             });
    //             glitchTimer->start(120);

    //             QTimer::singleShot(2500, [=]() {
    //                 glitchTimer->stop();
    //                 glitchTimer->deleteLater();

    //                 QGraphicsScene* level2Scene = new QGraphicsScene();
    //                 level2Scene->setSceneRect(0, 0, 800, 600);

    //                 QPixmap level2Bg("assets/closedlevel2.png");
    //                 if (level2Bg.isNull()) {
    //                     qDebug() << "ERROR: IMAGE NOT FOUND: assets/closedlevel2.png";
    //                 }

    //                 QGraphicsPixmapItem* background = level2Scene->addPixmap(level2Bg);
    //                 background->setZValue(-100);
    //                 background->setPos(0, 0);

    //                 view->setScene(level2Scene);
    //                 currentScene = level2Scene;

    //                 auto fitScene = [view, level2Scene]() {
    //                     view->fitInView(level2Scene->sceneRect(), Qt::IgnoreAspectRatio);
    //                 };
    //                 fitScene();
    //                 QTimer::singleShot(0, [fitScene]() { fitScene(); });
    //                 QTimer::singleShot(50, [fitScene]() { fitScene(); });

    //                 transitionOverlay->hide();
    //                 transitionOverlay->deleteLater();
    //             });
    //         }
    //     });

        // fadeTimer->start(35);
// });


    
    auto fitScene = [view, L1] () {
        view->fitInView(L1->getScene()->sceneRect(), Qt::IgnoreAspectRatio);
    };

    fitScene();
    QTimer::singleShot(0, [fitScene]() { fitScene(); });
    QTimer::singleShot(50, [fitScene]() { fitScene(); });

    pauseMenu* pause = new pauseMenu(view, currentLevel);
    QObject::connect(pause, &pauseMenu::leaveRequested, [view, this] () {
        showMainMenu(view, this);
    });


// ===== DEATH FADE OVERLAY =====
    QWidget* deathFadeOverlay = new QWidget(view->viewport());
    deathFadeOverlay->setGeometry(view->viewport()->rect());
    deathFadeOverlay->setStyleSheet("background-color: rgba(0,0,0,0);");
    deathFadeOverlay->hide();
    deathFadeOverlay->raise();
       
    // UI of the Game over screen.
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

    // Game over Text
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

    // Small Glitch Lines
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

    // Buttons
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

    QObject::connect(currentLevel->getPlayer(), &Player::died, [=]() {
    paused = true;

    // stop player focus
    view->clearFocus();

    // make sure overlays fill full screen
    deathFadeOverlay->setGeometry(view->viewport()->rect());
    gameOverOverlay->setGeometry(view->viewport()->rect());

    // Screen shake
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

        currentScene = new QGraphicsScene();
        QGraphicsView* newGameView = createGameView();
        newGameView->showFullScreen();

        emit gameStarted();

        view->hide();
    });
    
    QObject::connect(gameOverMenuButton, &QPushButton::clicked, [=]() {
    glitchTimer->stop();
    paused = false;

    deathFadeOverlay->hide();
    deathFadeOverlay->setStyleSheet("background-color: rgba(0,0,0,0);");

    showMainMenu(view, this);
});  

    return view;
}

void MenuWindow::resizeEvent(QResizeEvent* event) {
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

void showMainMenu(QGraphicsView* currentView, MenuWindow* menu) {
    menu->showFullScreen();
    menu->raise();
    menu->activateWindow();
    QApplication::processEvents();

    currentView->hide();
}
