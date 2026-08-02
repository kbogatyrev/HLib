#ifndef GRAMHASHER_H_INCLUDED
#define GRAMHASHER_H_INCLUDED

#include <map>
#include "Enums.h"
#include "EString.h"

namespace Hlib
{
    //
    // Initializing static class members at file scope
    //
    static ET_Subparadigm eStrToSubparadigm(const CEString& sKey)
    {
        static const map<CEString, ET_Subparadigm> mapStrToSubparadigm =
        {
            { L"AdjComp", SUBPARADIGM_COMPARATIVE }, { L"AdjL", SUBPARADIGM_LONG_ADJ }, { L"AdjS", SUBPARADIGM_SHORT_ADJ },
            { L"Adv", SUBPARADIGM_ADVERB }, { L"AspectPair", SUBPARADIGM_ASPECT_PAIR }, { L"Conj", SUBPARADIGM_CONJUNCTION }, 
            { L"Impv", SUBPARADIGM_IMPERATIVE }, { L"Inf", SUBPARADIGM_INFINITIVE }, { L"Interj", SUBPARADIGM_INTERJECTION }, 
            { L"Noun", SUBPARADIGM_NOUN }, { L"NumAdj", SUBPARADIGM_NUM_ADJ }, { L"Numeral24", SUBPARADIGM_NUM_2TO4 }, { L"Numeral", SUBPARADIGM_NUM },
            { L"Parenth", SUBPARADIGM_PARENTHESIS }, { L"Particle", SUBPARADIGM_PARTICLE }, { L"Past", SUBPARADIGM_PAST_TENSE }, 
            { L"PPastA", SUBPARADIGM_PART_PAST_ACT }, { L"PPastPL", SUBPARADIGM_PART_PAST_PASS_LONG }, { L"PPastPS", SUBPARADIGM_PART_PAST_PASS_SHORT }, 
            { L"Predic", SUBPARADIGM_PREDICATIVE }, { L"Prep", SUBPARADIGM_PREPOSITION }, { L"PPresA", SUBPARADIGM_PART_PRES_ACT }, 
            { L"PPresPL", SUBPARADIGM_PART_PRES_PASS_LONG }, { L"PPresPS", SUBPARADIGM_PART_PRES_PASS_SHORT }, { L"Pres", SUBPARADIGM_PRESENT_TENSE },
            { L"PronAdj", SUBPARADIGM_PRONOUN_ADJ }, { L"Pronoun", SUBPARADIGM_PRONOUN }, { L"VAdv_Past", SUBPARADIGM_ADVERBIAL_PAST }, 
            { L"VAdv_Pres", SUBPARADIGM_ADVERBIAL_PRESENT }
            
        };

        ET_Subparadigm eSubparadigm = SUBPARADIGM_UNDEFINED;
        try
        {
            if (1 == mapStrToSubparadigm.count(sKey))
            {
                eSubparadigm = mapStrToSubparadigm.at(sKey);
            }
        }
        catch (std::exception& ex)
        {
            CEString sMsg(L"STL exception: ");
            sMsg += CEString::sFromUtf8(ex.what());
            ERROR_LOG(sMsg);
        }

        return eSubparadigm;
    }

    static CEString sSubparadigmToStr(ET_Subparadigm eKey)
    {
        static const map<ET_Subparadigm, CEString> mapSubparadigmToStr =
        {
            { SUBPARADIGM_COMPARATIVE, L"AdjComp" },{ SUBPARADIGM_LONG_ADJ, L"AdjL" }, { SUBPARADIGM_SHORT_ADJ, L"AdjS" }, 
            { SUBPARADIGM_ADVERB, L"Adv" }, { SUBPARADIGM_ASPECT_PAIR, L"AspectPair" }, { SUBPARADIGM_CONJUNCTION, L"Conj" }, 
            { SUBPARADIGM_IMPERATIVE, L"Impv" }, { SUBPARADIGM_INFINITIVE, L"Inf" }, { SUBPARADIGM_INTERJECTION, L"Interj" }, 
            { SUBPARADIGM_NOUN, L"Noun" }, { SUBPARADIGM_NUM_ADJ, L"NumAdj" }, { SUBPARADIGM_NUM_2TO4, L"Numeral24" }, 
            { SUBPARADIGM_NUM, L"Numeral" }, { SUBPARADIGM_LAST_NAME_NOUN, L"LastNameNoun" }, { SUBPARADIGM_LAST_NAME_NOUN_F, L"LastNameNounFeminine" },
            { SUBPARADIGM_LAST_NAME_LONG_ADJ, L"LastNameLongAdj" }, { SUBPARADIGM_LAST_NAME_PRONOUN_ADJ, L"LastNamePronAdj" }, 
            { SUBPARADIGM_PARENTHESIS, L"Parenth" }, { SUBPARADIGM_PARTICLE, L"Particle" }, { SUBPARADIGM_PAST_TENSE, L"Past" }, 
            { SUBPARADIGM_PART_PAST_ACT, L"PPastA" }, { SUBPARADIGM_PART_PAST_PASS_LONG, L"PPastPL" }, 
            { SUBPARADIGM_PART_PAST_PASS_SHORT, L"PPastPS" }, { SUBPARADIGM_PREDICATIVE, L"Predic" }, 
            { SUBPARADIGM_PREPOSITION, L"Prep" }, { SUBPARADIGM_PART_PRES_ACT, L"PPresA" }, 
            { SUBPARADIGM_PART_PRES_PASS_LONG, L"PPresPL" }, { SUBPARADIGM_PART_PRES_PASS_SHORT, L"PPresPS" }, 
            { SUBPARADIGM_PRESENT_TENSE, L"Pres" }, { SUBPARADIGM_PRONOUN_ADJ, L"PronAdj" }, 
            { SUBPARADIGM_PRONOUN, L"Pronoun" }, { SUBPARADIGM_ADVERBIAL_PAST, L"VAdv_Past" }, 
            { SUBPARADIGM_ADVERBIAL_PRESENT, L"VAdv_Pres" }
        };

        CEString sSubparadigm;
        try
        {
            sSubparadigm = mapSubparadigmToStr.at(eKey);
        }
        catch (std::exception& ex)
        {
            CEString sMsg(L"Exception: ");
            sMsg += CEString::sToString(ex.what());
            ERROR_LOG(sMsg);
        }

        return sSubparadigm;
    }

    static ET_Number eStrToNumber(const CEString& sKey)
    {
        static const map<CEString, ET_Number> mapStrToNumber = { { L"Sg", NUM_SG }, { L"Pl", NUM_PL } };

        ET_Number eNumber = NUM_UNDEFINED;
        try
        {
            eNumber = mapStrToNumber.at(sKey);
        }
        catch (std::exception& ex)
        {
            CEString sMsg(L"Exception: ");
            sMsg += CEString::sToString(ex.what());
            ERROR_LOG(sMsg);
        }

        return eNumber;
    }

    static CEString sNumberToStr(ET_Number eKey)
    {
        static const map<ET_Number, CEString> mapNumberToStr = { { NUM_SG, L"Sg" }, { NUM_PL, L"Pl" } };

        CEString sNumber;
        try
        {
            sNumber = mapNumberToStr.at(eKey);
        }
        catch (std::exception& ex)
        {
            CEString sMsg(L"Exception: ");
            sMsg += CEString::sToString(ex.what());
            ERROR_LOG(sMsg);
        }

        return sNumber;
    }

    static ET_Gender eStrToGender(const CEString& sKey)
    {
        static const map<CEString, ET_Gender> mapStrToGender = { { L"M", GENDER_M }, { L"F", GENDER_F }, { L"N", GENDER_N } };

        ET_Gender eGender = GENDER_UNDEFINED;
        try
        {
            eGender = mapStrToGender.at(sKey);
        }
        catch (std::exception& ex)
        {
            CEString sMsg(L"Exception: ");
            sMsg += CEString::sToString(ex.what());
            ERROR_LOG(sMsg);
        }

        return eGender;
    }

    static CEString sGenderToStr(ET_Gender eKey)
    {
        static const map<ET_Gender, CEString> mapGenderToStr = { { GENDER_M, L"M" }, { GENDER_F, L"F" }, { GENDER_N, L"N" }, { GENDER_UNDEFINED, L"" } };

        CEString sGender;
        try
        {
            sGender = mapGenderToStr.at(eKey);
        }
        catch (std::exception& ex)
        {
            CEString sMsg(L"Exception: ");
            sMsg += CEString::sToString(ex.what());
            ERROR_LOG(sMsg);
        }

        return sGender;
    }

    static CEString sAnimacyToStr(ET_Animacy eKey)
    {
        static const map<ET_Animacy, CEString> mapAnimacyToStr = { { ANIM_YES, L"Anim" }, { ANIM_NO, L"Inanim" }, { ANIM_UNDEFINED, L"" } };

        CEString sAnimacy;
        try
        {
            sAnimacy = mapAnimacyToStr.at(eKey);
        }
        catch (std::exception& ex)
        {
            CEString sMsg(L"Exception: ");
            sMsg += CEString::sToString(ex.what());
            ERROR_LOG(sMsg);
        }

        return sAnimacy;
    }

    static ET_Case eStrToCase(const CEString& sKey)
    {
        static const map<CEString, ET_Case> mapStrToCase =
        {
            { L"N", CASE_NOM }, { L"A", CASE_ACC }, { L"G", CASE_GEN }, { L"Part", CASE_PART }, { L"D", CASE_DAT }, { L"I", CASE_INST },
            { L"P", CASE_PREP }, { L"L", CASE_LOC }, { L"Num", CASE_NUM }
        };

        ET_Case eCase = CASE_UNDEFINED;
        try
        {
            eCase = mapStrToCase.at(sKey);
        }
        catch (std::exception& ex)
        {
            CEString sMsg(L"Exception: ");
            sMsg += CEString::sToString(ex.what());
            ERROR_LOG(sMsg);
        }

        return eCase;
    }

    static CEString sCaseToStr(ET_Case eKey)
    {
        static const map<ET_Case, CEString> mapCaseToStr =
        {
            { CASE_NOM, L"N" }, { CASE_ACC, L"A" }, { CASE_GEN, L"G" }, { CASE_PART, L"Part" }, { CASE_DAT, L"D" }, { CASE_INST, L"I" },
            { CASE_PREP, L"P" }, { CASE_LOC, L"L" }, { CASE_NUM, L"Num" }
        };

        CEString sCase;

        try
        {
            sCase = mapCaseToStr.at(eKey);
        }
        catch (std::exception& ex)
        {
            CEString sMsg(L"Exception: ");
            sMsg += CEString::sToString(ex.what());
            ERROR_LOG(sMsg);
        }

        return sCase;
    }

    static ET_Person eStrToPerson(const CEString& sKey)
    {
        static const map<CEString, ET_Person> mapStrToPerson = { { L"1", PERSON_1 }, { L"2", PERSON_2 }, { L"3", PERSON_3 } };

        ET_Person ePerson = PERSON_UNDEFINED;

        try
        {
            ePerson = mapStrToPerson.at(sKey);
        }
        catch (std::exception& ex)
        {
            CEString sMsg(L"Exception: ");
            sMsg += CEString::sToString(ex.what());
            ERROR_LOG(sMsg);
        }

        return ePerson;
    }

    static CEString sPersonToStr(const ET_Person ePerson)
    {
        static const map<ET_Person, CEString> mapPersonToStr = { { PERSON_1, L"1" }, { PERSON_2, L"2" }, { PERSON_3, L"3" } };

        CEString sPerson;
        try
        {
            sPerson = mapPersonToStr.at(ePerson);
        }
        catch (std::exception& ex)
        {
            CEString sMsg(L"Exception: ");
            sMsg += CEString::sToString(ex.what());
            ERROR_LOG(sMsg);
        }

        return sPerson;
    }

    static CEString sPosToStr(const ET_PartOfSpeech ePos)
    {
        static const map<ET_PartOfSpeech, CEString> mapPosToStr =
        {
            { POS_UNDEFINED, L"" }, { POS_ADV, L"Adv" }, { POS_PREPOSITION, L"Prep" }, { POS_CONJUNCTION, L"Conj" }, { POS_PARTICLE, L"Particle" },
            { POS_COMPAR, L"AdjComp" }, { POS_PREDIC, L"Predic" }, { POS_INTERJ, L"Interj" }, { POS_PARENTH, L"Parenth" }
        };

        CEString sPos;
        try
        {
            sPos = mapPosToStr.at(ePos);
        }
        catch (std::exception& ex)
        {
            CEString sMsg(L"Exception: ");
            sMsg += CEString::sToString(ex.what());
            ERROR_LOG(sMsg);
        }

        return sPos;
    }

    class CGramInfo
    {
    public:
        //    CEString sLemma; // For reflexivity -- this is currently disabled

        ET_PartOfSpeech m_ePos;
        ET_Subparadigm m_eSubparadigm;
        ET_Case m_eCase;
        ET_Number m_eNumber;
        ET_Gender m_eGender;
        ET_Person m_ePerson;
        ET_Animacy m_eAnimacy;
        ET_Aspect m_eAspect;
        ET_Reflexivity m_eReflexivity;

    public:
        CGramInfo(ET_PartOfSpeech ePos,
            ET_Subparadigm eSubparadigm,
            ET_Case eCase,
            ET_Number eNumber,
            ET_Gender eGender,
            ET_Person ePerson,
            ET_Animacy eAnimacy,
            ET_Aspect eAspect,
            ET_Reflexivity eReflexivity) :
            m_ePos(ePos),
            m_eSubparadigm(eSubparadigm),
            m_eCase(eCase),
            m_eNumber(eNumber),
            m_eGender(eGender),
            m_ePerson(ePerson),
            m_eAnimacy(eAnimacy),
            m_eAspect(eAspect),
            m_eReflexivity(eReflexivity)
        {}

        void GramClear()
        {
            m_ePos = POS_UNDEFINED;
            m_eSubparadigm = SUBPARADIGM_UNDEFINED;
            m_eCase = CASE_UNDEFINED;
            m_eNumber = NUM_UNDEFINED;
            m_eGender = GENDER_UNDEFINED;
            m_ePerson = PERSON_UNDEFINED;
            m_eAnimacy = ANIM_UNDEFINED;
            m_eAspect = ASPECT_UNDEFINED;
            m_eReflexivity = REFL_UNDEFINED;
        }

        CGramInfo()
        {
            GramClear();
        }

        virtual ~CGramInfo() {}
    };

class CGramHasher : public CGramInfo
{
    enum etSubtype
    {
        ecSubtypeUndefined,
        ecParticiple,
        ecAdverbial
    };

    enum etTense
    {
        ecTenseUndefined,
        ecTensePresent,
        ecTensePast,
        ecTenseFuture
    };

public:
    CGramHasher()
    {
        GramClear();
    }

    CGramHasher (const CEString& sHash)
    {
        ET_ReturnCode rc = H_NO_ERROR;
        GramClear();
        rc = eDecodeHash (sHash);
        if (H_NO_ERROR != rc)
        {
            throw CException (rc, L"CGramHasher::eDecode() failed.");
        }
    }

    CGramHasher (ET_PartOfSpeech ePos, 
                 ET_Subparadigm eSubparadigm, 
                 ET_Case eCase, 
                 ET_Number eNumber, 
                 ET_Gender eGender,
                 ET_Person ePerson,
                 ET_Animacy eAnimacy,
                 ET_Aspect eAspect,
                 ET_Reflexivity eReflexivity) :
        CGramInfo (ePos, eSubparadigm, eCase, eNumber, eGender, ePerson, eAnimacy, eAspect, eReflexivity)
    {}

    CGramHasher (ET_Subparadigm eSubparadigm,   // For verbs
                 ET_Number eNumber, 
                 ET_Gender eGender, 
                 ET_Person ePerson,
                 ET_Animacy eAnimacy,
                 ET_Aspect eAspect,
                 ET_Case eCase,
                 ET_Reflexivity eReflexivity) :
        CGramInfo (POS_VERB, eSubparadigm, eCase, eNumber, eGender, ePerson, eAnimacy, eAspect, eReflexivity)
    {}

    CGramHasher (ET_Gender eGender,         // For nouns
                 ET_Animacy eAnimacy,
                 ET_Case eCase,
                 ET_Number eNumber) :
        CGramInfo (POS_NOUN, 
                   SUBPARADIGM_NOUN, 
                   eCase, 
                   eNumber, 
                   eGender, 
                   PERSON_UNDEFINED, 
                   eAnimacy, 
                   ASPECT_UNDEFINED, 
                   REFL_UNDEFINED)
    {}

    CGramHasher (ET_Subparadigm eSubparadigm, // For adjectives
                 ET_Number eNumber, 
                 ET_Gender eGender, 
                 ET_Animacy eAnimacy,
                 ET_Case eCase) :
        CGramInfo (POS_ADJ, 
                   eSubparadigm, 
                   eCase, 
                   eNumber, 
                   eGender, 
                   PERSON_UNDEFINED, 
                   eAnimacy, 
                   ASPECT_UNDEFINED, 
                   REFL_UNDEFINED)
    {}

    CEString sGramHash()
    {
        if (POS_UNDEFINED == m_ePos || POS_ADV == m_ePos || POS_PREPOSITION == m_ePos ||
            POS_CONJUNCTION == m_ePos || POS_PARTICLE == m_ePos || POS_COMPAR == m_ePos ||
            POS_PREDIC == m_ePos || POS_INTERJ == m_ePos || POS_PARENTH == m_ePos)
        {
            return sPosToStr(m_ePos);
        }

        CEString sHash;
        switch (m_eSubparadigm)
        {
        case SUBPARADIGM_NOUN:
        case SUBPARADIGM_PRONOUN:
            sHash = sSubparadigmToStr(m_eSubparadigm) + L"_" + sNumberToStr(m_eNumber) + L"_" + sCaseToStr(m_eCase);
            break;

        case SUBPARADIGM_LAST_NAME_NOUN:
            sHash = sSubparadigmToStr(m_eSubparadigm);
            if (NUM_SG == m_eNumber)
            {
                sHash += L"_" + sGenderToStr(GENDER_M);
            }
            sHash += L"_" + sNumberToStr(m_eNumber);
            sHash += L"_" + sCaseToStr(m_eCase);
            break;

        case SUBPARADIGM_LAST_NAME_NOUN_F:
            sHash = sSubparadigmToStr(m_eSubparadigm);
            if (NUM_SG == m_eNumber)
            {
                sHash += L"_" + sGenderToStr(GENDER_F);
            }
            sHash += L"_" + sNumberToStr(m_eNumber);
            sHash += L"_" + sCaseToStr(m_eCase);
            break;

        case SUBPARADIGM_NUM:
        case SUBPARADIGM_NUM_2TO4:
            if (m_eGender != GENDER_UNDEFINED)
            {
                sHash = sSubparadigmToStr(m_eSubparadigm) + L"_" + sGenderToStr(m_eGender) + L"_" + sCaseToStr(m_eCase);
                if (ANIM_NO == m_eAnimacy)
                {
                    sHash += L"_Inanim";
                }
                else if (ANIM_YES == m_eAnimacy)
                {
                    sHash += L"_Anim";
                }
            }
            else
            {
                sHash = sSubparadigmToStr(m_eSubparadigm) + L"_" + sCaseToStr(m_eCase);
            }
            break;

        case SUBPARADIGM_LONG_ADJ:
        case SUBPARADIGM_PRONOUN_ADJ:
        case SUBPARADIGM_NUM_ADJ:
        case SUBPARADIGM_PART_PRES_ACT:
        case SUBPARADIGM_PART_PRES_PASS_LONG:
        case SUBPARADIGM_PART_PAST_ACT:
        case SUBPARADIGM_PART_PAST_PASS_LONG:
            sHash = sSubparadigmToStr(m_eSubparadigm);
            if (NUM_SG == m_eNumber)
            {
                sHash += L"_" + sGenderToStr(m_eGender);
            }
            sHash += L"_" + sNumberToStr(m_eNumber);
            sHash += L"_" + sCaseToStr(m_eCase);

            if (m_eCase == CASE_ACC && (m_eNumber == NUM_PL || GENDER_M == m_eGender))
            {
                sHash += L"_" + sAnimacyToStr(m_eAnimacy);
            }
            break;

        case SUBPARADIGM_LAST_NAME_LONG_ADJ:
        case SUBPARADIGM_LAST_NAME_PRONOUN_ADJ:
            sHash = sSubparadigmToStr(m_eSubparadigm);
            if (NUM_SG == m_eNumber)
            {
                sHash += L"_" + sGenderToStr(m_eGender);
            }
            sHash += L"_" + sNumberToStr(m_eNumber);
            sHash += L"_" + sCaseToStr(m_eCase);
            break;

        case SUBPARADIGM_PAST_TENSE:
        case SUBPARADIGM_SHORT_ADJ:
        case SUBPARADIGM_PART_PRES_PASS_SHORT:
        case SUBPARADIGM_PART_PAST_PASS_SHORT:
            sHash = sSubparadigmToStr(m_eSubparadigm) + L"_";
            if (NUM_PL == m_eNumber)
            {
                sHash += sNumberToStr(m_eNumber);
            }
            else
            {
                sHash += sGenderToStr(m_eGender);
            }
            break;

        case SUBPARADIGM_PRESENT_TENSE:
        case SUBPARADIGM_IMPERATIVE:
            sHash = sSubparadigmToStr(m_eSubparadigm) + L"_" + sNumberToStr(m_eNumber) + L"_" + sPersonToStr(m_ePerson);
            break;

        case SUBPARADIGM_ADVERBIAL_PRESENT:
        case SUBPARADIGM_ADVERBIAL_PAST:
        case SUBPARADIGM_INFINITIVE:
        case SUBPARADIGM_COMPARATIVE:
        case SUBPARADIGM_UNDEFINED:
            sHash = sSubparadigmToStr(m_eSubparadigm);
            break;

        default:
            assert(0);
            {
                ERROR_LOG(L"Unable to parse subparadigm.");
                m_eSubparadigm = SUBPARADIGM_UNDEFINED;
                sHash = L"";
            }
        }       //  switch

        return sHash;

    } // sGramHash()

 
    ET_ReturnCode eDecodeHash (const CEString& sHash)
    {
        GramClear();
        CEString sSource(sHash);
        try
        {
            sSource.ResetSeparators();
            sSource.SetBreakChars(L"_");

            if (sSource.uiNFields() < 1)
            {
                CEString sMsg(L"Unable to parse hash: ");
                ERROR_LOG(sMsg + sHash);
                return ET_ReturnCode(H_ERROR_INVALID_ARG);
            }

            if (L"VAdv_Pres" == sSource)
            {
                m_eSubparadigm = SUBPARADIGM_ADVERBIAL_PRESENT;
            }
            else if (L"VAdv_Past" == sSource)
            { 
                m_eSubparadigm = SUBPARADIGM_ADVERBIAL_PAST;
            }
            else
            {
                m_eSubparadigm = eStrToSubparadigm(sSource.sGetField(0));
            }

            m_ePos = Hlib::eSubparadigmToPos(m_eSubparadigm);

            switch (m_eSubparadigm)
            {
                case SUBPARADIGM_NOUN:
                case SUBPARADIGM_PRONOUN:
                {
                    if (sSource.uiNFields() < 3)
                    {
                        CEString sMsg(L"No number and/or case: ");
                        ERROR_LOG(sMsg + sSource);
                        m_eSubparadigm = SUBPARADIGM_UNDEFINED;
                        return ET_ReturnCode(H_ERROR_INVALID_ARG);
                    }

                    m_eNumber = eStrToNumber(sSource.sGetField(1));
                    m_eCase = eStrToCase(sSource.sGetField(2));
                    break;
                }           // case SUBPARADIGM_NOUN

                case SUBPARADIGM_NUM:
                case SUBPARADIGM_NUM_2TO4:
                {
                    m_eGender = GENDER_UNDEFINED;
                    if (sSource.uiNFields() >= 3)
                    {
                        m_eGender = eStrToGender(sSource.sGetField(1));
                        m_eCase = eStrToCase(sSource.sGetField(2));
                    }
                    else if (sSource.uiNFields() >= 2)
                    {
                        m_eCase = eStrToCase(sSource.sGetField(1));
                    }

                    if (sSource.uiNFields() >= 4)
                    {
                        if (sSource.sGetField(3) == L"Inanim")
                        {
                            m_eAnimacy = ANIM_NO;
                        }
                        else if (sSource.sGetField(3) == L"Anim")
                        {
                            m_eAnimacy = ANIM_YES;
                        }
                        else
                        {
                            CEString sMsg(L"Unable to determine animacy: ");
                            ERROR_LOG(sMsg + sSource);
                            m_eSubparadigm = SUBPARADIGM_UNDEFINED;
                            return ET_ReturnCode(H_ERROR_INVALID_ARG);
                        }
                    }
                    break;
                }

                case SUBPARADIGM_LONG_ADJ:                       // e.g., AdjL_M_Sg_D, AdjL_Pl_N
                case SUBPARADIGM_PART_PRES_ACT:                  // e.g., говорящему, PPresA_M_Sg_D
                case SUBPARADIGM_PART_PRES_PASS_LONG:            // e.g., говоримому, PPresPL_M_Sg_D
                case SUBPARADIGM_PART_PAST_ACT:                  // e.g., говорившему, PPastA_M_Sg_D
                case SUBPARADIGM_PART_PAST_PASS_LONG:            // e.g., порождённому, PPastPL_M_Sg_D
                case SUBPARADIGM_PRONOUN_ADJ:                    // e.g., PronAdj_M_Sg_N
                case SUBPARADIGM_NUM_ADJ:                        // e.g., NumAdj_M_Sg_N
                {
                    m_eNumber = NUM_UNDEFINED;
                    m_eGender = GENDER_UNDEFINED;

                    if (sSource.uiNFields() < 3)
                    {
                        CEString sMsg(L"Unable to decode hash.");
                        ERROR_LOG(sMsg + sSource);
                        m_eSubparadigm = SUBPARADIGM_UNDEFINED;
                        return ET_ReturnCode(H_ERROR_INVALID_ARG);
                    }

                    if (L"Pl" == sSource.sGetField(1))
                    {
                        m_eNumber = NUM_PL;
                        m_eCase = eStrToCase(sSource.sGetField(2));
                    }
                    else
                    {
                        if (sSource.uiNFields() < 4)
                        {
                            CEString sMsg(L"Unable to decode hash.");
                            ERROR_LOG(sMsg + sSource);
                            m_eSubparadigm = SUBPARADIGM_UNDEFINED;
                            return ET_ReturnCode(H_ERROR_INVALID_ARG);
                        }

                        m_eNumber = NUM_SG;
                        m_eGender = eStrToGender(sSource.sGetField(1));
                        m_eNumber = eStrToNumber(sSource.sGetField(2));
                        m_eCase = eStrToCase(sSource.sGetField(3));
                    }
                    break;
                }           //  case SUBPARADIGM_LONG_ADJ

                case SUBPARADIGM_PAST_TENSE:
                case SUBPARADIGM_SHORT_ADJ:
                case SUBPARADIGM_PART_PRES_PASS_SHORT:
                case SUBPARADIGM_PART_PAST_PASS_SHORT:
                {
                    if (sSource.uiNFields() < 2)
                    {
                        CEString sMsg(L"Unable to decode hash.");
                        ERROR_LOG(sMsg + sSource);
                        m_eSubparadigm = SUBPARADIGM_UNDEFINED;
                        return ET_ReturnCode(H_ERROR_INVALID_ARG);
                    }

                    m_eNumber = NUM_UNDEFINED;
                    m_eGender = GENDER_UNDEFINED;
                    if (L"Pl" == sSource.sGetField(1))
                    {
                        m_eNumber = NUM_PL;
                    }
                    else
                    {
                        m_eNumber = NUM_SG;
                        m_eGender = eStrToGender(sSource.sGetField(1));
                    }
                    break;
                
                }           //   case SUBPARADIGM_SHORT_ADJ etc

                case SUBPARADIGM_PRESENT_TENSE:
                case SUBPARADIGM_IMPERATIVE:
                {
                    if (sSource.uiNFields() < 3)
                    {
                        CEString sMsg(L"Unable to decode hash.");
                        ERROR_LOG(sMsg + sSource);
                        m_eSubparadigm = SUBPARADIGM_UNDEFINED;
                        return ET_ReturnCode(H_ERROR_INVALID_ARG);
                    }
                    m_eNumber = eStrToNumber(sSource.sGetField(1));
                    m_ePerson = eStrToPerson(sSource.sGetField(2));

                    break;
                
                }       //  SUBPARADIGM_PRESENT_TENSE

                case SUBPARADIGM_ADVERB:
                case SUBPARADIGM_ADVERBIAL_PAST:
                case SUBPARADIGM_ADVERBIAL_PRESENT:
                case SUBPARADIGM_COMPARATIVE:
                case SUBPARADIGM_CONJUNCTION:
                case SUBPARADIGM_INFINITIVE:
                case SUBPARADIGM_INTERJECTION:
                case SUBPARADIGM_PARENTHESIS:
                case SUBPARADIGM_PARTICLE:
                case SUBPARADIGM_PREDICATIVE:
                case SUBPARADIGM_PREPOSITION:
                case SUBPARADIGM_ASPECT_PAIR:
                case SUBPARADIGM_UNDEFINED:
                {
                    break;
                }

                default:
                {
                    CEString sMsg(L"Unknown subparadigm: ");
                    ERROR_LOG(sMsg + sHash);
                    return ET_ReturnCode(H_ERROR_INVALID_ARG);
                }
            }
        }
        catch (CException& exc)
        {
            CEString sMsg(exc.szGetDescription());
            CEString sError;
            try
            {
                sMsg += L", error: ";
            }
            catch (...)
            {
                sMsg = L"Unknown error ";
            }

            ERROR_LOG(sMsg);
            return ET_ReturnCode(H_ERROR_INVALID_ARG);
        }

        return H_NO_ERROR;
    
	}   // eDecodeHash()

    void Initialize (ET_PartOfSpeech ePos, ET_Subparadigm eSp, ET_Gender eInitGender, ET_Animacy eInitAnimacy)    // Nouns
    {
        GramClear();
        m_ePos = ePos;
        m_eSubparadigm = eSp;
        m_eGender = eInitGender;
        m_eAnimacy = eInitAnimacy;
        m_eCase = CASE_NOM;
        m_eNumber = NUM_SG;
        m_eSubparadigm = eSp;
    }

    void Initialize (ET_Aspect eInitAspect, ET_Reflexivity eInitReflexive)    // Verbs
    {
        GramClear();
        m_ePos = POS_VERB;
        m_eAspect = eInitAspect;
        m_eReflexivity = eInitReflexive;

        m_eSubparadigm = SUBPARADIGM_PRESENT_TENSE;
        m_ePerson = PERSON_1;
        m_eNumber = NUM_SG;
    }

    void Initialize (ET_PartOfSpeech eInitPos)    // Adjectives and adjectival pronouns or last names
    {
        GramClear();

        m_ePos = eInitPos; 

        if (POS_LAST_NAME == eInitPos)
        { 
            m_eSubparadigm = SUBPARADIGM_LAST_NAME_LONG_ADJ;
            m_eGender = GENDER_M;
            m_eCase = CASE_NOM;
            m_eNumber = NUM_SG;
        }
        else
        {
            // POS_ADJ or POS_PRONOUN_ADJ
            m_eSubparadigm = SUBPARADIGM_LONG_ADJ;
            m_eGender = GENDER_M;
            m_eCase = CASE_NOM;
            m_eNumber = NUM_SG;
        }
    }

    void SetParadigm (ET_Subparadigm eInitSubparadigm)
    {
        if (m_ePos == POS_UNDEFINED)
        {
            return;  // The POS should be specified first
        }
        m_eSubparadigm = eInitSubparadigm;
        if (m_ePos == POS_ADJ || m_ePos == POS_PRONOUN_ADJ || m_ePos == POS_NUM_ADJ)
        {
            m_eGender = GENDER_M;
            m_eCase = CASE_NOM;
            m_eNumber = NUM_SG;
        }
        else if (m_ePos == POS_VERB)
        {
            if (eInitSubparadigm == SUBPARADIGM_PRESENT_TENSE)
            {
                m_ePerson = PERSON_1;
                m_eNumber = NUM_SG;
                m_eGender = GENDER_UNDEFINED;
                m_eCase = CASE_UNDEFINED;
            }
            else if (eInitSubparadigm == SUBPARADIGM_PAST_TENSE)
            {
                m_ePerson = PERSON_UNDEFINED;
                m_eNumber = NUM_SG;
                m_eGender = GENDER_M;
                m_eCase = CASE_UNDEFINED;
            }
            else if (eInitSubparadigm == SUBPARADIGM_PART_PRES_ACT ||
                eInitSubparadigm == SUBPARADIGM_PART_PRES_PASS_LONG ||
                eInitSubparadigm == SUBPARADIGM_PART_PRES_PASS_SHORT ||
                eInitSubparadigm == SUBPARADIGM_PART_PAST_ACT ||
                eInitSubparadigm == SUBPARADIGM_PART_PAST_PASS_LONG ||
                eInitSubparadigm == SUBPARADIGM_PART_PAST_PASS_SHORT)
            {
                m_ePerson = PERSON_UNDEFINED;
                m_eNumber = NUM_SG;
                m_eGender = GENDER_M;
                m_eCase = CASE_NOM;
            }
            else if (eInitSubparadigm == SUBPARADIGM_INFINITIVE ||
                eInitSubparadigm == SUBPARADIGM_ADVERBIAL_PRESENT ||
                eInitSubparadigm == SUBPARADIGM_ADVERBIAL_PAST)
            {
                m_ePerson = PERSON_UNDEFINED;
                m_eNumber = NUM_UNDEFINED;
                m_eGender = GENDER_UNDEFINED;
                m_eCase = CASE_UNDEFINED;
            }
            else if (eInitSubparadigm == SUBPARADIGM_IMPERATIVE)
            {
                m_ePerson = PERSON_2;
                m_eNumber = NUM_SG;
                m_eGender = GENDER_UNDEFINED;
                m_eCase = CASE_UNDEFINED;
            }
        }
    }

    bool bIncrement()
    {
        if (POS_NOUN == m_ePos)
        {
            ++m_eCase;
            if (m_eCase != CASE_COUNT)
            {
                return true;
            }
            else
            {
                m_eCase = CASE_NOM;
                ++m_eNumber;
                if (m_eNumber != NUM_COUNT)
                {
                    return true;
                }
                return false;
            }
        }

        if (POS_LAST_NAME == m_ePos && SUBPARADIGM_LAST_NAME_NOUN == m_eSubparadigm)
        {
            ++m_eCase;
            if (m_eCase != CASE_COUNT)
            {
                return true;
            }
            else
            {
                m_eCase = CASE_NOM;
                ++m_eNumber;
                if (m_eNumber != NUM_COUNT)
                {
                    return true;
                }
                return false;
            }
        }

        if (POS_LAST_NAME == m_ePos && SUBPARADIGM_LAST_NAME_NOUN_F == m_eSubparadigm)
        {
            ++m_eCase;
            if (m_eCase != CASE_COUNT)
            {
                return true;
            }
        }

        if (POS_LAST_NAME == m_ePos &&
            (SUBPARADIGM_LAST_NAME_LONG_ADJ == m_eSubparadigm
            || SUBPARADIGM_LAST_NAME_PRONOUN_ADJ == m_eSubparadigm))
        {
            ++m_eCase;
            if (m_eCase != CASE_COUNT)
            {
                return true;
            }
            else
            {
                m_eCase = CASE_NOM;
                if (GENDER_M == m_eGender)
                {
                    ++m_eGender;
                }
                else
                {
                    m_eGender = GENDER_UNDEFINED;
                    ++m_eNumber;
                }
                if (m_eNumber != NUM_COUNT)
                {
                    return true;
                }
                return false;
            }
        }

        if (m_ePos == POS_ADJ || m_ePos == POS_PRONOUN_ADJ || m_ePos == POS_NUM_ADJ)
        {
            if (m_eSubparadigm == SUBPARADIGM_LONG_ADJ || m_eSubparadigm == SUBPARADIGM_PRONOUN_ADJ)
            {
                if (m_eCase == CASE_ACC &&
                    (m_eGender == GENDER_M || m_eNumber == NUM_PL))
                {
                    if (m_eAnimacy == ANIM_UNDEFINED)
                    {
                        m_eAnimacy = ANIM_NO;
                        return true;
                    }
                    else if (m_eAnimacy == ANIM_NO)
                    {
                        m_eAnimacy = ANIM_YES;
                        return true;
                    }
                    else if (m_eAnimacy == ANIM_YES)
                    {
                        m_eAnimacy = ANIM_UNDEFINED;
                    }
                }
                ++m_eCase;
                while (m_eCase == CASE_PART || m_eCase == CASE_LOC || m_eCase == CASE_NUM)
                {
                    ++m_eCase;
                }
                if (m_eCase != CASE_COUNT)
                {
                    if (m_eCase == CASE_ACC && (m_eGender == GENDER_M || m_eNumber == NUM_PL))
                    {
                        m_eAnimacy = ANIM_NO;
                    }
                    return true;
                }
                else
                {
                    m_eCase = CASE_NOM;
                    if (m_eNumber == NUM_SG)
                    {
                        ++m_eGender;
                        if (m_eGender != GENDER_COUNT)
                        {
                            return true;
                        }
                        else
                        {
                            m_eGender = GENDER_UNDEFINED;
                            m_eNumber = NUM_PL;
                            return true;
                        }
                    }
                    else
                    {
                        return false;   // The user should proceed to the Short subparadigm manually
                    }
                }
            }
            else if (m_eSubparadigm == SUBPARADIGM_SHORT_ADJ)
            {
                if (m_eNumber == NUM_SG)
                {
                    ++m_eGender;
                    if (m_eGender != GENDER_COUNT)
                    {
                        return true;
                    }
                    else
                    {
                        m_eGender = GENDER_UNDEFINED;
                        m_eNumber = NUM_PL;
                        return true;
                    }
                }
                else
                {
                    return false;
                }
            }
        }

        if (m_ePos == POS_VERB)
        {
            if (m_eSubparadigm == SUBPARADIGM_PRESENT_TENSE)
            {
                ++m_ePerson;
                if (m_ePerson != PERSON_COUNT)
                {
                    return true;
                }
                else
                {
                    m_ePerson = PERSON_1;
                    ++m_eNumber;
                    if (m_eNumber != NUM_COUNT)
                    {
                        return true;
                    }
                    return false;
                }
            }
            if (m_eSubparadigm == SUBPARADIGM_PART_PRES_ACT ||
                m_eSubparadigm == SUBPARADIGM_PART_PAST_ACT ||
                m_eSubparadigm == SUBPARADIGM_PART_PRES_PASS_LONG ||
                m_eSubparadigm == SUBPARADIGM_PART_PAST_PASS_LONG)
            {
                if (m_eCase == CASE_ACC && (m_eGender == GENDER_M || m_eNumber == NUM_PL))
                {
                    if (m_eAnimacy == ANIM_UNDEFINED)
                    {
                        m_eAnimacy = ANIM_NO;
                        return true;
                    }
                    else if (m_eAnimacy == ANIM_NO)
                    {
                        m_eAnimacy = ANIM_YES;
                        return true;
                    }
                    else if (m_eAnimacy == ANIM_YES)
                    {
                        m_eAnimacy = ANIM_UNDEFINED;
                    }
                }
                ++m_eCase;
                while (m_eCase == CASE_PART || m_eCase == CASE_LOC || m_eCase == CASE_NUM)
                {
                    ++m_eCase;
                }
                if (m_eCase != CASE_COUNT)
                {
                    if (m_eCase == CASE_ACC && (m_eGender == GENDER_M || m_eNumber == NUM_PL))
                    {
                        m_eAnimacy = ANIM_NO;
                    }
                    return true;
                }
                else
                {
                    m_eCase = CASE_NOM;
                    if (m_eNumber == NUM_SG)
                    {
                        ++m_eGender;
                        if (m_eGender != GENDER_COUNT)
                        {
                            return true;
                        }
                        else
                        {
                            m_eGender = GENDER_UNDEFINED;
                            m_eNumber = NUM_PL;
                            return true;
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else if (m_eSubparadigm == SUBPARADIGM_PART_PAST_PASS_SHORT ||
                     m_eSubparadigm == SUBPARADIGM_PART_PRES_PASS_SHORT ||
                     m_eSubparadigm == SUBPARADIGM_PAST_TENSE)
            {
                if (m_eNumber == NUM_SG)
                {
                    ++m_eGender;
                    if (m_eGender != GENDER_COUNT)
                    {
                        return true;
                    }
                    else
                    {
                        m_eGender = GENDER_UNDEFINED;
                        m_eNumber = NUM_PL;
                        return true;
                    }
                }
                else
                {
                    return false;
                }
            }
            else if (m_eSubparadigm == SUBPARADIGM_IMPERATIVE)
            {
                ++m_eNumber;
                if (m_eNumber != NUM_COUNT)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else if (m_eSubparadigm == SUBPARADIGM_ADVERBIAL_PRESENT ||
                     m_eSubparadigm == SUBPARADIGM_ADVERBIAL_PAST ||
                     m_eSubparadigm == SUBPARADIGM_INFINITIVE)
            {
                return false;
            }
        }

        return false;

    }   // bIncrement()

};      //  class CGramHasher

}   // namespace Hlib

#endif // GRAMHASHER_H_INCLUDED
