#include "../include/menu_gameScene.hpp"
#include "../include/sideplayer.hpp"

bool paused = false;
QMediaPlayer* music;
QAudioOutput* audio;
double musicVolume = 0.12;
double sfxVolume = 1.0;

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
void MenuWindow::loadProgress()
{
    QSettings settings("Technoscape", "Game");
    hasStartedGame = settings.value("hasStartedGame", false).toBool();

    highestUnlockedLevel = settings.value("highestUnlockedLevel", 1).toInt();

    if (highestUnlockedLevel < 1) {
        highestUnlockedLevel = 1;
    }
}

void MenuWindow::saveProgress()
{
    QSettings settings("Technoscape", "Game");
    settings.setValue("highestUnlockedLevel", highestUnlockedLevel);
    settings.setValue("hasStartedGame", hasStartedGame);
}

void MenuWindow::unlockLevel(int level)
{
    if (level > highestUnlockedLevel) {
        highestUnlockedLevel = level;
        saveProgress();
    }
}

void MenuWindow::startLevel(int level)
{
    paused = false;
    audio->setVolume(musicVolume);

    currentLevel = level;
    currentScene = new QGraphicsScene();

    QGraphicsView* gameView = createGameView(currentScene);
    gameView->showFullScreen();
    this->hide();

    if (level == 1) {
        emit gameStarted();
        return;
    }

    if (level == 2) {
        QTimer::singleShot(0, [=]() {
            QGraphicsScene* oldScene = gameView->scene();

            QGraphicsScene* level2Scene = new QGraphicsScene();
            level2Scene->setParent(gameView);

            gameView->setScene(level2Scene);
            currentScene = level2Scene;

            if (oldScene) {
                oldScene->deleteLater();
            }

            setupLevel2Scene(level2Scene, gameView, activeGameOverScreen);
        });
        return;
    }
}

MenuWindow::MenuWindow(QGraphicsScene*& scene) : currentScene(scene) {

    // MAIN MENU DESIGN SECTION
    setWindowTitle("Technoscape");
    loadProgress();

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
    panel->setGeometry(0, 0, 450, 540);
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
    clickAudio->setVolume(sfxVolume);

    QSoundEffect* hoverPlayer = new QSoundEffect(this);
    hoverPlayer->setSource(QUrl::fromLocalFile(
    QCoreApplication::applicationDirPath() + "/assets/sounds/houver.wav"
    ));
    hoverPlayer->setVolume(sfxVolume);

    // Adding the buttons..
    QPushButton* startButton = new QPushButton("START GAME");
    QPushButton* continueButton = new QPushButton("CONTINUE GAME");
    continueButton->setVisible(hasStartedGame);
    QPushButton* settingsButton = new QPushButton("SETTINGS");
    QPushButton* howToPlayButton = new QPushButton("HOW TO PLAY");
    QPushButton* exitButton = new QPushButton("EXIT");
    HoverSoundFilter* hoverFilter = new HoverSoundFilter(hoverPlayer, this);
    startButton->installEventFilter(hoverFilter);
    settingsButton->installEventFilter(hoverFilter);
    howToPlayButton->installEventFilter(hoverFilter);
    exitButton->installEventFilter(hoverFilter);
    continueButton->installEventFilter(hoverFilter);

    QObject::connect(startButton, &QPushButton::clicked, [=]() {
        hasStartedGame = true;

        QSettings settings("Technoscape", "Game");
        settings.setValue("hasStartedGame", true);
        continueButton->setVisible(true);
        clickPlayer->stop();
        clickPlayer->setPosition(0);
        clickPlayer->play();
    });

    QObject::connect(continueButton, &QPushButton::clicked, [=]() {
    clickPlayer->stop();
    clickPlayer->setPosition(0);
    clickPlayer->play();
});

    QObject::connect(settingsButton, &QPushButton::clicked, [=]() {
        clickPlayer->stop();
        clickPlayer->setPosition(0);
        clickPlayer->play();
    });

    QObject::connect(howToPlayButton, &QPushButton::clicked, [=]() {
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
    continueButton->setStyleSheet(btnStyle);
    settingsButton->setStyleSheet(btnStyle);
    howToPlayButton->setStyleSheet(btnStyle);
    exitButton->setStyleSheet(btnStyle);

    startButton->setMinimumHeight(90);
    continueButton->setMinimumHeight(90);
    settingsButton->setMinimumHeight(90);
    howToPlayButton->setMinimumHeight(90);
    exitButton->setMinimumHeight(90);

    QGraphicsDropShadowEffect* startGlow = new QGraphicsDropShadowEffect;
    startGlow->setBlurRadius(40);
    startGlow->setColor(QColor(0, 255, 255));
    startGlow->setOffset(0, 0);
    startButton->setGraphicsEffect(startGlow);

    QGraphicsDropShadowEffect* continueGlow = new QGraphicsDropShadowEffect;
    continueGlow->setBlurRadius(40);
    continueGlow->setColor(QColor(0, 255, 255));
    continueGlow->setOffset(0, 0);
    continueButton->setGraphicsEffect(continueGlow);

    QGraphicsDropShadowEffect* settingsGlow = new QGraphicsDropShadowEffect;
    settingsGlow->setBlurRadius(40);
    settingsGlow->setColor(QColor(0, 255, 255));
    settingsGlow->setOffset(0, 0);
    settingsButton->setGraphicsEffect(settingsGlow);

    QGraphicsDropShadowEffect* howGlow = new QGraphicsDropShadowEffect;
    howGlow->setBlurRadius(40);
    howGlow->setColor(QColor(0, 255, 255));
    howGlow->setOffset(0, 0);
    howToPlayButton->setGraphicsEffect(howGlow);

    QGraphicsDropShadowEffect* exitGlow = new QGraphicsDropShadowEffect;
    exitGlow->setBlurRadius(40);
    exitGlow->setColor(QColor(0, 255, 255));
    exitGlow->setOffset(0, 0);
    exitButton->setGraphicsEffect(exitGlow);

    panelLayout->addWidget(startButton);
    panelLayout->addWidget(continueButton);
    panelLayout->addWidget(settingsButton);
    panelLayout->addWidget(howToPlayButton);
    panelLayout->addWidget(exitButton);

    // END OF MAIN MENU DESIGN SECTION

    // Events when the buttons are pressed
    QObject::connect(startButton, &QPushButton::clicked, [this]() {
        QTimer::singleShot(120, [this]() {
            paused = false;
            audio->setVolume(musicVolume);
            currentScene = new QGraphicsScene();
            QGraphicsView* gameView = createGameView(currentScene);
            gameView->showFullScreen();
            this->hide();

            // This is necessary for us to dynamically allocate the robots in the main program (main.cpp)
            emit gameStarted();
        });
    });

    QObject::connect(continueButton, &QPushButton::clicked, [this]() {
    QTimer::singleShot(120, [this]() {
        startLevel(highestUnlockedLevel);
    });
});

QObject::connect(settingsButton, &QPushButton::clicked, [=]() {
    QWidget* settingsOverlay = new QWidget(this);
    settingsOverlay->setGeometry(this->rect());
    settingsOverlay->setStyleSheet("background-color: rgba(0,0,0,210);");
    settingsOverlay->show();
    settingsOverlay->raise();

    QVBoxLayout* overlayLayout = new QVBoxLayout(settingsOverlay);
    overlayLayout->setAlignment(Qt::AlignCenter);

    QFrame* box = new QFrame(settingsOverlay);
    box->setFixedSize(560, 520);
    box->setStyleSheet(
        "QFrame { background-color: rgba(10,20,45,245);"
        "border: 3px solid #36e0ff;"
        "border-radius: 18px; }"
    );

    overlayLayout->addWidget(box);

    QVBoxLayout* layout = new QVBoxLayout(box);
    layout->setSpacing(16);
    layout->setContentsMargins(35, 30, 35, 30);

    QLabel* title = new QLabel("SETTINGS");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:white; font-size:32px; font-weight:bold; background: transparent;");

    QLabel* musicLabel = new QLabel("Music Volume");
    QLabel* sfxLabel = new QLabel("Sound Effects");
    

    QString labelStyle = "color:white; font-size:20px; font-weight:bold; background: transparent;";
    musicLabel->setStyleSheet(labelStyle);
    sfxLabel->setStyleSheet(labelStyle);
    

    QSlider* musicSlider = new QSlider(Qt::Horizontal);
    musicSlider->setRange(0, 100);
    musicSlider->setValue(audio->volume() * 100);

    QSlider* sfxSlider = new QSlider(Qt::Horizontal);
    sfxSlider->setRange(0, 100);
    sfxSlider->setValue(sfxVolume * 100);

    

    QObject::connect(musicSlider, &QSlider::valueChanged, [=](int value) {
        musicVolume = value / 100.0;
        audio->setVolume(musicVolume);
    });

    QObject::connect(sfxSlider, &QSlider::valueChanged, [=](int value) {
    sfxVolume = value / 100.0;

    hoverPlayer->setVolume(sfxVolume);
    clickAudio->setVolume(sfxVolume);
    });
    QPushButton* policyButton = new QPushButton("TERMS & POLICY");

    QPushButton* resetButton = new QPushButton("RESET PROGRESS");
    QPushButton* closeButton = new QPushButton("CLOSE");

    policyButton->setStyleSheet(btnStyle);
    resetButton->setStyleSheet(btnStyle);
    closeButton->setStyleSheet(btnStyle);


    QObject::connect(policyButton, &QPushButton::clicked, [=]() {
    QWidget* policyOverlay = new QWidget(settingsOverlay);
    policyOverlay->setGeometry(settingsOverlay->rect());
    policyOverlay->setStyleSheet("background-color: rgba(0,0,0,220);");
    policyOverlay->show();
    policyOverlay->raise();

    QVBoxLayout* policyLayout = new QVBoxLayout(policyOverlay);
    policyLayout->setAlignment(Qt::AlignCenter);

    QLabel* image = new QLabel(policyOverlay);
    QPixmap policyImg("assets/terms_policy.png");

    if (policyImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/terms_policy.png";
        image->setText("ERROR: terms_policy.png not found");
        image->setStyleSheet("color:white; font-size:28px; background: transparent;");
    } else {
        image->setPixmap(policyImg.scaled(1000, 620, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    image->setAlignment(Qt::AlignCenter);
    image->setStyleSheet("background: transparent;");

    QPushButton* backButton = new QPushButton("BACK", policyOverlay);
    backButton->setFixedSize(220, 60);
    backButton->setStyleSheet(btnStyle);

    policyLayout->addWidget(image, 0, Qt::AlignCenter);
    policyLayout->addWidget(backButton, 0, Qt::AlignCenter);

    QObject::connect(backButton, &QPushButton::clicked, [=]() {
        policyOverlay->deleteLater();
    });
});


    QObject::connect(resetButton, &QPushButton::clicked, [=]() {
        QMessageBox::StandardButton reply = QMessageBox::question(
            settingsOverlay,
            "Reset Progress",
            "Are you sure you want to reset your progress?",
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes) {
            QSettings settings("Technoscape", "Game");
            settings.clear();
            hasStartedGame = false;
            highestUnlockedLevel = 1;
            continueButton->setVisible(false);
            saveProgress();
        }
    });

    QObject::connect(closeButton, &QPushButton::clicked, [=]() {
        settingsOverlay->deleteLater();
    });

    layout->addWidget(title);
    layout->addWidget(musicLabel);
    layout->addWidget(musicSlider);
    layout->addWidget(sfxLabel);
    layout->addWidget(sfxSlider);
    layout->addWidget(policyButton);
    layout->addWidget(resetButton);
    layout->addWidget(closeButton);

    settingsOverlay->raise();
});

QObject::connect(howToPlayButton, &QPushButton::clicked, [=]() {
    QWidget* overlay = new QWidget(this);
    overlay->setGeometry(this->rect());
    overlay->setStyleSheet("background-color: rgba(0,0,0,220);");
    overlay->show();
    overlay->raise();

    QVBoxLayout* layout = new QVBoxLayout(overlay);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* image = new QLabel(overlay);
    QPixmap howImg("assets/how_to_play.png");

    if (howImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/how_to_play.png";
        image->setText("ERROR: how_to_play.png not found");
        image->setStyleSheet("color:white; font-size:28px;");
    } else {
        image->setPixmap(howImg.scaled(1000, 620, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    image->setAlignment(Qt::AlignCenter);
    image->setStyleSheet("background: transparent;");

    QPushButton* closeButton = new QPushButton("CLOSE", overlay);
    closeButton->setFixedSize(220, 60);
    closeButton->setStyleSheet(btnStyle);

    layout->addWidget(image, 0, Qt::AlignCenter);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);

    QObject::connect(closeButton, &QPushButton::clicked, [=]() {
        overlay->deleteLater();
    });

    overlay->raise();
});


    QObject::connect(exitButton, &QPushButton::clicked, []() {
        QTimer::singleShot(120, []() {
            QApplication::quit();
        });
    });

    showFullScreen();
}

// The function's purpose is to set up the scene
QGraphicsView* MenuWindow::createGameView(QGraphicsScene* scene) {

    scene->clear();
    currentLevel = 1;

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

    // auto addBox = [&](int x, int y, int w, int h) {
    //     scene->addItem(new Furniture(x, y, w, h));
    // };

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


    // w = h = 60, y= 517, x=15 (heart)
    // x 76, y = 542 (bar)
    QPixmap health_symbol_image (":/assets/health_symbol.png");
    health_symbol_image = health_symbol_image.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    QGraphicsPixmapItem* health_symbol = scene->addPixmap(health_symbol_image);
    health_symbol->setPos(15, 540);


    HealthBar* health_bar = new HealthBar;
    health_bar->setPos(76, 542);
    health_bar->setZValue(1000);


    // May the main character spawn!
    Player* player = new Player(0, 0);
    player->setHealthBar(health_bar);
    player->setPos(568, 300);
    scene->addItem(player);
    scene->addItem(health_bar);

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
    // spawnAccessKey(QPointF(500, 300));

    // WALLS
    addWall(48, 0, 723, 46);
    addWall(0, 0, 46, 346);
    addWall(0, 344, 74, 149);
    addWall(0, 494, 800, 106);
    addWall(771, 0, 29, 199);
    addWall(766, 198, 34, 296);
    addWall(582, 66, 67, 100);
    addWall(412, 66, 166, 25);
    addWall(45, 88, 22, 83);

    // DOOR
    Door* door = new Door(658, 155, 100, 25);
    scene->addItem(door);

    // TRAP
    addTrap(176, 47, 124, 14);

    QGraphicsView* view = new QGraphicsView(scene);
    scene->setParent(view);
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
    QPushButton* leaveButton = new QPushButton("LEAVE");

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

auto openPauseMenu = [=]() {
    if (paused) {
        return;
    }

    paused = true;

    if (currentLevel == 1) {
        if (player->isDead()) {
            return;
        }

        scene->setFocusItem(nullptr);
        player->clearFocus();
    }

    view->clearFocus();

    pauseOverlay->setGeometry(view->viewport()->rect());
    pauseOverlay->show();
    pauseOverlay->raise();
    pauseOverlay->setFocus();
};


auto closePauseMenu = [=]() {
    paused = false;
    pauseOverlay->hide();

    view->setFocus();

    if (currentLevel == 1) {
        scene->setFocusItem(player);
        player->setFocus();
    }
};


auto togglePauseMenu = [=]() {
    if (!paused) {
        openPauseMenu();
    } else {
        closePauseMenu();
    }
};

QObject::connect(pauseButton, &QPushButton::clicked, [=]() {
    openPauseMenu();
});

QShortcut* escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), view);
QObject::connect(escShortcut, &QShortcut::activated, [=]() {
    togglePauseMenu();
});

QObject::connect(continueButton, &QPushButton::clicked, [=]() {
    closePauseMenu();
});

QObject::connect(leaveButton, &QPushButton::clicked, [=]() {
    paused = true;
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

auto showGameOverScreen = [=]() {
    paused = true;

    view->clearFocus();

    deathFadeOverlay->setGeometry(view->viewport()->rect());
    gameOverOverlay->setGeometry(view->viewport()->rect());

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
};
activeGameOverScreen = showGameOverScreen;

QObject::connect(player, &Player::died, [=]() {
    showGameOverScreen();
});



QObject::connect(tryAgainButton, &QPushButton::clicked, [=]() {
    glitchTimer->stop();
    paused = false;

    deathFadeOverlay->hide();
    deathFadeOverlay->setStyleSheet("background-color: rgba(0,0,0,0);");
    gameOverOverlay->hide();

    int levelToRestart = currentLevel;

    if (levelToRestart == 1) {
        currentScene = new QGraphicsScene();
        QGraphicsView* newGameView = createGameView(currentScene);
        newGameView->showFullScreen();

        emit gameStarted();

        view->hide();
        view->deleteLater();
        return;
    }

    if (levelToRestart == 2) {
    QGraphicsScene* oldScene = view->scene();

    QGraphicsScene* level2Scene = new QGraphicsScene();
    level2Scene->setParent(view);

    view->setScene(level2Scene);
    currentScene = level2Scene;
    currentLevel = 2;

    if (oldScene) {
        oldScene->deleteLater();
    }

    setupLevel2Scene(level2Scene, view, showGameOverScreen);
    return;
    }
});

    QObject::connect(gameOverMenuButton, &QPushButton::clicked, [=]() {
    glitchTimer->stop();
    paused = false;

    deathFadeOverlay->hide();
    deathFadeOverlay->setStyleSheet("background-color: rgba(0,0,0,0);");

    showMainMenu(view, this);
});  

    QObject::connect(player, &Player::level2Requested, [=]() {
    paused = true;

    scene->setFocusItem(nullptr);
    player->clearFocus();
    view->clearFocus();

    QWidget* transitionOverlay = new QWidget(view->viewport());
    transitionOverlay->setGeometry(view->viewport()->rect());
    transitionOverlay->setStyleSheet("background-color: rgba(0,0,0,0);");
    transitionOverlay->show();
    transitionOverlay->raise();

    QWidget* introContainer = new QWidget(transitionOverlay);
    introContainer->setGeometry(0, 0, transitionOverlay->width(), transitionOverlay->height());
    introContainer->hide();

    QLabel* cyanText = new QLabel("LEVEL 2", introContainer);
    cyanText->setAlignment(Qt::AlignCenter);
    cyanText->setGeometry(0, 0, introContainer->width(), introContainer->height());
    cyanText->move(-4, 0);
    cyanText->setStyleSheet(
        "color: rgb(0,255,255);"
        "background: transparent;"
        "font-size: 54px;"
        "font-weight: 900;"
        "font-family: Impact, Arial Black, sans-serif;"
        "letter-spacing: 4px;"
    );

    QLabel* magentaText = new QLabel("LEVEL 2", introContainer);
    magentaText->setAlignment(Qt::AlignCenter);
    magentaText->setGeometry(0, 0, introContainer->width(), introContainer->height());
    magentaText->move(4, 0);
    magentaText->setStyleSheet(
        "color: rgb(255,0,200);"
        "background: transparent;"
        "font-size: 54px;"
        "font-weight: 900;"
        "font-family: Impact, Arial Black, sans-serif;"
        "letter-spacing: 4px;"
    );

    QLabel* mainText = new QLabel("LEVEL 2", introContainer);
    mainText->setAlignment(Qt::AlignCenter);
    mainText->setGeometry(0, 0, introContainer->width(), introContainer->height());
    mainText->setStyleSheet(
        "color: white;"
        "background: transparent;"
        "font-size: 54px;"
        "font-weight: 900;"
        "font-family: Impact, Arial Black, sans-serif;"
        "letter-spacing: 4px;"
    );

    QVector<QFrame*> glitchLines;
    for (int i = 0; i < 18; i++) {
        QFrame* line = new QFrame(introContainer);
        int x = rand() % 700;
        int y = 80 + rand() % 420;
        int w = 30 + rand() % 120;
        int h = 2 + rand() % 2;
        line->setGeometry(x, y, w, h);

        if (i % 3 == 0) {
            line->setStyleSheet("background-color: rgba(0,255,255,120); border: none;");
        } else if (i % 3 == 1) {
            line->setStyleSheet("background-color: rgba(255,0,200,120); border: none;");
        } else {
            line->setStyleSheet("background-color: rgba(255,255,255,60); border: none;");
        }

        line->hide();
        glitchLines.push_back(line);
    }

    QTimer* fadeTimer = new QTimer(transitionOverlay);
    int* alpha = new int(0);

    QObject::connect(fadeTimer, &QTimer::timeout, [=]() mutable {
        *alpha += 20;
        if (*alpha > 255) *alpha = 255;

        transitionOverlay->setStyleSheet(
            QString("background-color: rgba(0,0,0,%1);").arg(*alpha)
        );

        if (*alpha >= 255) {
            fadeTimer->stop();
            delete alpha;
            fadeTimer->deleteLater();

            introContainer->show();
            introContainer->raise();

            for (QFrame* line : glitchLines) {
                line->show();
            }

            QTimer* glitchTimer = new QTimer(introContainer);
            QObject::connect(glitchTimer, &QTimer::timeout, [=]() {
                static bool flip = false;
                flip = !flip;

                if (flip) {
                    cyanText->move(-7, 0);
                    magentaText->move(7, 0);
                } else {
                    cyanText->move(-4, 0);
                    magentaText->move(4, 0);
                }
            });
            glitchTimer->start(120);

            QTimer::singleShot(2500, [=]() {
                glitchTimer->stop();
                glitchTimer->deleteLater();

                QGraphicsScene* level2Scene = new QGraphicsScene();

                view->setScene(level2Scene);
                currentScene = level2Scene;

                currentLevel = 2;
                unlockLevel(2);

                setupLevel2Scene(level2Scene, view, showGameOverScreen);

                transitionOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                transitionOverlay->hide();
                transitionOverlay->deleteLater();
            });
        }
    });

    fadeTimer->start(35);
});



    return view;
}

void MenuWindow::setupLevel2Scene(QGraphicsScene* scene, QGraphicsView* view, std::function<void()> showGameOverScreen)
{
    scene->setParent(view);
    
    scene->clear();
    scene->setSceneRect(0, 0, 800, 600);
    currentLevel = 2;

    QPixmap level2Bg("assets/closedlevel2.png");
    if (level2Bg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/closedlevel2.png";
    }

    QGraphicsPixmapItem* background = scene->addPixmap(level2Bg);
    background->setZValue(-100);
    background->setPos(0, 0);

    auto addWall = [&](int x, int y, int w, int h) -> Wall* {
        Wall* wall = new Wall(x, y, w, h);
        scene->addItem(wall);
        return wall;
    };

    // =========================
    // LEVEL 2 COLLISION SHELL
    // =========================

    addWall(0,   417, 167, 183);   // left ground
    addWall(260, 417, 540, 183);   // right ground

    addWall(0,   0, 20, 600);      // left wall
    addWall(780, 0, 20, 600);      // right wall
    addWall(0,   0, 800, 20);      // ceiling

    // =========================
// TRAP 1 : FAKE FLOOR
// hole = x 167, y 416, w 93, h 184
// =========================

QPixmap fakeFloorImg("assets/fake_floor_panel.png");
if (fakeFloorImg.isNull()) {
    qDebug() << "ERROR: IMAGE NOT FOUND: assets/fake_floor_panel.png";
}

QGraphicsPixmapItem* fakeFloorSprite = scene->addPixmap(fakeFloorImg);
fakeFloorSprite->setPos(167, 416);
fakeFloorSprite->setZValue(20);

Wall** fakeFloorCollision = new Wall*(nullptr);
*fakeFloorCollision = new Wall(167, 417, 93, 20);
scene->addItem(*fakeFloorCollision);

QGraphicsRectItem* triggerZone = new QGraphicsRectItem(169, 414, 93, 35);
triggerZone->setPen(Qt::NoPen);
triggerZone->setBrush(Qt::NoBrush);
triggerZone->setZValue(10);
scene->addItem(triggerZone);

QGraphicsRectItem* killZone = new QGraphicsRectItem(167, 573, 88, 13);
killZone->setPen(Qt::NoPen);
killZone->setBrush(Qt::NoBrush);
killZone->setZValue(5);
scene->addItem(killZone);

QGraphicsRectItem* laserEffect = new QGraphicsRectItem(170, 501, 110, 120);
laserEffect->setPen(Qt::NoPen);
laserEffect->setBrush(QColor(255, 0, 0, 0));
laserEffect->setZValue(50);
scene->addItem(laserEffect);

QGraphicsBlurEffect* glow = new QGraphicsBlurEffect;
glow->setBlurRadius(25);
laserEffect->setGraphicsEffect(glow);

bool* trap1Triggered = new bool(false);
bool* trap1Open = new bool(false);
bool* trap1CoolingDown = new bool(false);
bool* trap1PlayerDead = new bool(false);
bool* trap1DeathSequenceRunning = new bool(false);

// temporary test player
SidePlayer* testPlayer = new SidePlayer();
testPlayer->setPos(90, 340);
scene->addItem(testPlayer);
scene->setFocusItem(testPlayer);
testPlayer->setFocus();


// =========================
// TRAP 2 : SPIKE CHASE
// =========================

// bait item that attracts the player
QPixmap baitImg("assets/key.gif");
QGraphicsPixmapItem* baitItem = scene->addPixmap(
    baitImg.scaled(90, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation)
);
baitItem->setPos(650, 350);
baitItem->setZValue(100);

QGraphicsDropShadowEffect* keyGlow = new QGraphicsDropShadowEffect;
keyGlow->setBlurRadius(45);
keyGlow->setColor(QColor(255, 220, 60, 220));
keyGlow->setOffset(0, 0);
baitItem->setGraphicsEffect(keyGlow);

QGraphicsRectItem* fakeKeyCollectZone = new QGraphicsRectItem(610, 300, 150, 140);
fakeKeyCollectZone->setPen(Qt::NoPen);
fakeKeyCollectZone->setBrush(Qt::NoBrush);
fakeKeyCollectZone->setZValue(99);
scene->addItem(fakeKeyCollectZone);

QGraphicsTextItem* fakeKeyText = scene->addText("Press C to collect");
fakeKeyText->setDefaultTextColor(Qt::white);
fakeKeyText->setFont(QFont("Arial", 16, QFont::Bold));
fakeKeyText->setPos(610, 315);
fakeKeyText->setZValue(1000);
fakeKeyText->hide();

KeyItem* realLevel2Key = new KeyItem(
    QCoreApplication::applicationDirPath() + "/assets/key.gif",
    60, 90
);
realLevel2Key->setPos(85, 320);
realLevel2Key->setZValue(900);
realLevel2Key->hide();
scene->addItem(realLevel2Key);

bool* fakeKeyCollected = new bool(false);
bool* trap3Finished = new bool(false);
QVector<QTimer*>* droneTimers = new QVector<QTimer*>();

QTimer* keyFloatTimer = new QTimer(scene);
int* keyFloatStep = new int(0);

QObject::connect(keyFloatTimer, &QTimer::timeout, [=]() mutable {
    (*keyFloatStep)++;

    qreal offset = qSin((*keyFloatStep) * 0.15) * 2.5;
    baitItem->setY(350 + offset);

    qreal opacity = 0.85 + (qSin((*keyFloatStep) * 0.18) * 0.15);
    baitItem->setOpacity(opacity);
});

keyFloatTimer->start(30);


// trigger zone after trap 1
QGraphicsRectItem* trap2Trigger = new QGraphicsRectItem(580, 320, 50, 60);
trap2Trigger->setPen(Qt::NoPen);
trap2Trigger->setBrush(Qt::NoBrush);
trap2Trigger->setZValue(10);
scene->addItem(trap2Trigger);

// spike visual
QPixmap spikeImg("assets/spike_wall.png");
if (spikeImg.isNull()) {
    qDebug() << "ERROR: IMAGE NOT FOUND: assets/spike_wall.png";
}

QGraphicsPixmapItem* spikeWall = scene->addPixmap(
spikeImg.scaled(95, 165, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
);
spikeWall->setPos(950, 252);
spikeWall->setZValue(400);
spikeWall->hide();

// invisible kill hitbox for spike
QGraphicsRectItem* spikeHitbox = new QGraphicsRectItem(880, 252, 55, 165);
spikeHitbox->setPen(Qt::NoPen);
spikeHitbox->setBrush(Qt::NoBrush);
spikeHitbox->setZValue(401);
spikeHitbox->hide();
scene->addItem(spikeHitbox);

bool* trap2Triggered = new bool(false);
bool* trap2Active = new bool(false);

// =========================
// TRAP 3 : DRONE LASER GAUNTLET
// =========================

QPixmap droneImg("assets/drone.png");
if (droneImg.isNull()) {
    qDebug() << "ERROR: IMAGE NOT FOUND: assets/drone.png";
}

QVector<QGraphicsPixmapItem*> drones;
QVector<QGraphicsRectItem*> droneLasers;
QVector<bool*> droneLaserOn;

QVector<int> droneX = {275, 375, 475, 575};
QVector<int> laserIntervals = {750, 950, 650, 850};

for (int i = 0; i < 4; i++) {
    QGraphicsPixmapItem* drone = scene->addPixmap(
        droneImg.scaled(65, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation)
    );

    drone->setPos(droneX[i], -80);
    drone->setZValue(500);
    drone->hide();
    drones.push_back(drone);

    QGraphicsDropShadowEffect* droneGlow = new QGraphicsDropShadowEffect;
    droneGlow->setBlurRadius(35);
    droneGlow->setColor(QColor(255, 0, 0, 180));
    droneGlow->setOffset(0, 0);
    drone->setGraphicsEffect(droneGlow);

    QGraphicsRectItem* laser = new QGraphicsRectItem(droneX[i] + 27, 135, 10, 282);
    laser->setPen(Qt::NoPen);
    laser->setBrush(QColor(255, 0, 0, 0));
    laser->setZValue(450);
    laser->hide();
    scene->addItem(laser);

    QGraphicsBlurEffect* laserGlow = new QGraphicsBlurEffect;
    laserGlow->setBlurRadius(18);
    laser->setGraphicsEffect(laserGlow);

    droneLasers.push_back(laser);
    droneLaserOn.push_back(new bool(false));
}

bool* trap3Started = new bool(false);

QShortcut* collectShortcut = new QShortcut(QKeySequence(Qt::Key_C), view);
collectShortcut->setContext(Qt::ApplicationShortcut);

QObject::connect(collectShortcut, &QShortcut::activated, [=]() {
    if (paused || !scene || !scene->views().size()) return;
    if (!testPlayer || !testPlayer->scene() || paused) return;
    if (!(*trap3Started)) return;
    if (*fakeKeyCollected) return;
    if (!testPlayer->sceneBoundingRect().intersects(fakeKeyCollectZone->sceneBoundingRect())) return;
    *fakeKeyCollected = true;
    *trap3Finished = true;

    baitItem->hide();
    fakeKeyText->hide();

    for (int i = 0; i < drones.size(); i++) {
        drones[i]->hide();
        droneLasers[i]->hide();
        droneLasers[i]->setBrush(QColor(255, 0, 0, 0));
        *droneLaserOn[i] = false;
    }

        for (QTimer* timer : *droneTimers) {
            if (timer) {
                timer->stop();
                timer->deleteLater();
            }
        }
        droneTimers->clear();
        realLevel2Key->show();
});


// sound
QMediaPlayer* hoverTrapSound = new QMediaPlayer(scene);
QAudioOutput* hoverTrapAudio = new QAudioOutput(scene);
hoverTrapSound->setAudioOutput(hoverTrapAudio);
hoverTrapSound->setSource(QUrl::fromLocalFile(
    QCoreApplication::applicationDirPath() + "/assets/sounds/houver.wav"
));
hoverTrapAudio->setVolume(sfxVolume);
QMediaPlayer* laserSound = new QMediaPlayer(scene);
QAudioOutput* laserAudio = new QAudioOutput(scene);
laserSound->setAudioOutput(laserAudio);
laserSound->setSource(QUrl::fromLocalFile(

QCoreApplication::applicationDirPath() + "/assets/sounds/trap_trigger.wav"
));
laserAudio->setVolume(sfxVolume);

QTimer* trap1LogicTimer = new QTimer(scene);

QObject::connect(trap1LogicTimer, &QTimer::timeout, [=]() {
    if (paused || !scene || !scene->views().size()) return;
    if (*trap1PlayerDead) return;
    if (!testPlayer || !testPlayer->scene() || paused) return;

    // trigger trap only if closed and not already running
    if (!(*trap1Triggered) && !(*trap1Open) && !(*trap1CoolingDown) &&
        testPlayer->collidesWithItem(triggerZone)) {

        *trap1Triggered = true;

        hoverTrapSound->stop();
        hoverTrapSound->setPosition(0);
        hoverTrapSound->play();
       

        // warning flash for 0.5 sec
        QTimer* warningTimer = new QTimer(scene);
        int* flashStep = new int(0);


        // open after 0.5 sec
        QTimer::singleShot(250, scene, [=]() {
            if (!testPlayer || !testPlayer->scene() || paused) return;
            *trap1Open = true;

            if (*fakeFloorCollision && (*fakeFloorCollision)->scene()) {
                scene->removeItem(*fakeFloorCollision);
                delete *fakeFloorCollision;
                *fakeFloorCollision = nullptr;
            }

            // floor drops
            QTimer* fallAnim = new QTimer(scene);
            QObject::connect(fallAnim, &QTimer::timeout, [=]() {
                if (paused || !scene || !scene->views().size()) return;
                if (!fakeFloorSprite || !fakeFloorSprite->scene()) {
                    fallAnim->stop();
                    fallAnim->deleteLater();
                    return;
                }

                fakeFloorSprite->moveBy(0, 10);

                if (fakeFloorSprite->y() > 650) {
                    fakeFloorSprite->hide();
                    fallAnim->stop();
                    fallAnim->deleteLater();
                }
            });
            fallAnim->start(16);

            // keep trap open for 5 sec, then reset
            QTimer::singleShot(2500, scene, [=]() {
                if (!testPlayer || !testPlayer->scene() || paused) return;
                *trap1CoolingDown = true;
                
                fakeFloorSprite->show();
                fakeFloorSprite->setPos(167, 416);

                if (!(*fakeFloorCollision)) {
                    *fakeFloorCollision = new Wall(167, 417, 93, 20);
                    scene->addItem(*fakeFloorCollision);
                }

                *trap1Triggered = false;
                *trap1Open = false;
                *trap1CoolingDown = false;
            });
        });
    }

    
if (*trap1Open && !(*trap1PlayerDead) && !(*trap1DeathSequenceRunning) &&
    testPlayer->collidesWithItem(killZone)) {

    *trap1PlayerDead = true;
    *trap1DeathSequenceRunning = true;

    testPlayer->setFrozen(true);

    // let the player stay visible in the shaft for 1 second
    QTimer::singleShot(250, scene, [=]() {
        if (paused || !testPlayer || !testPlayer->scene()) return;
        // laser effect for 0.5 seconds
        laserSound->stop();
        laserSound->setPosition(0);
        laserSound->play();


        laserEffect->setBrush(QColor(255, 0, 0, 255));
        laserEffect->setScale(1.1);

        QTimer* laserTimer = new QTimer(scene);
        int* laserStep = new int(0);

        QObject::connect(laserTimer, &QTimer::timeout, [=]() mutable {
            if (paused || !testPlayer || !testPlayer->scene()) return;
            (*laserStep)++;

            int intensity = (*laserStep % 2 == 0) ? 255 : 180;

            laserEffect->setBrush(QColor(255, 0, 0, intensity));

            qreal scale = (intensity == 255) ? 1.05 : 1.0;
            laserEffect->setScale(scale);

            if (*laserStep >= 6) {
                laserTimer->stop();
                laserEffect->setBrush(QColor(255, 0, 0, 0));
                delete laserStep;
                laserTimer->deleteLater();

                emit testPlayer->died();
            }
        });

        laserTimer->start(80);
    });

    return;
}
// =========================
// TRAP 2 LOGIC
// =========================
if (!(*trap2Triggered) && testPlayer->collidesWithItem(trap2Trigger)) {
    *trap2Triggered = true;

    hoverTrapSound->stop();
    hoverTrapSound->setPosition(0);
    hoverTrapSound->play();

    QTimer::singleShot(200, scene, [=]() {
        if (paused || !testPlayer || !testPlayer->scene()) return;
        *trap2Active = true;

        spikeWall->setPos(850, 252);
        spikeHitbox->setRect(880, 252, 55, 165);

        spikeWall->show();
        spikeHitbox->show();

        QTimer* spikeMoveTimer = new QTimer(scene);
        int* spikeStep = new int(0);

        QObject::connect(spikeMoveTimer, &QTimer::timeout, [=]() mutable {
            if (paused || !scene || !scene->views().size()) return;
            (*spikeStep)++;

            int speed = 5;
            if (*spikeStep > 30) speed = 6;
            if (*spikeStep > 60) speed = 7;

            spikeWall->moveBy(-speed, 0);
            spikeHitbox->moveBy(-speed, 0);

            if (!(*trap1PlayerDead) && testPlayer->collidesWithItem(spikeHitbox)) {
                *trap1PlayerDead = true;

                testPlayer->setFrozen(true);

                laserSound->stop();
                laserSound->setPosition(0);
                laserSound->play();

                QTimer::singleShot(450, scene, [=]() {
                    if (paused || !testPlayer || !testPlayer->scene()) return;
                    emit testPlayer->died();
                });

                spikeMoveTimer->stop();
                delete spikeStep;
                spikeMoveTimer->deleteLater();
                return;
            }

            // spike disappears near fake floor
    if (spikeWall->x() <= 260) {
        spikeHitbox->hide();

        QTimer* vanishTimer = new QTimer(scene);
        int* vanishStep = new int(0);

        QObject::connect(vanishTimer, &QTimer::timeout, [=]() mutable {
            if (paused || !scene || !scene->views().size()) return;
            (*vanishStep)++;

            spikeWall->setOpacity(1.0 - (*vanishStep * 0.15));
            spikeWall->moveBy(0, -3);

            if (*vanishStep >= 7) {
                spikeWall->hide();
                spikeWall->setOpacity(1.0);
                spikeWall->setPos(850, 252);

                vanishTimer->stop();
                delete vanishStep;
                vanishTimer->deleteLater();
            }
        });

        vanishTimer->start(30);

        *trap2Active = false;

        if (!(*trap3Started)) {
            *trap3Started = true;

            for (int i = 0; i < drones.size(); i++) {
                drones[i]->show();

                QTimer* dropTimer = new QTimer(scene);
                QObject::connect(dropTimer, &QTimer::timeout, [=]() {
                    if (paused || !scene || !scene->views().size()) return;
                    if (!testPlayer || !testPlayer->scene()) return;

                    drones[i]->moveBy(0, 5);

                    if (drones[i]->y() >= 90) {
                        drones[i]->setY(90);
                        dropTimer->stop();
                        dropTimer->deleteLater();

                        droneLasers[i]->show();

                        QTimer* droneTimer = new QTimer(scene);
                        int* phase = new int(0);
                        droneTimers->push_back(droneTimer);

                        QObject::connect(droneTimer, &QTimer::timeout, [=]() mutable {
                         if (*trap3Finished) {
                            droneLasers[i]->setBrush(QColor(255, 0, 0, 0));
                            *droneLaserOn[i] = false;
                            droneTimer->stop();
                            return;
                        }
                            if (paused || !scene || !scene->views().size()) return;
                            if (!testPlayer || !testPlayer->scene()) return;

                        (*phase)++;

                        if (*phase % 3 == 1) {
                            // VERY short warning
                            droneLasers[i]->setBrush(QColor(255, 80, 80, 40));
                            *droneLaserOn[i] = false;
                        }
                        else if (*phase % 3 == 2) {
                            // laser ON
                            droneLasers[i]->setBrush(QColor(255, 0, 0, 255));
                            *droneLaserOn[i] = true;
                        }
                        else {
                            // OFF
                            droneLasers[i]->setBrush(QColor(255, 0, 0, 0));
                            *droneLaserOn[i] = false;
                        }
                        });

                        droneTimer->start(laserIntervals[i]);
                    }
                });

                dropTimer->start(16);
            }
        }

        spikeMoveTimer->stop();
        delete spikeStep;
        spikeMoveTimer->deleteLater();
        return;
    }
        });

        spikeMoveTimer->start(16);
    });
}
// =========================
// TRAP 3 KILL LOGIC
// =========================
if (*trap3Started && !(*trap1PlayerDead)) {
    for (int i = 0; i < droneLasers.size(); i++) {
        if (*droneLaserOn[i] && testPlayer->collidesWithItem(droneLasers[i])) {
            *trap1PlayerDead = true;
            testPlayer->setFrozen(true);

            laserSound->stop();
            laserSound->setPosition(0);
            laserSound->play();

            QTimer::singleShot(250, scene, [=]() {
                if (paused || !testPlayer || !testPlayer->scene()) return;
                emit testPlayer->died();
            });

            return;
        }
    }
}
// =========================
// FAKE KEY INTERACTION TEXT
// =========================
if (*trap3Started && !(*fakeKeyCollected) && baitItem->isVisible()) {
    if (testPlayer->sceneBoundingRect().intersects(fakeKeyCollectZone->sceneBoundingRect())) {
        fakeKeyText->show();
    } else {
        fakeKeyText->hide();
    }
}
});
    trap1LogicTimer->start(16);

    // =========================
    // LEVEL 2 DOOR
    // interactive later
    // =========================
    Door* level2Door = new Door(57, 320, 58, 96);
    scene->addItem(level2Door);

    QObject::connect(testPlayer, &SidePlayer::died, [=]() {
        qDebug() << "Trap 1 death triggered";

        paused = true;
        testPlayer->setFrozen(true);


        testPlayer->playerDied();

        QTimer::singleShot(800, scene, [=]() {
            testPlayer->hide();             // testPlayer automatically gets cleaned up with the scene since it's a child of the scene, so just hide it to avoid segmentation error.
            if (showGameOverScreen) showGameOverScreen();
        });
    });
    
    auto fitScene = [view, scene]() {
        view->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
    };

    fitScene();
    QTimer::singleShot(0, [fitScene]() { fitScene(); });
    QTimer::singleShot(50, [fitScene]() { fitScene(); });

    paused = false;
    view->setFocusPolicy(Qt::StrongFocus);
    view->setFocus();
    view->activateWindow();
}

void MenuWindow::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    if (background) {
        background->setGeometry(0, 0, width(), height());
    }
    if (panel) {
        int panelW = 450;
        int panelH = 540;

        int panelX = (width() - panelW) / 2;
        int panelY = (height() - panelH) / 2 + 80;

        panel->setGeometry(panelX, panelY, panelW, panelH);

        if (title) {
            title->setGeometry(panelX - 90, panelY - 270, panelW + 180, 229);
        }
    }
}
void showMainMenu(QGraphicsView* currentView, MenuWindow* menu) {
    paused = true;

    menu->showFullScreen();
    menu->raise();
    menu->activateWindow();

    if (currentView) {
        currentView->setEnabled(false);
        currentView->hide();
        currentView->close();
        currentView->deleteLater();
    }
}