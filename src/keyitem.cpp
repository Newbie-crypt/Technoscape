#include "../include/keyitem.hpp"
#include <QPixmap>

KeyItem::KeyItem(const QString& gifPath, int w, int h, QGraphicsItem* parent)
    : QObject(), QGraphicsPixmapItem(parent), targetW(w), targetH(h) {
    movie = new QMovie(gifPath);

    // Making the key animate!!
    QObject::connect(movie, &QMovie::frameChanged, [this]() {
        QPixmap frame = movie->currentPixmap().scaled(targetW, targetH, Qt::IgnoreAspectRatio,
                                                      Qt::SmoothTransformation);
        setPixmap(frame);
    });

    movie->start();
}

KeyItem::~KeyItem() {
    if (movie) {
        movie->stop();
        delete movie;
    }
}