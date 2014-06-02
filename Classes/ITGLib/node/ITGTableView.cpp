//
//  ITGTableView.cpp
//  Conan
//
//  Created by forestj on 13. 9. 30..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//


#include "ITGTableView.h"
#include "CCTableViewCell.h"
#include "menu_nodes/CCMenu.h"
#include "support/CCPointExtension.h"
#include "CCSorting.h"
#include "layers_scenes_transitions_nodes/CCLayer.h"
#include "ITGScrollBar.h"

USING_NS_CC;
USING_NS_CC_EXT;


ITGTableView* ITGTableView::create(ITGTableViewDataSource* dataSource, CCSize size)
{
    return ITGTableView::create(dataSource, size, NULL);
}

ITGTableView* ITGTableView::create(ITGTableViewDataSource* dataSource, CCSize size, CCNode *container)
{
    ITGTableView *table = new ITGTableView();
    table->initWithViewSize(size, container);
    table->autorelease();
    table->setDataSource(dataSource);
    table->_updateCellPositions();
    table->_updateContentSize();
    
    return table;
}

bool ITGTableView::initWithViewSize(CCSize size, CCNode* container/* = NULL*/)
{
    if (ITGScrollView::initWithViewSize(size,container))
    {
        m_pCellsUsed      = new cocos2d::extension::CCArrayForObjectSorting();
        m_pCellsFreed     = new cocos2d::extension::CCArrayForObjectSorting();
        m_pIndices        = new std::set<unsigned int>();
        m_eVordering      = kITGTableViewFillBottomUp;
        this->setDirection(kITGScrollViewDirectionVertical);
        
        ITGScrollView::setDelegate(this);
        return true;
    }
    return false;
}

ITGTableView::ITGTableView()
: m_pTouchedCell(NULL)
, m_pIndices(NULL)
, m_pCellsUsed(NULL)
, m_pCellsFreed(NULL)
, m_pDataSource(NULL)
, m_pTableViewDelegate(NULL)
, m_eOldDirection(kITGScrollViewDirectionNone)
{
    
}

ITGTableView::~ITGTableView()
{
    CC_SAFE_DELETE(m_pIndices);
    CC_SAFE_RELEASE(m_pCellsUsed);
    CC_SAFE_RELEASE(m_pCellsFreed);
}

void ITGTableView::setVerticalFillOrder(ITGTableViewVerticalFillOrder fillOrder)
{
    if (m_eVordering != fillOrder) {
        m_eVordering = fillOrder;
        if (m_pCellsUsed->count() > 0) {
            this->reloadData();
        }
    }
}

ITGTableViewVerticalFillOrder ITGTableView::getVerticalFillOrder()
{
    return m_eVordering;
}

void ITGTableView::reloadData()
{
    m_eOldDirection = kITGScrollViewDirectionNone;
    CCObject* pObj = NULL;
    CCARRAY_FOREACH(m_pCellsUsed, pObj)
    {
        CCTableViewCell* cell = (CCTableViewCell*)pObj;
        
        if(m_pTableViewDelegate != NULL) {
            m_pTableViewDelegate->tableCellWillRecycle(this, cell);
        }
        
        m_pCellsFreed->addObject(cell);
        cell->reset();
        if (cell->getParent() == this->getContainer())
        {
            this->getContainer()->removeChild(cell, true);
        }
    }
    
    m_pIndices->clear();
    m_pCellsUsed->release();
    m_pCellsUsed = new cocos2d::extension::CCArrayForObjectSorting();
    
    this->_updateCellPositions();
    this->_updateContentSize();
    if (m_pDataSource->numberOfCellsInTableView(this) > 0)
    {
        this->scrollViewDidScroll(this);
    }
    
    
    /**
     modify by taewoo : when reload, check scroll bar hidden
     */
    if (m_pScrollBar) {
        if (m_pScrollBar->getIsAutoHide()) {
            m_pScrollBar->hide();
        }
    }
}

CCTableViewCell *ITGTableView::cellAtIndex(unsigned int idx)
{
    CCTableViewCell *found = NULL;
    
    if (m_pIndices->find(idx) != m_pIndices->end())
    {
        found = (CCTableViewCell *)m_pCellsUsed->objectWithObjectID(idx);
    }
    
    return found;
}

void ITGTableView::updateCellAtIndex(unsigned int idx)
{
    if (idx == CC_INVALID_INDEX)
    {
        return;
    }
    unsigned int uCountOfItems = m_pDataSource->numberOfCellsInTableView(this);
    if (0 == uCountOfItems || idx > uCountOfItems-1)
    {
        return;
    }
    
    CCTableViewCell* cell = this->cellAtIndex(idx);
    if (cell)
    {
        this->_moveCellOutOfSight(cell);
    }
    cell = m_pDataSource->tableCellAtIndex(this, idx);
    this->_setIndexForCell(idx, cell);
    this->_addCellIfNecessary(cell);
}

void ITGTableView::insertCellAtIndex(unsigned  int idx)
{
    if (idx == CC_INVALID_INDEX)
    {
        return;
    }
    
    unsigned int uCountOfItems = m_pDataSource->numberOfCellsInTableView(this);
    if (0 == uCountOfItems || idx > uCountOfItems-1)
    {
        return;
    }
    
    CCTableViewCell* cell = NULL;
    int newIdx = 0;
    
    cell = (CCTableViewCell*)m_pCellsUsed->objectWithObjectID(idx);
    if (cell)
    {
        newIdx = m_pCellsUsed->indexOfSortedObject(cell);
        for (unsigned int i=newIdx; i<m_pCellsUsed->count(); i++)
        {
            cell = (CCTableViewCell*)m_pCellsUsed->objectAtIndex(i);
            this->_setIndexForCell(cell->getIdx()+1, cell);
        }
    }
    
    //   [m_pIndices shiftIndexesStartingAtIndex:idx by:1];
    
    //insert a new cell
    cell = m_pDataSource->tableCellAtIndex(this, idx);
    this->_setIndexForCell(idx, cell);
    this->_addCellIfNecessary(cell);
    
    this->_updateCellPositions();
    this->_updateContentSize();
}

void ITGTableView::removeCellAtIndex(unsigned int idx)
{
    if (idx == CC_INVALID_INDEX)
    {
        return;
    }
    
    unsigned int uCountOfItems = m_pDataSource->numberOfCellsInTableView(this);
    if (0 == uCountOfItems || idx > uCountOfItems-1)
    {
        return;
    }
    
    unsigned int newIdx = 0;
    
    CCTableViewCell* cell = this->cellAtIndex(idx);
    if (!cell)
    {
        return;
    }
    
    newIdx = m_pCellsUsed->indexOfSortedObject(cell);
    
    //remove first
    this->_moveCellOutOfSight(cell);
    
    m_pIndices->erase(idx);
    this->_updateCellPositions();
    //    [m_pIndices shiftIndexesStartingAtIndex:idx+1 by:-1];
    for (unsigned int i=m_pCellsUsed->count()-1; i > newIdx; i--)
    {
        cell = (CCTableViewCell*)m_pCellsUsed->objectAtIndex(i);
        this->_setIndexForCell(cell->getIdx()-1, cell);
    }
}

CCTableViewCell *ITGTableView::dequeueCell()
{
    CCTableViewCell *cell;
    
    if (m_pCellsFreed->count() == 0) {
        cell = NULL;
    } else {
        cell = (CCTableViewCell*)m_pCellsFreed->objectAtIndex(0);
        cell->retain();
        m_pCellsFreed->removeObjectAtIndex(0);
        cell->autorelease();
    }
    return cell;
}

void ITGTableView::_addCellIfNecessary(CCTableViewCell * cell)
{
    if (cell->getParent() != this->getContainer())
    {
        this->getContainer()->addChild(cell);
    }
    m_pCellsUsed->insertSortedObject(cell);
    m_pIndices->insert(cell->getIdx());
    // [m_pIndices addIndex:cell.idx];
}

void ITGTableView::_updateContentSize()
{
    CCSize size = CCSizeZero;
    unsigned int cellsCount = m_pDataSource->numberOfCellsInTableView(this);
    
    if (cellsCount > 0)
    {
        float maxPosition = m_vCellsPositions[cellsCount];
        
        switch (this->getDirection())
        {
            case kITGScrollViewDirectionHorizontal:
                size = CCSizeMake(maxPosition, m_tViewSize.height);
                break;
            default:
                size = CCSizeMake(m_tViewSize.width, maxPosition);
                break;
        }
    }
    
    this->setContentSize(size);
    
	if (m_eOldDirection != m_eDirection)
	{
		if (m_eDirection == kITGScrollViewDirectionHorizontal)
		{
			this->setContentOffset(ccp(0,0));
		}
		else
		{
			this->setContentOffset(ccp(0,this->minContainerOffset().y));
		}
		m_eOldDirection = m_eDirection;
	}
    
}

CCPoint ITGTableView::_offsetFromIndex(unsigned int index)
{
    CCPoint offset = this->__offsetFromIndex(index);
    
    const CCSize cellSize = m_pDataSource->tableCellSizeForIndex(this, index);
    if (m_eVordering == kITGTableViewFillTopDown)
    {
        offset.y = this->getContainer()->getContentSize().height - offset.y - cellSize.height;
    }
    return offset;
}

CCPoint ITGTableView::__offsetFromIndex(unsigned int index)
{
    CCPoint offset;
    CCSize  cellSize;
    
    switch (this->getDirection())
    {
        case kITGScrollViewDirectionHorizontal:
            offset = ccp(m_vCellsPositions[index], 0.0f);
            break;
        default:
            offset = ccp(0.0f, m_vCellsPositions[index]);
            break;
    }
    
    return offset;
}

unsigned int ITGTableView::_indexFromOffset(CCPoint offset)
{
    int index = 0;
    const int maxIdx = m_pDataSource->numberOfCellsInTableView(this)-1;
    
    if (m_eVordering == kITGTableViewFillTopDown)
    {
        offset.y = this->getContainer()->getContentSize().height - offset.y;
    }
    index = this->__indexFromOffset(offset);
    if (index != -1)
    {
        index = MAX(0, index);
        if (index > maxIdx)
        {
            index = CC_INVALID_INDEX;
        }
    }
    
    return index;
}

int ITGTableView::__indexFromOffset(CCPoint offset)
{
    int low = 0;
    int high = m_pDataSource->numberOfCellsInTableView(this) - 1;
    float search;
    switch (this->getDirection())
    {
        case kITGScrollViewDirectionHorizontal:
            search = offset.x;
            break;
        default:
            search = offset.y;
            break;
    }
    
    while (high >= low)
    {
        int index = low + (high - low) / 2;
        float cellStart = m_vCellsPositions[index];
        float cellEnd = m_vCellsPositions[index + 1];
        
        if (search >= cellStart && search <= cellEnd)
        {
            return index;
        }
        else if (search < cellStart)
        {
            high = index - 1;
        }
        else
        {
            low = index + 1;
        }
    }
    
    if (low <= 0) {
        return 0;
    }
    
    return -1;
}

void ITGTableView::_moveCellOutOfSight(CCTableViewCell *cell)
{
    if(m_pTableViewDelegate != NULL) {
        m_pTableViewDelegate->tableCellWillRecycle(this, cell);
    }
    
    m_pCellsFreed->addObject(cell);
    m_pCellsUsed->removeSortedObject(cell);
    m_pIndices->erase(cell->getIdx());
    // [m_pIndices removeIndex:cell.idx];
    cell->reset();
    if (cell->getParent() == this->getContainer()) {
        this->getContainer()->removeChild(cell, true);;
    }
}

void ITGTableView::_setIndexForCell(unsigned int index, CCTableViewCell *cell)
{
    cell->setAnchorPoint(ccp(0.0f, 0.0f));
    cell->setPosition(this->_offsetFromIndex(index));
    cell->setIdx(index);
}

void ITGTableView::_updateCellPositions() {
    int cellsCount = m_pDataSource->numberOfCellsInTableView(this);
    m_vCellsPositions.resize(cellsCount + 1, 0.0);
    
    if (cellsCount > 0)
    {
        float currentPos = 0;
        CCSize cellSize;
        for (int i=0; i < cellsCount; i++)
        {
            m_vCellsPositions[i] = currentPos;
            cellSize = m_pDataSource->tableCellSizeForIndex(this, i);
            switch (this->getDirection())
            {
                case kITGScrollViewDirectionHorizontal:
                    currentPos += cellSize.width;
                    break;
                default:
                    currentPos += cellSize.height;
                    break;
            }
        }
        m_vCellsPositions[cellsCount] = currentPos;//1 extra value allows us to get right/bottom of the last cell
    }
    
}

void ITGTableView::scrollViewDidEndScroll(ITGScrollView *view)
{
    if (m_pTableViewDelegate) {
        m_pTableViewDelegate->scrollViewDidEndScroll(this);
    }
}

void ITGTableView::scrollViewDidScroll(ITGScrollView* view)
{
    unsigned int uCountOfItems = m_pDataSource->numberOfCellsInTableView(this);
    if (0 == uCountOfItems)
    {
        return;
    }
    
    if(m_pTableViewDelegate != NULL) {
        m_pTableViewDelegate->scrollViewDidScroll(this);
    }
    
    unsigned int startIdx = 0, endIdx = 0, idx = 0, maxIdx = 0;
    CCPoint offset = ccpMult(this->getContentOffset(), -1);
    maxIdx = MAX(uCountOfItems-1, 0);
    
    if (m_eVordering == kITGTableViewFillTopDown)
    {
        offset.y = offset.y + m_tViewSize.height/this->getContainer()->getScaleY();
    }
    startIdx = this->_indexFromOffset(offset);
	if (startIdx == CC_INVALID_INDEX)
	{
		startIdx = uCountOfItems - 1;
	}
    
    if (m_eVordering == kITGTableViewFillTopDown)
    {
        offset.y -= m_tViewSize.height/this->getContainer()->getScaleY();
    }
    else
    {
        offset.y += m_tViewSize.height/this->getContainer()->getScaleY();
    }
    offset.x += m_tViewSize.width/this->getContainer()->getScaleX();
    
    endIdx   = this->_indexFromOffset(offset);
    if (endIdx == CC_INVALID_INDEX)
	{
		endIdx = uCountOfItems - 1;
	}
    
#if 0 // For Testing.
    CCObject* pObj;
    int i = 0;
    CCARRAY_FOREACH(m_pCellsUsed, pObj)
    {
        CCTableViewCell* pCell = (CCTableViewCell*)pObj;
        CCLog("cells Used index %d, value = %d", i, pCell->getIdx());
        i++;
    }
    CCLog("---------------------------------------");
    i = 0;
    CCARRAY_FOREACH(m_pCellsFreed, pObj)
    {
        CCTableViewCell* pCell = (CCTableViewCell*)pObj;
        CCLog("cells freed index %d, value = %d", i, pCell->getIdx());
        i++;
    }
    CCLog("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
#endif
    
    if (m_pCellsUsed->count() > 0)
    {
        CCTableViewCell* cell = (CCTableViewCell*)m_pCellsUsed->objectAtIndex(0);
        
        idx = cell->getIdx();
        while(idx <startIdx)
        {
            this->_moveCellOutOfSight(cell);
            if (m_pCellsUsed->count() > 0)
            {
                cell = (CCTableViewCell*)m_pCellsUsed->objectAtIndex(0);
                idx = cell->getIdx();
            }
            else
            {
                break;
            }
        }
    }
    if (m_pCellsUsed->count() > 0)
    {
        CCTableViewCell *cell = (CCTableViewCell*)m_pCellsUsed->lastObject();
        idx = cell->getIdx();
        
        while(idx <= maxIdx && idx > endIdx)
        {
            this->_moveCellOutOfSight(cell);
            if (m_pCellsUsed->count() > 0)
            {
                cell = (CCTableViewCell*)m_pCellsUsed->lastObject();
                idx = cell->getIdx();
                
            }
            else
            {
                break;
            }
        }
    }
    
    for (unsigned int i=startIdx; i <= endIdx; i++)
    {
        //if ([m_pIndices containsIndex:i])
        if (m_pIndices->find(i) != m_pIndices->end())
        {
            continue;
        }
        this->updateCellAtIndex(i);
    }
}

void ITGTableView::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    if (!this->isVisible()) {
        return;
    }
    
    if (m_pTouchedCell)
    {
		CCRect bb = this->boundingBox();
        if (m_pParent)
        {
            bb.origin = m_pParent->convertToWorldSpace(bb.origin);
            
            if (bb.containsPoint(pTouch->getLocation()) && m_pTableViewDelegate != NULL)
            {
                m_pTableViewDelegate->tableCellUnhighlight(this, m_pTouchedCell);
                m_pTableViewDelegate->tableCellTouched(this, m_pTouchedCell);
            }
            
            m_pTouchedCell = NULL;
        }
    }
    
    ITGScrollView::ccTouchEnded(pTouch, pEvent);
}

bool ITGTableView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (!this->isVisible()) {
        return false;
    }
    
    bool touchResult = ITGScrollView::ccTouchBegan(pTouch, pEvent);
    
    if(m_pTouches->count() == 1) {
        unsigned int        index;
        CCPoint           point;
        
        point = this->getContainer()->convertTouchToNodeSpace(pTouch);
        
        index = this->_indexFromOffset(point);
		if (index == CC_INVALID_INDEX)
		{
			m_pTouchedCell = NULL;
		}
        else
		{
			m_pTouchedCell  = this->cellAtIndex(index);
		}
        
        if (m_pTouchedCell && m_pTableViewDelegate != NULL) {
            m_pTableViewDelegate->tableCellHighlight(this, m_pTouchedCell);
        }
    }
    else if(m_pTouchedCell) {
        if(m_pTableViewDelegate != NULL) {
            m_pTableViewDelegate->tableCellUnhighlight(this, m_pTouchedCell);
        }
        
        m_pTouchedCell = NULL;
    }
    
    return touchResult;
}

void ITGTableView::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    ITGScrollView::ccTouchMoved(pTouch, pEvent);
    
    if (m_pTouchedCell && isTouchMoved()) {
        if(m_pTableViewDelegate != NULL) {
            m_pTableViewDelegate->tableCellUnhighlight(this, m_pTouchedCell);
        }
        
        m_pTouchedCell = NULL;
    }
}

void ITGTableView::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    ITGScrollView::ccTouchCancelled(pTouch, pEvent);
    
    if (m_pTouchedCell) {
        if(m_pTableViewDelegate != NULL) {
            m_pTableViewDelegate->tableCellUnhighlight(this, m_pTouchedCell);
        }
        
        m_pTouchedCell = NULL;
    }
}

void ITGTableView::scrollingOffsetAtCellIndex(unsigned int idx, bool animate)
{
    
    CCPoint offset = offsetAtCellIndex(idx);
    if (animate) {
        setContentOffsetInDuration(offset, 0.5f);
    }
    else
    {
        setContentOffset(offset);
    }
    
}

CCPoint ITGTableView::offsetAtCellIndex(unsigned int idx)
{
    CCPoint p;
    CCSize cSize = getContentSize();
    CCSize tSize = cocos2d::CCNode::getContentSize();
    CCSize cellSize = getDataSource()->cellSizeForTable(this);
    float remain = 0;
    
    float offset = 0;
    //  horizontal 부분은 테스트 필요
    if (getDirection() == kITGScrollViewDirectionHorizontal) {
        if (cSize.width < tSize.width) {
            remain = tSize.width - cSize.width;
        }
        offset = cellSize.width * idx - remain;
        p = CCPointMake(offset, 0);
    }
    else
    {
        if (cSize.height < tSize.height) {
            remain = tSize.height - cSize.height;
        }
        
        offset = cellSize.height * idx - cSize.height + tSize.height - remain;
        if (offset < -cSize.height + tSize.height) {
            offset = -cSize.height + tSize.height;
        }
        else if(offset > 0)
        {
            offset = 0;
        }
        p = CCPointMake(0, offset);
    }
    
    return p;
}

void ITGTableView::reloadRetainOffset()
{
    CCPoint curOffset = getContentOffset();
    
    if (getDirection() == kITGScrollViewDirectionHorizontal)
    {
        reloadData();
        setContentOffset(curOffset);
    }
    else
    {
        CCSize size = getContainer()->getContentSize();
        
        reloadData();
        CCSize size2 = getContainer()->getContentSize();
        if (size2.height > size.height) {
            float increaseHeight = size2.height - size.height;
            curOffset.y -= increaseHeight;
            setContentOffset(curOffset);
        }
        else
        {
            setContentOffset(curOffset);
        }
    }
}

void ITGTableView::reloadCurrentViewCells()
{

    CCObject *pObj = NULL;
    CCARRAY_FOREACH(m_pCellsUsed, pObj)
    {
        CCTableViewCell *cell = (CCTableViewCell*)pObj;
        if (m_pDataSource) {
//            m_pDataSource->tableCellAtIndex(this, cell->getIdx());
            m_pDataSource->tableReloadCellAtIndex(this, (ITGBaseTableViewCell*)cell, cell->getIdx());
        }
    }
    
}


cocos2d::extension::CCArrayForObjectSorting* ITGTableView::getUsingCells()
{
    return m_pCellsUsed;
}


