/**
 * Structures and functions for grouping lexemes into tokens.  The tokenizer
 * reads through an array of lexemes (generated by the lexer) and groups them
 * into tokens based on their structure.  In addition, some lexemes with
 * semantic meaning (such as integers, floats, strings, and booleans) will have
 * their values extracted and stored.
 *
 * \file   tokenizer.h
 *
 * \author Justin J. Meza
 *
 * \date   2010-2012
 */

#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "error.h"

#undef DEBUG

/**
 * Represents a token type.  All of the token type names correspond to either
 * the semantic type of token data or the lexemes which make up the particular
 * token.
 *
 * \note Remember to update the keywords array (below) with the token image.
 */
typedef enum {
	TT_INTEGER,     /**< Integer literal. */
	TT_FLOAT,       /**< Decimal literal. */
	TT_STRING,      /**< String literal. */
	TT_IDENTIFIER,  /**< Identifier literal. */
	TT_BOOLEAN,     /**< Boolean literal. */
	TT_IT,          /**< \ref impvar "Implicit variable". */
	TT_ITZLIEKA,    /**< Inherited object declaration. */
	TT_NOOB,        /**< Nil keyword. */
	TT_NUMBR,       /**< Integer keyword. */
	TT_NUMBAR,      /**< Decimal keyword. */
	TT_TROOF,       /**< Boolean keyword. */
	TT_YARN,        /**< String keyword. */
	TT_BUKKIT,      /**< Array. */
	TT_EOF,         /**< End of file. */
	TT_NEWLINE,     /**< Newline. */
	TT_HAI,         /**< Beginning of main block. */
	TT_KTHXBYE,     /**< End of main block. */
	TT_HASA,        /**< Variable declaration. */
	TT_HASAN,       /**< Variable declaration. */
	TT_ITZA,        /**< Variable type initialization. */
	TT_ITZ,         /**< Variable value initialization. */
	TT_RNOOB,       /**< Deallocation. */
	TT_R,           /**< Assignment. */
	TT_ANYR,        /**< User-defined function argument separator. */
	TT_AN,          /**< Built-in function argument separator. */
	TT_SUMOF,       /**< Addition. */
	TT_DIFFOF,      /**< Subtraction. */
	TT_PRODUKTOF,   /**< Multiplication. */
	TT_QUOSHUNTOF,  /**< Division. */
	TT_MODOF,       /**< Modulo. */
	TT_BIGGROF,     /**< Greater than. */
	TT_SMALLROF,    /**< Less than. */
	TT_BOTHOF,      /**< Logical AND. */
	TT_EITHEROF,    /**< Logical OR. */
	TT_WONOF,       /**< Logical XOR. */
	TT_NOT,         /**< Logical NOT. */
	TT_MKAY,        /**< Infinite arity argument delimiter. */
	TT_ALLOF,       /**< Infinite arity logical AND. */
	TT_ANYOF,       /**< Infinite arity logical OR. */
	TT_BOTHSAEM,    /**< Equality. */
	TT_DIFFRINT,    /**< Inequality. */
	TT_MAEK,        /**< Cast. */
	TT_A,           /**< Cast target separator. */
	TT_ISNOWA,      /**< In-place cast. */
	TT_VISIBLE,     /**< Print. */
	TT_INVISIBLE,   /**< Print to standard error. */
	TT_SMOOSH,      /**< String concatenation. */
	TT_BANG,        /**< Exclamation point (!) */
	TT_GIMMEH,      /**< Input. */
	TT_ORLY,        /**< Conditional. */
	TT_YARLY,       /**< True branch. */
	TT_MEBBE,       /**< Else branch. */
	TT_NOWAI,       /**< False branch. */
	TT_OIC,         /**< Conditional and switch delimiter. */
	TT_WTF,         /**< Switch. */
	TT_OMG,         /**< Case. */
	TT_OMGWTF,      /**< Default case. */
	TT_GTFO,        /**< Break or return without value. */
	TT_IMINYR,      /**< Loop beginning. */
	TT_UPPIN,       /**< Auto increment loop variable. */
	TT_NERFIN,      /**< Auto decrement loop variable. */
	TT_YR,          /**< Function name delimiter. */
	TT_TIL,         /**< Do until. */
	TT_WILE,        /**< Do while. */
	TT_IMOUTTAYR,   /**< Loop ending. */
	TT_HOWIZ,       /**< Function definition beginning. */
	TT_IZ,          /**< Function scope delimiter. */
	TT_IFUSAYSO,    /**< Function definition end. */
	TT_FOUNDYR,     /**< Return with value. */
	TT_SRS,         /**< Indirect variable access. */
	TT_APOSTROPHEZ, /**< Array slot access ('Z). */
	TT_OHAIIM,      /**< Alternate array declaration. */
	TT_IMLIEK,      /**< Alternate inherited object declaration. */
	TT_KTHX,        /**< End of alternate array declaration. */

	TT_ENDOFTOKENS  /**< Sentinel end of this enum -- don't move it! */
} TokenType;

static const char *keywords[] = {
	"",            /* TT_INTEGER */
	"",            /* TT_FLOAT */
	"",            /* TT_STRING */
	"",            /* TT_IDENTIFIER */
	"",            /* TT_BOOLEAN */
	"ताज़ा",          /* TT_IT */
	"छापों",  /* TT_ITZLIEKA */
	"खाली",        /* TT_NOOB */
	"संख्या",       /* TT_NUMBR */
	"दशमलव",      /* TT_NUMBAR */
	"हाना",       /* TT_TROOF */
	"अक्षरमाला",        /* TT_YARN */
	"माला",      /* TT_BUKKIT */
	"",            /* TT_EOF */
	"",            /* TT_NEWLINE */
	"नमस्ते",         /* TT_HAI */
	"अलविदा",     /* TT_KTHXBYE */
	"चीज़",       /* TT_HASA */
	"चीज़",      /* TT_HASAN */
	"है एक",       /* TT_ITZA */
	"है",         /* TT_ITZ */
	"है खाली",      /* TT_RNOOB */
	"अब है",           /* TT_R */
	"और जानकारी",       /* TT_ANYR */
	"और",          /* TT_AN */
	"जोड़",      /* TT_SUMOF */
	"घाटा",     /* TT_DIFFOF */
	"गुणा",  /* TT_PRODUKTOF */
	"भाग", /* TT_QUOSHUNTOF */
	"बाकी",      /* TT_MODOF */
	"बड़ा",    /* TT_BIGGROF */
	"छोटा",   /* TT_SMALLROF */
	"दोनों",     /* TT_BOTHOF */
	"कोई एक",   /* TT_EITHEROF */
	"सिर्फ़ एक",      /* TT_WONOF */
	"नहीं",         /* TT_NOT */
	"बस",        /* TT_MKAY */
	"सब",      /* TT_ALLOF */
	"कुछ",      /* TT_ANYOF */
	"बराबर",   /* TT_BOTHSAEM */
	"अलग",    /* TT_DIFFRINT */
	"बनाओ",        /* TT_MAEK */
	"एक",           /* TT_A */
	"अब बन गया",    /* TT_ISNOWA */
	"दिखाओ",     /* TT_VISIBLE */
	"गलती",   /* TT_INVISIBLE */
	"जोड़ो",      /* TT_SMOOSH */
	"!",           /* TT_BANG */
	"दो",      /* TT_GIMMEH */
	"क्या?",      /* TT_ORLY */
	"हाँ",      /* TT_YARLY */
	"या फिर",       /* TT_MEBBE */
	"ना",      /* TT_NOWAI */
	"अंत",         /* TT_OIC */
	"पेड़",        /* TT_WTF */
	"शाखा",         /* TT_OMG */
	"नही तो",      /* TT_OMGWTF */
	"तोड़ो",        /* TT_GTFO */
	"शुरू करो",    /* TT_IMINYR */
	"बढ़ाओ",       /* TT_UPPIN */
	"बढ़ाओ",      /* TT_NERFIN */
	"यह",          /* TT_YR */
	"जब तक नहीं",         /* TT_TIL */
	"जब तक",        /* TT_WILE */
	"खतम करो", /* TT_IMOUTTAYR */
	"काम",      /* TT_HOWIZ */
	"बुलाओ",          /* TT_IZ */
	"कामखतम", /* TT_IFUSAYSO */
	"वापस",    /* TT_FOUNDYR */
	"सरस",         /* TT_SRS */
	"'का",          /* TT_APOSTROPHEZ */
	"O HAI IM",    /* TT_OHAIIM */
	"IM LIEK",     /* TT_IMLIEK */
	"खतम",        /* TT_KTHX */
	""             /* TT_ENDOFTOKENS */
};

/**
 * Stores token data with semantic meaning.
 */
typedef union {
	long long i; /**< Integer data. */
	float f;     /**< Decimal data. */
} TokenData;

/**
 * Stores a token type and any parsed values.
 */
typedef struct {
	TokenType type;    /**< The type of token. */
	TokenData data;    /**< The stored data of type \a type. */
	char *image;       /**< The characters that comprise the token. */
	const char *fname; /**< The name of the file containing the token. */
	unsigned int line; /**< The line number the token was on. */
} Token;

/**
 * \name Utilities
 *
 * Functions for performing helper tasks.
 */
/**@{*/
int isInteger(const char *);
int isFloat(const char *);
int isString(const char *);
int isIdentifier(const char *);
Token *isKeyword(LexemeList *, unsigned int *);
/**@}*/

/**
 * \name Token modifiers
 *
 * Functions for creating and deleting tokens.
 */
/**@{*/
Token *createToken(TokenType, const char *, const char *, unsigned int);
void deleteToken(Token *);
int addToken(Token ***, unsigned int *, Token*);
void deleteTokens(Token **);
unsigned int acceptLexemes(LexemeList *, unsigned int, const char *);
/**@}*/

/**
 * \name Lexeme tokenizer
 *
 * Generates tokens from lexemes.
 */
/**@{*/
Token **tokenizeLexemes(LexemeList *);
/**@}*/

#endif /* __TOKENIZER_H__ */
