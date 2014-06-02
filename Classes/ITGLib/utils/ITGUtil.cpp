//
//  ITGUtil.cpp
//  Conan
//
//  Created by forestj on 13. 9. 30..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#include "ITGUtil.h"
#include "ITGDate.h"
#include "CCBReader.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#import <mach/mach.h>
#import <mach/mach_host.h>
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif


using namespace std;
USING_NS_CC;
USING_NS_CC_EXT;

static ITGUtil *_g_sharedUtilInstance = NULL;

ITGUtil::ITGUtil()
{
}

ITGUtil::~ITGUtil()
{
}

#pragma mark class functions Implements

ITGUtil* ITGUtil::sharedUtils()
{
    if (_g_sharedUtilInstance == NULL) {
        _g_sharedUtilInstance = new ITGUtil();
    }
    return _g_sharedUtilInstance;
}

string* ITGUtil::stringSplitByToken(const string str,const string token)
{
    string origin = str;
    int cutAt;
    int index = 0;
    string *strResult = new string[100];
    
    while ((cutAt = origin.find_first_of(token)) != origin.npos) {
        if (cutAt > 0) {
            strResult[index++] = origin.substr(0, cutAt);
        }
        origin = origin.substr(cutAt+1);
    }
    if (origin.length() > 0) {
        strResult[index++] = origin.substr(0, cutAt);
    }
    
    return strResult;
}

vector<string> ITGUtil::stringSplitToVecByToken(const string str, const string token)
{
    string ss = str;
    vector<string> resVec;
    int cutAt;
    while ((cutAt = ss.find_first_of(token)) != ss.npos) {
        if (cutAt > 0) {
            resVec.push_back(ss.substr(0, cutAt));
        }
        ss = ss.substr(cutAt+1);
    }
    if (ss.length() > 0) {
        resVec.push_back(ss.substr(0, cutAt));
    }
    
    return resVec;
}





CCNode* ITGUtil::readFromCCBI(const char *pClassName, const char *pCCBIFileName, cocos2d::extension::CCNodeLoader *pCCNodeLoader)
{
    CCNodeLoaderLibrary *pLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();
    pLib->registerCCNodeLoader(pClassName, pCCNodeLoader);
    CCBReader *pReader = new CCBReader(pLib);
    CCNode *node = pReader->readNodeGraphFromFile(pCCBIFileName);
    if (node != NULL) {
        return node;
    }
    CC_SAFE_DELETE(node);
    pReader->release();
    return NULL;
    
}



CCBAnimationManager* ITGUtil::getAnimationManagerFromReadCCBI(const char *pCCBIFileName, cocos2d::CCNode *pParentNode, int zIndex)
{
    CCBAnimationManager *pMng = NULL;
    CCNodeLoaderLibrary *pLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();
    CCBReader *pReader = new CCBReader(pLib);
    CCNode *pNode = pReader->readNodeGraphFromFile(pCCBIFileName);
    if (pParentNode) {
        pParentNode->addChild(pNode, zIndex);
    }

    pMng = pReader->getAnimationManager();
    if (pMng) {
        CC_SAFE_RETAIN(pMng);
    }
    
    pReader->release();
    
    return pMng;
}

CCBAnimationManager* ITGUtil::getAnimationManagerFromReadCCBI(const char *pCCBIFileName, cocos2d::CCNode *pOwner, cocos2d::CCNode *pParentNode, int zIndex)
{
    CCBAnimationManager *pMng = NULL;
    CCNodeLoaderLibrary *pLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();
    CCBReader *pReader = new CCBReader(pLib);
    CCNode *pNode = pReader->readNodeGraphFromFile(pCCBIFileName, pOwner);
    //    if (pOwner->retainCount() > 1) {
    //        pOwner->release();
    //    }
    if (pParentNode) {
        pParentNode->addChild(pNode, zIndex);
    }

    pMng = pReader->getAnimationManager();
    if (pMng) {
        CC_SAFE_RETAIN(pMng);
    }
    
    pReader->release();
    
    return pMng;
}


CCNode* ITGUtil::getNodeFromReadCCBI(const char *pCCBIFileName, cocos2d::CCNode *pOwner)
{
    CCNodeLoaderLibrary *pLib = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();
    CCBReader *pReader = new CCBReader(pLib);
    CCNode *pNode = pReader->readNodeGraphFromFile(pCCBIFileName, pOwner);
    
    //    if (pOwner->retainCount() > 1) {
    //        pOwner->release();
    //    }
    if (pNode) {
        CC_SAFE_RETAIN(pNode);
    }
    pReader->release();
    return pNode;
}

void ITGUtil::PrintAnimationSequences(cocos2d::CCArray *pArr)
{
    //    CCObject *pElement = NULL;
    
    for (int i=0; i<pArr->count(); i++) {
        CCBSequence *pSec = (CCBSequence*)pArr->objectAtIndex(i);
        CCLog("%s", pSec->getName());
    }
}


std::vector<const char*> ITGUtil::GetAnimationTypes(cocos2d::CCArray *pArrAnimations, const char *pAnimationNamePrefix)
{
    std::vector<const char*> vResult;
    
    if (pArrAnimations->count() == 0 || !pArrAnimations->data) {
        return vResult;
    }
    
    //    CCObject *pElement = NULL;
    //    CCARRAY_FOREACH(pArrAnimations, pElement)
    //    {
    //        CCBSequence *pSec = (CCBSequence*)pElement;
    //        if (strstr(pSec->getName(), pAnimationNamePrefix) != NULL) {
    //            vResult.push_back(pSec->getName());
    //        }
    //    }
    for (int i=0; i<pArrAnimations->count(); i++) {
        CCBSequence *pSec = (CCBSequence*)pArrAnimations->objectAtIndex(i);
        if (strstr(pSec->getName(), pAnimationNamePrefix) != NULL) {
            vResult.push_back(pSec->getName());
        }
    }
    
    return vResult;
}


ITG_MEM_INFO ITGUtil::memory_info()
{
    ITG_MEM_INFO mem_info;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    mach_port_t host_port;
    mach_msg_type_number_t host_size;
    vm_size_t pagesize;
    
    host_port = mach_host_self();
    host_size = sizeof(vm_statistics_data_t) / sizeof(int);
    host_page_size(host_port, &pagesize);
    
    vm_statistics_data_t vm_stat;
    
    if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size) != KERN_SUCCESS)
        CCLog("Failed to fetch vm statistics");
    
    /* Stats in bytes */
    mem_info.used = (vm_stat.active_count +
                     vm_stat.inactive_count +
                     vm_stat.wire_count) * pagesize;
    mem_info.free = vm_stat.free_count * pagesize;
    mem_info.total = mem_info.used + mem_info.free;
#endif
    return mem_info;
    
}

void ITGUtil::printMemoryInfo()
{
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ITG_MEM_INFO mem = memory_info();
    CCLog("Total : %.2f Mb", (float)mem.total / (float)(1024 * 1024));
    CCLog("Used : %.2f Mb", (float)(mem.used - start_mem.used) / (float)(1024 * 1024));
    CCLog("Free : %.2f Mb", (float)mem.free / (float)(1024 * 1024));
#endif
}
void ITGUtil::applicationStartMemoryCheck()
{
    start_mem = memory_info();
}

void ITGUtil::addCommaToNum(char *str, char *buf, int bufLen)
{
    int len = strlen(str);
    assert(len > 0);
    assert(bufLen >= len + ((len-1)/3)+1);
    
    switch ((len-1)%3) {
            while (*str) {
                *buf++ = ',';
            case 2: *buf++ = *str++;
            case 1: *buf++ = *str++;
            case 0: *buf++ = *str++;
            }
    }
    *buf++ = '\0';
}


bool ITGUtil::ORDER_TAG_ASC(CCObject *p1, CCObject *p2)
{
    return ((CCNode*)p1)->getTag() < ((CCNode*)p2)->getTag();
}



void ITGUtil::setTextureImage(const char *szImageName, cocos2d::CCSprite *pSprite, const char *szDefaultImageName)
{
    CCTexture2D *pTxt = NULL;
    CCTextureCache *pCache = CCTextureCache::sharedTextureCache();
    
    pTxt = pCache->textureForKey(szImageName);
    if (!pTxt) {
        pCache->addImage(szImageName);
        pTxt = pCache->textureForKey(szImageName);
    }
    if (pTxt) {
        pSprite->setTexture(pTxt);
        pSprite->setTextureRect(CCRectMake(0, 0, pTxt->getPixelsWide(), pTxt->getPixelsHigh()));
    }
}

bool ITGUtil::checkGreeting(time_t tLastGreetingDT)
{
    ITGDate now;
    ITGDate last = ITGDate(tLastGreetingDT);
    long checkDateHour = now.year() * 1000000 + now.month() * 10000 + now.mday() * 100 + kGreetingCheckHour;
    long lastGreetingHour = last.year() * 1000000 + last.month() * 10000 + (last.mday() + 1) * 100 + last.hour(); // + 1Day
    
    if (checkDateHour < lastGreetingHour)
        return false;
    
    return true;
}

void ITGUtil::setFontAutoresizingLabelTTF(cocos2d::CCLabelTTF *pLabel, const char* szText, cocos2d::CCSize tMaxDemension, float fMaxFontSize)
{
    pLabel->setFontSize(fMaxFontSize);
    
    CCSize textSize;
    do {
        pLabel->setString(szText);
        textSize = pLabel->getContentSize();
        
        if (textSize.width <= tMaxDemension.width && textSize.height <= tMaxDemension.height) {
            break;
        }
        else
        {
            pLabel->setFontSize(pLabel->getFontSize()-1);
        }
        
    } while (true);
    
}

