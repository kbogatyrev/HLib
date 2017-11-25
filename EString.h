#ifndef C_ESTRING_H_INCLUDED
#define C_ESTRING_H_INCLUDED

#ifdef WIN32
#include "tchar.h"
#else
#include <wchar.h>
#endif

#include <codecvt>
#include <regex>
#include <string>
#include "Exception.h"

#ifndef WIN32
typedef int errno_t;
#endif

using namespace std;

namespace Hlib
{

static const wstring str_CyrillicLowercase = L"-абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
static const wchar_t * g_szRusVowels = L"аеёиоуыэюя";
static const wchar_t * g_szRusConsonants = L"бвгджзйклмнпрстфхцчшщ";
static const wchar_t * g_szVoicedConsonants = L"бвгджзйлмнр";
static const wchar_t * g_szNonVoicedConsonants = L"кпстфхцчшщ";
static const wchar_t * g_szHushers = L"жчшщ";

static const wchar_t * szDefaultBreakChars_ = L" \n";
static const wchar_t * szDefaultTabs_ = L"\t";
static const wchar_t * szDefaultPunctuation_ = L".,;:/?<>[]{}~!()-_\'\"\\";
//static const wchar_t * szDefaultPunctuation_ = L".,;:?!\'\"";
static const wchar_t * szDefaultEscapeChars_ = L"\27";
//static const wchar_t * szDefaultVowels_ = L"аеёиоуыэюя";
static unsigned int uiBlockSize_ = 10;
static const unsigned int cuiMaxSize_ = 100000;
static const unsigned int cuiMaxRegexLength_ = 2000;
static const unsigned int cuiMaxVowelsLength_ = 100;
static const unsigned int cuiMaxSeparatorLength_ = 1000;
static const unsigned int cuiMaxConversionLength_ = 129;
static const unsigned int cuiMaxSearchStringLength_ = 1000;
static const unsigned int cuiMaxSearchSetLength_ = 1000;

//
// Helper objects
//
enum ERelation
{
    ecRelFront      = 20001,
    ecEqual,
    ecLess,
    ecGreater,
    ecNotFound      = 2*cuiMaxSize_,
    ecRelBack       = ecGreater + 1
};

enum ETokenType
{
    ecTokenTypeFront        = 10001,
    ecTokenText,
    ecTokenSpace,           // sequence of break chars
    ecTokenBreakChars       = ecTokenSpace,
    ecTokenTab,             // sequence of tabs
    ecTokenDiacritics,
    ecTokenMeta,            // e.g., XML tag
    ecTokenPunctuation,     // sequence of punct. marks i.e. ; or ?!...
    ecTokenEndOfParagraph,  // B6 hex
    ecTokenRegexMatch,
    ecTokenTypeBack         = ecTokenRegexMatch + 1
};

struct StToken
{
    ETokenType eType;
    unsigned int uiOffset;
    unsigned int uiLength;
    unsigned int uiPosition;

    StToken()
    {
        eType = ecTokenTypeFront;
        uiOffset = 0;
        uiLength = 0;
    }

    virtual ~StToken() {};

    //
    // These operators are needed so we can sort tokens:
    //
    bool operator < (const StToken& stToken) const
    {
        return uiOffset < stToken.uiOffset;
    }

    bool operator == (const StToken& stToken) const
    {
        bool bType = (eType == stToken.eType);
        bool bOffset = (uiOffset == stToken.uiOffset);
        bool bLength = (uiLength == stToken.uiLength);

        return (bType && bOffset && bLength);
    }

    bool bIsLinearText() const
    {
        if (ecTokenTypeFront == eType ||
            ecTokenDiacritics == eType ||
            ecTokenMeta == eType ||
            ecTokenTypeBack == eType)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

};    // struct StToken

struct StBreakChars
{
    const wchar_t * m_szDefault;
    StBreakChars() : m_szDefault (szDefaultBreakChars_) {}
};

struct StTabs
{
    const wchar_t * m_szDefault;
    StTabs() : m_szDefault (szDefaultTabs_) {}
};

struct StPunctuation
{
    const wchar_t * m_szDefault;
    StPunctuation() : m_szDefault (szDefaultPunctuation_) {}
};

struct StEscape
{
    const wchar_t * m_szDefault;
    StEscape() : m_szDefault (szDefaultEscapeChars_) {}
};

struct StVowels
{
    const wchar_t * m_szDefault;
    StVowels() : m_szDefault(g_szRusVowels) {}
};

struct StRegex
{
    const wchar_t * m_szDefault;
    StRegex() : m_szDefault (NULL) {}
};

template<typename SeparatorPolicy>
class CSeparators : public SeparatorPolicy
{
friend class CEString;

using SeparatorPolicy::m_szDefault;

public:
    CSeparators() : m_szVolatile (NULL), m_bDisabled (false)
    {}

    CSeparators (const CSeparators& S) : m_szVolatile (NULL), m_bDisabled(false)
    {
        Set (S.m_szVolatile);
    }

    ~CSeparators()
    {
        if (m_szVolatile)
        {
            delete[] m_szVolatile;
        }
    }

    void Set (const wchar_t * szSeparators)
    {
        if (!szSeparators)
        {
            return;
        }

        m_bDisabled = false;

        auto uiLength = wcslen (szSeparators);
        if (uiLength > cuiMaxSeparatorLength_)
        {
            const wchar_t * szMsg = L"Separator string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (0 == uiLength)
        {
            const wchar_t * szMsg = L"Empty separator string.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (m_szVolatile)
        {
            delete[] m_szVolatile;
        }

        m_szVolatile = new wchar_t[uiLength+1];
        m_szVolatile[uiLength] = L'\0';

#ifdef WIN32
        errno_t error = wmemcpy_s (m_szVolatile, uiLength+1, szSeparators, uiLength);
        if (error)
        {
            const wchar_t * szMsg = L"wmemcpy_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
#else
        auto pRet = wmemcpy(m_szVolatile, szSeparators, uiLength);
        if (NULL == pRet)
        {
            const wchar_t * szMsg = L"wmemcpy error.";
            ERROR_LOG(szMsg);
            throw CException(H_EXCEPTION, szMsg);
        }
#endif
    
    }   //  void Set (...)

    const wchar_t * szGet() const
    {
        if (m_bDisabled)
        {
            return NULL;
        }
        return m_szVolatile ? m_szVolatile : m_szDefault;
    }

    unsigned int uiLength() const
    {
        if (m_bDisabled)
        {
            return 0;
        }
        return m_szVolatile ? (unsigned int)wcslen (m_szVolatile) : (unsigned int)wcslen (m_szDefault);
    }

    bool bDisabled()
    {
        return m_bDisabled;
    }

private:
    wchar_t * m_szVolatile;
    bool m_bDisabled;

};      // CSeparators : public SeparatorPolicy

class CEString
{

private:

    wchar_t * m_szData;

    unsigned int m_uiLength;
    unsigned int m_uiBlocksAllocated;
  
    CSeparators<StBreakChars> m_Breaks;
    CSeparators<StTabs> m_Tabs;
    CSeparators<StPunctuation> m_Punctuation;
    CSeparators<StEscape> m_Escape;
    CSeparators<StVowels> m_Vowels;
    CSeparators<StRegex> m_Regex;

    vector<StToken> m_vecTokens;
    vector<StToken> m_vecRegexMatches;

//    json11::Json m_JsonParser;

    bool m_bInvalid;

public:

    CEString() : m_uiLength(0), m_uiBlocksAllocated (1), m_bInvalid (true)
    {
        m_szData = new wchar_t[uiBlockSize_];
        m_szData[0] = L'\0';
    }

    CEString (const CEString& Source) : 
        m_szData (NULL), 
        m_uiLength (Source.m_uiLength), 
        m_uiBlocksAllocated (Source.m_uiBlocksAllocated),
        m_Breaks (Source.m_Breaks),
        m_Tabs (Source.m_Tabs),
        m_Punctuation (Source.m_Punctuation),
        m_Escape (Source.m_Escape),
        m_Vowels (Source.m_Vowels),
        m_bInvalid (Source.m_bInvalid)
    {
        if (Source.m_uiLength >= cuiMaxSize_ || 
            Source.m_uiLength >= Source.m_uiBlocksAllocated * uiBlockSize_)
        {
            const wchar_t * szMsg = L"Source string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        m_szData = new wchar_t[m_uiBlocksAllocated * uiBlockSize_];

#ifdef WIN32
        errno_t error = wmemmove_s (m_szData, (m_uiBlocksAllocated*uiBlockSize_)-1, Source.m_szData, m_uiLength); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
#else
    auto pRet = wmemcpy(m_szData, Source.m_szData, m_uiLength);
    if (NULL == pRet)
    {
        const wchar_t * szMsg = L"wmemcpy error.";
        ERROR_LOG(szMsg);
        throw CException(H_EXCEPTION, szMsg);
    }
#endif

        m_szData[m_uiLength] = L'\0';

        if (!m_bInvalid)
        {
            m_vecTokens = Source.m_vecTokens;
        }

        m_vecRegexMatches = Source.m_vecRegexMatches;
    
    }   //  Copy ctor

    CEString (const wchar_t * szSource, 
              const wchar_t * szBreaks = NULL, 
              const wchar_t * szTabs = NULL, 
              const wchar_t * szPunctuation = NULL,
              const wchar_t * szEscape = NULL,
              const wchar_t * szVowels = NULL) : m_szData (NULL), m_uiBlocksAllocated (0)
    {
        m_uiLength = (unsigned int)wcslen (szSource);
        if (m_uiLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Source string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        Assign (szSource, m_uiLength);

        ResetSeparators();

        if (szBreaks)
        {
            m_Breaks.Set (szBreaks);
        }

        if (szTabs)
        {
            m_Tabs.Set (szTabs);
        }
        
        if (szPunctuation)
        {
            m_Punctuation.Set (szPunctuation);
        }
        
        if (szEscape)
        {
            m_Escape.Set (szEscape);
        }

        if (szVowels)
        {
            m_Vowels.Set (szVowels);
        }
    
    }   //  CEString (const wchar_t *)

    CEString (wchar_t chrValue) : m_uiLength(1), m_uiBlocksAllocated (1), m_bInvalid (true)
    {
        unsigned int uiSize = m_uiBlocksAllocated * uiBlockSize_;
        if (uiSize >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Requested size exceeds maximum allowed.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }
        m_szData = new wchar_t[m_uiBlocksAllocated * uiBlockSize_];
        m_szData[0] = chrValue;
        m_szData[1] = L'\0';
    
    }   //  CEString (wchar_t chrValue)

    virtual ~CEString()
    {
        delete[] m_szData;
        m_szData = NULL;
    }


public:

    // Operators

    operator wchar_t *() const
    {
        return m_szData;
    }

    CEString& operator= (const wchar_t * szRhs)
    {
        unsigned int uiSourceLength = (unsigned int)wcslen (szRhs);
        Assign (szRhs, uiSourceLength);

        return *this;
    
    }   //  operator= 

    CEString& operator= (const CEString& sRhs)
    {
        if (&sRhs == this)
        {
            return *this;
        }

//        m_uiBlocksAllocated = sRhs.m_uiBlocksAllocated;
//        m_uiLength = sRhs.m_uiLength;
        Assign (sRhs.m_szData, sRhs.m_uiLength);
        
        return *this;
    
    }   //  operator= 

    CEString& operator+= (const wchar_t * szRhs)
    {
        unsigned int  uiRhsLength = (unsigned int)wcslen(szRhs);
        unsigned int uiNewSize = uiRhsLength + m_uiLength;
        if (uiNewSize >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Right-hand side string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        Concatenate (szRhs, uiRhsLength);

        return *this;

    }   //  operator+=

    CEString& operator+= (const CEString& sRhs)
    {
        unsigned int uiNewSize = sRhs.m_uiLength + m_uiLength;
        if (uiNewSize >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Right-hand side string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        Concatenate (sRhs.m_szData, sRhs.m_uiLength);

        return *this;

    }   //  operator+=

    // Mutators
    void ResetSeparators()
    {
        m_Breaks.m_bDisabled = false;
        m_Tabs.m_bDisabled = true;
        m_Punctuation.m_bDisabled = true;
        m_Escape.m_bDisabled = true;
        m_Vowels.m_bDisabled = true;
    }

    void SetBreakChars (const wchar_t * szBreakChars)
    {
        m_Breaks.Set (szBreakChars);
        m_bInvalid = true;
    }

    void SetTabs (const wchar_t * szTabs)
    {
        m_Tabs.Set (szTabs);
        m_bInvalid = true;
    }

    void SetPunctuation (const wchar_t * szPunctuation)
    {
        m_Punctuation.Set (szPunctuation);
        m_bInvalid = true;
    }
    
    void SetEscapeChars (const wchar_t * szEscapeChars)
    {
        m_Escape.Set (szEscapeChars);
        m_bInvalid = true;
    }

    void SetVowels (const wchar_t * szVowels)
    {
        m_Vowels.Set (szVowels);
        m_bInvalid = true;
    }

    // State/accessors
    bool bIsEmpty() const
    {
        return (0 == m_uiLength);
    }

    unsigned int uiLength() const
    {
        return m_uiLength;
    }

    unsigned char * pToBytes() const
    {
        return (unsigned char *)m_szData;
    }

    // Comparison
    static ERelation eCompare (const wchar_t * szLeft, const wchar_t * szRight)
    {
        int iRet = wcscmp (szLeft, szRight);
        if (0 == iRet)
        {
            return ecEqual;
        }

        return (iRet > 0) ? ecGreater : ecLess;

    }       // eCompare (...)

    static ERelation eCompare (const CEString& sLhs, const wchar_t * szRhs)
    {
        return eCompare (sLhs.m_szData, szRhs);

    }       // eCompare (...)

    static ERelation eCompare (const wchar_t * szLhs, const CEString& sRhs)
    {
        return eCompare (szLhs, sRhs.m_szData);

    }       // eCompare (...)

    static ERelation eCompare (const CEString& sLhs, const CEString& sRhs)
    {
        return eCompare (sLhs.m_szData, sRhs.m_szData);

    }       // eCompare (...)

    static ERelation eCompareNoCase (const wchar_t * szLeft, const wchar_t * szRight)
    {
#ifdef WIN32
        const wchar_t * szLocale = _wsetlocale (LC_ALL, L"");

        int iRet = _wcsicmp (szLeft, szRight);
        if (0 == iRet)
        {
            return ecEqual;
        }

        return (iRet > 0) ? ecGreater : ecLess;
#else
        setlocale(LC_ALL, "");
        long unsigned int uiLength = min(wcslen(szLeft), wcslen(szRight));
        if (uiLength > cuiMaxSize_)
        {
            uiLength = cuiMaxSize_ - 1;
        }

        long unsigned int uiChr = 0;
        for (; uiChr < uiLength; ++uiChr)
        {
            if (towupper(szLeft[uiChr]) != towupper(szRight[uiChr]))
            {
                break;
            }
        }

        if (uiChr == uiLength)
        {
            return ecEqual;
        }

        if (szLeft[uiChr] < szRight[uiChr])
        {
            return ecLess;
        }

        return ecGreater;

#endif
    }       // eCompare (...)

    // Search
    static bool bIn (const wchar_t chr, const wchar_t * szSearchSet) 
    {
        if (!szSearchSet)
        {
            return false;
        }

        auto uiSearchSetLength = wcslen (szSearchSet);
        if (uiSearchSetLength > cuiMaxSearchSetLength_)
        {
            const wchar_t * szMsg = L"Search set too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        for (unsigned int uiAt = 0; uiAt < cuiMaxSearchSetLength_; ++uiAt)
        {
            if (chr == szSearchSet[uiAt])
            {
                return true;
            }

            if (L'\0' == szSearchSet[uiAt])
            {
                return false;
            }
        }

        const wchar_t * szMsg = L"Search set too long.";
        ERROR_LOG (szMsg);
        throw CException (H_ERROR_INVALID_ARG, szMsg);
    
    }   //  bIn

    unsigned int uiFind (const wchar_t * szRhs, unsigned int uiStartAt = 0) const
    {
        if (0 == m_uiLength)
        {
            return ecNotFound;
        }

        auto uiLength = wcslen (szRhs);
        if (uiStartAt >= m_uiLength)
        {
            const wchar_t * szMsg = L"Search string start pos out of range.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (uiLength > m_uiLength - uiStartAt)
        {
            return ecNotFound;
        }

        if (uiLength > cuiMaxSearchStringLength_)
        {
            const wchar_t * szMsg = L"Search string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        const wchar_t * pPos = wcsstr (&m_szData[uiStartAt], szRhs);
        if (pPos)
        {
            return static_cast<unsigned int>(pPos - m_szData);
        }

        return ecNotFound;

    }   //  uiFind (...)

    unsigned int uiFindNoCase (const wchar_t * szRhs) const
    {
        if (0 == m_uiLength)
        {
            return ecNotFound;
        }

        auto uiRhsLength = wcslen (szRhs);
        if (uiRhsLength > m_uiLength)
        {
            return ecNotFound;
        }

        if (uiRhsLength >= cuiMaxSearchStringLength_)
        {
            const wchar_t * szMsg = L"Right-hand side string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

#ifdef WIN32
        const wchar_t * szLocale = _wsetlocale(LC_ALL, L"");
#else
        setlocale(LC_ALL, "");
#endif

        wchar_t szRhsCopy[cuiMaxSearchStringLength_];

#ifdef WIN32
        errno_t error = wmemmove_s(szRhsCopy, cuiMaxSearchStringLength_-1, szRhs, uiRhsLength); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s failed.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
        szRhsCopy[uiRhsLength] = L'\0';
        error = _wcslwr_s(szRhsCopy, cuiMaxSearchStringLength_);
        if (error)
        {
            const wchar_t * szMsg = L"_wcslwr_s failed.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
#else
        wmemmove(szRhsCopy, szRhs, uiRhsLength);
        szRhsCopy[uiRhsLength] = L'\0';
        for (int iChr; iChr < (int)uiRhsLength; ++iChr)
        {
            szRhsCopy[iChr] = towlower(szRhs[iChr]);
        }

#endif        
      
        wchar_t szLhsCopy[m_uiLength+1];

#ifdef WIN32
        error = wmemmove_s(szLhsCopy, m_uiLength, m_szData, m_uiLength);
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s failed.";
            ERROR_LOG(szMsg);
            throw CException(error, szMsg);
        }
        szLhsCopy[m_uiLength] = L'\0';

        error = _wcslwr_s(szLhsCopy, m_uiLength+1);
        if (error)
        {
            const wchar_t * szMsg = L"_wcslwr_s failed.";
            ERROR_LOG(szMsg);
            throw CException(error, szMsg);
        }
#else
        for (int iChr; iChr < (int)m_uiLength; ++iChr)
        {
            szLhsCopy[iChr] = towlower(m_szData[iChr]);
        }
        szLhsCopy[m_uiLength] = L'\0';
#endif

        const wchar_t * pPos = wcsstr(szLhsCopy, szRhsCopy);
        if (pPos)
        {
            return static_cast<unsigned int>(pPos - szLhsCopy);
        }
        else
        {
            return ecNotFound;
        }

    }   //  uiFindNoCase (...)

    unsigned int uiRFind (const wchar_t * szRhs) const
    {
        if (0 == m_uiLength)
        {
            ERROR_LOG (L"*** Warning: empty string");
            return ecNotFound;
        }

        auto uiRhsLength = wcslen (szRhs);
        if (uiRhsLength > m_uiLength)
        {
            return ecNotFound;
        }

        for (auto iAt = m_uiLength - uiRhsLength; iAt > 0; --iAt)
        {
            if (0 == wmemcmp (&m_szData[iAt], szRhs, uiRhsLength))
            {
                return (unsigned int)iAt;
            }
        }

        return ecNotFound;
    
    }   //  uiRFind

    unsigned int uiFindFirstOf (const wchar_t * szSet) const
    {
        size_t uiAt = wcscspn (m_szData, szSet);

        if (m_uiLength == uiAt)
        {
            return ecNotFound;
        }

        return (unsigned int)uiAt;
    
    }   //  uiFindFirstOf (...)

    unsigned int uiFindOneOf (unsigned int uiStartAt, const wchar_t * szSet) const
    {
        if (uiStartAt >= m_uiLength)
        {
            const wchar_t * szMsg = L"Search starts past end of string.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        size_t uiAt = wcscspn (&m_szData[uiStartAt], szSet) + uiStartAt;

        if (m_uiLength == uiAt)
        {
            return ecNotFound;
        }

        return (unsigned int)uiAt;
    
    }   //  uiFindFirstOf (...)

    unsigned int uiFindLastOf (const wchar_t * szSet) const
    {
        for (int iAt = m_uiLength-1; iAt >= 0; --iAt)
        {
            if (bIn (m_szData[iAt], szSet))
            {
                return (unsigned int)iAt;
            }
        }

        return ecNotFound;
    
    }   //  uiFindLastOf (...)

    bool bStartsWith (const wchar_t * szRhs) const
    {
        if (0 == m_uiLength)
        {
            return false;
        }

        auto uiRhsLength = wcslen (szRhs);
        if (uiRhsLength > uiRhsLength)
        {
            return false;
        }
        
        int iRet = wcsncmp (m_szData, szRhs, uiRhsLength);

        return (0 == iRet);
    
    }   // bStartsWith (...)

    bool bStartsWithNoCase (const wchar_t * szRhs) const
    {
        if (0 == m_uiLength)
        {
            return false;
        }

        auto uiRhsLength = wcslen (szRhs);
        if (m_uiLength < uiRhsLength)
        {
            return false;
        }
        
#ifdef WIN32
        const wchar_t * szLocale = _wsetlocale(LC_ALL, L"");

        int i_ret = _wcsnicmp (m_szData, szRhs, uiRhsLength);
        return (0 == i_ret);
#else
        setlocale(LC_ALL, "");

        long unsigned int uiChr = 0;
        for (; uiChr < uiRhsLength; ++uiChr)
        {
            if (towupper(m_szData[uiChr]) != towupper(szRhs[uiChr]))
            {
                break;
            }
        }

        if (uiChr == uiRhsLength)
        {
            return true;
        }

        return false;
#endif
    
    }   //  bStartsWithNoCase (...)

    bool bStartsWithOneOf (const wchar_t * szRhs) const
    {
        if (0 == m_uiLength)
        {
            return false;
        }

        return bIn (m_szData[0], szRhs);
    }

    bool bStartsWithOneOfNoCase (const wchar_t * szRhs) const
    {
        if (0 == m_uiLength)
        {
            return false;
        }

        wchar_t cLhs = m_szData[0];
        cLhs = towlower (cLhs);

        auto uiRhsLength = wcslen (szRhs);
        wchar_t * szRhsLowerCase = new wchar_t[uiRhsLength+1];

#ifdef WIN32
        wcscpy_s (szRhsLowerCase, uiRhsLength+1, szRhs);
#else
        wcscpy(szRhsLowerCase, szRhs);
#endif

        for (unsigned int uiAt = 0; uiAt < uiRhsLength; ++uiAt)
        {
            szRhsLowerCase[uiAt] = towlower (szRhs[uiAt]);
        }
        auto iAt = wcscspn (&cLhs, szRhsLowerCase);

        delete[] szRhsLowerCase;

        return (0 == iAt);
    
    }   //  bEndsWithOneOfNoCase (...)

    bool bEndsWith (const wchar_t * szRhs) const
    {
        if (0 == m_uiLength)
        {
            return false;
        }

        auto uiRhsLength = wcslen (szRhs);
        if (uiRhsLength > uiRhsLength)
        {
            return false;
        }
        
        int iRet = wcsncmp (&m_szData[m_uiLength-uiRhsLength], szRhs, uiRhsLength);
        
        return (0 == iRet);
    
    }   //  bEndsWith (...)

    bool bEndsWithNoCase (const wchar_t * szRhs) const
    {
        if (0 == m_uiLength)
        {
            return false;
        }

        auto uiRhsLength = wcslen (szRhs);
        if (uiRhsLength > m_uiLength)
        {
            return false;
        }
        
#ifdef WIN32
        int iRet = _wcsnicmp (&m_szData[m_uiLength-uiRhsLength], szRhs, uiRhsLength);        
        return (0 == iRet);
#else
        auto uiLChr = m_uiLength - uiRhsLength;
        unsigned int uiRChr = 0;
        for (; uiLChr < m_uiLength; ++uiLChr, ++uiRChr)
        {
            if (towupper(m_szData[uiLChr]) != towupper(szRhs[uiRChr]))
            {
                break;
            }
        }

        return uiRChr == uiRhsLength;
#endif    
    }   //  bEndsWithNoCase (...)

    bool bEndsWithOneOf (const wchar_t * szRhs) const
    {
        if (0 == m_uiLength)
        {
            return false;
        }

        return bIn (m_szData[m_uiLength-1], szRhs);
    }

    bool bEndsWithOneOfNoCase (const wchar_t * szRhs) const
    {
        if (0 == m_uiLength)
        {
            return false;
        }

        wchar_t cLhs = m_szData[m_uiLength-1];
        cLhs = towlower (cLhs);

        auto uiRhsLength = wcslen (szRhs);
        wchar_t * szRhsLowerCase = new wchar_t[uiRhsLength+1];

#ifdef WIN32
        wcscpy_s(szRhsLowerCase, uiRhsLength + 1, szRhs);
#else
        wcscpy(szRhsLowerCase, szRhs);
#endif

        for (unsigned int uiAt = 0; uiAt < uiRhsLength; ++uiAt)
        {
            szRhsLowerCase[uiAt] = towlower (szRhs[uiAt]);
        }
        auto iAt = wcscspn (&cLhs, szRhsLowerCase);

        delete[] szRhsLowerCase;

        return (0 == iAt);
    
    }   //  bEndsWithOneOfNoCase (...)

    // Manipulations
    CEString& sInsert (unsigned int uiInsertAt, const wchar_t chrInsert)
    {
        if (uiInsertAt > m_uiLength)
        {
            const wchar_t * szMsg = L"Bad insersion pos.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (L'\0' == chrInsert)
        {
            const wchar_t * szMsg = L"Bad insertion char.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        unsigned int uiNewLength = m_uiLength + 1;
        if (uiNewLength >= m_uiBlocksAllocated * uiBlockSize_)
        {
            wchar_t szInsert[2];
            szInsert[0] = chrInsert;
            szInsert[1] = L'\0';
            sInsert (uiInsertAt, szInsert);

            return *this;
        }

        for (unsigned int uiAt = m_uiLength+1; uiAt > uiInsertAt; --uiAt)
        {
            m_szData[uiAt] = m_szData[uiAt-1];
        }

        m_szData[uiInsertAt] = chrInsert;
        ++m_uiLength;
//        m_szData[++m_uiLength] = L'\0';

        return *this;

    }   // CEString& sInsert (...)

    CEString& sInsert (unsigned int uiInsertAt, const wchar_t * szInsert)
    {
        unsigned int uiCharsToInsert = static_cast<unsigned int>(wcslen (szInsert));
        if (uiInsertAt > m_uiLength)
        {
            const wchar_t * szMsg = L"Bad insersion pos.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (0 == uiCharsToInsert)
        {
            ERROR_LOG (L"*** Warning: attempt to insert empty string");
            return *this;
        }

        auto uiNewLength = m_uiLength + uiCharsToInsert;
        if (uiNewLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Insertions string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (uiNewLength >= m_uiBlocksAllocated * uiBlockSize_)
        {
            m_uiBlocksAllocated = (unsigned int)((uiNewLength+1)/uiBlockSize_) + 1;
        }

        auto uiNewAllocSize = m_uiBlocksAllocated * uiBlockSize_;
        wchar_t * szNewData = new wchar_t[uiNewAllocSize];

#ifdef WIN32
        errno_t error = wmemmove_s (szNewData, uiNewAllocSize-1, m_szData, uiInsertAt); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s failed.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }

        error = wmemmove_s (&szNewData[uiInsertAt], 
                            uiNewAllocSize-uiInsertAt-1, 
                            szInsert, 
                            uiCharsToInsert); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s failed.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }

        auto uiPastInsertion = uiInsertAt + uiCharsToInsert;
        error = wmemmove_s (&szNewData[uiPastInsertion], 
                            uiNewAllocSize-uiPastInsertion-1, 
                            &m_szData[uiInsertAt], 
                            m_uiLength - uiInsertAt); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s failed.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
#else
        wmemmove(szNewData, m_szData, uiInsertAt);
        wmemmove(&szNewData[uiInsertAt], szInsert, uiCharsToInsert);
        auto uiPastInsertion = uiInsertAt + uiCharsToInsert;
        wmemmove(&szNewData[uiPastInsertion], &m_szData[uiInsertAt], m_uiLength - uiInsertAt);
#endif

        m_uiLength += uiCharsToInsert;
        szNewData[m_uiLength] = L'\0';

        delete[] m_szData;
        m_szData = szNewData;

        return *this;

    }   //  CEString& sInsert (...)

    CEString& sReplace (unsigned int uiStartAt, wchar_t * szReplace)
    {
        auto uiInsertLength = wcslen (szReplace);
        if (uiStartAt + uiInsertLength > m_uiLength)
        {
            const wchar_t * szMsg = L"Illegal length.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

#ifdef WIN32
        int iBufSize = m_uiBlocksAllocated * uiBlockSize_ - uiStartAt;
        errno_t error = wmemcpy_s (&m_szData[uiStartAt], iBufSize, szReplace, uiInsertLength);
        if (error)
        {
            const wchar_t * szMsg = L"_memccpy failed.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
#else
        wmemcpy(&m_szData[uiStartAt], szReplace, uiInsertLength);
#endif

        return *this;
    
    }   //  CEString& sReplace (unsigned int uiStartAt, wchar_t * szReplace)

    CEString& sReplace (unsigned int uiAt, wchar_t chrReplace)
    {
        if (uiAt >= m_uiLength)
        {
            const wchar_t * szMsg = L"Illegal position.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        m_szData[uiAt] = chrReplace;

        return *this;

    }   //  CEString& sReplace (unsigned int uiStartAt, wchar_t chrReplace)

    CEString& sReplace (unsigned int uiAt, unsigned int uiMaxCharsToErase, wchar_t * szReplace)
    {
        if (uiAt >= m_uiLength)
        {
            const wchar_t * szMsg = L"Illegal insertion point.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        auto uiCharsToInsert = wcslen (szReplace);
        auto uiCharsToErase = min (uiMaxCharsToErase, m_uiLength - uiAt);
        auto uiNewLength = m_uiLength - uiCharsToErase + uiCharsToInsert;
        if (uiNewLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Replacement string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        auto uiBlocksToAllocate = ((uiNewLength+1)/uiBlockSize_) + 1;
        auto uiNewSize = uiBlocksToAllocate * uiBlockSize_;
        wchar_t * szNewData = new wchar_t[uiNewSize];

#ifdef WIN32
        errno_t error = wmemmove_s (szNewData, uiNewSize-1, m_szData, uiAt); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }

        error = wmemmove_s (&szNewData[uiAt], uiNewSize-uiAt-1, szReplace, uiCharsToInsert); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
        
        if (m_uiLength > uiAt+uiCharsToErase)
        {
            unsigned int uiRemainder = m_uiLength - (uiAt+uiCharsToErase) + 1;
            error = wmemmove_s (&szNewData[uiAt+uiCharsToInsert], 
                                uiNewSize-uiAt-uiCharsToInsert-1, 
                                &m_szData[uiAt+uiCharsToErase], 
                                uiRemainder); 
            if (error)
            {
                const wchar_t * szMsg = L"wmemmove_s error.";
                ERROR_LOG (szMsg);
                throw CException (error, szMsg);
            }
        }
#else
        wmemmove(szNewData, m_szData, uiAt);
        wmemmove(&szNewData[uiAt], szReplace, uiCharsToInsert);

        if (m_uiLength > uiAt + uiCharsToErase)
        {
            unsigned int uiRemainder = m_uiLength - (uiAt + uiCharsToErase) + 1;
            wmemmove(&szNewData[uiAt + uiCharsToInsert], &m_szData[uiAt + uiCharsToErase], uiRemainder);
        }
#endif

        delete[] m_szData;
        m_szData = szNewData;
        m_uiBlocksAllocated = static_cast<unsigned int>(uiBlocksToAllocate);
        m_uiLength = static_cast<unsigned int>(m_uiLength - uiCharsToErase + uiCharsToInsert);
        m_szData[m_uiLength] = L'\0';

        return *this;

    }   //  CEString& sReplace (unsigned int uiStartAt, wchar_t chrReplace)

    void Replace (unsigned int uiStartPos, unsigned int uiHowMany, wchar_t cWhat, wchar_t cWithWhat)
    {
        if (0 == m_uiLength)
        {
            ERROR_LOG (L"*** Warning: empty string.");
            return;
        }

        if (uiStartPos >= m_uiLength)
        {
            const wchar_t * szMsg = L"Start position beyond string end.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (uiStartPos + uiHowMany > m_uiLength)
        {
            ASSERT(0);
            ERROR_LOG (L"*** Warning: attempt to replace beyond string end.");
            uiHowMany = m_uiLength - uiStartPos;
        }

        const wchar_t * pcSearchAt = &m_szData[uiStartPos];
        wchar_t * pcReplaceAt = NULL;
        do
        {
            pcReplaceAt = wcschr (&m_szData[uiStartPos], cWhat);
            if (pcReplaceAt)
            {
                *pcReplaceAt = cWithWhat;
                if (L'\0' == *(++pcReplaceAt))
                {
                    pcSearchAt = NULL;
                }
                else
                {
                    pcSearchAt = pcReplaceAt;
                }
            }

        } while (pcReplaceAt);
    
    }   //  void Replace (...)

    CEString sReplace(wchar_t * szOld, wchar_t * szNew)
    {
        if (0 == m_uiLength)
        {
            ERROR_LOG(L"*** Warning: empty string.");
            return L"";
        }

        unsigned int uiAt = uiFind(szOld);
        while (uiAt != ecNotFound)
        {
            sReplace(uiAt, (unsigned int)wcslen(szOld), szNew);
            uiAt = uiFind(szOld);
        }

        return *this;

    }   //  CEString sReplace (...)

    void Erase()
    {
        if (m_uiLength >= (m_uiBlocksAllocated * uiBlockSize_))
        {
            const wchar_t * szMsg = L"Illegal length.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (L'\0' != m_szData[m_uiLength])
        {
            const wchar_t * szMsg = L"Malformed string.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (0 == m_uiLength)
        {
            return;
        }

        m_szData[0] = L'\0';
        m_uiLength = 0;

        Shrink();

    }   //  Erase()

    CEString& sErase (unsigned int uiFirst, unsigned int uiHowMany)
    {
        if (uiFirst >= m_uiLength)
        {
            const wchar_t * szMsg = L"Bad offset.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        //if (uiFirst + uiHowMany >= m_uiLength)
        //{
        //    wchar_t * szMsg = L"Bad length.";
        //    ERROR_LOG (szMsg);
        //    throw CException (H_ERROR_INVALID_ARG, szMsg);
        //}

        uiHowMany = min (uiHowMany, m_uiLength - uiFirst);

        unsigned int uiMoveFrom = uiFirst+uiHowMany;

#ifdef WIN32
        int iBufSize = (m_uiBlocksAllocated * uiBlockSize_) - uiFirst - 1;
        errno_t error = wmemmove_s(&m_szData[uiFirst], iBufSize, &m_szData[uiMoveFrom], m_uiLength - uiMoveFrom);
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG(szMsg);
            throw CException(error, szMsg);
        }
#else
        auto pRet = wmemmove(&m_szData[uiFirst], &m_szData[uiMoveFrom], m_uiLength - uiMoveFrom);
        if (NULL == pRet)
        {
            const wchar_t * szMsg = L"wmemcpy error.";
            ERROR_LOG(szMsg);
            throw CException(H_EXCEPTION, szMsg);
        }
#endif

        m_uiLength -= uiHowMany;
        m_szData[m_uiLength] = L'\0';

        Shrink();

        return *this;
        
    }   //  CEString& sErase (unsigned int uiFirst, unsigned int uiHowMany)

    CEString& sErase (unsigned int uiAt)
    {
        if (uiAt >= m_uiLength)
        {
            const wchar_t * szMsg = L"Bad offset.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        m_uiLength = uiAt;
        m_szData[m_uiLength] = L'\0';

        Shrink();

        return *this;
    
    }   //  CEString& sErase (...)

    CEString& sRemoveCharsFromEnd(unsigned int uiCharsToRemove)
    {
        if (uiCharsToRemove >= m_uiLength)
        {
            const wchar_t * szMsg = L"Bad offset.";
            ERROR_LOG(szMsg);
            throw CException(H_ERROR_INVALID_ARG, szMsg);
        }

        m_uiLength = m_uiLength - uiCharsToRemove;
        m_szData[m_uiLength] = L'\0';

        Shrink();

        return *this;

    }   //  CEString& sRemoveCharsFromEnd (...)
        
    // Case
    void ToLower()
    {
#ifdef WIN32
        const wchar_t * szLocale = _wsetlocale (LC_ALL, L"");

        errno_t error = _wcslwr_s (m_szData, m_uiLength + 1);
        if (error)
        {
            const wchar_t * szMsg = L"_wcslwr_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
#else
        setlocale(LC_ALL, "");

        for (unsigned int uiChr = 0; uiChr < m_uiLength; ++uiChr)
        {
            m_szData[uiChr] = towlower(m_szData[uiChr]);
        }
#endif
    }       //  ToLower()

    static CEString sToLower (const wchar_t * szSource)
    {   
#ifdef WIN32
        const wchar_t * szLocale = _wsetlocale (LC_ALL, L"");

        auto uiRhsLength = wcslen (szSource);
        if (uiRhsLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Argument string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        wchar_t * szCopy = new wchar_t[uiRhsLength+1];
        szCopy[uiRhsLength] = L'\0';

        errno_t error = wmemmove_s (szCopy, uiRhsLength, szSource, uiRhsLength); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }

        error = _wcslwr_s (szCopy, uiRhsLength + 1);
        if (error)
        {
            const wchar_t * szMsg = L"_wcslwr_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }

        return CEString(szCopy);
#else
        setlocale(LC_ALL, "");

        auto uiRhsLength = wcslen(szSource);
        if (uiRhsLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Argument string too long.";
            ERROR_LOG(szMsg);
            throw CException(H_ERROR_INVALID_ARG, szMsg);
        }

        wchar_t * szCopy = new wchar_t[uiRhsLength + 1];
        szCopy[uiRhsLength] = L'\0';

        auto pRet = wmemmove(szCopy, szSource, uiRhsLength);
        if (NULL == pRet)
        {
            const wchar_t * szMsg = L"wmemmove error.";
            ERROR_LOG(szMsg);
            throw CException(H_ERROR_POINTER, szMsg);
        }

        for (unsigned int uiChr = 0; uiChr < uiRhsLength; ++uiChr)
        {
            szCopy[uiChr] = towlower(szCopy[uiChr]);
        }

        return CEString(szCopy);
#endif
    }   //  sToLower (const wchar_t * szSource)

    static CEString sToLower (const CEString& sSource)
    {
#ifdef WIN32
        const wchar_t * szLocale = _wsetlocale (LC_ALL, L"");
#else
        setlocale(LC_ALL, "");
#endif
        CEString sCopy (sSource);
        sCopy.ToLower();

        return sCopy;
    }

    void ToUpper()
    {
#ifdef WIN32
        const wchar_t * szLocale = _wsetlocale (LC_ALL, L"");
        
        errno_t error = _wcsupr_s (m_szData, m_uiLength + 1);
        if (error)
        {
            const wchar_t * szMsg = L"_wcsupr_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
#else
        setlocale(LC_ALL, "");

        for (unsigned int uiChr = 0; uiChr < m_uiLength; ++uiChr)
        {
            m_szData[uiChr] = towupper(m_szData[uiChr]);
        }
#endif
    }       //  ToUpper()
    
    static CEString sToUpper (const wchar_t * szSource)
    {
#ifdef WIN32
        const wchar_t * szLocale = _wsetlocale (LC_ALL, L"");
#else
        setlocale(LC_ALL, "");
#endif

        auto uiRhsLength = wcslen (szSource);
        if (uiRhsLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Argument string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        wchar_t * szCopy = new wchar_t[uiRhsLength+1];
        szCopy[uiRhsLength] = L'\0';

#ifdef WIN32
        errno_t error = wmemmove_s (szCopy, uiRhsLength, szSource, uiRhsLength); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }

        error = _wcsupr_s (szCopy, uiRhsLength + 1);
        if (error)
        {
            const wchar_t * szMsg = L"_wcslwr_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
#else
        auto pRet = wmemmove(szCopy, szSource, uiRhsLength);
        if (NULL == pRet)
        {
            const wchar_t * szMsg = L"wmemmove error.";
            ERROR_LOG(szMsg);
            throw CException(H_ERROR_POINTER, szMsg);
        }

        for (unsigned int uiChr = 0; uiChr < uiRhsLength; ++uiChr)
        {
            szCopy[uiChr] = towupper(szCopy[uiChr]);
        }
#endif

        return CEString(szCopy);
    
    }   //  static CEstring sToUpper (const wchar_t * szSource)

    static CEString sToUpper (const CEString& sSource)
    {
#ifdef WIN32
        const wchar_t * szLocale = _wsetlocale (LC_ALL, L"");
#else
        setlocale(LC_ALL, "");
#endif
        CEString sCopy (sSource);
        sCopy.ToUpper();

        return sCopy;
    }

    CEString sSubstr (unsigned int uiOffset, unsigned int uiLength = cuiMaxSize_) const
    {
        if (uiLength > cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Source string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (0 == uiLength)
        {
            ERROR_LOG (L"*** Warning: empty string");
            return CEString (L"");
        }

        if (L'\0' != m_szData[m_uiLength])
        {
            const wchar_t * szMsg = L"Source string not null-terminated.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (uiOffset == m_uiLength)
        {
            ERROR_LOG (L"*** Warning: empty substring");
            return CEString (L"");
        }

        if (uiOffset > m_uiLength)
        {
            const wchar_t * szMsg = L"Invalid offset.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (uiLength < cuiMaxSize_)
        {
            if (uiOffset + uiLength > m_uiLength)
            {
                const wchar_t * szMsg = L"Invalid length.";
                ERROR_LOG (szMsg);
                throw CException (H_ERROR_UNEXPECTED, szMsg);
            }
        }

        CEString sResult;
        unsigned int uiCharsToMove = min (uiLength, m_uiLength - uiOffset);
        ASSERT(uiCharsToMove <= m_uiLength);
        wchar_t chrSave = m_szData[uiOffset + uiCharsToMove];
        m_szData[uiOffset + uiCharsToMove] = L'\0';
        sResult.Assign (&m_szData[uiOffset], uiCharsToMove);
        m_szData[uiOffset + uiCharsToMove] = chrSave;
        return sResult;

    }   //  CEString sSubstr (...)

    // Trim
    void TrimLeft (const wchar_t * szCharsToTrim)
    {
        if (0 == m_uiLength)
        {
            return;
        }

        if (m_uiLength >= (m_uiBlocksAllocated * uiBlockSize_))
        {
            const wchar_t * szMsg = L"Illegal length.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (L'\0' != m_szData[m_uiLength])
        {
            const wchar_t * szMsg = L"Malformed string.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        unsigned int uiAt = 0;
        for (; uiAt < m_uiLength; ++uiAt)
        {
            if (L'\0' == m_szData[uiAt])
            {
                const wchar_t * szMsg = L"Malformed string.";
                ERROR_LOG (szMsg);
                throw CException (H_ERROR_UNEXPECTED, szMsg);
            }

            if (!bIn (m_szData[uiAt], szCharsToTrim))
            {
                break;
            }
        }

        if (0 == uiAt)
        {
            return;
        }

        if (uiAt >= m_uiLength)
        {
            m_szData[0] = L'\0';
            m_uiLength = 0;
            Shrink();
            return;
        }

        m_uiLength -= uiAt;

#ifdef WIN32
        errno_t error = wmemmove_s (m_szData, (m_uiBlocksAllocated*uiBlockSize_)-1, &m_szData[uiAt], m_uiLength); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
#else
        auto pRet = wmemmove(m_szData, &m_szData[uiAt], m_uiLength);
#endif
        m_szData[m_uiLength] = L'\0';
    
    }       // TrimLeft (...)

    void TrimLeft()
    {
        TrimLeft (m_Breaks.szGet());
    }

    void TrimRight (const wchar_t * szCharsToTrim)
    {
        if (0 == m_uiLength)
        {
            return;
        }

        if (m_uiLength >= m_uiBlocksAllocated * uiBlockSize_)
        {
            const wchar_t * szMsg = L"Illegal length.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (L'\0' != m_szData[m_uiLength])
        {
            const wchar_t * szMsg = L"Malformed string.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        int iAt = (int)m_uiLength-1;
        for (; iAt >= 0; --iAt)
        {
            if (L'\0' == m_szData[iAt])
            {
                const wchar_t * szMsg = L"Malformed string.";
                ERROR_LOG (szMsg);
                throw CException (H_ERROR_UNEXPECTED, szMsg);
            }

            if (!bIn (m_szData[iAt], szCharsToTrim))
            {
                break;
            }
        }

        if (iAt == (int)m_uiLength-1)
        {
            return;
        }

        m_uiLength = (int)iAt+1;
        m_szData[m_uiLength] = L'\0';
        m_bInvalid = true;
        
        Shrink();

    }   //  void TrimRight()

    void TrimRight()
    {
        TrimRight (m_Breaks.szGet());
    }

    void Trim (const wchar_t * szCharsToTrim)
    {
        TrimLeft (szCharsToTrim);
        TrimRight (szCharsToTrim);
    }

    void Trim()
    {
        TrimLeft (m_Breaks.szGet());
        TrimRight (m_Breaks.szGet());
    }

    void Reverse()
    {
#ifdef WIN32
        _wcsrev (m_szData);
#else
        wchar_t chrTmp = 0;
        for (unsigned int uiStart = 0, uiEnd = m_uiLength-1; uiStart <= uiEnd; ++uiStart, --uiEnd)
        {
            chrTmp = m_szData[uiStart];
            m_szData[uiStart] = m_szData[uiEnd];
            m_szData[uiEnd] = chrTmp;
        }
#endif
    }


    // Tokens & fields
    CEString sGetField (int iAt, ETokenType eType = ecTokenText)
    {
//        Tokenize();
        vector<StToken>& rvecTokens = (ecTokenRegexMatch == eType) ? m_vecRegexMatches : m_vecTokens;
        vector<StToken>::iterator itToken = itFindToken (iAt, eType);
        if (rvecTokens.end() == itToken)
        {
            const wchar_t * szMsg = L"Failed to find token.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        return sSubstr ((*itToken).uiOffset, (*itToken).uiLength);
    
    }   // sGetField (...)

    StToken stGetField (int iAt, ETokenType eType = ecTokenText)
    {
//        Tokenize();
        vector<StToken>& rvecTokens = (ecTokenRegexMatch == eType) ? m_vecRegexMatches : m_vecTokens;
        vector<StToken>::iterator itToken = itFindToken (iAt, eType);
        if (rvecTokens.end() == itToken)
        {
            const wchar_t * szMsg = L"Failed to find token.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        return *itToken;
    
    }   //  stGetField (...)

//    ST_Token st_GetFieldFromOffset (int i_offset,
//                                    et_TokenType eo_type = ec_TokenText);

    StToken stGetTokenFromOffset (int iOffset, ETokenType eType = ecTokenText)
    {
//        Tokenize();
        vector<StToken>& rvecTokens = (ecTokenRegexMatch == eType) ? m_vecRegexMatches : m_vecTokens;
        vector<StToken>::iterator itToken = itTokenFromOffset (iOffset, eType);
        if (rvecTokens.end() == itToken)
        {
            const wchar_t * szMsg = L"Failed to find token.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }
        return (*itToken);
    }

    ETokenType eGetTokenType (unsigned int uiAt)
    {
        Tokenize();

        if (uiAt >= m_vecTokens.size())
        {
            const wchar_t * szMsg = L"Token index out of range.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        return m_vecTokens[uiAt].eType;
    
    }   //  eGetTokenType (...)

//    et_TokenType eo_GetTokenType (int i_offset, int i_at);

    StToken stGetToken (unsigned int uiAt)
    {
        Tokenize();

        if (uiAt >= m_vecTokens.size())
        {
            const wchar_t * szMsg = L"Token index out of range.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        return m_vecTokens[uiAt];
    
    }   //  stGetToken (...)

    const StToken& rstGetToken (unsigned int uiAt)
    {
        Tokenize();

        if (uiAt >= m_vecTokens.size())
        {
            const wchar_t * szMsg = L"Token index out of range.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        return m_vecTokens[uiAt];
    
    }   //  const StToken& rstGetToken (...)

    CEString sGetToken (unsigned int uiAt)
    {
        Tokenize();

        if (uiAt >= m_vecTokens.size())
        {
            const wchar_t * szMsg = L"Token index out of range.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        StToken stToken = m_vecTokens[uiAt];
        return CEString (sSubstr (stToken.uiOffset, stToken.uiLength));
    
    }   //  CEString sGetToken (...)

    bool bGetNextToken (StToken& stToken)
    {
        Tokenize();

        vector<StToken>::iterator it_ = find (m_vecTokens.begin(), m_vecTokens.end(), stToken);
        if (m_vecTokens.end() == it_)
        {
            wchar_t * szMsg = L"Token not found.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        ++it_;
        if (m_vecTokens.end() == it_)
        {
            return false;
        }
        else
        {
            stToken = *it_;
        }

        return true;

    }    //  bool bGetNextToken (...)

    bool bGetPrevToken (StToken& stToken)
    {
        Tokenize();

        vector<StToken>::iterator it_ = find (m_vecTokens.begin(), m_vecTokens.end(), stToken);
        if (m_vecTokens.end() == it_)
        {
            const wchar_t * szMsg = L"Token not found.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        if (m_vecTokens.begin() == it_)
        {
            return false;
        }

        --it_;
        stToken = *it_;

        return true;

    }    //  bool bGetPrevToken (...)

    unsigned int uiGetTokenNum (const StToken& stToken)
    {
        Tokenize();

        vector<StToken>::iterator it_ = find (m_vecTokens.begin(), m_vecTokens.end(), stToken);
        if (it_ == m_vecTokens.end())
        {
            const wchar_t * szMsg = L"Token not found.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        return static_cast<unsigned int>(it_ - m_vecTokens.begin());
    
    }   //  unsigned int uiGetTokenNum (...)

    unsigned int uiGetTokenOffset (unsigned int uiAt)
    {
        Tokenize();

        if (uiAt >= m_vecTokens.size())
        {
            const wchar_t * szMsg = L"m_vecTokens member index out of range";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        return m_vecTokens[uiAt].uiOffset;
    }

    unsigned int uiGetRegexOffset (unsigned int uiAt) const
    {
        if (uiAt >= m_vecRegexMatches.size())
        {
            const wchar_t * szMsg = L"m_vecRegexMatches member index out of range";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        return m_vecRegexMatches[uiAt].uiOffset;
    }

    unsigned int uiGetFieldOffset (unsigned int uiAt, ETokenType eType = ecTokenText)
    {
//        Tokenize();
     
        //if (uiAt >= m_vecTokens.size())
        //{
        //    wchar_t * szMsg = L"m_vecTokens member index out of range";
        //    ERROR_LOG (szMsg);
        //    throw CException (H_ERROR_INVALID_ARG, szMsg);
        //}
        
        vector<StToken>& rvecTokens = (ecTokenRegexMatch == eType) ? m_vecRegexMatches : m_vecTokens;
        vector<StToken>::iterator itToken = itFindToken (uiAt, eType);
        if (rvecTokens.end() == itToken)
        {
            const wchar_t * szMsg = L"Failed to find token.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        return (*itToken).uiOffset;
    
    }   //  uiGetFieldOffset (...)

    CEString sGetRegexMatch (unsigned int iAt)
    {
        //if (wcslen (m_szRegex) < 1)
        //{
        //    ERROR_LOG (L"Warning: Empty regex string");
        //    return CEString (L"");
        //}

        if (m_Regex.m_bDisabled)
        {
            ERROR_LOG (L"Warning: regex string missing or invalid");
            return CEString (L"");
        }

        if (iAt >= m_vecRegexMatches.size())
        {
            ERROR_LOG (L"*** m_vecRegexMatches member index out of range");
            return CEString (L"");

//            const wchar_t * szMsg = L"m_vecRegexMatches member index out of range";
//            ERROR_LOG (szMsg);
//            throw CException (H_ERROR_GENERAL, szMsg);
        }

        StToken stToken = m_vecRegexMatches[iAt];
        return sSubstr (stToken.uiOffset, stToken.uiLength);

    }   //  sGetRegexMatch (...)

    unsigned int uiGetNumOfFields (ETokenType eType = ecTokenText)
    {
        if (ecTokenRegexMatch != eType)
        {
            Tokenize();
        }

        int iTokens = 0;
        vector<StToken>& rvecTokens = (ecTokenRegexMatch == eType) ? m_vecRegexMatches : m_vecTokens;
        vector<StToken>::iterator it_ = rvecTokens.begin();
        for (; it_ != rvecTokens.end(); ++it_)
        {
            if ((*it_).eType == eType)
            {
                ++iTokens;
            }
        }
        
        return iTokens;
    
    }   //  uiGetNumOfFields (...)

/*
    unsigned int uiGetNumOfFields (unsigned int uiOffset, 
                                   unsigned int uiLength,
                                   ETokenType eType = ecTokenText)
    {
        Tokenize();

        vector<StToken>::iterator it_ = m_vecTokens.begin();
        for (; it_ < m_vecTokens.end(); ++it_)
        {
            if ((*it_).uiOffset >= uiOffset)
            {
                break;
            }
        }
        
        int iTokens = 0;
        for (; it_ != m_vecTokens.end(); ++it_)
        {
            if ((*it_).uiOffset >= uiOffset + uiLength)
            {
                break;
            }

            if ((*it_).eType == eType)
            {
                ++iTokens;
            }
        }
        
        if (m_vecTokens.end() == it_)
        {
            wstring sMsg (L"Token index out of range.");
            ERROR_LOG (sMsg);
            throw CException (H_ERROR_GENERAL, sMsg);
        }

        return iTokens;
    
    }   //  unsigned int uiGetNumOfFields (unsigned int uiOffset, ...)
*/

    unsigned int uiNFields (ETokenType eType = ecTokenText)
    {
        return uiGetNumOfFields (eType);
    }

//    unsigned int uiNFields (unsigned int uiOffset, 
//                            unsigned int uiLength, 
//                            ETokenType eType = ecTokenText)
//    {
//        return uiGetNumOfFields (uiOffset, uiLength, eType);
//    }

    unsigned int uiGetNumOfTokens()
    {
        Tokenize();
        return static_cast<unsigned int>(m_vecTokens.size());
    }

    unsigned int uiNTokens()
    {
        Tokenize();
        return static_cast<unsigned int>(m_vecTokens.size());
    }

    unsigned int uiGetNumOfRegexMatches()
    {
        return static_cast<unsigned int>(m_vecRegexMatches.size());
    }

    unsigned int uiNRegexMatches()
    {
        return static_cast<unsigned int>(m_vecRegexMatches.size());
    }

    unsigned int uiGetVisibleLength()
    {
        Tokenize();

        unsigned int uiVlength = 0;
        vector<StToken>::iterator it_ = m_vecTokens.begin();
        for (; it_ != m_vecTokens.end(); ++it_)    
        {
            if ((*it_).bIsLinearText())
            {
                uiVlength += (*it_).uiLength;
            }
        }
        return uiVlength;
    
    }   //  uiGetVisibleLength()

    unsigned int uiGetFieldLength (unsigned int uiAt, ETokenType eType = ecTokenText)
    {
        if (ecTokenRegexMatch != eType)
        {
            Tokenize();
        }

        vector<StToken>& rvecTokens = (ecTokenRegexMatch == eType) ? m_vecRegexMatches : m_vecTokens;
        if (uiAt >= rvecTokens.size())
        {
            const wchar_t * szMsg = L"Token index out of range.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        vector<StToken>::iterator itToken = itFindToken (uiAt, eType);
        if (rvecTokens.end() == itToken)
        {
            const wchar_t * szMsg = L"Failed to find token.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }
/*
        vector<StToken>::iterator it = rvecTokens.begin();
        for (; it != rvecTokens.end(); ++it)
        {
            if ((*it).eType == eType)
            {
                if (distance (rvecTokens.begin(), it) >= (int)uiAt)
                {
                    break;
                }
            }
        }
*/        
        if (rvecTokens.end() == itToken)
        {
            const wchar_t * szMsg = L"Token not found.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        return (*itToken).uiLength;

    }   //  uiGetFieldLength (...)

    //unsigned int uiGetFieldLength (unsigned int uiOffset, 
    //                               unsigned int uiAt,
    //                               ETokenType eType = ecTokenText)
    //{
    //}

    unsigned int uiGetNumOfSyllables() const
    {
        if (m_Vowels.uiLength() == 0)
        {
            ASSERT(0);
            const wchar_t * szMsg = L"Vowels not defined";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        unsigned int uiSyllables = 0;
        unsigned int uiAt = 0;
        do
        {
            unsigned int uiStartAt = uiAt;
            uiAt = (unsigned int)wcscspn (&m_szData[uiAt], m_Vowels.szGet());
            uiAt += uiStartAt;
            if (uiAt < m_uiLength)
            {
                ++uiSyllables;
            }

        } while (++uiAt < m_uiLength);

        return uiSyllables;

    }   //  uiGetNumOfSyllables()

    int uiNSyllables() const
    {
        return uiGetNumOfSyllables();
    }

    // Vowels & consonants
    bool bIsConsonant(wchar_t chr)
    {}

    bool bIsVowel(wchar_t chr)
    {}

    unsigned int uiGetVowelPos (unsigned int uiVowel = 0) const
    {
        if (m_Vowels.uiLength() == 0)
        {
            ASSERT(0);
            const wchar_t * szMsg = L"Vowels not defined";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        unsigned int uiAt = 0;
        bool bFound = false;
        for (unsigned int ui_ = 0; ui_ <= uiVowel; ++ui_)
        {
            unsigned int uiStartAt = uiAt;
            uiAt = (unsigned int)wcscspn (&m_szData[uiAt], m_Vowels.szGet());
            uiAt += uiStartAt;
            if (uiAt >= m_uiLength)
            {
                break;
            }
            if (uiVowel == ui_)
            {
                return uiAt;
            }
            ++uiAt;
        }

        ASSERT(0);
        const wchar_t * szMsg = L"Vowel position not found";
        ERROR_LOG (szMsg);
        throw CException (H_ERROR_GENERAL, szMsg);

        return 0;
    
    }   //  uiGetVowelPos (...)
    
    unsigned int uiGetSyllableFromVowelPos (unsigned int uiAbsPos) const
    {
        if (m_Vowels.uiLength() == 0)
        {
            ASSERT(0);
            const wchar_t * szMsg = L"Vowels not defined";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        if (m_uiLength <= uiAbsPos)
        {
            ASSERT(0);
            const wchar_t * szMsg = L"Vowel position invalid";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        unsigned int uiSyll = 0;
        unsigned int uiVowelPos = 0;
        do
        {
            unsigned int uiStartAt = uiVowelPos;
            uiVowelPos = (unsigned int)wcscspn (&m_szData[uiVowelPos], m_Vowels.szGet());
            uiVowelPos += uiStartAt;
            if (uiVowelPos < m_uiLength)
            {
                if (uiVowelPos == uiAbsPos)
                {
                    return uiSyll;
                }

                ++uiSyll;

                if (++uiVowelPos > uiAbsPos)
                {
                    ASSERT(0);
                    const wchar_t * szMsg = L"Vowel position not found";
                    ERROR_LOG (szMsg);
                    throw CException (H_ERROR_GENERAL, szMsg);
                    return 0;
                }
            }

        } while (uiVowelPos < m_uiLength);

        ASSERT(0);
        const wchar_t * szMsg = L"Vowel position not found";
        ERROR_LOG (szMsg);
        throw CException (H_ERROR_GENERAL, szMsg);
        return 0;

    }   //  uiGetSyllableFromVowelPos (...)


    // Regex
    bool bRegexMatch (const wchar_t * szRegex)
    {
        return bRegexEvaluate (szRegex, true);
    }

    bool bRegexSearch (const wchar_t * szRegex)
    {
        return bRegexEvaluate (szRegex, false);
    }


// TODO RegexReplace?
 
    //
    // Unicode conversions
    //
    string stl_sToUtf8()
    {
        wchar_t arrSource[cuiMaxSize_ + 1];
        int iLength = (int)min(m_uiLength, cuiMaxSize_);

#ifdef WIN32
        errno_t error = wmemmove_s(arrSource, cuiMaxSize_, m_szData, iLength);
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s failed.";
            ERROR_LOG(szMsg);
            throw CException(H_ERROR_GENERAL, szMsg);
        }
#else
        auto pRet = wmemmove(arrSource, m_szData, iLength);
        if (NULL == pRet)
        {
            const wchar_t * szMsg = L"wmemmove failed.";
            ERROR_LOG(szMsg);
            throw CException(H_ERROR_POINTER, szMsg);
        }
#endif
        arrSource[iLength] = L'\0';
        wstring_convert<codecvt_utf8<wchar_t>> converter;
        const string utf8_string = converter.to_bytes(arrSource);

        return utf8_string;
    }

    static CEString sFromUtf8(const string& stl_sSource)
    {
        wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(stl_sSource).c_str();
    }
 
    /*
    //
    // Json parsing
    //
    bool bParseJsonString()
    {
        string stl_sError;
        m_JsonParser = json11::Json::parse(stl_sToUtf8(), stl_sError);
        if (stl_sError.length() > 0)
        {
            wstring wsMsg(L"Json parsing error: ");
            wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            wsMsg += converter.from_bytes(stl_sError).c_str();
            ERROR_LOG(wsMsg.c_str());
            return false;
        }
        return true;
    }

    bool bGetJsonStringValue(CEString& sKey, CEString& sValue)
    {
        if (m_JsonParser.is_null())
        {
            const wchar_t * szMsg = L"Error: Json object is null.";
            ERROR_LOG(szMsg);
            return false;
        }

        if (m_JsonParser[sKey.stl_sToUtf8()].is_string())
        {
            string stl_sValue = m_JsonParser[sKey.stl_sToUtf8()].string_value();
            sValue = CEString(sFromUtf8(stl_sValue));
            return true;
        }
        else
        {
            const wchar_t * szMsg = L"Error: not a string value.";
            ERROR_LOG(szMsg);
            return false;
        }
    }

    bool bGetJsonNumericValue(CEString& sKey, double& dValue)
    {
        if (m_JsonParser.is_null())
        {
            const wchar_t * szMsg = L"Error: Json object is null.";
            ERROR_LOG(szMsg);
            return false;
        }

        if (m_JsonParser[sKey.stl_sToUtf8()].is_number())
        {
            dValue = m_JsonParser[sKey.stl_sToUtf8()].number_value();
            return true;
        }
        else
        {
            const wchar_t * szMsg = L"Error: not a numeric value.";
            ERROR_LOG(szMsg);
            return false;
        }
    }
    
    bool bGetJsonBooleanValue(CEString& sKey, bool& bValue)
    {
        if (m_JsonParser.is_null())
        {
            const wchar_t * szMsg = L"Error: Json object is null.";
            ERROR_LOG(szMsg);
            return false;
        }

        if (m_JsonParser[sKey.stl_sToUtf8()].is_bool())
        {
            bValue = m_JsonParser[sKey.stl_sToUtf8()].bool_value();
            return true;
        }
        else
        {
            const wchar_t * szMsg = L"Error: not a Boolean value.";
            ERROR_LOG(szMsg);
            return false;
        }
    }
    */
////////////////////////////////////////////////////////////////////////////////////////////


private:

//    void Null()
//    {
//        m_szRegex[0] = L'\0';
//          m_szVowels[0] = L'\0';
//        m_szBreakChars[0] = L'\0';
//        m_szTabs[0] = L'\0';
//        m_szPunctuation[0] = L'\0';
//        m_szEscapeChars[0] = L'\0';
//    }

    void Grow (unsigned int uiCharsToAdd)
    {
        unsigned int uiNewLength = m_uiLength + uiCharsToAdd;
        unsigned int uiNewBlocks = (uiNewLength+1)/uiBlockSize_;
        ASSERT(uiNewBlocks >= m_uiBlocksAllocated);
        if (uiNewBlocks == m_uiBlocksAllocated)
        {
            return;
        }
        wchar_t * szNewBuffer = new wchar_t[uiNewBlocks * uiBlockSize_];
        if (m_uiLength > 0)
        {
#ifdef WIN32
            errno_t error = wmemmove_s (szNewBuffer, (uiNewBlocks*uiBlockSize_)-1, m_szData, m_uiLength); 
            if (error)
            {
                const wchar_t * szMsg = L"wmemmove_s failed.";
                ERROR_LOG (szMsg);
                throw CException (error, szMsg);
            }
#else
            auto pRet = wmemmove(szNewBuffer, m_szData, m_uiLength);
            if (NULL == pRet)
            {
                const wchar_t * szMsg = L"wmemmove failed.";
                ERROR_LOG(szMsg);
                throw CException(H_ERROR_POINTER, szMsg);
            }

#endif
            szNewBuffer[m_uiLength] = L'\0';
        }
        delete[] m_szData;
        m_szData = szNewBuffer;
    
    }   //  Grow()

    void Shrink()
    {
        unsigned int uiBlocksToFree = ((m_uiBlocksAllocated * uiBlockSize_) - (m_uiLength + 1)) / uiBlockSize_;
        if (uiBlocksToFree < 1)
        {
            return;
        }

        m_uiBlocksAllocated -= uiBlocksToFree;
        m_uiBlocksAllocated = max (m_uiBlocksAllocated, 1u);
        wchar_t * szNewBuffer = new wchar_t[m_uiBlocksAllocated * uiBlockSize_];

#ifdef WIN32
        errno_t error = wmemmove_s (szNewBuffer, (m_uiBlocksAllocated*uiBlockSize_)-1, m_szData, m_uiLength); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s failed.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
#else
        auto pRet = wmemmove(szNewBuffer, m_szData, m_uiLength);
        if (NULL == pRet)
        {
            const wchar_t * szMsg = L"wmemmove failed.";
            ERROR_LOG(szMsg);
            throw CException(H_ERROR_POINTER, szMsg);
        }
#endif

        szNewBuffer[m_uiLength] = L'\0';
        delete[] m_szData;
        m_szData = szNewBuffer;
    
    }   //  Shrink()

    void Concatenate (const wchar_t * szRhs, unsigned int uiRhsLength)
    {
        unsigned int uiNewLength = m_uiLength + uiRhsLength;
        if (uiNewLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Right-hand side string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (uiNewLength >= m_uiBlocksAllocated * uiBlockSize_)
        {
            m_uiBlocksAllocated = ((uiNewLength+1)/uiBlockSize_) + 1;
            unsigned int uiAllocSize = m_uiBlocksAllocated * uiBlockSize_;
            wchar_t * szNewData = new wchar_t[uiAllocSize];

#ifdef WIN32
            errno_t error = wmemmove_s (szNewData, uiAllocSize-1, m_szData, m_uiLength); 
            if (error)
            {
                const wchar_t * szMsg = L"wmemmove_s error.";
                ERROR_LOG (szMsg);
                throw CException (error, szMsg);
            }
#else
            auto pRet = wmemmove(szNewData, m_szData, m_uiLength);
            if (NULL == pRet)
            {
                const wchar_t * szMsg = L"wmemmove error.";
                ERROR_LOG(szMsg);
                throw CException(H_ERROR_POINTER, szMsg);
            }
#endif
            delete[] m_szData;
            m_szData = szNewData;
        }

        int iAllocSize = m_uiBlocksAllocated * uiBlockSize_;

#ifdef WIN32
        errno_t error = wmemmove_s (&m_szData[m_uiLength], iAllocSize-m_uiLength-1, szRhs, uiRhsLength); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
#else
        auto pRet = wmemmove(&m_szData[m_uiLength], szRhs, uiRhsLength);
        if (NULL == pRet)
        {
            const wchar_t * szMsg = L"wmemmove error.";
            ERROR_LOG(szMsg);
            throw CException(H_ERROR_POINTER, szMsg);
        }
#endif

        m_uiLength = uiNewLength;
        m_szData[m_uiLength] = L'\0';

    }   //  void Concatenate (...)

    void Tokenize()
    {
        if (!m_bInvalid)
        {
            return;
        }

        m_bInvalid = false;
        m_vecTokens.clear();

        StToken stToken;
        for (unsigned int uiAt = 0; uiAt < m_uiBlocksAllocated * uiBlockSize_; ++uiAt)
        {
            wchar_t chrCurrent = m_szData[uiAt];

            if (L'\0' == chrCurrent)
            {
                if (uiAt != m_uiLength)
                {
                    m_bInvalid = true;
                    const wchar_t * szMsg = L"Unexpected NULL character.";
                    ERROR_LOG (szMsg);
                    throw CException (H_ERROR_UNEXPECTED, szMsg);
                }

                if (0 == uiAt)
                {
                    return;
                }

                if (ecTokenTypeFront == stToken.eType)
                {
                    m_bInvalid = true;
                    const wchar_t * szMsg = L"Illegal token type.";
                    ERROR_LOG (szMsg);
                    throw CException (H_ERROR_UNEXPECTED, szMsg);
                }

                m_vecTokens.push_back (stToken);
                return;
            }

            if (bIn (chrCurrent, m_Breaks.szGet()))
            {
                Advance (ecTokenBreakChars, uiAt, stToken);
                continue;
            }

            if (bIn (chrCurrent, m_Tabs.szGet()))
            {
                Advance (ecTokenTab, uiAt, stToken);
                continue;
            }

            if (bIn (chrCurrent, m_Punctuation.szGet()))
            {
                Advance (ecTokenPunctuation, uiAt, stToken);
                continue;
            }

            if (bIn (chrCurrent, m_Escape.szGet()))
            {

                ASSERT(m_uiLength > 0);
                if (uiAt >= m_uiLength-1)
                {
                    m_bInvalid = true;
                    const wchar_t * szMsg = L"Unexpected escape character.";
                    ERROR_LOG (szMsg);
                    throw CException (H_ERROR_UNEXPECTED, szMsg);
                }

                bool bDoubleEscape = false;
                if (chrCurrent == m_szData[uiAt+1])
                {
                    bDoubleEscape = true;
                }
                else if (uiAt > 0 && (chrCurrent == m_szData[uiAt-1]))
                {
                    bDoubleEscape = true;
                }

                if (!bDoubleEscape)
                {
                    AddTag (uiAt, stToken);
                    continue;
                }

            }   //  for (int uiAt = 0; ...)

            Advance (ecTokenText, uiAt, stToken);
            
        }   //  for ...

        m_bInvalid = true;
        const wchar_t * szMsg = L"Tokenizer failed.";
        ERROR_LOG (szMsg);
        throw CException (H_ERROR_UNEXPECTED, szMsg);

    }   //  Tokenize_ (...)

    void Advance (ETokenType eType, unsigned int uiOffset, StToken& stToken)
    {
        if (eType <= ecTokenTypeFront || eType > ecTokenTypeBack)
        {
            const wchar_t * szMsg = L"Unexpected token state.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (eType != stToken.eType)
        {
            if (ecTokenTypeFront != stToken.eType)
            {
                m_vecTokens.push_back (stToken);
            }

            stToken.eType = eType;
            stToken.uiOffset = uiOffset;
            stToken.uiLength = 1;
        }
        else
        {
            ++stToken.uiLength;
        }
    
    }   // void Advance (...)

    void AddTag (unsigned int uiOffset, StToken& stToken)
    {
        if (ecTokenMeta == stToken.eType || uiOffset < 0)
        {
            const wchar_t * szMsg = L"Unexpected token state.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (ecTokenTypeFront != stToken.eType)
        {
            m_vecTokens.push_back (stToken);
        }

        stToken.eType = ecTokenMeta;
        stToken.uiOffset = uiOffset;
        stToken.uiLength = 0;

        for (int iAt = uiOffset + 1; iAt < (int)(m_uiBlocksAllocated * uiBlockSize_); ++iAt)
        {
            if (L'\0' == m_szData[iAt])
            {
                const wchar_t * szMsg = L"Unterminated escape sequence.";
                ERROR_LOG (szMsg);
                throw CException (H_ERROR_UNEXPECTED, szMsg);
            }

            if (bIn (m_szData[iAt], m_Escape.szGet()))
            {
                if (uiOffset+1 == iAt)
                {
                    const wchar_t * szMsg = L"Empty escape sequence.";
                    ERROR_LOG (szMsg);
                    throw CException (H_ERROR_UNEXPECTED, szMsg);
                }
                stToken.uiLength = iAt - uiOffset + 1;
                m_vecTokens.push_back (stToken);
                break;
            }
        }
    }       //  AddTag (...)

    // Assign null-terminated string erasing current contents
    // uiSourceLength does not count final '0'
    void Assign (const wchar_t * szSource, unsigned int uiSourceLength)
    {
        if (uiSourceLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Source string too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (L'\0' != szSource[uiSourceLength])
        {
            const wchar_t * szMsg = L"Source string not null-terminated.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (uiSourceLength >= (m_uiBlocksAllocated * uiBlockSize_))
        {
            m_uiBlocksAllocated = ((uiSourceLength+1)/uiBlockSize_) + 1;
            if (m_szData)
            {
                delete[] m_szData;
            }
            m_szData = new wchar_t[uiBlockSize_ * m_uiBlocksAllocated];
        }

#ifdef WIN32
        errno_t error = wmemmove_s (m_szData, (uiBlockSize_*m_uiBlocksAllocated)-1, szSource, uiSourceLength); 
        if (error)
        {
            const wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
#else
        auto pRet = wmemmove(m_szData, szSource, uiSourceLength);
        if (NULL == pRet)
        {
            const wchar_t * szMsg = L"wmemmove error.";
            ERROR_LOG(szMsg);
            throw CException(H_ERROR_POINTER, szMsg);
        }
#endif

        m_szData[uiSourceLength] = L'\0';
        m_uiLength = uiSourceLength;
    
    }   //  Assign (...)

    vector<StToken>::iterator itFindToken (unsigned int uiAt, ETokenType eType)
    {
        vector<StToken>& rvecTokens = (ecTokenRegexMatch == eType) ? m_vecRegexMatches : m_vecTokens;
        if (ecTokenRegexMatch == eType)
        {
//            if ((0 == wcslen (m_szRegex)) || m_vecRegexMatches.empty())
            if (m_Regex.bDisabled() || m_vecRegexMatches.empty())
            {
                ERROR_LOG (L"*** Warning: no regex matches.");
                return rvecTokens.end();
            }
        }
        else
        {
            Tokenize();
        }

        if (uiAt >= rvecTokens.size())
        {
            const wchar_t * szMsg = L"Token position out of range.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        unsigned int uiField = 0;
        vector<StToken>::iterator it_ = rvecTokens.begin();
        for (; it_ != rvecTokens.end(); ++it_)
        {
            if (eType == (*it_).eType)
            {
                if (uiAt == uiField)
                {
                    break;
                }
                ++uiField;
            }
        }

        if (rvecTokens.end() == it_)
        {
            const wchar_t * szMsg = L"Token position out of range.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        return it_;

    }   //  itFindToken (...)

    vector<StToken>::iterator itTokenFromOffset (unsigned int uiOffset, ETokenType eType)
    {
        vector<StToken>& rvecTokens = (ecTokenRegexMatch == eType) ? m_vecRegexMatches : m_vecTokens;
        if (ecTokenRegexMatch == eType)
        {
//            if ((0 == wcslen (m_szRegex)) || m_vecRegexMatches.empty())
            if (m_Regex.bDisabled() || m_vecRegexMatches.empty())
            {
                ERROR_LOG (L"*** Warning: no regex matches.");
                return rvecTokens.end();
            }
        }
        else
        {
            Tokenize();
        }

        vector<StToken>::iterator it_ = rvecTokens.begin();
        for (; it_ != rvecTokens.end(); ++it_)
        {
            if ((*it_).uiOffset > uiOffset)
            {
                break;
            }
        }

        if (rvecTokens.begin() == it_)
        {
            const wchar_t * szMsg = L"Failed to find token.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        return --it_;

    }   //  itTokenFromOffset (...)

    bool bRegexEvaluate (const wchar_t * szRegex, bool bMatchMode = true)
    {
        if (0 == m_uiLength)
        {
            return false;
        }

        auto uiRegexLength = wcslen (szRegex);
        if (uiRegexLength >= cuiMaxRegexLength_)
        {
            const wchar_t * szMsg = L"Regular expression too long.";
            ERROR_LOG (szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        m_Regex.Set (szRegex);
/*
        errno_t error = wmemmove_s (m_szRegex, cuiMaxRegexLength_-1, szRegex, uiRegexLength); 
        if (error)
        {
            wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }

        m_szRegex[uiRegexLength] = L'\0';
*/
        bool bRet = false;

        try
        {
            wregex regex_ (m_Regex.szGet());
            wsmatch match_;
            wstring wstrData (m_szData);
            if (bMatchMode)
            {
                bRet = regex_match (wstrData, match_, regex_);
            }
            else
            {
                bRet = regex_search (wstrData, match_, regex_);
            }

            if (match_[0].matched)
            {
            m_vecRegexMatches.clear();
            for (unsigned int uiAt = 1; uiAt < match_.size(); ++uiAt)
            {
                StToken stToken;
                stToken.eType = ecTokenRegexMatch;
                if (match_[uiAt].length() > 0)
                {
                    stToken.uiOffset = static_cast <int> (match_.position(uiAt));
                }
                stToken.uiLength = static_cast<unsigned int>(match_.length(uiAt));
                m_vecRegexMatches.push_back (stToken);
            }
            }
        }
        catch (regex_error rxError_)
        {
            wstring str_msg (L"Regex error: ");
            str_msg += str_RegexError_ (rxError_.code());
            ERROR_LOG (str_msg.c_str());
// TODO: rethrow as native exc?
        }    

        return bRet;

    }   //  RegexEvaluate (...)

    wstring str_RegexError_ (regex_constants::error_type eo_errCode)
    {
        wstring str_error;
        switch (eo_errCode)
        {
            case regex_constants::error_badbrace:
            {
                str_error = L"error_badbrace -- ";
                str_error += L"the expression contained an invalid count in a { } expression";
                break;
            }
            case regex_constants::error_badrepeat:
            {
                str_error = L"error_badrepeat -- ";
                str_error += L"a repeat expression (one of '*', '?', '+', '{' ";
                str_error += L"in most contexts) was not preceded by an expression";
                break;
            }
            case regex_constants::error_brace:
            {
                str_error = L"error_brace -- the expression contained an unmatched '{' or '}'";
                break;
            }
            case regex_constants::error_brack:
            {
                str_error = L"error_brack -- the expression contained an unmatched '[' or ']'"; 
                break;
            }
            case regex_constants::error_collate:
            {
                str_error = L"error_collate -- ";
                str_error += L"the expression contained an invalid collating element name";
                break;
            }
            case regex_constants::error_complexity:
            {
                str_error = L"error_complexity -- an attempted match failed because it was too complex";
                break;
            }
            case regex_constants::error_ctype:
            {
                str_error = L"error_ctype -- the expression contained an invalid character class name";
                break;
            }
            case regex_constants::error_escape:
            {
                str_error = L"error_escape -- the expression contained an invalid escape sequence";
                break;
            }
            case regex_constants::error_paren:
            {
                str_error = L"error_paren -- the expression contained an unmatched '(' or ')'";
                break;
            }
            case regex_constants::error_range:
            {
                str_error = L"error_range -- ";
                str_error += L"the expression contained an invalid character range specifier";
                break;
            }
            case regex_constants::error_space:
            {
                str_error = L"error_space -- ";
                str_error += L"parsing a regular expression failed because there were not enough ";
                str_error += L"resources available";
                break;
            }
            case regex_constants::error_stack:
            {
                str_error = L"error_stack -- ";
                str_error += L"an attempted match failed because there was not enough memory available";
                break;
            }
            case regex_constants::error_backref:
            {
                str_error = L"error_backref -- the expression contained an invalid back reference";
                break;
            }

#ifdef WIN32
            case regex_constants::error_parse:
            {
                str_error = L"error_parse";
                break;
            }
            case regex_constants::error_syntax:
            {
                str_error = L"error_syntax";
                break;
            }
#endif

            default:
            {
                str_error = L"Unknown error";
            }
        }
            return str_error;

    }   //  str_RegexError_ (...)

friend const CEString operator+ (const CEString& sLhs, const wchar_t * szRhs)
{
    return CEString (sLhs) += szRhs;
}

friend const CEString operator+ (const wchar_t * szLhs, const CEString& sRhs)
{
    return CEString (szLhs) += sRhs;
}

friend const CEString operator+ (const CEString& sLhs, const CEString& sRhs)
{
    return CEString (sLhs) += sRhs;
}

friend bool operator== (const CEString& sLhs, const wchar_t * szRhs)
{
    return ecEqual == CEString::eCompare (sLhs, szRhs);
}

friend bool operator== (const wchar_t * szLhs, const CEString& sRhs)
{
    return ecEqual == CEString::eCompare (szLhs, sRhs);
}

friend bool operator== (const CEString& sLhs, const CEString& sRhs)
{
    return ecEqual == CEString::eCompare (sLhs, sRhs);
}

friend bool operator!= (const CEString& sLhs, const wchar_t * szRhs)
{return ecEqual != CEString::eCompare (sLhs, szRhs);
}

friend bool operator!= (const wchar_t * szLhs, const CEString& sRhs)
{
    return ecEqual != CEString::eCompare (szLhs, sRhs);
}

friend bool operator!= (const CEString& sLhs, const CEString& sRhs)
{
    return ecEqual != CEString::eCompare (sLhs, sRhs);
}

friend bool operator< (const CEString& sLhs, const wchar_t * szRhs)
{
    return ecLess == CEString::eCompare (sLhs, szRhs);
}

friend bool operator< (const wchar_t * szLhs, const CEString& sRhs)
{
    return ecLess == CEString::eCompare (szLhs, sRhs);
}

friend bool operator< (const CEString& sLhs, const CEString& sRhs)
{
    return ecLess == CEString::eCompare (sLhs, sRhs);
}

friend bool operator> (const CEString& sLhs, const wchar_t * szRhs)
{
    return ecGreater == CEString::eCompare (sLhs, szRhs);
}

friend bool operator> (const wchar_t * szLhs, const CEString& sRhs)
{
    return ecGreater == CEString::eCompare (szLhs, sRhs);
}

friend bool operator> (const CEString& sLhs, const CEString& sRhs)
{
    return ecGreater == CEString::eCompare (sLhs, sRhs);
}

friend bool operator<= (const CEString& sLhs, const wchar_t * szRhs)
{
    ERelation eRet = CEString::eCompare (sLhs, szRhs);
    return (ecLess == eRet || ecEqual == eRet);
}

friend bool operator<= (const wchar_t * szLhs, const CEString& sRhs)
{
    ERelation eRet = CEString::eCompare (szLhs, sRhs);
    return (ecLess == eRet || ecEqual == eRet);
}

friend bool operator<= (const CEString& sLhs, const CEString& sRhs)
{
    ERelation eRet = CEString::eCompare (sLhs, sRhs);
    return (ecLess == eRet || ecEqual == eRet);
}

friend bool operator>= (const CEString& sLhs, const wchar_t * szRhs)
{
    ERelation eRet = CEString::eCompare (sLhs, szRhs);
    return (ecGreater == eRet || ecEqual == eRet);
}

friend bool operator>= (const wchar_t * szLhs, const CEString& sRhs)
{
    ERelation eRet = CEString::eCompare (szLhs, sRhs);
    return (ecGreater == eRet || ecEqual == eRet);
}

friend bool operator>= (const CEString& sLhs, const CEString& sRhs)
{
    ERelation eRet = CEString::eCompare (sLhs, sRhs);
    return (ecGreater == eRet || ecEqual == eRet);
}


};   //  class  CEString

     // Conversions
template<typename T>
CEString sToString(T tSource)
{
#ifdef WIN32
    wchar_t arrStrValue[cuiMaxConversionLength_];
    errno_t iError = _itow_s(tSource, arrStrValue, cuiMaxConversionLength_, 10);
    if (iError)
    {
        const wchar_t * szMsg = L"Conversion error.";
        ERROR_LOG(szMsg);
        throw CException(iError, szMsg);
    }
    return CEString(arrStrValue);
#else
    return CEString(to_wstring(tSource).c_str());
#endif
}

template<>
CEString sToString(int64_t llSource)
{
#ifdef WIN32
    wchar_t arrStrValue[cuiMaxConversionLength_];
    errno_t iError = _i64tow_s(llSource, arrStrValue, cuiMaxConversionLength_, 10);
    if (iError)
    {
        const wchar_t * szMsg = L"Conversion error.";
        ERROR_LOG(szMsg);
        throw CException(iError, szMsg);
    }
    return CEString(arrStrValue);
#else
    return CEString(to_wstring(llSource).c_str());
#endif
}

template<>
CEString sToString(uint64_t ullSource)
{
#ifdef WIN32
    wchar_t arrStrValue[cuiMaxConversionLength_];
    errno_t iError = _ui64tow_s(ullSource, arrStrValue, cuiMaxConversionLength_, 10);
    if (iError)
    {
        const wchar_t * szMsg = L"Conversion error.";
        ERROR_LOG(szMsg);
        throw CException(iError, szMsg);
    }
    return CEString(arrStrValue);
#else
    return CEString(to_wstring(ullSource).c_str());
#endif
}

template<>
CEString sToString(double dSource)
{
#ifdef WIN32
    char arrStrValueMultibyte[cuiMaxConversionLength_];
    wchar_t arrStrValueUtf16[cuiMaxConversionLength_];

    errno_t iError = _gcvt_s((char *)arrStrValueMultibyte, sizeof(arrStrValueMultibyte), dSource, 10);
    if (iError)
    {
        const wchar_t * szMsg = L"Conversion error.";
        ERROR_LOG(szMsg);
        throw CException(iError, szMsg);
    }

    auto iLength = strlen(arrStrValueMultibyte);
    if (iLength <= 0)
    {
        const wchar_t * szMsg = L"Conversion error: empty string.";
        ERROR_LOG(szMsg);
        throw CException(iError, szMsg);
    }

    iLength = min(iLength, cuiMaxConversionLength_);

    for (unsigned int iAt = 0; iAt < iLength; ++iAt)
    {
        int iRet = mbtowc(&arrStrValueUtf16[iAt], &arrStrValueMultibyte[iAt], 1);
        if (iRet <= 0)
        {
            const wchar_t * szMsg = L"Conversion error: failed to conver to UTF-16.";
            ERROR_LOG(szMsg);
            throw CException(iError, szMsg);
        }
    }
    arrStrValueUtf16[iLength] = L'\0';

    return CEString(arrStrValueUtf16);
#else
    return CEString(to_wstring(dSource).c_str());
#endif

}   //  sToString (double dSource)

template<>
CEString sToString(const char * pchrSource)    // only basic conversions
{
#ifdef WIN32
    int iBytesNeeded = MultiByteToWideChar(CP_ACP, 0, pchrSource, -1, NULL, 0);
    if (iBytesNeeded < 1)
    {
        const wchar_t * szMsg = L"Conversion error: string too short.";
        ERROR_LOG(szMsg);
        throw CException(H_ERROR_INVALID_ARG, szMsg);
    }

    wchar_t * szNewBuf = new wchar_t[iBytesNeeded];
    int iRet = MultiByteToWideChar(CP_ACP, 0, pchrSource, -1, szNewBuf, iBytesNeeded);
    return CEString(szNewBuf);
#else
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    wstring kiki = converter.from_bytes(pchrSource);
    return CEString (converter.from_bytes(pchrSource).c_str());
#endif
}

}   //  namespace Hlib

#endif
