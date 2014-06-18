//
//  BPScrollableBGLayer.h
//  bankPanic
//
//  Created by 전 홍림 on 2014. 6. 18..
//  Copyright (c) 2014년 __MyCompanyName__. All rights reserved.
//

#ifndef bankPanic_BPScrollableBGLayer_
#define bankPanic_BPScrollableBGLayer_

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BPScrollableBGLayer : public cocos2d::CCLayer
{

public:

	virtual ~BPScrollableBGLayer();
	bool init(const char *szBGImage, float velocity, int iBGImageCount);
    
    static BPScrollableBGLayer* createNode(const char *szBGImage, float velocity, int iBGImageCount);
    

    void moveScrolling(float fMoveX);

protected:
    
    float                       m_fVelocity;
    
    CCArray                     *m_arrBGImage;
    CCSpriteBatchNode           *m_pBGBatchNode;
    
};

#endif