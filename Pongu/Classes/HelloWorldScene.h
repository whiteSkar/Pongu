#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <math.h>

USING_NS_CC;

typedef enum
{
    NOT_STARTED,
    STARTED,
    FINISHED
} GameState;

typedef enum
{
    NOT_TOUCHED = 0,
    LEFT = -1,
    RIGHT = 1
} TouchDirection;

typedef std::pair<float, float> UnitVector;

const int BAR_VERTICAL_OFFSET = 50;

const float DEFAULT_MY_BAR_SPEED = 500.0;
const float DEFAULT_COMPUTER_BAR_SPEED = 300.0;
const float DEFAULT_BALL_SPEED = 400.0;

class HelloWorld : public Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    void initialize();
    void restart(Ref* pSender);

    void initializeMyBarPosition();
    void initializeComputerBarPosition();
    void initializeBallPosition();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

    bool onTouchBegan(Touch *touch, Event *event);
    void onTouchMoved(Touch *touch, Event *event);
    void onTouchEnded(Touch *touch, Event *event);

    void HelloWorld::update(float dt);
    void HelloWorld::moveMyBar(float dt);
    void HelloWorld::moveBall(float dt);

    void HelloWorld::determineBallDirection();

    GameState gameState;

    Size directorVisibleSize;
    Point directorOrigin;

    Sprite *myBar;
    Sprite *computerBar;
    Sprite *ball;

    UnitVector ballDirection;

    TouchDirection touchDirection;

private:
    void HelloWorld::determineTouchDirection(Touch *touch);
};

#endif // __HELLOWORLD_SCENE_H__
