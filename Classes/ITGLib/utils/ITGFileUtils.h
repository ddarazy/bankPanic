//
//  ZBFileUtils.h
//  ZombieA
//
//  Created by forestj on 13. 5. 10..
//  Copyright (c) 2013년 Andromeda Games. All rights reserved.
//

#ifndef ZombieA_ITGFileUtils_
#define ZombieA_ITGFileUtils_

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class ITGFileUtils : public cocos2d::CCObject
{

public:

	virtual ~ITGFileUtils();
    
    static ITGFileUtils* sharedUtils();
    
    /**
     @brief 디렉토리 생성(각 os별 생성이 가능한 디렉토리를 root로 하여 원하는 디렉토리를 생성한다.)
     @param path
     @return bool
     */
    bool createDirectory(const std::string path);
    
    bool removeFile(const std::string fullpath);
    
    bool removeDirectory(const std::string fullpath);
    
    /**
     @brief 파일 유무 체크(각 os별 접근이 가능한 root디렉토리를 기준으로 파일 유무 체크)
     @param path 파일의 존재 유무를 체크할 경로
     @return bool
     */
    bool isExistFile(const std::string path);
    
    bool isExistFileFullpath(const std::string fullpath);
    
    /**
     @breif path의 마지막에 "/"가 있는지 여부 체크. 없을 경우 자동으로 마지막에 "/"를 붙여준다.
     */
    void appendLastDirectorySeperator(std::string &path);
    
    /**
        @brief path의 마지막에 다른 패스를 추가하기 위한 함수
     */
    void appendLastPathComponent(std::string &path, std::string filename);
    
    /**
     @brief url이나 파일 경로에서 가장 마지막에 있는 파일명(디렉토리명)을 찾는다.
     @param path
     @result string
     */
    std::string lastComponentFromFilePath(const std::string path);
    
    /**
     @brief url이나 파일 경로, 파일명에서 해당 파일의 확장명을 반환한다.
     @param path
     @return string
     */
    std::string findFileFormat(const std::string path);
    
    /**
     @brief path의 파일을 읽어 텍스트 파일을 반환한다.
     */
    std::string fileRead(const std::string path, const char* pszMode);

    std::string getAppendRootDirectory(const std::string file);

    /**
        @brief 해당 디렉토리에 .nomedia 파일을 생성한다.
     */
    bool createNoMedia(const std::string path);
    
    /**
        @breif uncompress zip file
     */
    bool uncompress(const std::string file, const std::string storagePath);

};

#endif