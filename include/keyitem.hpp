#ifndef KEYITEM_HPP
#define KEYITEM_HPP

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QMovie>
#include <QString>

class KeyItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

    private:
    QMovie* movie;
    int targetW;
    int targetH;

    public:
    KeyItem(const QString& gifPath, int w, int h, QGraphicsItem* parent = nullptr);
    ~KeyItem();
};

#endif