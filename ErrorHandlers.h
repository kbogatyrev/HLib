/**
*/

#ifndef ERRORHANDLERS_H_INCLUDED
#define ERRORHANDLERS_H_INCLUDED

#include <sstream>
#include <strstream>
#include <ios>

#include <windows.h>
#include <vector>
#include <string>
#include <ctime>

#ifdef WIN32
#include <crtdbg.h>
#include <tchar.h>
#endif

using namespace std;

class CError;

static CError * pError;

class CError
{
public:
    CError()
    {
#ifdef WIN32
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
    };

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
    void DebugTrace(int uiType, wstring wPath, wstring wFunction, int uiLine, const wchar_t * szMyMsg)
    {
        DebugTrace(uiType, wPath.c_str(), wFunction.c_str(), uiLine, szMyMsg);
    }

    void DebugTrace(unsigned int uiType, const wchar_t * pwchrPath, const wchar_t * pwchrFunction, unsigned int uiLine, const wchar_t * pwchrMyMsg)
    {
#ifdef _DEBUG
        wstring sTxt = wstring(pwchrMyMsg) + wstring(L"\r\n");
        int iNewLength = sTxt.length();
        wchar_t * pTxt = new wchar_t[iNewLength + 1];
        errno_t err = wcscpy_s(pTxt, iNewLength + 1, sTxt.c_str());
        if (!err)
        {
            _CrtDbgReportW(uiType, pwchrPath, uiLine, NULL, L"%s", pTxt);
        }
        else
        {
            _CrtDbgReportW(uiType, pwchrPath, uiLine, NULL, L"%s", L"DebugTrace(): msg formatting failed");
        }
#endif

        wstringstream ioToString;
        ioToString << uiLine;
        wstring sLocation = wstring(pwchrPath) + wstring(L("\t")) + ioToString.str() + wstring(L("\t")) + wstring(pwchrFunction);
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
#ifdef WIN32
        if (m_vecLog.empty())
        {
            ::MessageBox (NULL, L"No errors", L"ECSting Test", MB_ICONINFORMATION);
            return;
        }

        OPENFILENAME ofn;       // common dialog box structure
        wchar_t szFile[260];       // buffer for file name
        HWND hwnd;              // owner window
        HANDLE hf;              // file handle

        // Initialize OPENFILENAME
        ZeroMemory (&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd = 0;
        ofn.lpstrFile = szFile;
        // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
        // use the contents of szFile to initialize itself.
        ofn.lpstrFile[0] = L'\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_OVERWRITEPROMPT;

        // Display the Open dialog box. 

        if (GetSaveFileName (&ofn) == TRUE) 
        {
            hf = CreateFile (ofn.lpstrFile, 
                             GENERIC_WRITE,
                             0,
                             (LPSECURITY_ATTRIBUTES) NULL,
                             CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             (HANDLE) NULL);
            if (INVALID_HANDLE_VALUE == hf)
            {
                ::MessageBox (NULL, L"Unable to create log file", L"Kai Errors", MB_ICONERROR);
                return;
            }

            vector<wstring>::iterator it_ = m_vecLog.begin();
            for (; it_ != m_vecLog.end(); ++it_)
            {
                wstring sLine (*it_);
                sLine += L"\r\n";
                DWORD dwBytesWritten = 0;
                BOOL uiRet = WriteFile (hf, sLine.c_str(), sLine.length()*sizeof(wchar_t), &dwBytesWritten, NULL);
            }
        }
#endif    
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
#ifdef WIN32
        localtime_s (&stLocalTime,  &timeCurrent);
#else
        stLocalTime = *localtime(&timeCurrent);
#endif
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
#ifdef WIN32
        LPTSTR szName = L"\\\\.\\pipe\\ZalConversionLog"; 
  
        HANDLE hPipe = NULL;
        while (1) 
        { 
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

        DWORD dwWritten = 0;
        UINT uiRet = WriteFile (hPipe,            // pipe handle 
                                sMsg.c_str(),   // message 
                                sMsg.length()  * sizeof (wchar_t),  // message length 
                                &dwWritten,       // bytes written 
                                NULL);             // not overlapped 
        CloseHandle (hPipe); 
 
        return uiRet ? true : false;
#else
        return true;
#endif

    }   // bWriteLog()

};

#define ERROR_LOG(sMsg__) \
    CError * pErrorHandler__ = CError::pGetInstance(); \
    pErrorHandler__->DebugTrace(_CRT_ERROR, _T(__FILE__), _T(__FUNCTION__), __LINE__, sMsg__);

#define ASSERT(bBoolExpr__) if (!(bBoolExpr__)) \
    {\
        CError * pErrorHandler__ = CError::pGetInstance(); \
        pErrorHandler__->DebugTrace(_CRT_ERROR, _T(__FILE__), _T(__FUNCTION__), __LINE__, L"Assertion failed."); \
    }

#endif
