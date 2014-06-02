//
//  ITGEditBoxLayer.h
//  Conan
//
//  Created by forestj on 2013. 12. 26..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#ifndef Conan_ITGEditBoxLayer_
#define Conan_ITGEditBoxLayer_

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class ITGEditBoxLayerDelegate {
    
    
public:
    virtual void editTextFieldEditingEnd(std::string text)=0;
    virtual void editTextFieldChanged(std::string text){};
};


class ITGEditBoxLayer : public CCLayerColor
, public CCEditBoxDelegate
, public CCIMEDelegate
{

public:

	virtual ~ITGEditBoxLayer();
	virtual bool init();
    
    CREATE_FUNC(ITGEditBoxLayer);
    
    void showEditBox(bool isShow, const char *szInitText);
    void setEditBoxPlaceHolder(const char *szPlaceHolder);
    
public:
    CC_SYNTHESIZE(ITGEditBoxLayerDelegate*, m_pEditDelegate, EditDelegate);
    
protected:
    virtual void editBoxEditingDidBegin(CCEditBox* editBox) {};
    virtual void editBoxEditingDidEnd(CCEditBox* editBox) {};
    virtual void editBoxTextChanged(CCEditBox* editBox, const std::string& text);
    virtual void editBoxReturn(CCEditBox* editBox);
    
    virtual void keyBackClicked();
    virtual void keyMenuClicked();
    
    virtual void keyboardWillShow(CCIMEKeyboardNotificationInfo& info);
    virtual void keyboardWillHide(CCIMEKeyboardNotificationInfo& info);

private:
    CCEditBox           *m_pEditBox;
};

#endif