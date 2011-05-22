#pragma once

//#include "stdafx.h"

class CException
{

public:
    enum EException;

protected:
    int m_iErrorCode;
    std::wstring m_sDescription;

public:

//
// C-tors and d-tors
//
public:
    CException()
    {
        m_iErrorCode = -1;
    };

    CException (int iErrorCode, const std::wstring& sDescription)
        : m_iErrorCode (iErrorCode), m_sDescription (sDescription)
    {}

    virtual ~CException() {};

public:
    int iGetErrorCode()
    {
        return m_iErrorCode;
    }
    wstring sGetDescription()
    {
        return m_sDescription;
    }

};
