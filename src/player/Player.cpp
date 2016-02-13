/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */
#include "Player.h"
using namespace std;

Player::Player(SDL_Keycode forwardKey, SDL_Keycode backwardKey, SDL_Keycode rightKey, SDL_Keycode leftKey) :
        forwardKey(forwardKey), backwardKey(backwardKey), rightKey(rightKey), leftKey(leftKey),
        moveForward(false), moveRight(false), moveLeft(false), moveBackward(false),
        speed(2.0f), position( glm::vec3( 0.0f, 0.0f, 0.0f )),
        lastX(0.0f), lastY(0.0f), firstTime(true), life(100), gold(0),
        mapping(map<SDL_Keycode,string>()), atkPerSec( 0.5f ),
atk( 2 ), lastAttack( 0.0f ), key(1)
{
    //Make all the key callback setup     
    mapping.insert( pair<SDL_Keycode,string>(forwardKey, "moveForward"));
    mapping.insert( pair<SDL_Keycode,string>(backwardKey, "moveBackward"));
    mapping.insert( pair<SDL_Keycode,string>(rightKey, "moveRight"));
    mapping.insert( pair<SDL_Keycode,string>(leftKey, "moveLeft"));
    
    c = new Controller();
    c->setKeyPressCallback(forwardKey, bind(&Player::startMoveForward, this));
    c->setKeyPressCallback(backwardKey, bind(&Player::startMoveBackward, this));
    c->setKeyPressCallback(rightKey, bind(&Player::startMoveRight, this));
    c->setKeyPressCallback(leftKey, bind(&Player::startMoveLeft, this));
    c->setKeyReleaseCallback(forwardKey, bind(&Player::stopMoveForward, this));
    c->setKeyReleaseCallback(backwardKey, bind(&Player::stopMoveBackward, this));
    c->setKeyReleaseCallback(rightKey, bind(&Player::stopMoveRight, this));
    c->setKeyReleaseCallback(leftKey, bind(&Player::stopMoveLeft, this));
    c->setKeyPressCallback(SDLK_UP, bind(&Player::startMoveUp, this));
    c->setKeyReleaseCallback(SDLK_UP, bind(&Player::stopMoveUp, this));
    c->setKeyPressCallback(SDLK_DOWN, bind(&Player::startMoveDown, this));
    c->setKeyReleaseCallback(SDLK_DOWN, bind(&Player::stopMoveDown, this));
    std::function<void(int, int)> f = bind(&Player::mouseMotion, this, std::placeholders::_1, std::placeholders::_2);
    c->setMouseCallback(f);
    cam = new Camera( position );

    aabb.min = glm::vec3();
    aabb.max = glm::vec3();
    aabb.size = glm::vec3();
    aabb.center = glm::vec3();
}

Player::~Player()
{
    if(c)
    { delete c; }
    if(cam)
    { delete cam; }
}

//Is the player currently having a key ?
bool Player::hasKey()
{
    return key > 0 ? true : false;
}

//Is the player attacking ?
bool Player::isAttacking( float deltaTime )
{
    lastAttack += deltaTime;
    if ( lastAttack >= atkPerSec)
    {
        lastAttack = 0.0f;
        return true;
    }return false;
}


//Pretty self explaining
void Player::startMoveUp()
{
    moveUp = true;
}
void Player::startMoveDown()
{
    moveDown = true;
}
void Player::stopMoveDown()
{
    moveDown=false;
}
void Player::stopMoveUp()
{
    moveUp = false;
}
void Player::stopMoveForward()
{
    moveForward = false;
}
void Player::stopMoveBackward()
{
    moveBackward = false;
}
void Player::stopMoveRight()
{
    moveRight = false;
}
void Player::stopMoveLeft()
{
    moveLeft = false;
}
void Player::startMoveForward()
{
    moveForward = true;
}
void Player::startMoveBackward()
{
    moveBackward = true;
}
void Player::startMoveRight()
{
    moveRight = true;
}
void Player::startMoveLeft()
{
    moveLeft = true;
}

//Apply movement based on the boolean seen ^^^^^ before ^^^^^
void Player::move(float deltaTime)
{
    glm::vec3 pos = position;
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;

    glm::vec3 t = cam->getFront();
    t.y = 0.0;
    t = glm::normalize(t);
    glm::vec3 v = glm::vec3( t.z, t.y, -t.x); 
    glm::vec3 u = glm::cross(t,v);
    if(moveForward)
    {
        pos += speed*deltaTime*t;
    }
    if(moveBackward)
    {
        pos -= speed*deltaTime*t;
    }
    if(moveRight)
    {
        pos += speed*deltaTime*v;
    }
    if(moveLeft)
    {
        pos -= speed*deltaTime*v;
    }
    if(moveUp)
    {
        pos += speed*deltaTime*u;
    }
    if(moveDown)
    {
        pos -= speed*deltaTime*u;
    }
    
    position = pos;
    computeAABB();
    updateCamera();
}

//Update the camera along the player position
void Player::updateCamera()
{
    cam->setPosition( position );
}

//Callback for the mouse
void Player::mouseMotion(int x, int y)
{
    //Not using x and y because SDL is bugged in relative coordinates
    //Create dependency but better for motion use
    int t1;
    int t2;
    SDL_GetRelativeMouseState(&t1,&t2);
    float sensitivity = 0.05;
    t1 *= sensitivity;
    t2 *= -sensitivity;

    cam->addYaw( t1 );
    cam->addPitch( t2 );
    cam->updateCameraVectors();    
}

//Compute AABB for the player (the player having no model we cant auto compute it
void Player::computeAABB()
{
    float halfSize = 0.2f;
    aabb.max.x = position.x + halfSize;
    aabb.max.y = position.y + halfSize;
    aabb.max.z = position.z + halfSize;
    aabb.min.x = position.x - halfSize;
    aabb.min.y = position.y - 0.4f;
    aabb.min.z = position.z - halfSize;

    aabb.size = glm::vec3(halfSize*2);
    aabb.center = position;
}
