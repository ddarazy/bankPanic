//
//  ITGScrollView.h
//  Conan
//
//  Created by forestj on 13. 9. 30..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#ifndef Conan_ITGScrollView_
#define Conan_ITGScrollView_

#include "cocos2d.h"
#include "ExtensionMacros.h"

class ITGScrollBar;

USING_NS_CC;

/**
 * @addtogroup GUI
 * @{
 */

typedef enum {
	kITGScrollViewDirectionNone = -1,
    kITGScrollViewDirectionHorizontal = 0,
    kITGScrollViewDirectionVertical,
    kITGScrollViewDirectionBoth
} ITGScrollViewDirection;

class ITGScrollView;

class ITGScrollViewDelegate
{
public:
    virtual ~ITGScrollViewDelegate() {}
    virtual void scrollViewDidScroll(ITGScrollView* view) = 0;
    virtual void scrollViewDidZoom(ITGScrollView* view) = 0;
    virtual void scrollViewDidEndScroll(ITGScrollView* view){};
};


/**
 * ScrollView support for cocos2d for iphone.
 * It provides scroll view functionalities to cocos2d projects natively.
 */
class ITGScrollView : public CCLayer
{
public:
    ITGScrollView();
    virtual ~ITGScrollView();
    
    bool init();
    virtual void registerWithTouchDispatcher();
    
    /**
     added forestj - paging enabled functions - begin
     */
    bool isPagingEnabled()  {return m_bPagingEnabled; }
    void setPagingEnabled(bool bPagingEnabled)  {m_bPagingEnabled = bPagingEnabled;}
//    void setPageSize(CCSize _pageSize){pageSize = _pageSize;}
//    CCSize getPageSize()    {return pageSize;}
    
    /**
     added forestj - paging enabled functions - end
     */
    
    
    /**
     * Returns an autoreleased scroll view object.
     *
     * @param size view size
     * @param container parent object
     * @return autoreleased scroll view object
     */
    static ITGScrollView* create(CCSize size, CCNode* container = NULL);
    
    /**
     * Returns an autoreleased scroll view object.
     *
     * @param size view size
     * @param container parent object
     * @return autoreleased scroll view object
     */
    static ITGScrollView* create();
    
    /**
     * Returns a scroll view object
     *
     * @param size view size
     * @param container parent object
     * @return scroll view object
     */
    bool initWithViewSize(CCSize size, CCNode* container = NULL);
    
    
    /**
     * Sets a new content offset. It ignores max/min offset. It just sets what's given. (just like UIKit's UIScrollView)
     *
     * @param offset new offset
     * @param If YES, the view scrolls to the new offset
     */
    void setContentOffset(CCPoint offset, bool animated = false);
    CCPoint getContentOffset();
    /**
     * Sets a new content offset. It ignores max/min offset. It just sets what's given. (just like UIKit's UIScrollView)
     * You can override the animation duration with this method.
     *
     * @param offset new offset
     * @param animation duration
     */
    void setContentOffsetInDuration(CCPoint offset, float dt);
    
    void setZoomScale(float s);
    /**
     * Sets a new scale and does that for a predefined duration.
     *
     * @param s a new scale vale
     * @param animated if YES, scaling is animated
     */
    void setZoomScale(float s, bool animated);
    
    float getZoomScale();
    
    /**
     * Sets a new scale for container in a given duration.
     *
     * @param s a new scale value
     * @param animation duration
     */
    void setZoomScaleInDuration(float s, float dt);
    /**
     * Returns the current container's minimum offset. You may want this while you animate scrolling by yourself
     */
    CCPoint minContainerOffset();
    /**
     * Returns the current container's maximum offset. You may want this while you animate scrolling by yourself
     */
    CCPoint maxContainerOffset();
    /**
     * Determines if a given node's bounding box is in visible bounds
     *
     * @return YES if it is in visible bounds
     */
    bool isNodeVisible(CCNode * node);
    /**
     * Provided to make scroll view compatible with SWLayer's pause method
     */
    void pause(CCObject* sender);
    /**
     * Provided to make scroll view compatible with SWLayer's resume method
     */
    void resume(CCObject* sender);
    
    
    bool isDragging() {return m_bDragging;}
    bool isTouchMoved() { return m_bTouchMoved; }
    bool isBounceable() { return m_bBounceable; }
    void setBounceable(bool bBounceable) { m_bBounceable = bBounceable; }
    
    /**
     * size to clip. CCNode boundingBox uses contentSize directly.
     * It's semantically different what it actually means to common scroll views.
     * Hence, this scroll view will use a separate size property.
     */
    CCSize getViewSize() { return m_tViewSize; }
    void setViewSize(CCSize size);
    
    CCNode * getContainer();
    void setContainer(CCNode * pContainer);
    
    /**
     * direction allowed to scroll. ITGScrollViewDirectionBoth by default.
     */
    ITGScrollViewDirection getDirection() { return m_eDirection; }
    virtual void setDirection(ITGScrollViewDirection eDirection) { m_eDirection = eDirection; }
    
    ITGScrollViewDelegate* getDelegate() { return m_pDelegate; }
    void setDelegate(ITGScrollViewDelegate* pDelegate) { m_pDelegate = pDelegate; }
    
    /** override functions */
    // optional
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    virtual void setContentSize(const CCSize & size);
    virtual const CCSize& getContentSize() const;
    
	void updateInset();
    /**
     * Determines whether it clips its children or not.
     */
    bool isClippingToBounds() { return m_bClippingToBounds; }
    void setClippingToBounds(bool bClippingToBounds) { m_bClippingToBounds = bClippingToBounds; }
    
    virtual void visit();
    virtual void addChild(CCNode * child, int zOrder, int tag);
    virtual void addChild(CCNode * child, int zOrder);
    virtual void addChild(CCNode * child);
    void setTouchEnabled(bool e);
    
    //  scrollbar added by forestj
    /**
     modify by taewoo add arg (bAutoHide : when scrollview animation stop, scroll bar hide automatically.
     */
    void showScrollBar(bool bShow, bool bAutoHide);
    
private:
    /**
     * Relocates the container at the proper offset, in bounds of max/min offsets.
     *
     * @param animated If YES, relocation is animated
     */
    void relocateContainer(bool animated);
    /**
     * implements auto-scrolling behavior. change SCROLL_DEACCEL_RATE as needed to choose
     * deacceleration speed. it must be less than 1.0f.
     *
     * @param dt delta
     */
    void deaccelerateScrolling(float dt);
    /**
     * This method makes sure auto scrolling causes delegate to invoke its method
     */
    void performedAnimatedScroll(float dt);
    /**
     * Expire animated scroll delegate calls
     */
    void stoppedAnimatedScroll(CCNode* node);
    /**
     * clip this view so that outside of the visible bounds can be hidden.
     */
    void beforeDraw();
    /**
     * retract what's done in beforeDraw so that there's no side effect to
     * other nodes.
     */
    void afterDraw();
    /**
     * Zoom handling
     */
    void handleZoom();
    
protected:
    CCRect getViewRect();
    
    /**
     added forestj - paging enabled variables - begin
     */
    bool m_bPagingEnabled;
//    CCSize pageSize;
    
    /**
     added forestj - paging enabled variables - end
     */

    
    /**
     * current zoom scale
     */
    float m_fZoomScale;
    /**
     * min zoom scale
     */
    float m_fMinZoomScale;
    /**
     * max zoom scale
     */
    float m_fMaxZoomScale;
    /**
     * scroll view delegate
     */
    ITGScrollViewDelegate* m_pDelegate;
    
    ITGScrollViewDirection m_eDirection;
    /**
     * If YES, the view is being dragged.
     */
    bool m_bDragging;
    
    /**
     * Content offset. Note that left-bottom point is the origin
     */
    CCPoint m_tContentOffset;
    
    /**
     * Container holds scroll view contents, Sets the scrollable container object of the scroll view
     */
    CCNode* m_pContainer;
    /**
     * Determiens whether user touch is moved after begin phase.
     */
    bool m_bTouchMoved;
    /**
     * max inset point to limit scrolling by touch
     */
    CCPoint m_fMaxInset;
    /**
     * min inset point to limit scrolling by touch
     */
    CCPoint m_fMinInset;
    /**
     * Determines whether the scroll view is allowed to bounce or not.
     */
    bool m_bBounceable;
    
    bool m_bClippingToBounds;
    
    /**
     * scroll speed
     */
    CCPoint m_tScrollDistance;
    /**
     * Touch point
     */
    CCPoint m_tTouchPoint;
    /**
     * length between two fingers
     */
    float m_fTouchLength;
    /**
     * UITouch objects to detect multitouch
     */
    CCArray* m_pTouches;
    /**
     * size to clip. CCNode boundingBox uses contentSize directly.
     * It's semantically different what it actually means to common scroll views.
     * Hence, this scroll view will use a separate size property.
     */
    CCSize m_tViewSize;
    /**
     * max and min scale
     */
    float m_fMinScale, m_fMaxScale;
    /**
     * scissor rect for parent, just for restoring GL_SCISSOR_BOX
     */
    CCRect m_tParentScissorRect;
    bool m_bScissorRestored;
    
    /**
     *  scrollBar
     */
    ITGScrollBar *m_pScrollBar;
    bool m_bShowScrollBar;

};

// end of GUI group
/// @}




#endif