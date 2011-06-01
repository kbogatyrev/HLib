// StringTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ErrorHandlers.h"
#include "EString.h"

int _tmain(int argc, _TCHAR* argv[])
{

    // Ctors
    CEString sEmptyString;
    if (0 != sEmptyString.uiLength() || 0 != sEmptyString.uiGetNumOfTokens() || 
        0 != sEmptyString.uiGetNumOfFields() || 0 != sEmptyString.uiGetVisibleLength())
    {
        ERROR_LOG (L"Initialization error");
    }

    CEString sSizedString (15); 
    if (0 != sSizedString.uiLength() || 0 != sSizedString.uiGetNumOfTokens() || 
        0 != sSizedString.uiGetNumOfFields() || 0 != sSizedString.uiGetVisibleLength())
    {
        ERROR_LOG (L"Initialization error");
    }

    CEString sCopy (sEmptyString);
    if (0 != sCopy.uiLength() || 0 != sCopy.uiGetNumOfTokens() || 
        0 != sCopy.uiGetNumOfFields() || 0 != sCopy.uiGetVisibleLength())
    {
        ERROR_LOG (L"Initialization error");
    }


    CEString sFromCString (L"0123456789");
    if (10 != sFromCString.uiLength() || 1 != sFromCString.uiGetNumOfTokens() || 
        1 != sFromCString.uiGetNumOfFields() || 10 != sFromCString.uiGetVisibleLength())
    {
        ERROR_LOG (L"Initialization error");
    }

    // Operators
    CEString sLhs (L"01234");
    CEString sRhs (L"56789");
    
    bool bRet = (sLhs == sRhs);
    if (bRet)
    {
        ERROR_LOG (L"Comparison error");
    }

    bRet = (sLhs == L"01234");
    if (!bRet)
    {
        ERROR_LOG (L"Comparison error");
    }

    bRet = (L"01234" == sLhs);
    if (!bRet)
    {
        ERROR_LOG (L"Comparison error");
    }

    bRet = (sLhs < sRhs);
    if (!bRet)
    {
        ERROR_LOG (L"Comparison error");
    }

    bRet = (sLhs > sRhs);
    if (!bRet)
    {
        ERROR_LOG (L"Comparison error");
    }

    bRet = (sLhs <= sRhs);
    if (!bRet)
    {
        ERROR_LOG (L"Comparison error");
    }

    bRet = (sLhs >= sRhs);
    if (bRet)
    {
        ERROR_LOG (L"Comparison error");
    }

    bRet = (sLhs >= sLhs);
    if (!bRet)
    {
        ERROR_LOG (L"Comparison error");
    }


    sEmptyString = L"0123456";
    if (sEmptyString != L"0123456")
    {
        ERROR_LOG (L"Assignemnt or comparison error");
    }

    sEmptyString = sRhs;
    if (sEmptyString != sRhs)
    {
        ERROR_LOG (L"Assignemnt or comparison error");
    }

    CEString sResult = sLhs + sRhs;
    if (sResult != L"0123456789")
    {
        ERROR_LOG (L"Concatenation or comparison error");
    }
    sResult += L"<--Concatenated";
    if (sResult != L"0123456789<--Concatenated")
    {
        ERROR_LOG (L"Concatenation or comparison error");
    }

    CEString sInsertable (L"0123789");
    sInsertable.Insert (4, L"456");
    if (sInsertable != L"0123456789")
    {
        ERROR_LOG (L"Insertion error");
    }

    sInsertable = L"012456789";
    sInsertable.Insert (3, L'3');
    if (sInsertable != L"0123456789")
    {
        ERROR_LOG (L"Insertion error");
    }

    CEString sErasable (L"012abcd3456789");
    sErasable.Erase (3, 4);
    if (sInsertable != L"0123456789")
    {
        ERROR_LOG (L"Erase error");
    }
    
    sErasable = L"012a3456789";
    sErasable.Erase (3);
    if (sInsertable != L"0123456789")
    {
        ERROR_LOG (L"Erase error");
    }

    sErasable.Erase();
    if (!sErasable.bIsEmpty() || sErasable.uiLength() != 0)
    {
        ERROR_LOG (L"Erase error");
    }

    CEString sTrimmable (L"     01234     ");
    sTrimmable.TrimLeft();
    if (sTrimmable != L"01234     ")
    {
        ERROR_LOG (L"Trim or comparison error");
    }

    sTrimmable.TrimRight();
    if (sTrimmable != L"01234")
    {
        ERROR_LOG (L"Trim or comparison error");
    }

    sTrimmable = L"     01234     ";
    sTrimmable.Trim();
    if (sTrimmable != L"01234")
    {
        ERROR_LOG (L"Trim or comparison error");
    }

    sTrimmable = L"=&=&=01234&&&==";
    sTrimmable.TrimLeft (L"=&");
    if (sTrimmable != L"01234&&&==")
    {
        ERROR_LOG (L"Trim or comparison error");
    }

    sTrimmable.Trim (L"=&");
    if (sTrimmable != L"01234")
    {
        ERROR_LOG (L"Trim or comparison error");
    }

    sTrimmable = L"=&=&=01234&&&==";
    sTrimmable.Trim (L"=&");
    if (sTrimmable != L"01234")
    {
        ERROR_LOG (L"Trim or comparison error");
    }

    CEString sWhole (L"0123456789");
    CEString sSubstr = sWhole.sSubstr (1, 3);
    if (sSubstr != L"123")
    {
        ERROR_LOG (L"Trim or comparison error");
    }

    CEString sFields (L"123 456 789");
    CEString sField = sFields.sGetField (1);
    if (sField != L"456")
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    StToken stToken = sFields.stGetField (0);
    if (3 != stToken.uiLength || 0 != stToken.uiOffset || ecTokenText != stToken.eType)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    stToken = sFields.stGetField (0, ecTokenSpace);
    if (1 != stToken.uiLength || 3 != stToken.uiOffset || ecTokenSpace != stToken.eType)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    try
    {
//        stToken = sFields.stGetField (99);
//        ERROR_LOG (L"Tokenizer or comparison error");   // Exception expected
    }
    catch (CException& ex)
    {
//        ::MessageBox (NULL, ex.sGetDescription().c_str(), L"Kai Exception", MB_ICONWARNING);
    }

//    ST_Token st_GetFieldFromOffset (int i_offset,
//                                    et_TokenType eo_type = ec_TokenText);

    stToken = sFields.stGetTokenFromOffset (6);
    if (3 != stToken.uiLength || 4 != stToken.uiOffset || ecTokenText != stToken.eType)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    ETokenType eType = sFields.eGetTokenType (1);
//    et_TokenType eo_GetTokenType (int i_offset, int i_at);
    if (ecTokenBreakChars != eType)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    stToken = sFields.stGetToken (1);
    if (1 != stToken.uiLength || 3 != stToken.uiOffset || ecTokenSpace != stToken.eType)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    const StToken& rstToken = sFields.rstGetToken (1);
    if (1 != stToken.uiLength || 3 != stToken.uiOffset || ecTokenSpace != stToken.eType)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    CEString sToken = sFields.sGetToken (1);
    if (L" " != sToken)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    bool b_ = sFields.bGetNextToken (stToken);
    if (!b_ || ecTokenText != stToken.eType || 4 != stToken.uiOffset || 3 != stToken.uiLength)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    b_ = sFields.bGetPrevToken (stToken);
    if (!b_ || ecTokenBreakChars != stToken.eType || 3 != stToken.uiOffset || 1 != stToken.uiLength)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    unsigned int uiTokenNum = sFields.uiGetTokenNum (stToken);
    if (1 != uiTokenNum)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    unsigned int uiFields = sFields.uiGetNumOfFields();
    if (3 != uiFields)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    uiFields = sFields.uiGetNumOfFields (ecTokenSpace);
    if (2 != uiFields)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

//    uiFields = sFields.uiGetNumOfFields (3, 6);

    uiFields = sFields.uiNFields();
    if (3 != uiFields)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

//    uiFields = sFields.uiNFields (3, 6); 

    unsigned int uiTokens = sFields.uiGetNumOfTokens();
    if (5 != uiTokens)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    uiTokens = sFields.uiNTokens();
    if (5 != uiTokens)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    unsigned int uiVLength = sFields.uiGetVisibleLength();
    if (11 != uiVLength)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    unsigned int uiFLength = sFields.uiGetFieldLength (1);
    if (3 != uiFLength)
    {
        ERROR_LOG (L"Tokenizer or comparison error");
    }

    CEString s (L"abcdefg");
    wchar_t * szData = s;


    CEString sConvert = CEString::sToString (9999999999999);

    unsigned long ul_ = 999999;
    sConvert = CEString::sToString (ul_);

    //
    // Done!
    //
    CError::pGetInstance()->Flush();
}
