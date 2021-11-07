#ifndef C_EXCEPTION_H_INCLUDED
#define C_EXCEPTION_H_INCLUDED

//#include "stdafx.h"
//#include <ASSERT.h>
#include <algorithm>
#include "Enums.h"
#include "Logging.h"

namespace Hlib
{

class CException
{
    static const unsigned int cuiMaxTextLength = 5000;

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
        errno_t error = wmemmove_s (m_arrDescription, cuiMaxTextLength, szDescription, iLength);                
        if (error)
        {
            ASSERT(0);
            const wchar_t * szMsg = L"wmemmove_s failed.";
            ERROR_LOG (szMsg);
        }
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
