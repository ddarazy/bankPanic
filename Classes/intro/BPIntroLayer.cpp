//
//  BPIntroLayer.cpp
//  bankPanic
//
//  Created by forestj on 2014. 4. 23..
//  Copyright (c) 2014년 __MyCompanyName__. All rights reserved.
//

#include "BPIntroLayer.h"
#include "ITGUtil.h"
#include "BPGameLayer.h"

BPIntroLayer::~BPIntroLayer()
{
}

CCScene* BPIntroLayer::scene()
{
    CCScene *scene = CCScene::create();
    if (scene) {
        BPIntroLayer *node = (BPIntroLayer*)ITGUtil::sharedUtils()->loadCustomLayerFromCCBI<BPIntroLayer>("BPIntroLayer", "BPIntroLayer.ccbi", NULL, NULL);
        if (node) {
            scene->addChild(node);
        }
    }
    return scene;
}


bool BPIntroLayer::init()
{
    if (!CCLayer::init()) {
        return false;
    }
    setTouchEnabled(true);
	return true;
}

void BPIntroLayer::onEnter()
{
    CCLayer::onEnter();
}

void BPIntroLayer::onExit()
{
    CCLayer::onExit();
}

void BPIntroLayer::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    
}

void BPIntroLayer::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    CCDirector::sharedDirector()->replaceScene(BPGameLayer::scene());
}