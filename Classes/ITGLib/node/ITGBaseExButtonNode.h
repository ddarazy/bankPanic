//
//  ITGBaseExButtonNode.h
//  Conan
//
//  Created by forestj on 13. 10. 1..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#ifndef Conan_ITGBaseExButtonNode_
#define Conan_ITGBaseExButtonNode_

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class ITGBaseExButtonNodeDelegate {
    
    
public:
    virtual void touchButtonIndex(cocos2d::CCNode *pSender, int buttonID){};
    virtual void touchButtonNoneSelectedIndex(cocos2d::CCNode *pSender, int buttonID){};
    virtual void longPressButtonIndex(cocos2d::CCNode *pSender, int buttonID){};
    virtual void longPressButtonReleaseIndex(cocos2d::CCNode *pSender, int buttonID){};

};

class ITGBaseExButtonNode : public cocos2d::CCNode
{

public:
	static ITGBaseExButtonNode* create();

public:
	ITGBaseExButtonNode();
	virtual ~ITGBaseExButtonNode();
	virtual bool init();
    
    virtual void OnBtnPress()=0;
    virtual void OnBtnRelease()=0;
    virtual void OnBtnTouchOut()=0;

    
    
public:
    CC_SYNTHESIZE(ITGBaseExButtonNodeDelegate*, m_pDelegate, Delegate);
    CC_SYNTHESIZE(int, m_iButtonId, ButtonId);
};

#endif