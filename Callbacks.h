/**
*/

#ifndef CALLBACKS_H_INCLUDED
#define CALLBACKS_H_INCLUDED

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

#endif
