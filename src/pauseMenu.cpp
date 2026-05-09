#include "pauseMenu.hpp"

extern bool paused;

pauseMenu::pauseMenu(QGraphicsView* inputView, gameLevel* currentLevel) {

    level = currentLevel;
    view = inputView;
    
    // ===== WORKING PAUSE UI ON TOP OF GAME =====

    pauseButton = new QPushButton(view);
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

    leftBar = new QFrame(pauseButton);
    leftBar->setGeometry(16, 12, 8, 32);
    leftBar->setStyleSheet(
        "background-color: white;"
        "border-radius: 3px;"
    );
    leftBar->show();

    rightBar = new QFrame(pauseButton);
    rightBar->setGeometry(32, 12, 8, 32);
    rightBar->setStyleSheet(
        "background-color: white;"
        "border-radius: 3px;"
    );
    rightBar->show();

    pauseOverlay = new QWidget(view->viewport());
    pauseOverlay->setGeometry(view->viewport()->rect());
    pauseOverlay->setStyleSheet("background-color: rgba(0,0,0,140);");
    pauseOverlay->hide();
    pauseOverlay->raise();

    overlayLayout = new QVBoxLayout(pauseOverlay);
    overlayLayout->setContentsMargins(0, 0, 0, 0);
    overlayLayout->setAlignment(Qt::AlignCenter);

    pausePanel = new QFrame;
    pausePanel->setFixedSize(420, 260);
    overlayLayout->addWidget(pausePanel, 0, Qt::AlignCenter);

    pausePanel->setStyleSheet(
        "QFrame {"
        "   background-color: rgba(10, 20, 45, 230);"
        "   border: 3px solid #36e0ff;"
        "   border-radius: 18px;"
        "}"
    );


    pauseLayout = new QVBoxLayout(pausePanel);
    pauseLayout->setSpacing(18);
    pauseLayout->setContentsMargins(35, 30, 35, 30);

    pauseTitle = new QLabel("PAUSED");
    pauseTitle->setAlignment(Qt::AlignCenter);
    pauseTitle->setStyleSheet(
        "color: white;"
        "font-size: 30px;"
        "font-weight: bold;"
    );

    continueButton = new QPushButton("CONTINUE");
    leaveButton = new QPushButton("LEAVE");

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
        openPauseMenu();
    });

    escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), view);
    QObject::connect(escShortcut, &QShortcut::activated, [=]() {
        togglePauseMenu();
    });

    QObject::connect(continueButton, &QPushButton::clicked, [=]() {
        closePauseMenu();
    });

    QObject::connect(leaveButton, &QPushButton::clicked, [=]() {
        paused = true;
        emit leaveRequested();
    });
}

void pauseMenu::openPauseMenu() {
    Player* player = level->getPlayer();
    if (player && player->isDead()) {
        return;
    }

    if (paused) {
        return;
    }

    paused = true;

    // remove control from the focus item (player or side player)
    level->getScene()->setFocusItem(nullptr);
    if (player) player->clearFocus();
    view->clearFocus();

    pauseOverlay->setGeometry(view->viewport()->rect());
    pauseOverlay->show();
    pauseOverlay->raise();
    pauseOverlay->setFocus();
}

void pauseMenu::closePauseMenu() {
    paused = false;
    pauseOverlay->hide();

    // give control back to player
    view->setFocus();
    Player* player = level->getPlayer();
    if (player) {
        level->getScene()->setFocusItem(player);
        player->setFocus();
    }
}

void pauseMenu::togglePauseMenu() {
    if (!paused) {
        openPauseMenu();
    } else {
        closePauseMenu();
    }
}