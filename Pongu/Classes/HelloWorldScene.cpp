#include "HelloWorldScene.h"

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    scene->getPhysicsWorld()->setGravity(Vect(0,0));

    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
    layer->setPhysicsWorld(scene->getPhysicsWorld());

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }

    srand(time(NULL));
    
    directorVisibleSize = Director::getInstance()->getVisibleSize();
    directorOrigin = Director::getInstance()->getVisibleOrigin();

    const int screenEdgeWidth = 5;
    auto screenEdgeBody = PhysicsBody::createEdgeBox(directorVisibleSize, PhysicsMaterial(1, 1, 0), screenEdgeWidth);
    auto screenEdgeNode = Node::create();
    screenEdgeNode->setPosition(Point(directorVisibleSize.width/2 + directorOrigin.x, directorVisibleSize.height/2 + directorOrigin.y));
    screenEdgeNode->setPhysicsBody(screenEdgeBody);
    this->addChild(screenEdgeNode);

    gameState = NOT_STARTED;
    touchDirection = NOT_TOUCHED;
    ballDirection = UnitVector(0, 0);

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::restart, this));
    
	closeItem->setPosition(Point(directorVisibleSize.width - closeItem->getContentSize().width/2, closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("Pongu", "Arial", 24);
    
    // position the label on the center of the screen
    label->setPosition(Point(directorVisibleSize.width/2, directorVisibleSize.height - label->getContentSize().height));

    this->addChild(label, 1);

    // Remove scale when got proper sized sprites
    const float barScale = 0.3;
    const float ballScale = 0.08;

    // My bar
    myBar = Sprite::create("whiteBar.png");
	myBar->setScale(barScale);
    this->initializeMyBarPosition();

    auto myBarBody = PhysicsBody::createBox(myBar->getBoundingBox().size, PhysicsMaterial(1, 1, 0), Point(0,0));
    myBarBody->setDynamic(false);
	myBarBody->setCollisionBitmask(BAR_COLLISION_MASK);
	myBarBody->setContactTestBitmask(true);
    myBar->setPhysicsBody(myBarBody);

    this->addChild(myBar, 0);

    // Computer bar
    computerBar = Sprite::create("whiteBar.png");
	computerBar->setScale(barScale);
    this->initializeComputerBarPosition();

    auto computerBarBody = PhysicsBody::createBox(computerBar->getBoundingBox().size, PhysicsMaterial(1, 1, 0), Point(0,0));
    computerBarBody->setDynamic(false);
	myBarBody->setCollisionBitmask(BAR_COLLISION_MASK);
	myBarBody->setContactTestBitmask(true);
    computerBar->setPhysicsBody(computerBarBody);

    this->addChild(computerBar, 0);

    // The ball
    ball = Sprite::create("blueBall.png");
    ball->setScale(ballScale);
    this->initializeBallPosition();

    auto ballBody = PhysicsBody::createCircle(ball->getBoundingBox().size.width/2, PhysicsMaterial(1, 1, 0), Point(0,0));
	ballBody->setCollisionBitmask(BALL_COLLISION_MASK);
	ballBody->setContactTestBitmask(true);
    ball->setPhysicsBody(ballBody);

    this->addChild(ball, 0);

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);

    touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

    this->scheduleUpdate();

    return true;
}

bool HelloWorld::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (gameState == NOT_STARTED)
    {
		this->updateBallVelocity();
    }

    gameState = STARTED;

    this->determineTouchDirection(touch);

    return true;
}

void HelloWorld::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    this->determineTouchDirection(touch);
}

void HelloWorld::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    touchDirection = NOT_TOUCHED;
}

bool HelloWorld::onContactBegin(cocos2d::PhysicsContact &contact)
{
	PhysicsBody *a = contact.getShapeA()->getBody();
	PhysicsBody *b = contact.getShapeB()->getBody();

	// TODO: Find out how to properly use collision bit masks
	if (a->getCollisionBitmask() == 1 && b->getCollisionBitmask() == 2 ||
		a->getCollisionBitmask() == 2 && b->getCollisionBitmask() == 1)
	{
		this->updateBallVelocity();
	}

	return true;
}

void HelloWorld::update(float dt)
{
    if (gameState == STARTED)
    {
        this->updateMyBar(dt);
		this->updateComputerBar(dt);
    }
}

void HelloWorld::updateMyBar(float dt)
{
    if (touchDirection == NOT_TOUCHED) return;

    float distanceToMove = DEFAULT_MY_BAR_SPEED * touchDirection * dt;
	this->updateBarPosition(myBar, distanceToMove);
}

void HelloWorld::updateComputerBar(float dt)
{
	float barPosX = computerBar->getPositionX();
	float ballPosX = ball->getPositionX();

	float barDirection = 0;
	if (barPosX < ballPosX)
	{
		barDirection = 1;
	}
	else if (barPosX > ballPosX)
	{
		barDirection = -1;
	}

	float distanceToMove = DEFAULT_COMPUTER_BAR_SPEED * barDirection * dt;
	this->updateBarPosition(computerBar, distanceToMove);
}

void HelloWorld::updateBarPosition(Sprite *bar, float distanceToMove)
{
	bar->setPositionX(bar->getPositionX() + distanceToMove);

	float BarWidthHalf = bar->getBoundingBox().size.width / 2;
    if (bar->getPositionX() + BarWidthHalf > directorVisibleSize.width)
        bar->setPositionX(directorVisibleSize.width - BarWidthHalf);
    else if(bar->getPositionX() - BarWidthHalf < 0)
        bar->setPositionX(BarWidthHalf);
}

void HelloWorld::determineBallDirection()
{
    auto ballPositionVectorFromMyBar = ball->getPosition() - myBar->getPosition();
    auto angle = ballPositionVectorFromMyBar.getAngle();

    ballDirection = UnitVector(cos(angle), sin(angle));
}

void HelloWorld::initialize()
{
    gameState = NOT_STARTED;
    touchDirection = NOT_TOUCHED;
    ballDirection = UnitVector(0, 0);
}

void HelloWorld::restart(Ref* pSender)
{
    this->initialize();
    this->initializeMyBarPosition();
    this->initializeComputerBarPosition();
    this->initializeBallPosition();
    this->ball->getPhysicsBody()->setVelocity(Vect(0, 0));
}

void HelloWorld::initializeMyBarPosition()
{
    myBar->setPosition(Point(directorVisibleSize.width / 2, BAR_VERTICAL_OFFSET));
}

void HelloWorld::initializeComputerBarPosition()
{
    computerBar->setPosition(Point(directorVisibleSize.width / 2, directorVisibleSize.height - BAR_VERTICAL_OFFSET));
}

void HelloWorld::initializeBallPosition()
{
    auto myBarSize = myBar->getBoundingBox().size;
    auto ballHeight = ball->getBoundingBox().size.height;
    auto ballRandomXPos = rand() % (int)myBarSize.width + myBar->getPositionX() - myBarSize.width/2;
    ball->setPosition(Point(ballRandomXPos, myBar->getPositionY() + myBarSize.height/2 + ballHeight/2));
}

#pragma region private methods

void HelloWorld::setPhysicsWorld(cocos2d::PhysicsWorld *world)
{
   this->sceneWorld = world;
}

void HelloWorld::determineTouchDirection(Touch *touch)
{
    if (touch->getLocation().x <= directorVisibleSize.width / 2)
        touchDirection = LEFT;
    else
        touchDirection = RIGHT;
}

void HelloWorld::updateBallVelocity()
{
	this->determineBallDirection();
	this->ball->getPhysicsBody()->setVelocity(Vect(DEFAULT_BALL_SPEED * ballDirection.first, DEFAULT_BALL_SPEED * ballDirection.second));
}

#pragma endregion private methods