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
#include <QFrame>
#include <QResizeEvent>
#include <QLinearGradient>
#include <QPainterPath>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QCoreApplication>
#include <QEnterEvent>
#include <QUrl>
#include "../include/players.hpp"
#include "../include/wall.hpp"
#include "../include/furniture.hpp"
#include "../include/door.hpp"
#include "../include/trap.hpp"
#include "../include/machine.hpp"
#include "../include/classes.hpp"


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

class SoundButton : public QPushButton {
public:
    SoundButton(const QString& text, QWidget* parent = nullptr)
        : QPushButton(text, parent) {

        clickPlayer = new QMediaPlayer(this);
        clickAudio = new QAudioOutput(this);
        clickPlayer->setAudioOutput(clickAudio);
        clickPlayer->setSource(QUrl::fromLocalFile(
            QCoreApplication::applicationDirPath() + "/assets/sounds/click.wav"
        ));
        clickAudio->setVolume(1.0);

        hoverPlayer = new QMediaPlayer(this);
        hoverAudio = new QAudioOutput(this);
        hoverPlayer->setAudioOutput(hoverAudio);
        hoverPlayer->setSource(QUrl::fromLocalFile(
            QCoreApplication::applicationDirPath() + "/assets/sounds/houver.wav"
        ));
        hoverAudio->setVolume(1.0);

        QObject::connect(this, &QPushButton::clicked, [this]() {
            clickPlayer->stop();
            clickPlayer->setPosition(0);
            clickPlayer->play();
        });
    }
protected:
    void enterEvent(QEnterEvent* event) override {
        hoverPlayer->stop();
        hoverPlayer->setPosition(0);
        hoverPlayer->play();
        QPushButton::enterEvent(event);
    }

private:
    QMediaPlayer* clickPlayer;
    QAudioOutput* clickAudio;
    QMediaPlayer* hoverPlayer;
    QAudioOutput* hoverAudio;
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

    Player* player = new Player(0, 0);
    player->setPos(568, 400);
    scene->addItem(player);
    scene->setFocusItem(player);
    player->setFocus();

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
    addBox(226, 88, 8, 18);
    addBox(234, 102, 10, 4);
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

QFrame* pausePanel = new QFrame(pauseOverlay);
pausePanel->setFixedSize(420, 260);

int panelX = (pauseOverlay->width() - pausePanel->width()) / 2;
int panelY = (pauseOverlay->height() - pausePanel->height()) / 2;
pausePanel->move(panelX, panelY);
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
        paused = true;
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
    // ===== END PAUSE UI =====

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
            qDebug() << "ERROR: IMAGE NOT FOUND: assets/menu_bg.png";
        }

        background->setPixmap(bg);
        background->setScaledContents(true);
        background->setGeometry(0, 0, width(), height());
        background->lower();

        panel = new QFrame(this);
        panel->setGeometry(520, 408, 450, 430);
        panel->setStyleSheet(
            "QFrame {"
            "   background-color: rgba(10, 20, 45, 145);"
            "   border: 2px solid #36e0ff;"
            "   border-radius: 16px;"
            "}"
        );

        title = new TitleWidget(this);
        title->setGeometry(panel->x() - 90, panel->y() - 270, panel->width() + 180, 229);

        QGraphicsDropShadowEffect* titleGlow = new QGraphicsDropShadowEffect;
        titleGlow->setBlurRadius(100);
        titleGlow->setColor(QColor(0, 255, 255, 220));
        titleGlow->setOffset(0, 0);
        title->setGraphicsEffect(titleGlow);

        QVBoxLayout* panelLayout = new QVBoxLayout(panel);
        panelLayout->setSpacing(28);
        panelLayout->setContentsMargins(32, 32, 32, 32);

        SoundButton* startButton = new SoundButton("START GAME");
        SoundButton* settingsButton = new SoundButton("SETTINGS");
        SoundButton* exitButton = new SoundButton("EXIT");

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
    }
};

void showMainMenu(QGraphicsView* currentView) {
    MenuWindow* menu = new MenuWindow();
    menu->showFullScreen();
    QApplication::processEvents();
    currentView->hide();
    currentView->close();
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    music = new QMediaPlayer;
    audio = new QAudioOutput;

    music->setAudioOutput(audio);
    music->setSource(QUrl::fromLocalFile(
        QCoreApplication::applicationDirPath() + "/assets/sounds/music.mp3"
    ));
    music->setLoops(QMediaPlayer::Infinite);
    audio->setVolume(0.12);
    music->play();

    MenuWindow menu;

    return app.exec();
}
