#pragma once

#include "tchar.h"
#include <Regex>
#include "Exception.h"

using namespace std;

static const wchar_t * szDefaultBreakChars_ = L" \n";
static const wchar_t * szDefaultTabs_ = L"\t";
static const wchar_t * szDefaultPunctuation_ = L".,;:/?<>[]{}~!()-_\'\"\\";
static const wchar_t * szDefaultEscapeChars_ = L"\27";
static unsigned int uiAllocationUnit_ = 1000;
static const unsigned int cuiMaxSize_ = 100000;
static const unsigned int cuiMaxRegexLength_ = 1000;
static const unsigned int cuiMaxVowelsLength_ = 100;
static const unsigned int cuiMaxSeparatorLength_ = 1000;

//
// Helper objects
//
enum ERelation
{
    ecRelFront      = 20001,
    ecEqual,
    ecLess,
    ecGreater,
    ecRelBack       = ecGreater + 1
};

enum ETokenType
{
    ecTokenTypeFront		= 10001,
    ecTokenText,
    ecTokenSpace,           // sequence of break chars
    ecTokenBreakChars       = ecTokenSpace,
    ecTokenTab,             // sequence of tabs
    ecTokenDiacritics,
	ecTokenMeta,	        // e.g., XML tag
    ecTokenPunctuation,     // sequence of punct. marks i.e. ; or ?!...
    ecTokenEndOfParagraph,  // B6 hex
    ecTokenRegexMatch,
    ecTokenTypeBack			= ecTokenRegexMatch + 1
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

};	// struct StToken

struct StBreakChars
{
    const wchar_t * m_szDefault;
    wchar_t m_szVolatile[cuiMaxSeparatorLength_];

    StBreakChars() : m_szDefault (szDefaultBreakChars_) {}
};

struct StTabs
{
    const wchar_t * m_szDefault;
    wchar_t m_szVolatile[cuiMaxSeparatorLength_];

    StTabs() : m_szDefault (szDefaultTabs_) {}
};

struct StPunctuation
{
    const wchar_t * m_szDefault;
    wchar_t m_szVolatile[cuiMaxSeparatorLength_];

    StPunctuation() : m_szDefault (szDefaultPunctuation_) {}
};

struct StEscape
{
    const wchar_t * m_szDefault;
    wchar_t m_szVolatile[cuiMaxSeparatorLength_];

    StEscape() : m_szDefault (szDefaultEscapeChars_) {}
};

template<typename SeparatorPolicy>
class CSeparators : public SeparatorPolicy
{
public:
    CSeparators()
    {
        m_szVolatile[0] = L'\0';
    }

    void Set (const wchar_t * szSeparators)
    {
        unsigned int uiLength = wcslen (szSeparators);
        if (uiLength > cuiMaxSeparatorLength_)
        {
            wchar_t * szMsg = L"Separator string too long.";
            ERROR_LOG (szMsg);
            throw CException (E_INVALIDARG, szMsg);
        }

        if (0 == uiLength)
        {
            wchar_t * szMsg = L"Empty separator string.";
            ERROR_LOG (szMsg);
            throw CException (E_INVALIDARG, szMsg);
        }

        errno_t error = wmemcpy_s (m_szVolatile, cuiMaxSeparatorLength_, szSeparators, uiLength);
	    if (error)
	    {
            wchar_t * szMsg = L"wmemcpy_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
	    }
    
    }   //  void Set (...)

    const wchar_t * szGet()
    {
        return m_szVolatile[0] ? m_szVolatile : m_szDefault;
    }
};

class  CEString
{

public:


private:

    wchar_t * m_szData;

    unsigned int m_uiLength;
    unsigned int m_uiBlocksAllocated;

	wchar_t m_szVowels[cuiMaxVowelsLength_];
//    wchar_t m_szBreakChars[cuiMaxSeparatorLength_];
    
    CSeparators<StBreakChars> m_Breaks;
    CSeparators<StTabs> m_Tabs;
    CSeparators<StPunctuation> m_Punctuation;
    CSeparators<StEscape> m_Escape;

    wchar_t m_szTabs[cuiMaxSeparatorLength_];
    wchar_t m_szPunctuation[cuiMaxSeparatorLength_];
    wchar_t m_szEscapeChars[cuiMaxSeparatorLength_];
    wchar_t m_szRegex[cuiMaxRegexLength_];

    vector<StToken> m_vecTokens;
    vector<StToken> m_vecRegexMatches;

    bool m_bInvalid;


private:
/*
    void SetSeparators (const wchar_t * szSource, wchar_t * szTarget)
    {
        unsigned int uiLength = wcslen (szTarget);
        if (uiLength > cuiMaxSeparatorLength_)
        {
            wchar_t * szMsg = L"Separator string too long.";
            ERROR_LOG (szMsg);
            throw CException (E_INVALIDARG, szMsg);
        }

        errno_t uiRet = wmemcpy_s (szTarget, cuiMaxSeparatorLength_, szSource, uiLength);
        if (0 != uiRet)
        {
            wchar_t * szMsg = L"wmemcpy_s error.";
            ERROR_LOG (szMsg);
            throw CException (uiRet, szMsg);
        }
    }

    const wchar_t * szGetBreakChars()
    {
        if (L'\0' == m_szBreakChars[0])
        {
            return szDefaultBreakChars_;
        }

        return m_szBreakChars;
    }

    const wchar_t * szGetTabs()
    {
        if (L'\0' == m_szTabs[0])
        {
            return szDefaultTabs_;
        }

        return m_szTabs;
    }

    const wchar_t * szGetPunctuation()
    {
        if (L'\0' == m_szPunctuation[0])
        {
            return szDefaultPunctuation_;
        }

        return m_szPunctuation;
    }

    const wchar_t * szGetEscapeChars()
    {
        if (L'\0' == m_szEscapeChars[0])
        {
            return szDefaultEscapeChars_;
        }

        return m_szEscapeChars;
    }
*/

public:

    CEString() : m_uiLength(0), m_uiBlocksAllocated (1), m_bInvalid (true)
    {
        m_szData = new wchar_t[uiAllocationUnit_];
        m_szData[0] = L'\0';
    }

    CEString (unsigned int uiBlocks) : m_uiLength(0), m_uiBlocksAllocated (uiBlocks), m_bInvalid (true)
    {
        unsigned int uiSize = m_uiBlocksAllocated * uiAllocationUnit_;
        if (uiSize > cuiMaxSize_)
        {
            wchar_t * szMsg = L"Requested size exceeds maximum allowed.";
            ERROR_LOG (szMsg);
            throw CException (E_INVALIDARG, szMsg);
        }
        m_szData = new wchar_t[m_uiBlocksAllocated * uiAllocationUnit_];
        m_szData[0] = L'\0';
    }

    CEString (const CEString& coSource) : 
        m_szData (NULL), 
        m_uiLength (coSource.m_uiLength), 
        m_uiBlocksAllocated (coSource.m_uiBlocksAllocated),
        m_bInvalid (coSource.m_bInvalid)
    {
        if (coSource.m_uiLength > cuiMaxSize_ || 
            coSource.m_uiLength >= coSource.m_uiBlocksAllocated * uiAllocationUnit_)
        {
            wchar_t * szMsg = L"Source string too long.";
            ERROR_LOG (szMsg);
            throw CException (E_INVALIDARG, szMsg);
        }

        m_szData = new wchar_t[m_uiBlocksAllocated * uiAllocationUnit_];
        errno_t error = wmemmove_s (m_szData, m_uiLength, coSource.m_szData, m_uiLength); 
	    if (error)
	    {
            wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
	    }

        m_szData[m_uiLength] = L'\0';

        if (!m_bInvalid)
        {
            m_vecTokens = coSource.m_vecTokens;
        }

        m_vecRegexMatches = coSource.m_vecRegexMatches;
    
    }   //  Copy ctor

    CEString (const wchar_t * szSource) : m_szData(NULL), m_uiBlocksAllocated(0), m_bInvalid (true)
    {
        m_uiLength = wcslen (szSource);
        if (m_uiLength > cuiMaxSize_)
        {
            wchar_t * szMsg = L"Source string too long.";
            ERROR_LOG (szMsg);
            throw CException (E_INVALIDARG, szMsg);
        }

        Assign (szSource, m_uiLength);
    
    }   //  CEString (const wchar_t *)

    virtual ~CEString()
    {
        delete[] m_szData;
    }


public:
/*
    void SetBreakChars (const wchar_t * szBreakChars)
    {
        SetSeparators (szBreakChars, m_szBreakChars);
    }

    void SetPunctuation (const wchar_t * szPunctuation)
    {
        SetSeparators (szPunctuation, m_szPunctuation);
    }

    void SetEscapeChars (const wchar_t * szEscapeChars)
    {
        SetSeparators (szEscapeChars, m_szEscapeChars);
    }

    void SetTabs (const wchar_t * szTabs)
    {
        SetSeparators (szTabs, m_szTabs);
    }
*/

    static ERelation eCompare (const wchar_t * szLeft, const wchar_t * szRight)
    {
        if (0 == szLeft && 0 == szRight)
        {
            return ecEqual;
        }

        if (0 == szLeft)
        {
            ecLess;
        }

        if (0 == szRight)
        {
            return ecGreater;
        }

        for (int iAt = 0; iAt < cuiMaxSize_; ++iAt)
        {
            if (L'\0' == szLeft[iAt] && L'\0' == szLeft[iAt])
            {
                return ecEqual;
            }
            if (L'\0' == szLeft[iAt])
            {
                return ecLess;
            }

            if (L'\0' == szRight[iAt])
            {
                return ecGreater;
            }

            if (szLeft[iAt] < szRight[iAt])
            {
                return ecLess;
            }
            if (szLeft[iAt] > szRight[iAt])
            {
                return ecGreater;
            }
        }

        wchar_t * szMsg = L"Arguments too long.";
        ERROR_LOG (szMsg);
        throw CException (E_INVALIDARG, szMsg);

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


    //
    // Operators
    //

    operator wchar_t *() const
    {
        return m_szData;
    }

    CEString& operator= (const wchar_t * szRhs)
    {
        unsigned int uiSourceLength = wcslen (szRhs);
        Assign (szRhs, uiSourceLength);

        return *this;
    
    }   //  operator= 

    CEString& operator= (const CEString& sRhs)
    {
        if (&sRhs == this)
        {
            return *this;
        }

        m_uiBlocksAllocated = sRhs.m_uiBlocksAllocated;
        m_uiLength = sRhs.m_uiLength;
        Assign (sRhs.m_szData, m_uiLength);
        
        return *this;
    
    }   //  operator= 

    CEString& operator+= (const wchar_t * szRhs)
    {
        unsigned int uiRhsLength = wcslen (szRhs);
        unsigned int uiNewSize = uiRhsLength + m_uiLength;
        if (uiNewSize > cuiMaxSize_)
        {
            wchar_t * szMsg = L"Right-hand side string too long.";
            ERROR_LOG (szMsg);
            throw CException (E_INVALIDARG, szMsg);
        }

        Concatenate (szRhs, uiRhsLength);

        return *this;

    }   //  operator+=

    CEString& operator+= (const CEString& sRhs)
    {
        unsigned int uiNewSize = sRhs.m_uiLength + m_uiLength;
        if (uiNewSize > cuiMaxSize_)
        {
            wchar_t * szMsg = L"Right-hand side string too long.";
            ERROR_LOG (szMsg);
            throw CException (E_INVALIDARG, szMsg);
        }

        Concatenate (sRhs.m_szData, sRhs.m_uiLength);

        return *this;

    }   //  operator+=

    //
    // Methods
    //

    bool bIsEmpty()
    {
        return (0 == m_uiLength);
    }

    unsigned int uiLength()
    {
        return m_uiLength;
    }

    void TrimLeft (const wchar_t * szCharsToTrim)
    {
        if (m_uiLength >= (m_uiBlocksAllocated * uiAllocationUnit_))
        {
            wchar_t * szMsg = L"Illegal length.";
            ERROR_LOG (szMsg);
            throw CException (E_UNEXPECTED, szMsg);
        }

        if (L'\0' != m_szData[m_uiLength])
        {
            wchar_t * szMsg = L"Malformed string.";
            ERROR_LOG (szMsg);
            throw CException (E_UNEXPECTED, szMsg);
        }

        unsigned int uiAt = 0;
        for (; uiAt < m_uiLength; ++uiAt)
        {
            if (L'\0' == m_szData[uiAt])
            {
                wchar_t * szMsg = L"Malformed string.";
                ERROR_LOG (szMsg);
                throw CException (E_UNEXPECTED, szMsg);
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
        errno_t error = wmemmove_s (m_szData, m_uiLength, &m_szData[uiAt], m_uiLength); 
        if (error)
        {
            wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }
        m_szData[m_uiLength] = L'\0';
    
    }       // TrimLeft (...)

    void TrimLeft()
    {
        TrimLeft (m_Breaks.szGet());
    }

    void TrimRight (const wchar_t * szCharsToTrim)
    {
        if (m_uiLength >= m_uiBlocksAllocated * uiAllocationUnit_)
        {
            wchar_t * szMsg = L"Illegal length.";
            ERROR_LOG (szMsg);
            throw CException (E_UNEXPECTED, szMsg);
        }

        if (L'\0' != m_szData[m_uiLength])
        {
            wchar_t * szMsg = L"Malformed string.";
            ERROR_LOG (szMsg);
            throw CException (E_UNEXPECTED, szMsg);
        }

        int iAt = (int)m_uiLength-1;
        for (; iAt >= 0; --iAt)
        {
            if (L'\0' == m_szData[iAt])
            {
                wchar_t * szMsg = L"Malformed string.";
                ERROR_LOG (szMsg);
                throw CException (E_UNEXPECTED, szMsg);
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
        
        Shrink();

    }   //  void TrimRight()

    void TrimRight()
    {
        TrimRight (m_Breaks.szGet());
    }

    void Trim (wchar_t * szCharsToTrim)
    {
        TrimLeft (szCharsToTrim);
        TrimRight (szCharsToTrim);
    }

    void Trim()
    {
        TrimLeft (m_Breaks.szGet());
        TrimRight (m_Breaks.szGet());
    }

    bool bIn (const wchar_t chr, const wchar_t * szSearchSet)
    {
        if (m_uiLength < 1)
        {
            return false;
        }

        for (int iAt = 0; iAt < 1000; ++iAt)
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

        wchar_t * szMsg = L"Search set too long.";
        ERROR_LOG (szMsg);
        throw CException (E_INVALIDARG, szMsg);
    
    }   //  bIn

    CEString sSubstr (unsigned int uiOffset, unsigned int uiLength)
    {
        if (uiLength >= cuiMaxSize_)
        {
            wchar_t * szMsg = L"Source string too long.";
            ERROR_LOG (szMsg);
            throw CException (E_INVALIDARG, szMsg);
        }

        if (L'\0' != m_szData[m_uiLength])
        {
            wchar_t * szMsg = L"Source string not null-terminated.";
            ERROR_LOG (szMsg);
            throw CException (E_UNEXPECTED, szMsg);
        }

        if (uiOffset >= m_uiLength)
        {
            wchar_t * szMsg = L"Invalid offset.";
            ERROR_LOG (szMsg);
            throw CException (E_UNEXPECTED, szMsg);
        }

        if (uiOffset + uiLength > m_uiLength)
        {
            wchar_t * szMsg = L"Invalid length.";
            ERROR_LOG (szMsg);
            throw CException (E_UNEXPECTED, szMsg);
        }

        unsigned int uiBlocksToAllocate = ((uiLength+1)/uiAllocationUnit_) + 1;

        CEString sResult (uiBlocksToAllocate);
        errno_t error = wmemmove_s (sResult.m_szData, uiLength, &m_szData[uiOffset], uiLength); 
	    if (error)
	    {
            wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
	    }
        
        sResult.m_szData[m_uiLength] = L'\0';
        sResult.m_uiLength = uiLength;

        return sResult;

    }   //  CEString sSubstr (...)

//    int i_CompareNoCase (const std::wstring& str) const;

    CEString sGetField (int iAt, ETokenType eType = ecTokenText)
    {
// TODO exc handling
        vector<StToken>::iterator itToken = itFindToken (iAt, eType);
        if (m_vecTokens.end() == itToken)
	    {
            wchar_t * szMsg = L"Failed to find token.";
            ERROR_LOG (szMsg);
            throw CException (E_FAIL, szMsg);
	    }

        return sSubstr ((*itToken).uiOffset, (*itToken).uiLength);
    
    }   // sGetField (...)


    StToken stGetField (int iAt, ETokenType eType = ecTokenText)
    {
// TODO exc handling
        vector<StToken>::iterator itToken = itFindToken (iAt, eType);
        if (m_vecTokens.end() == itToken)
	    {
            wchar_t * szMsg = L"Failed to find token.";
            ERROR_LOG (szMsg);
            throw CException (E_FAIL, szMsg);
	    }

        return *itToken;
    
    }   //  stGetField (...)

//    ST_Token st_GetFieldFromOffset (int i_offset,
//                                    et_TokenType eo_type = ec_TokenText);

    StToken stGetTokenFromOffset (int iOffset)
    {
        vector<StToken>::iterator it_ = itTokenFromOffset (iOffset);
        return (*it_);
    }

    ETokenType eGetTokenType (unsigned int uiAt)
    {
        Tokenize();

        if (uiAt >= m_vecTokens.size())
        {
            wchar_t * szMsg = L"Token index out of range.";
            ERROR_LOG (szMsg);
            throw CException (E_FAIL, szMsg);
        }

        return m_vecTokens[uiAt].eType;
    
    }   //  eGetTokenType (...)

//    et_TokenType eo_GetTokenType (int i_offset, int i_at);

    StToken stGetToken (unsigned int uiAt)
    {
        Tokenize();

        if (uiAt >= m_vecTokens.size())
        {
            wchar_t * szMsg = L"Token index out of range.";
            ERROR_LOG (szMsg);
            throw CException (E_FAIL, szMsg);
        }

        return m_vecTokens[uiAt];
    
    }   //  stGetToken (...)

    const StToken& rstGetToken (unsigned int uiAt)
    {
        Tokenize();

        if (uiAt >= m_vecTokens.size())
        {
            wchar_t * szMsg = L"Token index out of range.";
            ERROR_LOG (szMsg);
            throw CException (E_FAIL, szMsg);
        }

        return m_vecTokens[uiAt];
    
    }   //  const StToken& rstGetToken (...)

    CEString sGetToken (unsigned int uiAt)
    {
        Tokenize();

        if (uiAt >= m_vecTokens.size())
        {
            wchar_t * szMsg = L"Token index out of range.";
            ERROR_LOG (szMsg);
            throw CException (E_FAIL, szMsg);
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
            throw CException (E_FAIL, szMsg);
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
            wchar_t * szMsg = L"Token not found.";
            ERROR_LOG (szMsg);
            throw CException (E_FAIL, szMsg);
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
            wchar_t * szMsg = L"Token not found.";
            ERROR_LOG (szMsg);
            throw CException (E_FAIL, szMsg);
        }

        return it_ - m_vecTokens.begin();
    
    }   //  unsigned int uiGetTokenNum (...)

    unsigned int uiGetNumOfFields (ETokenType eType = ecTokenText)
    {
        Tokenize();

        int iTokens = 0;
        vector<StToken>::iterator it_ = m_vecTokens.begin();
        for (; it_ != m_vecTokens.end(); ++it_)
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
            throw CException (E_FAIL, sMsg);
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
        return m_vecTokens.size();
    }

    unsigned int uiNTokens()
    {
        Tokenize();
        return m_vecTokens.size();
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
        Tokenize();

        if (uiAt >= m_vecTokens.size())
        {
            wchar_t * szMsg = L"Token index out of range.";
            ERROR_LOG (szMsg);
            throw CException (E_FAIL, szMsg);
        }

        vector<StToken>::iterator it = m_vecTokens.begin();
        for (; it != m_vecTokens.end(); ++it)
        {
            if ((*it).eType == eType)
            {
                if (distance (m_vecTokens.begin(), it) >= (int)uiAt)
                {
                    break;
                }
            }
        }
        
        if (m_vecTokens.end() == it)
        {
            wchar_t * szMsg = L"Token not found.";
            ERROR_LOG (szMsg);
            throw CException (E_FAIL, szMsg);
        }

        return (*it).uiLength;

    }   //  uiGetFieldLength (...)

    //unsigned int uiGetFieldLength (unsigned int uiOffset, 
    //                               unsigned int uiAt,
    //                               ETokenType eType = ecTokenText)
    //{
    //}

    void RegexMatch (const wchar_t * szRegex)
    {
        RegexEvaluate (szRegex, true);
    }

    void RegexSearch (const wchar_t * szRegex)
    {
        RegexEvaluate (szRegex, false);
    }

// TODO RegexReplace?

////////////////////////////////////////////////////////////////////////////////////////////


private:

//    void Null()
//    {
//        m_szRegex[0] = L'\0';
//	    m_szVowels[0] = L'\0';
//        m_szBreakChars[0] = L'\0';
//        m_szTabs[0] = L'\0';
//        m_szPunctuation[0] = L'\0';
//        m_szEscapeChars[0] = L'\0';
//    }

    void Grow (unsigned int uiCharsToAdd)
    {
        wchar_t * szNewBuffer = new wchar_t[m_uiLength + uiCharsToAdd];
	    errno_t error = wmemmove_s (szNewBuffer, m_uiLength, m_szData, m_uiLength); 
	    if (error)
	    {
            wchar_t * szMsg = L"wmemmove_s failed.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
	    }
        szNewBuffer[m_uiLength] = L'\0';

        delete[] m_szData;
        m_szData = szNewBuffer;
    
    }   //  Grow()

    void Shrink()
    {
        unsigned int uiBlocksToFree = ((m_uiBlocksAllocated * uiAllocationUnit_) - m_uiLength) / uiAllocationUnit_;
        if (uiBlocksToFree < 1)
        {
            return;
        }

        m_uiBlocksAllocated -= uiBlocksToFree;
        wchar_t * szNewBuffer = new wchar_t[m_uiBlocksAllocated * uiAllocationUnit_];
	    errno_t error = wmemmove_s (szNewBuffer, m_uiLength, m_szData, m_uiLength); 
	    if (error)
	    {
            wchar_t * szMsg = L"wmemmove_s failed.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
	    }
        szNewBuffer[m_uiLength] = L'\0';

        delete[] m_szData;
        m_szData = szNewBuffer;
    
    }   //  Grow()

    void Concatenate (const wchar_t * szRhs, unsigned int uiRhsLength)
    {
        unsigned int uiNewLength = m_uiLength + uiRhsLength;
        if (uiNewLength > cuiMaxSize_)
        {
            wchar_t * szMsg = L"Right-hand side string too long.";
            ERROR_LOG (szMsg);
            throw CException (E_INVALIDARG, szMsg);
        }

        if (uiNewLength >= m_uiBlocksAllocated * uiAllocationUnit_)
        {
            m_uiBlocksAllocated = (uiNewLength/uiAllocationUnit_) + 1;
            wchar_t * szNewData = new wchar_t[m_uiBlocksAllocated * uiAllocationUnit_];
            errno_t error = wmemmove_s (szNewData, m_uiLength, m_szData, m_uiLength); 
	        if (error)
	        {
                wchar_t * szMsg = L"wmemmove_s error.";
                ERROR_LOG (szMsg);
                throw CException (error, szMsg);
	        }
            delete[] m_szData;
            m_szData = szNewData;
        }

        errno_t error = wmemmove_s (&m_szData[m_uiLength], uiRhsLength, szRhs, uiRhsLength); 
        if (error)
        {
            wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
        }

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
        for (unsigned int uiAt = 0; uiAt < m_uiBlocksAllocated * uiAllocationUnit_; ++uiAt)
        {
            wchar_t chrCurrent = m_szData[uiAt];

            if (L'\0' == chrCurrent)
            {
                if (uiAt != m_uiLength)
                {
                    m_bInvalid = true;
                    wchar_t * szMsg = L"Unexpected NULL character.";
                    ERROR_LOG (szMsg);
                    throw CException (E_UNEXPECTED, szMsg);
                }

                if (0 == uiAt)
                {
                    return;
                }

                if (ecTokenTypeFront == stToken.eType)
                {
                    m_bInvalid = true;
                    wchar_t * szMsg = L"Illegal token type.";
                    ERROR_LOG (szMsg);
                    throw CException (E_UNEXPECTED, szMsg);
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

                ATLASSERT(m_uiLength > 0);
                if (uiAt >= m_uiLength-1)
                {
                    m_bInvalid = true;
                    wchar_t * szMsg = L"Unexpected escape character.";
                    ERROR_LOG (szMsg);
                    throw CException (E_UNEXPECTED, szMsg);
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
        wchar_t * szMsg = L"Tokenizer failed.";
        ERROR_LOG (szMsg);
        throw CException (E_UNEXPECTED, szMsg);

    }   //  Tokenize_ (...)

    void Advance (ETokenType eType, unsigned int uiOffset, StToken& stToken)
    {
        if (eType <= ecTokenTypeFront || eType > ecTokenTypeBack)
        {
            wchar_t * szMsg = L"Unexpected token state.";
            ERROR_LOG (szMsg);
            throw CException (E_UNEXPECTED, szMsg);
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
            wchar_t * szMsg = L"Unexpected token state.";
            ERROR_LOG (szMsg);
            throw CException (E_UNEXPECTED, szMsg);
        }

        if (ecTokenTypeFront != stToken.eType)
        {
            m_vecTokens.push_back (stToken);
        }

        stToken.eType = ecTokenMeta;
        stToken.uiOffset = uiOffset;
        stToken.uiLength = 0;

        for (int iAt = uiOffset + 1; iAt < (int)(m_uiBlocksAllocated * uiAllocationUnit_); ++iAt)
        {
            if (L'\0' == m_szData[iAt])
            {
                wchar_t * szMsg = L"Unterminated escape sequence.";
                ERROR_LOG (szMsg);
                throw CException (E_UNEXPECTED, szMsg);
            }

            if (bIn (m_szData[iAt], m_Escape.szGet()))
            {
                if (uiOffset+1 == iAt)
                {
                    wchar_t * szMsg = L"Empty escape sequence.";
                    ERROR_LOG (szMsg);
                    throw CException (E_UNEXPECTED, szMsg);
                }
                stToken.uiLength = iAt - uiOffset + 1;
                m_vecTokens.push_back (stToken);
                break;
            }
        }
    }       //  AddTag (...)

    // Assign null-terminated string erasing current contents
    // uiSourceLength does not count file '0'
    void Assign (const wchar_t * szSource, unsigned int uiSourceLength)
    {
        if (uiSourceLength >= cuiMaxSize_)
        {
            wchar_t * szMsg = L"Source string too long.";
            ERROR_LOG (szMsg);
            throw CException (E_INVALIDARG, szMsg);
        }

        if (L'\0' != szSource[uiSourceLength])
        {
            wchar_t * szMsg = L"Source string not null-terminated.";
            ERROR_LOG (szMsg);
            throw CException (E_UNEXPECTED, szMsg);
        }

        if (uiSourceLength >= (m_uiBlocksAllocated * uiAllocationUnit_))
        {
            m_uiBlocksAllocated = ((uiSourceLength+1)/uiAllocationUnit_) + 1;
            if (m_szData)
            {
                delete[] m_szData;
            }
            m_szData = new wchar_t[uiAllocationUnit_ * m_uiBlocksAllocated];
        }

        errno_t error = wmemmove_s (m_szData, uiSourceLength, szSource, uiSourceLength); 
	    if (error)
	    {
            wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
	    }

        m_szData[uiSourceLength] = L'\0';
        m_uiLength = uiSourceLength;
    
    }   //  Assign (...)

    vector<StToken>::iterator itFindToken (unsigned int uiAt, ETokenType eType = ecTokenText)
    {
        Tokenize();

        if (uiAt >= m_vecTokens.size())
        {
            wchar_t * szMsg = L"Token position out of range.";
            ERROR_LOG (szMsg);
            throw CException (E_UNEXPECTED, szMsg);
        }

        unsigned int uiField = 0;
        vector<StToken>::iterator it_ = m_vecTokens.begin();
        for (; it_ != m_vecTokens.end(); ++it_)
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

        if (m_vecTokens.end() == it_)
        {
            wchar_t * szMsg = L"Token position out of range.";
            ERROR_LOG (szMsg);
            throw CException (E_UNEXPECTED, szMsg);
        }

        return it_;

    }   //  uiFindToken (...)

    vector<StToken>::iterator itTokenFromOffset (unsigned int uiOffset)
    {
        Tokenize();

        vector<StToken>::iterator it_ = m_vecTokens.begin();
        for (; it_ != m_vecTokens.end(); ++it_)
        {
            if ((*it_).uiOffset > uiOffset)
            {
                break;
            }
        }

        if (m_vecTokens.begin() == it_)
        {
            wchar_t * szMsg = L"Failed to find token.";
            ERROR_LOG (szMsg);
            throw CException (E_FAIL, szMsg);
        }

        return --it_;

    }   //  itTokenFromOffset (...)

    void RegexEvaluate (const wchar_t * szRegex, bool bMatchMode = true)
    {
        unsigned int uiRegexLength = wcslen (szRegex);
        if (uiRegexLength > cuiMaxRegexLength_)
        {
            wchar_t * szMsg = L"Regular expression too long.";
            ERROR_LOG (szMsg);
            throw CException (E_INVALIDARG, szMsg);
        }

        errno_t error = wmemmove_s (m_szRegex, uiRegexLength, szRegex, uiRegexLength); 
	    if (error)
	    {
            wchar_t * szMsg = L"wmemmove_s error.";
            ERROR_LOG (szMsg);
            throw CException (error, szMsg);
	    }

        m_szRegex[uiRegexLength] = L'\0';   

        try
        {
            tr1::wregex regex_ (m_szRegex);
            tr1::wsmatch match_;
            if (bMatchMode)
            {
                tr1::regex_match (wstring (m_szData), match_, regex_);
            }
            else
            {
                tr1::regex_search (wstring (m_szData), match_, regex_);
            }

            m_vecRegexMatches.clear();
            for (unsigned int uiAt = 1; uiAt < match_.size(); ++uiAt)
            {
                StToken stToken;
                stToken.eType = ecTokenRegexMatch;
                stToken.uiOffset = static_cast <int> (match_.position (uiAt));
                stToken.uiLength = match_.length (uiAt);
                m_vecRegexMatches.push_back (stToken);
            }
        }
        catch (tr1::regex_error rxError_)
        {
            wstring str_msg (L"Regex error: ");
            str_msg += str_RegexError_ (rxError_.code());
            ERROR_LOG (str_msg);
// TODO: rethrow as native exc?
        }    
    }   //  RegexEvaluate (...)

    wstring str_RegexError_ (tr1::regex_constants::error_type eo_errCode)
    {
        wstring str_error;
        switch (eo_errCode)
        {
            case tr1::regex_constants::error_badbrace:
            {
                str_error = L"error_badbrace -- ";
                str_error += L"the expression contained an invalid count in a { } expression";
                break;
            }
            case tr1::regex_constants::error_badrepeat:
            {
                str_error = L"error_badrepeat -- ";
                str_error += L"a repeat expression (one of '*', '?', '+', '{' ";
                str_error += L"in most contexts) was not preceded by an expression";
                break;
            }
            case tr1::regex_constants::error_brace:
            {
                str_error = L"error_brace -- the expression contained an unmatched '{' or '}'";
                break;
            }
            case tr1::regex_constants::error_brack:
            {
                str_error = L"error_brack -- the expression contained an unmatched '[' or ']'"; 
                break;
            }
            case tr1::regex_constants::error_collate:
            {
                str_error = L"error_collate -- ";
                str_error += L"the expression contained an invalid collating element name";
                break;
            }
            case tr1::regex_constants::error_complexity:
            {
                str_error = L"error_complexity -- an attempted match failed because it was too complex";
                break;
            }
            case tr1::regex_constants::error_ctype:
            {
                str_error = L"error_ctype -- the expression contained an invalid character class name";
                break;
            }
            case tr1::regex_constants::error_escape:
            {
                str_error = L"error_escape -- the expression contained an invalid escape sequence";
                break;
            }
            case tr1::regex_constants::error_paren:
            {
                str_error = L"error_paren -- the expression contained an unmatched '(' or ')'";
                break;
            }
            case tr1::regex_constants::error_range:
            {
                str_error = L"error_range -- ";
                str_error += L"the expression contained an invalid character range specifier";
                break;
            }
            case tr1::regex_constants::error_space:
            {
                str_error = L"error_space -- ";
                str_error += L"parsing a regular expression failed because there were not enough ";
                str_error += L"resources available";
                break;
            }
            case tr1::regex_constants::error_stack:
            {
                str_error = L"error_stack -- ";
                str_error += L"an attempted match failed because there was not enough memory available";
                break;
            }
            case tr1::regex_constants::error_backref:
            {
                str_error = L"error_backref -- the expression contained an invalid back reference";
                break;
            }
            case tr1::regex_constants::error_parse:
            {
                str_error = L"error_parse";
                break;
            }
            case tr1::regex_constants::error_syntax:
            {
                str_error = L"error_syntax";
                break;
            }
            default:
            {
                str_error = L"Unknown error";
            }
        }
            return str_error;

    }   //  str_RegexError_ (...)


};   //  class  CEString

const CEString operator+ (const CEString& sLhs, const wchar_t * szRhs)
{
    return CEString (sLhs) += szRhs;
}

const CEString operator+ (const wchar_t * szLhs, const CEString& sRhs)
{
    return CEString (szLhs) += sRhs;
}

const CEString operator+ (const CEString& sLhs, const CEString& sRhs)
{
    return CEString (sLhs) += sRhs;
}

bool operator== (const CEString& sLhs, const wchar_t * szRhs)
{
    return ecEqual == CEString::eCompare (sLhs, szRhs);
}

bool operator== (const wchar_t * szLhs, const CEString& sRhs)
{
    return ecEqual == CEString::eCompare (szLhs, sRhs);
}

bool operator== (const CEString& sLhs, const CEString& sRhs)
{
    return ecEqual == CEString::eCompare (sLhs, sRhs);
}

bool operator!= (const CEString& sLhs, const wchar_t * szRhs)
{
    return ecEqual != CEString::eCompare (sLhs, szRhs);
}

bool operator!= (const wchar_t * szLhs, const CEString& sRhs)
{
    return ecEqual != CEString::eCompare (szLhs, sRhs);
}

bool operator!= (const CEString& sLhs, const CEString& sRhs)
{
    return ecEqual != CEString::eCompare (sLhs, sRhs);
}

bool operator< (const CEString& sLhs, const wchar_t * szRhs)
{
    return ecLess == CEString::eCompare (sLhs, szRhs);
}

bool operator< (const wchar_t * szLhs, const CEString& sRhs)
{
    return ecLess == CEString::eCompare (szLhs, sRhs);
}

bool operator< (const CEString& sLhs, const CEString& sRhs)
{
    return ecLess == CEString::eCompare (sLhs, sRhs);
}

bool operator> (const CEString& sLhs, const wchar_t * szRhs)
{
    return ecGreater == CEString::eCompare (sLhs, szRhs);
}

bool operator> (const wchar_t * szLhs, const CEString& sRhs)
{
    return ecGreater == CEString::eCompare (szLhs, sRhs);
}

bool operator> (const CEString& sLhs, const CEString& sRhs)
{
    return ecLess == CEString::eCompare (sLhs, sRhs);
}

bool operator<= (const CEString& sLhs, const wchar_t * szRhs)
{
    ERelation eRet = CEString::eCompare (sLhs, szRhs);
    return (ecLess == eRet || ecEqual == eRet);
}

bool operator<= (const wchar_t * szLhs, const CEString& sRhs)
{
    ERelation eRet = CEString::eCompare (szLhs, sRhs);
    return (ecLess == eRet || ecEqual == eRet);
}

bool operator<= (const CEString& sLhs, const CEString& sRhs)
{
    ERelation eRet = CEString::eCompare (sLhs, sRhs);
    return (ecLess == eRet || ecEqual == eRet);
}

bool operator>= (const CEString& sLhs, const wchar_t * szRhs)
{
    ERelation eRet = CEString::eCompare (sLhs, szRhs);
    return (ecGreater == eRet || ecEqual == eRet);
}

bool operator>= (const wchar_t * szLhs, const CEString& sRhs)
{
    ERelation eRet = CEString::eCompare (szLhs, sRhs);
    return (ecGreater == eRet || ecEqual == eRet);
}

bool operator>= (const CEString& sLhs, const CEString& sRhs)
{
    ERelation eRet = CEString::eCompare (sLhs, sRhs);
    return (ecGreater == eRet || ecEqual == eRet);
}
