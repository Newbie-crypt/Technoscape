#include "levelThree.hpp"
#include <QPainter>
#include <QLinearGradient>
#include <QFontMetrics>
 
levelThree::levelThree() : gameLevel() {}
levelThree::~levelThree() {
    // for (int i = 0; i < 10; i++) {
    //     if (drone[i]) delete drone[i]; // QPointer is null if the drone already self-deleted
    // }
    // for (int i = 0; i < number_of_brutes; i++) {
    //     if (brutes[i]) delete brutes[i];
    // }
}

void levelThree::setupScene() {

    scene->clear();
    levelBg.load(":/assets/levelThree/levelThreeScene.png");
    if (levelBg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level1_closed.png";
    }
    


    background = scene->addPixmap(levelBg);
    background->setZValue(-100);

    scene->setSceneRect(0, 0, levelBg.width(), levelBg.height());

    setupWalls();

    QPixmap health_symbol_image (":/assets/health_symbol.png");
    health_symbol_image = health_symbol_image.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);


    health_symbol = new QLabel(view);
    health_symbol->setPixmap(health_symbol_image);
    health_symbol->setAttribute(Qt::WA_TransparentForMouseEvents);
    health_symbol->move(15, 800);
    health_symbol->show();

    health_bar = new HealthBar(view);
    health_bar->move(80, 800);
    health_bar->show();


    // May the main character spawn!
    player = new Player(0, 0);
    player->setHealthBar(health_bar);
    player->setPos(568, 300);
    scene->addItem(player);


    QObject::connect(player, &Player::died, this, &gameLevel::playerDied);

    scene->setFocusItem(player);
    player->setFocus();

    QTimer* timer = new QTimer(this);
    QPointer<Player> safePlayer = player;
    QObject::connect(timer, &QTimer::timeout, [this, safePlayer] () {
        // The player gets deleted when the player dies and the user clicks play again. To prevent
        // Any segmentation fault, this if statement is used.
        if (!safePlayer) return;

        // Here, we are using the magic of lerp, a mathematical function which uses linear interpolation for the 
        // view to smoothly follow the player.
        QPointF current = this->view->mapToScene(this->view->viewport()->rect().center());
        QPointF target  = safePlayer->pos();
        QPointF next    = current + (target - current) * 0.1;
        this->view->centerOn(next);
    });

    timer->start(16);

    
    startWaveOne();

    QObject::connect(this, &levelThree::waveOneComplete, []{
        std::cout << "\n\n\nDone!\n\n\n";
    });
}

void levelThree::setupSpawnKeyEvent() {}

// For transparency, the banner is completely made by Claude
// ── Wave announcement banner ──────────────────────────────────────────────
// Circular HUD-style badge, fixed at the top-centre of the view.
class WaveBanner : public QWidget {
    int tick = 0;
    static constexpr int SZ = 220; // diameter
    int waveNumber;
public:
    explicit WaveBanner(QWidget* parent, int w) : QWidget(parent), waveNumber(w) {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_TranslucentBackground);
        setFixedSize(SZ, SZ);
        // view->showFullScreen() is called after setupScene(), so parent->width()
        // is 0 here. Defer positioning until the event loop has processed the
        // fullscreen resize event and the parent has its final dimensions.
        QTimer::singleShot(0, [this]() {
            if (parentWidget())
                move((parentWidget()->width() - SZ) / 2, 18);
        });
        // Elliptical mask so the widget corners are invisible/transparent.
        setMask(QRegion(0, 0, SZ, SZ, QRegion::Ellipse));
        QTimer* t = new QTimer(this);
        QObject::connect(t, &QTimer::timeout, [this]() {
            ++tick;
            update();
            if (tick >= 80) deleteLater();
        });
        t->start(33);
        raise();
        show();
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::TextAntialiasing);

        // Fade in / hold / fade out.
        double a = 1.0;
        if (tick < 12)      a = tick / 12.0;
        else if (tick > 68) a = (80 - tick) / 12.0;
        a = qBound(0.0, a, 1.0);
        auto ci = [&](int v) { return (int)(v * a); };

        const int cx = SZ / 2, cy = SZ / 2;
        const int r  = SZ / 2 - 3;

        // Circular background (radial gradient).
        QRadialGradient bg(cx, cy, r);
        bg.setColorAt(0.0, QColor(0, 12, 40, ci(245)));
        bg.setColorAt(1.0, QColor(0,  3, 15, ci(255)));
        p.setBrush(bg);
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(cx, cy), r, r);

        // Subtle scan-lines inside the circle.
        p.setClipRegion(QRegion(0, 0, SZ, SZ, QRegion::Ellipse));
        for (int y = 0; y < SZ; y += 3)
            p.fillRect(0, y, SZ, 1, QColor(0, 100, 200, ci(10)));
        p.setClipping(false);

        // Rotating outer arc (speeds up slightly over time).
        int arcStart = -(tick * 5) % 360;
        p.setPen(QPen(QColor(0, 220, 255, ci(210)), 3, Qt::SolidLine, Qt::RoundCap));
        p.drawArc(cx - r + 4, cy - r + 4, (r - 4) * 2, (r - 4) * 2,
                  arcStart * 16, 110 * 16);
        // Counter-rotating shorter arc for depth.
        p.setPen(QPen(QColor(255, 60, 60, ci(140)), 2, Qt::SolidLine, Qt::RoundCap));
        p.drawArc(cx - r + 4, cy - r + 4, (r - 4) * 2, (r - 4) * 2,
                  (-arcStart + 180) * 16, 50 * 16);

        // Outer ring (pulsing).
        double pulse = 0.65 + 0.35 * std::sin(tick * 0.22);
        p.setPen(QPen(QColor(0, 200, 255, ci((int)(170 * pulse))), 1));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPointF(cx, cy), r, r);

        // Inner ring.
        p.setPen(QPen(QColor(0, 140, 255, ci(55)), 1));
        p.drawEllipse(QPointF(cx, cy), r - 12, r - 12);

        // Clock tick marks (12 positions).
        for (int i = 0; i < 12; i++) {
            double angle = (i * 30 - 90) * M_PI / 180.0;
            bool major = (i % 3 == 0);
            int len = major ? 9 : 4;
            p.setPen(QPen(QColor(0, 200, 255, ci(major ? 150 : 70)), major ? 2 : 1));
            p.drawLine(QPointF(cx + (r - 1)       * std::cos(angle), cy + (r - 1)       * std::sin(angle)),
                       QPointF(cx + (r - 1 - len)  * std::cos(angle), cy + (r - 1 - len)  * std::sin(angle)));
        }

        // "WAVE" — character-by-character reveal.
        const QString part1 = "WAVE", part2 = QString("0%1").arg(waveNumber);
        int revealed  = tick / 2;
        QString shown1 = part1.left(qMin((int)part1.length(), revealed));
        QString shown2 = part2.left(qMin((int)part2.length(),
                                         std::max(0, revealed - (int)part1.length())));

        QFont f1("Arial Black", 15, QFont::Black);
        f1.setLetterSpacing(QFont::AbsoluteSpacing, 4);
        p.setFont(f1);
        // Glow pass.
        p.setPen(QColor(0, 180, 255, ci(28)));
        for (int o : {-2, 0, 2}) p.drawText(QRect(o, cy - 52, SZ, 26), Qt::AlignCenter, shown1);
        // Crisp pass.
        bool flicker = tick > 14 && (rand() % 12 == 0);
        p.setPen(QColor(200, 242, 255, ci(flicker ? 150 : 255)));
        p.drawText(QRect(0, cy - 52, SZ, 26), Qt::AlignCenter, shown1);

        // Divider line (appears once "WAVE" is fully shown).
        if (revealed >= (int)part1.length()) {
            p.setPen(QPen(QColor(0, 200, 255, ci(90)), 1));
            p.drawLine(cx - 38, cy - 20, cx + 38, cy - 20);
        }

        // "01" — larger number.
        QFont f2("Arial Black", 30, QFont::Black);
        f2.setLetterSpacing(QFont::AbsoluteSpacing, 3);
        p.setFont(f2);
        // Glow.
        p.setPen(QColor(0, 180, 255, ci(30)));
        for (int o : {-2, 0, 2}) p.drawText(QRect(o, cy - 16, SZ, 46), Qt::AlignCenter, shown2);
        // Crisp.
        p.setPen(QColor(0, 220, 255, ci(245)));
        p.drawText(QRect(0, cy - 16, SZ, 46), Qt::AlignCenter, shown2);

        // Sub-text with animated dots.
        p.setFont(QFont("Courier New", 7));
        p.setPen(QColor(0, 175, 255, ci(130)));
        p.drawText(QRect(0, cy + 35, SZ, 16), Qt::AlignCenter,
                   "STAND BY" + QString((tick / 7) % 4, '.'));

        // Pulsing centre dot at the bottom.
        double p2 = 0.4 + 0.6 * std::abs(std::sin(tick * 0.25));
        p.setBrush(QColor(0, 220, 255, ci((int)(200 * p2))));
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(cx, cy + 54), 3, 3);
    }
};
// ─────────────────────────────────────────────────────────────────────────

void levelThree::startWaveOne() {
    new WaveBanner(view, 1);

    // Spawn enemies once the banner has finished — 2200ms lands while the
    // banner is fading out so there's no dead gap between announcement and action.
    QTimer::singleShot(2200, this, [this]() {
        srand(time(0));
        const int number_of_robots = 5;
        for (int i = 0; i < number_of_robots; i++) {
            robots[i] = new Robot(player);
            scene->addItem(robots[i]);
            robots[i]->setPos(rand() % 1200 + 95, rand() % 890 + 40);
        }
        QTimer* waveTimer = new QTimer(this);
        QObject::connect(waveTimer, &QTimer::timeout, [this, waveTimer]() {
            QList presentObjects = scene->items();
            for (auto& o : presentObjects)
                if (dynamic_cast<Enemy*>(o)) return;
            emit waveOneComplete();
            waveTimer->stop();
        });
        waveTimer->setInterval(20);
        waveTimer->start();
    });
}

void levelThree::setupWalls() {
    addWall(235, 92, 127, 111);
    addWall(235, 366, 138, 97);
    addWall(383, 418, 37, 51);
    addWall(244, 682, 140, 71);
    addWall(403, 681, 34, 56);
    addWall(1096, 327, 45, 46);
    addWall(969, 131, 52, 73);
    addWall(1086, 136, 51, 120);
    addWall(1185, 213, 44, 114);
    addWall(1243, 215, 55, 157);
    addWall(1215, 534, 47, 81);
    addWall(989, 529, 93, 73);
    addWall(963, 561, 28, 57);
    addWall(1030, 608, 25, 28);
    addWall(1083, 546, 71, 61);
    addWall(972, 836, 35, 85);
    addWall(1135, 757, 106, 89);
    addWall(1083, 781, 41, 55);
    addWall(383, 138, 33, 75);
    addWall(237, 561, 79, 52);
    addWall(211, 840, 172, 137);
    addWall(433, 900, 33, 83);
    addWall(458, 134, 31, 61);
    addWall(237, 88, 182, 129);
    addWall(883, 148, 29, 61);
    addWall(971, 196, 131, 129);
    addWall(0, 0, 1379, 65);
    addWall(29, 67, 78, 1038);
    addWall(108, 242, 38, 204);
    addWall(111, 679, 28, 410);
    addWall(88, 1035, 1223, 44);
    addWall(1293, 928, 90, 179);
    addWall(1329, 0, 73, 927);
    addWall(1274, 51, 49, 124);
    addWall(1293, 173, 31, 22);
}

