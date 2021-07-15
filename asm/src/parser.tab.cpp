/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30704

/* Bison version string.  */
#define YYBISON_VERSION "3.7.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <map>
#include <regex>
#include <tuple>
#include <limits>
#include <vector>
#include <utility>
#include <fstream>
#include <optional>
#include <iostream>
#include <stdexcept>
#include <filesystem>

extern int yylex();
extern int yyparse();
extern std::FILE* yyin;

extern int line_number;

std::uint16_t macro_iden{ 1 },
			  write_head{ 0 };

struct Instruction;

std::vector<std::uint8_t> code{ 0 };

enum class Qualifier
{
	Macro,
	Ascii,

	Value8,
	Value16,

	Variable8,
	Variable16,
};

std::vector<std::uint8_t> assemble([[maybe_unused]] const Instruction& insn) noexcept
{

}

auto contains(std::map<std::string, std::pair<Qualifier, std::uint16_t>>& idents, std::string key, std::pair<Qualifier, std::uint16_t>&& value) noexcept
{
	if (idents.try_emplace(std::move(key), std::move(value)).first != idents.end()) [[likely]]
		;
	else
	{
		std::cerr << "Identifier " << key.c_str() << " was multiply defined";
		std::exit(50);
	}
}

std::uint16_t ident2int(const std::string& str, std::map<std::string, std::pair<Qualifier, std::uint16_t>>& idents) noexcept
{
	auto res = idents.at(str);

	switch (res.first)
	{
	case Qualifier::Macro:
		std::cout << "Macro identifiers cannot be converted implicitly to integer";
		std::exit(5);
		break;

	case Qualifier::Ascii:
		std::cout << "Ascii identifiers cannot be converted implicitly to integer";
		std::exit(5);
		break;

	case Qualifier::Value8:  [[fallthrough]];
	case Qualifier::Value16:
		return res.second;
		break;

	case Qualifier::Variable8:
		return std::uint16_t{ code[res.second] };
		break;

	case Qualifier::Variable16:
		auto lower = code[static_cast<int>(res.second) + 0];
		auto upper = code[static_cast<int>(res.second) + 1];
		return static_cast<std::uint16_t>(static_cast<std::uint16_t>(lower) | (static_cast<std::uint16_t>(upper) << 8));
	}
}

//maps any string identifier to a 16bit integer with a 'direct-ness' tag
std::map<std::string, std::pair<Qualifier, std::uint16_t>> identifiers{};

//stores all of the parsed macros
std::vector<Instruction> macros{};

int yyerror(const char* s);

#line 170 "parser.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* T_EOF  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_T_ENDL = 3,                     /* T_ENDL  */
  YYSYMBOL_T_INT = 4,                      /* T_INT  */
  YYSYMBOL_T_REGISTER = 5,                 /* T_REGISTER  */
  YYSYMBOL_T_IDENTIFIER = 6,               /* T_IDENTIFIER  */
  YYSYMBOL_T_STRING = 7,                   /* T_STRING  */
  YYSYMBOL_T_PLUS = 8,                     /* T_PLUS  */
  YYSYMBOL_T_MINUS = 9,                    /* T_MINUS  */
  YYSYMBOL_T_TIMES = 10,                   /* T_TIMES  */
  YYSYMBOL_T_DIVIDE = 11,                  /* T_DIVIDE  */
  YYSYMBOL_T_LSHIFT = 12,                  /* T_LSHIFT  */
  YYSYMBOL_T_RSHIFT = 13,                  /* T_RSHIFT  */
  YYSYMBOL_T_AMPERSAND = 14,               /* T_AMPERSAND  */
  YYSYMBOL_T_CARET = 15,                   /* T_CARET  */
  YYSYMBOL_T_TILDE = 16,                   /* T_TILDE  */
  YYSYMBOL_T_PIPE = 17,                    /* T_PIPE  */
  YYSYMBOL_T_COMMA = 18,                   /* T_COMMA  */
  YYSYMBOL_T_COLON = 19,                   /* T_COLON  */
  YYSYMBOL_T_LPAREN = 20,                  /* T_LPAREN  */
  YYSYMBOL_T_RPAREN = 21,                  /* T_RPAREN  */
  YYSYMBOL_T_LBRACE = 22,                  /* T_LBRACE  */
  YYSYMBOL_T_RBRACE = 23,                  /* T_RBRACE  */
  YYSYMBOL_T_LBRACK = 24,                  /* T_LBRACK  */
  YYSYMBOL_T_RBRACK = 25,                  /* T_RBRACK  */
  YYSYMBOL_T_EQUAL = 26,                   /* T_EQUAL  */
  YYSYMBOL_T_HASH = 27,                    /* T_HASH  */
  YYSYMBOL_T_PERCENT = 28,                 /* T_PERCENT  */
  YYSYMBOL_T_ORIGIN = 29,                  /* T_ORIGIN  */
  YYSYMBOL_T_VAR8 = 30,                    /* T_VAR8  */
  YYSYMBOL_T_VAR16 = 31,                   /* T_VAR16  */
  YYSYMBOL_T_ALIAS8 = 32,                  /* T_ALIAS8  */
  YYSYMBOL_T_ALIAS16 = 33,                 /* T_ALIAS16  */
  YYSYMBOL_T_ASCII = 34,                   /* T_ASCII  */
  YYSYMBOL_UNARY = 35,                     /* UNARY  */
  YYSYMBOL_YYACCEPT = 36,                  /* $accept  */
  YYSYMBOL_program = 37,                   /* program  */
  YYSYMBOL_directive = 38,                 /* directive  */
  YYSYMBOL_number = 39,                    /* number  */
  YYSYMBOL_paren_expr = 40,                /* paren_expr  */
  YYSYMBOL_expression = 41,                /* expression  */
  YYSYMBOL_label = 42,                     /* label  */
  YYSYMBOL_operand = 43,                   /* operand  */
  YYSYMBOL_instruction = 44,               /* instruction  */
  YYSYMBOL_statement = 45,                 /* statement  */
  YYSYMBOL_statement_with_endl = 46,       /* statement_with_endl  */
  YYSYMBOL_statements = 47,                /* statements  */
  YYSYMBOL_imm = 48,                       /* imm  */
  YYSYMBOL_mem = 49                        /* mem  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


/* Second part of user prologue.  */
#line 134 "parser.y"

	enum class Mode : unsigned
	{
		None,
		Memory,
		Register,
		Immediate,
	};

	enum class Addr : unsigned
	{
		Direct,
		Indirect,
	};

	enum class Rsel : unsigned
	{
		None,
		R0,
		R1,
		R2
	};

	enum class Size : unsigned
	{
		None,
		WordAll,
		ByteLow,
		ByteHigh,
	};

	enum class Opcode : unsigned
	{
		Nop,

		Add,
		Subtract,
		Multiply,
		Divide,

		Xor,
		Or,
		And,
		Not,
		Compl,

		Load,
		Unload,
		Transfer,

		EnableInts,
		DisableInts,

		Interrupt,
		Reset,

		Push,
		Pop,
	};

	struct ModRM
	{
		Rsel rsel;
		Mode mode1, mode2;
		Addr addr1, addr2;

		constexpr auto from_byte(std::uint8_t byte) noexcept
		{
			rsel  = static_cast<Rsel>((byte & 0b11000000) >> 6);
			mode1 = static_cast<Mode>((byte & 0b00110000) >> 4);
			mode2 = static_cast<Mode>((byte & 0b00001100) >> 2);
			addr1 = static_cast<Addr>((byte & 0b00000010) >> 1);
			addr2 = static_cast<Addr>((byte & 0b00000001) >> 0);
		}

		constexpr auto to_byte(void) noexcept
		{
			auto   byte  =  static_cast<std::uint8_t>(0);
				   byte |= (static_cast<std::uint8_t>(rsel)  << 6);
				   byte |= (static_cast<std::uint8_t>(mode1) << 4);
				   byte |= (static_cast<std::uint8_t>(mode2) << 2);
				   byte |= (static_cast<std::uint8_t>(addr1) << 1);
				   byte |= (static_cast<std::uint8_t>(addr2) << 0);
			return byte;
		}

		constexpr ModRM(std::uint8_t byte) noexcept
			{ from_byte(byte); }
	};

	struct Operand
	{
		ModRM modrm;

		union
		{
			std::uint8_t val8;
			std::uint16_t val16;
		};
	};

	struct Mnemonic
	{
		Mode mode;
		Addr addr;
		Opcode opcode;
	};

	struct Instruction
	{
		Mnemonic mnemonic;
		std::optional<Operand> operand1, operand2;
	};

#line 368 "parser.tab.cpp"


#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   121

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  36
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  14
/* YYNRULES -- Number of rules.  */
#define YYNRULES  44
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  79

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   290


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   251,   251,   253,   256,   262,   270,   278,   284,   294,
     295,   297,   298,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   314,   316,   317,   318,
     320,   321,   322,   324,   325,   326,   328,   329,   330,   332,
     333,   335,   336,   338,   339
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "T_EOF", "error", "\"invalid token\"", "T_ENDL", "T_INT", "T_REGISTER",
  "T_IDENTIFIER", "T_STRING", "T_PLUS", "T_MINUS", "T_TIMES", "T_DIVIDE",
  "T_LSHIFT", "T_RSHIFT", "T_AMPERSAND", "T_CARET", "T_TILDE", "T_PIPE",
  "T_COMMA", "T_COLON", "T_LPAREN", "T_RPAREN", "T_LBRACE", "T_RBRACE",
  "T_LBRACK", "T_RBRACK", "T_EQUAL", "T_HASH", "T_PERCENT", "T_ORIGIN",
  "T_VAR8", "T_VAR16", "T_ALIAS8", "T_ALIAS16", "T_ASCII", "UNARY",
  "$accept", "program", "directive", "number", "paren_expr", "expression",
  "label", "operand", "instruction", "statement", "statement_with_endl",
  "statements", "imm", "mem", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290
};
#endif

#define YYPACT_NINF (-36)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -36,    14,     7,   -36,   -36,    15,    29,    43,    53,    54,
      55,    57,   -36,   -36,   -36,    47,   -36,   -36,   -36,    -3,
      -3,     5,   -36,   -36,   -36,   -36,   -36,    38,    39,    56,
      67,    88,   -36,   -36,    42,    42,   -36,   -36,   -36,   -36,
      17,    42,    42,    42,    42,   -36,    42,    42,   -36,   -36,
      77,    59,   -36,    91,    91,    91,    91,   -36,   -36,    42,
      42,    42,    42,    42,    42,    42,    42,    42,   -36,   -36,
     107,   107,    -8,    -8,   -36,   -36,    44,    69,   101
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
      40,     0,     2,     1,    38,    30,     0,     0,     0,     0,
       0,     0,    34,    35,    33,     0,    39,    29,    26,     0,
       0,    31,    27,    28,     9,    10,     3,     0,     0,     0,
       0,     0,    37,    36,     0,     0,    41,    42,    43,    44,
       0,     0,     0,     0,     0,     8,     0,     0,    13,    14,
       0,     0,    32,     7,     6,     4,     5,    24,    25,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    11,    12,
      15,    16,    17,    18,    19,    20,    22,    21,    23
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -36,   -36,   -36,    -4,    -1,   -35,   -36,    81,   -36,   -36,
     -36,   -36,   -36,   -36
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,    12,    48,    49,    50,    13,    21,    14,    15,
      16,     2,    22,    23
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      51,    24,    26,    25,    63,    64,    53,    54,    55,    56,
       4,    57,    58,     5,     3,    36,    38,    34,    37,    39,
      17,    35,    17,    40,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    24,    18,    25,     6,     7,     8,     9,
      10,    11,    19,    20,    19,    20,    24,    32,    25,    27,
      33,    46,    59,    60,    61,    62,    63,    64,    47,    28,
      29,    30,    34,    31,    41,    42,    35,    59,    60,    61,
      62,    63,    64,    65,    66,    43,    67,    59,    60,    61,
      62,    63,    64,    65,    69,    59,    60,    61,    62,    63,
      64,    65,    66,    44,    67,    45,     0,     0,    68,    59,
      60,    61,    62,    63,    64,    65,    66,     0,    67,    59,
      60,    61,    62,    63,    64,    65,    66,    61,    62,    63,
      64,    52
};

static const yytype_int8 yycheck[] =
{
      35,     4,     6,     6,    12,    13,    41,    42,    43,    44,
       3,    46,    47,     6,     0,    19,    20,    20,    19,    20,
       5,    24,     5,    18,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     4,    19,     6,    29,    30,    31,    32,
      33,    34,    27,    28,    27,    28,     4,     0,     6,     6,
       3,     9,     8,     9,    10,    11,    12,    13,    16,     6,
       6,     6,    20,     6,    26,    26,    24,     8,     9,    10,
      11,    12,    13,    14,    15,    19,    17,     8,     9,    10,
      11,    12,    13,    14,    25,     8,     9,    10,    11,    12,
      13,    14,    15,    26,    17,     7,    -1,    -1,    21,     8,
       9,    10,    11,    12,    13,    14,    15,    -1,    17,     8,
       9,    10,    11,    12,    13,    14,    15,    10,    11,    12,
      13,    40
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    37,    47,     0,     3,     6,    29,    30,    31,    32,
      33,    34,    38,    42,    44,    45,    46,     5,    19,    27,
      28,    43,    48,    49,     4,     6,    39,     6,     6,     6,
       6,     6,     0,     3,    20,    24,    39,    40,    39,    40,
      18,    26,    26,    19,    26,     7,     9,    16,    39,    40,
      41,    41,    43,    41,    41,    41,    41,    41,    41,     8,
       9,    10,    11,    12,    13,    14,    15,    17,    21,    25,
      41,    41,    41,    41,    41,    41,    41,    41,    41
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    36,    37,    38,    38,    38,    38,    38,    38,    39,
      39,    40,    40,    41,    41,    41,    41,    41,    41,    41,
      41,    41,    41,    41,    41,    41,    42,    43,    43,    43,
      44,    44,    44,    45,    45,    45,    46,    46,    46,    47,
      47,    48,    48,    49,    49
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     4,     4,     4,     4,     3,     1,
       1,     3,     3,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     2,     1,     1,     1,
       1,     2,     4,     1,     1,     1,     2,     2,     1,     2,
       0,     2,     2,     2,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= T_EOF)
    {
      yychar = T_EOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: statements  */
#line 251 "parser.y"
                    { std::puts("Parsing..."); }
#line 1377 "parser.tab.cpp"
    break;

  case 3: /* directive: T_ORIGIN number  */
#line 253 "parser.y"
                           {
								write_head = static_cast<std::uint16_t>((yyvsp[0].ival));
						   }
#line 1385 "parser.tab.cpp"
    break;

  case 4: /* directive: T_ALIAS8 T_IDENTIFIER T_COLON expression  */
#line 256 "parser.y"
                                                            {
														//identifiers[$2] = std::make_pair(Qualifier::Value8, $4);
														contains(identifiers, (yyvsp[-2].sval), { Qualifier::Value8, (yyvsp[0].ival) }); 
														code[write_head] = static_cast<std::uint8_t>((yyvsp[0].ival));
														++write_head;
													}
#line 1396 "parser.tab.cpp"
    break;

  case 5: /* directive: T_ALIAS16 T_IDENTIFIER T_EQUAL expression  */
#line 262 "parser.y"
                                                             {
														//identifiers[$2] = std::make_pair(Qualifier::Value16, $4);
														contains(identifiers, (yyvsp[-2].sval), { Qualifier::Value16, (yyvsp[0].ival) });
														code[write_head] = static_cast<std::uint8_t>((yyvsp[0].ival) & 0x00FF);
														++write_head;
														code[write_head] = static_cast<std::uint8_t>((yyvsp[0].ival) & 0xFF00);
														++write_head;
													}
#line 1409 "parser.tab.cpp"
    break;

  case 6: /* directive: T_VAR16 T_IDENTIFIER T_EQUAL expression  */
#line 270 "parser.y"
                                                           {
														//identifiers[$2] = std::make_pair(Qualifier::Variable16, $4);
														contains(identifiers, (yyvsp[-2].sval), { Qualifier::Variable16, write_head });
														code[write_head] = static_cast<std::uint8_t>((yyvsp[0].ival) & 0x00FF);
														++write_head;
														code[write_head] = static_cast<std::uint8_t>((yyvsp[0].ival) & 0xFF00);
														++write_head;
												   }
#line 1422 "parser.tab.cpp"
    break;

  case 7: /* directive: T_VAR8 T_IDENTIFIER T_EQUAL expression  */
#line 278 "parser.y"
                                                          {
													//identifiers[$2] = std::make_pair(Qualifier::Variable8, $4);
													contains(identifiers, (yyvsp[-2].sval), { Qualifier::Variable8, write_head });
													code[write_head] = static_cast<std::uint8_t>((yyvsp[0].ival));
													++write_head;
												  }
#line 1433 "parser.tab.cpp"
    break;

  case 8: /* directive: T_ASCII T_IDENTIFIER T_STRING  */
#line 284 "parser.y"
                                                 {
											identifiers[(yyvsp[-1].sval)] = std::make_pair(Qualifier::Ascii, write_head);

											auto chars = (yyvsp[0].sval);
											auto count = (std::strlen(chars) + 1);
											std::memcpy(&code[write_head], &chars, count);
											write_head += static_cast<std::uint16_t>(count);
										 }
#line 1446 "parser.tab.cpp"
    break;

  case 9: /* number: T_INT  */
#line 294 "parser.y"
                     { (yyval.ival) = (yyvsp[0].ival);                         }
#line 1452 "parser.tab.cpp"
    break;

  case 10: /* number: T_IDENTIFIER  */
#line 295 "parser.y"
                             { (yyval.ival) = ident2int((yyvsp[0].sval), identifiers); }
#line 1458 "parser.tab.cpp"
    break;

  case 11: /* paren_expr: T_LPAREN expression T_RPAREN  */
#line 297 "parser.y"
                                         { (yyval.ival) = (yyvsp[-1].ival); }
#line 1464 "parser.tab.cpp"
    break;

  case 12: /* paren_expr: T_LBRACK expression T_RBRACK  */
#line 298 "parser.y"
                                                     { (yyval.ival) = (yyvsp[-1].ival); }
#line 1470 "parser.tab.cpp"
    break;

  case 13: /* expression: number  */
#line 300 "parser.y"
                                                              { (yyval.ival) =  (yyvsp[0].ival);       }
#line 1476 "parser.tab.cpp"
    break;

  case 14: /* expression: paren_expr  */
#line 301 "parser.y"
                                                                      { (yyval.ival) =  (yyvsp[0].ival);       }
#line 1482 "parser.tab.cpp"
    break;

  case 15: /* expression: expression T_PLUS expression  */
#line 302 "parser.y"
                                                                      { (yyval.ival) =  (yyvsp[-2].ival) +  (yyvsp[0].ival); }
#line 1488 "parser.tab.cpp"
    break;

  case 16: /* expression: expression T_MINUS expression  */
#line 303 "parser.y"
                                                                      { (yyval.ival) =  (yyvsp[-2].ival) -  (yyvsp[0].ival); }
#line 1494 "parser.tab.cpp"
    break;

  case 17: /* expression: expression T_TIMES expression  */
#line 304 "parser.y"
                                                                      { (yyval.ival) =  (yyvsp[-2].ival) *  (yyvsp[0].ival); }
#line 1500 "parser.tab.cpp"
    break;

  case 18: /* expression: expression T_DIVIDE expression  */
#line 305 "parser.y"
                                                                      { (yyval.ival) =  (yyvsp[-2].ival) /  (yyvsp[0].ival); }
#line 1506 "parser.tab.cpp"
    break;

  case 19: /* expression: expression T_LSHIFT expression  */
#line 306 "parser.y"
                                                                      { (yyval.ival) =  (yyvsp[-2].ival) << (yyvsp[0].ival); }
#line 1512 "parser.tab.cpp"
    break;

  case 20: /* expression: expression T_RSHIFT expression  */
#line 307 "parser.y"
                                                                      { (yyval.ival) =  (yyvsp[-2].ival) >> (yyvsp[0].ival); }
#line 1518 "parser.tab.cpp"
    break;

  case 21: /* expression: expression T_CARET expression  */
#line 308 "parser.y"
                                                                      { (yyval.ival) =  (yyvsp[-2].ival) ^  (yyvsp[0].ival); }
#line 1524 "parser.tab.cpp"
    break;

  case 22: /* expression: expression T_AMPERSAND expression  */
#line 309 "parser.y"
                                                                      { (yyval.ival) =  (yyvsp[-2].ival) &  (yyvsp[0].ival); }
#line 1530 "parser.tab.cpp"
    break;

  case 23: /* expression: expression T_PIPE expression  */
#line 310 "parser.y"
                                                                      { (yyval.ival) =  (yyvsp[-2].ival) |  (yyvsp[0].ival); }
#line 1536 "parser.tab.cpp"
    break;

  case 24: /* expression: T_MINUS expression  */
#line 311 "parser.y"
                                                                      { (yyval.ival) = -(yyvsp[0].ival);       }
#line 1542 "parser.tab.cpp"
    break;

  case 25: /* expression: T_TILDE expression  */
#line 312 "parser.y"
                                                                      { (yyval.ival) = ~(yyvsp[0].ival);       }
#line 1548 "parser.tab.cpp"
    break;

  case 27: /* operand: imm  */
#line 316 "parser.y"
                    { (yyval.ival) = (yyvsp[0].ival); }
#line 1554 "parser.tab.cpp"
    break;

  case 28: /* operand: mem  */
#line 317 "parser.y"
                            { (yyval.ival) = (yyvsp[0].ival); }
#line 1560 "parser.tab.cpp"
    break;

  case 29: /* operand: T_REGISTER  */
#line 318 "parser.y"
                            { (yyval.ival) = (yyvsp[0].ival); }
#line 1566 "parser.tab.cpp"
    break;

  case 41: /* imm: T_HASH number  */
#line 335 "parser.y"
                       { (yyval.ival) = (yyvsp[0].ival); }
#line 1572 "parser.tab.cpp"
    break;

  case 42: /* imm: T_HASH paren_expr  */
#line 336 "parser.y"
                           { (yyval.ival) = (yyvsp[0].ival); }
#line 1578 "parser.tab.cpp"
    break;

  case 43: /* mem: T_PERCENT number  */
#line 338 "parser.y"
                          { (yyval.ival) = (yyvsp[0].ival); }
#line 1584 "parser.tab.cpp"
    break;

  case 44: /* mem: T_PERCENT paren_expr  */
#line 339 "parser.y"
                              { (yyval.ival) = (yyvsp[0].ival); }
#line 1590 "parser.tab.cpp"
    break;


#line 1594 "parser.tab.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= T_EOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == T_EOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 341 "parser.y"


int yyerror(const char* s)
{
	std::cout << s << '\n';
	return 0;
}

int __cdecl main(const int argc, const char** argv) noexcept
{
	switch (argc)
	{
	case 2:
	{
		//local string split function to use
		auto split = [&](const std::string& input, const std::string& delim) noexcept
		{
			std::size_t pos_start = 0, pos_end, delim_len = delim.length();
			std::string token;
			std::vector<std::string> res;

			while ((pos_end = input.find(delim, pos_start)) != std::string::npos)
			{
				token = input.substr(pos_start, pos_end - pos_start);
				pos_start = pos_end + delim_len;
				res.emplace_back(token);
			}

			res.emplace_back(input.substr(pos_start));
			return res;
		};

		//fetch the filepath off the command line
		std::string fp(argv[1]);

		//open the file specified
		std::ifstream file(fp);

		//assert that the file supplied is readable
		if (!file.good())
		{
			std::cerr << "Failed to open the specified file";
			return 2;
		}

		//obtain the overall file size
		const auto length = std::filesystem::file_size(fp);

		//allocate and initialize a properly sized buffer
		std::string buffer(length, '\0');

		//read the entire file into the buffer
		file.read(buffer.data(), length);
		
		//ties together each basic component of a macro
		struct Macro { std::string identifier, statements; std::vector<std::string> arguments; };

		//local cache of macros that are found
		std::map<std::string, Macro> macro_list{};

		//detects a valid macro declaration
		std::regex rx_macro_decl(R"(\.macro ([a-zA-Z_]\w*)\(([^\)]*)\)\s?\{([^\}]*)\})");

		//detects a valid macro invokation
		std::regex rx_macro_invk(R"(([a-zA-Z_]\w*)\([^\)]*\))");

		//will store each of the string regex matches that are made during the search
		std::smatch matches{};

		//discover all macro declarations present in the source file
		while (std::regex_search(buffer, matches, rx_macro_decl))
		{
			//move the argument contents to a new string
			auto extracted = std::move(matches[2].str());

			//remove any whitespace contained in the arguments list
			std::erase(std::remove_if(extracted.begin(), extracted.end(),
				[&](char c) { return std::isspace(static_cast<unsigned char>(c)); }), extracted.end());

			//tokenize the string using the commas as delimiters
			if (macro_list.try_emplace(std::move(matches[1].str()), { matches[1].str(), matches[3].str(), split(extracted, ",") }).first != macro_list.end()) [[likely]]
				;
			else
			{
				std::cerr << "Macro " << matches[1].str() << " was multiply defined";
				return 100;
			}
		}

		//then erase the macro source code from the file
		std::regex_replace(buffer, rx_macro_decl, "");

		//discover all macro invokations present in the source file
		while (std::regex_search(buffer, matches, rx_macro_invk))
		{
			//move the argument contents to a new string
			auto extracted = std::move(matches[2].str());

			//remove any whitespace contained in the arguments list
			std::erase(std::remove_if(extracted.begin(), extracted.end(),
				[&](char c) { return std::isspace(static_cast<unsigned char>(c)); }), extracted.end());

			//tokenize the string using the commas as delimiters
			auto args = split(extracted, ",");

			//dynamically assert that the invoked macro actually exists
			{
				try
				{
					//verify that the correct number of arguments was supplied
					auto&& str = matches[1].str();
					auto m = macro_list.at(str);
					auto s = m.arguments.size();
					auto a = args.size();

					if (a != s) [[unlikely]]
					{
						std::cerr << "Macro " << str << " was supplied " << a << " arguments, but takes " << s << " arguments";
						return 110;
					}
				}

				//in the case that it doesn't
				catch (std::out_of_range)
				{
					std::cerr << "Macro " << matches[1].str() << " was invoked, but not defined";
					return 120;
				}
			}
			
			//iterate over each of the arguments
			for (auto i = 0; i < args.size(); ++i)
			{
				std::regex_replace(buffer, macro_list.at(matches[1]).arguments[i], args[i]);
			}
		}

		//lex and parse the file contents
		do
		{
			yyparse();
		} while(!std::feof(yyin));
		break;
	}

	default:
		//otherwise throw an error
		std::cerr << "Usage: assemble [filepath]";
		return 1;
	}

	//indicate program success
	return 0;
}
