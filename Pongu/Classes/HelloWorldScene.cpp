#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

// android cannot use std::to_string?? what?????
template<typename T>
std::string to_string(const T& t) {
	std::ostringstream os;
	os << t;
	return os.str();
};

Scene* HelloWorld::createScene()
{
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    scene->getPhysicsWorld()->setGravity(Vect(0,0));

    auto layer = HelloWorld::create();
    layer->setPhysicsWorld(scene->getPhysicsWorld());

    scene->addChild(layer);

    return scene;
}

bool HelloWorld::init()
{
    if (!Layer::init())
    {
        return false;
    }

    srand(time(NULL));
    
    directorSize = Director::getInstance()->getVisibleSize();
    directorOrigin = Director::getInstance()->getVisibleOrigin();

    const int screenEdgeWidth = 5;
	auto screenLeftEdgeBody = PhysicsBody::createEdgeSegment(Point(0, 0), Point(0, directorSize.height), PHYSICS_MATERIAL_NO_FRICTION, screenEdgeWidth);
	screenLeftEdgeBody->setCollisionBitmask(EDGE_COLLISION_MASK);
	screenLeftEdgeBody->setContactTestBitmask(true);
    auto screenLeftEdgeNode = Node::create();
    screenLeftEdgeNode->setPosition(Point(directorOrigin.x, directorOrigin.y));
    screenLeftEdgeNode->setPhysicsBody(screenLeftEdgeBody);
    this->addChild(screenLeftEdgeNode);

	auto screenRightEdgeBody = PhysicsBody::createEdgeSegment(Point(0, 0), Point(0, directorSize.height), PHYSICS_MATERIAL_NO_FRICTION, screenEdgeWidth);
	screenRightEdgeBody->setCollisionBitmask(EDGE_COLLISION_MASK);
	screenRightEdgeBody->setContactTestBitmask(true);
    auto screenRightEdgeNode = Node::create();
    screenRightEdgeNode->setPosition(Point(directorOrigin.x + directorSize.width, directorOrigin.y));
    screenRightEdgeNode->setPhysicsBody(screenRightEdgeBody);
    this->addChild(screenRightEdgeNode);

    this->initialize();

	// Debugging purpose for restarting manually
 //   auto closeItem = MenuItemImage::create(
 //                                          "images/CloseNormal.png",
 //                                          "images/CloseSelected.png",
 //                                          CC_CALLBACK_1(HelloWorld::restart, this));
 //   
	//closeItem->setPosition(Point(directorSize.width - closeItem->getContentSize().width/2, closeItem->getContentSize().height/2));

 //   auto menu = Menu::create(closeItem, NULL);
 //   menu->setPosition(Point::ZERO);
 //   this->addChild(menu, 1);

    // Remove scale when got proper sized sprites
    const float barScale = 0.6;
    const float ballScale = 0.17;

    // My bar
    myBar = Sprite::create("images/whiteBar.png");
	myBar->setScale(barScale);
    this->initializeMyBarPosition();

    auto myBarBody = PhysicsBody::createBox(myBar->getBoundingBox().size, PHYSICS_MATERIAL_NO_FRICTION, Point::ZERO);
    myBarBody->setDynamic(false);
	myBarBody->setCollisionBitmask(BAR_COLLISION_MASK);
	myBarBody->setContactTestBitmask(true);
    myBar->setPhysicsBody(myBarBody);

    this->addChild(myBar, 0);

    // Computer bar
    computerBar = Sprite::create("images/whiteBar.png");
	computerBar->setScale(barScale);
    this->initializeComputerBarPosition();

    auto computerBarBody = PhysicsBody::createBox(computerBar->getBoundingBox().size, PHYSICS_MATERIAL_NO_FRICTION, Point::ZERO);
    computerBarBody->setDynamic(false);
	//computerBarBody->setCollisionBitmask(BAR_COLLISION_MASK);
	computerBarBody->setContactTestBitmask(true);
    computerBar->setPhysicsBody(computerBarBody);

    this->addChild(computerBar, 0);

    // The ball
    ball = Sprite::create("images/blueBall.png");
    ball->setScale(ballScale);
    this->initializeBallPosition();

    auto ballBody = PhysicsBody::createCircle(ball->getBoundingBox().size.width/2, PHYSICS_MATERIAL_NO_FRICTION, Point::ZERO);
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
	contactListener->onContactSeperate = CC_CALLBACK_1(HelloWorld::onContactSeperate, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

    this->scheduleUpdate();

    // Why does the font name not seem to apply?
	levelLabel = LabelTTF::create("", "Arial", 36);
	this->updateLevelLabel();
    levelLabel->setPosition(Point(directorOrigin.x + directorSize.width - levelLabel->getContentSize().width - 50, directorOrigin.y + directorSize.height - levelLabel->getContentSize().height - 10));
    levelLabel->setAnchorPoint(Point::ZERO);
    this->addChild(levelLabel, 999);

    return true;
}

bool HelloWorld::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (gameState == NOT_STARTED)
    {
        this->moveMyBarAndBallToPosX(touch->getLocation().x);
    }

	if (gameState == STARTED)
	{
		this->determineTouchDirection(touch);
	}
    
    return true;
}

void HelloWorld::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (gameState == NOT_STARTED)
    {
        this->moveMyBarAndBallToPosX(touch->getLocation().x);
    }

    if (gameState == STARTED)
	{
		this->determineTouchDirection(touch);
	}
}

void HelloWorld::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (gameState == NOT_STARTED)
    {
        gameState = STARTED;
        this->updateBallVelocity(true);
    }

    if (gameState == FINISHED)
	{
		this->restart(this);
	}

    touchDirection = NOT_TOUCHED;
}

/* 
 * Using onContactBegin and playing sound effect here makes the collision too far
 * Using onContactSeperate and playing sound effet here maeks the collision to go too deep
 * Using onContactPostSolve has the same effect as onContactBegin
 * What should I do? Am I doing something wrong?
 * For now, use seperate since it's better to go deep than doesn't even collide
 */
 void HelloWorld::onContactSeperate(cocos2d::PhysicsContact &contact)
{
	PhysicsBody *a = contact.getShapeA()->getBody();
	PhysicsBody *b = contact.getShapeB()->getBody();

	// TODO: Find out how to properly use collision bit masks
	// Computer bar shouldn't pass this condition as updateBallVelocity will update the velocity using 'myBar' position
	if ((a->getCollisionBitmask() == BALL_COLLISION_MASK && b->getCollisionBitmask() == BAR_COLLISION_MASK) ||
		(a->getCollisionBitmask() == BAR_COLLISION_MASK && b->getCollisionBitmask() == BALL_COLLISION_MASK))
	{
		this->updateBallVelocity(false);
	}

	if ((a->getCollisionBitmask() == EDGE_COLLISION_MASK && b->getCollisionBitmask() == BALL_COLLISION_MASK) ||
		(a->getCollisionBitmask() == BALL_COLLISION_MASK && b->getCollisionBitmask() == EDGE_COLLISION_MASK))
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_COLLISION_BALL_EDGE.c_str());
	}
	else
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_COLLISION_BALL_BAR.c_str());
	}
}

void HelloWorld::update(float dt)
{
    if (gameState == STARTED)
    {
        this->updateMyBar(dt);
		this->updateComputerBar(dt);

		this->checkGameEndCondition();
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

	int distanceOffset = 10;
	float barDirection = 0;
	if (barPosX < ballPosX - distanceOffset)
	{
		barDirection = 1;
	}
	else if (barPosX > ballPosX + distanceOffset)
	{
		barDirection = -1;
	}

	float barSpeed;
	if (isComputerBarMaxSpeed)
	{
		barSpeed = MAX_COMPUTER_BAR_SPEED;
	}
	else
	{
		barSpeed = std::min(BASE_COMPUTER_BAR_SPEED + COMPUTER_BAR_SPEED_UP_INTERVAL * level, MAX_COMPUTER_BAR_SPEED);
		if (barSpeed == MAX_COMPUTER_BAR_SPEED)
		{
			isComputerBarMaxSpeed = true;
		}
	}
	float distanceToMove = barSpeed * barDirection * dt;
	this->updateBarPosition(computerBar, distanceToMove);
}

void HelloWorld::updateBarPosition(Sprite *bar, float distanceToMove)
{
	bar->setPositionX(bar->getPositionX() + distanceToMove);

	float BarWidthHalf = bar->getBoundingBox().size.width / 2;
    if (bar->getPositionX() + BarWidthHalf > directorSize.width)
        bar->setPositionX(directorSize.width - BarWidthHalf);
    else if(bar->getPositionX() - BarWidthHalf < 0)
        bar->setPositionX(BarWidthHalf);
}

void HelloWorld::determineBallDirection(Sprite *bar)
{
    auto ballPositionVectorFromBar = ball->getPosition() - bar->getPosition();
    auto angle = ballPositionVectorFromBar.getAngle();

    ballDirection = UnitVector(cos(angle), sin(angle));
}

void HelloWorld::initialize()
{
	level = 1;
	isComputerBarMaxSpeed = false;
	ballSpeed = DEFAULT_BALL_SPEED;

	this->reset();
	this->preloadImages();
}

void HelloWorld::reset()
{
    gameState = NOT_STARTED;
    touchDirection = NOT_TOUCHED;
    ballDirection = UnitVector(0, 0);
}

void HelloWorld::restart(Ref* pSender)
{
    this->reset();
    this->initializeMyBarPosition();
    this->initializeComputerBarPosition();
    this->initializeBallPosition();
	this->updateLevelLabel();
    this->ball->getPhysicsBody()->setVelocity(Vect(0, 0));
}

void HelloWorld::initializeMyBarPosition()
{
    myBar->setPosition(Point(directorSize.width / 2, BAR_VERTICAL_OFFSET));
}

void HelloWorld::initializeComputerBarPosition()
{
    computerBar->setPosition(Point(directorSize.width / 2, directorSize.height - BAR_VERTICAL_OFFSET));
}

void HelloWorld::initializeBallPosition()
{
    auto myBarSize = myBar->getBoundingBox().size;
    auto ballHeight = ball->getBoundingBox().size.height;
    auto ballRandomXPos = (rand() % (int)myBarSize.width/2) + myBar->getPositionX() - myBarSize.width/4;
    ball->setPosition(Point(ballRandomXPos, myBar->getPositionY() + myBarSize.height/2 + ballHeight/2));
}

#pragma region private methods

void HelloWorld::setPhysicsWorld(cocos2d::PhysicsWorld *world)
{
   this->sceneWorld = world;
}

void HelloWorld::determineTouchDirection(Touch *touch)
{
    if (touch->getLocation().x <= directorSize.width / 2)
        touchDirection = LEFT;
    else
        touchDirection = RIGHT;
}

void HelloWorld::updateBallVelocity(bool shouldSpeedChange)
{
	this->determineBallDirection(myBar);
	if (isComputerBarMaxSpeed && shouldSpeedChange)
	{
		ballSpeed += BALL_SPEED_UP_INTERVAL;
        ballSpeed = std::min(ballSpeed, MAX_BALL_SPEED);
	}

	this->ball->getPhysicsBody()->setVelocity(Vect(ballSpeed * ballDirection.first, ballSpeed * ballDirection.second));
}

void HelloWorld::updateLevelLabel()
{
    std::string levelString = "";
    if (level >= MAX_GAME_LEVEL)
    {
        levelString = "MAX LEVEL";
    }
    else
    {
        levelString = "Level " + to_string(level);
    }
	
	this->levelLabel->setString(levelString);
}

void HelloWorld::checkGameEndCondition()
{
	auto ballPosY = ball->getPositionY();
	auto ballHeightHalf = ball->getBoundingBox().size.height/2;
	if (ballPosY + ballHeightHalf < directorOrigin.y)
	{
		this->gameEnd(false);
	}
	else if (ballPosY - ballHeightHalf > directorOrigin.y + directorSize.height)
	{
		this->gameEnd(true);
	}
}

void HelloWorld::gameEnd(bool isWin)
{
	gameState = FINISHED;

	if (isWin)
		this->win();
	else
		this->lose();
}

void HelloWorld::win()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_WIN.c_str(), false, 1.0F, 0.0F, 0.2F);	// gain doesn't seem to work??
	level++;
}

void HelloWorld::lose()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_LOSE.c_str(), false, 1.0F, 0.0F, 0.2F);
}

void HelloWorld::moveMyBarAndBallToPosX(float posX)
{
    float posXBeforeMove = myBar->getPositionX();

    myBar->setPositionX(posX);
    if (myBar->getPositionX() + myBar->getBoundingBox().size.width/2 > directorOrigin.x + directorSize.width)
    {
        myBar->setPositionX(directorOrigin.x + directorSize.width - myBar->getBoundingBox().size.width/2);
    }
    else if (myBar->getPositionX() - myBar->getBoundingBox().size.width/2 < directorOrigin.x)
    {
        myBar->setPositionX(directorOrigin.x + myBar->getBoundingBox().size.width/2);
    }
    float displacement = myBar->getPositionX() - posXBeforeMove;

    ball->setPositionX(ball->getPositionX() + displacement);
}

void HelloWorld::preloadImages()
{
	// What is the proper way to preload all audio??
	auto images = std::vector<std::string>();
	images.push_back(SOUND_COLLISION_BALL_BAR);
	images.push_back(SOUND_COLLISION_BALL_EDGE);
	images.push_back(SOUND_WIN);
	images.push_back(SOUND_LOSE);

	for (auto &image : images)
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(image.c_str());
	}
}

#pragma endregion private methods
