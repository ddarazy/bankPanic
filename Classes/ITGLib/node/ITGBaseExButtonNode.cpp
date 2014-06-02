//
//  ITGBaseExButtonNode.cpp
//  Conan
//
//  Created by forestj on 13. 10. 1..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#include "ITGBaseExButtonNode.h"



ITGBaseExButtonNode::ITGBaseExButtonNode()
: m_pDelegate(NULL)
, m_iButtonId(-1)
{
}

ITGBaseExButtonNode::~ITGBaseExButtonNode()
{
}



bool ITGBaseExButtonNode::init()
{
    if (!CCNode::init()) {
        return false;
    }
	return true;
}