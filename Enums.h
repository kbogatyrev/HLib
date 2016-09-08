#ifndef ENUMS_H_INCLUDED
#define ENUMS_H_INCLUDED

namespace Hlib
{

    typedef enum ET_Ternary
    {
        TERNARY_FALSE,
        TERNARY_TRUE,
        TERNARY_BOTH

    }     ET_Ternary;

    typedef enum ET_ReturnCode
    {
        H_NO_ERROR =            0,
        H_FALSE,
        H_TRUE,
        H_NO_MORE,
        H_ERROR_GENERAL =      -1,
        H_ERROR_UNEXPECTED =   -2,
        H_ERROR_POINTER =      -3,
        H_ERROR_INVALID_ARG =  -4,
        H_ERROR_DB =           -5,
        H_EXCEPTION =          -6,            // an exception was caught and handled upstream
        H_CTOR =               -7,            // exception thrown from a ctor
        H_NOTIMPL =            -8,            // interface method not implemented (but may be in the future)
        H_UNAVAILABLE =        -9             // interface method not implemented (and never will be) for this consumer

    }   ET_Error;

    class CErrorCode
    {
    public:
        static bool bError(ET_ReturnCode eRc)
        {
            if ((int)eRc >= 0) return false;
            return true;
        }
    };

    typedef enum ET_PartOfSpeech
    {
        POS_UNDEFINED,
        POS_NOUN,
        POS_VERB,
        POS_ADJ,
        POS_PRONOUN,
        POS_PRONOUN_ADJ,
        POS_PRONOUN_PREDIC,
        POS_NUM,
        POS_NUM_ADJ,
        POS_ADV,
        POS_COMPAR,
        POS_PREDIC,
        POS_PREP,
        POS_CONJ,
        POS_PARTICLE,
        POS_INTERJ,
        POS_PARENTH,
        POS_NULL,
        POS_COUNT

    }     ET_PartOfSpeech;

    typedef enum ET_InflectionType
    {
        INFLECTION_TYPE_UNDEFINED = -1,
        CONJUGATIONS_COUNT = 16,
        INFLECTION_TYPE_COUNT = 16

    }     ET_InflectionType;

    typedef enum ET_Conjugation
    {
        CONJUGATION_UNDEFINED,
        E_VERB,
        I_VERB,
        CONJUGATION_COUNT

    }     ET_Conjugation;

    typedef enum ET_Case
    {
        CASE_UNDEFINED,
        CASE_NOM,
        CASE_ACC,
        CASE_GEN,
        CASE_PART,
        CASE_DAT,
        CASE_INST,
        CASE_PREP,
        CASE_LOC,
        CASE_NUM,
        CASE_COUNT

    }     ET_Case;

    typedef enum ET_Number
    {
        NUM_UNDEFINED,
        NUM_SG,
        NUM_PL,
        NUM_COUNT

    }     ET_Number;

    typedef enum ET_Subparadigm
    {
        SUBPARADIGM_UNDEFINED,
        SUBPARADIGM_NOUN,
        SUBPARADIGM_LONG_ADJ,
        SUBPARADIGM_SHORT_ADJ,
        SUBPARADIGM_COMPARATIVE,
        SUBPARADIGM_PRONOUN_ADJ,
        SUBPARADIGM_INFINITIVE,
        SUBPARADIGM_PRESENT_TENSE,
        SUBPARADIGM_PAST_TENSE,
        SUBPARADIGM_IMPERATIVE,
        SUBPARADIGM_ADVERBIAL_PRESENT,
        SUBPARADIGM_ADVERBIAL_PAST,
        SUBPARADIGM_PART_PRES_ACT,
        SUBPARADIGM_PART_PRES_PASS_LONG,
        SUBPARADIGM_PART_PRES_PASS_SHORT,
        SUBPARADIGM_PART_PAST_ACT,
        SUBPARADIGM_PART_PAST_PASS_LONG,
        SUBPARADIGM_PART_PAST_PASS_SHORT,
        SUBPARADIGM_COUNT

    }     ET_Subparadigm;

    typedef enum ET_Gender
    {
        GENDER_UNDEFINED,
        GENDER_M,
        GENDER_F,
        GENDER_N,
        GENDER_COUNT

    }     ET_Gender;

    typedef enum ET_Aspect
    {
        ASPECT_UNDEFINED,
        ASPECT_IMPERFECTIVE,
        ASPECT_PERFECTIVE,
        ASPECT_COUNT

    }     ET_Aspect;

    typedef enum ET_Person
    {
        PERSON_UNDEFINED,
        PERSON_1,
        PERSON_2,
        PERSON_3,
        PERSON_COUNT

    }     ET_Person;

    typedef enum ET_Animacy
    {
        ANIM_UNDEFINED,
        ANIM_YES,
        ANIM_NO,
        ANIM_COUNT

    }     ET_Animacy;

    typedef enum ET_Voice
    {
        VOICE_UNDEFINED,
        VOICE_ACTIVE,
        VOICE_PASSIVE,
        VOICE_COUNT

    }     ET_Voice;

    typedef enum ET_Reflexive
    {
        REFL_UNDEFINED,
        REFL_YES,
        REFL_NO,
        REFL_COUNT

    }     ET_Reflexive;

    typedef enum ET_AccentType
    {
        AT_UNDEFINED,
        AT_A,
        AT_A1,
        AT_B,
        AT_B1,
        AT_C,
        AT_C1,
        AT_C2,
        AT_D,
        AT_D1,
        AT_E,
        AT_F,
        AT_F1,
        AT_F2,
        AT_COUNT

    }     ET_AccentType;

    typedef enum ET_EndingClass
    {
        ENDING_CLASS_UNDEFINED,
        ENDING_CLASS_NOUN,
        ENDING_CLASS_LONG_ADJECTIVE,
        ENDING_CLASS_PRONOUN,
        ENDING_CLASS_SHORT_ADJECTIVE,
        ENDING_CLASS_PRESENT_TENSE,
        ENDING_CLASS_INFINITIVE,
        ENDING_CLASS_PAST_TENSE,
        ENDING_CLASS_IMPERATIVE,
        ENDING_CLASS_COMPARATIVE,
        ENDING_CLASS_COUNT

    }     ET_EndingClass;

    typedef enum ET_StressLocation
    {
        STRESS_LOCATION_UNDEFINED,
        STRESS_LOCATION_STEM,
        STRESS_LOCATION_ENDING,
        STRESS_LOCATION_COUNT

    }     ET_StressLocation;

    typedef enum ET_StressType
    {
        STRESS_TYPE_UNDEFINED,
        STRESS_PRIMARY,
        STRESS_SECONDARY,
        STRESS_TYPE_COUNT

    }     ET_StressType;

    typedef enum ET_StemAuslaut
    {
        STEM_AUSLAUT_UNDEFINED,
        STEM_AUSLAUT_SH,
        STEM_AUSLAUT_NOT_SH,
        STEM_AUSLAUT_CONSONANT,
        STEM_AUSLAUT_NOT_CONSONANT,
        STEM_AUSLAUT_COUNT

    }     ET_StemAuslaut;

    typedef enum ET_StemAugment
    {
        STEM_AUGMENT_UNDEFINED,
        STEM_AUGMENT_COUNT = 4

    }     ET_StemAugment;

    typedef enum ET_Status
    {
        STATUS_UNDEFINED,
        STATUS_COMMON,
        STATUS_OBSOLETE,
        STATUS_RARE,
        STATUS_QUESTIONABLE,
        STATUS_INCORRECT,
        STATUS_COUNT

    }     ET_Status;

    typedef enum ET_TestResult
    {
        TEST_RESULT_UNDEFINED,
        TEST_RESULT_OK,
        TEST_RESULT_FAIL,
        TEST_RESULT_INCOMPLETE,
        TEST_RESULT_COUNT

    }     ET_TestResult;

    //static void operator++ (ET_MainSymbol& eo_ms) 
    //{
    //    eo_ms = (ET_MainSymbol)(eo_ms + 1);
    //}

    static void operator++ (ET_Case& eC)
    {
        eC = (ET_Case)(eC + 1);
    }

    static void operator++ (ET_Number& eN)
    {
        eN = (ET_Number)(eN + 1);
    }

    static void operator++ (ET_Gender& eG)
    {
        eG = (ET_Gender)(eG + 1);
    }

    static void operator++ (ET_Animacy& eo_a)
    {
        eo_a = (ET_Animacy)(eo_a + 1);
    }

    static void operator++ (ET_Person& eo_p)
    {
        eo_p = (ET_Person)(eo_p + 1);
    }

    static ET_PartOfSpeech eSubparadigmToPos(ET_Subparadigm eSubparadigm)
    {
        ET_PartOfSpeech ePos = POS_UNDEFINED;

        switch (eSubparadigm)
        {
        case SUBPARADIGM_NOUN:
            ePos = POS_NOUN;
            break;

        case SUBPARADIGM_LONG_ADJ:
        case SUBPARADIGM_SHORT_ADJ:
        case SUBPARADIGM_COMPARATIVE:
            ePos = POS_ADJ;
            break;
                
        case SUBPARADIGM_PRONOUN_ADJ:
            ePos = POS_PRONOUN;
            break;
                
        case  SUBPARADIGM_INFINITIVE:
        case  SUBPARADIGM_PRESENT_TENSE:
        case  SUBPARADIGM_PAST_TENSE:
        case  SUBPARADIGM_IMPERATIVE:
        case  SUBPARADIGM_ADVERBIAL_PRESENT:
        case  SUBPARADIGM_ADVERBIAL_PAST:
        case  SUBPARADIGM_PART_PRES_ACT:
        case  SUBPARADIGM_PART_PRES_PASS_LONG:
        case  SUBPARADIGM_PART_PRES_PASS_SHORT:
        case  SUBPARADIGM_PART_PAST_ACT:
        case  SUBPARADIGM_PART_PAST_PASS_LONG:
        case  SUBPARADIGM_PART_PAST_PASS_SHORT:
            ePos = POS_VERB;
            break;

        default:
            ePos = POS_UNDEFINED;
        
        }   // switch

        return ePos;
    
    }       //  eSubparadigmToPos

}   // namespace Hlib

#endif // ENUMS_H_INCLUDED
