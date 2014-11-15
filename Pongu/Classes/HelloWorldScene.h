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

const int BAR_COLLISION_MASK = 2;
const int BALL_COLLISION_MASK = 1;

const float DEFAULT_MY_BAR_SPEED = 400.0;
const float DEFAULT_COMPUTER_BAR_SPEED = 400.0;
const float DEFAULT_BALL_SPEED = 600;

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

	bool onContactBegin(cocos2d::PhysicsContact &contact);

    void update(float dt);
    void updateMyBar(float dt);
	void updateComputerBar(float dt);

    void determineBallDirection();

    GameState gameState;

    Size directorVisibleSize;
    Point directorOrigin;

    Sprite *myBar;
    Sprite *computerBar;
    Sprite *ball;

    UnitVector ballDirection;

    TouchDirection touchDirection;

private:
	void setPhysicsWorld(cocos2d::PhysicsWorld *world);

    void determineTouchDirection(Touch *touch);
	void updateBallVelocity();
	void updateBarPosition(Sprite *bar, float distanceToMove);

    cocos2d::PhysicsWorld *sceneWorld;
};

#endif // __HELLOWORLD_SCENE_H__
