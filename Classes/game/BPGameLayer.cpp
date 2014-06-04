//
//  BPGameLayer.cpp
//  bankPanic
//
//  Created by forestj on 2014. 4. 23..
//  Copyright (c) 2014년 __MyCompanyName__. All rights reserved.
//

#include "BPGameLayer.h"
#include "ITGUtil.h"
#include "BPDoorNode.h"


#define kMAP_MOVE_SPEED    10
#define kMAP_MOVE_MIN_DISTANCE  50
#define kMAP_DISTANCE_PER_SECOND_MIN 1000

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define kMAP_MOVE_VELOCITY  7           //  number is high -> slow, number is low -> fast
#else
#define kMAP_MOVE_VELOCITY  5           //  number is high -> slow, number is low -> fast
#endif

BPGameLayer::~BPGameLayer()
{
    CC_SAFE_RELEASE_NULL(m_arrDoorList);
    CC_SAFE_RELEASE_NULL(m_arrElevatorList);
}

CCScene* BPGameLayer::scene()
{
    CCScene *scene = CCScene::create();
    if (scene) {
        BPGameLayer *node = (BPGameLayer*)ITGUtil::sharedUtils()->loadCustomLayerFromCCBI<BPGameLayer>("BPGameLayer", "BPGameLayer.ccbi", NULL, NULL);
        if (node) {
            node->initObjects();
            scene->addChild(node);
        }
    }
    return scene;
}

bool BPGameLayer::init()
{
    if (!ITGModalEnableLayer::init()) {
        return false;
    }
	return true;
}

void BPGameLayer::onEnter()
{
    ITGModalEnableLayer::onEnter();
    
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

void BPGameLayer::onExit()
{
    ITGModalEnableLayer::onExit();
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}


#pragma mark Touch func
bool BPGameLayer::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    m_tVelocityStartTime = clock();
    
    return true;
}

void BPGameLayer::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    
}

void BPGameLayer::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    if (isOpeningDoor())
    {
        CCPoint loc = pTouch->getLocation();
        
        for (int i=0; i<m_arrDoorList->count(); i++) {
            BPDoorNode *pDoor = (BPDoorNode*)m_arrDoorList->objectAtIndex(i);
            if (pDoor->IsOpenDoor()) {
                if (pDoor->boundingBox().containsPoint(loc)) {
                    CCLog("fire");
                    CCParticleSystemQuad *pExplode = (CCParticleSystemQuad*)ITGUtil::sharedUtils()->getNodeFromReadCCBI("BPBloodEffect.ccbi", NULL);
                    addChild(pExplode, 1000);
                    pExplode->setPosition(loc);
                    pExplode->setAutoRemoveOnFinish(true);
//                    pDoor->setOpenDoor(false);
                    pDoor->closeDoor();

                    pExplode->resetSystem();
                    
                    BPElevatorNode *pElevator = (BPElevatorNode*)m_arrElevatorList->objectAtIndex(i);
                    pElevator->resetAppearHuman();
                    pElevator->startAppearHuman();

                    break;
                }
            }
            
        }
        
    }
    else
    {
        moveVelocity(pTouch);
    }
    
}

#pragma mark BPElevatorNodeDelegate
void BPGameLayer::arrivedHuman(BPElevatorNode *pElevator)
{
    CCLog("BPGameLayer::arrivedHuman : %d", pElevator->getElevatorNumber());
    visibleDoorsCheck();
    doorsStatusCheck();
}



#pragma mark functions

bool BPGameLayer::isOpeningDoor()
{
    for (int i=0; i<m_arrDoorList->count(); i++) {
        BPDoorNode *pDoor = (BPDoorNode*)m_arrDoorList->objectAtIndex(i);
        if (pDoor->IsOpenDoor()) {
            return true;
        }
    }
    return false;
}


void BPGameLayer::initObjects()
{
    m_arrDoorList = CCArray::create();
    m_arrDoorList->retain();
    
    m_arrElevatorList = CCArray::create();
    m_arrElevatorList->retain();
    
    m_eGameStatus = kGAME_STATUS_HUMAN_APPEAR_READY;
    
    m_fNoMovingLimitTime = 1;
    m_fNoMovingTimeDelta = 0;
    m_bNoMovingTimeCount = false;
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    m_fMinOffsetX = -winSize.width;
    m_fMaxOffsetX = winSize.width * 3;
    
    //  elevator indicator
    float fElevatorUnitWidth = winSize.width / 12;
    
    m_fCellWidth = winSize.width / 3;
    m_fMovingUnit = m_fCellWidth / 10;
    //  door
    float offset_x = 0;
    for (int i=0; i<12; i++) {
        //  door
        BPDoorNode *pDoor = BPDoorNode::createNode();
        offset_x = (m_fCellWidth * i) + m_fCellWidth * 0.5;
        pDoor->setAnchorPoint(ccp(0.5, 1));
        pDoor->setPosition(ccp(offset_x, winSize.height * 0.65));
        pDoor->setDoorNumber(i+1);
        addChild(pDoor, 1);
        m_arrDoorList->addObject(pDoor);
        
        //  elevator
        BPElevatorNode *pElevator = BPElevatorNode::createNode();
        pElevator->setElevatorNumber(i+1);
        pElevator->setDelegate(this);
        pElevator->setPosition(ccp(fElevatorUnitWidth * i + fElevatorUnitWidth * 0.5, 17));
        m_pTopBoard->addChild(pElevator, 2);
        pElevator->startAppearHuman();
        m_arrElevatorList->addObject(pElevator);
    }
    
    //  target mark
    for (int i=0; i<3; i++) {
        CCSprite *pTargetMark = CCSprite::createWithSpriteFrameName("target_mark.png");
        pTargetMark->setAnchorPoint(ccp(0.5, 0.5));
        pTargetMark->setPosition(ccp(m_fCellWidth * i + m_fCellWidth * 0.5, winSize.height * 0.45));
        addChild(pTargetMark, 10);
        
    }
}

void BPGameLayer::updateContents()
{
    
}

bool BPGameLayer::moveVelocity(cocos2d::CCTouch *pTouch)
{
    CCPoint start = pTouch->getStartLocation();
    CCPoint end = pTouch->getLocation();
    float dist = ccpDistance(end, start);
    //  min move distance
    if (dist > 50) {
        CCPoint diffPoint = ccpSub(end, start);
        
        //  vector calc
        float length = sqrtf(diffPoint.x * diffPoint.x + diffPoint.y * diffPoint.y);
        CCPoint desiredDirection;
        if (length < 0.000001) {
            desiredDirection = ccp(0, 1);
        }
        else
            desiredDirection = ccpMult(diffPoint, 1/length);
        
        CCPoint tVector = ccpMult(desiredDirection, kMAP_MOVE_SPEED);
        tVector.y = 0;
        if (tVector.x < 0) {
            tVector.x = -m_fMovingUnit;
        }
        else
            tVector.x = m_fMovingUnit;
        
        
        m_tVelocity = ccpMult(desiredDirection, kMAP_MOVE_SPEED);
        m_tVelocity.y = 0;
        if (m_tVelocity.x < 0) {
            m_tVelocity.x = -m_fMovingUnit;
        }
        else
        {
            m_tVelocity.x = m_fMovingUnit;
        }
        
        //  touch end time
        clock_t end_time = clock();

        //  get diff time (x.xxxx sec)
        float diff = (((float)end_time - (float)m_tVelocityStartTime) / 1000000.0F);

        
        //  distance per second
        float distPerSec = dist / diff;
        
        if (distPerSec > kMAP_DISTANCE_PER_SECOND_MIN) {
            float mul = m_tCurrentVelocity.x * tVector.x;
            if (mul >= 0) {
                m_arrVectorList.push(tVector);
            }
            else
            {
                while (!m_arrVectorList.empty()) {
                    m_arrVectorList.pop();
                }
                m_arrVectorList.push(tVector);
            }
            
//            CCLog("velocity size : %lu, %f", m_arrVectorList.size(), tVector.x);
            
            if (!m_bDoorMoving) {
//                if (m_bNoMovingTimeCount) {
//                    m_bNoMovingTimeCount = false;
//                    unschedule(schedule_selector(BPGameLayer::updateStatus));
//                }
                doorsCancelResearvedOpen();
//                doorsStatusCheck();
                m_iScrollingCount = 10;
                m_bDoorMoving = true;
                schedule(schedule_selector(BPGameLayer::updateMovingDoors));
            }
        }
        return true;
    }
    
    return false;
}

void BPGameLayer::updateMovingDoors(float delta)
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    if (m_tCurrentVelocity.equals(CCPointZero))
    {
        if (m_arrVectorList.empty())
        {
            //  door moving stop
            m_bDoorMoving = false;
            m_tCurrentVelocity = CCPointZero;
            unschedule(schedule_selector(BPGameLayer::updateMovingDoors));
            
            //  화면에 등장하고 있는 문 체크
//            CCSize winSize = CCDirector::sharedDirector()->getWinSize();
//            CCRect winRect = CCRectMake(0, 0, winSize.width, winSize.height);
//            for (int i=0; i<m_arrDoorList->count(); i++) {
//                BPDoorNode *pDoor = (BPDoorNode*)m_arrDoorList->objectAtIndex(i);
//                if (winRect.containsPoint(pDoor->getPosition())) {
//                    //  open ready
//                    pDoor->setEnableOpenDoor(true);
//                }
//                else
//                {
//                    //  disable open
//                    pDoor->setEnableOpenDoor(false);
//                }
//            }
            visibleDoorsCheck();
            
            //  count down stop ready
            if (!m_bNoMovingTimeCount)
            {
                m_eGameStatus = kGAME_STATUS_DOOR_MOVING_STOP;
                m_bNoMovingTimeCount = true;
//                schedule(schedule_selector(BPGameLayer::updateStatus));
                
            }
            
            return;
        }
        m_iScrollingCount = 10;
        m_tCurrentVelocity = m_arrVectorList.front();
        m_arrVectorList.pop();
    }

    m_iScrollingCount--;
    CCPoint p;
    for (int i=0; i<m_arrDoorList->count(); i++) {
        BPDoorNode *pNode = (BPDoorNode*)m_arrDoorList->objectAtIndex(i);
        p = pNode->getPosition();
        p = ccpAdd(p, m_tCurrentVelocity);
        
        pNode->setPosition(p);
    }
    
    //  left
    if (m_tCurrentVelocity.x < 0)
    {
        BPDoorNode *pDoorFirst = (BPDoorNode*)m_arrDoorList->objectAtIndex(0);
        float fFirstX = pDoorFirst->getPositionX();
        float fFirstSize = pDoorFirst->getContentSize().width;
        
        if (fFirstX + fFirstSize < -winSize.width) {
            BPDoorNode *pDoorLast = (BPDoorNode*)m_arrDoorList->lastObject();
            
            float offset_x = pDoorLast->getPositionX() + m_fCellWidth;
            pDoorFirst->setPositionX(offset_x);
            
            m_arrDoorList->removeObject(pDoorFirst);
            m_arrDoorList->addObject(pDoorFirst);
        }
    }
    else    //  right
    {
        BPDoorNode *pDoorLast = (BPDoorNode*)m_arrDoorList->lastObject();
        float fLastX = pDoorLast->getPositionX();
        
        if (fLastX > winSize.width * 2)
        {
            BPDoorNode *pDoorFirst = (BPDoorNode*)m_arrDoorList->objectAtIndex(0);
            float offset_x = pDoorFirst->getPositionX() - m_fCellWidth;
            pDoorLast->setPositionX(offset_x);
            
            m_arrDoorList->removeObject(pDoorLast);
            m_arrDoorList->insertObject(pDoorLast, 0);
        }
        
    }
    
    if (m_iScrollingCount <= 0)
    {
        m_tCurrentVelocity = CCPointZero;
    }

}

//void BPGameLayer::updateStatus(float delta)
//{
//    if (m_eGameStatus == kGAME_STATUS_HUMAN_APPEAR_READY)
//    {
//        m_fDeltaTime += delta;
//        if (m_fDeltaTime > 1) {
//            m_fDeltaTime = 0;
//            for (int i=0; i<m_arrDoorList->count(); i++) {
//                BPDoorNode *pDoor = (BPDoorNode*)m_arrDoorList->objectAtIndex(i);
//                if (pDoor->getEnableOpenDoor() && !pDoor->IsOpenDoor())
//                {
//                    BPElevatorNode *pElevator = (BPElevatorNode*)m_arrElevatorList->objectAtIndex(i);
//                    if (pElevator->IsArrivedHuman())
//                    {
////                        pDoor->setOpenDoor(true);
//                        break;
//                    }
//                }
//            }
//        }
//    }
//    else if(m_eGameStatus == kGAME_STATUS_DOOR_MOVING_STOP)
//    {
//        m_fNoMovingTimeDelta += delta;
//        if (m_fNoMovingTimeDelta > m_fNoMovingLimitTime) {
//            m_eGameStatus = kGAME_STATUS_HUMAN_APPEAR_READY;
//            m_fNoMovingTimeDelta = 0;
//            m_fDeltaTime = 0;
//        }
//    }
//}


void BPGameLayer::doorsStatusCheck()
{
    for (int i=0; i<m_arrDoorList->count(); i++) {
        BPDoorNode *pDoor = (BPDoorNode*)m_arrDoorList->objectAtIndex(i);
        if (pDoor->getEnableOpenDoor() && !pDoor->IsOpenDoor())
        {
            BPElevatorNode *pElevator = (BPElevatorNode*)m_arrElevatorList->objectAtIndex(i);
            if (pElevator->IsArrivedHuman())
            {
                pDoor->prepareOpenDoor();
                break;
            }
        }
    }
}

void BPGameLayer::doorsCancelResearvedOpen()
{
    for (int i=0; i<m_arrDoorList->count(); i++) {
        BPDoorNode *pDoor = (BPDoorNode*)m_arrDoorList->objectAtIndex(i);
        pDoor->clearOpenDoor();
    }
}

void BPGameLayer::visibleDoorsCheck()
{
    //  화면에 등장하고 있는 문 체크
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCRect winRect = CCRectMake(0, 0, winSize.width, winSize.height);
    for (int i=0; i<m_arrDoorList->count(); i++) {
        BPDoorNode *pDoor = (BPDoorNode*)m_arrDoorList->objectAtIndex(i);
        if (winRect.containsPoint(pDoor->getPosition())) {
            //  open ready
            pDoor->setEnableOpenDoor(true);
        }
        else
        {
            //  disable open
            pDoor->setEnableOpenDoor(false);
        }
    }

}
