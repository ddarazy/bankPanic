//
//  ITGTypingLabelNode.h
//  Conan
//
//  Created by forestj on 13. 10. 18..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#ifndef Conan_ITGTypingLabelNode_
#define Conan_ITGTypingLabelNode_

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

class ITGTypingLabelNodeDelegate {
    
public:

    /**
        @brief 타이핑 애니메이션이 종료될 때 호출되는 함수 - Typing Animation Delegate - Call animation ended
        경우에 따라 타이핑이 종료되는 시점이 필요한 경우가 있음. 예를 들면 종료가 호출되고 스킵 버튼을 보여주어야 할 경우 등 다양한 경우가 있을 수 있음)
     */
    virtual void typtingLabelNodeAnimationEnded(){};
    /**
        @brief 타이핑 한글자마다 변경되고 있음을 알려준다. 이곳에서 타이핑 이펙트 사운드를 사용하면 좋음.
     */
    virtual void typingLabelNodeChanged(){};
};

class ITGTypingLabelNode : public CCNode
{

public:
    
    static ITGTypingLabelNode * create(const char *string, const char *fontName, float fontSize);
    
	virtual ~ITGTypingLabelNode();
	virtual bool init();
    
    CREATE_FUNC(ITGTypingLabelNode);
    
    virtual void onEnter();
    virtual void onExit();
    
    /**
        @brief set will typing animtion text -> start animation
     */
    void setTypingString(const char *label);
    /**
        @brief force animation end
     */
    bool typingStringEnd();
    /**
        @brief CCLabelText setDimentions & this(CCNode) setContentsSize
     */
    void setDimentionSize(CCSize &tDimention);
    
    void setHorizontalAlignment(CCTextAlignment hAlignment);
    
    void setVerticalAlignment(CCVerticalTextAlignment vAlignment);

    
    virtual void update(float delta);
    
    // reset label
    void resetLabelString();
    
public:
    CC_SYNTHESIZE(float, m_fTypingInterval, TypingInterval);
    CC_SYNTHESIZE_READONLY(bool, m_bTyping, Typing);
    CC_SYNTHESIZE(ITGTypingLabelNodeDelegate*, m_pDelegate, Delegate);
protected:
    string m_pCurLabelString;
    
    unsigned int m_iCurLabelCount;
    unsigned int m_iTotalLabelCount;
    
    CCLabelTTF          *m_pLabel;
    float               m_fCurDelta;

};

#endif