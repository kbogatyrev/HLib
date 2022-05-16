#ifndef ST_LEXEME_HASHER_INCLUDED
#define ST_LEXEME_HASHER_INCLUDED

#include <map>
#include "SqliteWrapper.h"
#include "MD5.h"

namespace Hlib
{

struct StLexemeHasher
{
    map<int, ET_StressType> m_mapStress;
    CEString m_sSourceForm;
    CEString m_sMainSymbol;
    int m_iInflectionType;
    int m_iAccentType1;
    int m_iAccentType2;
    CEString m_sComment;

    StLexemeHasher() : m_iInflectionType(-1), m_iAccentType1(-1), m_iAccentType2(-1)
    {}

    CEString sHash()
    {
        CEString sText (m_sSourceForm);

        map<int, ET_StressType>::iterator itStress = m_mapStress.begin();
        for (; itStress != m_mapStress.end(); ++ itStress)
        {
            unsigned char * pchrStress = (unsigned char *)&(*itStress).first;
            sText += 32 + pchrStress[0];
            pchrStress = (unsigned char *)&(*itStress).second;
            sText += 32 + pchrStress[0];
        }

        sText += m_sMainSymbol;

        unsigned char * pchrAt = (unsigned char *)&m_iInflectionType;
        sText += 32 + pchrAt[0];

        pchrAt = (unsigned char *)&m_iAccentType1;
        sText += 32 + pchrAt[0];

        pchrAt = (unsigned char *)&m_iAccentType2;
        sText += 32 + pchrAt[0];

        sText += m_sComment;

        CMD5 md5;
        return md5.sHash (sText);

    }   // sHash()

    bool bSaveToDb (CSqlite * pDbHandle, int64_t llDescriptorId, int64_t llInflectionId)
    {
        try
        {
            pDbHandle->PrepareForInsert (L"lexeme_hash_to_descriptor", 3);
            pDbHandle->Bind (1, sHash());
            pDbHandle->Bind (2, llDescriptorId);
            pDbHandle->Bind (3, llInflectionId);
            pDbHandle->InsertRow();
            pDbHandle->Finalize();
        }
        catch (CException& exc)
        {
            CEString sMsg (exc.szGetDescription());
            CEString sError;
            try
            {
                pDbHandle->GetLastError (sError);
                sMsg += CEString (L", error %d: ");
                sMsg += sError;
            }
            catch (...)
            {
                sMsg = L"Apparent DB error ";
            }
        
            sMsg += CEString::sToString(pDbHandle->iGetLastError());
            ERROR_LOG (sMsg);

            return false;
        }

        return true;

    }   //  bool bSaveToDb (...)


};  //  struct StLexemeHasher

}   //  namespace Hlib

#endif
