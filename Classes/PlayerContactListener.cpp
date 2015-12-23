#include "PlayerContactListener.h"

PlayerContactListener::PlayerContactListener(Player* player) : m_player(player)
{
}


PlayerContactListener::~PlayerContactListener()
{
}

void PlayerContactListener::BeginContact(b2Contact * contact)
{
	GameConstants::FloorUserData* dataA = (GameConstants::FloorUserData*)contact->GetFixtureA()->GetUserData();
	GameConstants::FloorUserData* dataB = (GameConstants::FloorUserData*)contact->GetFixtureB()->GetUserData();

	if ( (dataA != NULL && dataA->isFloor) || (dataB != NULL && dataB->isFloor))
	{
		CCLOG("BeginContact");
		m_player->OnFloor(true);
	}
}

void PlayerContactListener::EndContact(b2Contact * contact)
{
	GameConstants::FloorUserData* dataA = (GameConstants::FloorUserData*)contact->GetFixtureA()->GetUserData();
	GameConstants::FloorUserData* dataB = (GameConstants::FloorUserData*)contact->GetFixtureB()->GetUserData();

	if ((dataA != NULL && dataA->isFloor) || (dataB != NULL && dataB->isFloor))
	{
		CCLOG("EndContact");
		m_player->OnFloor(false);
	}
}
