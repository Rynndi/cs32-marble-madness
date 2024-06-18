

#include "Level.h"
#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <iomanip>
using namespace std;

class Agent;
class Marble;
class Actor;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}
StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

int StudentWorld::init(){
    //load level
    m_isLevelDone = false;
    Level lev(assetPath());
    //get the current level
    int levelN = getLevel();
   //level prints out the integer corresponding to the level
    string level = "level0" + to_string(levelN)+ ".txt";
    Level::LoadResult result = lev.loadLevel(level);
    if (result == Level::load_fail_file_not_found){
        cerr << "Could not find level00.txt data file\n";
        //if the player file can't be found, return player won
        return GWSTATUS_PLAYER_WON;
    }
    //if there's bad formatting, return error
    else if (result == Level::load_fail_bad_format){
        cerr << "Your level was improperly formatted\n";
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success) {
        cerr << "Successfully loaded level\n";
        //prints out the dimensions of the world
        for(int i =0; i < VIEW_WIDTH; i++) {
            for (int j=0; j < VIEW_HEIGHT; j++) {
                //get the contents of the txt file
                Level::MazeEntry ge = lev.getContentsOf(i,j);
                //push all actors onto the vector if it detects it
                switch(ge) {
                    case Level::wall:
                        actorVec.push_back(new Wall(this, i, j));
                        break;
                    case Level::player:
                        m_player = new Player(this, i, j);
                        break;
                    case Level::marble:
                        actorVec.push_back(new Marble(this, i, j));
                        break;
                    case Level::pit:
                        actorVec.push_back(new Pit(this, i,j));
                        break;
                    case Level::crystal:
                        actorVec.push_back(new Crystal(this, i,j));
                        m_crystalsLeft++;
                        break;
                    case Level::restore_health:
                        actorVec.push_back(new RestoreHealthGoodie(this, i, j));
                        break;
                    case Level::extra_life:
                        actorVec.push_back(new ExtraLifeGoodie(this, i, j));
                        break;
                    case Level::exit:
                        actorVec.push_back(new Exit(this, i,j));
                        break;
                    case Level::ammo:
                        actorVec.push_back(new AmmoGoodie(this, i, j));
                        break;
                    case Level::horiz_ragebot:
                        actorVec.push_back(new HorizontalRageBot(this, i, j));
                        break;
                    case Level::vert_ragebot:
                        actorVec.push_back(new VerticalRageBot(this, i,j));
                        break;
                 
                    case Level::thiefbot_factory:
                        actorVec.push_back(new ThiefBotFactory(this, i, j,Level::thiefbot_factory));
                        break;
                    case Level::mean_thiefbot_factory:
                        actorVec.push_back(new ThiefBotFactory(this, i, j, Level::mean_thiefbot_factory));
                        break;
                    case Level::empty:
                        break;

                }
            
            }
        }
    }
//    if (result != Level::load_fail_bad_format) return GWSTATUS_CONTINUE_GAME;
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move(){
    //display the text on top of the screen
    displayGameText();
    //player does something
    m_player->doSomething();
    //if player is dead, decrease lives, play sound, play screen
    if(!m_player->isAlive()) {
        decLives();
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }
    //add bonus to the level if done
    if(m_isLevelDone == true) {
        increaseScore(m_bonus);
        m_bonus = 1000;
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    list<Actor*>::iterator it = actorVec.begin();
    while(it != actorVec.end()) {
        //if dead, delete it
        if(!(*it)->isAlive()){
            (*it)->setDead();
        }
        //every actor on the vector does something
        (*it)->doSomething();
        it++;
        //ask all actors currently active in the game world to do something
        //exit object senses player there->changes to true with member function
    }
    list<Actor*>::iterator it2 = actorVec.begin();
    while(it2 != actorVec.end()) {
        if((*it2)->isAlive()) it2++;
        else{
            delete(*it2);
            //erase pointer from vector
            it2 = actorVec.erase(it2);
        }
    }
    if(m_bonus>0) {
        m_bonus--;
    }
    displayGameText();
    return GWSTATUS_CONTINUE_GAME;
 
}


bool StudentWorld::isBlocked(Agent* player){
    //switch (fake movement)
    //compare if fake movement matches the current coordinate
    int aX = player->getX();
    int aY = player->getY();
    int fakeDir = player->getDirection();
    //check if at coordinate
    switch(fakeDir) {
        case(GraphObject::up):
            aY++;
            break;
        case (GraphObject::down):
            aY--;
            break;
        case (GraphObject::left):
            aX--;
            break;
        case (GraphObject::right):
            aX++;
            break;
    }
    
    //if player is on the spot
    if (getPlayer()->getX() == aX && getPlayer()->getY() == aY) {
        return true;
    }
    
    list<Actor*>::iterator it = actorVec.begin();
       while(it != actorVec.end()){
           if((*it)->getX() == aX && (*it)->getY() == aY){
               //if the item is a marble
               if((*it)->isMarble()){
                   if(!moveMarble((*it))) {
                       return true;
                   }
                   else{
                       m_player->moveTo(aX,aY);
                       return false;
                   }
               }
               else if ((*it)->isRobot()) return true;
               else if((*it)->isWall()) return true;
               else if((*it)->isPit()) return true;
               else if ((*it)->isThiefBotFactory()) return true;
               else if((*it)->isCollectable()) return false;
               
               else if ((*it)->isExit()){
                   it++;
                   continue;
               }
               else{
                   return false;
               }
           }
           it++;
       }
       return false;
}
//can you move the marble?
bool StudentWorld::moveMarble(Actor* marble) {
    //moves whatever direction the player does
    int bX = marble->getX();
    int bY= marble->getY();
    int playDir = m_player->getDirection();
    
    //attempted direction of the marble based on the direction the player moves
    switch(playDir) {
        case(GraphObject::up):
            bY++;
            break;
        case (GraphObject::down):
            bY--;
            break;
        case (GraphObject::left):
            bX--;
            break;
        case (GraphObject::right):
            bX++;
            break;
    }
    // Marble is trying to move outside the game world boundaries
    if (bX <= 0 || bX >= VIEW_WIDTH-1 || bY <= 0 || bY >= VIEW_HEIGHT-1) {
        return false;
    }
    list<Actor*>::iterator it = actorVec.begin();
    while(it != actorVec.end()){
        //if item on actor is collectable, wall, exit or robot, can't move marble onto it
        if((*it)->isMarble() || (*it)->isWall() || (*it)->isExit() || (*it)->isRobot() || (*it)->isCollectable()){
            //if marble is blocked by the actor and the player's direction
            if (marble->isBlockedMarble((*it),playDir))
                //can't move
                return false;
        }
        //marble manages pit
        if((*it)->isPit()){
            //if the marble is blocked by the pit
            if (marble->isBlockedMarble((*it),playDir)){
                //delete both items once superimposed
                (*it)->setDead();
                marble->setDead();
            }
        }
        it++;
    }
    //else move the marble to an empty slot if it's not blocked
    marble->moveTo(bX, bY);
    return true;
}
//asks if the player is on the same location as an actor
bool StudentWorld::amIAlsoHere(Actor* checkActor){
    if(m_player->getX() == checkActor->getX() && m_player->getY() == checkActor->getY()) {
        return true;
    }
    else return false;
}
//clean all actors on the vector
void StudentWorld::cleanUp(){
    while (!actorVec.empty()) {
        //delete what the pointer is pointing to
        delete actorVec.back();
        actorVec.pop_back();
    }
    //delete the player pointer
    delete m_player;
    m_player = nullptr;
}
//destructing the studentworld
StudentWorld::~StudentWorld() {
    cleanUp();
    list<Actor*>::iterator it = actorVec.begin();
    while(it != actorVec.end()) {
        delete(*it);
        actorVec.erase(it);
        it++;
    }
}
//display the game of the text
void StudentWorld::displayGameText() {
    int currHP = getPlayer()->getHP()/20*100;
    cout.setf(ios::fixed);
    cout.precision(0);
    ostringstream os_score;
        os_score << setw(7) << setfill('0') << getScore();
        string score = os_score.str();
    ostringstream os_level;
        os_level << setw(2) << setfill('0') << getLevel();
        string level = os_level.str();
    ostringstream os_life;
        os_life <<setw(2)<<setfill(' ')<<getLives();
        string lives = os_life.str();
    ostringstream os_HP;
        os_HP << setw(3) << setfill(' ') << currHP;
        string health = os_HP.str()+'%';
    ostringstream os_Ammo;
        os_Ammo << setw(3) << setfill(' ') << m_player->getAmmo();
        string ammo = os_Ammo.str();
    ostringstream os_bonus;
        os_bonus << setw(4) << setfill(' ') << m_bonus;
        string bonus = os_bonus.str();
       
       string screen= "Score: " + score + "  Level: " + level + "  Lives: " + lives + "  Health: " + health + "  Ammo: " + ammo + "  Bonus: " + bonus;
       setGameStatText(screen);
}
//create the pea to be fired by the player/robot
void StudentWorld::createPea(Agent* agentFire) {
    //get the X coordinates of the agent
    int aX = agentFire->getX();
    int aY = agentFire->getY();
    //get the direction the agent is facing
    int Dir2= agentFire->getDirection();
    //move pea one ahead
    switch(Dir2){
        case(GraphObject::up):
            aY++;
            break;
        case (GraphObject::down):
            aY--;
            break;
        case (GraphObject::left):
            aX--;
            break;
        case (GraphObject::right):
            aX++;
            break;
    }
    //push the pea one location in front of whatever is calling it
    actorVec.push_front(new Pea(this, aX, aY, Dir2));
}

//helps when come upon a factory and agent on same square
//does the pea hit the player?
bool StudentWorld:: doesAttackPlayer(int pX, int pY, Actor* pea){
    //if on the same slot as the player
    if (getPlayer()->getX() == pX && getPlayer()->getY() == pY){
        m_player->takeDamage();
        return true;
    }
    
    std::list<Actor*>::iterator it = actorVec.begin();
    while (it!= actorVec.end()){
        //only if agent or marble there (pea is exception)

        //passes through collectable and pit and factory
        if((*it)->getX() == pX && (*it)->getY() == pY && (*it)!=pea && !(*it)->isPit() && !(*it)->isCollectable() && !(*it)->isThiefBotFactory()){
            //item that is hit has to take damage
            (*it)->takeDamage();
            return true;
        }
        it++;
    }

    return false;
}

void StudentWorld::addPeas(int peas) {
    m_player->addAmmo(20);
    return;
}

bool StudentWorld::isRobotBlocked(Agent* robot){

    int aX = robot->getX();
    int aY = robot->getY();
    int fakeDir = robot->getDirection();
    //check if at coordinate
    switch(fakeDir) {
        case(GraphObject::up):
            aY++;
            break;
        case (GraphObject::down):
            aY--;
            break;
        case (GraphObject::left):
            aX--;
            break;
        case (GraphObject::right):
            aX++;
            break;
    }
    //check the player. Robot is blocked by player
    if(m_player->getX() == aX && m_player->getY() == aY){
        return true;
    }
    //check everything else on the vector
  //the robot is blocked by walls, pits, factories, marbles, and other robots
    list<Actor*>::iterator it = actorVec.begin();
       while(it != actorVec.end()){
           if((*it)->getX() == aX && (*it)->getY() == aY){
            if ((*it)->isRobot()) return true;
            else if((*it)->isWall()) return true;
            else if((*it)->isPit()) return true;
            else if((*it)->isThiefBotFactory()) return true;
            else if((*it)->isMarble())return true;
            else if((*it)->isThiefBot()) return true;
           }
           it++;
       }
       return false;
}
//does the robot fire
//robots only fire when player is in line of sight
bool StudentWorld::doesRobotFire(Agent* checkRobot) {
    int aX = checkRobot->getX();
    int aY = checkRobot->getY();

    int Dir = checkRobot->getDirection();
    
    //get X and Y location of the player
    int playLocX = getPlayer()->getX();
    int playLocY = getPlayer()->getY();
    
//    //rageBot must determine whether it should fire cannon
//    //if player is in the same row or column AND is currently facing the player AND there are no obstacles in the way, it will fire a pea towards the player
    
        //if the robot is facing left, fires when player's in same column and greater Y location
        if(Dir == GraphObject::left && aY==playLocY && playLocX < aX){
            aX--;
            //Y location of both has to be the same
            while(aX != playLocX){
                list<Actor*>::const_iterator it = actorVec.begin();
            //sees through all the actors at that location
                while((it)!=actorVec.end()){
                    if ((*it)->getX() == aX && (*it)->getY() == aY){
                        //if player is blocked by a wall, robot, factory or marble, robot can't fire
                        if((*it)->isWall() ||(*it)->isRobot() || (*it)->isMarble()|| (*it)->isThiefBotFactory())
                            return false;
                    }
                    it++;
                }
                aX--;
            }
            createPea(checkRobot);
            playSound(SOUND_ENEMY_FIRE);
            return true;
                
        }
        //if the robot is facing right, fires when player's in same column and has a lesser Y location
        else if(Dir == GraphObject::right && aY==playLocY && playLocX > aX){
            aX++;
            //loops through the location
            while(aX != playLocX){
                    list<Actor*>::const_iterator it = actorVec.begin();
                //sees through all the actors at that location
                    while((it)!=actorVec.end()){
                        if ((*it)->getX() == aX && (*it)->getY() == aY){
                      
                            if((*it)->isWall() ||(*it)->isRobot() || (*it)->isMarble()|| (*it)->isThiefBotFactory())
                                return false;
                        }
                        it++;
                    }
                    aX++;
            }
            createPea(checkRobot);
            playSound(SOUND_ENEMY_FIRE);
            return true;
                
        }
       //if the robot is facing up, has to be in the same x coordinate, but player Y must be greater
        else if(Dir == GraphObject::up && aY<playLocY && playLocX == aX){
            aY++;
            //Y location of both has to be the same
          
            while(aY != playLocY){
                list<Actor*>::const_iterator it = actorVec.begin();
            //sees through all the actors at that location
                while((it)!=actorVec.end()){
                    if ((*it)->getX() == aX && (*it)->getY() == aY){
                        //ADD IN ROBOT FACTORY LATER!
                        if((*it)->isWall() ||(*it)->isRobot() || (*it)->isMarble() || (*it)->isThiefBotFactory())
                            return false;
                    }
                    it++;
                }
                aY++;
            }
            createPea(checkRobot);
            playSound(SOUND_ENEMY_FIRE);
            return true;
                
        }
        else if(Dir == GraphObject::down && aY>playLocY && playLocX == aX){
            aY--;
            //Y location of both has to be the same
          
            while(aY != playLocY){
                list<Actor*>::const_iterator it = actorVec.begin();
            //sees through all the actors at that location
                while((it)!=actorVec.end()){
                    if ((*it)->getX() == aX && (*it)->getY() == aY)
{
                        //ADD IN ROBOT FACTORY LATER!
                        if((*it)->isWall() ||(*it)->isRobot() || (*it)->isMarble()|| (*it)->isThiefBotFactory())
                            return false;
                    }
                    it++;
                }
                    aY--;
            }
            createPea(checkRobot);
            playSound(SOUND_ENEMY_FIRE);
            return true;
        }
    return false;
}

//return goodie pointer to be reassigned to another spot later, nullptr if none 
Actor* StudentWorld::goodieOnThief(Agent* thief) {
    list<Actor*>::const_iterator it = actorVec.begin();
    while(it!= actorVec.end()){
        if((*it)->isCollectable() && thief->getX() == (*it)->getX() && thief->getY() == (*it)->getY()){
            //goodie pointer pointing to it
            Actor* goodie = (*it);
            return goodie;
        }
        it++;
    }
    return nullptr;
    
}

void StudentWorld::createThief(int x, int y, char type){
    //if character type matches that of the mean thief, push back onto vector
    if(type == Level:: mean_thiefbot_factory) {
        actorVec.push_back(new MeanThiefBot(this,x,y));
      
    }
    if(type == Level::thiefbot_factory){
        actorVec.push_back(new ThiefBot(this,x,y,IID_THIEFBOT, 5));
    }
    playSound(SOUND_ROBOT_BORN);
}

int StudentWorld::thievesInRange(int x, int y) {
    int count =0;

    //set the world boundaries for the lower left, upper left, lower right and upper right
    int u_x = x+3;
    int l_x =x-3;
    int u_y =y+3;
    int l_y =y-3;

    if(u_x > 14)
        u_x = 14;
    
    if(u_y > 14)
        u_y = 14;
    
    if (l_x < 0)
        l_x = 0;
    
    if (l_y < 0)
        l_y = 0;

    list<Actor*>::iterator it = actorVec.begin();
     while (it!= actorVec.end()){
         if((*it)->isThiefBot() || (*it)->isMeanThiefBot()) {
             //If the count is less than 3 AND there is no ThiefBot of any type on the same square as the factory,
             if((*it)->getX() >= l_x && (*it)->getX() <= u_x && (*it)->getY() >= l_y && (*it)->getY() <= u_y){
                 count++;
             }
         }
         it++;
     }
    //return the number of thieves within the range
     return count;
   
}
//check to see if robot is in the same spot that isn't itself
//has to be able to walk on collectable
bool StudentWorld::isSameSpot(Actor* exc){
    list<Actor*>::iterator it = actorVec.begin();
    while (it!= actorVec.end()){
        if((exc)->getX() == (*it)->getX() && (exc)->getY() == (*it)->getY() && (*it)!=exc){
            return true;
        }
        it++;
    }
    return false;
}
