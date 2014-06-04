//
//  BPGameLayer.h
//  bankPanic
//
//  Created by forestj on 2014. 4. 23..
//  Copyright (c) 2014년 __MyCompanyName__. All rights reserved.
//

#ifndef bankPanic_BPGameLayer_
#define bankPanic_BPGameLayer_

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ITGModalEnableLayer.h"
#include "BPElevatorNode.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

typedef enum
{
    kGAME_STATUS_READY = 0,                        //  게임 준비
    kGAME_STATUS_HUMAN_APPEAR_READY,               //  문에 사람이 다가와 있으면 문이 열릴 수 있는 상태
    kGAME_STATUS_DOOR_MOVING,                      //  문 이동중
    kGAME_STATUS_DOOR_MOVING_STOP,                 //  문 이동이 멈춘 후 더이상 이동이 없는지 대기하는 상태
    KGAME_STATUS_OPENING_DOOR,                     //  문이 열려 있으며 이동이 불가능한 상태
    KGAME_STATUS_GAME_OVER,                        //  체력이 다하여 플레이어가 더이상 게임을 할 수 없을때(죽음)
    KGAME_STATUS_GAME_CLEAR,                       //  게임의 조건을 달성하여 스테이지를 클리어 했을 때
    

}eGAME_STATUS;

class BPGameLayer : public ITGModalEnableLayer
, public CCBMemberVariableAssigner
, public BPElevatorNodeDelegate

{

public:

	virtual ~BPGameLayer();
	virtual bool init();
    
    CREATE_FUNC(BPGameLayer);
    
    static CCScene* scene();
    
protected:
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

    
#pragma mark CCBMemberVariableAssigner
    virtual bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
    {
        CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, "m_pTopBoard", CCNode*, m_pTopBoard);
        return false;
    };
    
#pragma mark BPElevatorNodeDelegate
    virtual void arrivedHuman(BPElevatorNode *pElevator);
    
    void initObjects();
    void updateContents();
    
    bool isOpeningDoor();
    
    bool moveVelocity(CCTouch *pTouch);
    /**
        @brief 문이 이동되는 스케쥴
     */
    void updateMovingDoors(float delta);
    /**
        @brief 문 이동이 완료된 후 멈춰 있을 때 멈춘 시간을 체크하는 타이머
     */
//    void updateStatus(float delta);
    void doorsStatusCheck();
    void doorsCancelResearvedOpen();
    void visibleDoorsCheck();

    
    
protected:
    CCNode                      *m_pTopBoard;
    
    CCArray                     *m_arrElevatorList;
    CCArray                     *m_arrDoorList;
    
    //  이동되지 않은 시간 체크
    float                       m_fNoMovingTimeDelta;
    float                       m_fNoMovingLimitTime;
    bool                        m_bNoMovingTimeCount;
    
    
    //  door moving
    bool                        m_bDoorMoving;
    int                         m_iDoorMovingCount;
    clock_t                     m_tVelocityStartTime;
    CCPoint                     m_tVelocity;
    int                         m_iScrollingCount;
    int                         m_iMaxScrollingCount;
    
    float                       m_fMinOffsetX;
    float                       m_fMaxOffsetX;
    
    float                       m_fCellWidth;
    float                       m_fMovingUnit;
    float                       m_fMovingDeltaTime;
    float                       m_fMovingLimitTime;
    
    float                       m_fDeltaTime;
    
    queue<CCPoint>              m_arrVectorList;
    CCPoint                     m_tCurrentVelocity;
    
    eGAME_STATUS                m_eGameStatus;

};

#endif