//
//  ITGHttpRequest.cpp
//  IronTreeGlobal
//
//  Created by honglim jeon on 13. 10. 18..
//  Copyright (c) 2013년 __IronTreeGlobal__. All rights reserved.
//

#include "ITGHttpRequest.h"
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
//#include "../../../libs/cocos2dx/platform/CCThread.h"
#include "../../cocos2dx/platform/CCThread.h"
#include "ITGUtil.h"
#include "ITGFileUtils.h"

using namespace cocos2d;

static pthread_t        s_requestThread;
static pthread_mutex_t  s_requestQueueMutex;
static pthread_mutex_t  s_responseQueueMutex;
static sem_t*           s_pSem = NULL;
static unsigned long    s_asyncRequestCount = 0;

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#define CC_ASYNC_HTTPREQUEST_USE_NAMED_SEMAPHORE 1
#else
#define CC_ASYNC_HTTPREQUEST_USE_NAMED_SEMAPHORE 0
#endif

#if CC_ASYNC_HTTPREQUEST_USE_NAMED_SEMAPHORE
#define CC_ASYNC_HTTPREQUEST_SEMAPHORE "ccHttpAsync"
#else
static sem_t s_sem;
#endif

static bool need_quit = false;

static std::queue<ITGHttpRequestPacket *> *s_requestQueue = NULL;
static std::queue<ITGHttpResponsePacket *> *s_responseQueue = NULL;

static ITGHttpRequestPacket *requestPacket = NULL;

static ITGHttpRequest *_g_singleton_httprequest = NULL;

static char errorBuffer[CURL_ERROR_SIZE];
typedef size_t (*write_callback)(void *ptr, size_t size, size_t nmemb, void *stream);

//callback function used by libcurl for save file
size_t writeFile(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int written = fwrite(ptr, size, nmemb, (FILE *)stream);
    
    return written;
}

//callback function used by libcurl for collect response data
size_t writeData(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::string *str = (std::string *)stream;
    
    size_t sizes = size * nmemb;
    str->append((char *)ptr, sizes);
    
    return sizes;
}


#pragma mark local functions declared
void releaseRequestQueue();
bool configureCURL(CURL *handle);
int processGetTask(ITGHttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode);
int processPostTask(ITGHttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode);
int processDownloadTask(ITGHttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode);

void ITGHttpRequest::purgeSharedHttpRequest()
{
    CC_SAFE_RELEASE_NULL(_g_singleton_httprequest);
}

ITGHttpRequest *ITGHttpRequest::sharedHttpRequest()
{
    if (_g_singleton_httprequest == NULL) {
        _g_singleton_httprequest = new ITGHttpRequest();
    }
    
    return _g_singleton_httprequest;
}




#pragma mark local functions implements
//Worker thread
static void *requestThread(void *data)
{
    CCThread thread;
    thread.createAutoreleasePool();
    
//    ITGHttpRequestPacket *req = NULL;
    
    requestPacket = NULL;

    
    while (true) {
        //Wait for http request tasks from main thread
        int semWaitRet = sem_wait(s_pSem);
        if (semWaitRet < 0) {
            CCLog("HttpRequest async thread semaphore error: %s\n", strerror(errno));
            break;
        }
        
        std::queue<ITGHttpRequestPacket *> *pQueue = s_requestQueue;
        pthread_mutex_lock(&s_requestQueueMutex); //Get request task from queue
        if (pQueue->empty()) {
            pthread_mutex_unlock(&s_requestQueueMutex);
            
            if (need_quit) {
                CCLog("queue empty");
                break;
            } else {
                continue;
            }
        } else {
            if (need_quit) {
                CCLog("queue no empty");
                pthread_mutex_unlock(&s_requestQueueMutex);
                break;
            }
            CCLog("http continue!!");
//            req = pQueue->front();
            requestPacket = pQueue->front();
            pQueue->pop();
            pthread_mutex_unlock(&s_requestQueueMutex);
        }
        
        CCLog("ITGHttpRequest 01 : %s", requestPacket->url.getCString());
        
        //Create a response packet and assume it will successed
        ITGHttpResponsePacket *responsePacket = new ITGHttpResponsePacket();
//        responsePacket->request = req;
        responsePacket->request = requestPacket;
        responsePacket->succeed = true;
        CCLog("ITGHttpRequest 02");
        //Process the request
        if (requestPacket->reqType == kHttpRequestGet) { //Get Request
            int32_t ret = processGetTask(requestPacket, writeData, &responsePacket->responseData, &responsePacket->responseCode);
            if (ret != 0) {
                responsePacket->succeed = false;
                responsePacket->responseData = errorBuffer;
            }
        } else if (requestPacket->reqType == kHttpRequestPost) { //Post Request
            CCLog("ITGHttpRequest 03");
            int32_t ret = processPostTask(requestPacket, writeData, &responsePacket->responseData, &responsePacket->responseCode);
            CCLog("kHttpRequestPost result code : %d", ret);
            if (ret != 0) {
                responsePacket->succeed = false;
                responsePacket->responseData = errorBuffer;
            }
        } else if (requestPacket->reqType == kHttpRequestDownloadFile) { //Download File Request

            bool fullyDownloaded = true;
//            std::vector<std::string>::iterator iter;
            std::vector<std::string>::iterator iter;
            std::string saveFileName;
            std::string needDownload;
            
            ITGFileUtils::sharedUtils()->appendLastDirectorySeperator(requestPacket->saveDir);
            
            for (iter = requestPacket->files.begin(); iter != requestPacket->files.end(); ++iter) {
//                needDownload = *iter;
                needDownload = std::string(*iter);

                saveFileName = ITGFileUtils::sharedUtils()->lastComponentFromFilePath(needDownload);
                
                //If the download url is http://www.xxx.com/yyy.html
                //The saved file name must be yyy.html

//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//                req->saveDir = "/sdcard/CardCaptureFootball/" + req->saveDir;
//#endif
                CCLog("save dir : %s", requestPacket->saveDir.c_str());
                if (requestPacket->saveDir.empty()) {
                    CCLog("saveDir empty!!");
                    saveFileName = CCFileUtils::sharedFileUtils()->getWritablePath() + saveFileName;
                }
                else
                {
                    //  디렉토리 없으면 디렉토리 생성
                    if (!ITGFileUtils::sharedUtils()->isExistFile(requestPacket->saveDir)) {
                        CCLog("create directory!!");
                        if (!ITGFileUtils::sharedUtils()->createDirectory(requestPacket->saveDir)) {
                            CCLog("make directory failed!!");
                        }
                    }
                    
                    CCLog("requestPacket->saveDir : %s", requestPacket->saveDir.c_str());
                    CCLog("before %s", saveFileName.c_str());
//                    saveFileName = CCFileUtils::sharedFileUtils()->getWritablePath() + requestPacket->saveDir + saveFileName;
                    saveFileName = requestPacket->saveDir + saveFileName;
                    CCLog("after %s", saveFileName.c_str());
                }
                
                CCLog("saveFileName : %s", saveFileName.c_str());
                
                //  이미 캐쉬에 존재하는 파일일 경우 네트웍 다운로드를 요청하지 않는다.
                if (ITGFileUtils::sharedUtils()->isExistFileFullpath(saveFileName)) {
//                    CCLOG("already downloaded : %s", (responsePacket->request->saveDir + ZBFileUtils::sharedUtils()->lastComponentFromFilePath(saveFileName)).c_str());
                    responsePacket->responseFiles.push_back(saveFileName);
                    requestPacket->complete_count++;
                    continue;
                }
                else
                {
                    FILE *handle = fopen(saveFileName.c_str(), "w+");
                    if (!handle) {
                        fullyDownloaded = false;
                        break;
                    }
                    requestPacket->url = needDownload;
                    requestPacket->complete_count++;
                    CCLog("requestPacket->url : %s", requestPacket->url.getCString());
                    int32_t ret = processDownloadTask(requestPacket, writeFile, handle, &responsePacket->responseCode);
                    if (handle) {
                        fclose(handle);
                    }
                    if (ret != 0) {
                        fullyDownloaded = false;
                        CCLog("ITGHttpRequest download & write failed!! %d", ret);
                        ITGFileUtils::sharedUtils()->removeFile(saveFileName);
                        break;
                    }
                    else
                    {
                        CCLOG("download and save success : %s", (responsePacket->request->saveDir + ITGFileUtils::sharedUtils()->lastComponentFromFilePath(saveFileName)).c_str());
                        responsePacket->responseFiles.push_back(saveFileName);

                    }
                }


            }
            
            //Only consider download task successfully when all the files downloaded
            if (!fullyDownloaded) {
                responsePacket->succeed = false;
                responsePacket->responseData = errorBuffer;
            }
            
        }
        
        pthread_mutex_lock(&s_responseQueueMutex);
        s_responseQueue->push(responsePacket);
        pthread_mutex_unlock(&s_responseQueueMutex);
    }
    
    //If worker thread received quit signal, clean up un-completed request queue
    releaseRequestQueue();
    
    if (s_pSem != NULL) {
#if CC_ASYNC_HTTPREQUEST_USE_NAMED_SEMAPHORE
        sem_unlink(CC_ASYNC_HTTPREQUEST_SEMAPHORE);
        sem_close(s_pSem);
#else
        sem_destroy(s_pSem);
#endif
        
        s_pSem = NULL;
        
        pthread_mutex_destroy(&s_requestQueueMutex);
        pthread_mutex_destroy(&s_responseQueueMutex);
        
        delete s_requestQueue;
        delete s_responseQueue;
    }
    
    pthread_exit(NULL);
    
    return 0;
}

//Release Http request task queue
void releaseRequestQueue()
{
    pthread_mutex_lock(&s_requestQueueMutex);
    
    int32_t requestQueueSize = s_requestQueue->size();
    if (requestQueueSize > 0) {
        for (int32_t i = 0; i < requestQueueSize; ++i) {
            ITGHttpRequestPacket *packet = s_requestQueue->front();
            s_requestQueue->pop();
            
            delete packet;
        }
        
        s_asyncRequestCount -= requestQueueSize;
    }
    
    pthread_mutex_unlock(&s_requestQueueMutex);
}

//Configure curl's timeout property
bool configureCURL(CURL *handle)
{
    if (!handle) {
        return false;
    }
    
    int32_t code;
    code = curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errorBuffer);
//    CCLog("CURLOPT_ERRORBUFFER : %d", code);
    if (code != CURLE_OK) {
        return false;
    }

    code = curl_easy_setopt(handle, CURLOPT_TIMEOUT, ITGHttpRequest::sharedHttpRequest()->getDownloadTimeout());
//    CCLog("CURLOPT_TIMEOUT : %d", code);
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, ITGHttpRequest::sharedHttpRequest()->getConnectTimeout());
//    CCLog("CURLOPT_CONNECTTIMEOUT : %d", code);
    if (code != CURLE_OK) {
        return false;
    }
    
    return true;
}

int progressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
//    CCLog("dltotal : %f, dlnow : %f, ultotal : %f, ulnow : %f", dltotal, dlnow, ultotal, ulnow);
    
    if (requestPacket && requestPacket->pDelegate) {
        ITGHttpProgressValue val;
        val.download_total = dltotal;
        val.download_now = dlnow;
        val.upload_total = ultotal;
        val.upload_now = ulnow;
        val.total_file_count = requestPacket->files.size();
        val.complete_count = requestPacket->complete_count;
        
        requestPacket->pDelegate->progressUpdate(val);
        
    }
    
    return 0;
}

//Process Get Request
int processGetTask(ITGHttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode)
{
    CURLcode code = CURL_LAST;
    CURL *curl = curl_easy_init();
    
    do {
        if (!configureCURL(curl)) {
            break;
        }
        
        code = curl_easy_setopt(curl, CURLOPT_URL, task->url.getCString());
        if (code != CURLE_OK) {
            *responseCode = code;
            break;
        }
        code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        if (code != CURLE_OK) {
            *responseCode = code;
            break;
        }

        if (ITGHttpRequest::sharedHttpRequest()->getUseSSL())
        {
            code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
            if (code != CURLE_OK) {
                *responseCode = code;
                break;
            }
            
            code = curl_easy_setopt(curl, CURLOPT_SSLVERSION, 3);
            if (code != CURLE_OK) {
                *responseCode = code;
                break;
            }
        }
        
        if (task->pDelegate) {
            code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
            if (code != CURLE_OK) {
                *responseCode = code;
                break;
            }
            
            code = curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &progressCallback);
            if (code != CURLE_OK) {
                *responseCode = code;
                break;
            }
        }
        
        code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, stream);
        if (code != CURLE_OK) {
            *responseCode = code;
            break;
        }
        code = curl_easy_perform(curl);
        if (code != CURLE_OK) {
            *responseCode = code;
            break;
        }
        
        code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, responseCode);
        if (code != CURLE_OK || *responseCode / 100 != 2) {
            code = CURLE_HTTP_RETURNED_ERROR;
        }
    } while (0);
    if (curl) {
        curl_easy_cleanup(curl);
    }
    
    
    return (code == CURLE_OK ? 0 : 1);
}

//Process POST Request
int processPostTask(ITGHttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode)
{
    CURLcode code = CURL_LAST;
    CURL *curl = curl_easy_init();
    
    do {
        if (!configureCURL(curl)) {
            break;
        }
        
        code = curl_easy_setopt(curl, CURLOPT_URL, task->url.getCString());
        if (code != CURLE_OK) {
            *responseCode = code;
            break;
        }
        code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        if (code != CURLE_OK) {
            *responseCode = code;
            break;
        }
        if (ITGHttpRequest::sharedHttpRequest()->getUseSSL())
        {
            code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
            if (code != CURLE_OK) {
                *responseCode = code;
                break;
            }
            
            code = curl_easy_setopt(curl, CURLOPT_SSLVERSION, 3);
            if (code != CURLE_OK) {
                *responseCode = code;
                break;
            }
        }

        code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, stream);
        if (code != CURLE_OK) {
            *responseCode = code;
            break;
        }

        code = curl_easy_setopt(curl, CURLOPT_POST, 1);
        if (code != CURLE_OK) {
            *responseCode = code;
            break;
        }
        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, task->reqData.c_str());
        if (code != CURLE_OK) {
            *responseCode = code;
            break;
        }
        code = curl_easy_perform(curl);
        if (code != CURLE_OK) {
            *responseCode = code;
            break;
        }
        
        code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, responseCode);
        if (code != CURLE_OK || *responseCode / 100 != 2) {
            code = CURLE_HTTP_RETURNED_ERROR;
        }
    } while (0);
    if (curl) {
        curl_easy_cleanup(curl);
    }
    
    return (code == CURLE_OK ? 0 : 1);
}


//Process Download Request
int processDownloadTask(ITGHttpRequestPacket *task, write_callback callback, void *stream, int32_t *responseCode)
{
    return processGetTask(task, callback, stream, responseCode);
}

ITGHttpRequest::ITGHttpRequest()
{
    m_timeout = 60;
    m_connectTimeout = 30;
    m_bAutoPacketFilter = false;
    m_currentUid = 0;
    m_bUseSSL = true;
}

ITGHttpRequest::~ITGHttpRequest()
{
    need_quit = true;
    
    if (s_pSem != NULL) {
        sem_post(s_pSem);
    }
}

#pragma mark class functions implements

bool ITGHttpRequest::lazyInitThreadSemphore()
{
    if (s_pSem != NULL) {
        return true;
    } else {
#if CC_ASYNC_HTTPREQUEST_USE_NAMED_SEMAPHORE
        s_pSem = sem_open(CC_ASYNC_HTTPREQUEST_SEMAPHORE, O_CREAT, 0644, 0);
        if (s_pSem == SEM_FAILED) {
            CCLog("Open HttpRequest Semaphore failed");
            s_pSem = NULL;
            return false;
        }
#else
        int semRet = sem_init(&s_sem, 0, 0);
        if (semRet < 0) {
            CCLog("Init HttpRequest Semaphore failed");
            return false;
        }
        
        s_pSem = &s_sem;
#endif
        
        s_requestQueue = new std::queue<ITGHttpRequestPacket *>();
        s_responseQueue = new std::queue<ITGHttpResponsePacket *>();
        
        pthread_mutex_init(&s_requestQueueMutex, NULL);
        pthread_mutex_init(&s_responseQueueMutex, NULL);
        
        pthread_create(&s_requestThread, NULL, requestThread, NULL);
        pthread_detach(s_requestThread);
        
        need_quit = false;
    }
    
    return true;
}

unsigned int ITGHttpRequest::addGetTask(const char *url, cocos2d::CCObject *pTarget, SEL_CallFuncND pSelector)
{
    bool init = lazyInitThreadSemphore();
    if (!init) {
        return NULL;
    }
    
    if (0 == s_asyncRequestCount) {
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(ITGHttpRequest::httpRequestCallback), this, 0, false);
    }
    
    m_currentUid++;
    
    ++s_asyncRequestCount;
    if (pTarget) {
        pTarget->retain();
    }
    
    ITGHttpRequestPacket *packet = new ITGHttpRequestPacket();
    packet->reqType = kHttpRequestGet;
    packet->url = url;
//    packet->url = std::string(url);
    packet->pTarget = pTarget;
    packet->pSelector = pSelector;
    packet->packet_uid = m_currentUid;
    
    pthread_mutex_lock(&s_requestQueueMutex);
    s_requestQueue->push(packet);
    pthread_mutex_unlock(&s_requestQueueMutex);
    
    //Notify thread start to work
    sem_post(s_pSem);

    return packet->packet_uid;
}

unsigned int ITGHttpRequest::addPostTask(const char *url, const char *postData, cocos2d::CCObject *pTarget, SEL_CallFuncND pSelector)
{
    bool init = lazyInitThreadSemphore();
    if (!init) {
        return NULL;
    }
    
    m_currentUid++;
    
    if (0 == s_asyncRequestCount) {
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(ITGHttpRequest::httpRequestCallback), this, 0, false);
    }
    
    ++s_asyncRequestCount;
    if (pTarget) {
        pTarget->retain();
    }
    
    ITGHttpRequestPacket *packet = new ITGHttpRequestPacket();
    packet->reqType = kHttpRequestPost;
//    packet->url = std::string(url);
    packet->url = url;
    CCLog("url : %s", packet->url.getCString());
    packet->reqData = std::string(postData);
    CCLog("postData : %s", packet->reqData.c_str());
    packet->pTarget = pTarget;
    packet->pSelector = pSelector;
    packet->packet_uid = m_currentUid;
    
    pthread_mutex_lock(&s_requestQueueMutex);
    CCLog("pthread_mutex_lock post");
    s_requestQueue->push(packet);
    pthread_mutex_unlock(&s_requestQueueMutex);
    
    //Notify thread start to work
    CCLog("Notify thread post");
    sem_post(s_pSem);
    

    return packet->packet_uid;
}

unsigned int ITGHttpRequest::addDownloadTask(std::vector<std::string> urls, const char *saveDir, cocos2d::CCObject *pTarget, SEL_CallFuncND pSelector, ITGHttpRequestDelegate *pDelegate)
{
    bool init = lazyInitThreadSemphore();
    if (!init) {
        return NULL;
    }
        m_currentUid++;
    
    if (0 == s_asyncRequestCount) {
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(ITGHttpRequest::httpRequestCallback), this, 0, false);
    }
    
    ++s_asyncRequestCount;
    if (pTarget) {
        pTarget->retain();
    }
    
    ITGHttpRequestPacket *packet = new ITGHttpRequestPacket();
    packet->reqType = kHttpRequestDownloadFile;
    packet->files = urls;

    packet->pTarget = pTarget;
    packet->pSelector = pSelector;
    packet->pDelegate = pDelegate;
    packet->complete_count = 0;
    packet->packet_uid = m_currentUid;
    


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//    packet->saveDir = CCFileUtils::sharedFileUtils()->getWritablePath() + std::string(saveDir);
    packet->saveDir = CCFileUtils::sharedFileUtils()->getWritablePath();
    ITGFileUtils::sharedUtils()->appendLastPathComponent(packet->saveDir, saveDir);
#else
    packet->saveDir = std::string(saveDir);
#endif

    pthread_mutex_lock(&s_requestQueueMutex);
    s_requestQueue->push(packet);
    pthread_mutex_unlock(&s_requestQueueMutex);
    
    sem_post(s_pSem);
    
    return packet->packet_uid;
}

void ITGHttpRequest::httpRequestCallback(float delta)
{
    std::queue<ITGHttpResponsePacket *> *pQueue = s_responseQueue;
    
    pthread_mutex_lock(&s_responseQueueMutex);
    if (pQueue->empty()) {
        pthread_mutex_unlock(&s_responseQueueMutex);
    } else {
        
        CCLog("ITGHttpRequest::httpRequestCallback");
        ITGHttpResponsePacket *packet = pQueue->front();
        pQueue->pop();
        
        pthread_mutex_unlock(&s_responseQueueMutex);
        
        --s_asyncRequestCount;
        if (0 == s_asyncRequestCount) {
            CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(ITGHttpRequest::httpRequestCallback), this);
        }
        
        ITGHttpRequestPacket *orgRequest = packet->request;
        CCObject *pTarget = orgRequest->pTarget;
        SEL_CallFuncND pSelector = orgRequest->pSelector;
        
        //  packet filter
        if (m_bAutoPacketFilter) {
            CCLog("packet->succeed : %d, %s", (int)packet->succeed, packet->responseData.c_str());

            if (packet->request->reqType != kHttpRequestDownloadFile) {
                if (packet->succeed) {
//                    CCLog("success len : %d", packet->responseData.length());
                    
                    if (packet->responseData.length() > 0) {
//                        std::string::size_type pos = packet->responseData.find("{");
//                        CCLog("result data warning : %s", packet->responseData.c_str());
//                        if (pos != 1) {
//                            if (pos != std::string::npos) {
//                                packet->responseData = packet->responseData.substr(pos);
//                            }
//                            else
//                                packet->succeed = false;
//                            
//                        }
                    }
                    else
                    {
                        CCLog("data length is zero!!");
                    }
                }
                
            }
        }
        
        if (pTarget && pSelector) {
            (pTarget->*pSelector)((CCNode *)this, packet);
            
            pTarget->release();
        }
        
        delete orgRequest;
        delete packet;
    }

}

void ITGHttpRequest::cancelAllOperation()
{
    
}


