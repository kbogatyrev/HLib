/**
*/

#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

#include <ios>
#ifdef WIN32
    #include <io.h>
#else
#endif
#include <fcntl.h>
#include <codecvt>
#include <locale>
#include <memory>

#include <vector>
#include <string>
#include <ctime>

#include <iostream>
#include <algorithm>

using namespace std;

namespace Hlib
{

    class CLogger;

    static CLogger* pLogger;

    class CLogger
    {
    public:
        CLogger()
        {
        }

        virtual ~CLogger() {
            delete pLogger;
        }

        static CLogger* pGetInstance()
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
        void LogUtf8(const char* pchrPath, const char* pchrFunction, unsigned int uiLine, const wchar_t* pwMsg)
        {
            string sLocation = string(pchrPath) + string("\t") + to_string(uiLine) + string("\t") + string(pchrFunction) + string("\t");
            auto wsDescription = wstring(const_cast<wchar_t*>(pwMsg));
            auto sFormattedMsg = sFormat(sToNarrowString(wsDescription).c_str(), sLocation.c_str());
            cout << sFormattedMsg << endl;
        }

        void LogWstr(const char* pchrPath, const char* pchrFunction, unsigned int uiLine, const wchar_t* pwMsg)
        {
#ifdef WIN32
            _setmode(_fileno(stdout), _O_U16TEXT);
#endif
            wstring wsLocation = wsToWstring(string(pchrPath)) + wstring(L"\t") + to_wstring(uiLine) + wstring(L"\t") + wsToWstring(string(pchrFunction)) + wstring(L"\t");
            auto wsOut = wsFormat(pwMsg, wsLocation.c_str());
            m_vecLog.push_back(wsOut);
            wcout << wsOut << endl;
        }

        void WriteMsgUtf8(const char* szBriefDescription,
            const char* szLocation,
            const char* szDetailedDescription = "",
            int iErrCode = -1)
        {
            string sFormattedMsg = sFormat(szBriefDescription, szLocation, szDetailedDescription, iErrCode);
            cout << sFormattedMsg.c_str() << endl;
        }

        wstring sGetLastError()
        {
            if (m_vecLog.size() > 0)
            {
                return m_vecLog.back();
            }
            else
            {
                wstring sMsg(L"No last error description.");
                return sMsg;
            }
        }

    private:
        wstring wsFormat(const wchar_t* wszBriefDescription,
            const wchar_t* wszLocation,
            const wchar_t* wszDetailedDescription = L"",
            int iErrCode = -1)
        {
            time_t timeCurrent;
            time(&timeCurrent);
            tm* pstLocalTime = localtime(&timeCurrent);
            wstring wsTimeStamp = to_wstring(pstLocalTime->tm_year + 1900);
            wsTimeStamp += L"-";
            wsTimeStamp += to_wstring(pstLocalTime->tm_mon + 1);
            wsTimeStamp += L"-";
            wsTimeStamp += to_wstring(pstLocalTime->tm_mday);
            wsTimeStamp += L"-";
            wsTimeStamp += to_wstring(pstLocalTime->tm_hour);
            wsTimeStamp += L":";
            wsTimeStamp += to_wstring(pstLocalTime->tm_min);
            wsTimeStamp += L":";
            wsTimeStamp += to_wstring(pstLocalTime->tm_sec);
            wstring wsMsg = wsTimeStamp + L"\t" + wszBriefDescription + L"\t" + wszLocation + L"\t";
            if (iErrCode >= 0)
            {
                wsMsg += L"\t" + to_wstring(iErrCode);
            }

            if (wszDetailedDescription)
            {
                wsMsg += L"\t";
                wsMsg += wszDetailedDescription;
            }

            return wsMsg;

        }    //  wsFormat(...)

        string sFormat(const char* szBriefDescription,
            const char* szLocation,
            const char* szDetailedDescription = "",
            int iErrCode = -1)
        {
            time_t timeCurrent;
            time(&timeCurrent);
            tm* pstLocalTime = localtime(&timeCurrent);
            string sTimeStamp = to_string(pstLocalTime->tm_year + 1900);
            sTimeStamp += "-";
            sTimeStamp += to_string(pstLocalTime->tm_mon + 1);
            sTimeStamp += "-";
            sTimeStamp += to_string(pstLocalTime->tm_mday);
            sTimeStamp += "-";
            sTimeStamp += to_string(pstLocalTime->tm_hour);
            sTimeStamp += ":";
            sTimeStamp += to_string(pstLocalTime->tm_min);
            sTimeStamp += ":";
            sTimeStamp += to_string(pstLocalTime->tm_sec);
            string sMsg = sTimeStamp + "\t" +
                szBriefDescription + "\t" +
                szLocation + "\t";
            if (iErrCode >= 0)
            {
                sMsg += "\t" + to_string(iErrCode);
            }

            if (szDetailedDescription)
            {
                sMsg += "\t";
                sMsg += szDetailedDescription;
            }

            return sMsg;

        }    //  sFormat (...)

        string sToNarrowString(wstring& wsSource)
        {
            mbstate_t state = mbstate_t();
            auto pwSource = wsSource.c_str();
            size_t len = 1 + wcsrtombs(nullptr, &pwSource, 0, &state);
            auto pUtf8 = make_unique<char[]>(len);
            wcsrtombs(pUtf8.get(), &pwSource, len, &state);
            return string(pUtf8.get());
        }

        wstring wsToWstring(const string& sSource)
        {
            mbstate_t state = mbstate_t();
            auto pSource = sSource.c_str();
            size_t len = 1 + mbsrtowcs(nullptr, &pSource, 0, &state);
            auto pWchr = make_unique<wchar_t[]>(len);
            mbsrtowcs(pWchr.get(), &pSource, len, &state);
            return wstring(pWchr.get());
        }

    };      //  CLogger

#define MESSAGE_LOG(sMsg__) {\
    CLogger * pErrorHandler__ = CLogger::pGetInstance(); \
        pErrorHandler__->LogWstr(__FILE__, __FUNCTION__, __LINE__, sMsg__); \
}

#define ERROR_LOG(sMsg__) {\
    CLogger * pErrorHandler__ = CLogger::pGetInstance(); \
        pErrorHandler__->LogWstr(__FILE__, __FUNCTION__, __LINE__, sMsg__); \
}

#define ASSERT(bBoolExpr__) if (!(bBoolExpr__)) {\
    {\
    CLogger * pErrorHandler__ = CLogger::pGetInstance(); \
        pErrorHandler__->LogUtf8(__FILE__, __FUNCTION__, __LINE__, L"Assertion failed."); \
    } \
}

}   // namespace Hlib

#endif
