#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

int DRAG_BODYS_TAG = 0;

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	auto body = PhysicsBody::createBox(Size(65.0f, 65.0f), PHYSICSBODY_MATERIAL_DEFAULT);
	body->setPositionOffset(Vec2(2.0f, 2.0f));

	auto touchListener = EventListenerTouchOneByOne::create();

	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	auto keyboardPressedEventListener = EventListenerKeyboard::create();
	keyboardPressedEventListener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event)
	{
#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )
		((HelloWorld*)event->getCurrentTarget())->KeyboardPressedInputHandler(keyCode);
#endif
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardPressedEventListener, this);

	auto keyboardReleasedEventListener = EventListenerKeyboard::create();
	keyboardReleasedEventListener->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event* event)
	{
#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )
		((HelloWorld*)event->getCurrentTarget())->KeyboardReleasedInputHandler(keyCode);
#endif
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardReleasedEventListener, this);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// Close
	{
		auto closeItem = MenuItemImage::create(
			"CloseNormal.png",
			"CloseSelected.png",
			CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

		closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
			origin.y + visibleSize.height - closeItem->getContentSize().height / 2));

		// create menu, it's an autorelease object
		auto menu = Menu::create(closeItem, NULL);
		menu->setPosition(Vec2::ZERO);
		this->addChild(menu, 1);
	}
#if ( CC_TARGET_PLATFORM != CC_PLATFORM_WIN32 )
	// Jump
	{
		auto button = ui::Button::create("UpArrow.png", "UpArrow.png", "UpArrow.png");
		button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
			switch (type)
			{
			case ui::Widget::TouchEventType::ENDED:
				player->Jump();
				break;
			default:
				break;
			}
		});

		button->setPosition(Vec2(origin.x + visibleSize.width - (button->getContentSize().width / 2),
			origin.y + (button->getContentSize().height / 2)));

		this->addChild(button);
	}

	// Switch Form
	{
		auto button = ui::Button::create("WaterIcon.png", "WaterIcon.png", "WaterIcon.png");
		button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
			switch (type)
			{
			case ui::Widget::TouchEventType::ENDED:
				player->SwitchForm(this, _world, systemA);
				break;
			default:
				break;
			}
		});

		button->setPosition(Vec2(origin.x + visibleSize.width - (button->getContentSize().width / 2),
			origin.y + 10 + button->getContentSize().height + (button->getContentSize().height / 2)));

		this->addChild(button);
	}

	// Move Left
	{
		auto button = ui::Button::create("LeftArrow.png", "LeftArrow.png", "LeftArrow.png");
		button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
			switch (type)
			{
			case ui::Widget::TouchEventType::BEGAN:
				movingLeft = true;
				break;
			case ui::Widget::TouchEventType::ENDED:
				movingLeft = false;
				break;
			case ui::Widget::TouchEventType::CANCELED:
				movingLeft = false;
				break;
			default:
				break;
			}
		});

		button->setPosition(Vec2(origin.x + (button->getContentSize().width / 2),
			origin.y + (button->getContentSize().height / 2)));

		this->addChild(button);
	}
	// Move Left
	{
		auto button = ui::Button::create("RightArrow.png", "RightArrow.png", "RightArrow.png");
		button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
			switch (type)
			{
			case ui::Widget::TouchEventType::BEGAN:
				movingRight = true;
				break;
			case ui::Widget::TouchEventType::ENDED:
				movingRight = false;
				break;
			case ui::Widget::TouchEventType::CANCELED:
				movingRight = false;
				break;
			default:
				break;
			}
		});

		button->setPosition(Vec2(origin.x + 10 + button->getContentSize().width + (button->getContentSize().width / 2),
			origin.y + (button->getContentSize().height / 2)));

		this->addChild(button);
	}
#endif

	b2Vec2 gravity;
	gravity.Set(0.0f, -9.8f);
	
	_world = new b2World(gravity);
	_world->SetAllowSleeping(true);

	m_b2dDebugDraw = new GLESDebugDraw(SCALE_RATIO);

	m_b2dDebugDraw->SetFlags(b2Draw::e_shapeBit
		| b2Draw::e_jointBit
		| b2Draw::e_aabbBit
		| b2Draw::e_pairBit
		| b2Draw::e_centerOfMassBit
		| b2Draw::e_particleBit);

	_world->SetDebugDraw(m_b2dDebugDraw);


	const cocos2d::Vec2 s_centre = Director::getInstance()->getOpenGLView()->getVisibleSize() * 0.5f;
	const cocos2d::Vec2 s_origin = Director::getInstance()->getOpenGLView()->getVisibleOrigin();
	CCLOG("s_origin (%F, %F)", s_origin.x, s_origin.y);
//Walls
	floorData = new GameConstants::FloorUserData();
	{
		b2Body* walls;
		b2BodyDef wallsDef;
		wallsDef.type = b2BodyType::b2_staticBody;
		wallsDef.position.Set(s_centre.x / SCALE_RATIO, (s_origin.y + 10) / SCALE_RATIO);
		//wallsDef.userData = sprite;
		walls = _world->CreateBody(&wallsDef);

		b2EdgeShape wallsShape = b2EdgeShape();
		wallsShape.Set(b2Vec2(-(s_centre.x / SCALE_RATIO), 0), b2Vec2((s_centre.x / SCALE_RATIO), 0));

		b2FixtureDef wallsShapeDef;
		wallsShapeDef.shape = &wallsShape;
		wallsShapeDef.density = 10.0f;
		wallsShapeDef.friction = 0.4f;
		wallsShapeDef.restitution = 0.1f;

		b2Fixture *_wallFixture;
		_wallFixture = walls->CreateFixture(&wallsShapeDef);
		_wallFixture->SetUserData(floorData);
	}
	{
		b2Body* walls;
		b2BodyDef wallsDef;
		wallsDef.type = b2BodyType::b2_staticBody;
		wallsDef.position.Set(s_centre.x / SCALE_RATIO, (s_origin.y + s_centre.y) / SCALE_RATIO);
		//wallsDef.userData = sprite;
		walls = _world->CreateBody(&wallsDef);

		b2EdgeShape wallsShape = b2EdgeShape();
		wallsShape.Set(b2Vec2(-(s_centre.x / SCALE_RATIO), (s_centre.y / SCALE_RATIO)), b2Vec2((s_centre.x / SCALE_RATIO), (s_centre.y / SCALE_RATIO)));

		b2FixtureDef wallsShapeDef;
		wallsShapeDef.shape = &wallsShape;
		wallsShapeDef.density = 10.0f;
		wallsShapeDef.friction = 0.4f;
		wallsShapeDef.restitution = 0.1f;

		b2Fixture *_wallFixture;
		_wallFixture = walls->CreateFixture(&wallsShapeDef);
		_wallFixture->SetUserData(floorData);
	}
	{
		b2Body* walls;
		b2BodyDef wallsDef;
		wallsDef.type = b2BodyType::b2_staticBody;
		wallsDef.position.Set(s_centre.x / SCALE_RATIO, (s_origin.y + s_centre.y) / SCALE_RATIO);
		//wallsDef.userData = sprite;
		walls = _world->CreateBody(&wallsDef);

		b2EdgeShape wallsShape = b2EdgeShape();
		wallsShape.Set(b2Vec2(-(s_centre.x / SCALE_RATIO), -(s_centre.y / SCALE_RATIO)), b2Vec2(-(s_centre.x / SCALE_RATIO), (s_centre.y / SCALE_RATIO)));

		b2FixtureDef wallsShapeDef;
		wallsShapeDef.shape = &wallsShape;
		wallsShapeDef.density = 10.0f;
		wallsShapeDef.friction = 0.4f;
		wallsShapeDef.restitution = 0.1f;

		b2Fixture *_wallFixture;
		_wallFixture = walls->CreateFixture(&wallsShapeDef);
		_wallFixture->SetUserData(floorData);
	}
	{
		b2Body* walls;
		b2BodyDef wallsDef;
		wallsDef.type = b2BodyType::b2_staticBody;
		wallsDef.position.Set(s_centre.x / SCALE_RATIO, (s_origin.y + s_centre.y) / SCALE_RATIO);
		//wallsDef.userData = sprite;
		walls = _world->CreateBody(&wallsDef);

		b2EdgeShape wallsShape = b2EdgeShape();
		wallsShape.Set(b2Vec2((s_centre.x / SCALE_RATIO), -(s_centre.y / SCALE_RATIO)), b2Vec2((s_centre.x / SCALE_RATIO), (s_centre.y / SCALE_RATIO)));

		b2FixtureDef wallsShapeDef;
		wallsShapeDef.shape = &wallsShape;
		wallsShapeDef.density = 10.0f;
		wallsShapeDef.friction = 0.4f;
		wallsShapeDef.restitution = 0.1f;

		b2Fixture *_wallFixture;
		_wallFixture = walls->CreateFixture(&wallsShapeDef);
		_wallFixture->SetUserData(floorData);
	}

	player = new Player();
	PlayerContactListener* playerContactListener = new PlayerContactListener(player);
	player->Init(_world, b2Vec2( s_centre.x, s_centre.y), (Layer*)this, playerContactListener);

	//particals
	{
		particleSystemDef.radius = 3.0f / SCALE_RATIO;
		particleSystemDef.density = .5f;
		particleSystemDef.strictContactCheck = false;
		particleSystemDef.repulsiveStrength = 2;
		particleSystemDef.powderStrength = 0;
		particleSystemDef.dampingStrength = 0;

		systemA = _world->CreateParticleSystem(&particleSystemDef);

		
		paddleShape.SetAsBox(32.5f / SCALE_RATIO, 40.5f / SCALE_RATIO);

		group.flags = b2_waterParticle;
		
		group.shape = &paddleShape;
		group.color.Set(rand() % 128 + 128, rand() % 128 + 128, rand() % 128 + 128, 255);
		group.position.Set(s_centre.x / SCALE_RATIO, ((s_centre.y * 2 ) / 3) * 2 / SCALE_RATIO);

		//systemA->CreateParticleGroup(group);
		//systemA->ApplyLinearImpulse(0, 800, b2Vec2(5000, 5000));
	}

	this->scheduleUpdate();
    return true;
}

void HelloWorld::update(float deltaTime)
{
	int velocityIterations = 8;
	int positionIterations = 1;

	// Instruct the world to perform a single step of simulation. It is
	// generally best to keep the time step and iterations fixed.
	_world->Step(deltaTime, velocityIterations, positionIterations);

	bool blockFound = false;

	// Iterate over the bodies in the physics world
	for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != NULL) {
			// Synchronize the AtlasSprites position and rotation with the corresponding body
			CCSprite* myActor = (CCSprite*)b->GetUserData();
			myActor->setPosition(CCPointMake(b->GetPosition().x * SCALE_RATIO, b->GetPosition().y * SCALE_RATIO));
			myActor->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));

			if (myActor->getTag() == 1)
			{
				static int maxSpeed = 10;

				b2Vec2 velocity = b->GetLinearVelocity();
				float32 speed = velocity.Length();

				if (speed > maxSpeed) {
					b->SetLinearDamping(0.5);
				}
				else if (speed < maxSpeed) {
					b->SetLinearDamping(0.0);
				}
			}

			if (myActor->getTag() == 2) {
				blockFound = true;
			}
		}
	}

	if (!player->GetCurrentlyLiquid()) {
		//const b2Vec2 center = b2Vec2(touchPos.x, touchPos.y) / SCALE_RATIO;
		const b2Vec2 center = player->GetPhysicsLocaton();
		const b2Vec2* positions = systemA->GetPositionBuffer();
		int count = systemA->GetParticleCount();
		for (int i = 0; i < count; ++i)
		{
			b2Vec2 directon = center - positions[i];
			float xMag = directon.x * directon.x;
			float yMag = directon.y * directon.y;
			float mag = sqrtf(xMag + yMag);
			directon.x /= mag;
			directon.y /= mag;

			const b2Vec2 force = directon * 0.1f;

			systemA->ParticleApplyForce(i, force);
		}

		const b2ParticleBodyContact* contacts = systemA->GetBodyContacts();
		int contactCount = systemA->GetBodyContactCount();

		for (int i = 0; i < contactCount; ++i)
		{
			GameConstants::FloorUserData* data = (GameConstants::FloorUserData*)contacts[i].fixture->GetUserData();
			if ( data != NULL && data->isPlayer) {
				systemA->DestroyParticle(contacts[i].index);
			}
		}
	}

	player->Update(systemA);

#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )
	if (KeyHeld(cocos2d::EventKeyboard::KeyCode::KEY_W) || KeyHeld(cocos2d::EventKeyboard::KeyCode::KEY_SPACE))
	{
		player->Jump();
	}
	if (KeyHeld(cocos2d::EventKeyboard::KeyCode::KEY_D))
	{
		player->Move(50);
	}
	if (KeyHeld(cocos2d::EventKeyboard::KeyCode::KEY_A))
	{
		player->Move(-50);
	}
	if (KeyPressed(cocos2d::EventKeyboard::KeyCode::KEY_E)) {
		player->SwitchForm(this, _world, systemA);
	}

	m_pressedKeys.clear();
#else
	if (movingLeft) {
		player->Move(-50);
	}
	if (movingRight) {
		player->Move(50);
	}
#endif
}

void HelloWorld::draw(cocos2d::Renderer* renderer, const Mat4& transform, uint32_t flags)
{
	_world->DrawDebugData();
}

bool HelloWorld::onTouchBegan(cocos2d::Touch * touch, cocos2d::Event *)
{
	touchPos = touch->getLocation();
	return true;
}

void HelloWorld::onTouchEnded(cocos2d::Touch * touch, cocos2d::Event *)
{
	//systemA->CreateParticleGroup(group);
	//player->Jump();
	//CCLOG("This Work On Android");
}

void HelloWorld::onTouchMoved(cocos2d::Touch * touch, cocos2d::Event *)
{
	touchPos = touch->getLocation();

}

void HelloWorld::onTouchCancelled(cocos2d::Touch * touch, cocos2d::Event *)
{
}

#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )
void HelloWorld::KeyboardPressedInputHandler(cocos2d::EventKeyboard::KeyCode keyCode)
{
	m_heldKeys.insert_or_assign(keyCode, std::chrono::high_resolution_clock::now());
	m_pressedKeys.insert_or_assign(keyCode, true);
}

void HelloWorld::KeyboardReleasedInputHandler(cocos2d::EventKeyboard::KeyCode keyCode)
{
	m_heldKeys.erase(keyCode);
}

bool HelloWorld::KeyHeld(cocos2d::EventKeyboard::KeyCode key)
{
	return m_heldKeys.find(key) != m_heldKeys.end();
}

bool HelloWorld::KeyPressed(cocos2d::EventKeyboard::KeyCode key)
{
	return m_pressedKeys.find(key) != m_pressedKeys.end();
}

double HelloWorld::TimeKeyPressedFor(cocos2d::EventKeyboard::KeyCode key)
{
	if(!KeyHeld(key))
		return 0.0;

	return std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::high_resolution_clock::now() - m_heldKeys[key]).count();
}
#endif

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
