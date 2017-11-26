/**
*/

#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

#include <sstream>
#include <ios>
#include <time.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <vector>
#include <string>
#include <ctime>

#ifdef WIN32
#include <crtdbg.h>
#include <tchar.h>
#include <Windows.h>
#include <Commdlg.h>
#else
#include <locale>
#endif

using namespace std;

namespace Hlib
{

class CLogger;

static CLogger * pLogger;

class CLogger
{
public:
    CLogger()
    {
#ifdef WIN32
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
    }

    virtual ~CLogger() {}

    static CLogger * pGetInstance()
    {
        if (pLogger)
        {
            return pLogger;
        }

        pLogger = new CLogger();

        return pLogger;
    }

private:
    vector<wstring> m_vecLog;

public:
    void DebugTrace(int uiType, wstring wFunction, int uiLine, const wchar_t * szMyMsg)
    {
        DebugTrace(uiType, L"", wFunction.c_str(), uiLine, szMyMsg);
    }

    void DebugTrace(unsigned int uiType, const wchar_t * pwchrPath, const wchar_t * pwchrFunction, unsigned int uiLine, const wchar_t * pwchrMyMsg)
    {
#ifdef _DEBUG
        wstring sTxt = wstring(pwchrMyMsg) + wstring(L"\r\n");
        auto iNewLength = sTxt.length();
        wchar_t * pTxt = new wchar_t[iNewLength + 1];
        errno_t err = wcscpy_s(pTxt, iNewLength + 1, sTxt.c_str());
        if (!err)
        {
#ifdef WIN32
            _CrtDbgReportW(uiType, pwchrPath, uiLine, NULL, L"%s", pTxt);
#endif
        }
        else
        {
#ifdef WIN32
            _CrtDbgReportW(uiType, pwchrPath, uiLine, NULL, L"%s", L"DebugTrace(): msg formatting failed");
#endif
        }
#endif

        wstringstream ioToString;
        ioToString << uiLine;
        wstring sLocation = wstring(pwchrPath) + wstring(L"\t") + ioToString.str() + wstring(L"\t") + wstring(pwchrFunction);
        CLogger * pErrorHandler = CLogger::pGetInstance();
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
        
//        DebugTrace(sFormattedMsg);
//        ATLTRACE2(sFormattedMsg.c_str());
//        ATLTRACE2(L"\r\n");

   }

    void Flush()
    {
        if (m_vecLog.empty())
        {
#ifdef WIN32
            ::MessageBox (NULL, L"No errors", L"ECSting Test", MB_ICONINFORMATION);
#endif
            return;
        }

#ifdef _WINDOWS
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
                BOOL uiRet = WriteFile (hf, sLine.c_str(), (DWORD)sLine.length()*sizeof(wchar_t), &dwBytesWritten, NULL);
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
    }

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
            sMsg += szDetailedDescription;
        }

        return sMsg;
    
    }    //  str_Format_ (...)
    public:

        static bool bWriteLog(const wstring& sMsg)
        {
#ifdef WIN32
            const wchar_t * pchrName = L"\\\\.\\pipe\\HMessageLog";

            HANDLE hPipe = NULL;
            while (1)
            {
                hPipe = CreateNamedPipe(
                    pchrName,
                    PIPE_ACCESS_OUTBOUND,
                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                    1,
                    512,
                    512,
                    0,
                    NULL);

                if (INVALID_HANDLE_VALUE != hPipe)
                {
                    break;
                }

                if (GetLastError() != ERROR_PIPE_BUSY)
                {
                    return false;
                }

                if (!WaitNamedPipe(pchrName, 500))
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

            BOOL python = ConnectNamedPipe(hPipe, NULL);
            DWORD dwWritten = 0;
            UINT uiRet = WriteFile(hPipe,           // pipe handle 
                sMsg.c_str(),    // message 
                (DWORD)sMsg.length() * sizeof(wchar_t),  // message length 
                &dwWritten,      // bytes written 
                NULL);           // not overlapped 
            CloseHandle(hPipe);

            return uiRet ? true : false;

        }    // bWriteLog()
#else
            return true;
#endif

};

#ifdef WIN32
#define MESSAGE_LOG(sMsg__) \
    CLogger * pLogger__ = CLogger::pGetInstance(); \
    pLogger__->LogMessage(sMsg__.c_str());

#define ERROR_LOG(sMsg__) \
    CLogger * pErrorHandler__ = CLogger::pGetInstance(); \
    pErrorHandler__->DebugTrace(_CRT_ERROR, _T(__FILE__), _T(__FUNCTION__), __LINE__, sMsg__);

#define ASSERT(bBoolExpr__) if (!(bBoolExpr__)) \
    {\
    CLogger * pErrorHandler__ = CLogger::pGetInstance(); \
        pErrorHandler__->DebugTrace(_CRT_ERROR, _T(__FILE__), _T(__FUNCTION__), __LINE__, L"Assertion failed."); \
    }
#else
#define MESSAGE_LOG(sMsg__) \
    CLogger * pLogger__ = CLogger::pGetInstance(); \
    pLogger__->LogMessage(sMsg__.c_str());

#define ERROR_LOG(sMsg__) \
    CLogger * pErrorHandler__ = CLogger::pGetInstance(); \
    wstring wFunction = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(__PRETTY_FUNCTION__); \
    pErrorHandler__->DebugTrace(0, wFunction, __LINE__, sMsg__);

#define ASSERT(bBoolExpr__) if (!(bBoolExpr__)) \
    {\
        CLogger * pErrorHandler__ = CLogger::pGetInstance(); \
        wstring wFunction = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(__PRETTY_FUNCTION__); \
        pErrorHandler__->DebugTrace(0, wFunction, __LINE__, L"Assertion failed."); \
    }

#endif

}   // namespace Hlib

#endif
