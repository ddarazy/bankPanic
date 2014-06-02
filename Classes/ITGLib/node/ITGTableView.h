//
//  ITGTableView.h
//  Conan
//
//  Created by forestj on 13. 9. 30..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#ifndef Conan_ITGTableView_
#define Conan_ITGTableView_

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ITGScrollView.h"
#include "CCTableViewCell.h"
#include "CCSorting.h"

#include <set>
#include <vector>

USING_NS_CC;
USING_NS_CC_EXT;

class ITGTableView;
class CCArrayForObjectSorting;

typedef enum {
    kITGTableViewFillTopDown,
    kITGTableViewFillBottomUp
} ITGTableViewVerticalFillOrder;

/**
 * Sole purpose of this delegate is to single touch event in this version.
 */
class ITGTableViewDelegate : public ITGScrollViewDelegate
{
public:
    /**
     * Delegate to respond touch event
     *
     * @param table table contains the given cell
     * @param cell  cell that is touched
     */
    virtual void tableCellTouched(ITGTableView* table, CCTableViewCell* cell) = 0;
    
    /**
     * Delegate to respond a table cell press event.
     *
     * @param table table contains the given cell
     * @param cell  cell that is pressed
     */
    virtual void tableCellHighlight(ITGTableView* table, CCTableViewCell* cell){};
    
    /**
     * Delegate to respond a table cell release event
     *
     * @param table table contains the given cell
     * @param cell  cell that is pressed
     */
    virtual void tableCellUnhighlight(ITGTableView* table, CCTableViewCell* cell){};
    
    /**
     * Delegate called when the cell is about to be recycled. Immediately
     * after this call the cell will be removed from the scene graph and
     * recycled.
     *
     * @param table table contains the given cell
     * @param cell  cell that is pressed
     */
    virtual void tableCellWillRecycle(ITGTableView* table, CCTableViewCell* cell){};
    
};

class ITGBaseTableViewCell;
/**
 * Data source that governs table backend data.
 */
class ITGTableViewDataSource
{
public:
    virtual ~ITGTableViewDataSource() {}
    
    /**
     * cell size for a given index
     *
     * @param idx the index of a cell to get a size
     * @return size of a cell at given index
     */
    virtual CCSize tableCellSizeForIndex(ITGTableView *table, unsigned int idx) {
        return cellSizeForTable(table);
    };
    /**
     * cell height for a given table.
     *
     * @param table table to hold the instances of Class
     * @return cell size
     */
    virtual CCSize cellSizeForTable(ITGTableView *table) {
        return CCSizeZero;
    };
    /**
     * a cell instance at a given index
     *
     * @param idx index to search for a cell
     * @return cell found at idx
     */
    virtual CCTableViewCell* tableCellAtIndex(ITGTableView *table, unsigned int idx) = 0;
    /**
     * Returns number of cells in a given table view.
     *
     * @return number of cells
     */
    virtual unsigned int numberOfCellsInTableView(ITGTableView *table) = 0;
    
    /**
        a cell refresh
     *  @param cell
     *  @param idx
     */
    virtual void tableReloadCellAtIndex(ITGTableView *table, ITGBaseTableViewCell *cell, unsigned int idx){}
    
};


class ITGTableView : public ITGScrollView, public ITGScrollViewDelegate
{

public:
    ITGTableView();
    virtual ~ITGTableView();
    
    /**
     * An intialized table view object
     *
     * @param dataSource data source
     * @param size view size
     * @return table view
     */
    static ITGTableView* create(ITGTableViewDataSource* dataSource, CCSize size);
    /**
     * An initialized table view object
     *
     * @param dataSource data source;
     * @param size view size
     * @param container parent object for cells
     * @return table view
     */
    static ITGTableView* create(ITGTableViewDataSource* dataSource, CCSize size, CCNode *container);
    
    /**
     * data source
     */
    ITGTableViewDataSource* getDataSource() { return m_pDataSource; }
    void setDataSource(ITGTableViewDataSource* source) { m_pDataSource = source; }
    /**
     * delegate
     */
    ITGTableViewDelegate* getDelegate() { return m_pTableViewDelegate; }
    void setDelegate(ITGTableViewDelegate* pDelegate) { m_pTableViewDelegate = pDelegate; }
    
    /**
     * determines how cell is ordered and filled in the view.
     */
    void setVerticalFillOrder(ITGTableViewVerticalFillOrder order);
    ITGTableViewVerticalFillOrder getVerticalFillOrder();
    
    
    bool initWithViewSize(CCSize size, CCNode* container = NULL);
    /**
     * Updates the content of the cell at a given index.
     *
     * @param idx index to find a cell
     */
    void updateCellAtIndex(unsigned int idx);
    /**
     * Inserts a new cell at a given index
     *
     * @param idx location to insert
     */
    void insertCellAtIndex(unsigned int idx);
    /**
     * Removes a cell at a given index
     *
     * @param idx index to find a cell
     */
    void removeCellAtIndex(unsigned int idx);
    /**
     * reloads data from data source.  the view will be refreshed.
     */
    void reloadData();
    /**
     * Dequeues a free cell if available. nil if not.
     *
     * @return free cell
     */
    CCTableViewCell *dequeueCell();
    
    /**
     * Returns an existing cell at a given index. Returns nil if a cell is nonexistent at the moment of query.
     *
     * @param idx index
     * @return a cell at a given index
     */
    CCTableViewCell *cellAtIndex(unsigned int idx);
    
    
    virtual void scrollViewDidScroll(ITGScrollView* view);
    virtual void scrollViewDidZoom(ITGScrollView* view) {}
    virtual void scrollViewDidEndScroll(ITGScrollView* view);
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    /**
     @brief 해당 셀의 인덱스로 셀의 위치 정보를 가져오기 위한 함수
     */
    cocos2d::CCPoint offsetAtCellIndex(unsigned int idx);
    /**
     @brief 해당 셀 위치로 이동하기 위한 함수
     */
    void scrollingOffsetAtCellIndex(unsigned int idx, bool animate);
    /**
     @brief reloadData를 호출해도 현재 유지되고 있는 위치를 그대로 유지시켜주기 위한 함수
     */
    void reloadRetainOffset();
    
    void reloadCurrentViewCells();
    /**
        @brief 현재 사용중(화면에 보여지는)인 셀리스트를 반환 - 테이블 리스트 갱신시 사용
     */
    cocos2d::extension::CCArrayForObjectSorting* getUsingCells();
    
protected:
    
    CCTableViewCell *m_pTouchedCell;
    /**
     * vertical direction of cell filling
     */
    ITGTableViewVerticalFillOrder m_eVordering;
    
    /**
     * index set to query the indexes of the cells used.
     */
    std::set<unsigned int>* m_pIndices;
    
    /**
     * vector with all cell positions
     */
    std::vector<float> m_vCellsPositions;
    //NSMutableIndexSet *indices_;
    /**
     * cells that are currently in the table
     */
    cocos2d::extension::CCArrayForObjectSorting* m_pCellsUsed;
    /**
     * free list of cells
     */
    cocos2d::extension::CCArrayForObjectSorting* m_pCellsFreed;
    /**
     * weak link to the data source object
     */
    ITGTableViewDataSource* m_pDataSource;
    /**
     * weak link to the delegate object
     */
    ITGTableViewDelegate* m_pTableViewDelegate;
    
	ITGScrollViewDirection m_eOldDirection;
    
    int __indexFromOffset(CCPoint offset);
    unsigned int _indexFromOffset(CCPoint offset);
    CCPoint __offsetFromIndex(unsigned int index);
    CCPoint _offsetFromIndex(unsigned int index);
    
    void _moveCellOutOfSight(CCTableViewCell *cell);
    void _setIndexForCell(unsigned int index, CCTableViewCell *cell);
    void _addCellIfNecessary(CCTableViewCell * cell);
    
    void _updateCellPositions();
public:
    void _updateContentSize();

};

#endif