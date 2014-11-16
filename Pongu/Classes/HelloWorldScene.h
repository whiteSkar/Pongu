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

const int BALL_COLLISION_MASK = 1;
const int BAR_COLLISION_MASK = 2;
const int EDGE_COLLISION_MASK = 3;

const float DEFAULT_BALL_SPEED = 650;
const float MAX_BALL_SPEED = 1200;
const float BALL_SPEED_UP_INTERVAL = (MAX_BALL_SPEED - DEFAULT_BALL_SPEED) / 20;

const float DEFAULT_COMPUTER_BAR_SPEED = 450;
const float COMPUTER_BAR_SPEED_UP_INTERVAL = 10;
const float BASE_COMPUTER_BAR_SPEED = DEFAULT_COMPUTER_BAR_SPEED - COMPUTER_BAR_SPEED_UP_INTERVAL;
const float MAX_COMPUTER_BAR_SPEED = DEFAULT_BALL_SPEED * cos(30 * M_PI / 180);	// 30 degress

const float DEFAULT_MY_BAR_SPEED = MAX_COMPUTER_BAR_SPEED;	// ~563

const PhysicsMaterial PHYSICS_MATERIAL_NO_FRICTION = PhysicsMaterial(1, 1, 0);

const std::string SOUND_COLLISION_BALL_BAR = "audio/ball_bar_collision.wav";
const std::string SOUND_COLLISION_BALL_EDGE = "audio/ball_edge_collision.wav";
const std::string SOUND_WIN = "audio/win.wav";
const std::string SOUND_LOSE = "audio/lose.wav";

class HelloWorld : public Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static Scene* createScene();

	GameState gameState;

    Size directorSize;
    Point directorOrigin;

    Sprite *myBar;
    Sprite *computerBar;
    Sprite *ball;

	int level;
	LabelTTF *levelLabel;

    UnitVector ballDirection;

    TouchDirection touchDirection;

	bool isComputerBarMaxSpeed;

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
	void initialize();
    void reset();
    void restart(Ref* pSender);

    void initializeMyBarPosition();
    void initializeComputerBarPosition();
    void initializeBallPosition();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

    bool onTouchBegan(Touch *touch, Event *event);
    void onTouchMoved(Touch *touch, Event *event);
    void onTouchEnded(Touch *touch, Event *event);

	void onContactSeperate(cocos2d::PhysicsContact &contact);

    void update(float dt);
    void updateMyBar(float dt);
	void updateComputerBar(float dt);

    void determineBallDirection(Sprite *bar);

private:
    cocos2d::PhysicsWorld *sceneWorld;

	float ballSpeed;

	void setPhysicsWorld(cocos2d::PhysicsWorld *world);

    void determineTouchDirection(Touch *touch);
	void updateBallVelocity(bool shouldSpeedChange);
	void updateBarPosition(Sprite *bar, float distanceToMove);
	void updateLevelLabel();

	void checkGameEndCondition();
	void gameEnd(bool isWin);
	void win();
	void lose();

	void preloadImages();
};

#endif // __HELLOWORLD_SCENE_H__
