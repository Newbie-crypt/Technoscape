#include "menu_gameScene.hpp"

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

MenuWindow::MenuWindow() {

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
            createGameView(new levelOne);
            this->hide();
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
QGraphicsView* MenuWindow::createGameView(gameLevel* inputLevel) {

    // if the level is level one, perform these level-specific functions..
    if (levelOne* L1 = dynamic_cast<levelOne*> (inputLevel)) {
        currentLevel = L1;
        currentLevel->setupScene();
        L1->spawnEnemies();
        currentLevel->setupSpawnKeyEvent();
    }
    // else if level 2..... and so on.


    QGraphicsView* view = new QGraphicsView(currentLevel->getScene());
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(0);
    view->setBackgroundBrush(Qt::black);
    view->setAlignment(Qt::AlignCenter);
    view->showFullScreen();

    // The reason why we are using a QTimer in this section of code is to
    // wait until fullscreen window activation events have finished processing before the 
    // player item ends up with the scene focus
    // Without this, you wouldn't be able to move the player in the first place :)
    QTimer::singleShot(0, [this, view]() {
        view->setFocus();
        currentLevel->getScene()->setFocusItem(currentLevel->getPlayer());
        currentLevel->getPlayer()->setFocus();
    });


    auto fitScene = [view, this] () {
        view->fitInView(currentLevel->getScene()->sceneRect(), Qt::IgnoreAspectRatio);
    };

    fitScene();
    QTimer::singleShot(0, [fitScene]() { fitScene(); });
    QTimer::singleShot(50, [fitScene]() { fitScene(); });

    pauseMenu* pause = new pauseMenu(view, currentLevel);
    QObject::connect(pause, &pauseMenu::leaveRequested, [view, this] () {
        showMainMenu(view, this);
    });

    gameOver* death_screen = new gameOver(view, currentLevel);

    QObject::connect(death_screen, &gameOver::tryAgainRequested, [this] {
        createGameView(currentLevel);
    });
    
    QObject::connect(death_screen, &gameOver::mainMenuRequested, [this, view] {
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
