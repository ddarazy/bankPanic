//
//  ITGBaseTableViewCell.cpp
//  Conan
//
//  Created by forestj on 13. 10. 7..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#include "ITGBaseTableViewCell.h"



ITGBaseTableViewCell::ITGBaseTableViewCell()
: m_pDelegate(NULL)
, pCellNode(NULL)
, iCellID(0)
{
}

ITGBaseTableViewCell::~ITGBaseTableViewCell()
{
}

bool ITGBaseTableViewCell::init()
{
    if (!CCTableViewCell::init()) {
        return false;
    }
    
	return true;
}

void ITGBaseTableViewCell::cbCellTouchAnimationEnded(cocos2d::CCNode *pNode)
{
    if (m_pDelegate) {
        m_pDelegate->cellTouchAnimationEnded(this);
    }
}



ITGBaseTableViewCellNode::ITGBaseTableViewCellNode()
: m_pAnimationMng(NULL)
, m_pBaseCell(NULL)
{
    
}

ITGBaseTableViewCellNode::~ITGBaseTableViewCellNode()
{
    if (m_pAnimationMng)
    {
        CC_SAFE_RELEASE_NULL(m_pAnimationMng);
    }
}

bool ITGBaseTableViewCellNode::init()
{
    if (!CCNode::init()) {
        return false;
    }
    return true;
}

void ITGBaseTableViewCellNode::cbClickAnimationEnded(cocos2d::CCNode *pNode)
{
    if (m_pBaseCell) {
        m_pBaseCell->cbCellTouchAnimationEnded(this);
    }
}

