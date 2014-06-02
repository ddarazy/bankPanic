//
//  ITGCustomNodeLoader.h
//  Conan
//
//  Created by forestj on 13. 9. 30..
//
//

#ifndef Conan_ITGCustomNodeLoader_h
#define Conan_ITGCustomNodeLoader_h


#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

template<typename createClass>
class ITGCustomLayerLoader : public CCLayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(ITGCustomLayerLoader, loader);
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(createClass);
};

template<typename createClass>
class ITGCustomNodeLoader : public CCNodeLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(ITGCustomNodeLoader, loader);
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(createClass);
};


#endif
