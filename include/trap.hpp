#ifndef TRAP_HPP
#define TRAP_HPP

#include <QGraphicsRectItem>

class Trap : public QGraphicsRectItem {
    public:
    Trap(int x, int y, int w, int h);

    bool canDamage() const;
    void trigger();
    void resetCooldown();

    private:
    bool ready;
};

#endif