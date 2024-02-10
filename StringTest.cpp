#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING


#include <stdlib.h>
#include "Logging.h"
#include "EString.h"
#include "Exception.h"

using namespace Hlib;

int main()
{
    bool bErrors{ false };

    CEString* pHeapTest = NULL;
    pHeapTest = new CEString(L"test");
    delete pHeapTest;

    wstring sReplaceableB(L"0123456789012345");
    wstring sReplacedB = sReplaceableB.replace(5, 6, L"abcd");

    CEString sReplacableC(L"0123456789012345678901234567890123567890");
    CEString sReplacedCC = sReplacableC.sReplace((wchar_t*)L"567890", (wchar_t*)L"abcd");
    CEString sReplacedC = sReplacableC.sReplace(5, 6, (wchar_t*)L"abcd");

    sReplaceableB = L"0123456789";
    sReplacedB = sReplaceableB.replace(5, 3, L"a");

    sReplaceableB = L"0123456789";
    sReplacedB = sReplaceableB.replace(8, 2, L"ab");

    sReplaceableB = L"0123456789";
    sReplacedB = sReplaceableB.replace(5, 3, L"a");

    sReplaceableB = L"0123456789";
    sReplacedB = sReplaceableB.replace(8, 2, L"abc");

    wstring sEraseableB(L"0123456789");
    wstring sErasedB = sEraseableB.erase(5, 3);

    sEraseableB = L"0123456789";
    sErasedB = sEraseableB.erase(5, 5);

    sEraseableB = L"0123456789";
    sErasedB = sEraseableB.erase(5, 7);

    sEraseableB = L"0123456789";
    sErasedB = sEraseableB.erase(5);

    sEraseableB = L"0123456789";
    sErasedB = sEraseableB.erase();

    try
    {
        sEraseableB = L"0123456789";
        sErasedB = sEraseableB.erase(12, 7);
    }
    catch (...)
    {
    }

    try
    {
        sEraseableB = L"0123456789";
        sErasedB = sEraseableB.erase(12);
        bErrors = true;     // Should be rest in exc. handler
    }
    catch (...)
    {
        bErrors = false;
        //        ERROR_LOG(L"Exception expected");
    }

    // Ctors
    CEString sEmptyString;
    if (0 != sEmptyString.uiLength() || 0 != sEmptyString.uiGetNumOfTokens() ||
        0 != sEmptyString.uiGetNumOfFields() || 0 != sEmptyString.uiGetVisibleLength())
    {
        bErrors = true;
        ERROR_LOG(L"Initialization error");
    }


    CEString sCopy(sEmptyString);
    if (0 != sCopy.uiLength() || 0 != sCopy.uiGetNumOfTokens() ||
        0 != sCopy.uiGetNumOfFields() || 0 != sCopy.uiGetVisibleLength())
    {
        bErrors = true;
        ERROR_LOG(L"Initialization error");
    }

    sCopy = L"0123456789";
    CEString sCopy2(sCopy);

    sCopy2.SetBreakChars(L" -/");
    CEString sCopy3(sCopy2);

    CEString sFromCString(L"0123456789");
    if (10 != sFromCString.uiLength() || 1 != sFromCString.uiGetNumOfTokens() ||
        1 != sFromCString.uiGetNumOfFields() || 10 != sFromCString.uiGetVisibleLength())
    {
        bErrors = true;
        ERROR_LOG(L"Initialization error");
    }

    // TODO: operator ()

    CEString sSquareBracketsTest(L"0123456789");
    sSquareBracketsTest[1] = L'a';
    if (sSquareBracketsTest != L"0a23456789")
    {
        bErrors = true;
        ERROR_LOG(L"Square brackets operator error");
    }

    sSquareBracketsTest = L"0123456789";
    sSquareBracketsTest[0] = L'a';
    if (sSquareBracketsTest != L"a123456789")
    {
        bErrors = true;
        ERROR_LOG(L"Square brackets operator error");
    }

    sSquareBracketsTest = L"0123456789";
    sSquareBracketsTest[9] = L'a';
    if (sSquareBracketsTest != L"012345678a")
    {
        bErrors = true;
        ERROR_LOG(L"Square brackets operator error");
    }

    sSquareBracketsTest = L"0123456789";
    CEString sLetter = sSquareBracketsTest[1];
    if (L"1" != sLetter)
    {
        bErrors = true;
        ERROR_LOG(L"Square brackets operator error");
    }

    // Comparison
    ERelation eRet = CEString::eCompare(L"1234567", L"1234567");
    if (ecEqual != eRet)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    eRet = CEString::eCompare(L"1234567", L"1234566");
    if (eRet != ecGreater)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    eRet = CEString::eCompare(L"1234566", L"1234567");
    if (eRet != ecLess)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    eRet = CEString::eCompare(L"123456", L"1234567");
    if (eRet != ecLess)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    eRet = CEString::eCompare(L"1234567", L"123456");
    if (eRet != ecGreater)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    eRet = CEString::eCompareNoCase(L"AbCdEfG", L"ABCDEFg");
    if (ecEqual != eRet)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    eRet = CEString::eCompareNoCase(L"АбВгДЕ", L"АБВГДе");
    if (ecEqual != eRet)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    bool bRet = CEString::bIn(L'2', L"0123456789");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bIn() failed.");
    }

    bRet = CEString::bIn(L'a', L"0123456789");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bIn() failed.");
    }

    bRet = CEString::bStringOverAlphabet(L"adj", L"abcdefghj");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStringOverAlphabet() failed.");
    }

    bRet = CEString::bStringOverAlphabet(L"абвпрояё", L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStringOverAlphabet() failed.");
    }

    bRet = CEString::bStringOverAlphabet(L"abcopqxyz", L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStringOverAlphabet() failed.");
    }

    bRet = CEString::bStringOverAlphabet(L"123", L"abcdefghj");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStringOverAlphabet() failed.");
    }

    CEString sSearcheable(L"0123456789");
    unsigned int uiFindRet = sSearcheable.uiFind(L"123");
    if (1 != uiFindRet)
    {
        bErrors = true;
        ERROR_LOG(L"uiFind() failed.");
    }

    uiFindRet = sSearcheable.uiFind(L"abc");
    if (ecNotFound != uiFindRet)
    {
        bErrors = true;
        ERROR_LOG(L"uiFind() failed.");
    }

    sSearcheable = L"aBcDeFЮяgHiJ";
    uiFindRet = sSearcheable.uiFindNoCase(L"bCDEFюЯ");
    if (ecNotFound == uiFindRet)
    {
        bErrors = true;
        ERROR_LOG(L"uiFindNoCase() failed.");
    }

    sSearcheable = L"012345543210";
    uiFindRet = sSearcheable.uiRFind(L"5");
    if (6 != uiFindRet)
    {
        bErrors = true;
        ERROR_LOG(L"uiRFind() failed.");
    }

    //    unsigned int uiRFindNoCase (const wchar_t * szRhs) const

    sSearcheable = L"0123456789";
    uiFindRet = sSearcheable.uiFindFirstOf(L"234");
    if (2 != uiFindRet)
    {
        bErrors = true;
        ERROR_LOG(L"uiFindFirstOf() failed.");
    }

    //    unsigned int uiFindFirstOfNoCase (const wchar_t * szSet) const
    sSearcheable = L"0120120123456789";
    uiFindRet = sSearcheable.uiFindOneOf(3, L"234");
    if (5 != uiFindRet)
    {
        bErrors = true;
        ERROR_LOG(L"uiFindOneOf() failed.");
    }

    sSearcheable = L"0123456789";
    uiFindRet = sSearcheable.uiFindLastOf(L"234");
    if (4 != uiFindRet)
    {
        bErrors = true;
        ERROR_LOG(L"uiFindLastOf() failed.");
    }

    //    unsigned int uiFindLastOfNoCase (const wchar_t * szSet) const
    bRet = sSearcheable.bStartsWith(L"012");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStartsWith() failed.");
    }

    bRet = sSearcheable.bStartsWith(L"234");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStartsWith() failed.");
    }

    sSearcheable = L"aBcDeFgHiJ";
    bRet = sSearcheable.bStartsWithNoCase(L"abcd");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStartsWithNoCase() failed.");
    }

    sSearcheable = L"0123456789";
    bRet = sSearcheable.bStartsWithOneOf(L"012");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStartsWithOneOf() failed.");
    }

    bRet = sSearcheable.bStartsWithOneOf(L"123");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStartsWithOneOf() failed.");
    }

    sSearcheable = L"aBcDeFgHiJ";
    bRet = sSearcheable.bStartsWithOneOfNoCase(L"abc");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStartsWithOneOf() failed.");
    }

    bRet = sSearcheable.bStartsWithOneOfNoCase(L"bc");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStartsWithOneOf() failed.");
    }

    sSearcheable = L"аБвГдЕёжзи";
    bRet = sSearcheable.bStartsWithOneOfNoCase(L"абв");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStartsWithOneOf() failed.");
    }

    bRet = sSearcheable.bStartsWithOneOfNoCase(L"бв");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStartsWithOneOf() failed.");
    }

    sSearcheable = L"0123456789";
    bRet = sSearcheable.bEndsWith(L"789");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStartsWith() failed.");
    }

    bRet = sSearcheable.bEndsWith(L"123");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bStartsWith() failed.");
    }

    sSearcheable = L"abcdeFgHiJ";
    bRet = sSearcheable.bEndsWithNoCase(L"hij");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bEndsWithNoCase() failed.");
    }

    bRet = sSearcheable.bEndsWithNoCase(L"ghi");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bEndsWithNoCase() failed.");
    }

    sSearcheable = L"абвгдЕёЖзИ";
    bRet = sSearcheable.bEndsWithNoCase(L"жзи");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bEndsWithNoCase() failed.");
    }

    bRet = sSearcheable.bEndsWithNoCase(L"ёжз");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bEndsWithNoCase() failed.");
    }

    sSearcheable = L"0123456789";
    bRet = sSearcheable.bEndsWithOneOf(L"ab9");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bEndsWithOneOf() failed.");
    }

    bRet = sSearcheable.bEndsWithOneOf(L"ab8");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bEndsWithOneOf() failed.");
    }

    sSearcheable = L"aBcDeFgHiJ";
    bRet = sSearcheable.bEndsWithOneOfNoCase(L"abj");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bEndsWithOneOfNoCase failed.");
    }

    bRet = sSearcheable.bEndsWithOneOfNoCase(L"abc");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bEndsWithOneOfNoCase failed.");
    }

    sSearcheable = L"абвгдЕёЖзИ";
    bRet = sSearcheable.bEndsWithOneOfNoCase(L"abи");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bEndsWithOneOfNoCase failed.");
    }

    bRet = sSearcheable.bEndsWithOneOfNoCase(L"abc");
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"bEndsWithOneOfNoCase failed.");
    }

    // Operators
    CEString sLhs(L"01234");
    CEString sRhs(L"56789");

    bRet = (sLhs == sRhs);
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    bRet = (sLhs == L"01234");
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    bRet = (L"01234" == sLhs);
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    //    CString csLhs (L"01234");
    //    CString csRhs (L"56789");
    //    bRet = (L"01234" == csLhs);
    //    if (!bRet)
    //    {
    //        ERROR_LOG(L"CString behavior does not match CEString behavior");
    //    }

    bRet = (sLhs < sRhs);
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    bRet = (sLhs > sRhs);
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    bRet = (sLhs <= sRhs);
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    bRet = (sLhs >= sRhs);
    if (bRet)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }

    bRet = (sLhs >= sLhs);
    if (!bRet)
    {
        bErrors = true;
        ERROR_LOG(L"Comparison error");
    }


    sEmptyString = L"0123456";
    if (sEmptyString != L"0123456")
    {
        bErrors = true;
        ERROR_LOG(L"Assignemnt or comparison error");
    }

    sEmptyString = sRhs;
    if (sEmptyString != sRhs)
    {
        bErrors = true;
        ERROR_LOG(L"Assignemnt or comparison error");
    }

    //CEString sResult = sLhs + sRhs;
    //if (sResult != L"0123456789")
    //{
    //    ERROR_LOG(L"Concatenation or comparison error");
    //}
    //sResult += L"<--Concatenated";
    //if (sResult != L"0123456789<--Concatenated")
    //{
    //    ERROR_LOG(L"Concatenation or comparison error");
    //}

    CEString sInsertable(L"0123789");
    CEString sInserted = sInsertable.sInsert(4, L"456");
    if (sInserted != sInsertable || sInsertable != L"0123456789")
    {
        bErrors = true;
        ERROR_LOG(L"Insertion error");
    }

    sInsertable = L"012456789";
    sInserted = sInsertable.sInsert(3, L'3');
    if (sInserted != sInsertable || sInsertable != L"0123456789")
    {
        bErrors = true;
        ERROR_LOG(L"Insertion error");
    }

    CEString sErasable(L"012abcd3456789");
    CEString sErased = sErasable.sErase(3, 4);
    if (sErased != sErasable || sErasable != L"0123456789")
    {
        bErrors = true;
        ERROR_LOG(L"Erase error");
    }

    sErasable = L"0123456789";
    sErased = sErasable.sErase(3, 7);
    if (sErased != sErasable || sErasable != L"012")
    {
        bErrors = true;
        ERROR_LOG(L"Erase error");
    }

    sErasable = L"0123456789";
    sErased = sErasable.sErase(3, 40);
    if (sErased != sErasable || sErasable != L"012")
    {
        bErrors = true;
        ERROR_LOG(L"Erase error");
    }

    sErasable = L"0123456789";
    sErased = sErasable.sErase(3);
    if (sErased != sErasable || sErasable != L"012")
    {
        bErrors = true;
        ERROR_LOG(L"Erase error");
    }

    sErasable = L"0123456789a";
    sErased = sErasable.sErase(10);
    if (sErased != sErasable || sErasable != L"0123456789")
    {
        bErrors = true;
        ERROR_LOG(L"Erase error");
    }

    sErasable.Erase();
    if (!sErasable.bIsEmpty() || sErasable.uiLength() != 0)
    {
        bErrors = true;
        ERROR_LOG(L"Erase error");
    }

    sErasable = L"0123456789";

    CEString sConvertToUppercase(L"aAbBcC");
    sConvertToUppercase.ToUpper();
    if (sConvertToUppercase != L"AABBCC")
    {
        bErrors = true;
        ERROR_LOG(L"ToUpper error");
    }

    sConvertToUppercase = CEString::sToUpper(L"aAbBcC");
    if (sConvertToUppercase != L"AABBCC")
    {
        bErrors = true;
        ERROR_LOG(L"ToUpper error");
    }

    CEString sConvertToUppercaseCyr(L"aABbcCаАбБвВ");
    sConvertToUppercaseCyr.ToUpper();
    if (sConvertToUppercaseCyr != L"AABBCCААББВВ")
    {
        bErrors = true;
        ERROR_LOG(L"ToUpper error for Cyrillic");
    }

    sConvertToUppercaseCyr = CEString::sToUpper(L"aAbBcCаАбБвВ");
    if (sConvertToUppercaseCyr != L"AABBCCААББВВ")
    {
        bErrors = true;
        ERROR_LOG(L"sToUpper error for Cyrillic");
    }

    CEString sConvertToLowercase(L"aABbcC");
    sConvertToLowercase.ToLower();
    if (sConvertToLowercase != L"aabbcc")
    {
        bErrors = true;
        ERROR_LOG(L"ToLower error");
    }

    sConvertToLowercase = CEString::sToLower(L"aAbBcC");
    if (sConvertToLowercase != L"aabbcc")
    {
        bErrors = true;
        ERROR_LOG(L"ToLower error");
    }

    CEString sConvertToLowercaseCyr(L"aABbcCаАбБвВ");
    sConvertToLowercaseCyr.ToLower();
    if (sConvertToLowercaseCyr != L"aabbccааббвв")
    {
        bErrors = true;
        ERROR_LOG(L"ToLower error for Cyrillic");
    }

    sConvertToLowercaseCyr = CEString::sToLower(L"aAbBcCаАбБвВ");
    if (sConvertToLowercaseCyr != L"aabbccааббвв")
    {
        bErrors = true;
        ERROR_LOG(L"sToLower error for Cyrillic");
    }

    CEString sFromAscii = CEString::sToString("abcdefgxyzABCDEFGXYZ01234567890.,!");
    if (sFromAscii != L"abcdefgxyzABCDEFGXYZ01234567890.,!")
    {
        bErrors = true;
        ERROR_LOG(L"sToString error for ascii conversion");
    }

    CEString sReplaceable(L"01abcd6789");
    CEString sReplaced = sReplaceable.sReplace(2, (wchar_t*)L"2345");
    if (sReplaced != sReplaceable || sReplaceable != L"0123456789")
    {
        bErrors = true;
        ERROR_LOG(L"Replace error");
    }

    sReplaceable = L"0123456abc";
    sReplaced = sReplaceable.sReplace(7, (wchar_t*)L"789");
    if (sReplaced != sReplaceable || sReplaceable != L"0123456789")
    {
        bErrors = true;
        ERROR_LOG(L"Replace error");
    }

    sReplaceable = L"0123456a89";
    sReplaced = sReplaceable.sReplace(7, L'7');
    if (sReplaced != sReplaceable || sReplaceable != L"0123456789")
    {
        bErrors = true;
        ERROR_LOG(L"Replace error");
    }

    sReplaceable = L"012345678a";
    sReplaced = sReplaceable.sReplace(9, L'9');
    if (sReplaced != sReplaceable || sReplaceable != L"0123456789")
    {
        bErrors = true;
        ERROR_LOG(L"Replace error");
    }


    sReplaceable = L"01234abc89";
    sReplaced = sReplaceable.sReplace(5, 3, (wchar_t*)L"567");
    if (sReplaced != sReplaceable || sReplaceable != L"0123456789")
    {
        bErrors = true;
        ERROR_LOG(L"Replace error");
    }

    sReplaceable = L"01234aaa6789";
    sReplaced = sReplaceable.sReplace(5, 3, (wchar_t*)L"5");
    if (sReplaced != sReplaceable || sReplaceable != L"0123456789")
    {
        bErrors = true;
        ERROR_LOG(L"Replace error");
    }

    sReplaceable = L"01234567ab";
    sErased = sReplaceable.sReplace(8, 2, (wchar_t*)L"89");
    if (sReplaced != sReplaceable || sReplaceable != L"0123456789")
    {
        bErrors = true;
        ERROR_LOG(L"Replace error");
    }

    sReplaceable = L"01234567a9";
    sReplaced = sReplaceable.sReplace(8, 2, (wchar_t*)L"8");
    if (sReplaced != sReplaceable || sReplaceable != L"012345678")
    {
        bErrors = true;
        ERROR_LOG(L"Replace error");
    }

    sReplaceable = L"01234567ab";
    sReplaced = sReplaceable.sReplace(8, 2, (wchar_t*)L"890");
    if (sReplaced != sReplaceable || sReplaceable != L"01234567890")
    {
        bErrors = true;
        ERROR_LOG(L"Replace error");
    }

    sReplaceable = L"0ё2345ё78ёё";
    sReplaceable.Replace(0, 10, L'ё', L'е');
    if (sReplaceable != L"0е2345е78ее")
    {
        bErrors = true;
        ERROR_LOG(L"Replace error");
    }

    CEString sTrimmable(L" 01234     ");
    sTrimmable.TrimLeft();
    if (sTrimmable != L"01234     ")
    {
        bErrors = true;
        ERROR_LOG(L"Trim or comparison error");
    }

    sTrimmable.TrimRight();
    if (sTrimmable != L"01234")
    {
        bErrors = true;
        ERROR_LOG(L"Trim or comparison error");
    }

    sTrimmable = L"     01234     ";
    sTrimmable.Trim();
    if (sTrimmable != L"01234")
    {
        bErrors = true;
        ERROR_LOG(L"Trim or comparison error");
    }

    sTrimmable = L"=&=&=01234&&&==";
    sTrimmable.TrimLeft(L"=&");
    if (sTrimmable != L"01234&&&==")
    {
        bErrors = true;
        ERROR_LOG(L"Trim or comparison error");
    }

    sTrimmable.Trim(L"=&");
    if (sTrimmable != L"01234")
    {
        bErrors = true;
        ERROR_LOG(L"Trim or comparison error");
    }

    sTrimmable = L"=&=&=01234&&&==";
    sTrimmable.Trim(L"=&");
    if (sTrimmable != L"01234")
    {
        bErrors = true;
        ERROR_LOG(L"Trim or comparison error");
    }

    CEString sReversable(L"0123456789");
    sReversable.Reverse();
    if (sReversable != L"9876543210")
    {
        bErrors = true;
        ERROR_LOG(L"Reversing error");
    }

    CEString sWhole(L"0123456789");
    CEString sSubstr = sWhole.sSubstr(1, 3);
    if (sSubstr != L"123")
    {
        bErrors = true;
        ERROR_LOG(L"Trim or comparison error");
    }

    sSubstr = sWhole.sSubstr(7);
    if (sSubstr != L"789")
    {
        bErrors = true;
        ERROR_LOG(L"Trim or comparison error");
    }

    CEString sFields(L"123 456 789");
    sFields.SetBreakChars(L" ");
    CEString sField = sFields.sGetField(1);
    if (sField != L"456")
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    StToken stToken = sFields.stGetField(0);
    if (3 != stToken.uiLength || 0 != stToken.uiOffset || ecTokenText != stToken.eType)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    stToken = sFields.stGetField(0, ecTokenSpace);
    if (1 != stToken.uiLength || 3 != stToken.uiOffset || ecTokenSpace != stToken.eType)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }
    try
    {
        //        stToken = sFields.stGetField (99);
        //        ERROR_LOG(L"Tokenizer or comparison error");   // Exception expected
    }
    catch (CException& ex)
    {
        bErrors = true;
        ERROR_LOG(ex.szGetDescription());
    }

    //    ST_Token st_GetFieldFromOffset (int i_offset,
    //                                    et_TokenType eo_type = ec_TokenText);

    stToken = sFields.stGetTokenFromOffset(6);
    if (3 != stToken.uiLength || 4 != stToken.uiOffset || ecTokenText != stToken.eType)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    ETokenType eType = sFields.eGetTokenType(1);
    //    et_TokenType eo_GetTokenType (int i_offset, int i_at);
    if (ecTokenBreakChars != eType)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    stToken = sFields.stGetToken(1);
    if (1 != stToken.uiLength || 3 != stToken.uiOffset || ecTokenSpace != stToken.eType)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    const StToken& rstToken = sFields.rstGetToken(1);
    if (1 != rstToken.uiLength || 3 != rstToken.uiOffset || ecTokenSpace != rstToken.eType)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    CEString sToken = sFields.sGetToken(1);
    if (sToken != L" ")
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    try
    {
        CEString sToken1 = sFields.sGetToken(999);
    }
    catch (CException& ex)
    {
        CEString sMsg(L"Exception handled correctly");
        //        sMsg += ex.szGetDescription();
        //        sMsg += L" handled correctly.";
        ERROR_LOG(sMsg);
    }

    bool b_ = sFields.bGetNextToken(stToken);
    if (!b_ || ecTokenText != stToken.eType || 4 != stToken.uiOffset || 3 != stToken.uiLength)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    b_ = sFields.bGetPrevToken(stToken);
    if (!b_ || ecTokenBreakChars != stToken.eType || 3 != stToken.uiOffset || 1 != stToken.uiLength)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    unsigned int uiTokenNum = sFields.uiGetTokenNum(stToken);
    if (1 != uiTokenNum)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    unsigned int uiFields = sFields.uiGetNumOfFields();
    if (3 != uiFields)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    uiFields = sFields.uiGetNumOfFields(ecTokenSpace);
    if (2 != uiFields)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    //    uiFields = sFields.uiGetNumOfFields (3, 6);

    uiFields = sFields.uiNFields();
    if (3 != uiFields)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    //    uiFields = sFields.uiNFields (3, 6); 

    unsigned int uiTokens = sFields.uiGetNumOfTokens();
    if (5 != uiTokens)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    uiTokens = sFields.uiNTokens();
    if (5 != uiTokens)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    unsigned int uiVLength = sFields.uiGetVisibleLength();
    if (11 != uiVLength)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    unsigned int uiFLength = sFields.uiGetFieldLength(1);
    if (3 != uiFLength)
    {
        bErrors = true;
        ERROR_LOG(L"Tokenizer or comparison error");
    }

    //    CEString s (L"abcdefg");
    //    wchar_t * szData = (wchar_t*)s;

    CEString sSyllables(L"бавогузюы");
    sSyllables.SetVowels(L"аеёиоуыэюя");
    unsigned int uiSyllables = sSyllables.uiGetNumOfSyllables();
    if (5 != uiSyllables)
    {
        bErrors = true;
        ERROR_LOG(L"Syllable count error");
    }

    uiSyllables = sSyllables.uiNSyllables();
    if (5 != uiSyllables)
    {
        bErrors = true;
        ERROR_LOG(L"Syllable count error");
    }


    // Vowels & consonants
    unsigned int uiVowelPos = sSyllables.uiGetVowelPos(3);
    if (7 != uiVowelPos)
    {
        bErrors = true;
        ERROR_LOG(L"Vowel position error");
    }

    unsigned int uiSyllPos = sSyllables.uiGetSyllableFromVowelPos(7);
    if (3 != uiSyllPos)
    {
        bErrors = true;
        ERROR_LOG(L"Syllable position error");
    }

    {
        CEString sConvert = CEString::sToString(9999999999999);
        if (L"9999999999999" != sConvert)
        {
            bErrors = true;
            ERROR_LOG(L"Large int conversion error");
        }
        int i_ = 999999;
        sConvert = CEString::sToString(i_);
        if (L"999999" != sConvert)
        {
            bErrors = true;
            ERROR_LOG(L"Int conversion error");
        }
    }

    {
        CEString sConvert = CEString::sToString(999999999.9999);
        double d_ = 999999.999;
        sConvert = CEString::sToString(d_);
    }

    {
        CEString sConvert(L"12345");
        int i_ = CEString::iToInt(sConvert);
        if (i_ != 12345)
        {
            bErrors = true;
            ERROR_LOG(L"EString --> int conversion error");
        }
    }

    {
        CEString sConvert = L"abcdefg";
        try
        {
            [[maybe_unused]] int i_ = CEString::iToInt(sConvert);
            bErrors = true;
            ERROR_LOG(L"Expected EString --> int exception not thrown.")
        }
        catch (CException& e)
        {
            if (e.iGetErrorCode() != H_ERROR_INVALID_ARG)
            {
                bErrors = true;
                ERROR_LOG(L"Expected EString --> int exception not thrown.");
            }
        }
    }

    CEString sConvert(L"999999999999999999");
    try
    {
        [[maybe_unused]] int i_ = CEString::iToInt(sConvert);
        bErrors = true;
        ERROR_LOG(L"Expected EString --> int exception not thrown.")
    }
    catch (CException& e)
    {
        if (e.iGetErrorCode() != H_ERROR_INVALID_ARG)
        {
            bErrors = true;
            ERROR_LOG(L"Expected EString --> int exception not thrown.");
        }
    }

    {
        CEString sRangeLoopTest(L"0123456789");
        wstring s_;
        for (auto c_ : sRangeLoopTest)
        {
            s_ += c_;
        }
        if (wstring(L"0123456789") != s_)
        {
            bErrors = true;
            ERROR_LOG(L"Int conversion error");
        }
    }

    {
        CEString sMoveSource(L"abcdefgабвгдежABCDEFGАБВГДЕЖ0123456789");
        auto sSourceCopy(sMoveSource);      // save a copy so we can compare it to the target after move
        auto sMoveTarget = std::move(sMoveSource);
        if (sSourceCopy != sMoveTarget)
        {
            bErrors = true;
            ERROR_LOG(L"Move error");
        }
    }

    //
    // Done!
    //
    //CLogger::pGetInstance()->Flush();

    if (!bErrors)
    {
        std::wcout << L"\n*** OK\n";
    }
    else
    {
        std::wcout << L"\n*** Test failed\n";
    }

    //_CrtDumpMemoryLeaks();


}
