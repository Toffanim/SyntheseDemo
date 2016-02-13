#if !defined(PLAYER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice:  $
   ======================================================================== */

#define PLAYER_H

#include "../mesh/Model.h"
#include "../camera/Camera.h"
#include "../controller/Controller.h"
#include <SDL2/SDL.h>
#include <map>
#include <glm/glm.hpp>
#include "../actor/Actor.h"

class Player
{
public:
    Player(SDL_Keycode forwardKey = SDLK_z,
           SDL_Keycode backwardKey = SDLK_s,
           SDL_Keycode rightKey = SDLK_q,
           SDL_Keycode leftKey = SDLK_d);
    ~Player();

    
    void mouseMoveCallback( int x, int y );
    void startMoveForward();
    void startMoveBackward();
    void startMoveRight();
    void startMoveLeft();
    void stopMoveForward();
    void stopMoveBackward();
    void stopMoveRight();
    void stopMoveLeft();
    void stopMoveUp();;
    void startMoveUp();
    void startMoveDown();
    void stopMoveDown();

    void move(float deltaTime);
    void updateCamera();
    void mouseMotion(int x, int y);

    bool hasKey();
    bool isAttacking( float deltaTime );

    void addLife( int value ) { life += value; }
    void addGold( int value ) { gold += value; }
    void addKey( int value ) { key += value; }
    int getGold() { return(gold);}
    int getLife() { return(life);}
    int getKey() {return(key);}
    
    std::map<SDL_Keycode, std::string> getMapping(){return(mapping);}
    Controller* getController(){return(c);}
    Camera* getCamera(){return(cam);}
    void setPosition( glm::vec3 p ) { position = p; updateCamera();}
    glm::vec3 getPosition(){return(position);}
    AABB& getAABB(){return(aabb);}

    bool getMoveLeft() { return(moveLeft);}
    bool getMoveRight() { return(moveRight); }
    bool getMoveForward() { return(moveForward); }
    bool getMoveBackward() { return(moveBackward); }

    int getAttack() { return(atk); }
private:
    SDL_Keycode forwardKey;
    SDL_Keycode backwardKey;
    SDL_Keycode rightKey;
    SDL_Keycode leftKey;

    bool moveLeft;
    bool moveRight;
    bool moveForward;
    bool moveBackward;
    bool moveUp;
    bool moveDown;
    bool firstTime;

    int life;
    int gold;
    int key;
    
    std::map<SDL_Keycode, std::string> mapping;
    float speed;
    glm::vec3 position;
    Controller* c;
    Camera* cam;

    float lastX;
    float lastY;

    float atkPerSec;
    int atk;
    float lastAttack;
    
    AABB aabb;
    void computeAABB();
};

#endif
