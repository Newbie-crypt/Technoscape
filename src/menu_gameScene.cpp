#include "menu_gameScene.hpp"
#include <thread>                                                                                                                            
#include <chrono>


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

void MenuWindow::loadProgress() {
    QSettings settings("Technoscape", "Game");
    hasStartedGame = settings.value("hasStartedGame", false).toBool();
    highestUnlockedLevel = settings.value("highestUnlockedLevel", 1).toInt();
    if (highestUnlockedLevel < 1) highestUnlockedLevel = 1;
}

void MenuWindow::saveProgress() {
    QSettings settings("Technoscape", "Game");
    settings.setValue("highestUnlockedLevel", highestUnlockedLevel);
    settings.setValue("hasStartedGame", hasStartedGame);
}

void MenuWindow::unlockLevel(int level) {
    if (level > highestUnlockedLevel) {
        highestUnlockedLevel = level;
        saveProgress();
    }
}

void MenuWindow::startLevel(int level) {
    paused = false;
    audio->setVolume(musicVolume);

    currentLevelNumber = level;

    QGraphicsView* gameView = nullptr;

    if (level == 1) {
        gameView = createGameView(new levelOne(nullptr));
    }

    if (level == 2) {
        gameView = createGameView(new levelTwo);
    }

    if (level == 3) {
        gameView = createGameView(new levelThree);
    }

    if (level == 4) {
        gameView = createGameView(new levelFour);
    }
    if (level == 5) {
        gameView = createGameView(new bossFight);
    }

    if (!gameView) return;

    gameView->showFullScreen();
    gameView->raise();
    gameView->activateWindow();
    gameView->repaint();
    QApplication::processEvents();

    QTimer::singleShot(100, this, [this, gameView]() {
        this->hide();

        gameView->setFocus();
        gameView->activateWindow();

        if (Player* p = currentLevel->getPlayer()) {
            currentLevel->getScene()->setFocusItem(p);
            p->setFocus();
        } else if (levelTwo* L2 = dynamic_cast<levelTwo*>(currentLevel)) {
            currentLevel->getScene()->setFocusItem(L2->getSidePlayer());
            L2->getSidePlayer()->setFocus();
        } else if (levelFour* L4 = dynamic_cast<levelFour*>(currentLevel)) {
            currentLevel->getScene()->setFocusItem(L4->getSidePlayer());
            L4->getSidePlayer()->setFocus();
        }
    });

    emit gameStarted();
    

}

MenuWindow::MenuWindow() {


    // MAIN MENU DESIGN SECTION
    setWindowTitle("Technoscape");
    loadProgress();

    background = new QLabel(this);
    QPixmap bg(":/assets/menu_bg.png");

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
    clickPlayer->setSource(QUrl("qrc:/assets/sounds/click.wav"));
    clickAudio->setVolume(sfxVolume);

    QSoundEffect* hoverPlayer = new QSoundEffect(this);
    hoverPlayer->setSource(QUrl(
    "qrc:/assets/sounds/houver.wav"
    ));
    hoverPlayer->setVolume(sfxVolume);

    // Adding the buttons..
    QPushButton* startButton = new QPushButton("START GAME");
    continueButton = new QPushButton("CONTINUE GAME");
    continueButton->setVisible(hasStartedGame);
    QPushButton* settingsButton = new QPushButton("SETTINGS");
    QPushButton* howToPlayButton = new QPushButton("HOW TO PLAY");
    QPushButton* exitButton = new QPushButton("EXIT");
    HoverSoundFilter* hoverFilter = new HoverSoundFilter(hoverPlayer, this);
    startButton->installEventFilter(hoverFilter);
    continueButton->installEventFilter(hoverFilter);
    settingsButton->installEventFilter(hoverFilter);
    howToPlayButton->installEventFilter(hoverFilter);
    exitButton->installEventFilter(hoverFilter);

    auto playClick = [=]() {
        clickAudio->setVolume(sfxVolume);
        clickPlayer->stop();
        clickPlayer->setPosition(0);
        clickPlayer->play();
    };

    QObject::connect(startButton, &QPushButton::clicked, [=]() {
        playClick();

        QSettings settings("Technoscape", "Game");

        hasStartedGame = true;
        settings.setValue("hasStartedGame", true);
        continueButton->setVisible(true);

        bool introPlayed = settings.value("introPlayed", false).toBool();

        if (introPlayed) {
            QTimer::singleShot(120, this, [this]() {
                startLevel(1);
            });
        } else {
            playIntroVideo();
        }
    });

    QObject::connect(continueButton, &QPushButton::clicked, playClick);
    QObject::connect(settingsButton, &QPushButton::clicked, playClick);
    QObject::connect(howToPlayButton, &QPushButton::clicked, playClick);
    QObject::connect(exitButton, &QPushButton::clicked, playClick);

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

    auto applyGlow = [](QPushButton* button) {
        QGraphicsDropShadowEffect* glow = new QGraphicsDropShadowEffect;
        glow->setBlurRadius(40);
        glow->setColor(QColor(0, 255, 255));
        glow->setOffset(0, 0);
        button->setGraphicsEffect(glow);
    };
    applyGlow(startButton);
    applyGlow(continueButton);
    applyGlow(settingsButton);
    applyGlow(howToPlayButton);
    applyGlow(exitButton);

    panelLayout->addWidget(startButton);
    panelLayout->addWidget(continueButton);
    panelLayout->addWidget(settingsButton);
    panelLayout->addWidget(howToPlayButton);
    panelLayout->addWidget(exitButton);

    // END OF MAIN MENU DESIGN SECTION

    // Continue saved progress: jumps to the highest level the player unlocked.
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

        QLabel* settingsTitle = new QLabel("SETTINGS");
        settingsTitle->setAlignment(Qt::AlignCenter);
        settingsTitle->setStyleSheet("color:white; font-size:32px; font-weight:bold; background: transparent;");

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

        QObject::connect(musicSlider, &QSlider::valueChanged, [](int value) {
            musicVolume = value / 100.0;
            audio->setVolume(musicVolume);
        });

        // Just update the global; play sites apply sfxVolume each time, so
        // we don't hammer setVolume on every drag tick (which crashed the
        // QSoundEffect backend on first game start).
        QObject::connect(sfxSlider, &QSlider::valueChanged, [](int value) {
            sfxVolume = value / 100.0;
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
            QPixmap policyImg(":/assets/terms_policy.png");

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

        layout->addWidget(settingsTitle);
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
        QPixmap howImg(":/assets/how_to_play.png");

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

void MenuWindow::playLevelTransition(QGraphicsView* gameView, int level) {
    paused = true;

    QWidget* transitionOverlay = new QWidget(gameView->viewport());
    transitionOverlay->setGeometry(gameView->viewport()->rect());
    transitionOverlay->setStyleSheet("background-color: rgba(0,0,0,0);");
    transitionOverlay->show();
    transitionOverlay->raise();

    QWidget* introContainer = new QWidget(transitionOverlay);
    introContainer->setGeometry(0, 0, transitionOverlay->width(), transitionOverlay->height());
    introContainer->hide();
    int nextLevel = level + 1;
    QString text = "LEVEL " + QString::number(nextLevel);
    QLabel* cyanText = new QLabel(text, introContainer);
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

    QLabel* magentaText = new QLabel(text, introContainer);
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

    QLabel* mainText = new QLabel(text, introContainer);
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

            for (QFrame* line : glitchLines) line->show();

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

                gameLevel* oldLevel = currentLevel;
                QGraphicsView* oldView = gameView;

                // Build and present the new fullscreen view BEFORE retiring the
                // old one — the new fullscreen window will cover the old, so the
                // desktop never flashes between the two.

                unlockLevel(nextLevel);
                startLevel(nextLevel);

                oldView->lower();

               QTimer::singleShot(150, oldView, [oldView]() {
               oldView->hide();
               oldView->close();
               oldView->deleteLater();
              });
                if (oldLevel) oldLevel->deleteLater();

                transitionOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                transitionOverlay->deleteLater();
            });
        }
    });

    fadeTimer->start(35);
}

// The function's purpose is to set up the scene
QGraphicsView* MenuWindow::createGameView(gameLevel* inputLevel) {
    view = new QGraphicsView;
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(0);
    view->setBackgroundBrush(Qt::black);
    view->setAlignment(Qt::AlignCenter);

    // ORIGINAL LEVEL SELECTION STRUCTURE
    if (levelOne* L1 = dynamic_cast<levelOne*>(inputLevel)) {
        currentLevel = L1;
        currentLevel->setView(view);
        currentLevel->setupScene();
        L1->spawnEnemies();
        currentLevel->setupSpawnKeyEvent();
    } 
    else if (levelTwo* L2 = dynamic_cast<levelTwo*>(inputLevel)) {
        currentLevel = L2;
        currentLevel->setView(view);
        currentLevel->setupScene();
    } 
    else if (levelThree* L3 = dynamic_cast<levelThree*>(inputLevel)) {
        currentLevel = L3;
        currentLevel->setView(view);
        currentLevel->setupScene();
    }
    else if (levelFour* L4 = dynamic_cast<levelFour*>(inputLevel)) {
        currentLevel = L4;
        currentLevel->setView(view);
        currentLevel->setupScene();
    }
    else if (bossFight* L5 = dynamic_cast<bossFight*>(inputLevel)) {
        currentLevel = L5;
        currentLevel->setView(view);
        currentLevel->setupScene();
        QObject::connect(L5, &gameLevel::levelComplete, [this, L5]() {
            audio->setVolume(0);
            for (QWidget* child : view->findChildren<QWidget*>()) child->hide();
            playOutroVideo(this->view);
        });
    }

    view->setScene(currentLevel->getScene());
    view->showFullScreen();

    view->fitInView(200, 200, 800, 600);

    QTimer::singleShot(0, [this]() {
        this->view->setFocus();

        if (Player* p = currentLevel->getPlayer()) {
            currentLevel->getScene()->setFocusItem(p);
            p->setFocus();
        } 
        else if (levelTwo* L2 = dynamic_cast<levelTwo*>(currentLevel)) {
            currentLevel->getScene()->setFocusItem(L2->getSidePlayer());
            L2->getSidePlayer()->setFocus();
        } 
        else if (levelFour* L4 = dynamic_cast<levelFour*>(currentLevel)) {
            currentLevel->getScene()->setFocusItem(L4->getSidePlayer());
            L4->getSidePlayer()->setFocus();
        }
    });

    pauseMenu* pause = new pauseMenu(view, currentLevel);
    QObject::connect(pause, &pauseMenu::leaveRequested, [this]() {
        gameLevel* oldLevel = currentLevel;

        showMainMenu(this->view, this);

        if (oldLevel) {
            QTimer::singleShot(600, oldLevel, [oldLevel]() {
                oldLevel->deleteLater();
            });
        }
    });

    gameOver* death_screen = new gameOver(view, currentLevel);

    QObject::connect(death_screen, &gameOver::tryAgainRequested, [this]() {
        int restartTo = currentLevelNumber;

        gameLevel* oldLevel = currentLevel;
        QGraphicsView* oldView = this->view;

        QWidget* blackCover = nullptr;

        if (oldView) {
            oldView->setEnabled(false);

            blackCover = new QWidget(oldView);
            blackCover->setGeometry(oldView->rect());
            blackCover->setStyleSheet("background-color: black;");
            blackCover->show();
            blackCover->raise();

            oldView->raise();
            oldView->activateWindow();
            oldView->repaint();
            QApplication::processEvents();
            oldView->setScene(nullptr);
        }

        if (oldLevel) {
            delete oldLevel;
            oldLevel = nullptr;
        }

        startLevel(restartTo);

        if (oldView) {
            QTimer::singleShot(250, oldView, [oldView]() {
                oldView->hide();
                oldView->close();
                oldView->deleteLater();
            });
        }
    });

    QObject::connect(death_screen, &gameOver::mainMenuRequested, [this]() {
        gameLevel* oldLevel = currentLevel;

        showMainMenu(this->view, this);

        if (oldLevel) {
            QTimer::singleShot(600, oldLevel, [oldLevel]() {
                oldLevel->deleteLater();
            });
        }
    });

    // add a transition here when adding a level
    // Level 1 -> Level 2
    if (Player* player = currentLevel->getPlayer()) {
        QObject::connect(player, &Player::level2Requested, [this]() {
            playLevelTransition(this->view, currentLevelNumber);
        });
    }

    // Level 2 -> Level 3
    if (levelTwo* L2 = dynamic_cast<levelTwo*>(currentLevel)) {
        QObject::connect(L2->getSidePlayer(), &SidePlayer::enterDoorRequested, [this]() {
            playLevelTransition(this->view, currentLevelNumber);
        });
    }

    // Level 3 -> Level 4
    if (levelThree* L3 = dynamic_cast<levelThree*>(currentLevel)) {
        QObject::connect(L3, &gameLevel::levelComplete, [this]() {
            playLevelTransition(this->view, currentLevelNumber);
        });
    }
    // Level 4 -> Level 5
    if (levelFour* L4 = dynamic_cast<levelFour*>(currentLevel)) {
        QObject::connect(L4, &gameLevel::levelComplete, [this]() {
            playLevelTransition(this->view, currentLevelNumber);
        });
    }

    return this->view;
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
    QApplication::processEvents();

    if (currentView) {
        currentView->setEnabled(false);

        // Check here if there are any segmentation faults.
        QTimer::singleShot(150, currentView, [currentView]() {
            currentView->hide();
            currentView->close();
            currentView->deleteLater();
        });
    }
}

void MenuWindow::playIntroVideo()
{
    audio->setVolume(0);
    QVideoWidget* videoWidget = new QVideoWidget(this);
    videoWidget->setGeometry(this->rect());
    videoWidget->show();
    videoWidget->raise();

    QMediaPlayer* videoPlayer = new QMediaPlayer(videoWidget);
    QAudioOutput* videoAudio = new QAudioOutput(videoWidget);

    videoPlayer->setVideoOutput(videoWidget);
    videoPlayer->setAudioOutput(videoAudio);

    videoAudio->setVolume(0.8);

    videoPlayer->setSource(QUrl("qrc:/assets/intro.mp4"));

    QObject::connect(videoPlayer, &QMediaPlayer::mediaStatusChanged,
                     this,
                     [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            QSettings settings("Technoscape", "Game");
            settings.setValue("introPlayed", true);

            videoWidget->hide();
            videoWidget->deleteLater();

            QTimer::singleShot(120, this, [this]() {
            startLevel(1);
        });
        }
    });

    videoPlayer->play();
}

void MenuWindow::playOutroVideo(QGraphicsView* gameView)
{
    if (!gameView) {
        showMainMenu(nullptr, this);
        return;
    }

    paused = true;

    QVideoWidget* videoWidget = new QVideoWidget(gameView);
    videoWidget->setGeometry(gameView->rect());
    videoWidget->show();
    videoWidget->raise();

    QMediaPlayer* videoPlayer = new QMediaPlayer(videoWidget);
    QAudioOutput* videoAudio = new QAudioOutput(videoWidget);

    videoPlayer->setVideoOutput(videoWidget);
    videoPlayer->setAudioOutput(videoAudio);

    videoAudio->setVolume(0.8);

    videoPlayer->setSource(QUrl("qrc:/assets/outro.mp4"));

    gameLevel* oldLevel = currentLevel;

    QObject::connect(videoPlayer, &QMediaPlayer::mediaStatusChanged,
                     this,
                     [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            videoWidget->hide();
            videoWidget->deleteLater();

            showMainMenu(gameView, this);

            if (oldLevel) {
                QTimer::singleShot(600, oldLevel, [oldLevel]() {
                    oldLevel->deleteLater();
                });
            }
        }
    });

    videoPlayer->play();
}