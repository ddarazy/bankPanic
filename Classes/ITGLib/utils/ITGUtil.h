//
//  ITGUtil.h
//  Conan
//
//  Created by forestj on 13. 9. 30..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#ifndef Conan_ITGUtil_
#define Conan_ITGUtil_

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ITGCustomNodeLoader.h"
//#include <iomanip>
#include <locale>


USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

typedef struct _ITG_MEM_INFO
{
    unsigned int used;
    unsigned int free;
    unsigned int total;
    
} ITG_MEM_INFO;

#define kGreetingCheckHour  16

class ITGUtil : public cocos2d::CCObject
{

private:
    ITG_MEM_INFO start_mem;
    
    
    

public:
	ITGUtil();
	virtual ~ITGUtil();
    
    static ITGUtil* sharedUtils();
    
    /**
     @brief string을 토큰으로 쪼개어 배열로 반환
     @param str origin string
     @param token token word
     @return string*
     */
    std::string* stringSplitByToken(const std::string str,const std::string token);
    /**
     @brief string을 토큰으로 쪼개어 배열로 반환
     @param str origin string
     @param token token word
     @return vector<string>
     */
    std::vector<std::string> stringSplitToVecByToken(const std::string str, const std::string token);
    
    
    /**
     @brief string join(배열로 이루어진 string을 구분자로 구분하여 하나의 문자열로 만들어준다.)
     */
    template <typename T>
    std::string stringJoin(const T &v, const std::string &delim)
    {
        std::ostringstream s;
        for (int i=0; i<v.size(); i++) {
            if (i != 0) {
                s << delim;
            }
            s << v[i];
        }
        
        return s.str();
    }
    
    
    
    
    /**
     @brief dynamic read ccbi class
     */
    static cocos2d::CCNode* readFromCCBI(const char* pClassName, const char* pCCBIFileName, cocos2d::extension::CCNodeLoader * pCCNodeLoader);
    
    /**
     @brief dynamic read ccbi & get CCBAnimationManager
     */
    static cocos2d::extension::CCBAnimationManager* getAnimationManagerFromReadCCBI(const char *pCCBIFileName, cocos2d::CCNode *pParentNode, int zIndex);
    
    static cocos2d::extension::CCBAnimationManager* getAnimationManagerFromReadCCBI(const char *pCCBIFileName, cocos2d::CCNode *pOwner, cocos2d::CCNode *pParentNode, int zIndex);
    
    static void PrintAnimationSequences(cocos2d::CCArray *pArr);
    
    static std::vector<const char*> GetAnimationTypes(cocos2d::CCArray *pArrAnimations, const char *pAnimationNamePrefix);
    
    static cocos2d::CCNode* getNodeFromReadCCBI(const char *pCCBIFileName, cocos2d::CCNode *pOwner);
    
    /**
     @brief ccbi를 layer로 로드하기 위한 함수. 로드 클래스를 동적생성하여 해당 로더로 클래스를 생성해준다.
     @param pClassName   - class name
     @param pCCBIFileName    - 로드할 ccbi 파일 명
     @param pOwner           - 로드할 ccbi의 owner - owner가 있으면 추가, 없으면 기본(document root)
     @param pParent          - 부모 노드, 부모노드가 있으면 자동으로 addchild 해주고 없으면 그냥 반환한다.
     */
    template <typename T>
    T* loadCustomLayerFromCCBI(const char *pClassName, const char* pCCBIFileName,  cocos2d::CCNode *pOwner, CCNode *pParent)
    {
        CCNodeLoaderLibrary *pLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();
        pLib->registerCCNodeLoader(pClassName, ITGCustomLayerLoader<T>::loader());
        
        CCBReader *pReader = new CCBReader(pLib);
        CCNode *pNode = NULL;
        if (pOwner) {
            pNode = pReader->readNodeGraphFromFile(pCCBIFileName, pOwner);
        }
        else
        {
            pNode = pReader->readNodeGraphFromFile(pCCBIFileName);
        }
        
        if (pNode) {
            if (pParent) {
                pParent->addChild(pNode);
            }
        }
        pReader->autorelease();
        
        return (T*)pNode;
    }
    
    
    /**
     @brief ccbi를 node로 로드하기 위한 함수. 로드 클래스를 동적생성하여 해당 로더로 클래스를 생성해준다.
     @param pClassName          - class name
     @param pCCBIFileName       - 로드할 ccbi 파일 명
     @param pOwner              - 로드할 ccbi의 owner - owner가 있으면 추가, 없으면 기본(document root)
     @param pParent             - 부모 노드, 부모노드가 있으면 자동으로 addchild 해주고 없으면 그냥 반환한다.
     */
    template <typename T>
    T* loadCustomNodeFromCCBI(const char *pClassName, const char* pCCBIFileName,  cocos2d::CCNode *pOwner, CCNode *pParent)
    {
        CCNodeLoaderLibrary *pLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();
        pLib->registerCCNodeLoader(pClassName, ITGCustomNodeLoader<T>::loader());
        
        CCBReader *pReader = new CCBReader(pLib);
        CCNode *pNode = NULL;
        if (pOwner) {
            pNode = pReader->readNodeGraphFromFile(pCCBIFileName, pOwner);
        }
        else
        {
            pNode = pReader->readNodeGraphFromFile(pCCBIFileName);
        }
        
        if (pNode) {
            
            if (pParent) {
                pParent->addChild(pNode);
            }
            
        }
//        pNode->retain();
        pReader->autorelease();
        
        
        
        return (T*)pNode;
    }
    
    /**
     @brief memory info
     
     */
    ITG_MEM_INFO memory_info(void);
    
    void printMemoryInfo();
    void applicationStartMemoryCheck();
    
    void addCommaToNum(char *str, char* buf, int bufLen);
    
    static bool ORDER_TAG_ASC(CCObject *p1, CCObject *p2);
    
    template <class T>
    std::string FormatWithCommas(T value)
    {
        stringstream ss;
        ss << fixed << value;
        string s = ss.str();
        string intPart, decPart;
        int pos = s.find(".");
        if (pos != string::npos) {
            intPart = s.substr(0, pos);
            decPart = s.substr(pos, pos-s.length());
            //  remove trailing zeros
            for (int i=decPart.length()-1; i > 0; i--) {
                if (decPart[i] == '0') {
                    decPart.erase(i);
                }
                else
                    break;
            }
            
            //  remove decimal point if no decimals
            if (decPart.length() == 0) {
                decPart = "";
            }
        }
        else
        {
            intPart = s;
        }

        if (intPart.length() > 3) {
            for (int i=intPart.length() - 3; i > 0 ; i -= 3) {
                intPart.insert(i, ",");
            }
        }
        
        s = intPart + decPart;
        return s;

    }
    
    void setTextureImage(const char* szImageName, CCSprite *pSprite, const char *szDefaultImageName = NULL);
    
    //  else project is deprecate
    bool checkGreeting(time_t tLastGreetingDT);
    
    void setFontAutoresizingLabelTTF(CCLabelTTF *pLabel, const char* szText, CCSize tMaxDemension, float fMaxFontSize=25);
    
    
    
    
};

#endif