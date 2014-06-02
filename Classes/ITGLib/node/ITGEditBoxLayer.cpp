//
//  ITGEditBoxLayer.cpp
//  Conan
//
//  Created by forestj on 2013. 12. 26..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#include "ITGEditBoxLayer.h"


ITGEditBoxLayer::~ITGEditBoxLayer()
{
}


bool ITGEditBoxLayer::init()
{
    if (!CCLayerColor::initWithColor(ccc4(255, 255, 255, 255))) {
        return false;
    }
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
//    setColor(ccWHITE);
    setContentSize(winSize);
    
    if (!m_pEditBox) {
        
//        m_pEditBox = CCEditBox::create(CCSizeMake(winSize.width, winSize.height*0.5), CCScale9Sprite::create("ui_common_back_label_001.png"));
        m_pEditBox = CCEditBox::create(CCSizeMake(winSize.width, winSize.height*0.5), CCScale9Sprite::create("ui_common_back_transbox_001.png"));

        m_pEditBox->setDelegate(this);
        m_pEditBox->setPosition(ccp(0, 0));
        m_pEditBox->setAnchorPoint(ccp(0, 0));
        m_pEditBox->setMaxLength(256);
        m_pEditBox->setFontSize(20);
        m_pEditBox->setFontColor(ccBLACK);
//        m_pEditBox->setFontName(JAPANESE_FONT);
        m_pEditBox->setInputFlag(kEditBoxInputFlagSensitive);
        m_pEditBox->setReturnType(kKeyboardReturnTypeDone);
        this->addChild(m_pEditBox);
    }

    setKeypadEnabled(true);
//    showEditBox(true, "");
	return true;
}

void ITGEditBoxLayer::setEditBoxPlaceHolder(const char *szPlaceHolder)
{
    if (m_pEditBox) {
        m_pEditBox->setPlaceHolder(szPlaceHolder);
    }
}

void ITGEditBoxLayer::showEditBox(bool isShow, const char *szInitText)
{
    setVisible(isShow);
    m_pEditBox->setText(szInitText);
    
    if (isShow) {
        m_pEditBox->touchDownAction(NULL, CCControlEventTouchDown);
    }
}


#pragma mark CCEditBoxDelegate
void ITGEditBoxLayer::editBoxTextChanged(CCEditBox* editBox, const std::string& text)
{
    if (m_pEditDelegate) {
        m_pEditDelegate->editTextFieldChanged(text);
    }
}
void ITGEditBoxLayer::editBoxReturn(CCEditBox* editBox)
{
    editBox->detachWithIME();
    
    if (m_pEditDelegate) {
        m_pEditDelegate->editTextFieldEditingEnd(m_pEditBox->getText());
    }
    
    showEditBox(false, NULL);

}

void ITGEditBoxLayer::keyBackClicked()
{
    CCLog("ITGEditBoxLayer::keyBackClicked");
    if (m_pEditDelegate) {
        m_pEditDelegate->editTextFieldEditingEnd(m_pEditBox->getText());
    }
    
    showEditBox(false, NULL);
}

void ITGEditBoxLayer::keyMenuClicked()
{
    CCLog("ITGEditBoxLayer::keyMenuClicked");
    m_pEditBox->touchDownAction(NULL, CCControlEventTouchDown);
}

void ITGEditBoxLayer::keyboardWillShow(CCIMEKeyboardNotificationInfo &info)
{
    CCLog("ITGEditBoxLayer::keyboardWillShow");
}

void ITGEditBoxLayer::keyboardWillHide(CCIMEKeyboardNotificationInfo &info)
{
    CCLog("ITGEditBoxLayer::keyboardWillHide");
}
