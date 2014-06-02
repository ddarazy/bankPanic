//
//  ITGModalEnableLayer.h
//  Conan
//
//  Created by forestj on 13. 9. 30..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#ifndef Conan_ITGModalEnableLayer_
#define Conan_ITGModalEnableLayer_

#include "cocos2d.h"

#define kModalViewTag 987456

USING_NS_CC;

class ITGModalEnableLayer : public cocos2d::CCLayer
{

public:
	ITGModalEnableLayer();
	virtual ~ITGModalEnableLayer();
	virtual bool init();
    
    CREATE_FUNC(ITGModalEnableLayer);
    
    void addModal(cocos2d::CCNode *child, int zOrder);
    void removeModal(cocos2d::CCNode *child, bool cleanup);
    void removeModal(bool cleanup);
    
    virtual void registerWithTouchDispatcher(void){};
    
protected:
    bool isModal(){return m_isModal;}
    
private:
    void saveTouchesRecursive(CCNode *pNode);
    void touchesRecursive();
    
private:
    std::vector<CCTouchDelegate*> m_arrSavedTouches;
    bool m_isModal;

};

#endif