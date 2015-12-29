#pragma once
#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "GameConstants.h"
#include "PlayerContactListener.h"
#include "HelloWorldScene.h"

class PlayerContactListener;

class Player
{
public:
	Player();
	~Player();

	void Init(b2World* world, const b2Vec2 startPos, cocos2d::Layer* layer, PlayerContactListener* contactListener);
	void Move(float Speed);
	void Jump();
	void SwitchForm(cocos2d::Layer* layer, b2World* world, b2ParticleSystem* particleSystem);

	void Update(b2ParticleSystem* particleSystem);

	b2Vec2 GetPhysicsLocaton();

	void OnFloor(bool onFloor); 
	
	bool GetCurrentlyLiquid();

private:
	void BecomeLiquid(cocos2d::Layer* layer, b2World* world, b2ParticleSystem* particleSystem);
	void BecomeSolid(cocos2d::Layer* layer, b2World* world, b2ParticleSystem* particleSystem);

	cocos2d::Sprite* m_sprite;

	b2BodyDef m_bodyDef;
	b2Body *m_body;
	b2Vec2 m_bodyCenter;

	b2PolygonShape m_shape;
	b2FixtureDef m_fixtureDef;
	b2Fixture *m_fixture;

	PlayerContactListener* m_contactListener;
	bool m_onFloor = false;

	std::chrono::high_resolution_clock::time_point m_jumpTime;

	b2ParticleGroupDef m_particleGroupDef;
	bool m_currentlyLiquid;
	bool m_switchingState;
	int m_particleCreateCount;

	float m_playerScale;
	const b2Vec2 m_playerSize = b2Vec2(32.5f / SCALE_RATIO, 40.5f / SCALE_RATIO);
	GameConstants::FloorUserData* userData;
};

