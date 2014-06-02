//
//  BPIntroLayer.h
//  bankPanic
//
//  Created by forestj on 2014. 4. 23..
//  Copyright (c) 2014년 __MyCompanyName__. All rights reserved.
//

#ifndef bankPanic_BPIntroLayer_
#define bankPanic_BPIntroLayer_

#include "cocos2d.h"

USING_NS_CC;

class BPIntroLayer : public CCLayer
{

public:

	virtual ~BPIntroLayer();
	virtual bool init();
    
    CREATE_FUNC(BPIntroLayer);
    
    static CCScene* scene();
    
protected:
    
    virtual void onEnter();
    virtual void onExit();
    
    
    virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);

};

#endif