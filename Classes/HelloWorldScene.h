#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#define COCOS2D_DEBUG 1

#include "cocos2d.h"
#include "ui\CocosGUI.h"
#include "Box2D\Box2D.h"
#include "GLES-Render.h"
#include "GameConstants.h"
#include "Player.h"
#include "b2djson\b2dJson.h"
#include <map>

class Player;

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
	void update(float) override;

	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) ;
	
	virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchMoved(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchCancelled(cocos2d::Touch*, cocos2d::Event*);

#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )
	void KeyboardPressedInputHandler(cocos2d::EventKeyboard::KeyCode keyCode);
	void KeyboardReleasedInputHandler(cocos2d::EventKeyboard::KeyCode keyCode);


	std::map<cocos2d::EventKeyboard::KeyCode, std::chrono::high_resolution_clock::time_point> m_heldKeys;
	std::map<cocos2d::EventKeyboard::KeyCode, bool> m_pressedKeys;
	std::map<cocos2d::EventKeyboard::KeyCode, bool> m_releasedKeys;

	bool KeyHeld(cocos2d::EventKeyboard::KeyCode key);
	bool KeyPressed(cocos2d::EventKeyboard::KeyCode key);
	double TimeKeyPressedFor(cocos2d::EventKeyboard::KeyCode key);
#endif
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);


	b2World* _world;
	b2Draw* m_b2dDebugDraw;

	Player* player;

	b2ParticleGroupDef group;
	b2ParticleSystemDef particleSystemDef;
	b2PolygonShape paddleShape = b2PolygonShape();
	b2ParticleSystem* systemA;

	cocos2d::Vec2 touchPos = cocos2d::Vec2();

	GameConstants::FloorUserData* floorData;

	bool movingLeft = false;
	bool movingRight = false;
};

#endif // __HELLOWORLD_SCENE_H__
