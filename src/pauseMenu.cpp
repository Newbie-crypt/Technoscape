#include "pauseMenu.hpp"

extern bool paused;

pauseMenu::pauseMenu(QGraphicsView* view, gameLevel* currentLevel) {
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
        if (currentLevel->getPlayer()->isDead()) {
            return;
        }

        if (paused) {
            return;
        }

        paused = true;

        // remove control from player
        currentLevel->getScene()->setFocusItem(nullptr);
        currentLevel->getPlayer()->clearFocus();
        view->clearFocus();

        pauseOverlay->setGeometry(view->viewport()->rect());
        pauseOverlay->show();
        pauseOverlay->raise();
        pauseOverlay->setFocus();
    };

    auto closePauseMenu = [=]() {
        paused = false;
        pauseOverlay->hide();

        // give control back to player
        view->setFocus();
        currentLevel->getScene()->setFocusItem(currentLevel->getPlayer());
        currentLevel->getPlayer()->setFocus();
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
        paused = false;
        emit leaveRequested();
    });
}