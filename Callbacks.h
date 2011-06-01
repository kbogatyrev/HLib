/**
*/

#pragma once

#include "StdAfx.h"

using namespace std;

class CProgressCallback
{
//private:

public:
    CProgressCallback()
    {
    }

    ~CProgressCallback()
    {
    }
    
    virtual int operator()(int iPercentDone) const 
    { 
        return iPercentDone; 
    }

};
