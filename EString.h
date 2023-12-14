#ifndef C_ESTRING_H_INCLUDED
#define C_ESTRING_H_INCLUDED

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING

#include <codecvt>
#include <memory>
#include <regex>
#include <iterator> // For std::forward_iterator_tag
#include <cstddef>  // For std::ptrdiff_t
#include <cwctype>
#include <cwchar>
#include <cassert>
#include <locale>

#include "Exception.h"
#include "Logging.h"

using namespace std;

namespace Hlib
{

//
// Helper objects
//
enum ERelation
{
    ecRelFront      = 20001,
    ecEqual,
    ecLess,
    ecGreater,
    ecNotFound      = 2*100000,
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
    static constexpr wchar_t szDefaultBreakChars_[] = L" \n";

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

//static const wchar_t * szDefaultBreakChars_ = L" \n";
//static const wchar_t * szDefaultTabs_ = L"\t";
//static const wchar_t * szDefaultPunctuation_ = L".,;:/?<>[]{}~!()-_\'\"\\…";
//static const wchar_t * szDefaultPunctuation_ = Ln".,;:?!\'\"";
//static const wchar_t * szDefaultEscapeChars_ = L"\27";
//static const wchar_t * szDefaultVowels_ = L"аеёиоуыэюя";
struct StBreakChars
{
    const wchar_t * m_szDefault;
    StBreakChars() : m_szDefault (L" \n") {}
};

struct StTabs
{
    const wchar_t * m_szDefault;
    StTabs() : m_szDefault (L"\t") {}
};

struct StPunctuation
{
    const wchar_t * m_szDefault;
    StPunctuation() : m_szDefault (L".,;:/?<>[]{}~!()-_\'\"\\…") {}
};

struct StEscape
{
    const wchar_t * m_szDefault;
    StEscape() : m_szDefault (L"\27") {}
};

struct StVowels
{
    const wchar_t * m_szDefault;
    StVowels() : m_szDefault(L"аеёиоуыэюя") {}
};

struct StRegex
{
    const wchar_t * m_szDefault;
    StRegex() : m_szDefault (nullptr) {}
};

template<typename SeparatorPolicy>
class CSeparators : public SeparatorPolicy
{
friend class CEString;
using SeparatorPolicy::m_szDefault;

private:
    static const unsigned int cuiMaxSeparatorLength_ = 1000;

public:
    CSeparators() : m_szVolatile (nullptr), m_bDisabled (false)
    {}

    CSeparators (const CSeparators& S) : m_szVolatile (nullptr), m_bDisabled(false)
    {
        Set (S.m_szVolatile.get());
    }

    ~CSeparators()
    {
//        if (m_szVolatile)
//        {
//            delete[] m_szVolatile;
//            m_szVolatile = nullptr;
//        }
    }

    void Set (const wchar_t * szSeparators)
    {
        if (!szSeparators)
        {
            m_bDisabled = true;
            return;
        }

        m_bDisabled = false;

        auto uiLength = wcslen (szSeparators);
        if (uiLength > cuiMaxSeparatorLength_)
        {
            wstring wstrMsg (L"Separator string too long.");
            ERROR_LOG(wstrMsg.c_str());
            throw CException (H_ERROR_INVALID_ARG, wstrMsg.c_str());
        }

        if (0 == uiLength)
        {
            m_bDisabled = true;
//            const wchar_t * szMsg = L"Empty separator string.";
//            ERROR_LOG(szMsg);
//            throw CException (H_ERROR_INVALID_ARG, szMsg);
            return;
        }

//        if (m_szVolatile)
//        {
//            delete[] m_szVolatile;
//            m_szVolatile = nullptr;
//        }

        m_szVolatile = make_unique<wchar_t[]>(uiLength + 1);
        m_szVolatile[uiLength] = L'\0';

        wmemcpy(m_szVolatile.get(), szSeparators, uiLength);

    }   //  void Set (...)

    void Reset()
    {
//        if (m_szVolatile)
//        {
//            delete[] m_szVolatile;
//            m_szVolatile = nullptr;
//        }
    }

    const wchar_t * szGet() const
    {
        if (m_bDisabled)
        {
            return NULL;
        }
        return (m_szVolatile) ? m_szVolatile.get() : m_szDefault;
    }

    unsigned int uiLength() const
    {
        if (m_bDisabled)
        {
            return 0;
        }
        return m_szVolatile ? (unsigned int)wcslen (m_szVolatile.get()) : (unsigned int)wcslen (m_szDefault);
    }

    bool bDisabled()
    {
        return m_bDisabled;
    }

private:
    unique_ptr<wchar_t[]> m_szVolatile;
    bool m_bDisabled;
};

class CEString
{
public:
//    static constexpr wstring str_CyrillicLowercase = L"-абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    static constexpr wchar_t g_szRusLowercase[] = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    static constexpr wchar_t g_szRusUppercase[] = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    static constexpr wchar_t g_szRusVowels[] = L"аеёиоуыэюя";
    static constexpr wchar_t g_szRusConsonants[] = L"бвгджзйклмнпрстфхцчшщ";
    static constexpr wchar_t g_szRusVoicedConsonants[] = L"бвгджзйлмнр";
    static constexpr wchar_t g_szRusVoicelessConsonants[] = L"кпстфхцчшщ";
    static constexpr wchar_t g_szRusHushers[] = L"жчшщ";

    wchar_t static const g_chrCombiningAcuteAccent = u'\u0301';
    wchar_t static const g_chrCombiningGraveAccent = u'\u0300';

    static constexpr wchar_t szDefaultTabs_[] = L"\t";
    static constexpr wchar_t szDefaultPunctuation_[] = L".,;:/?<>[]{}~!()-_\'\"\\…";
    //static const wchar_t * szDefaultPunctuation_ = Ln".,;:?!\'\"";
    static constexpr wchar_t szDefaultEscapeChars_[] = L"\27";
    //static const wchar_t * szDefaultVowels_ = L"аеёиоуыэюя";
    static constexpr unsigned int uiBlockSize_ = 10;
    static constexpr unsigned int cuiMaxSize_ = 100000;
    static constexpr unsigned int cuiMaxRegexLength_ = 2000;
    static constexpr unsigned int cuiMaxVowelsLength_ = 100;
    static constexpr unsigned int cuiMaxSeparatorLength_ = 1000;
    static constexpr unsigned int cuiMaxConversionLength_ = 129;
    static constexpr unsigned int cuiMaxSearchStringLength_ = 1000;
    static constexpr unsigned int cuiMaxSearchSetLength_ = 1000;

private:
    unique_ptr<wchar_t[]> m_szData;

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

    struct Iterator     // needed to enable range for loops
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = wchar_t;
        using pointer = wchar_t*; 
        using reference = wchar_t&;

        Iterator(wchar_t * p) : m_pData(p) {}

        wchar_t * operator->() { return m_pData; }
        wchar_t& operator*() const { return *m_pData; }

        // Prefix increment
        Iterator& operator++() { m_pData++; return *this; }

        // Postfix increment
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const Iterator& pLhs, const Iterator& pRhs) 
        { 
            return pLhs.m_pData == pRhs.m_pData; 
        };
        
        friend bool operator!= (const Iterator& pLhs, const Iterator& pRhs) 
        { 
            return pLhs.m_pData != pRhs.m_pData; 
        };

    private:
        wchar_t * m_pData;
    };

    Iterator begin() 
    { 
        return Iterator(&m_szData[0]); 
    }
    Iterator end() 
    { 
        return Iterator(&m_szData[m_uiLength]); 
    }

    // end of Iterator implementation

    CEString() : m_uiLength(0), m_uiBlocksAllocated (1), m_bInvalid (true)
    {
        m_szData = make_unique<wchar_t[]>(uiBlockSize_);
        m_szData[0] = L'\0';

        m_Breaks.m_bDisabled = false;
        m_Tabs.m_bDisabled = false;
        m_Punctuation.m_bDisabled = false;
        m_Escape.m_bDisabled = false;
    }

/*
    CEString (unsigned int uiBlocks) : m_uiLength(0), m_uiBlocksAllocated (uiBlocks), m_bInvalid (true)
    {
        unsigned int uiSize = m_uiBlocksAllocated * uiBlockSize_;
        if (uiSize >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Requested size exceeds maximum allowed.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }
        m_szData = new wchar_t[m_uiBlocksAllocated * uiBlockSize_];
        m_szData[0] = L'\0';
    }
*/

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
            wstring wstrMsg(L"Source string too long.");            
            ERROR_LOG(wstrMsg.c_str());
            throw CException (H_ERROR_INVALID_ARG, wstrMsg.c_str());
        }

        m_szData = make_unique<wchar_t[]>(m_uiBlocksAllocated * uiBlockSize_);
        if (m_uiLength > m_uiBlocksAllocated * uiBlockSize_)
        {
            wstring wstrMsg(L"Buffer overflow.");
            ERROR_LOG(wstrMsg.c_str());
            throw CException (H_EXCEPTION, wstrMsg.c_str());
        }
        wmemcpy(m_szData.get(), Source.m_szData.get(), m_uiLength); 
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
              const wchar_t * szVowels = NULL) : m_szData (NULL), m_uiBlocksAllocated (0), m_bInvalid(true)
    {
        m_uiLength = (unsigned int)wcslen (szSource);
        if (m_uiLength >= cuiMaxSize_)
        {
            wstring wstrMsg(L"Source string too long.");
            ERROR_LOG(wstrMsg.c_str());
            throw CException (H_ERROR_INVALID_ARG, wstrMsg.c_str());
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
            ERROR_LOG(szMsg); 
             
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }
        m_szData = make_unique<wchar_t[]>(m_uiBlocksAllocated * uiBlockSize_);
        m_szData[0] = chrValue;
        m_szData[1] = L'\0';
    
    }   //  CEString (wchar_t chrValue)

    virtual ~CEString()
    {
//        delete[] m_szData;
//        m_szData = NULL;
    }


public:

    // Operators

    operator wchar_t *() const
    {
        return m_szData.get();
    }

    wchar_t chrGetAt(int iAt)
    {
        if (iAt < 0 || iAt >= (int)m_uiLength)
        {
            return 0;
        }

        return m_szData[iAt];
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

        Assign (sRhs.m_szData.get(), sRhs.m_uiLength);
        
        return *this;
    
    }   //  operator= 

    CEString& operator+= (const wchar_t * szRhs)
    {
        unsigned int  uiRhsLength = (unsigned int)wcslen(szRhs);
        unsigned int uiNewSize = uiRhsLength + m_uiLength;
        if (uiNewSize >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Right-hand side string too long.";
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        Concatenate (sRhs.m_szData.get(), sRhs.m_uiLength);

        return *this;

    }   //  operator+=

/*
    bool operator== (const wchar_t * szRhs)
    {
        return ecEqual == CEString::eCompare (m_szData, szRhs);
    }

    bool operator== (const CEString& sRhs)
    {
        return ecEqual == CEString::eCompare (m_szData, sRhs);
    }

    bool operator== (CEString& sRhs)
    {
        return ecEqual == CEString::eCompare (m_szData, sRhs);
    }

    bool operator!= (const wchar_t * szRhs)
    {
        return ecEqual != CEString::eCompare (m_szData, szRhs);
    }

    bool operator!= (const CEString& sRhs)
    {
        return ecEqual != CEString::eCompare (m_szData, sRhs);
    }

    bool operator< (const wchar_t * szRhs)
    {
        return ecLess == CEString::eCompare (m_szData, szRhs);
    }

    bool operator< (const CEString& sRhs)
    {
        return ecLess == CEString::eCompare (m_szData, sRhs);
    }

    bool operator> (const wchar_t * szRhs)
    {
        return ecGreater == CEString::eCompare (m_szData, szRhs);
    }

    bool operator> (const CEString& sRhs)
    {
        return ecGreater == CEString::eCompare (m_szData, sRhs);
    }

    bool operator<= (const wchar_t * szRhs)
    {
        ERelation eRet = CEString::eCompare (m_szData, szRhs);
        return (ecLess == eRet || ecEqual == eRet);
    }

    bool operator<= (const CEString& sRhs)
    {
        ERelation eRet = CEString::eCompare (m_szData, sRhs);
        return (ecLess == eRet || ecEqual == eRet);
    }

    bool operator>= (const wchar_t * szRhs)
    {
        ERelation eRet = CEString::eCompare (m_szData, szRhs);
        return (ecGreater == eRet || ecEqual == eRet);
    }

    bool operator>= (const CEString& sRhs)
    {
        ERelation eRet = CEString::eCompare (m_szData, sRhs);
        return (ecGreater == eRet || ecEqual == eRet);
    }
*/

    // Mutators
    void ResetSeparators()
    {
        m_Breaks.Reset();
        m_Breaks.m_bDisabled = false;
        m_Tabs.Reset();
        m_Tabs.m_bDisabled = true;
        m_Punctuation.Reset();
        m_Punctuation.m_bDisabled = true;
        m_Escape.Reset();
        m_Escape.m_bDisabled = true;
        m_Vowels.Reset();
        m_Vowels.m_bDisabled = true;
    }

    void SetBreakChars (const wchar_t * szBreakChars)
    {
        m_Breaks.Set (szBreakChars);
        m_bInvalid = true;
    }

    void EnableBreaks()
    {
        m_Breaks.m_bDisabled = false;
    }

    void SetTabs (const wchar_t * szTabs)
    {
        m_Tabs.Set (szTabs);
        m_bInvalid = true;
    }

    void EnableTabs()
    {
        m_Tabs.m_bDisabled = false;
    }

    void SetPunctuation (const wchar_t * szPunctuation)
    {
        m_Punctuation.Set (szPunctuation);
        m_bInvalid = true;
    }

    void EnablePunctuation()
    {
        m_Punctuation.m_bDisabled = false;
    }
    
    void SetEscapeChars (const wchar_t * szEscapeChars)
    {
        m_Escape.Set (szEscapeChars);
        m_bInvalid = true;
    }

    void EnableEscapeChars()
    {
        m_Escape.m_bDisabled = false;
    }

    void SetVowels (const wchar_t * szVowels)
    {
        m_Vowels.Set (szVowels);
        m_bInvalid = true;
    }

    void EnableVowels()
    {
        m_Vowels.m_bDisabled = false;
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
        return (unsigned char *)m_szData.get();
    }

    // Iterators

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
        return eCompare (sLhs.m_szData.get(), szRhs);

    }       // eCompare (...)

    static ERelation eCompare (const wchar_t * szLhs, const CEString& sRhs)
    {
        return eCompare (szLhs, sRhs.m_szData.get());

    }       // eCompare (...)

    static ERelation eCompare (const CEString& sLhs, const CEString& sRhs)
    {
        return eCompare (sLhs.m_szData.get(), sRhs.m_szData.get());

    }       // eCompare (...)

    static ERelation eCompareNoCase (const wchar_t * szLeft, const wchar_t * szRight)
    {
        auto sizeLengthLeft = wcslen(szLeft);
        auto sizeLengthRight = wcslen(szRight);

        if (sizeLengthLeft >= cuiMaxSize_  || sizeLengthRight >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Search string too long.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }
        
        setlocale(LC_ALL, "");
        
        for (int iAt = 0; iAt < (int)min(sizeLengthLeft, sizeLengthRight); ++iAt)
        {            
            if (towlower(szLeft[iAt]) < towlower(szRight[iAt]))
            {
                return ERelation::ecLess;
            }
            if (towlower(szLeft[iAt]) > towlower(szRight[iAt]))
            {
                return ERelation::ecGreater;
            }
        }

        if (sizeLengthLeft < sizeLengthRight)
        {
            return ERelation::ecLess;
        }

        if (sizeLengthLeft > sizeLengthRight)
        {
            return ERelation::ecGreater;
        }

        return ERelation::ecEqual;

    }       // eCompareNoCase (...)

    // Search
    static bool bIn (const wchar_t chr, const wchar_t * szSearchSet) 
    {
        if (!szSearchSet || !chr)
        {
            return false;
        }

        auto uiSearchSetLength = wcslen (szSearchSet);
        if (uiSearchSetLength > cuiMaxSearchSetLength_)
        {
            const wchar_t * szMsg = L"Search set too long.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        for (int iAt = 0; iAt < (int)cuiMaxSearchSetLength_; ++iAt)
        {
            if (chr == szSearchSet[iAt])
            {
                return true;
            }

            if (L'\0' == szSearchSet[iAt])
            {
                return false;
            }
        }

        const wchar_t * szMsg = L"Search set too long.";
        ERROR_LOG(szMsg);
        throw CException (H_ERROR_INVALID_ARG, szMsg);
    
    }   //  bIn

    static bool bStringOverAlphabet(const CEString& str, const wchar_t * szAlphabet)
    {
        CEString sRegex(L"^([");
        sRegex += szAlphabet;
        sRegex += L"]+)";
        wregex regex(sRegex.m_szData.get());
        wsmatch match;
        wstring wstrData(str);
        auto bRet = regex_match(wstrData, match, regex);

        return bRet && match[0].matched;
    }

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
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (uiLength > m_uiLength - uiStartAt)
        {
            return ecNotFound;
        }

        if (uiLength > cuiMaxSearchStringLength_)
        {
            const wchar_t * szMsg = L"Search string too long.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        const wchar_t * pPos = wcsstr (&m_szData[uiStartAt], szRhs);
        if (pPos)
        {
            return static_cast<unsigned int>(pPos - m_szData.get());
        }

        return ecNotFound;

    }   //  uiFind (...)

    unsigned int uiFindNoCase (const wchar_t * szSearchStr)
    {
        if (0 == m_uiLength)
        {
            return ecNotFound;
        }

        setlocale(LC_ALL, "");
        
        auto uiSearchStrLength = wcslen(szSearchStr);
        if (uiSearchStrLength > m_uiLength)
        {
            return ecNotFound;
        }

        if (uiSearchStrLength >= cuiMaxSearchStringLength_)
        {
            const wchar_t * szMsg = L"Search string too long.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, L"Search string too long.");
        }

        auto spSearchStrLC = make_unique<wchar_t[]>(uiSearchStrLength+1);
        if (NULL == spSearchStrLC)
        {
            const wchar_t * szMsg = L"Unable to allocate temp. buffer for search string.";
            ERROR_LOG(szMsg);
            throw CException(H_EXCEPTION, szMsg);
        }

//        wmemcpy(spSearchStrLC.get(), szSearchStr, uiSearchStrLength+1); 
        for (int iAt = 0; iAt < (int)uiSearchStrLength; ++iAt)
        {
            spSearchStrLC[iAt] = towlower(szSearchStr[iAt]);
        }

        auto spDataLC = make_unique<wchar_t[]>(m_uiLength+1);
        if (NULL == spDataLC)
        {
            const wchar_t * szMsg = L"Unable to allocate temp. buffer for search string.";
            ERROR_LOG(szMsg);
            throw CException(H_EXCEPTION, szMsg);
        }

//        wmemcpy(spDataLC.get(), m_szData.get(), m_uiLength+1); 
        for (int iAt = 0; iAt < (int)m_uiLength; ++iAt)
        {
            spDataLC[iAt] = towlower(m_szData[iAt]);
        }

        const wchar_t * pPos = wcsstr(spDataLC.get(), spSearchStrLC.get());
        if (pPos)
        {
            return static_cast<unsigned int>(pPos - spDataLC.get());
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
            ERROR_LOG(L"*** Warning: empty string");
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

/*
    unsigned int uiRFindNoCase (const wchar_t * szRhs) const
    {
&&&&
        if (0 == m_uiLength)
        {
            ERROR_LOG(L"*** Warning: empty string");
            return ecNotFound;
        }

        unsigned int uiRhsLength = wcslen (szRhs);
        if (uiRhsLength > m_uiLength)
        {
            return ecNotFound;
        }

        for (int iAt = m_uiLength - uiRhsLength; iAt > 0; --iAt)
        {
            if (0 == wcscmp (m_szData, szRhs))
            {
                return (unsigned int)iAt;
            }
        }

        return ecNotFound;
    
    }   //  uiRFindNoCase
*/

    unsigned int uiFindFirstOf (const wchar_t * szSet) const
    {
        size_t uiAt = wcscspn (m_szData.get(), szSet);

        if (m_uiLength == uiAt)
        {
            return ecNotFound;
        }

        return (unsigned int)uiAt;
    
    }   //  uiFindFirstOf (...)

/*
    unsigned int uiFindFirstOfNoCase (const wchar_t * szSet) const
    {
&&&&
        size_t uiAt = wcscspn (m_szData, szSet);

        if (m_uiLength == uiAt)
        {
            return ecNotFound;
        }

        return uiAt;
    
    }   //  uiFindFirstOfNoCase (...)
*/

    unsigned int uiFindOneOf (unsigned int uiStartAt, const wchar_t * szSet) const
    {
        if (uiStartAt >= m_uiLength)
        {
            const wchar_t * szMsg = L"Search starts past end of string.";
            ERROR_LOG(szMsg);
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

/*
    unsigned int uiFindLastOfNoCase (const wchar_t * szSet) const
    {
&&&&
        for (int iAt = m_uiLength-1; iAt >= 0; --iAt)
        {
            if (bIn (m_szData[iAt], szSet))
            {
                return (unsigned int)iAt;
            }
        }

        return ecNotFound;
    
    }   //  uiFindLastOfNoCase (...)
*/

    bool bStartsWith (const wchar_t * szRhs) const
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
        
        int iRet = wcsncmp (m_szData.get(), szRhs, uiRhsLength);

        return (0 == iRet);
    
    }   // bStartsWith (...)

    bool bStartsWithNoCase (const wchar_t * szRhs) const
    {
        setlocale(LC_ALL, "");

        if (0 == m_uiLength)
        {
            return false;
        }

        auto uiRhsLength = wcslen(szRhs);
        if (uiRhsLength > cuiMaxSearchStringLength_)
        {
            ERROR_LOG(L"Search string too long.");
            return false;
        }

        if (uiRhsLength > m_uiLength)
        {
            return false;
        }

        auto spRhsLC = make_unique<wchar_t[]>(uiRhsLength+1);
        if (!spRhsLC)
        {
            ERROR_LOG(L"Unable to allocate memory for search string.");
            return false;
        }
        
        for (int iAt = 0; iAt < (int)uiRhsLength; ++iAt)
        {
            spRhsLC[iAt] = towlower(szRhs[iAt]);
        }

        auto spDataLC = make_unique<wchar_t[]>(m_uiLength+1);
        if (!spDataLC)
        {
            ERROR_LOG(L"Unable to allocate memory for data.");
            return false;
        }

        for (int iAt = 0; iAt < (int)m_uiLength; ++iAt)
        {
            spDataLC[iAt] = towlower(m_szData[iAt]);
        }
        
        int iRet = wcsncmp (spDataLC.get(), spRhsLC.get(), uiRhsLength);

        return (0 == iRet);
    
    }   //  bStartsWithNoCase (...)

    bool bStartsWithOneOf (const wchar_t * szRhs) const
    {
        if (0 == m_uiLength)
        {
            return false;
        }

        return bIn (m_szData[0], szRhs);
    }

    bool bStartsWithOneOfNoCase(const wchar_t * szRhs) const
    {
        setlocale(LC_ALL, "");

        if (0 == m_uiLength)
        {
            return false;
        }

        wchar_t cLhs = m_szData[0];
        cLhs = towlower (cLhs);

        auto uiRhsLength = wcslen (szRhs);
        if (uiRhsLength >= cuiMaxSearchSetLength_)
        {
            const wchar_t * szMsg = L"Search string too long.";
            ERROR_LOG(szMsg);
            return false;
        }

        unique_ptr<wchar_t[]> szRhsLC = make_unique<wchar_t[]>(uiRhsLength+1);
        wmemcpy(szRhsLC.get(), szRhs, uiRhsLength+1);
        for (int iAt = 0; iAt < (int)uiRhsLength; ++iAt)
        {
            auto chrFound = wcschr(g_szRusUppercase, szRhsLC.get()[iAt]);
            if (chrFound)
            {
                szRhsLC.get()[iAt] = g_szRusLowercase[iAt];
            }
        }

        auto iAt = wcscspn (&cLhs, szRhsLC.get());

        return (0 == iAt);
    
    }   //  bEndsWithOneOfNoCase (...)

    bool bEndsWith (const wchar_t * szRhs) const
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
        
        int iRet = wcsncmp (&m_szData[m_uiLength-uiRhsLength], szRhs, uiRhsLength);
        
        return (0 == iRet);
    
    }   //  bEndsWith (...)

    bool bEndsWithNoCase (const wchar_t * szRhs)
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

        auto spRhsLC = make_unique<wchar_t[]>(uiRhsLength+1);
        if (NULL == spRhsLC)
        {
            ERROR_LOG(L"Unable to allocate temp. buffer for search string.");
            return false;
        }

        wmemcpy(spRhsLC.get(), szRhs, uiRhsLength+1);

        for (int iAt = 0; iAt < (int)uiRhsLength; ++iAt)
        {
            spRhsLC[iAt] = towlower(spRhsLC[iAt]);
        }

        auto spDataLC = make_unique<wchar_t[]>(m_uiLength+1);
        if (NULL == spDataLC)
        {
            ERROR_LOG(L"Unable to allocate temp. buffer for data.");
            return false;
        }

        wmemcpy(spDataLC.get(), m_szData.get(), m_uiLength+1);

        for (int iAt = 0; iAt < (int)m_uiLength; ++iAt)
        {
            spDataLC[iAt] = towlower(spDataLC[iAt]);
        }

        int iRet = wcsncmp(&spDataLC.get()[m_uiLength-uiRhsLength], spRhsLC.get(), uiRhsLength);

        return (0 == iRet);
    
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
        unique_ptr<wchar_t[]> szRhsLowerCase = make_unique<wchar_t[]> (uiRhsLength + 1);
        wcscpy(szRhsLowerCase.get(), szRhs);
        for (unsigned int uiAt = 0; uiAt < uiRhsLength; ++uiAt)
        {
            szRhsLowerCase[uiAt] = towlower (szRhs[uiAt]);
        }
        auto iAt = wcscspn(&cLhs, szRhsLowerCase.get());

        return (0 == iAt);
    
    }   //  bEndsWithOneOfNoCase (...)

    // Manipulations
    CEString& sInsert (unsigned int uiInsertAt, const wchar_t chrInsert)
    {
        if (uiInsertAt > m_uiLength)
        {
            const wchar_t * szMsg = L"Bad insersion pos.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (L'\0' == chrInsert)
        {
            const wchar_t * szMsg = L"Bad insertion char.";
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (0 == uiCharsToInsert)
        {
            ERROR_LOG(L"*** Warning: attempt to insert empty string");
            return *this;
        }

        auto uiNewLength = m_uiLength + uiCharsToInsert;
        if (uiNewLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Insertions string too long.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (uiNewLength >= m_uiBlocksAllocated * uiBlockSize_)
        {
            m_uiBlocksAllocated = (unsigned int)((uiNewLength+1)/uiBlockSize_) + 1;
        }

        auto uiNewAllocSize = m_uiBlocksAllocated * uiBlockSize_;
        unique_ptr<wchar_t[]> szNewData = make_unique<wchar_t[]>(uiNewAllocSize);

        wmemmove(szNewData.get(), m_szData.get(), uiInsertAt); 
        wmemmove(&szNewData[uiInsertAt], szInsert, uiCharsToInsert); 

        auto uiPastInsertion = uiInsertAt + uiCharsToInsert;
        wmemmove (&szNewData[uiPastInsertion], &m_szData[uiInsertAt], m_uiLength - uiInsertAt); 

        m_uiLength += uiCharsToInsert;
        szNewData[m_uiLength] = L'\0';

        m_szData = move(szNewData);

        return *this;

    }   //  CEString& sInsert (...)

    CEString& sReplace (unsigned int uiStartAt, wchar_t * szReplace)
    {
        auto uiInsertLength = wcslen (szReplace);
        if (uiStartAt + uiInsertLength > m_uiLength)
        {
            const wchar_t * szMsg = L"Illegal length.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

//        int iBufSize = m_uiBlocksAllocated * uiBlockSize_ - uiStartAt;
        wmemcpy(&m_szData[uiStartAt], szReplace, uiInsertLength);

        return *this;
    
    }   //  CEString& sReplace (unsigned int uiStartAt, wchar_t * szReplace)

    CEString& sReplace (unsigned int uiAt, wchar_t chrReplace)
    {
        if (uiAt >= m_uiLength)
        {
            const wchar_t * szMsg = L"Illegal position.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        m_szData[uiAt] = chrReplace;

        return *this;

    }   //  CEString& sReplace (unsigned int uiStartAt, wchar_t chrReplace)

    CEString& sReplace (unsigned int uiAt, unsigned int uiMaxCharsToErase, const wchar_t * szReplace)
    {
        if (uiAt >= m_uiLength)
        {
            const wchar_t * szMsg = L"Illegal insertion point.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        auto uiCharsToInsert = wcslen (szReplace);
        auto uiCharsToErase = min (uiMaxCharsToErase, m_uiLength - uiAt);
        auto uiNewLength = m_uiLength - uiCharsToErase + uiCharsToInsert;
        if (uiNewLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Replacement string too long.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        auto uiBlocksToAllocate = ((uiNewLength+1)/uiBlockSize_) + 1;
        auto uiNewSize = uiBlocksToAllocate * uiBlockSize_;
        unique_ptr<wchar_t[]> szNewData = make_unique<wchar_t[]>(uiNewSize);
        wmemmove(szNewData.get(), m_szData.get(), uiAt); 
        wmemmove(&szNewData[uiAt], szReplace, uiCharsToInsert); 
        
        if (m_uiLength > uiAt+uiCharsToErase)
        {
            unsigned int uiRemainder = m_uiLength - (uiAt+uiCharsToErase) + 1;
            wmemmove(&szNewData[uiAt+uiCharsToInsert], &m_szData[uiAt+uiCharsToErase], uiRemainder); 
        }

        m_szData = move(szNewData);
        m_uiBlocksAllocated = static_cast<unsigned int>(uiBlocksToAllocate);
        m_uiLength = static_cast<unsigned int>(m_uiLength - uiCharsToErase + uiCharsToInsert);
        m_szData[m_uiLength] = L'\0';

        return *this;

    }   //  CEString& sReplace (unsigned int uiStartAt, wchar_t chrReplace)

    void Replace (unsigned int uiStartPos, unsigned int uiHowMany, wchar_t cWhat, wchar_t cWithWhat)
    {
        if (0 == m_uiLength)
        {
            ERROR_LOG(L"*** Warning: empty string.");
            return;
        }

        if (uiStartPos >= m_uiLength)
        {
            const wchar_t * szMsg = L"Start position beyond string end.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (uiStartPos + uiHowMany > m_uiLength)
        {
//            assert(0);
            ERROR_LOG(L"*** Warning: attempt to replace beyond string end.");
            uiHowMany = m_uiLength - uiStartPos;
        }

//        const wchar_t * pcSearchAt = &m_szData[uiStartPos];
        wchar_t * pcReplaceAt = NULL;
        do
        {
            pcReplaceAt = wcschr(&m_szData[uiStartPos], cWhat);
            if (pcReplaceAt)
            {
                *pcReplaceAt = cWithWhat;
//                if (L'\0' == *(++pcReplaceAt))
//                {
//                    pcSearchAt = NULL;
//                }
//                else
//                {
//                    pcSearchAt = pcReplaceAt;
//                }
            }

        } while (pcReplaceAt);
    
    }   //  void Replace (...)

    CEString sReplace(const wchar_t * szOld, wchar_t * szNew)
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
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (L'\0' != m_szData[m_uiLength])
        {
            const wchar_t * szMsg = L"Malformed string.";
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        //if (uiFirst + uiHowMany >= m_uiLength)
        //{
        //    wchar_t * szMsg = L"Bad length.";
        //    ERROR_LOG(szMsg);
        //    throw CException (H_ERROR_INVALID_ARG, szMsg);
        //}

        uiHowMany = min (uiHowMany, m_uiLength - uiFirst);

        unsigned int uiMoveFrom = uiFirst+uiHowMany;
//        int iBufSize = (m_uiBlocksAllocated * uiBlockSize_) - uiFirst - 1;
        wmemmove(&m_szData[uiFirst], &m_szData[uiMoveFrom], m_uiLength-uiMoveFrom); 

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
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        m_uiLength = uiAt;
        m_szData[m_uiLength] = L'\0';

        Shrink();

        return *this;
    
    }   //  CEString& sErase (...)

    CEString& sRemoveCharsFromEnd(unsigned int uiCharsToRemove)
    {
        if (uiCharsToRemove > m_uiLength)
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
        setlocale(LC_ALL, "");

        for (int iAt = 0; iAt < (int)m_uiLength; ++iAt)
        {
            m_szData[iAt] = towlower(m_szData[iAt]);
        }
    }

    static CEString sToLower (const wchar_t * szSource)
    {
        setlocale(LC_ALL, "");

        auto uiRhsLength = wcslen (szSource);
        if (uiRhsLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Argument string too long.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        unique_ptr<wchar_t[]> szCopy = make_unique<wchar_t[]>(uiRhsLength+1);
        szCopy[uiRhsLength] = L'\0';

//        wmemmove(szCopy.get(), szSource, uiRhsLength); 
        for (int iAt = 0; iAt < (int)uiRhsLength; ++iAt)
        {
            szCopy[iAt] = towlower(szSource[iAt]);
        }

        return CEString(szCopy.get());
    
    }   //  sToLower (const wchar_t * szSource)

    static CEString sToLower (const CEString& sSource)
    {
        CEString sCopy (sSource);
        sCopy.ToLower();
        return sCopy;
    }

    void ToUpper()
    {
        setlocale(LC_ALL, "");
        
        for (int iAt = 0; iAt < (int)m_uiLength; ++iAt)
        {
            m_szData[iAt] = towupper(m_szData[iAt]);
        }
    }
    
    static CEString sToUpper (const wchar_t * szSource)
    {
        setlocale(LC_ALL, "");
        
        auto uiRhsLength = wcslen (szSource);
        if (uiRhsLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Argument string too long.";
            throw CException (H_ERROR_INVALID_ARG, szMsg);
            ERROR_LOG(szMsg);
        }

        unique_ptr<wchar_t[]> szCopy = make_unique<wchar_t[]>(uiRhsLength+1);
        szCopy[uiRhsLength] = L'\0';

//        wmemmove(szCopy.get(), szSource, uiRhsLength); 
        for (int iAt = 0; iAt < (int)uiRhsLength; ++iAt)
        {
            szCopy[iAt] = towupper(szSource[iAt]);
        }

        return CEString(szCopy.get());
    }

    static CEString sToUpper (const CEString& sSource)
    {
        CEString sCopy (sSource);
        sCopy.ToUpper();
        return sCopy;
    }

    CEString sSubstr (unsigned int uiOffset, unsigned int uiLength = cuiMaxSize_) const
    {
        if (uiLength > cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Source string too long.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (0 == uiLength)
        {
            ERROR_LOG(L"*** Warning: empty string");
            return CEString (L"");
        }

        if (L'\0' != m_szData[m_uiLength])
        {
            const wchar_t * szMsg = L"Source string not null-terminated.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (uiOffset == m_uiLength)
        {
            ERROR_LOG(L"*** Warning: empty substring");
            return CEString (L"");
        }

        if (uiOffset > m_uiLength)
        {
            const wchar_t * szMsg = L"Invalid offset.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (uiLength < cuiMaxSize_)
        {
            if (uiOffset + uiLength > m_uiLength)
            {
                const wchar_t * szMsg = L"Invalid length.";
                ERROR_LOG(szMsg);
                throw CException (H_ERROR_UNEXPECTED, szMsg);
            }
        }

        CEString sResult;
        unsigned int uiCharsToMove = min (uiLength, m_uiLength - uiOffset);
        assert(uiCharsToMove <= m_uiLength);
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
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (L'\0' != m_szData[m_uiLength])
        {
            const wchar_t * szMsg = L"Malformed string.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        unsigned int uiAt = 0;
        for (; uiAt < m_uiLength; ++uiAt)
        {
            if (L'\0' == m_szData[uiAt])
            {
                const wchar_t * szMsg = L"Malformed string.";
                ERROR_LOG(szMsg);
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
        wmemmove (m_szData.get(), &m_szData[uiAt], m_uiLength); 
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
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (L'\0' != m_szData[m_uiLength])
        {
            const wchar_t * szMsg = L"Malformed string.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        int iAt = (int)m_uiLength-1;
        for (; iAt >= 0; --iAt)
        {
            if (L'\0' == m_szData[iAt])
            {
                const wchar_t * szMsg = L"Malformed string.";
                ERROR_LOG(szMsg);
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
//        _wcsrev (m_szData.get());
        for(int iAt = 0; iAt < (int)m_uiLength/2; ++iAt)
        {
            wchar_t chrTmp = m_szData[iAt];
            m_szData[iAt] = m_szData[m_uiLength-iAt-1];
            m_szData[m_uiLength-iAt-1] = chrTmp;
        }
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
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
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
            const wchar_t * szMsg = L"Token not found.";
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        return m_vecTokens[uiAt].uiOffset;
    }

    unsigned int uiGetRegexOffset (unsigned int uiAt) const
    {
        if (uiAt >= m_vecRegexMatches.size())
        {
            const wchar_t * szMsg = L"m_vecRegexMatches member index out of range";
            ERROR_LOG(szMsg);
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
        //    ERROR_LOG(szMsg);
        //    throw CException (H_ERROR_INVALID_ARG, szMsg);
        //}
        
        vector<StToken>& rvecTokens = (ecTokenRegexMatch == eType) ? m_vecRegexMatches : m_vecTokens;
        vector<StToken>::iterator itToken = rvecTokens.end();
        try
        {
            itToken = itFindToken(uiAt, eType);
        }
        catch(...)
        {
            const wchar_t* szMsg = L"FindToken() failed: ";
            ERROR_LOG(szMsg);
            return 0;
        }

        if (rvecTokens.end() == itToken)
        {
            const wchar_t * szMsg = L"Failed to find token.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        return (*itToken).uiOffset;
    
    }   //  uiGetFieldOffset (...)

    CEString sGetRegexMatch (unsigned int iAt)
    {
        //if (wcslen (m_szRegex) < 1)
        //{
        //    ERROR_LOG(L"Warning: Empty regex string");
        //    return CEString (L"");
        //}

        if (m_Regex.m_bDisabled)
        {
            ERROR_LOG(L"Warning: regex string missing or invalid");
            return CEString (L"");
        }

        if (iAt >= m_vecRegexMatches.size())
        {
            ERROR_LOG(L"*** m_vecRegexMatches member index out of range");
            return CEString (L"");

//            const wchar_t * szMsg = L"m_vecRegexMatches member index out of range";
//            ERROR_LOG(szMsg);
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
            ERROR_LOG(sMsg);
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
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        vector<StToken>::iterator itToken = itFindToken (uiAt, eType);
        if (rvecTokens.end() == itToken)
        {
            const wchar_t * szMsg = L"Failed to find token.";
            ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
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
            wchar_t szMsgBuf[1000];
            wcscpy(szMsgBuf, L"Vowels not defined, string = ");
            wcscat(szMsgBuf, m_szData.get());
            ERROR_LOG(szMsgBuf);
            throw CException (H_ERROR_GENERAL, szMsgBuf);
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
    static bool bIsConsonant(wchar_t chr)
    {
        return bIn(chr, g_szRusConsonants);
    }

    static bool bIsVowel(wchar_t chr)
    {
        return bIn(chr, g_szRusVowels);
    }

    unsigned int uiGetVowelPos (unsigned int uiVowel = 0) const
    {
        if (m_Vowels.uiLength() == 0)
        {
//            assert(0);
            const wchar_t * szMsg = L"Vowels not defined";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        unsigned int uiAt = 0;
//        bool bFound = false;
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

//        assert(0);
        const wchar_t * szMsg = L"Vowel position not found";
        ERROR_LOG(szMsg);
        throw CException (H_ERROR_GENERAL, szMsg);

        return 0;
    
    }   //  uiGetVowelPos (...)
    
    unsigned int uiGetSyllableFromVowelPos (unsigned int uiAbsPos) const
    {
        if (m_Vowels.uiLength() == 0)
        {
//            assert(0);
            const wchar_t * szMsg = L"Vowels not defined";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        if (m_uiLength <= uiAbsPos)
        {
//            assert(0);
            const wchar_t * szMsg = L"Vowel position invalid";
            ERROR_LOG(szMsg);
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
//                    assert(0);
                    const wchar_t * szMsg = L"Vowel position not found";
                    ERROR_LOG(szMsg);
                    throw CException (H_ERROR_GENERAL, szMsg);
                    return 0;
                }
            }

        } while (uiVowelPos < m_uiLength);

//        assert(0);
        const wchar_t * szMsg = L"Vowel position not found";
        ERROR_LOG(szMsg);
        throw CException (H_ERROR_GENERAL, szMsg);
        return 0;

    }   //  uiGetSyllableFromVowelPos (...)

    // Conversions    
    template<typename T> 
    static CEString sToString(T tSource)
    {
        auto wstrValue = to_wstring(tSource);
        return CEString(wstrValue.c_str());
    }

//    template<typename fake>
//    static CEString sToString (int64_t llSource)
//    {
//        auto wstrValue = to_wstring(llSource);
//        return CEString(wstrValue.c_str());
//    }

//    template<typename fake>
//    static CEString sToString (uint64_t ullSource)
//    {
//        auto wstrValue = to_wstring(ullSource);
//        return CEString(wstrValue.c_str());
//    }

//    template<typename fake>
//    static CEString sToString (double dSource)
//    {
//        auto wstrValue = to_wstring(dSource);
//        return CEString(wstrValue.c_str());
    
//    }   //  sToString (double dSource)

//    template<typename fake>
    static CEString sToString(const char * pchrSource)    // only basic conversions
    {
        std::wstring str = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(pchrSource);
        return CEString(str.c_str());
    }

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
        wmemmove(arrSource, m_szData.get(), iLength+1);
        wstring_convert<codecvt_utf8<wchar_t>> converter;
        const string utf8_string = converter.to_bytes(arrSource);

        return utf8_string;
    }

    static string stl_sToUtf8(const wchar_t * szSource)
    {
        wchar_t arrSource[cuiMaxSize_ + 1];
        auto uiLength = (unsigned int)wcslen(szSource);
        int iLength = (int)min(uiLength, cuiMaxSize_);
        wmemmove(arrSource, szSource, iLength+1);
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
        assert(uiNewBlocks >= m_uiBlocksAllocated);
        if (uiNewBlocks == m_uiBlocksAllocated)
        {
            return;
        }
        
        unique_ptr<wchar_t[]> szNewBuffer = make_unique<wchar_t[]>(uiNewBlocks * uiBlockSize_);
        if (m_uiLength > 0)
        {
            wmemmove(szNewBuffer.get(), m_szData.get(), m_uiLength+1); 
        }
//        delete[] m_szData;
        m_szData = move(szNewBuffer);
    
    }   //  Grow()

    void Shrink()
    {
        unsigned int uiBlocksToFree = ((m_uiBlocksAllocated * uiBlockSize_) - (m_uiLength + 1)) / uiBlockSize_;
        if (uiBlocksToFree < 1)
        {
            return;
        }

        m_uiBlocksAllocated -= uiBlocksToFree;
        m_uiBlocksAllocated = max ((int)m_uiBlocksAllocated, 1);
        unique_ptr<wchar_t[]> szNewBuffer = make_unique<wchar_t[]>(m_uiBlocksAllocated * uiBlockSize_);
        wmemmove(szNewBuffer.get(), m_szData.get(), m_uiLength + 1); 
        m_szData = move(szNewBuffer);
    
    }   //  Shrink()

    void Concatenate (const wchar_t * szRhs, unsigned int uiRhsLength)
    {
        unsigned int uiNewLength = m_uiLength + uiRhsLength;
        if (uiNewLength >= cuiMaxSize_)
        {
            const wchar_t * szMsg = L"Right-hand side string too long.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (uiNewLength >= m_uiBlocksAllocated * uiBlockSize_)
        {
            m_uiBlocksAllocated = ((uiNewLength+1)/uiBlockSize_) + 1;
            unsigned int uiAllocSize = m_uiBlocksAllocated * uiBlockSize_;
            unique_ptr<wchar_t[]> szNewData = make_unique<wchar_t[]>(uiAllocSize);
            wmemmove(szNewData.get(), m_szData.get(), m_uiLength + 1);
            m_szData = move(szNewData);
        }

        wmemmove(&m_szData[m_uiLength], szRhs, uiRhsLength + 1); 
        m_uiLength = uiNewLength;

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
                    ERROR_LOG(szMsg);
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
                    ERROR_LOG(szMsg);
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

                assert(m_uiLength > 0);
                if (uiAt >= m_uiLength-1)
                {
                    m_bInvalid = true;
                    const wchar_t * szMsg = L"Unexpected escape character.";
                    ERROR_LOG(szMsg);
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
        ERROR_LOG(szMsg);
        throw CException (H_ERROR_UNEXPECTED, szMsg);

    }   //  Tokenize_ (...)

    void Advance (ETokenType eType, unsigned int uiOffset, StToken& stToken)
    {
        if (eType <= ecTokenTypeFront || eType > ecTokenTypeBack)
        {
            const wchar_t * szMsg = L"Unexpected token state.";
            ERROR_LOG(szMsg);
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
        if (ecTokenMeta == stToken.eType)
        {
            const wchar_t * szMsg = L"Unexpected token state.";
            ERROR_LOG(szMsg);
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
                ERROR_LOG(szMsg);
                throw CException (H_ERROR_UNEXPECTED, szMsg);
            }

            if (bIn (m_szData[iAt], m_Escape.szGet()))
            {
                if ((int)uiOffset+1 == iAt)
                {
                    const wchar_t * szMsg = L"Empty escape sequence.";
                    ERROR_LOG(szMsg);
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
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        if (L'\0' != szSource[uiSourceLength])
        {
            const wchar_t * szMsg = L"Source string not null-terminated.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_UNEXPECTED, szMsg);
        }

        if (uiSourceLength >= (m_uiBlocksAllocated * uiBlockSize_))
        {
            m_uiBlocksAllocated = ((uiSourceLength+1)/uiBlockSize_) + 1;
//            if (m_szData)
//            {
//                delete[] m_szData;
//            }
            m_szData = make_unique<wchar_t[]>(uiBlockSize_ * m_uiBlocksAllocated);
        }

        wmemmove(m_szData.get(), szSource, uiSourceLength); 

        m_szData[uiSourceLength] = L'\0';
        m_uiLength = uiSourceLength;

        m_bInvalid = true;
    
    }   //  Assign (...)

    vector<StToken>::iterator itFindToken (unsigned int uiAt, ETokenType eType)
    {
        vector<StToken>& rvecTokens = (ecTokenRegexMatch == eType) ? m_vecRegexMatches : m_vecTokens;
        if (ecTokenRegexMatch == eType)
        {
//            if ((0 == wcslen (m_szRegex)) || m_vecRegexMatches.empty())
            if (m_Regex.bDisabled() || m_vecRegexMatches.empty())
            {
                ERROR_LOG(L"*** Warning: no regex matches.");
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
            ERROR_LOG(szMsg);
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
            wchar_t szMsgBuf[1000];
            wcscpy(szMsgBuf, L"Token position out of range, string = ");
            wcscat(szMsgBuf, m_szData.get());
            ERROR_LOG(szMsgBuf);
            throw CException (H_ERROR_UNEXPECTED, szMsgBuf);
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
                ERROR_LOG(L"*** Warning: no regex matches.");
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
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_GENERAL, szMsg);
        }

        return --it_;

    }   //  itTokenFromOffset (...)

    bool bRegexEvaluate (const wchar_t * szRegex, bool bMatchMode = true)
    {
        if (nullptr == szRegex)
        {
            const wchar_t* szMsg = L"Regular expression is null.";
            ERROR_LOG(szMsg);
            throw CException(H_ERROR_INVALID_ARG, szMsg);
        }

        if (0 == m_uiLength)
        {
            return false;
        }

        auto uiRegexLength = wcslen (szRegex);
        if (uiRegexLength >= cuiMaxRegexLength_)
        {
            const wchar_t * szMsg = L"Regular expression too long.";
            ERROR_LOG(szMsg);
            throw CException (H_ERROR_INVALID_ARG, szMsg);
        }

        m_Regex.Set (szRegex);
/*
        errno_t error = wmemmove_s (m_szRegex, cuiMaxRegexLength_-1, szRegex, uiRegexLength); 
        if (error)
        {
            wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG(szMsg);
            throw CException (error, szMsg);
        }

        m_szRegex[uiRegexLength] = L'\0';
*/
        bool bRet = false;

        try
        {
            wregex regex_ (m_Regex.szGet());
            wsmatch match_;
            wstring wstrData (m_szData.get());
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
        catch (regex_error& rxError_)
        {
            wstring str_msg (L"Regex error: ");
            str_msg += str_RegexError_ (rxError_.code());
            ERROR_LOG(str_msg.c_str());
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
//            case regex_constants::error_parse:
//            {
//                str_error = L"error_parse";
//                break;
//            }
//            case regex_constants::error_syntax:
//            {
//                str_error = L"error_syntax";
//                break;
//            }
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

}   //  namespace Hlib

#endif
