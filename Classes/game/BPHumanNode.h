//
//  BPHumanNode.h
//  bankPanic
//
//  Created by forestj on 2014. 4. 29..
//  Copyright (c) 2014년 __MyCompanyName__. All rights reserved.
//

#ifndef bankPanic_BPHumanNode_
#define bankPanic_BPHumanNode_

#include "cocos2d.h"

USING_NS_CC;

typedef enum
{
    kHUMAN_TYPE_CITIZEN = 0,            //  시민
    kHUMAN_TYPE_GUNMEN,                 //  총잡이
    kHUMAN_TYPE_WANTED,                 //  현상수배자
    
}eHUMAN_TYPE;

class BPHumanNode : public cocos2d::CCNode
{

public:

	virtual ~BPHumanNode();
	virtual bool init();
    
    CREATE_FUNC(BPHumanNode);
    
    void appearHuman();
    
    
protected:
    virtual void onEnter();
    virtual void onExit();
    

public:
    CC_SYNTHESIZE_READONLY(eHUMAN_TYPE, m_eHumanType, HumanType);

protected:
    CCSprite                        *m_pHumanSprite;
    
};

#endif