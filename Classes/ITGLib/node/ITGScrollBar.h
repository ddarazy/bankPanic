//
//  ITGScrollBar.h
//  Conan
//
//  Created by forestj on 13. 9. 30..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#ifndef Conan_ITGScrollBar_
#define Conan_ITGScrollBar_

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ITGScrollView.h"
#include <string>

USING_NS_CC;
USING_NS_CC_EXT;


class ITGScrollBar : public cocos2d::CCLayerColor
{

public:
	static ITGScrollBar* createScrollBar(ITGScrollView *sv,
                                         float offset,
                                         const std::string &h = "ui_common_icon_scroll.png",
                                         const std::string &h_back = "ui_common_icon_scrollbar.png",
                                         const std::string &v = "ui_common_icon_scroll.png",
                                         const std::string &v_back = "ui_common_icon_scrollbar.png");

	ITGScrollBar();
	virtual ~ITGScrollBar();
	bool init(ITGScrollView *sv, float offset, const std::string &h, const std::string &h_back, const std::string &v, const std::string &v_back);
    void setBarRefresh();
    
    
protected:
    void setBarRefreshH();
    void setBarRefreshV();
    bool dynamicScrollSize;
    
public:
    bool getDynamicScrollSize() {return dynamicScrollSize;};
    void setDynamicScrollSize(bool b)
    {
        dynamicScrollSize = b;
        setBarRefresh();
    }
    
    /**
     add by taewoo : hide value
     */
    void hide();
    CC_SYNTHESIZE(bool, m_bIsAutoHide, IsAutoHide);
    
private:
    CC_SYNTHESIZE(CCScale9Sprite*, barV, BarV);
    CC_SYNTHESIZE(CCScale9Sprite*, barH, BarH);
    CC_SYNTHESIZE(CCScale9Sprite*, barVBack, BarVBack);
    CC_SYNTHESIZE(CCScale9Sprite*, barHBack, BarHBack);
    CC_SYNTHESIZE(ITGScrollView*, scrollView, ScrollView);

};

#endif