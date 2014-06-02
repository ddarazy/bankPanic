//
//  ZBFileUtils.cpp
//  ZombieA
//
//  Created by forestj on 13. 5. 10..
//  Copyright (c) 2013년 Andromeda Games. All rights reserved.
//

#include "ITGFileUtils.h"
#include "ITGUtil.h"
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include <dirent.h>
#include <sys/stat.h>
#endif


#include "support/zip_support/unzip.h"

#define ITG_BUFFER_SIZE     8192
#define ITG_MAX_FILENAME    512


static ITGFileUtils *_g_sharedInstance = NULL;

#pragma mark local functions define
bool makeDirectory(const std::string path);


#pragma mark Contructor & Destroy


ITGFileUtils::~ITGFileUtils()
{
}

#pragma mark local functions Implements
bool makeDirectory(const std::string path)
{
//  old version
//    string command = string("mkdir '") + path + "'";
//    CCLog("makeDirectory : %s", command.c_str());
//    return system(command.c_str()) == 0 ? true : false;
    
    //  new version
    mode_t processMask = umask(0);
    int ret = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    umask(processMask);
    if (ret != 0 && (errno != EEXIST)) {
        return false;
    }

    return true;

}


#pragma mark class functions Implements

ITGFileUtils* ITGFileUtils::sharedUtils()
{
    if (_g_sharedInstance == NULL) {
        _g_sharedInstance = new ITGFileUtils();
    }
    return _g_sharedInstance;
}

bool ITGFileUtils::removeFile(const std::string fullpath)
{

//    string command = string("rm -f '") + path + "'";
//    CCLog("removeFile : %s", command.c_str());
//    return system(command.c_str()) == 0 ? true : false;
    
    if (CCFileUtils::sharedFileUtils()->isFileExist(fullpath)) {
        int ret = remove(fullpath.c_str());
        return ret == 0 ? true : false;
    }
    else
        return true;
    
}



bool ITGFileUtils::removeDirectory(const std::string fullpath)
{
    if (CCFileUtils::sharedFileUtils()->isFileExist(fullpath))
    {
        DIR *pDir = NULL;
        
        pDir = opendir (fullpath.c_str());
        
        if (!pDir)
        {
            removeFile(fullpath);
        }
        else
        {
            struct dirent *dir_entry;
            DIR *isDir = NULL;
            while ((dir_entry = readdir(pDir)) != NULL)
            {
                if (!(strcmp(dir_entry->d_name, ".") == 0) && !(strcmp(dir_entry->d_name, "..") == 0))
                {
                    string path = fullpath + dir_entry->d_name;
                    
                    isDir = opendir(path.c_str());
                    if (isDir) {
                        removeDirectory(path);
                        closedir(isDir);
                    }
                    else
                    {
                        removeFile(path);
                    }
//                    CCLog("isDir : %s", isDir ? "true" : "false");
//                    CCLog("dir name : %s", path.c_str());
                }
                
            }
            closedir(pDir);
        }
        

        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        string command = string("rm -rf '") + fullpath + "'";
        CCLog("removeDirectory : %s", command.c_str());
        return system(command.c_str()) == 0 ? true : false;
#else
        int ret = rmdir(fullpath.c_str());
        CCLog("fullpath : %s, result : %d", fullpath.c_str(), ret);
        return ret == 0 ? true : false;
#endif
        


    }
    else
        return true;
}


bool ITGFileUtils::isExistFile(const string path)
{
    string dir = CCFileUtils::sharedFileUtils()->getWritablePath() + path;
//    CCLog("isExistFile : %d", (int)CCFileUtils::sharedFileUtils()->isFileExist(dir));
    return CCFileUtils::sharedFileUtils()->isFileExist(dir);
}

bool ITGFileUtils::isExistFileFullpath(const std::string fullpath)
{
    return CCFileUtils::sharedFileUtils()->isFileExist(fullpath);
}

std::string ITGFileUtils::getAppendRootDirectory(const std::string file)
{
    string dir = CCFileUtils::sharedFileUtils()->getWritablePath() + file;
    
    return dir;
}

//  application root 디렉토리를 기준으로 원하는 경로에 디렉토리 설정
bool ITGFileUtils::createDirectory(const string path)
{
    if (isExistFile(path)) {
        return true;
    }
    string dir = CCFileUtils::sharedFileUtils()->getWritablePath();
    
    string::size_type pos = path.rfind(dir);
    string path2;
//    CCLog("path find pos : %d, length : %d", pos, path.length());
    if (pos != std::string::npos) {
        path2 = path.substr(dir.length());
    }
    else
        path2 = path;
    
    //  depth가 2단계 이상일 경우 이전 depth의 디렉토리가 존재하지 않으면 다음 depth의 디렉토리를 생성할 수 없다.
    //  그래서 각 depth 단계별로 디렉토리가 존재하는지 체크한 후 없을 경우 디렉토리를 생성한다.
    vector<string> vPathTokenArray = ITGUtil::sharedUtils()->stringSplitToVecByToken(path2, "/");

    
    
//    for (string &i : vPathTokenArray) {
//        dir.append(i).append("/");
//        if (!CCFileUtils::sharedFileUtils()->isFileExist(dir)) {
//            //            CCLog("no exist : %s", dir.c_str());
//            makeDirectory(dir);
//        }
//    }
    
    for (int i=0; i<vPathTokenArray.size(); i++)
    {
        string s = vPathTokenArray[i];
//        CCLog("path token : %s", s.c_str());
        dir.append(s).append("/");
        if (!CCFileUtils::sharedFileUtils()->isFileExist(dir)) {
            makeDirectory(dir);
        }
        
    }
    return CCFileUtils::sharedFileUtils()->isFileExist(dir);
}


void ITGFileUtils::appendLastDirectorySeperator(std::string &path)
{
    if (path.length() == 0) {
        return;
    }
    string::size_type pos = path.rfind("/");
    if (pos != string::npos) {
        if (pos != path.length()-1) {
            path.append("/");
        }
    }
    else
    {
        path.append("/");
    }
}

void ITGFileUtils::appendLastPathComponent(std::string &path, std::string filename)
{
    appendLastDirectorySeperator(path);
    
    string::size_type pos = filename.find("/");
    if (pos != 0) {
        path.append(filename);
    }
    else
    {
        path.append(filename.substr(1));
    }
}



string ITGFileUtils::lastComponentFromFilePath(const std::string path)
{
    string filename;
    std::string::size_type pos = path.rfind("/");
    
    if (pos != std::string::npos) {
        filename = path.substr(pos + 1);
    } else {
        filename = path;
    }
    return filename;
}

string ITGFileUtils::findFileFormat(const std::string path)
{
    string format;
    string filename = lastComponentFromFilePath(path);
    
    std::string::size_type pos = filename.rfind(".");
    
    if (pos != std::string::npos) {
        format = filename.substr(pos + 1);
    } else {
        format = filename;
    }
    
    return format;
}

string ITGFileUtils::fileRead(const std::string path, const char* pszMode)
{
    unsigned char *pBuffer = NULL;
    unsigned long bufferSize = 0;
    
    pBuffer = CCFileUtils::sharedFileUtils()->getFileData(path.c_str(), pszMode, &bufferSize);
    return string((char*)pBuffer);
}

bool ITGFileUtils::createNoMedia(const std::string path)
{

    string filepath = path + ".nomedia";
    if (isExistFile(filepath)) {
        return true;
    }
    return makeDirectory(filepath);
}


bool ITGFileUtils::uncompress(const std::string file, const std::string storagePath)
{
    unzFile zipfile = unzOpen(file.c_str());
    
    if (!zipfile) {
        CCLog("can not open download zip file %s", file.c_str());
        unzClose(zipfile);
        return false;
    }
    
    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK) {
        CCLog("can not read file global info of %s", file.c_str());
        unzClose(zipfile);
        return false;
    }
    
    char readBuffer[ITG_BUFFER_SIZE];
    
//    uLong i;
    for (int i=0; i<global_info.number_entry; ++i) {
        unz_file_info file_info;
        char fileName[ITG_MAX_FILENAME];
        if (unzGetCurrentFileInfo(zipfile, &file_info, fileName, ITG_MAX_FILENAME, NULL, 0, NULL, 0) != UNZ_OK) {
            CCLog("can not read file info");
            unzClose(zipfile);
            return false;
        }
        
//        string fullPath = CCFileUtils::sharedFileUtils()->getWritablePath() + fileName;
        string fullPath = storagePath + fileName;
        
        string checkFile;
        checkFile.append(fileName);
        

        string::size_type pos = checkFile.rfind("/");
        if (pos != string::npos) {
            checkFile = checkFile.substr(0, pos);

            string dir;
            dir.append(storagePath);
            dir.append(checkFile);

            createDirectory(dir);
            
        }
        
//        CCLog("file info full path : %s", fullPath.c_str());
//        CCLog("unzip file name : %s", fileName);
        
        const size_t filenamelength = strlen(fileName);
        if (fileName[filenamelength-1] == '/') {
            // Entry is a direcotry, so create it.
            // If the directory exists, it will failed scilently.
            if (!createDirectory(fullPath))
            {
                CCLOG("can not create directory %s", fullPath.c_str());
                unzClose(zipfile);
                return false;
            }
        }
        else
        {
            // Entry is a file, so extract it.
            
            // Open current file.
            if (unzOpenCurrentFile(zipfile) != UNZ_OK)
            {
                CCLOG("can not open file %s", fileName);
                unzClose(zipfile);
                return false;
            }
            
            
            // Create a file to store current file.
            FILE *out = fopen(fullPath.c_str(), "wb");
            if (! out)
            {
                CCLOG("can not open destination file %s", fullPath.c_str());
                unzCloseCurrentFile(zipfile);
                unzClose(zipfile);
                return false;
            }
            
            // Write current file content to destinate file.
            int error = UNZ_OK;
            do
            {
                error = unzReadCurrentFile(zipfile, readBuffer, ITG_BUFFER_SIZE);
                if (error < 0)
                {
                    CCLOG("can not read zip file %s, error code is %d", fileName, error);
                    unzCloseCurrentFile(zipfile);
                    unzClose(zipfile);
                    return false;
                }
                
                if (error > 0)
                {
                    fwrite(readBuffer, error, 1, out);
                }
            } while(error > 0);
            
            fclose(out);
            
        }
        
        unzCloseCurrentFile(zipfile);
        
        // Goto next entry listed in the zip file.
        if ((i+1) < global_info.number_entry)
        {
            if (unzGoToNextFile(zipfile) != UNZ_OK)
            {
                CCLOG("can not read next file");
                unzClose(zipfile);
                return false;
            }
        }
        
    }

    return true;
}

