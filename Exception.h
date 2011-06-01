#pragma once

#include "stdafx.h"

class CException
{

    static const unsigned int cuiMaxTextLength = 5000;

public:
    enum EException;

protected:
    int m_iErrorCode;
    wchar_t m_arrDescription[cuiMaxTextLength+1];

public:

//
// C-tors and d-tors
//
public:
    CException()
    {
        m_iErrorCode = -1;
    };

    CException (int iErrorCode, const wchar_t * szDescription)
        : m_iErrorCode (iErrorCode)
    {
        int iLength = min (wcslen (szDescription), cuiMaxTextLength);
        memmove_s (m_arrDescription, cuiMaxTextLength, szDescription, iLength);                
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
