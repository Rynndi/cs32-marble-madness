
//#ifndef ACTOR_H_
//#define ACTOR_H_
//
//#include "GraphObject.h"
//
//// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
//
//#endif // ACTOR_H_
#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"


class StudentWorld;
//base class
class Actor : public GraphObject
{
  public:
    Actor(StudentWorld* sw, int imageID, int x, int y, int hp, int dir):GraphObject(imageID, x, y, 0, 1.0){m_world = sw; setVisible(true); m_hp = hp;setDirection(dir);}
    
    virtual void doSomething();
    virtual bool blocksPlayer(){return false;}
    StudentWorld* getWorld() const { return m_world;}
    virtual bool isBlockedMarble(Actor* marble,int dir) {return false;}
    void setDead(){m_life = false;}
    bool isAlive(){return m_life;}
   
    virtual double getHP() {return m_hp;}
    virtual double decreaseHP(int dec) {return m_hp = m_hp- dec;}
    virtual double increaseHP(int dec) {return m_hp = m_hp + dec;}
    
    virtual bool stopsPea() const {return false;}
    virtual void takeDamage(){}
  
  
    virtual void setStolen(bool b){}
    virtual bool isStealable(){ return false;}
    virtual void switchTheftStatus(){}

    
    virtual bool isPlayer() {return false;}
    virtual bool isExit() {return false;}
    virtual bool isWall() {return false;}
    virtual bool isMarble() {return false;}
    virtual bool isPit(){return false;}
    virtual bool isPea() {return false;}
    virtual bool isRobot() {return false;}
    virtual bool isCollectable() {return false;}
    virtual bool isThiefBotFactory() {return false;}
    virtual bool isThiefBot() {return false;}
    virtual bool isMeanThiefBot() {return false;}
   
    
  private:
        StudentWorld* m_world;
        bool m_life = true;
        int m_hp;
};

//includes robots and player
class Agent: public Actor{
public:
    Agent(StudentWorld* sw, int x, int y, int ID, int hp, int dir): Actor(sw, ID, x, y, hp, dir){setVisible(true);}
    virtual void doSomething(){}
    virtual bool stopsPea() const {return true;}
    virtual bool isPlayer() {return false;}
    virtual bool blocksPlayer() {return true;}
    virtual void takeDamage();

private:
};

class Player: public Agent{
public:
    Player(StudentWorld* sw, int x, int y): Agent(sw, x, y, IID_PLAYER, 2000000, right){}
    virtual void doSomething();
    int getAmmo() {return ammo;}
    virtual void addAmmo(int a);
    void restoreHealth();
 
    void decreaseAmmo(){ammo = ammo -1;}
    virtual bool stopsPea() const {return true;}
    virtual bool isPlayer() {return true;}
    void takeDamage(){Agent::takeDamage(); getWorld()->playSound(SOUND_PLAYER_IMPACT);}
    
private:
    int ammo = 20000000;

};

class Robot: public Agent{
public:
    Robot(StudentWorld* sw, int x, int y, int ID, int dir, int hp): Agent(sw, x, y, ID, hp, dir)
    {
        //set tick within robot's constructor 
        m_maxTick = (28 - getWorld()->getLevel())/4;
       // levelNumber is the current // level number (0, 1, 2, etc.)
        if ( m_maxTick < 3)
            m_maxTick = 3; // no RageBot moves more frequently than this

        m_currTick = 1;
        setDirection(dir);
    }
    virtual void doSomething(){}
    bool blocksMarble() {return true;}
    virtual bool isRobot() {return true;}

    virtual bool stopsPea() const {return true;}
    void takeDamage();
    void increaseTick(){m_currTick++;}
    //if the curr tick is equal to the max tick, reset current tick
    void resetTick(){ if(m_currTick == m_maxTick) m_currTick = 1; }
    //get the current Tick
    int getCurrTick() {return m_currTick;}
    //get the max tick
    int getMaxTick() {return m_maxTick;}
    virtual int getBonus() {return exp;}

private:
    int exp;
    int m_currTick;
    int m_maxTick;
};

class RageBot: public Robot {
public:
    RageBot(StudentWorld* sw, int x, int y, int dir): Robot(sw, x, y, IID_RAGEBOT, dir, 10) {}
    virtual void doSomething();
    virtual bool stopsPea() const {return true;}
    virtual int getBonus() {return exp;}
//    virtual void takeDamage(){Agent::takeDamage();}
private:
    int exp = 100;

    
};

class HorizontalRageBot: public RageBot{
public:
    HorizontalRageBot(StudentWorld* sw, int x, int y): RageBot(sw, x, y, right){}
    
   
};

class VerticalRageBot: public RageBot{
public:
    VerticalRageBot(StudentWorld* sw, int x, int y): RageBot(sw, x, y, down){setDirection(down);}
//    virtual void doSomething();
   
};

class Wall: public Actor {
public:
    //every wall has a pointer to the world, a starting x and y
    Wall(StudentWorld* sw, int x, int y): Actor(sw,IID_WALL, x, y, 1, none){}
    virtual bool blocksPlayer() {return true;}
    virtual bool isWall() {return true;}
    virtual bool stopsPea() const {return true;}
private:

};

class Marble: public Actor{
public :
    Marble(StudentWorld* sw, int x, int y): Actor(sw, IID_MARBLE, x, y, 10, none){}
    //bool moveMarble(Actor* Marble);
    virtual bool blocksPlayer() {return true;}
    bool blocksMarble() {return true;}
    //take in location, see if there's a marble at this location
    virtual bool isMarble(){return true;}
    virtual bool isBlockedMarble(Actor* marble,int dir);
   
    virtual bool stopsPea() const {return true;}
    virtual void takeDamage();
    
private:

};

class Pit: public Actor {
public:
    //every wall has a pointer to the world, a starting x and y
    Pit(StudentWorld* sw, int x, int y): Actor(sw,IID_PIT, x, y, 1, none){}
    virtual bool blocksPlayer() {return true;}
    virtual bool isPit(){return true;}
   

private:

};
class Crystal: public Actor {
public:
    //every wall has a pointer to the world, a starting x and y
    Crystal(StudentWorld* sw, int x, int y): Actor(sw,IID_CRYSTAL, x, y, 1, none){}
    virtual bool blocksPlayer() {return false;}
    virtual bool isCollectable() {return true;}
    virtual void doSomething();
    virtual bool isStealable() {return true;}

private:

};
class Exit: public Actor{
public:
    Exit(StudentWorld* sw, int x, int y): Actor(sw,IID_EXIT, x, y, 1, none){setVisible(false);}
    virtual bool isExit() {return true;}
    virtual void doSomething();
    
};

class Pea: public Actor{
public:
    Pea(StudentWorld* sw, int x, int y, int dir): Actor(sw,IID_PEA, x, y, 1, none) {
        setDirection(dir);
    }
    virtual void doSomething();
    virtual bool isPea(){return true;}
    
private:
 
};

class Goodie: public Actor {
public:
    Goodie(StudentWorld* sw, int x, int y, int ID): Actor(sw,ID, x, y, 1,none){}
    virtual bool blocksPlayer() {return false;}
    virtual bool isCollectable() {return true;}
    virtual void doSomething(){}
    
    virtual void switchTheftStatus() {m_status = !m_status;}
    virtual void setStolen(bool b) { m_status = b;}
    virtual bool isStealable(){ return m_status;}
private:
    //check if a goodie is stolen
    //true if stolen, can't be picked up
    bool m_status = false;

};
class AmmoGoodie: public Goodie{
public:
    AmmoGoodie(StudentWorld* sw, int x, int y): Goodie(sw, x, y, IID_AMMO){}
    virtual void doSomething();
private:
};


class RestoreHealthGoodie: public Goodie{
public:
    RestoreHealthGoodie(StudentWorld* sw, int x, int y): Goodie(sw, x, y, IID_RESTORE_HEALTH){}
    virtual void doSomething();
private:
    

};

class ExtraLifeGoodie: public Goodie{
public:
    ExtraLifeGoodie(StudentWorld* sw, int x, int y): Goodie(sw, x, y, IID_EXTRA_LIFE){}
    virtual void doSomething();

};

class ThiefBot: public Robot {
public:
    ThiefBot(StudentWorld* sw, int x, int y, int ID, int hp): Robot(sw, x, y, IID_THIEFBOT, right, hp){}
    virtual void doSomething();
    void updateDistMoved(int amount) { currDistance += amount;}
    int getDistanceMoved() { return currDistance;}
    virtual int getDistanceBeforeTurning() {return distanceBeforeTurning;}
    Actor* getActorGoodie() {return goodie;}
    virtual void ThiefBotMove();
    virtual int getBonus() {return exp;}
    virtual void takeDamage();
    virtual bool isThiefBot() {return true;}
 

private:
    int distanceBeforeTurning;
    int currDistance; 
    Actor* goodie = nullptr;
    int exp = 10;

};

class MeanThiefBot: public ThiefBot {
public:
    MeanThiefBot(StudentWorld* sw, int x, int y): ThiefBot(sw, x, y, IID_MEAN_THIEFBOT, 8){}
    virtual void doSomething();
    virtual bool isMeanThiefBot() {return true;}
    
private:
};

//check an area around the factory, counts how many thiefbots are in that area
class ThiefBotFactory: public Actor{
public:
    ThiefBotFactory(StudentWorld* sw, int x, int y, char type): Actor(sw, IID_ROBOT_FACTORY, x, y, 1, none){m_type = type;}
    virtual void doSomething();
    virtual bool isThiefBotFactory() {return true;}
    
private:
    char m_type;
};


#endif // ACTOR_H_


