//
//  ITGScrollBar.cpp
//  Conan
//
//  Created by forestj on 13. 9. 30..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#include "ITGScrollBar.h"

#define barFixSize 10.0f
#define barMinSize 20.0f


ITGScrollBar::ITGScrollBar()
{
    dynamicScrollSize = true;
    barH = NULL;
    barV = NULL;
    m_bIsAutoHide = false;
    scrollView = NULL;
}

ITGScrollBar::~ITGScrollBar()
{
    
}

bool ITGScrollBar::init(ITGScrollView *sv, float offset, const std::string &h, const std::string &h_back, const std::string &v, const std::string &v_back)
{
    scrollView = sv;
    
    CCLayerColor::initWithColor(ccc4(0, 255, 0, 0));
    ITGScrollViewDirection direction = sv->getDirection();
    
    switch (direction) {
        case kITGScrollViewDirectionHorizontal:
        {
            barHBack = CCScale9Sprite::create(h_back.c_str(), CCRectMake(0, 0, 15, 11), CCRectMake(6, 4, 5, 2));
            barHBack->setAnchorPoint(ccp(0, 0));
            barHBack->setPreferredSize(CCSizeMake(scrollView->getViewSize().width, 10));
            barHBack->setPosition(ccp(0, scrollView->getContentSize().height));
            addChild(barHBack);
            
            barH = CCScale9Sprite::create(h.c_str(), CCRectMake(0, 0, 13, 10), CCRectMake(6, 3, 1, 3));
            barH->setAnchorPoint(ccp(0.5, 0));
            barH->setPosition(ccp(0, offset));
            addChild(barH);
            sv->CCLayer::addChild(this, 50);
            setBarRefresh();
        }
            break;
        case kITGScrollViewDirectionVertical:
        {
            barVBack = CCScale9Sprite::create(v_back.c_str(), CCRectMake(0, 0, 15, 11), CCRectMake(6, 4, 5, 2));
            barVBack->setAnchorPoint(ccp(1, 0));
            barVBack->setPreferredSize(CCSizeMake(15, scrollView->getViewSize().height));
            barVBack->setPosition(ccp(scrollView->getContentSize().width, 0));
            addChild(barVBack);
            
            barV = CCScale9Sprite::create(v.c_str(), CCRectMake(0, 0, 13, 10), CCRectMake(6, 3, 1, 3));
            barV->setAnchorPoint(ccp(1.0, 0.5f));
            barV->setPosition(ccp(0, offset));
            addChild(barV);
            sv->CCLayer::addChild(this, 50);
            setBarRefresh();
        }
            break;
        case kITGScrollViewDirectionBoth:
        {
            barHBack = CCScale9Sprite::create(h_back.c_str(), CCRectMake(0, 0, 15, 11), CCRectMake(6, 4, 5, 2));
            barHBack->setAnchorPoint(ccp(0.5, 0));
            barHBack->setPreferredSize(CCSizeMake(scrollView->getViewSize().width, 15));
            barHBack->setPosition(ccp(0, scrollView->getContentSize().height));
            addChild(barHBack);
            
            barH = CCScale9Sprite::create(h.c_str(), CCRectMake(0, 0, 13, 10), CCRectMake(6, 3, 1, 3));
            barH->setAnchorPoint(ccp(0.5, 0));
            barH->setPosition(ccp(0, offset));
            addChild(barH);
            setBarRefreshH();
            
            barVBack = CCScale9Sprite::create(v_back.c_str(), CCRectMake(0, 0, 15, 11), CCRectMake(6, 4, 5, 2));
            barVBack->setAnchorPoint(ccp(1, 0));
            barVBack->setPreferredSize(CCSizeMake(15, scrollView->getViewSize().height));
            barVBack->setPosition(ccp(scrollView->getContentSize().width, 0));
            addChild(barVBack);
            
            barV = CCScale9Sprite::create(v.c_str(), CCRectMake(0, 0, 13, 10), CCRectMake(6, 3, 1, 3));
            barV->setAnchorPoint(ccp(1.0, 0.5f));
            barV->setPosition(ccp(0, offset));
            addChild(barV);
            setBarRefreshV();
            
            sv->CCLayer::addChild(this, 50);
        }
            break;
        case kITGScrollViewDirectionNone:
            break;
            
    }
    
	return true;
}

ITGScrollBar* ITGScrollBar::createScrollBar(ITGScrollView *sv,
                                            float offset,
                                            const std::string &h,
                                            const std::string &h_back,
                                            const std::string &v,
                                            const std::string &v_back)
{
    ITGScrollBar *obj = new ITGScrollBar();
    if (obj->init(sv, offset, h, h_back, v, v_back)) {
        obj->autorelease();
        return obj;
    }
    else
    {
        delete obj;
        return NULL;
    }
}

void ITGScrollBar::setBarRefreshH()
{
    ITGScrollView *sv = scrollView;
    CCSize viewSize = sv->getViewSize();
    float total = sv->getContainer()->getContentSize().width - viewSize.width;
    float scrollBarSize = viewSize.width * viewSize.width / total;

    if (total < 0) {
        barH->stopAllActions();
        barH->setOpacity(0);
        barHBack->stopAllActions();
        barHBack->setOpacity(0);
        return;
    }
    
    if (dynamicScrollSize) {
        barH->setContentSize(CCSizeMake(scrollBarSize, barH->getContentSize().height));
    }
    else
        barH->setContentSize(barH->getOriginalSize());
    
    CCSize barContentSize = barH->getContentSize();
    
    float offset = -sv->getContentOffset().x;
    float offsetY = -sv->getContentOffset().y;
    
    barHBack->setPosition(ccp(offset, barHBack->getPositionY()));
    
    float percent = offset / total;
    percent = MIN(1, percent);
    percent = MAX(0, percent);
    percent = percent * (viewSize.width - barContentSize.width) / viewSize.width;
    float position = (percent * viewSize.width) + barContentSize.width / 2.0f;
    barH->setPosition(ccp(offset + position, offsetY));
    
    if (barH->getOpacity() < 255) {
        barH->stopAllActions();
        barH->setOpacity(255);
        barHBack->stopAllActions();
        barHBack->setOpacity(255);
    }
}

void ITGScrollBar::setBarRefreshV()
{
    ITGScrollView *sv = scrollView;
    CCSize viewSize = sv->getViewSize();
    float total = sv->getContainer()->getContentSize().height - viewSize.height;
    float scrollBarSize = viewSize.height * viewSize.height / total;
    
    if (total < 0) {
        barV->stopAllActions();
        barV->setOpacity(0);
        barVBack->stopAllActions();
        barVBack->setOpacity(0);
        return;
    }
    
    if (dynamicScrollSize) {
        barV->setContentSize(CCSizeMake(barV->getContentSize().width, scrollBarSize));
    }
    else
    {
        barV->setContentSize(barV->getOriginalSize());
    }
    
    CCSize barContentSize = barV->getContentSize();
    
    float offset = -sv->getContentOffset().y;
    float offsetX = -sv->getContentOffset().x;
    
    barVBack->setPosition(ccp(barVBack->getPositionX(), offset));
    
    float percent = offset / total;
    percent = MIN(1, percent);
    percent = MAX(0, percent);
    percent = percent * (viewSize.height - barContentSize.height) / viewSize.height;
    float position = (percent * viewSize.height) + barContentSize.height / 2.0f;
    barV->setPosition(ccp(offsetX + viewSize.width - 2, offset + position));
    
    if (barV->getOpacity() < 255) {
        barV->stopAllActions();
        barV->setOpacity(255);
        barVBack->stopAllActions();
        barVBack->setOpacity(255);
    }
}

void ITGScrollBar::setBarRefresh()
{
    ITGScrollView *sv = scrollView;
    if (sv->getDirection() == kITGScrollViewDirectionHorizontal) {
        setBarRefreshH();
    }
    else if(sv->getDirection() == kITGScrollViewDirectionVertical)
    {
        setBarRefreshV();
    }
    else if(sv->getDirection() == kITGScrollViewDirectionBoth)
    {
        setBarRefreshV();
        setBarRefreshH();
    }
}

void ITGScrollBar::hide()
{
    if (barH) {
        barH->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCFadeOut::create(1.0f), NULL));
        barHBack->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCFadeOut::create(1.0f), NULL));
    }
    if (barV) {
        barV->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCFadeOut::create(1.0f), NULL));
        barVBack->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCFadeOut::create(1.0f), NULL));
    }
}