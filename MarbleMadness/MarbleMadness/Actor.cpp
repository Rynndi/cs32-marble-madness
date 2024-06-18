//#include "Actor.h"
//#include "StudentWorld.h"
//
//// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

//all methods in Actor can't access the actorVec directly

//delete all the dead actors on the vector
void Actor::doSomething(){
    if (!isAlive()){
        setDead();
    }
}
void Player::doSomething() {
    int key;
    //get the position of the player
    int aX = getX();
    int aY = getY();
    //get the key input
    if(getWorld()->getKey(key)){
        switch(key){
                //if pressed up
                //set the direction by 1, and then move to the position
            case KEY_PRESS_UP:
                setDirection(up);
                aY++;
                if (!getWorld()->isBlocked(this))
                    moveTo(aX, aY);
                break;
            
            case KEY_PRESS_DOWN:
                setDirection(down);
                aY--;
                if (!getWorld()->isBlocked(this))
                    moveTo(aX, aY);
                break;
                
            case KEY_PRESS_RIGHT:
                setDirection(right);
                aX++;
                if (!getWorld()->isBlocked(this))
                    moveTo(aX, aY);
                break;
                
            case KEY_PRESS_LEFT:
                setDirection(left);
                aX--;
                if (!getWorld()->isBlocked(this))
                    moveTo(aX, aY);
                break;
            //if the ammo is greater than zero, create a pea and fire
            case KEY_PRESS_SPACE:
                if(getAmmo() >0) {
                    getWorld()->createPea(this);
                    decreaseAmmo();
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
//
                }
                break;
            //instantly sets dead
            case KEY_PRESS_ESCAPE:
                setDead();
                break;
        }
    }
}
//if the marble is blocked by another marble, take in the direction of 
bool Marble::isBlockedMarble(Actor* marble,int dir){
    int aX = this->getX();
    int aY = this->getY();
    
    switch(dir) {
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
        if(marble->getX() == aX && marble->getY() == aY){
            return true;
        }
       
    return false;
}
//add 20 to the player's ammo
void Player::addAmmo(int a) {
    ammo = getAmmo() + 20;
}
//robots and players take damage when hit
void Agent::takeDamage() {
    int hp = getHP() - 2;
    if(hp <=0) {
        decreaseHP(getHP());
        setDead();
    }
    else{
        decreaseHP(2);
    }
}
//marbles take damage when hit
void Marble::takeDamage(){
    int hp = getHP() - 2;
    if(hp <=0) {
        decreaseHP(getHP());
        setDead();
    }
    else{
        decreaseHP(2);
    }
}
//robots are overridden because they have to make a sound
void Robot::takeDamage() {
    Agent::takeDamage();
    getWorld()->playSound(SOUND_ROBOT_IMPACT);
    if(getHP() <= 0) {
        getWorld()->increaseScore(getBonus());
        getWorld()->playSound(SOUND_ROBOT_DIE);
    }
}
//must assign thiefbots
void ThiefBot::takeDamage() {
    Agent::takeDamage();

    getWorld()->playSound(SOUND_ROBOT_IMPACT);
    if(getHP() <= 0) {
        getWorld()->increaseScore(10);
        getWorld()->playSound(SOUND_ROBOT_DIE);
        //if there is is an actor goodie with the robot
        if(getActorGoodie() != nullptr) {
            //set it visible
            getActorGoodie()->setVisible(true);
            //can be stolen again
            getActorGoodie()->setStolen(false);
        }
    }
}

void Crystal::doSomething() {
    if(!isAlive()) {
        return;
    }
    //if player on same block as a crystal
    if(getWorld()-> amIAlsoHere(this)){
        getWorld()->increaseScore(50);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        //decrease the crystals every time the player steps onto it 
        getWorld()->decCrystal();
        setDead();

    }
}
void AmmoGoodie::doSomething() {
    if(!isAlive()) {
        return;
    }
    if(getWorld()-> amIAlsoHere(this)){
        getWorld()->increaseScore(100);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->addPeas(20);
        setDead();
    }
}
void RestoreHealthGoodie::doSomething() {
    if(!isAlive()) {
        return;
    }
    if(getWorld()-> amIAlsoHere(this)){
        getWorld()->increaseScore(500);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        //reset health to 20 (does not work for other values of health)
        int giveMax = 2000000000-(getWorld()->getPlayer()->getHP());
        getWorld()->getPlayer()->increaseHP(giveMax);
        setDead();
    
    }
}
void ExtraLifeGoodie::doSomething() {
    if(!isAlive()) {
        return;
    }
    if(getWorld()-> amIAlsoHere(this)){
        getWorld()->increaseScore(1000);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->incLives();

        setDead();
    
    }
}

void Exit::doSomething(){
    //if crystal count is equal to zero
    //shows up
    if(!isVisible() && getWorld()->noMoreCrystals()){
        setVisible(true);
        getWorld()->playSound(SOUND_REVEAL_EXIT);
    }
    //if it's visible and the player's also on it
    else if(isVisible() && getWorld()->amIAlsoHere(this)) {
        getWorld()->playSound(SOUND_FINISHED_LEVEL);
        getWorld()->increaseScore(2000);
        //level is completed 
        getWorld()->setLevelDone();
    }
}

void Pea::doSomething() {
    if(!isAlive()) {
        return;
    }
    //get the direction of the pea
    int aX = this->getX();
    int aY = this->getY();
    int Dir = getDirection();
    //if the pea can attack the player, set dead and return true
    if(getWorld()->doesAttackPlayer(aX, aY, this)){
        setDead();
        return;
    }
    
    //check if at coordinate
    switch(Dir) {
        case(GraphObject::up):
            aY++;
            moveTo(aX, aY);
            break;
        case (GraphObject::down):
            aY--;
            moveTo(aX, aY);
            break;
        case (GraphObject::left):
            aX--;
            moveTo(aX, aY);
            break;
        case (GraphObject::right):
            aX++;
            moveTo(aX, aY);
            break;
        }
    //if the pea can attack the player, setDead and return true
    if(getWorld()->doesAttackPlayer(aX, aY, this)){
        setDead();
        return;
    }
    
}

void RageBot::doSomething() {
    if(!isAlive()) {
        return;
    }
    //  If the ThiefBot is supposed to “rest” during the current tick
    //it must do nothing during the current tick other than to update its tick count.
    if(getCurrTick() != getMaxTick()){
        increaseTick();
        return;
    }
    //else rest during tick, don't fire
    int aX = this->getX();
    int aY = this->getY();
    int Dir = getDirection();

       if( getWorld()->doesRobotFire(this)){
           resetTick();
           return;
       }
        switch(Dir) {
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
        //if no obstruction, ragebot will move to it
        if (!getWorld()->isRobotBlocked(this)){
            moveTo(aX, aY);
            resetTick();
            return;
        }
        
        //otherwise, ragebot tries to move to adjacent square in direction it is facing if square does not contain obstruction
        else{//reverse direction it's facing
            switch(Dir) {
                case(GraphObject::up):
                    setDirection(down);
                    break;
                case (GraphObject::down):
                    setDirection(up);
                    break;
                case (GraphObject::left):
                    setDirection(right);
                    break;
                    
                case (GraphObject::right):
                    setDirection(left);
                    break;
            }
            resetTick();
            return;
        }
}

void ThiefBot::doSomething() {
    if(!isAlive()) {
        return;
    }
    //  If the ThiefBot is supposed to “rest” during the current tick
    //it must do nothing during the current tick other than to update its tick count.
    if(getCurrTick() != getMaxTick()){
        increaseTick();
        return;
    }
    //if thiefbot is on same square as a goodie, and has not ever picked up a goodie, 1 in 10 chance they will pick it up
    int num = randInt(1, 10);
    if(num == 4){
        //goodie can be picked up
        if (getWorld()->goodieOnThief(this) != nullptr) {
            Actor* ptr = getWorld()->goodieOnThief(this);
            //if not stolen
            if(!ptr->isStealable()) {
                //thiefbot needs to remember what it stole, change status
                goodie = ptr;
                ptr->setStolen(true);
                //turn invisible
                ptr->setVisible(false);
                //robot eats the goodie
                getWorld()->playSound(SOUND_ROBOT_MUNCH);
                resetTick();
                return;
            }
        }
    }
    //otherwise, if thiefbot has not yet moved distanceBeforeTurning() squares in its current direction, will try to move to adjacent square in direction it's facing if square doesn't contain obstruction
//    obstructions: the player, a marble, a wall, a pit, a robot, a robot Factory
    if(getDistanceMoved() < getDistanceBeforeTurning() && !getWorld()->isRobotBlocked(this)) {
        ThiefBotMove();
    }
   
//else, getDistanceMoved== getDistanceBeforeTurning or is blocked
//assign the direction to the cardinal direction
    int pdir = randInt(0,3);
    int dir = 90 * pdir;
    //a. Select a random integer from 1 to 6 inclusive to be the new value of
    //distanceBeforeTurning.
    distanceBeforeTurning = randInt(1,6);
    setDirection(dir);
    //can robot move there
    switch(dir) {
            //all directions are integers.
        case (right):
            setDirection(dir);
            //if robot is not blocked here
            if (!getWorld()->isRobotBlocked(this)) {
                ThiefBotMove();
            }
            break;
        case (left):
            if (!getWorld()->isRobotBlocked(this)) {
                ThiefBotMove();
            }
            break;
        case(up):
            if (!getWorld()->isRobotBlocked(this)) {
                ThiefBotMove();
            }
            break;
        case(down):
            if (!getWorld()->isRobotBlocked(this)) {
                ThiefBotMove();
            }
            break;
        default:
            break;
            
    }
    resetTick();
    return;

}
//moves the thiefbot
void ThiefBot::ThiefBotMove() {
    int aX = getX();
    int aY = getY();
    //set the direction to move it to
    int Dir = getDirection();
        switch(Dir) {
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
        //if no obstruction, thiefbot will move to it
        if (!getWorld()->isRobotBlocked(this)){
            moveTo(aX, aY);
            //update the distance moved by 1 
            updateDistMoved(1);
            resetTick();
            return;
        }
        else{
            resetTick();
            return;
            
        }
}

void MeanThiefBot::doSomething() {
    if(!isAlive()) {
        return;
    }
    //  If the ThiefBot is supposed to “rest” during the current tick
    //it must do nothing during the current tick other than to update its tick count.
    if(getCurrTick() != getMaxTick()){
        increaseTick();
        return;
    }
    //fires
    if( getWorld()->doesRobotFire(this)){
        resetTick();
        return;
    }
    //same as regular thiefbot
    ThiefBot::doSomething();

}

void ThiefBotFactory::doSomething(){
// the Factory will create a new ThiefBot of the type that factory manufactures and add it to the maze on the same square as the factory. If it creates a new ThiefBot, it must play the SOUND_ROBOT_BORN sound effect.
    int num = randInt(0, 49);
    //If the count is less than 3 in the range AND there is no ThiefBot of any type on the same square as the factory,
    if (getWorld()->thievesInRange(getX(), getY()) < 3 && !getWorld()->isSameSpot(this)){
        if (num == 4) {
            getWorld()->createThief(getX(), getY(), m_type);
        }
    }
}

