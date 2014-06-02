//
//  BPDoorNode.h
//  bankPanic
//
//  Created by 전 홍림 on 2014. 4. 24..
//  Copyright (c) 2014년 __MyCompanyName__. All rights reserved.
//

#ifndef bankPanic_BPDoorNode_
#define bankPanic_BPDoorNode_

#include "cocos2d.h"
#include "cocos-ext.h"


USING_NS_CC;
USING_NS_CC_EXT;

class BPHumanNode;

class BPDoorNode : public cocos2d::CCNode
, public CCBMemberVariableAssigner
{
public:

	virtual ~BPDoorNode();
	virtual bool init();
    
    CREATE_FUNC(BPDoorNode);
    static BPDoorNode* createNode();
    
    void setOpenDoor(bool bOpen);
    bool IsOpenDoor(){return m_bOpenDoor;};
    
    void setDoorNumber(uint32_t doorNumber);
    
protected:
    virtual void onEnter();
    virtual void onExit();
    

#pragma mark CCBMemberVariableAssigner
    virtual bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
    {
        CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pEnemyAppearNode", CCNode*, m_pEnemyAppearNode);
        CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pDoorNumber", CCLabelTTF*, m_pDoorNumber);
        return false;
    };
    
    void initObjects();
    

public:
    CC_SYNTHESIZE(bool, m_bEnableOpenDoor, EnableOpenDoor);
    CC_SYNTHESIZE_READONLY(uint32_t, m_iDoorNumber, DoorNumber);
protected:
    CCBAnimationManager                     *m_pAnimationMng;
    CCNode                                  *m_pEnemyAppearNode;
    CCLabelTTF                              *m_pDoorNumber;
    bool                                    m_bOpenDoor;
    
    BPHumanNode                             *m_pHumanNode;
};

#endif