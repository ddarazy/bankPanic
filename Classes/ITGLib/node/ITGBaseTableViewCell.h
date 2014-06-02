//
//  ITGBaseTableViewCell.h
//  Conan
//
//  Created by forestj on 13. 10. 7..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#ifndef Conan_ITGBaseTableViewCell_
#define Conan_ITGBaseTableViewCell_

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;
USING_NS_CC;

class ITGBaseTableViewCell;



class ITGBaseTableViewCellNode : public CCNode {
    
    
public:
    ITGBaseTableViewCellNode();
    virtual ~ITGBaseTableViewCellNode();
    
    CREATE_FUNC(ITGBaseTableViewCellNode);
    
    virtual bool init();
    
    virtual void OnCellNodeTouch(){};
    virtual void OnCellNodeHighlight(){};
    virtual void OnCellNodeUnhighlight(){};
    virtual void OnClickInCellNode(unsigned int btnId){};

protected:
    void cbClickAnimationEnded(CCNode *pNode);

public:
    CC_SYNTHESIZE(ITGBaseTableViewCell*, m_pBaseCell, BaseCell);
    
protected:
    CCBAnimationManager             *m_pAnimationMng;
};



class ITGBaseTableViewCellDelegate {
public:
    virtual void cellTouchAnimationEnded(ITGBaseTableViewCell *cell) {};
    virtual void cellInButtonClick(ITGBaseTableViewCell *cell, CCObject* pSender) {};
};

class ITGBaseTableViewCell : public CCTableViewCell
{

public:
	ITGBaseTableViewCell();
	virtual ~ITGBaseTableViewCell();
    
    CREATE_FUNC(ITGBaseTableViewCell);
    
	virtual bool init();
    
    virtual void OnCellTouch(){};
    virtual void OnCellHighlight(){};
    virtual void OnCellUnhighlight(){};
    void cbCellTouchAnimationEnded(CCNode *pNode);
    void cellTouchInCell(unsigned int btnIdx);
    
public:
    CC_SYNTHESIZE(ITGBaseTableViewCellDelegate*, m_pDelegate, Delegate);
    ITGBaseTableViewCellNode                *pCellNode;
    CC_SYNTHESIZE(unsigned int, iCellID, CellID);
    
};




#endif