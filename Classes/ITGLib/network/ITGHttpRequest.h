//
//  ZB_HttpRequest.h
//  ZombieA
//
//  Created by honglim jeon on 13. 5. 8..
//  Copyright (c) 2013년 __Andromeda Games__. All rights reserved.
//

#ifndef ZombieA_ITGHttpRequest_
#define ZombieA_ITGHttpRequest_

#include "cocos2d.h"
#include <curl/curl.h>


USING_NS_CC;

class ITGHttpRequestDelegate;



//  Http Request type
typedef enum {
    kHttpRequestNone = 0,
    kHttpRequestGet,
    kHttpRequestPost,
    kHttpRequestDownloadFile
}ITGHttpRequestType;

typedef enum _eRequestPageType
{
    kRequestPageType_None = 0,
    kRequestPageType_Common,                //  common use
    kRequestPageType_UserInfo,              //  use in user info page
    kRequestPageType_Town,                  //  use in town (all function)
    kRequestPageType_Mission,               //  use in mission page
    kRequestPageType_Crime,                 //  use in crime page
    kRequestPageType_Shop,                  //  use in shop page
    kRequestPageType_Gacha,                 //  use in gacha page
    kRequestPageType_Friends,               //  use in friends page
    kRequestPageType_Item,                  //  use in item page
    kRequestPageType_Present,               //  use in present page
    kRequestPageType_Collection,            //  use in collection page
    kRequestPageType_Options,               //  use in option page
    kRequestPageType_Debug,                 //  user in debug mode page
    kRequestPageType_Sell,
    kRequestPageType_Info
}eRequestPageType;


typedef struct _tRequestTask
{
    unsigned int        uid;
    ITGHttpRequestType  reqType;
    CCString            url;
    CCString            postData;
    CCObject            *pTarget;
    SEL_CallFuncND      pSelector;
    bool                isCanceled;
    eRequestPageType    eReqPageType;
    
    _tRequestTask()
    {
        uid         = 0;
        reqType     = kHttpRequestNone;
        pTarget     = NULL;
        pSelector   = NULL;
        isCanceled  = false;
        eReqPageType = kRequestPageType_None;
    }
    
}tRequestTask;

//  Request struct
typedef struct _ITGHttpRequestPacket{
    ITGHttpRequestType          reqType;
    CCString                    url;
    std::string                 reqData;
    std::vector<std::string>    files;
    uint32_t                    complete_count;     //  download / upload complete count
    uint32_t                    packet_uid;         //  uid
//    std::string                 reqName;
    std::string                 saveDir;
    CCObject                    *pTarget;
    SEL_CallFuncND              pSelector;
    ITGHttpRequestDelegate      *pDelegate;
    
    _ITGHttpRequestPacket()
    {
        reqType         = kHttpRequestNone;
        complete_count  = 0;
        packet_uid      = 0;
        pTarget         = NULL;
        pSelector       = NULL;
        pDelegate       = NULL;
    }
}ITGHttpRequestPacket;

//Response structure
typedef struct {
    ITGHttpRequestPacket        *request;
    tRequestTask                *reqTask;
    std::string                 responseData;
    int32_t                     responseCode;
    std::vector<std::string>    responseFiles;
    bool                        succeed;
} ITGHttpResponsePacket;

//  download / upload progress value
typedef struct _ITGHttpProgressValue
{
    double download_total;
    double download_now;
    double upload_total;
    double upload_now;
    uint32_t total_file_count;
    uint32_t complete_count;
    
    _ITGHttpProgressValue()
    {
        download_total  = 0;
        download_now    = 0;
        upload_total    = 0;
        upload_now      = 0;
        total_file_count = 0;
        complete_count  = 0;
    }

}ITGHttpProgressValue;


class ITGHttpRequestDelegate {
    
public:
    virtual void progressUpdate(ITGHttpProgressValue value)=0;
};

/**
 *  @brief  Singleton that handles asynchronous http requests
 *  Once the request complate, a callback will issued in main thread when it provided during make request
 */
class ITGHttpRequest : public cocos2d::CCObject
{

public:
	ITGHttpRequest();
	virtual ~ITGHttpRequest();
    
    /**
        @brief return the shared instance
        @return ZB_HttpRequest*
     */
    static ITGHttpRequest* sharedHttpRequest();
    
    /** Relase the shared instance **/
    static void purgeSharedHttpRequest();
    
    /**
     * Add a get request to task queue
     * @param url url want to get
     * @param pTarget  callback target
     * @param pSelector callback selector
     * @return unsigned int
     */
    unsigned int addGetTask(const char *url, CCObject *pTarget, SEL_CallFuncND pSelector);
    /**
        Sample Source :
     
     std::string url = "http://myurl.com/text.php?key=val";
     // request get data
     request->addGetTask(url, this, callfuncND_selector(Class::onHttpRequestCompleted));
     
     // call back function
     void Class::onHttpRequestCompleted(cocos2d::CCObject *pSender, void *data)
     {
        ITGHttpResponsePacket *response = (ITGHttpResponsePacket*)data;
        if (response->request->reqType == kHttpRequestGet || response->request->reqType == kHttpRequestPost)
        {
            if (response->succeed) {
                CCLog("Content: %s", response->responseData.c_str());
     
            } else {
                CCLog("Error: %s", response->responseData.c_str());
            }
        }
        else if (response->request->reqType == kHttpRequestDownloadFile)
        {
            if (response->succeed) {
                CCLog("Request file download complate!!");
                std::vector<std::string>::iterator iter;
                std::string filename;
                for (iter = response->responseFiles.begin(); iter != response->responseFiles.end(); ++iter) {
                    filename = *iter;
                    CCLOG("downloaded file : %s", (response->request->saveDir + ITGUtils::sharedUtils()->lastComponentFromFilePath(filename)).c_str());
                }
            }
            else
            {
                CCLog("Error: %s", response->responseData.c_str());
            }
        }
     }
     
     */
    /**
     * Add a get request to task queue
     * @param url the post destination url
     * @param postData data want to post
     * @param pTarget  callback target
     * @param pSelector callback selector
     * @return unsigned int
     */
    unsigned int addPostTask(const char *url, const char *postData, CCObject *pTarget, SEL_CallFuncND pSelector);
    /**
        Sample Source : 
     std::string url = "http://myurl.com/text.php";
     std::string postData = "key=val";
     // reqeust post data
     request->addPostTask(url, postData, this, callfuncND_selector(Class::onHttpRequestCompleted));
     */
    /**
     * Add a get request to task queue
     * @param url urls want to download
     * @param pTarget  callback target
     * @param pSelector callback selector
     * @return unsigned int
     */
    unsigned int addDownloadTask(std::vector<std::string> urls, const char *saveDir, cocos2d::CCObject *pTarget, SEL_CallFuncND pSelector, ITGHttpRequestDelegate *pDelegate);
    /**
        Sample Source :
     
     std::string url = "http://myurl.com/text.php";
     
     std::vector<std::string> downloads;
     // push download url
     downloads.push_back("http://www.baidu.com/index.html");
     downloads.push_back("http://i2.media.daumcdn.net/photo-media/201305/09/osen/20130509115204964.jpg");
     // request download files - set download save directory
     request->addDownloadTask(downloads, "/", this, callfuncND_selector(TitleScene::onHttpRequestCompleted));

     */
    


    void cancelAllOperation();
    

public:
    /**
     * Change the connect timeout
     * @param timeout
     * @return NULL
     */
    inline void setConnectTimeout(int32_t timeout) {m_connectTimeout = timeout;};
    
    /**
     * Change the download timeout
     * @param timeout
     * @return NULL
     */
    inline void setDownloadTimeout(int32_t timeout) {m_timeout = timeout;};
    
    /**
     * Get connect timeout
     * @return int
     */
    inline int32_t getConnectTimeout() {return m_connectTimeout;}
    
    /**
     * Get download timeout
     * @return int
     */
    inline int32_t getDownloadTimeout() {return m_timeout;}
    
    

private:
    /**
     * Init pthread mutex, semaphore, and create new thread for http requests
     * @return bool
     */
    bool lazyInitThreadSemphore();
    /** Poll function called from main thread to dispatch callbacks when http requests finished **/
    void httpRequestCallback(float delta);
    
public:
    CC_SYNTHESIZE_READONLY(bool, m_bUseSSL, UseSSL);
    
    
private:
    int32_t m_connectTimeout;
    int32_t m_timeout;
    unsigned int m_currentUid;
    
    bool m_bAutoPacketFilter;

};



#endif


