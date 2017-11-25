#ifndef C_EXCEPTION_H_INCLUDED
#define C_EXCEPTION_H_INCLUDED

#include <algorithm>
#include "Enums.h"
#include "Logging.h"

namespace Hlib
{

static const unsigned int cuiMaxTextLength = 5000;
    
class CException
{

protected:
    int m_iErrorCode;
    wchar_t m_arrDescription[cuiMaxTextLength+1];

public:

//
// C-tors and d-tors
//
public:
    CException() : m_iErrorCode(-1)
    {}

    CException (int iErrorCode, const wchar_t * szDescription) : m_iErrorCode (iErrorCode)
    {
        auto iLength = min ((unsigned int)wcslen (szDescription), cuiMaxTextLength);
#ifdef WIN32
        int error = wmemmove_s (m_arrDescription, cuiMaxTextLength, szDescription, iLength);                
        if (error)
        {
            ASSERT(0);
            wchar_t * szMsg = L"wmemmove_s failed.";
            ERROR_LOG (szMsg);
        }
#else
        memmove(m_arrDescription, szDescription, iLength);
#endif
        m_arrDescription[iLength] = L'\0';
    }

    virtual ~CException() {};

public:
    int iGetErrorCode()
    {
        return m_iErrorCode;
    }

    wchar_t * szGetDescription()
    {
        return m_arrDescription;
    }

};


}   //  namespace Hlib

#endif
