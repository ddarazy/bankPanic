//
//  BPElevatorNode.h
//  bankPanic
//
//  Created by 전 홍림 on 2014. 4. 26..
//  Copyright (c) 2014년 __MyCompanyName__. All rights reserved.
//

#ifndef bankPanic_BPElevatorNode_
#define bankPanic_BPElevatorNode_

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BPElevatorNode;

class BPElevatorNodeDelegate
{
public:
    virtual void arrivedHuman(BPElevatorNode *pElevator)=0;
};

class BPElevatorNode : public cocos2d::CCNode
, public CCBMemberVariableAssigner
{

public:
	virtual ~BPElevatorNode();
	virtual bool init();
    
    CREATE_FUNC(BPElevatorNode);

    static BPElevatorNode* createNode();
    
    void startAppearHuman();
    void resetAppearHuman();
    
    bool IsArrivedHuman(){return m_bArrivedHuman;};
    
    void setElevatorNumber(uint32_t eNumber);
    
protected:
    virtual void onEnter();
    virtual void onExit();
    
#pragma mark CCBMemberVariableAssigner
    virtual bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
    {
        CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pAlertBoardNode", CCLayerColor*, m_pAlertBoardNode);
        CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pAlertLabelNode", CCLabelTTF*, m_pAlertLabelNode);
        CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pSpawnPosition", CCNode*, m_pSpawnPosition);
        CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pTargetPosition", CCNode*, m_pTargetPosition);
        CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pElevatorNumber", CCLabelTTF*, m_pElevatorNumber);
        return false;
    }
    
    
    void initObjects();
    void updateContents();
    
    virtual void update(float delta);
    
public:
    CC_SYNTHESIZE(BPElevatorNodeDelegate*, m_pDelegate, Delegate);
    CC_SYNTHESIZE_READONLY(uint32_t, m_iElevatorNumber, ElevatorNumber);
    
protected:
    CCLayerColor                        *m_pAlertBoardNode;
    CCLabelTTF                          *m_pAlertLabelNode;
    CCNode                              *m_pSpawnPosition;
    CCNode                              *m_pTargetPosition;
    CCLabelTTF                          *m_pElevatorNumber;
    
    float                               m_fDelayStart;
    CCPoint                             m_tVelocity;
    CCSprite                            *m_pHumanSprite;
    bool                                m_bHumanAppearUpdate;
    
    bool                                m_bArrivedHuman;

    

};

#endif