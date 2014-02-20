/**
*/

#ifndef CALLBACKS_H_INCLUDED
#define CALLBACKS_H_INCLUDED

using namespace std;

namespace Hlib
{

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
        cout << iPercentDone << endl;
        return iPercentDone;
    }

};

}   //  namespace Hlib

#endif
