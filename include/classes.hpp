#include <iostream>
#include <string>
using namespace std;

//for variable names, in the class I used whatever was logical, and for constructors I either took the first letter of the variable name, or the capital version
//if it was already used.

class Item //used in Game_Object as an array of pointers to items.
{
    private:
    double damage;
    int durability;
    double cooldown;

    public:
    Item(double d, double D, double c) : damage(d), durability(D), cooldown(c) {} 
    bool canAttack();
    ~Item();
};

class Game_Object{
    private:
    string name;
    int level;
    double health;
    Item** Inventory; //array of pointers to allow polymorphic behavior with items

    public:
    Game_Object(string n, int l, double h, Item** i) : name(n), level(l), health(h), Inventory(i) {}
    virtual void update() = 0;
    virtual ~Game_Object() {}
};

class Enemy:public Game_Object
{   
    private:
    bool isChasing();
    bool isAttacking();
    double speed;
    int id;

    public:
    Enemy(string n, int l, double h, Item** i, double s, int I) : Game_Object(n,l, h, i), speed(s), id(I) { } 
};

class Robot:public Enemy
{
    private:
    string type;

    public:
    Robot(string n, int l, double h, Item** i, double s, int I, string t) : Enemy(n, l, h, i, s, I), type(t) {};
};


class Drone:public Enemy
{
    //honestly at a loss for what to put here. Couldn't think of anything.
};

class Tank:public Robot
{
    private:
    double defence;

    public:
    Tank(string n, int l, double h, Item** i, double s, int I, string t, double d) : Robot(n, l, h, i, s, I, t), defence(d) {}
};

class Shooter:public Robot
{
    private:
    double range;

    public:
    Shooter(string n, int l, double h, Item** i, double s, int I, string t, double r) : Robot(n, l, h, i, s, I, t), range(r) {}
    
};

class Suicider:public Robot
{
    private:
    double explodedamage;

    public:
    Suicider(string n, int l, double h, Item** i, double s, int I, string t, double e) : Robot(n, l, h, i, s, I, t), explodedamage(e) {}
};