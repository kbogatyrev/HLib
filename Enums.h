#ifndef ENUMS_H_INCLUDED
#define ENUMS_H_INCLUDED

namespace Hlib
{
    enum ET_Ternary
    {
        TERNARY_FALSE,
        TERNARY_TRUE,
        TERNARY_BOTH
    };

    enum ET_ReturnCode
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
        H_UNAVAILABLE =        -9,            // interface method not implemented (and never will be) for this consumer
        H_FILE_IO_ERROR =     -10
    };

    class CErrorCode
    {
    public:
        static bool bError(ET_ReturnCode eRc)
        {
            if ((int)eRc >= 0) return false;
            return true;
        }
    };

    enum ET_MainSymbol
    {
        MS_START,
        MS_M = MS_START,
        MS_MO,
        MS_ZH,
        MS_ZHO,
        MS_S,
        MS_SO,
        MS_MO_ZHO,
        MS_MN,
        MS_MN_NEOD,
        MS_MN_ODUSH,
        MS_MN_OT,
        MS_P,
        MS_MS,
        MS_MS_P,
        MS_CHISL,
        MS_CHISL_P,
        MS_SV,
        MS_NSV,
        MS_SV_NSV,
        MS_N,
        MS_PREDL,
        MS_SOJUZ,
        MS_PREDIC,
        MS_VVODN,
        MS_SRAVN,
        MS_CHAST,
        MS_MEZHD,
        MS_PREDIC_MS,
        MS_END,
        MS_UNDEFINED
    };

    enum ET_PartOfSpeech
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
        POS_PREPOSITION,
        POS_CONJUNCTION,
        POS_PARTICLE,
        POS_INTERJ,         // межд.
        POS_PARENTH,        // вводн.
        POS_NULL,
        POS_COUNT
    };

    enum ET_InflectionType
    {
        INFLECTION_TYPE_UNDEFINED = -1,
        CONJUGATIONS_COUNT = 16,
        INFLECTION_TYPE_COUNT = 16
    };

    enum ET_Conjugation
    {
        CONJUGATION_UNDEFINED,
        E_VERB,
        I_VERB,
        CONJUGATION_COUNT
    };

    enum ET_Case
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

    };

    enum ET_Number
    {
        NUM_UNDEFINED,
        NUM_SG,
        NUM_PL,
        NUM_COUNT

    };

    enum ET_Subparadigm
    {
        SUBPARADIGM_UNDEFINED,
        SUBPARADIGM_NOUN,
        SUBPARADIGM_LONG_ADJ,
        SUBPARADIGM_SHORT_ADJ,
        SUBPARADIGM_COMPARATIVE,
        SUBPARADIGM_PRONOUN,
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
        SUBPARADIGM_NUM_2TO4,
        SUBPARADIGM_NUM,
        SUBPARADIGM_NUM_ADJ,
        SUBPARADIGM_ASPECT_PAIR,
        SUBPARADIGM_PARTICLE,
        SUBPARADIGM_PREPOSITION,
        SUBPARADIGM_ADVERB,
        SUBPARADIGM_CONJUNCTION,
        SUBPARADIGM_INTERJECTION,
        SUBPARADIGM_PARENTHESIS,
        SUBPARADIGM_PREDICATIVE,
        SUBPARADIGM_COUNT
    };

    enum ET_Gender
    {
        GENDER_UNDEFINED,
        GENDER_M,
        GENDER_F,
        GENDER_N,
        GENDER_COUNT
    };

    enum ET_Aspect
    {
        ASPECT_UNDEFINED,
        ASPECT_IMPERFECTIVE,
        ASPECT_PERFECTIVE,
        ASPECT_COUNT
    };

    enum ET_Person
    {
        PERSON_UNDEFINED,
        PERSON_1,
        PERSON_2,
        PERSON_3,
        PERSON_COUNT
    };

    enum ET_Animacy
    {
        ANIM_UNDEFINED,
        ANIM_YES,
        ANIM_NO,
        ANIM_COUNT
    };

    enum ET_Voice
    {
        VOICE_UNDEFINED,
        VOICE_ACTIVE,
        VOICE_PASSIVE,
        VOICE_COUNT
    };

    enum ET_Reflexivity
    {
        REFL_UNDEFINED,
        REFL_YES,
        REFL_NO,
        REFL_COUNT
    };

    enum ET_Tense
    {
        TENSE_UNDEFINED,
        TENSE_PRESENT,
        TENSE_PAST,
        TENSE_COUNT
    };

    enum ET_AccentType
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
    };

    enum ET_EndingClass
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
        ENDING_CLASS_ADVERBIAL,
        ENDING_CLASS_COUNT
    };

    enum ET_StressLocation
    {
        STRESS_LOCATION_UNDEFINED,
        STRESS_LOCATION_STEM,
        STRESS_LOCATION_ENDING,
        STRESS_LOCATION_COUNT
    };

    enum ET_StressType
    {
        STRESS_TYPE_UNDEFINED,
        STRESS_PRIMARY,
        STRESS_SECONDARY,
        STRESS_TYPE_COUNT
    };

    enum ET_StemAuslaut
    {
        STEM_AUSLAUT_UNDEFINED,
        STEM_AUSLAUT_SH,
        STEM_AUSLAUT_NOT_SH,
        STEM_AUSLAUT_CONSONANT,
        STEM_AUSLAUT_NOT_CONSONANT,
        STEM_AUSLAUT_V,         //  past adverbials only
        STEM_AUSLAUT_NOT_V,     //  past adverbials only
        STEM_AUSLAUT_VELAR,     //  comparatives
        STEM_AUSLAUT_NOT_VELAR, //  comparatives
        STEM_AUSLAUT_COUNT

    };

//    enum ET_StemAugment
//    {
//        STEM_AUGMENT_UNDEFINED,
//        STEM_AUGMENT_COUNT = 4
//    };

    enum ET_Status
    {
        STATUS_UNDEFINED,
        STATUS_COMMON,
        STATUS_OBSOLETE,
        STATUS_RARE,
        STATUS_QUESTIONABLE,
        STATUS_ASSUMED,
        STATUS_INCORRECT,
        STATUS_COUNT
    };

    enum ET_TestResult
    {
        TEST_RESULT_UNDEFINED,
        TEST_RESULT_OK,
        TEST_RESULT_FAIL,
        TEST_RESULT_INCOMPLETE,
        TEST_RESULT_COUNT
    };

    enum ET_VowelStressRelation
    {
        VOWEL_STRESS_RELATION_UNDEFINED,
        STRESSED,
        PRETONIC,
        FIRST_PRETONIC,
        OTHER_PRETONIC,
        POSTTONIC,
        UNSTRESSED,
        VOWEL_STRESS_RELATION_COUNT
    };

    enum ET_PhonemicContext
    {
        PHONEMIC_CONTEXT_UNDEFINED,
        VOWEL,
        CONSONANT,
        HARD_CONSONANT,
        HARD_PAIRED_CONSONANT,
        SOFT_CONSONANT,
        SOFT_CONSONANT_NO_CH_SHCH,
        VOICELESS,
        PHONEMIC_CONTEXT_COUNT
    };

    enum ET_ContextDirection
    {
        CONTEXT_DIRECTION_UNDEFINED,
        LEFT_CONTEXT,
        RIGHT_CONTEXT,
        CONTEXT_DIRECTION_COUNT
    };


    enum ET_MorphemicContext
    {
        MORPHEMIC_CONTEXT_UNDEFINED,
        ENDING,
        NOT_ENDING,
        ROOT,
        ROOT_AUSLAUT,
        MORPHEMIC_CONTEXT_COUNT
    };

    enum ET_SyllabicPosition
    {
        SYLLABIC_POSITION_UNDEFINED,
        SYLLABIC_POSITION_ULTIMA,
        SYLLABIC_POSITION_PENULT,
        SYLLABIC_POSITION_ANTEPENULT,
        SYLLABIC_POSITION_COUNT
    };

    enum ET_Boundary
    {
        BOUNDARY_UNDEFINED,
        BOUNDARY_WORD,
        BOUNDARY_NOT_PROCLITIC,
        BOUNDARY_SYNTAGM,
        BOUNDARY_COUNT
    };

    enum ET_Sound
    {
        SOUND_UNDEFINED,
        STRESSED_A,                         // 1
        STRESSED_O,
        STRESSED_E,
        I,
        Y,
        U,
        PRETONIC_A,
        PRETONIC_LONG_A,
        PRETONIC_E,         // экран, поэтесса ($36, $45)
        PRETONIC_HARD_IE,                   // 10
        PRETONIC_SOFT_IE,
        SCHWA,
        UNSTRESSED_FRONT_VOWEL,
        UNSTRESSED_O,   // какао
        UNSTRESSED_E,
        P_HARD,
        P_SOFT,
        B_HARD,
        B_SOFT,
        M_HARD,                             // 20
        M_SOFT,
        F_HARD,
        F_SOFT,
        V_HARD,
        V_SOFT,
        N_HARD,
        N_SOFT,
        T_HARD,
        T_SOFT,
        D_HARD,                             // 30
        D_SOFT,
        TS_HARD,
        TS_SOFT,    // Цюрих, хуацяо
        CH,
        S_HARD,
        S_SOFT,
        Z_HARD,
        Z_SOFT,
        L_HARD,
        L_SOFT,                             // 40
        R_HARD,
        R_SOFT,
        SH_HARD,
        SH_SOFT,    // Шяуляй, Герхард Шюрер
        ZH_HARD,
        ZH_SOFT,    // Жюль, Жён Африк
        JOD,
        K_HARD,
        K_SOFT,
        G_HARD,                             // 50
        G_SOFT,
        X_HARD,
        X_SOFT,
        G_FRICATIVE_HARD,
        G_FRICATIVE_SOFT,
        SOUND_COUNT
    
    };      //  ET_Consonant

    enum ET_Transform
    {
        TRANSFORM_UNDEFINED,
        SOFTEN,
        VOICE,
        DEVOICE,
        GEMINATE,
        SELF,
        TRANSFORM_COUNT
    
    };      // ET_Transform

    enum ET_Place
    {
        PLACE_UNDEFINED,
        PLACE_BILABIAL,
        PLACE_LABIODENTAL,
        PLACE_DENTAL,
        PLACE_ALVEOLAR,
        PLACE_ALVEOPALATAL,
        PLACE_PALATAL,
        PLACE_VELAR,
        PLACE_COUNT
    };

    enum ET_Manner
    {
        MANNER_UNDEFINED,
        MANNER_STOP,
        MANNER_FRICATIVE,
        MANNER_AFFRICATE,
        MANNER_NASAL,
        MANNER_APPROXIMANT,
        MANNER_TRILL,
        MANNER_COUNT
    };

    enum ET_Voicedness
    {
        VOICEDNESS_UNDEFINED,
        VOICEDNESS_VOICELESS,
        VOICEDNESS_VOICED,
        VOICEDNESS_COUNT
    };

    enum ET_Palatalization
    {
        PALATALIZATION_UNDEFINED,
        PALATALIZATION_HARD,
        PALATALIZATION_SOFT,
        PALATALIZATION_COUNT
    };

    enum ET_Nasality
    {
        NASALITY_UNDEFINED,
        NASALITY_NON_NASAL,
        NASALITY_NASAL,
        NASALITY_COUNT
    };

    enum ET_AcousticFeatureValue
    {
        ACOUSTIC_FEATURE_VALUE_UNDEFINED,
        ACOUSTIC_FEATURE_VALUE_NULL,
        ACOUSTIC_FEATURE_VALUE_NO,
        ACOUSTIC_FEATURE_VALUE_YES,
        ACOUSTIC_FEATURE_VALUE_COUNT
    };

    enum ET_Position
    {
        POSITION_UNDEFINED,
        POSITION_SENTENCE_START,
        POSITION_SENTENCE_END,
        POSITION_VERSE_START,
        POSITION_VERSE_END,
        POSITION_TACT_GROUP_START,
        POSITION_TACT_GROUP_END,
        POSITION_WORD_FORM_START,
        POSITION_WORD_FORM_END,
        POSITON_COUNT
    
    };  //  ET_Position

    enum ET_Frontness
    {
        FRONTNESS_UNDEFINED,
        FRONTNESS_FRONT,
        FRONTNESS_CENTRAL,
        FRONTNESS_BACK,
        FRONTNESS_COUNT
    };

    enum ET_Height
    {
        HEIGHT_UNDEFINED,
        HEIGHT_HIGH,
        HEIGHT_MID,
        HEIGHT_LOW,
        HEIGHT_COUNT
    };

    enum ET_Length
    {
        LENGTH_UNDEFINED,
        LENGTH_SHORT,
        LENGTH_LONG,
        LENGTH_COUNT
    };

    enum ET_WordStressType
    {
        WORD_STRESS_TYPE_UNDEFINED,
        WORD_STRESS_TYPE_AUTONOMOUS,
        WORD_STRESS_TYPE_PROCLITIC,
        WORD_STRESS_TYPE_ENCLITIC,
        WORD_STRESS_TYPE_CLITIC,
        WORD_STRESS_TYPE_COUNT

    };  //  ET_WordStressType

    enum ET_RuleStrength
    {
        RULE_STRENGTH_UNDEFINED,
        RULE_STRENGTH_DEFAULT,
        RULE_STRENGTH_RECOMMENDED,
        RULE_STRENGTH_VARIATION,
        RULE_STRENGTH_OPTIONAL,
        RULE_STRENGTH_COUNT
    };

    enum ET_TextMetadata
    {
        TEXT_METADATA_UNDEFINED,
        TEXT_METADATA_AUTHOR, 
        TEXT_METADATA_BOOK, 
        TEXT_METADATA_PAGE,
        TEXT_METADATA_TITLE,
        TEXT_METADATA_CHAPTER,
        TEXT_METADATA_FOOTNOTE_REF,
        TEXT_METADATA_FOOTNOTE_TEXT,
        TEXT_METADATA_DATE,
        TEXT_METADATA_DEDICATION,
        TEXT_METADATA_COUNT
    };

    [[maybe_unused]]static void operator++ (ET_MainSymbol& eo_ms) 
    {
        eo_ms = (ET_MainSymbol)(eo_ms + 1);
    }

    [[maybe_unused]] static void operator++ (ET_Case& eC)
    {
        eC = (ET_Case)(eC + 1);
    }

    [[maybe_unused]] static void operator++ (ET_Number& eN)
    {
        eN = (ET_Number)(eN + 1);
    }

    [[maybe_unused]] static void operator++ (ET_Gender& eG)
    {
        eG = (ET_Gender)(eG + 1);
    }

    [[maybe_unused]] static void operator++ (ET_Animacy& eo_a)
    {
        eo_a = (ET_Animacy)(eo_a + 1);
    }

    [[maybe_unused]] static void operator++ (ET_Person& eo_p)
    {
        eo_p = (ET_Person)(eo_p + 1);
    }

    [[maybe_unused]] static ET_PartOfSpeech eSubparadigmToPos(ET_Subparadigm eSubparadigm)
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
                
        case SUBPARADIGM_PRONOUN:
        case SUBPARADIGM_PRONOUN_ADJ:
            ePos = POS_PRONOUN;
            break;

        case SUBPARADIGM_NUM:
        case SUBPARADIGM_NUM_2TO4:
            ePos = POS_NUM;
            break;
                
        case SUBPARADIGM_NUM_ADJ:
            ePos = POS_NUM_ADJ;
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

        case SUBPARADIGM_PARTICLE:
            ePos = POS_PARTICLE;
            break;

        case SUBPARADIGM_PREPOSITION:
            ePos = POS_PREPOSITION;
            break;

        case SUBPARADIGM_ADVERB:
            ePos = POS_ADV;
            break;

        case SUBPARADIGM_CONJUNCTION:
            ePos = POS_CONJUNCTION;
            break;

        case SUBPARADIGM_INTERJECTION:
            ePos = POS_INTERJ;
            break;

        case SUBPARADIGM_PARENTHESIS:
            ePos = POS_PARENTH;
            break;

        case SUBPARADIGM_PREDICATIVE:
            ePos = POS_PREDIC;
            break;

        case SUBPARADIGM_UNDEFINED:
        default:
            ePos = POS_UNDEFINED;
        
        }   // switch

        return ePos;
    
    }       //  eSubparadigmToPos

}   // namespace Hlib

#endif // ENUMS_H_INCLUDED
