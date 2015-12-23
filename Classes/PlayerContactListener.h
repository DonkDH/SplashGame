#pragma once
#include "Box2D/Box2D.h"
#include "HelloWorldScene.h"
#include "Player.h"

class Player;

class PlayerContactListener : public b2ContactListener
{
public:
	PlayerContactListener(Player* player);
	~PlayerContactListener();

	// Called when two fixtures begin to touch
	virtual void BeginContact(b2Contact* contact);

	// Called when two fixtures cease to touch
	virtual void EndContact(b2Contact* contact);

private:
	Player* m_player;
};

