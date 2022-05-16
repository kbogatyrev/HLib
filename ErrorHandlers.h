/**
*/

#ifndef ERRORHANDLERS_H_INCLUDED
#define ERRORHANDLERS_H_INCLUDED

#include <sstream>
#include <strstream>
#include <ios>

#include <vector>
#include <string>
#include <ctime>
#include <stddef.h>

using namespace std;

class CError;

static CError * pError;

class CError
{
public:
    CError() = default;

    virtual ~CError() {};

    static CError * pGetInstance()
    {
        if (pError)
        {
            return pError;
        }

        pError = new CError();

        return pError;
    }

private:
    vector<wstring> m_vecLog;

public:
    void DebugTrace(unsigned int uiType, const wchar_t * pwchrPath, const wchar_t * pwchrFunction, unsigned int uiLine, const wchar_t * pwchrMyMsg)
    {
#ifdef _DEBUG
        wstring sTxt = wstring(pwchrMyMsg) + wstring(L"\r\n");
        int iNewLength = sTxt.length();
        wchar_t * pTxt = new wchar_t[iNewLength + 1];
        wcscpy(pTxt, sTxt.c_str());
#endif
        wstringstream ioToString;
        ioToString << uiLine;
        wstring sLocation = wstring(pwchrPath) + wstring(_T("\t")) + ioToString.str() + wstring(_T("\t")) + wstring(pwchrFunction);
        CError * pErrorHandler = CError::pGetInstance();
        pErrorHandler->HandleError(pwchrMyMsg, sLocation.c_str());
    }

    void HandleError (const wchar_t * szBriefDescription,
                      const wchar_t * szLocation,
                      const wchar_t * szDetailedDescription = L"",
                      int iErrCode = -1,
                      bool bWrite = false) 
    {
        wstring sFormattedMsg = sFormat (szBriefDescription, szLocation, szDetailedDescription, iErrCode);
        if (bWrite)
        {
            bWriteLog (sFormattedMsg);
        }
        else
        {
            m_vecLog.push_back (sFormattedMsg);
        }
   }

    void Flush()
    {
        if (m_vecLog.empty())
        {
//            ::MessageBox (NULL, L"No errors", L"ECSting Test", MB_ICONINFORMATION);
            return;
        }

    
    }   //  void Flush()

public:
    wstring sGetLastError()
    {
        if (m_vecLog.size() > 0)
        {
            return m_vecLog.back();
        }
        else
        {
            wstring sMsg (L"No last error description.");
            return sMsg;
        }
    }

    template <typename T>
    wstring sToString (T from)
    {
        wstringstream io_;
        io_ << from;
        return io_.str();
    };

    wstring sFormat (const wchar_t * szBriefDescription, 
                     const wchar_t * szLocation, 
                     const wchar_t * szDetailedDescription = L"", 
                     int iErrCode = -1)
    {
        return sFormat_ (szBriefDescription,
                         szLocation,
                         szDetailedDescription,
                         iErrCode);
    }

private:
    wstring sFormat_ (const wchar_t * szBriefDescription, 
                      const wchar_t * szLocation, 
                      const wchar_t * szDetailedDescription, 
                      int iErrCode)
    {
        time_t timeCurrent;
        time (&timeCurrent);
        tm stLocalTime;
        errno_t iRet = localtime_s (&stLocalTime,  &timeCurrent);
        wstring sTimeStamp = sToString (stLocalTime.tm_year + 1900);
        sTimeStamp += L"-";
        sTimeStamp += sToString (stLocalTime.tm_mon + 1);
        sTimeStamp += L"-";
        sTimeStamp += sToString (stLocalTime.tm_mday);
        sTimeStamp += L"-";
        sTimeStamp += sToString (stLocalTime.tm_hour);
        sTimeStamp += L":";
        sTimeStamp += sToString (stLocalTime.tm_min);
        sTimeStamp += L":";
        sTimeStamp += sToString (stLocalTime.tm_sec);
        wstring sMsg = sTimeStamp + L"\t" +
                       szBriefDescription + L"\t" + 
                       szLocation + L"\t";
        if (iErrCode >= 0)
        {
            sMsg += L"\t" + sToString (iErrCode);
        }

        if (szDetailedDescription)
        {
            sMsg += L"\t";
            sMsg + szDetailedDescription;
        }

        return sMsg;
    
    }    //  str_Format_ (...)

    bool bWriteLog (const wstring& sMsg)
    {
        LPTSTR szName = L"\\\\.\\pipe\\ZalConversionLog"; 
  
        HANDLE hPipe = NULL;
        while (1) 
        { 
/*            
            hPipe = CreateFile (szName, 
                                GENERIC_WRITE,
                                0,              // no sharing 
                                NULL,           // default security attributes
                                OPEN_EXISTING,  // opens existing pipe 
                                0,              // default attributes 
                                NULL);          // no template file 
 
            if (INVALID_HANDLE_VALUE != hPipe) 
            {
                break;
            }
 
            if (GetLastError() != ERROR_PIPE_BUSY) 
            {
                return false;
            }
 
            if (!WaitNamedPipe (szName, 500)) 
            { 
                return false;
            } 
*/            
        }

/*
   dwMode = PIPE_READMODE_MESSAGE; 
   fSuccess = SetNamedPipeHandleState( 
      hPipe,    // pipe handle 
      &dwMode,  // new pipe mode 
      NULL,     // don't set maximum bytes 
      NULL);    // don't set maximum time 
   if (!fSuccess) 
   {
      printf("SetNamedPipeHandleState failed"); 
      return 0;
   }
*/

//        DWORD dwWritten = 0;
//        UINT uiRet = WriteFile (hPipe,            // pipe handle 
//                                sMsg.c_str(),   // message 
//                                sMsg.length()  * sizeof (wchar_t),  // message length 
//                                &dwWritten,       // bytes written 
//                                NULL);             // not overlapped 
//        CloseHandle (hPipe); 
 
        return uiRet ? true : false;

    }   // bWriteLog()

};

#define ERROR_LOG(sMsg__) \
    CError * pErrorHandler__ = CError::pGetInstance(); \
    pErrorHandler__->DebugTrace(__FILE__, __FUNCTION__, __LINE__, sMsg__);

#define ASSERT(bBoolExpr__) if (!(bBoolExpr__)) \
    {\
        CError * pErrorHandler__ = CError::pGetInstance(); \
        pErrorHandler__->DebugTrace(__FILE__, __FUNCTION__, __LINE__, L"Assertion failed."); \
    }

#endif
