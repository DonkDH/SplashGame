#include "Player.h"



Player::Player() : m_currentlyLiquid(false), m_switchingState(false)
{
	m_body = NULL;
	m_sprite = NULL;
	m_fixture = NULL;

	m_jumpTime = std::chrono::high_resolution_clock::now();
}


Player::~Player()
{
	m_body = NULL;
	m_sprite->removeFromParentAndCleanup(true);
	m_fixture = NULL;
}

void Player::Init(b2World* world, const b2Vec2 startPos, cocos2d::Layer* layer, PlayerContactListener* contactListener )
{
	m_playerScale = 1.0f;

	m_sprite = cocos2d::Sprite::create("whiteSprite.png");
	m_sprite->setPosition(cocos2d::Vec2(startPos.x, startPos.y));
	m_sprite->setScale(cocos2d::Director::getInstance()->getContentScaleFactor());
	layer->addChild(m_sprite);

	m_bodyDef.type = b2_dynamicBody;
	m_bodyDef.position.Set(startPos.x / SCALE_RATIO, startPos.y / SCALE_RATIO);
	m_bodyDef.userData = m_sprite;
	m_bodyDef.angularDamping = 1;
	m_bodyDef.fixedRotation = true;
	m_body = world->CreateBody(&m_bodyDef);

	m_shape = b2PolygonShape();
	m_shape.SetAsBox(m_playerSize.x * m_playerScale, m_playerSize.y * m_playerScale);
	

	m_fixtureDef.shape = &m_shape;
	m_fixtureDef.density = 0.5f;
	m_fixtureDef.friction = 0.4f;
	m_fixtureDef.restitution = 0.1f;

	m_fixture = m_body->CreateFixture(&m_fixtureDef);
	
	userData = new GameConstants::FloorUserData();
	userData->isFloor = false;
	userData->isPlayer = true;
	m_fixture->SetUserData(userData);
	
	m_contactListener = contactListener;
	world->SetContactListener(m_contactListener);


	m_particleGroupDef.flags = b2_waterParticle;
	m_particleGroupDef.shape = &m_shape;
	m_particleGroupDef.color.Set(rand() % 128 + 128, 0, 0, 255);
}

void Player::Move(float Speed)
{
	if (!m_currentlyLiquid)
	{
		float maxVelocvity = 10000.0f;
		if (m_body != NULL) {
			m_body->ApplyForce(b2Vec2(Speed, 0), m_body->GetPosition(), true);
			b2Vec2 velocity = m_body->GetLinearVelocity();
			if (velocity.x > maxVelocvity) {
				m_body->SetLinearVelocity(b2Vec2(maxVelocvity, velocity.y));
			}
			else if (velocity.x < -maxVelocvity) {
				m_body->SetLinearVelocity(b2Vec2(-maxVelocvity, velocity.y));
			}
		}
	}
}

void Player::Jump()
{
	if (!m_currentlyLiquid)
	{
		double lastJumpTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::high_resolution_clock::now() - m_jumpTime).count();
		if (m_onFloor && lastJumpTime > 500) {
			m_body->ApplyLinearImpulse(b2Vec2(0, 40.0f), m_body->GetPosition(), true);
			m_jumpTime = std::chrono::high_resolution_clock::now();
		}
	}
}

void Player::SwitchForm(cocos2d::Layer * layer, b2World * world, b2ParticleSystem * particleSystem)
{
	if (!m_switchingState) {
		if (!m_currentlyLiquid)
		{
			BecomeLiquid(layer, world, particleSystem);
		}
		else
		{
			BecomeSolid(layer, world, particleSystem);
		}
	}
}

void Player::Update(b2ParticleSystem* particleSystem)
{
	if (m_switchingState) {
		m_switchingState = particleSystem->GetParticleCount() > 0;

		if (m_switchingState) {
			m_playerScale = 0.1f + (0.9f * (1.0f - ((1.0f / (float)m_particleCreateCount) * particleSystem->GetParticleCount())));
		}
		else {
			m_playerScale = 1.0f;
		}

		b2PolygonShape* shape = (b2PolygonShape*)m_fixture->GetShape();
		shape->SetAsBox(m_playerSize.x * m_playerScale, m_playerSize.y * m_playerScale);
		m_sprite->setScale(m_playerScale * cocos2d::Director::getInstance()->getContentScaleFactor());
	}
}

void Player::BecomeLiquid(cocos2d::Layer* layer, b2World* world, b2ParticleSystem * particleSystem)
{
	world->DestroyBody(m_body);
	
	m_sprite->retain();
	m_sprite->removeFromParent();

	b2Vec2 pos = GetPhysicsLocaton();
	m_particleGroupDef.position.Set(pos.x, pos.y);

	particleSystem->CreateParticleGroup(m_particleGroupDef);
	particleSystem->ApplyLinearImpulse(0, particleSystem->GetParticleCount(), m_body->GetLinearVelocity() * 15);

	m_currentlyLiquid = true;
	m_switchingState = false;
	m_particleCreateCount = particleSystem->GetParticleCount();
}

void Player::BecomeSolid(cocos2d::Layer * layer, b2World* world, b2ParticleSystem * particleSystem)
{
	m_playerScale = 0.1f;
	m_switchingState = true;
	const b2Vec2* posList = particleSystem->GetPositionBuffer();
	const int particleCount = particleSystem->GetParticleCount();

	b2Vec2 averageCenter = b2Vec2(0,0);
	for (int i = 0; i < particleCount; ++i)
	{
		averageCenter.x += posList[i].x;
		averageCenter.y += posList[i].y;
	}

	averageCenter.x /= ((float)particleCount);
	averageCenter.y /= ((float)particleCount);

	layer->addChild(m_sprite);
	m_sprite->setScaleX(m_playerScale);
	m_sprite->setScaleY(m_playerScale);

	m_bodyDef.position = averageCenter;

	m_body = world->CreateBody(&m_bodyDef);

	m_fixture = m_body->CreateFixture(&m_fixtureDef);
	m_fixture->SetUserData(userData);
	b2PolygonShape* shape = (b2PolygonShape*)m_fixture->GetShape();
	shape->SetAsBox(m_playerSize.x * m_playerScale, m_playerSize.y * m_playerScale);

	m_sprite->setPosition(cocos2d::Vec2(averageCenter.x * SCALE_RATIO, averageCenter.y * SCALE_RATIO));
	m_currentlyLiquid = false;
}



b2Vec2 Player::GetPhysicsLocaton()
{
	if (!m_currentlyLiquid)
	{
		return m_body->GetPosition();
	}
}

void Player::OnFloor(bool onFloor)
{
	m_onFloor = onFloor;
}

bool Player::GetCurrentlyLiquid()
{
	return m_currentlyLiquid;
}


