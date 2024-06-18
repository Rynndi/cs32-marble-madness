

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>
#include <list> 


using namespace std;

class Agent;
class Actor;
class Player;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool isBlocked(Agent* player);
    bool isRobotBlocked(Agent* robot);
    
    bool isBlockedMarble(Actor* marble,int dir);
    bool moveMarble(Actor* marble);
    
    void removeActor(Actor* victim);
    bool amIAlsoHere(Actor* checkActor);
    
    void decCrystal(){ m_crystalsLeft--;}
    bool noMoreCrystals() { return (m_crystalsLeft == 0);}
    
    void addPeas(int peas);
    void createPea(Agent* agentFire);
    bool doesAttackPlayer(int pX, int pY, Actor *pea);
    
    void displayGameText();
    void setLevelDone() {m_isLevelDone = true;}
    Player* getPlayer() {return m_player; }
    bool doesRobotFire(Agent* checkRobot);

    Actor* goodieOnThief(Agent* thief);
    void createThief(int x, int y, char type);
    int thievesInRange(int x, int y);
    bool isSameSpot(Actor* exc);
       
private:
//    the player is the only actor allowed to not be stored in the single actor container.
    Player* m_player;
    list<Actor*> actorVec;
    int m_crystalsLeft;
    int m_bonus = 1000;
    bool m_isLevelDone;
};

#endif // STUDENTWORLD_H_
