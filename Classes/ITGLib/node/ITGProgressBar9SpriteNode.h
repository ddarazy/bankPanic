//
//  ITGProgressBar9SpriteNode.h
//
//
//  Created by forestj on 13. 5. 24..
//  Copyright (c) 2013년 IronTree Global. All rights reserved.
//

#ifndef ITG_ITGProgressBar9SpriteNode_
#define ITG_ITGProgressBar9SpriteNode_

#include "cocos2d.h"
#include "cocos-ext.h"



typedef enum  {
    kITGProgressBar9SpriteAlignHorizontal,
    kITGProgressBar9SpriteAlignVertical,
} ITGProgressBar9SpriteAlignment;

class ITGProgressBar9SpriteNode : public cocos2d::CCNode
{

public:


public:
	ITGProgressBar9SpriteNode();
	virtual ~ITGProgressBar9SpriteNode();
	virtual bool init();
    
    /**
        @brief
        inset 설정하는 방법
        CCRect.origin.x = 반복되지 않는 좌측 마지막 구간, 예를들어 x=8일 경우 반복되지 않는 구간의 width = 8이다.
        CCRect.origin.y = 반복되지 않는 좌측 마지막 구간, 예를들어 y=8일 경우 반복되지 않는 구간의 height = 8이다.
        CCRect.size.width = 반복되는 구간의 width
        CCRect.size.height = 반복되는 구간의 height
     
        결론적으로 inset의 설정 방법은 반복되는 구간을 설정하는 것이다. 설정에서 제외된 부분은 반복되지 않은 부분으로 판단하여 반복시키지 않는다.
     */
    static ITGProgressBar9SpriteNode* create(const cocos2d::CCSize &barSize, const char* pEmptyBarFileName, const char *pFillBarFileName, const cocos2d::CCRect &fileSize, const cocos2d::CCRect &inset);
    static ITGProgressBar9SpriteNode* create(const cocos2d::CCSize &barSize, cocos2d::extension::CCScale9Sprite *pEmptyBarSprite, cocos2d::extension::CCScale9Sprite *pFillBarSprite);
    
    /**
        sample source
     CCScale9Sprite *pEmpty = CCScale9Sprite::create("minigame_paper_yell.PNG", CCRectMake(0, 0, 23, 14), CCRectMake(8, 0, 7, 14));
     CCScale9Sprite *pFill = CCScale9Sprite::create("minigame_paper_blue.PNG", CCRectMake(0, 0, 23, 14), CCRectMake(8, 0, 7, 14));
     
     //  sprite를 미리 설정하여 생성하는 방법
     ZBProgressBar9SpriteNode *progress = ZBProgressBar9SpriteNode::create(CCSizeMake(400, 14), pEmpty, pFill);
     progress->setPosition(ccp(100, 400));
     addChild(progress, 50);
     progress->setPercentage(15);
     
     // 프로그래스바 생성시 설정값 포함 생성 방법
     ZBProgressBar9SpriteNode *progress = ZBProgressBar9SpriteNode::create(CCSizeMake(400, 14), "minigame_paper_yell.PNG", "minigame_paper_blue.PNG", CCRectMake(0, 0, 23, 14), CCRectMake(8, 0, 7, 14));
     progress->setAnchorPoint(ccp(0, 0));
     addChild(progress, 50);
     progress->setPercentage(5);

     */

    inline ITGProgressBar9SpriteAlignment getAlignment(void)    {   return m_eAlignment;    };
    inline float getPercentage(void)    {   return m_fPercentage;   };
    inline cocos2d::extension::CCScale9Sprite* getFillSprite(void)    {   return m_pFillSprite;   };
    inline cocos2d::extension::CCScale9Sprite* getEmptySprite(void)   {   return m_pEmptySprite;  };
    
    bool initWithSprite(cocos2d::extension::CCScale9Sprite *emptyBarSprite, cocos2d::extension::CCScale9Sprite *fillBarSprite);

    //  no animation
    void setPercentage(float fPercentage);
    //  animation enable
    void setPercentage(float fPercentage, bool animate);
    void setSprite(cocos2d::extension::CCScale9Sprite *pEmptySprite, cocos2d::extension::CCScale9Sprite *pFillSprite);

    void setAlignment(ITGProgressBar9SpriteAlignment eAlignment);
    
    /**
        @brief percent로 예상했을 때의 프로그래스 바 사이즈
    */
    cocos2d::CCSize getExpectablePercentageSize(float fExpectPercent);
    
    cocos2d::CCSize getPercentageSize() {   return m_pFillSprite->getPreferredSize();  };

#pragma mark protected method
protected:
    void updateProgress(bool animate);
    
    
#pragma mark protected variable
protected:
    ITGProgressBar9SpriteAlignment m_eAlignment;
    float m_fPercentage;
    cocos2d::extension::CCScale9Sprite *m_pFillSprite;
    cocos2d::extension::CCScale9Sprite *m_pEmptySprite;
    
    bool m_bReverseDirection;
};

#endif