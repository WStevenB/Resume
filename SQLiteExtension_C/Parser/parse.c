/*
** 2000-05-29
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** Driver template for the LEMON parser generator.
**
** The "lemon" program processes an LALR(1) input grammar file, then uses
** this template to construct a parser.  The "lemon" program inserts text
** at each "%%" line.  Also, any "P-a-r-s-e" identifer prefix (without the
** interstitial "-" characters) contained in this template is changed into
** the value of the %name directive from the grammar.  Otherwise, the content
** of this template is copied straight through into the generate parser
** source file.
**
** The following is the concatenation of all %include directives from the
** input grammar file:
*/
#include <stdio.h>
/************ Begin %include sections from the grammar ************************/
#line 48 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"

#include "sqliteInt.h"

/*
** Disable all error recovery processing in the parser push-down
** automaton.
*/
#define YYNOERRORRECOVERY 1

/*
** Make yytestcase() the same as testcase()
*/
#define yytestcase(X) testcase(X)

/*
** Indicate that sqlite3ParserFree() will never be called with a null
** pointer.
*/
#define YYPARSEFREENEVERNULL 1

/*
** Alternative datatype for the argument to the malloc() routine passed
** into sqlite3ParserAlloc().  The default is size_t.
*/
#define YYMALLOCARGTYPE  u64

/*
** An instance of this structure holds information about the
** LIMIT clause of a SELECT statement.
*/
struct LimitVal {
  Expr *pLimit;    /* The LIMIT expression.  NULL if there is no limit */
  Expr *pOffset;   /* The OFFSET expression.  NULL if there is none */
};

/*
** An instance of the following structure describes the event of a
** TRIGGER.  "a" is the event type, one of TK_UPDATE, TK_INSERT,
** TK_DELETE, or TK_INSTEAD.  If the event is of the form
**
**      UPDATE ON (a,b,c)
**
** Then the "b" IdList records the list "a,b,c".
*/
struct TrigEvent { int a; IdList * b; };

/*
** Disable lookaside memory allocation for objects that might be
** shared across database connections.
*/
static void disableLookaside(Parse *pParse){
  pParse->disableLookaside++;
  pParse->db->lookaside.bDisable++;
}

#line 402 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"

  /*
  ** For a compound SELECT statement, make sure p->pPrior->pNext==p for
  ** all elements in the list.  And make sure list length does not exceed
  ** SQLITE_LIMIT_COMPOUND_SELECT.
  */
  static void parserDoubleLinkSelect(Parse *pParse, Select *p){
    if( p->pPrior ){
      Select *pNext = 0, *pLoop;
      int mxSelect, cnt = 0;
      for(pLoop=p; pLoop; pNext=pLoop, pLoop=pLoop->pPrior, cnt++){
        pLoop->pNext = pNext;
        pLoop->selFlags |= SF_Compound;
      }
      if( (p->selFlags & SF_MultiValue)==0 && 
        (mxSelect = pParse->db->aLimit[SQLITE_LIMIT_COMPOUND_SELECT])>0 &&
        cnt>mxSelect
      ){
        sqlite3ErrorMsg(pParse, "too many terms in compound SELECT");
      }
    }
  }
#line 830 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"

  /* This is a utility routine used to set the ExprSpan.zStart and
  ** ExprSpan.zEnd values of pOut so that the span covers the complete
  ** range of text beginning with pStart and going to the end of pEnd.
  */
  static void spanSet(ExprSpan *pOut, Token *pStart, Token *pEnd){
    pOut->zStart = pStart->z;
    pOut->zEnd = &pEnd->z[pEnd->n];
  }

  /* Construct a new Expr object from a single identifier.  Use the
  ** new Expr to populate pOut.  Set the span of pOut to be the identifier
  ** that created the expression.
  */
  static void spanExpr(ExprSpan *pOut, Parse *pParse, int op, Token t){
    Expr *p = sqlite3DbMallocRawNN(pParse->db, sizeof(Expr)+t.n+1);
    if( p ){
      memset(p, 0, sizeof(Expr));
      p->op = (u8)op;
      p->flags = EP_Leaf;
      p->iAgg = -1;
      p->u.zToken = (char*)&p[1];
      memcpy(p->u.zToken, t.z, t.n);
      p->u.zToken[t.n] = 0;
      if( sqlite3Isquote(p->u.zToken[0]) ){
        if( p->u.zToken[0]=='"' ) p->flags |= EP_DblQuoted;
        sqlite3Dequote(p->u.zToken);
      }
#if SQLITE_MAX_EXPR_DEPTH>0
      p->nHeight = 1;
#endif  
    }
    pOut->pExpr = p;
    pOut->zStart = t.z;
    pOut->zEnd = &t.z[t.n];
  }
#line 947 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"

  /* This routine constructs a binary expression node out of two ExprSpan
  ** objects and uses the result to populate a new ExprSpan object.
  */
  static void spanBinaryExpr(
    Parse *pParse,      /* The parsing context.  Errors accumulate here */
    int op,             /* The binary operation */
    ExprSpan *pLeft,    /* The left operand, and output */
    ExprSpan *pRight    /* The right operand */
  ){
    pLeft->pExpr = sqlite3PExpr(pParse, op, pLeft->pExpr, pRight->pExpr, 0);
    pLeft->zEnd = pRight->zEnd;
  }

  /* If doNot is true, then add a TK_NOT Expr-node wrapper around the
  ** outside of *ppExpr.
  */
  static void exprNot(Parse *pParse, int doNot, ExprSpan *pSpan){
    if( doNot ){
      pSpan->pExpr = sqlite3PExpr(pParse, TK_NOT, pSpan->pExpr, 0, 0);
    }
  }
#line 1021 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"

  /* Construct an expression node for a unary postfix operator
  */
  static void spanUnaryPostfix(
    Parse *pParse,         /* Parsing context to record errors */
    int op,                /* The operator */
    ExprSpan *pOperand,    /* The operand, and output */
    Token *pPostOp         /* The operand token for setting the span */
  ){
    pOperand->pExpr = sqlite3PExpr(pParse, op, pOperand->pExpr, 0, 0);
    pOperand->zEnd = &pPostOp->z[pPostOp->n];
  }                           
#line 1038 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"

  /* A routine to convert a binary TK_IS or TK_ISNOT expression into a
  ** unary TK_ISNULL or TK_NOTNULL expression. */
  static void binaryToUnaryIfNull(Parse *pParse, Expr *pY, Expr *pA, int op){
    sqlite3 *db = pParse->db;
    if( pA && pY && pY->op==TK_NULL ){
      pA->op = (u8)op;
      sqlite3ExprDelete(db, pA->pRight);
      pA->pRight = 0;
    }
  }
#line 1066 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"

  /* Construct an expression node for a unary prefix operator
  */
  static void spanUnaryPrefix(
    ExprSpan *pOut,        /* Write the new expression node here */
    Parse *pParse,         /* Parsing context to record errors */
    int op,                /* The operator */
    ExprSpan *pOperand,    /* The operand */
    Token *pPreOp         /* The operand token for setting the span */
  ){
    pOut->zStart = pPreOp->z;
    pOut->pExpr = sqlite3PExpr(pParse, op, pOperand->pExpr, 0, 0);
    pOut->zEnd = pOperand->zEnd;
  }
#line 1278 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"

  /* Add a single new term to an ExprList that is used to store a
  ** list of identifiers.  Report an error if the ID list contains
  ** a COLLATE clause or an ASC or DESC keyword, except ignore the
  ** error while parsing a legacy schema.
  */
  static ExprList *parserAddExprIdListTerm(
    Parse *pParse,
    ExprList *pPrior,
    Token *pIdToken,
    int hasCollate,
    int sortOrder
  ){
    ExprList *p = sqlite3ExprListAppend(pParse, pPrior, 0);
    if( (hasCollate || sortOrder!=SQLITE_SO_UNDEFINED)
        && pParse->db->init.busy==0
    ){
      sqlite3ErrorMsg(pParse, "syntax error after column name \"%.*s\"",
                         pIdToken->n, pIdToken->z);
    }
    sqlite3ExprListSetName(pParse, p, pIdToken, 1);
    return p;
  }
#line 231 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols
** in a format understandable to "makeheaders".  This section is blank unless
** "lemon" is run with the "-m" command-line option.
***************** Begin makeheaders token definitions *************************/
/**************** End makeheaders token definitions ***************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    YYNOCODE           is a number of type YYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    YYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    sqlite3ParserTOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal 
**                       symbols.
**    YYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is sqlite3ParserTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    sqlite3ParserARG_SDECL     A static variable declaration for the %extra_argument
**    sqlite3ParserARG_PDECL     A parameter declaration for the %extra_argument
**    sqlite3ParserARG_STORE     Code to store %extra_argument into yypParser
**    sqlite3ParserARG_FETCH     Code to extract %extra_argument from yypParser
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YY_MAX_SHIFT       Maximum value for shift actions
**    YY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    YY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    YY_MIN_REDUCE      Maximum value for reduce actions
**    YY_ERROR_ACTION    The yy_action[] code for syntax error
**    YY_ACCEPT_ACTION   The yy_action[] code for accept
**    YY_NO_ACTION       The yy_action[] code for no-op
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define YYCODETYPE unsigned char
#define YYNOCODE 254
#define YYACTIONTYPE unsigned short int
#define YYWILDCARD 96
#define sqlite3ParserTOKENTYPE Token
typedef union {
  int yyinit;
  sqlite3ParserTOKENTYPE yy0;
  Select* yy3;
  ExprList* yy14;
  With* yy59;
  SrcList* yy65;
  Expr* yy132;
  int yy328;
  ExprSpan yy346;
  struct TrigEvent yy378;
  IdList* yy408;
  struct {int value; int mask;} yy429;
  TriggerStep* yy473;
  struct LimitVal yy476;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define sqlite3ParserARG_SDECL Parse *pParse;
#define sqlite3ParserARG_PDECL ,Parse *pParse
#define sqlite3ParserARG_FETCH Parse *pParse = yypParser->pParse
#define sqlite3ParserARG_STORE yypParser->pParse = pParse
#define YYFALLBACK 1
#define YYNSTATE             459
#define YYNRULE              334
#define YY_MAX_SHIFT         458
#define YY_MIN_SHIFTREDUCE   671
#define YY_MAX_SHIFTREDUCE   1004
#define YY_MIN_REDUCE        1005
#define YY_MAX_REDUCE        1338
#define YY_ERROR_ACTION      1339
#define YY_ACCEPT_ACTION     1340
#define YY_NO_ACTION         1341
/************* End control #defines *******************************************/

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= YY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between YY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and YY_MAX_SHIFTREDUCE           reduce by rule N-YY_MIN_SHIFTREDUCE.
**
**   N between YY_MIN_REDUCE            Reduce by rule N-YY_MIN_REDUCE
**     and YY_MAX_REDUCE
**
**   N == YY_ERROR_ACTION               A syntax error has occurred.
**
**   N == YY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == YY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as either:
**
**    (A)   N = yy_action[ yy_shift_ofst[S] + X ]
**    (B)   N = yy_default[S]
**
** The (A) formula is preferred.  The B formula is used instead if:
**    (1)  The yy_shift_ofst[S]+X value is out of range, or
**    (2)  yy_lookahead[yy_shift_ofst[S]+X] is not equal to X, or
**    (3)  yy_shift_ofst[S] equal YY_SHIFT_USE_DFLT.
** (Implementation note: YY_SHIFT_USE_DFLT is chosen so that
** YY_SHIFT_USE_DFLT+X will be out of range for all possible lookaheads X.
** Hence only tests (1) and (2) need to be evaluated.)
**
** The formulas above are for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define YY_ACTTAB_COUNT (1581)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */   328,  837,  354,  830,    5,  205,  205,  824,  101,  102,
 /*    10 */    92,  847,  847,  859,  862,  851,  851,   99,   99,  100,
 /*    20 */   100,  100,  100,  302,   98,   98,   98,   98,   97,   97,
 /*    30 */    96,   96,   96,   95,  354,  328,  982,  982,  134,  829,
 /*    40 */   829,  831,  701,  101,  102,   92,  847,  847,  859,  862,
 /*    50 */   851,  851,   99,   99,  100,  100,  100,  100,  227,   98,
 /*    60 */    98,   98,   98,   97,   97,   96,   96,   96,   95,  354,
 /*    70 */    97,   97,   96,   96,   96,   95,  354,  794,  982,  982,
 /*    80 */   328,   96,   96,   96,   95,  354,  795,   77,  101,  102,
 /*    90 */    92,  847,  847,  859,  862,  851,  851,   99,   99,  100,
 /*   100 */   100,  100,  100,  453,   98,   98,   98,   98,   97,   97,
 /*   110 */    96,   96,   96,   95,  354, 1340,  157,  157,    2,  328,
 /*   120 */   704,  148,  446,   53,   53,  952,  357,  101,  102,   92,
 /*   130 */   847,  847,  859,  862,  851,  851,   99,   99,  100,  100,
 /*   140 */   100,  100,  103,   98,   98,   98,   98,   97,   97,   96,
 /*   150 */    96,   96,   95,  354,  963,  963,  328,  332, 1297,  431,
 /*   160 */   416,   62,   95,  354,  101,  102,   92,  847,  847,  859,
 /*   170 */   862,  851,  851,   99,   99,  100,  100,  100,  100,   61,
 /*   180 */    98,   98,   98,   98,   97,   97,   96,   96,   96,   95,
 /*   190 */   354,  328,   94,   91,  180,  279,  964,  965,  822,  101,
 /*   200 */   102,   92,  847,  847,  859,  862,  851,  851,   99,   99,
 /*   210 */   100,  100,  100,  100,  703,   98,   98,   98,   98,   97,
 /*   220 */    97,   96,   96,   96,   95,  354,  328,  943, 1333,  387,
 /*   230 */   709, 1333,  384,  378,  101,  102,   92,  847,  847,  859,
 /*   240 */   862,  851,  851,   99,   99,  100,  100,  100,  100,  303,
 /*   250 */    98,   98,   98,   98,   97,   97,   96,   96,   96,   95,
 /*   260 */   354,  328,  943, 1334,  337,  702, 1334,  244,  415,  101,
 /*   270 */   102,   92,  847,  847,  859,  862,  851,  851,   99,   99,
 /*   280 */   100,  100,  100,  100,  341,   98,   98,   98,   98,   97,
 /*   290 */    97,   96,   96,   96,   95,  354,  328,  450,  450,  450,
 /*   300 */   838,  941,  742,  742,  101,  102,   92,  847,  847,  859,
 /*   310 */   862,  851,  851,   99,   99,  100,  100,  100,  100,  303,
 /*   320 */    98,   98,   98,   98,   97,   97,   96,   96,   96,   95,
 /*   330 */   354,  328, 1283,  952,  357,  823,  941, 1001,  321,  101,
 /*   340 */   102,   92,  847,  847,  859,  862,  851,  851,   99,   99,
 /*   350 */   100,  100,  100,  100,  350,   98,   98,   98,   98,   97,
 /*   360 */    97,   96,   96,   96,   95,  354,  328,  740,  740,   94,
 /*   370 */    91,  180,  822,  302,  101,  102,   92,  847,  847,  859,
 /*   380 */   862,  851,  851,   99,   99,  100,  100,  100,  100,  926,
 /*   390 */    98,   98,   98,   98,   97,   97,   96,   96,   96,   95,
 /*   400 */   354,  328,  353,  353,  353,  414,  149,  755,  755,  101,
 /*   410 */   102,   92,  847,  847,  859,  862,  851,  851,   99,   99,
 /*   420 */   100,  100,  100,  100,  974,   98,   98,   98,   98,   97,
 /*   430 */    97,   96,   96,   96,   95,  354,  328,  270,  338,  963,
 /*   440 */   963,  160,  963,  963,  101,  102,   92,  847,  847,  859,
 /*   450 */   862,  851,  851,   99,   99,  100,  100,  100,  100,  364,
 /*   460 */    98,   98,   98,   98,   97,   97,   96,   96,   96,   95,
 /*   470 */   354,  155,  442,  452,  737,  716,  364,  363,  252,  970,
 /*   480 */   328,  964,  965,  737,  964,  965,   80,  711,  101,  102,
 /*   490 */    92,  847,  847,  859,  862,  851,  851,   99,   99,  100,
 /*   500 */   100,  100,  100,  174,   98,   98,   98,   98,   97,   97,
 /*   510 */    96,   96,   96,   95,  354,  328,  848,  848,  860,  863,
 /*   520 */   822,   83,  298,  101,   90,   92,  847,  847,  859,  862,
 /*   530 */   851,  851,   99,   99,  100,  100,  100,  100,  364,   98,
 /*   540 */    98,   98,   98,   97,   97,   96,   96,   96,   95,  354,
 /*   550 */   328,  906,  906,  390,   84,  410,   82,  346,  382,  102,
 /*   560 */    92,  847,  847,  859,  862,  851,  851,   99,   99,  100,
 /*   570 */   100,  100,  100,  453,   98,   98,   98,   98,   97,   97,
 /*   580 */    96,   96,   96,   95,  354,  328,  339,  852,  676,  677,
 /*   590 */   678,  343,  425,   52,   52,   92,  847,  847,  859,  862,
 /*   600 */   851,  851,   99,   99,  100,  100,  100,  100,   73,   98,
 /*   610 */    98,   98,   98,   97,   97,   96,   96,   96,   95,  354,
 /*   620 */    88,  448,  760,    3, 1210,   25,  178,  759,  822,  818,
 /*   630 */   963,  963,  779,   88,  448,  205,    3,  214,  171,  289,
 /*   640 */   408,  284,  407,  201,  922,  232,  270,  301,   85,   86,
 /*   650 */   282,  300,  264,  368,  253,   87,  355,  355,  963,  963,
 /*   660 */   243,   85,   86,  244,  415,  453,  982,  451,   87,  355,
 /*   670 */   355,   59,  964,  965,  196,  458,  673,  405,  402,  401,
 /*   680 */   451,  245,  723,  116,  437,   53,   53,  230,  400,  923,
 /*   690 */   750,  134,  119,  134,  389,  406,  837,  437,  455,  454,
 /*   700 */   964,  965,  824,  724,  179,  924,  134,  743,  982,  837,
 /*   710 */   223,  455,  454,  779,  934,  824,  376,  744, 1278, 1278,
 /*   720 */   453,  347,  453,   23,  453,  699,   88,  448,  344,    3,
 /*   730 */   779,  432,  900,  412,  829,  829,  831,  832,   19,  419,
 /*   740 */    53,   53,   32,   32,   10,   10,    1,  829,  829,  831,
 /*   750 */   832,   19,  720,  258,   85,   86,  764,  763,  340,    9,
 /*   760 */     9,   87,  355,  355,  122,  247,  122,  699,   94,   91,
 /*   770 */   180,  376,  358,  451,  420,  422,  431,  433,  413,  963,
 /*   780 */   963,   88,  448,  229,    3,  179,  100,  100,  100,  100,
 /*   790 */   437,   98,   98,   98,   98,   97,   97,   96,   96,   96,
 /*   800 */    95,  354,  837,  159,  455,  454,  818,  122,  824,   85,
 /*   810 */    86,  779,  423,  134,  248,  323,   87,  355,  355,  963,
 /*   820 */   963,  964,  965,  202,  177,  255,  765,  257,  451,  264,
 /*   830 */   373,  263,  963,  963,  251,  919,   78,  448,  233,    3,
 /*   840 */   829,  829,  831,  832,   19,  437,   98,   98,   98,   98,
 /*   850 */    97,   97,   96,   96,   96,   95,  354,  837,  892,  455,
 /*   860 */   454,  964,  965,  824,   85,   86,  963,  963,  122,  453,
 /*   870 */   270,   87,  355,  355,  964,  965,  270,  719,  256,  228,
 /*   880 */   270,  453,  793,  451,  318,  317,  316,  217,  314,   53,
 /*   890 */    53,  686,  374,  226,  226,  829,  829,  831,  832,   19,
 /*   900 */   437,   53,   53,  453,  752,   24,  418,  105,  964,  965,
 /*   910 */   372,  231,  837,  794,  455,  454,  418,  925,  824,  352,
 /*   920 */   351,  361,  795,   53,   53,  431,  421,  963,  963,   94,
 /*   930 */    91,  180,  173,  172,  182,  781,  250,  431,  430, 1307,
 /*   940 */   705,  705,  732,  196,  453,  249,  405,  402,  401,  382,
 /*   950 */   829,  829,  831,  832,   19,  453,  184,  400,  939,  431,
 /*   960 */   411,  792,  689,  185,   10,   10,  134,  183,  259,  964,
 /*   970 */   965,  359,  989,  352,  351,   53,   53,  822,  342,  987,
 /*   980 */   837,  988,  830,  963,  963,  914,  824,  270,  329,  100,
 /*   990 */   100,  100,  100,   93,   98,   98,   98,   98,   97,   97,
 /*  1000 */    96,   96,   96,   95,  354,  990,  818,  990,   94,   91,
 /*  1010 */   180,  426,  397,  134,  453,  922,  362,  453,  829,  829,
 /*  1020 */   831,  913,  894,  200,  919,  964,  965,  718,  890,  264,
 /*  1030 */   373,  263,  371,  159,   10,   10,  262,   10,   10,  894,
 /*  1040 */   896,  226,  226,  348,  882,  327,  320, 1003,  235,  453,
 /*  1050 */   159,  428,  780,  453,  418,  270,  385,  122,  313,  990,
 /*  1060 */   923,  990,  324,  453,  236,  266,  236,  366,  453,   36,
 /*  1070 */    36,  270,  453,   37,   37,  382,  924,  453,  436,  950,
 /*  1080 */     2,  374, 1217,   12,   12,  899,  453,  899,   27,   27,
 /*  1090 */   453,   76,   38,   38,  815,  453,  749,   39,   39,  383,
 /*  1100 */   333,  894,  453,  695,  334,  282,   40,   40,  453,  271,
 /*  1110 */    41,   41,  745,  453,  274,   42,   42,  453,  967,  453,
 /*  1120 */  1004,  453,   28,   28,  453,  270,  453,  122,   29,   29,
 /*  1130 */   453,  760,  453,   31,   31,  453,  759,   43,   43,   44,
 /*  1140 */    44,   45,   45,  453,   46,   46,   11,   11,  453,  276,
 /*  1150 */    47,   47,  107,  107,  453,   48,   48,  453,  278,  453,
 /*  1160 */   967,  453,  234,   49,   49,  453,  269,  453,   33,   33,
 /*  1170 */   453,  714,  453,  917,   50,   50,  453,   51,   51,   34,
 /*  1180 */    34,  124,  124,  453,  380,  125,  125,  126,  126,  453,
 /*  1190 */    57,   57,   35,   35,  453,  277,  108,  108,  453,  320,
 /*  1200 */   942,  200,  453,   54,   54,  453,  122,  453,  134,  109,
 /*  1210 */   109,  453,  981,  453,  110,  110,  365,  453,  106,  106,
 /*  1220 */   453,  381,  123,  123,  453,  121,  121,  114,  114,  712,
 /*  1230 */   453,  113,  113,  111,  111,  453,  733,  112,  112,  903,
 /*  1240 */    56,   56,  453,  902,   58,   58,  199,  198,  197,   20,
 /*  1250 */    55,   55,  449,  334,  748,   26,   26,  388,  692,  439,
 /*  1260 */   443,  447,   30,   30,   21,  335,  135,  272,  120,  275,
 /*  1270 */   164,  712,  889, 1004,  225,  177,  938,  820,  333,  322,
 /*  1280 */   204,  375,  377,  265,  204,  204,   76,  122,  722,  721,
 /*  1290 */   833,  122,  122,  122,  122,  729,  730,  398,  280,  288,
 /*  1300 */   210,   76,  757,  788,  885,   79,  204,  210,  717,  169,
 /*  1310 */   287,  898,  697,  898,  889,  118,  283,  770,  239,  786,
 /*  1320 */   821,  766,  777,  434,  435,  304,  305,  393,  828,  700,
 /*  1330 */   291,  293,  833,  220,  694,  683,  682,  684,  957,  295,
 /*  1340 */   161,  319,    7,  810,  367,  254,  261,  916,  379,  175,
 /*  1350 */   297,  438,  311,  170,  998,  960,  137,  933,  207,  995,
 /*  1360 */   931,  336,   60,   63,  403,  146,  158,  286,  887,  886,
 /*  1370 */   716,  132,   73,  369,  370,  396,  386,  187,  162,  191,
 /*  1380 */    68,  392,  778,  268,  139,  901,  221,  192,  156,  141,
 /*  1390 */   394,  273,  193,  325,  685,  142,  143,  807,  144,  150,
 /*  1400 */   817,  409,  194,  345,  736,  735,  918,  734,  881,  714,
 /*  1410 */   424,   72,  206,    6,  708,  299,  326,  707,  349,  285,
 /*  1420 */   727,   81,  706,  972,  429,  774,  775,  104,  726,  773,
 /*  1430 */   290,   75,  215,   74,  441,  445,  292,  427,  294,  691,
 /*  1440 */    22,  772,  296,  958,  456,  216,  218,  219,  457,  117,
 /*  1450 */   867,  680,  679,  674,  672,  356,  237,  168,  246,  127,
 /*  1460 */   181,  128,  360,  115,  138,  897,  310,  307,  756,  306,
 /*  1470 */   308,  309,  129,  816,  895,  130,  746,  240,  260,  140,
 /*  1480 */   912,  241,  242,  186,  145,   64,  147,  131,  915,  188,
 /*  1490 */    65,   66,   67,  189,  911,    8,   13,  190,  904,  204,
 /*  1500 */   267,  151,  992,  391,  152,  163,  688,  395,  287,  195,
 /*  1510 */   281,  153,  399,   69,   14,  404,  330,  136,  331,   15,
 /*  1520 */    70,  725,  238,  836,  835,  133,  865,  754,   71,   16,
 /*  1530 */   417,  758,    4,  787,  176,  222,  224,  154,  440,  203,
 /*  1540 */   782,  880,   79,   76,   17,  866,   18,  864,  921,  208,
 /*  1550 */   920,  209,  869,  947,  165,  953,  948,  211,  166,  444,
 /*  1560 */   868,  167,  212,  834,  315,  698,   89, 1007,  213,  312,
 /*  1570 */  1007, 1299, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007,
 /*  1580 */  1298,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    19,   95,   53,   97,   22,   24,   24,  101,   27,   28,
 /*    10 */    29,   30,   31,   32,   33,   34,   35,   36,   37,   38,
 /*    20 */    39,   40,   41,  153,   43,   44,   45,   46,   47,   48,
 /*    30 */    49,   50,   51,   52,   53,   19,   55,   55,   92,  133,
 /*    40 */   134,  135,  173,   27,   28,   29,   30,   31,   32,   33,
 /*    50 */    34,   35,   36,   37,   38,   39,   40,   41,   22,   43,
 /*    60 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
 /*    70 */    47,   48,   49,   50,   51,   52,   53,   61,   97,   97,
 /*    80 */    19,   49,   50,   51,   52,   53,   70,   26,   27,   28,
 /*    90 */    29,   30,   31,   32,   33,   34,   35,   36,   37,   38,
 /*   100 */    39,   40,   41,  153,   43,   44,   45,   46,   47,   48,
 /*   110 */    49,   50,   51,   52,   53,  145,  146,  147,  148,   19,
 /*   120 */   173,   22,  252,  173,  174,    1,    2,   27,   28,   29,
 /*   130 */    30,   31,   32,   33,   34,   35,   36,   37,   38,   39,
 /*   140 */    40,   41,   81,   43,   44,   45,   46,   47,   48,   49,
 /*   150 */    50,   51,   52,   53,   55,   56,   19,  158,  122,  209,
 /*   160 */   210,   24,   52,   53,   27,   28,   29,   30,   31,   32,
 /*   170 */    33,   34,   35,   36,   37,   38,   39,   40,   41,   79,
 /*   180 */    43,   44,   45,   46,   47,   48,   49,   50,   51,   52,
 /*   190 */    53,   19,  223,  224,  225,   23,   97,   98,  153,   27,
 /*   200 */    28,   29,   30,   31,   32,   33,   34,   35,   36,   37,
 /*   210 */    38,   39,   40,   41,  173,   43,   44,   45,   46,   47,
 /*   220 */    48,   49,   50,   51,   52,   53,   19,   22,   23,  230,
 /*   230 */    23,   26,  233,  153,   27,   28,   29,   30,   31,   32,
 /*   240 */    33,   34,   35,   36,   37,   38,   39,   40,   41,  153,
 /*   250 */    43,   44,   45,   46,   47,   48,   49,   50,   51,   52,
 /*   260 */    53,   19,   22,   23,  219,   23,   26,  119,  120,   27,
 /*   270 */    28,   29,   30,   31,   32,   33,   34,   35,   36,   37,
 /*   280 */    38,   39,   40,   41,  188,   43,   44,   45,   46,   47,
 /*   290 */    48,   49,   50,   51,   52,   53,   19,  169,  170,  171,
 /*   300 */    23,   96,  191,  192,   27,   28,   29,   30,   31,   32,
 /*   310 */    33,   34,   35,   36,   37,   38,   39,   40,   41,  153,
 /*   320 */    43,   44,   45,   46,   47,   48,   49,   50,   51,   52,
 /*   330 */    53,   19,    0,    1,    2,   23,   96,  249,  250,   27,
 /*   340 */    28,   29,   30,   31,   32,   33,   34,   35,   36,   37,
 /*   350 */    38,   39,   40,   41,  188,   43,   44,   45,   46,   47,
 /*   360 */    48,   49,   50,   51,   52,   53,   19,  191,  192,  223,
 /*   370 */   224,  225,  153,  153,   27,   28,   29,   30,   31,   32,
 /*   380 */    33,   34,   35,   36,   37,   38,   39,   40,   41,  243,
 /*   390 */    43,   44,   45,   46,   47,   48,   49,   50,   51,   52,
 /*   400 */    53,   19,  169,  170,  171,  115,   22,  117,  118,   27,
 /*   410 */    28,   29,   30,   31,   32,   33,   34,   35,   36,   37,
 /*   420 */    38,   39,   40,   41,  186,   43,   44,   45,   46,   47,
 /*   430 */    48,   49,   50,   51,   52,   53,   19,  153,  219,   55,
 /*   440 */    56,   24,   55,   56,   27,   28,   29,   30,   31,   32,
 /*   450 */    33,   34,   35,   36,   37,   38,   39,   40,   41,  153,
 /*   460 */    43,   44,   45,   46,   47,   48,   49,   50,   51,   52,
 /*   470 */    53,   84,  252,  153,  180,  181,  170,  171,  194,  172,
 /*   480 */    19,   97,   98,  189,   97,   98,  139,  180,   27,   28,
 /*   490 */    29,   30,   31,   32,   33,   34,   35,   36,   37,   38,
 /*   500 */    39,   40,   41,   26,   43,   44,   45,   46,   47,   48,
 /*   510 */    49,   50,   51,   52,   53,   19,   30,   31,   32,   33,
 /*   520 */   153,  139,  153,   27,   28,   29,   30,   31,   32,   33,
 /*   530 */    34,   35,   36,   37,   38,   39,   40,   41,  232,   43,
 /*   540 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
 /*   550 */    19,  108,  109,  110,  138,   49,  140,   19,  153,   28,
 /*   560 */    29,   30,   31,   32,   33,   34,   35,   36,   37,   38,
 /*   570 */    39,   40,   41,  153,   43,   44,   45,   46,   47,   48,
 /*   580 */    49,   50,   51,   52,   53,   19,  219,  101,    7,    8,
 /*   590 */     9,   53,  153,  173,  174,   29,   30,   31,   32,   33,
 /*   600 */    34,   35,   36,   37,   38,   39,   40,   41,  131,   43,
 /*   610 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
 /*   620 */    19,   20,  116,   22,   23,   22,  153,  121,  153,   85,
 /*   630 */    55,   56,   26,   19,   20,   24,   22,   99,  100,  101,
 /*   640 */   102,  103,  104,  105,   12,  240,  153,  227,   47,   48,
 /*   650 */   112,  153,  108,  109,  110,   54,   55,   56,   55,   56,
 /*   660 */   200,   47,   48,  119,  120,  153,   55,   66,   54,   55,
 /*   670 */    56,  211,   97,   98,   99,  149,  150,  102,  103,  104,
 /*   680 */    66,  155,   65,  157,   83,  173,  174,  194,  113,   57,
 /*   690 */   164,   92,   22,   92,  219,   78,   95,   83,   97,   98,
 /*   700 */    97,   98,  101,   86,   98,   73,   92,   75,   97,   95,
 /*   710 */    23,   97,   98,   26,  153,  101,  153,   85,  119,  120,
 /*   720 */   153,  209,  153,  197,  153,   55,   19,   20,  111,   22,
 /*   730 */   124,  164,   11,  164,  133,  134,  135,  136,  137,  153,
 /*   740 */   173,  174,  173,  174,  173,  174,   22,  133,  134,  135,
 /*   750 */   136,  137,  182,   16,   47,   48,   49,  212,  187,  173,
 /*   760 */   174,   54,   55,   56,  197,  153,  197,   97,  223,  224,
 /*   770 */   225,  153,  246,   66,  164,   19,  209,  210,  209,   55,
 /*   780 */    56,   19,   20,  220,   22,   98,   38,   39,   40,   41,
 /*   790 */    83,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   800 */    52,   53,   95,  153,   97,   98,   85,  197,  101,   47,
 /*   810 */    48,  124,   56,   92,  153,  165,   54,   55,   56,   55,
 /*   820 */    56,   97,   98,  213,  214,   88,   49,   90,   66,  108,
 /*   830 */   109,  110,   55,   56,  153,  164,   19,   20,  220,   22,
 /*   840 */   133,  134,  135,  136,  137,   83,   43,   44,   45,   46,
 /*   850 */    47,   48,   49,   50,   51,   52,   53,   95,  153,   97,
 /*   860 */    98,   97,   98,  101,   47,   48,   55,   56,  197,  153,
 /*   870 */   153,   54,   55,   56,   97,   98,  153,  182,  141,    5,
 /*   880 */   153,  153,  176,   66,   10,   11,   12,   13,   14,  173,
 /*   890 */   174,   17,  221,  195,  196,  133,  134,  135,  136,  137,
 /*   900 */    83,  173,  174,  153,  196,  234,  208,   22,   97,   98,
 /*   910 */   239,  194,   95,   61,   97,   98,  208,  194,  101,   47,
 /*   920 */    48,  194,   70,  173,  174,  209,  210,   55,   56,  223,
 /*   930 */   224,  225,   47,   48,   60,  124,   62,  209,  210,   23,
 /*   940 */    55,   56,   26,   99,  153,   71,  102,  103,  104,  153,
 /*   950 */   133,  134,  135,  136,  137,  153,   82,  113,  153,  209,
 /*   960 */   210,  176,   21,   89,  173,  174,   92,   93,  153,   97,
 /*   970 */    98,  244,  100,   47,   48,  173,  174,  153,  187,  107,
 /*   980 */    95,  109,   97,   55,   56,  153,  101,  153,  114,   38,
 /*   990 */    39,   40,   41,   42,   43,   44,   45,   46,   47,   48,
 /*  1000 */    49,   50,   51,   52,   53,  133,   85,  135,  223,  224,
 /*  1010 */   225,  209,   19,   92,  153,   12,  142,  153,  133,  134,
 /*  1020 */   135,  153,  153,   30,  164,   97,   98,  182,  194,  108,
 /*  1030 */   109,  110,   91,  153,  173,  174,  240,  173,  174,  170,
 /*  1040 */   171,  195,  196,  219,  103,  165,   22,   23,  187,  153,
 /*  1050 */   153,  187,  124,  153,  208,  153,  153,  197,  161,  133,
 /*  1060 */    57,  135,  165,  153,  184,  153,  186,  221,  153,  173,
 /*  1070 */   174,  153,  153,  173,  174,  153,   73,  153,   75,  147,
 /*  1080 */   148,  221,  141,  173,  174,  133,  153,  135,  173,  174,
 /*  1090 */   153,   26,  173,  174,  164,  153,  194,  173,  174,  239,
 /*  1100 */   107,  232,  153,  167,  168,  112,  173,  174,  153,  153,
 /*  1110 */   173,  174,  194,  153,  153,  173,  174,  153,   55,  153,
 /*  1120 */    96,  153,  173,  174,  153,  153,  153,  197,  173,  174,
 /*  1130 */   153,  116,  153,  173,  174,  153,  121,  173,  174,  173,
 /*  1140 */   174,  173,  174,  153,  173,  174,  173,  174,  153,  153,
 /*  1150 */   173,  174,  173,  174,  153,  173,  174,  153,  153,  153,
 /*  1160 */    97,  153,  240,  173,  174,  153,  194,  153,  173,  174,
 /*  1170 */   153,  106,  153,  164,  173,  174,  153,  173,  174,  173,
 /*  1180 */   174,  173,  174,  153,   19,  173,  174,  173,  174,  153,
 /*  1190 */   173,  174,  173,  174,  153,   16,  173,  174,  153,   22,
 /*  1200 */    23,   30,  153,  173,  174,  153,  197,  153,   92,  173,
 /*  1210 */   174,  153,   26,  153,  173,  174,  100,  153,  173,  174,
 /*  1220 */   153,   56,  173,  174,  153,  173,  174,  173,  174,   55,
 /*  1230 */   153,  173,  174,  173,  174,  153,  153,  173,  174,   59,
 /*  1240 */   173,  174,  153,   63,  173,  174,  108,  109,  110,   22,
 /*  1250 */   173,  174,  167,  168,  164,  173,  174,   77,  164,  164,
 /*  1260 */   164,  164,  173,  174,   37,  247,  248,   88,   22,   90,
 /*  1270 */    24,   97,   55,   96,  213,  214,   23,   23,  107,   26,
 /*  1280 */    26,   23,   23,   23,   26,   26,   26,  197,  100,  101,
 /*  1290 */    55,  197,  197,  197,  197,    7,    8,   23,   23,  101,
 /*  1300 */    26,   26,   23,   23,   23,   26,   26,   26,  153,  123,
 /*  1310 */   112,  133,   23,  135,   97,   26,  153,  215,  212,  153,
 /*  1320 */   153,  153,  153,  153,  192,  153,  153,  236,  153,  153,
 /*  1330 */   212,  212,   97,  235,  153,  153,  153,  153,  153,  212,
 /*  1340 */   198,  151,  199,  202,  216,  216,  241,  202,  241,  185,
 /*  1350 */   216,  229,  201,  199,   67,  156,  245,  160,  122,   69,
 /*  1360 */   160,  160,  242,  242,  177,   22,  222,  176,  176,  176,
 /*  1370 */   181,   27,  131,   18,  160,   18,  160,  159,  222,  159,
 /*  1380 */   138,   74,  160,  237,  190,  238,  160,  159,   22,  193,
 /*  1390 */   178,  160,  159,  178,  160,  193,  193,  202,  193,  190,
 /*  1400 */   190,  107,  159,   76,  175,  175,  202,  175,  202,  106,
 /*  1410 */   125,  107,  160,   22,  175,  160,  178,  177,   53,  175,
 /*  1420 */   183,  138,  175,  175,  126,  218,  218,  130,  183,  218,
 /*  1430 */   217,  129,   25,  128,  178,  178,  217,  127,  217,  163,
 /*  1440 */    26,  218,  217,   13,  162,  154,  154,    6,  152,  179,
 /*  1450 */   226,  152,  152,  152,    4,    3,  179,   22,  143,  166,
 /*  1460 */    15,  166,   94,   16,  132,   23,  202,  205,  207,  206,
 /*  1470 */   204,  203,  166,  120,   23,  111,   20,  228,   16,  123,
 /*  1480 */     1,  231,  231,  125,  123,   37,  132,  111,   56,   64,
 /*  1490 */    37,   37,   37,  122,    1,    5,   22,  107,   80,   26,
 /*  1500 */   141,   80,   87,   72,  107,   24,   20,   19,  112,  105,
 /*  1510 */    23,   22,   79,   22,   22,   79,  251,  248,  251,   22,
 /*  1520 */    22,   58,   79,   23,   23,   68,   23,  116,   26,   22,
 /*  1530 */    26,   23,   22,   56,  122,   23,   23,   22,   24,   64,
 /*  1540 */   124,   23,   26,   26,   64,   23,   64,   23,   23,   26,
 /*  1550 */    23,   22,   11,   23,   22,    1,   23,   26,   22,   24,
 /*  1560 */    23,   22,  122,   23,   15,   23,   22,  253,  122,   23,
 /*  1570 */   253,  122,  253,  253,  253,  253,  253,  253,  253,  253,
 /*  1580 */   122,
};
#define YY_SHIFT_USE_DFLT (1581)
#define YY_SHIFT_COUNT    (458)
#define YY_SHIFT_MIN      (-94)
#define YY_SHIFT_MAX      (1554)
static const short yy_shift_ofst[] = {
 /*     0 */   124,  601,  874,  614,  762,  762,  762,  762,  921,  -19,
 /*    10 */    16,   16,  100,  762,  762,  762,  762,  762,  762,  762,
 /*    20 */   872,  872,  575,  544,  721,  599,   61,  137,  172,  207,
 /*    30 */   242,  277,  312,  347,  382,  417,  461,  461,  461,  461,
 /*    40 */   461,  461,  461,  461,  461,  461,  461,  461,  461,  461,
 /*    50 */   461,  461,  461,  461,  496,  461,  531,  566,  566,  707,
 /*    60 */   762,  762,  762,  762,  762,  762,  762,  762,  762,  762,
 /*    70 */   762,  762,  762,  762,  762,  762,  762,  762,  762,  762,
 /*    80 */   762,  762,  762,  762,  762,  762,  762,  762,  762,  762,
 /*    90 */   762,  762,  817,  762,  762,  762,  762,  762,  762,  762,
 /*   100 */   762,  762,  762,  762,  762,  762,  951,  748,  748,  748,
 /*   110 */   748,  748,  803,   23,   32,  764,  941,  993,  926,  926,
 /*   120 */   764,  110,  148,  -51, 1581, 1581, 1581,  538,  538,  538,
 /*   130 */    99,   99,  632,  632,  387,  205,  240,  764,  764,  764,
 /*   140 */   764,  764,  764,  764,  764,  764,  764,  764,  764,  764,
 /*   150 */   764,  764,  764,  764,  764,  764,  764,  332, 1116, 1063,
 /*   160 */  1063,  148,  -54,  -54,  -54,  -54,  -54,  -54, 1581, 1581,
 /*   170 */  1581,  885,  -94,  -94,  384,  844,  777,  603,  811,  928,
 /*   180 */   724,  764,  764,  764,  764,  764,  764,  764,  764,  764,
 /*   190 */   764,  764,  764,  764,  764,  764,  764,  617,  617,  617,
 /*   200 */   764,  764,  687,  764,  764,  764,  -18,  764,  764, 1003,
 /*   210 */   764,  764,  764,  764,  764,  764,  764,  764,  764,  764,
 /*   220 */   443, 1180,  611,  611,  611,  606,  290,  506,  581,  477,
 /*   230 */   756,  756, 1165,  477, 1165, 1065,  916, 1171,  852,  756,
 /*   240 */   416,  852,  852, 1186, 1015, 1246, 1287, 1236, 1236, 1290,
 /*   250 */  1290, 1236, 1343, 1344, 1241, 1355, 1355, 1355, 1355, 1236,
 /*   260 */  1357, 1241, 1343, 1344, 1344, 1241, 1236, 1357, 1242, 1307,
 /*   270 */  1236, 1236, 1357, 1366, 1236, 1357, 1236, 1357, 1366, 1294,
 /*   280 */  1294, 1294, 1327, 1366, 1294, 1303, 1294, 1327, 1294, 1294,
 /*   290 */  1285, 1304, 1285, 1304, 1285, 1304, 1285, 1304, 1236, 1391,
 /*   300 */  1236, 1283, 1366, 1365, 1365, 1366, 1297, 1298, 1302, 1305,
 /*   310 */  1310, 1241, 1407, 1414, 1430, 1430, 1441, 1441, 1441, 1441,
 /*   320 */  1581, 1581, 1581, 1581, 1581, 1581, 1581, 1581,  486,  737,
 /*   330 */  1024, 1177, 1179, 1138,  670, 1253, 1227, 1254, 1258, 1259,
 /*   340 */  1260, 1274, 1275, 1174, 1188, 1288, 1198, 1279, 1280, 1217,
 /*   350 */  1281,  952, 1178, 1289, 1235,   36, 1450, 1452, 1435, 1315,
 /*   360 */  1445, 1368, 1447, 1442, 1451, 1353, 1332, 1364, 1356, 1456,
 /*   370 */  1358, 1462, 1479, 1361, 1354, 1448, 1453, 1454, 1455, 1376,
 /*   380 */  1432, 1425, 1371, 1493, 1490, 1474, 1390, 1359, 1418, 1473,
 /*   390 */  1421, 1415, 1431, 1397, 1481, 1486, 1488, 1396, 1404, 1489,
 /*   400 */  1433, 1491, 1492, 1487, 1497, 1436, 1463, 1498, 1443, 1457,
 /*   410 */  1500, 1501, 1503, 1502, 1411, 1507, 1508, 1510, 1504, 1412,
 /*   420 */  1512, 1513, 1477, 1475, 1515, 1416, 1516, 1480, 1517, 1482,
 /*   430 */  1518, 1516, 1522, 1524, 1525, 1523, 1527, 1529, 1541, 1530,
 /*   440 */  1532, 1514, 1531, 1533, 1536, 1535, 1531, 1537, 1539, 1540,
 /*   450 */  1542, 1544, 1440, 1446, 1449, 1458, 1546, 1549, 1554,
};
#define YY_REDUCE_USE_DFLT (-132)
#define YY_REDUCE_COUNT (327)
#define YY_REDUCE_MIN   (-131)
#define YY_REDUCE_MAX   (1306)
static const short yy_reduce_ofst[] = {
 /*     0 */   -30,  567,  526,  569,  -50,  716,  728,  750,  671,  545,
 /*    10 */   706,  785,  146,  571,  791,  861,  512,  802,  864,  420,
 /*    20 */   306,  869,  880,  846,  860,  610,  -31,  -31,  -31,  -31,
 /*    30 */   -31,  -31,  -31,  -31,  -31,  -31,  -31,  -31,  -31,  -31,
 /*    40 */   -31,  -31,  -31,  -31,  -31,  -31,  -31,  -31,  -31,  -31,
 /*    50 */   -31,  -31,  -31,  -31,  -31,  -31,  -31,  -31,  -31,  586,
 /*    60 */   896,  900,  910,  915,  919,  924,  933,  937,  942,  949,
 /*    70 */   955,  960,  964,  966,  968,  971,  973,  977,  979,  982,
 /*    80 */   990,  995, 1001, 1004, 1006, 1008, 1012, 1014, 1017, 1019,
 /*    90 */  1023, 1030, 1036, 1041, 1045, 1049, 1052, 1054, 1058, 1060,
 /*   100 */  1064, 1067, 1071, 1077, 1082, 1089,  -31,  -31,  -31,  -31,
 /*   110 */   -31,  -31,  -31,  -31,  -31,  727,   -1,  294,  128,  233,
 /*   120 */   897,  -31,  698,  -31,  -31,  -31,  -31,  307,  307,  307,
 /*   130 */   563,  618,  111,  176, -130,   88,   88,  650,  284,  493,
 /*   140 */   717,  723,  834,  902,  918,  405,   45,  796,  219,  367,
 /*   150 */   922,  475,  972,   96,  824,  220,  166,  932,  930,  936,
 /*   160 */  1085,  708, 1009, 1090, 1094, 1095, 1096, 1097, 1018, 1061,
 /*   170 */   460, -131,  -53,   41,   80,  238,  320,  369,  439,  473,
 /*   180 */   498,  561,  612,  661,  681,  705,  805,  815,  832,  868,
 /*   190 */   903,  912,  956,  961,  996, 1005, 1083,  570,  695,  845,
 /*   200 */  1155, 1163, 1102, 1166, 1167, 1168, 1106, 1169, 1170, 1132,
 /*   210 */  1172, 1173, 1175,  320, 1176, 1181, 1182, 1183, 1184, 1185,
 /*   220 */  1091, 1098, 1118, 1119, 1127, 1102, 1142, 1143, 1190, 1141,
 /*   230 */  1128, 1129, 1105, 1145, 1107, 1187, 1164, 1189, 1191, 1134,
 /*   240 */  1122, 1192, 1193, 1151, 1154, 1199, 1111, 1197, 1200, 1120,
 /*   250 */  1121, 1201, 1144, 1194, 1195, 1196, 1202, 1203, 1205, 1214,
 /*   260 */  1218, 1204, 1156, 1209, 1210, 1206, 1216, 1220, 1147, 1146,
 /*   270 */  1222, 1226, 1228, 1212, 1231, 1233, 1234, 1243, 1215, 1229,
 /*   280 */  1230, 1232, 1237, 1238, 1239, 1240, 1244, 1245, 1247, 1248,
 /*   290 */  1207, 1213, 1208, 1219, 1211, 1221, 1223, 1225, 1252, 1224,
 /*   300 */  1255, 1249, 1256, 1250, 1251, 1257, 1261, 1263, 1262, 1266,
 /*   310 */  1268, 1264, 1276, 1282, 1291, 1292, 1296, 1299, 1300, 1301,
 /*   320 */  1265, 1267, 1269, 1293, 1295, 1270, 1277, 1306,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */  1288, 1278, 1278, 1278, 1210, 1210, 1210, 1210, 1278, 1101,
 /*    10 */  1130, 1130, 1262, 1339, 1339, 1339, 1339, 1339, 1339, 1209,
 /*    20 */  1339, 1339, 1339, 1339, 1278, 1105, 1138, 1339, 1339, 1339,
 /*    30 */  1339, 1211, 1212, 1339, 1339, 1339, 1261, 1263, 1148, 1147,
 /*    40 */  1146, 1145, 1244, 1117, 1143, 1134, 1136, 1140, 1211, 1205,
 /*    50 */  1206, 1204, 1208, 1212, 1339, 1139, 1174, 1189, 1173, 1339,
 /*    60 */  1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*    70 */  1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*    80 */  1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*    90 */  1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*   100 */  1339, 1339, 1339, 1339, 1339, 1339, 1183, 1188, 1195, 1187,
 /*   110 */  1184, 1176, 1175, 1177, 1178, 1339, 1024, 1072, 1339, 1339,
 /*   120 */  1339, 1179, 1339, 1180, 1192, 1191, 1190, 1269, 1296, 1295,
 /*   130 */  1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*   140 */  1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*   150 */  1339, 1339, 1339, 1339, 1339, 1339, 1339, 1288, 1278, 1030,
 /*   160 */  1030, 1339, 1278, 1278, 1278, 1278, 1278, 1278, 1274, 1105,
 /*   170 */  1096, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*   180 */  1339, 1339, 1266, 1264, 1339, 1225, 1339, 1339, 1339, 1339,
 /*   190 */  1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*   200 */  1339, 1339, 1339, 1339, 1339, 1339, 1101, 1339, 1339, 1339,
 /*   210 */  1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1290,
 /*   220 */  1339, 1239, 1101, 1101, 1101, 1103, 1085, 1095, 1009, 1142,
 /*   230 */  1119, 1119, 1328, 1142, 1328, 1047, 1310, 1044, 1130, 1119,
 /*   240 */  1207, 1130, 1130, 1102, 1095, 1339, 1331, 1110, 1110, 1330,
 /*   250 */  1330, 1110, 1153, 1075, 1142, 1081, 1081, 1081, 1081, 1110,
 /*   260 */  1021, 1142, 1153, 1075, 1075, 1142, 1110, 1021, 1243, 1325,
 /*   270 */  1110, 1110, 1021, 1218, 1110, 1021, 1110, 1021, 1218, 1073,
 /*   280 */  1073, 1073, 1062, 1218, 1073, 1047, 1073, 1062, 1073, 1073,
 /*   290 */  1123, 1118, 1123, 1118, 1123, 1118, 1123, 1118, 1110, 1213,
 /*   300 */  1110, 1339, 1218, 1222, 1222, 1218, 1137, 1124, 1135, 1133,
 /*   310 */  1131, 1142, 1027, 1065, 1293, 1293, 1289, 1289, 1289, 1289,
 /*   320 */  1336, 1336, 1274, 1305, 1305, 1049, 1049, 1305, 1339, 1339,
 /*   330 */  1339, 1339, 1339, 1339, 1300, 1339, 1227, 1339, 1339, 1339,
 /*   340 */  1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*   350 */  1339, 1339, 1339, 1339, 1339, 1159, 1339, 1005, 1271, 1339,
 /*   360 */  1339, 1270, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*   370 */  1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*   380 */  1339, 1339, 1327, 1339, 1339, 1339, 1339, 1339, 1339, 1242,
 /*   390 */  1241, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*   400 */  1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*   410 */  1339, 1339, 1339, 1339, 1087, 1339, 1339, 1339, 1314, 1339,
 /*   420 */  1339, 1339, 1339, 1339, 1339, 1339, 1132, 1339, 1125, 1339,
 /*   430 */  1339, 1318, 1339, 1339, 1339, 1339, 1339, 1339, 1339, 1339,
 /*   440 */  1339, 1339, 1280, 1339, 1339, 1339, 1279, 1339, 1339, 1339,
 /*   450 */  1339, 1339, 1161, 1339, 1160, 1164, 1339, 1015, 1339,
};
/********** End of lemon-generated parsing tables *****************************/

/* The next table maps tokens (terminal symbols) into fallback tokens.  
** If a construct like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
**
** This feature can be used, for example, to cause some keywords in a language
** to revert to identifiers if they keyword does not apply in the context where
** it appears.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
    0,  /*          $ => nothing */
    0,  /*       SEMI => nothing */
   55,  /*    EXPLAIN => ID */
   55,  /*      QUERY => ID */
   55,  /*       PLAN => ID */
   55,  /*      BEGIN => ID */
    0,  /* TRANSACTION => nothing */
   55,  /*   DEFERRED => ID */
   55,  /*  IMMEDIATE => ID */
   55,  /*  EXCLUSIVE => ID */
    0,  /*     COMMIT => nothing */
   55,  /*        END => ID */
   55,  /*   ROLLBACK => ID */
   55,  /*  SAVEPOINT => ID */
   55,  /*    RELEASE => ID */
    0,  /*         TO => nothing */
    0,  /*      TABLE => nothing */
    0,  /*     CREATE => nothing */
   55,  /*         IF => ID */
    0,  /*        NOT => nothing */
    0,  /*     EXISTS => nothing */
   55,  /*       TEMP => ID */
    0,  /*         LP => nothing */
    0,  /*         RP => nothing */
    0,  /*         AS => nothing */
   55,  /*    WITHOUT => ID */
    0,  /*      COMMA => nothing */
    0,  /*         OR => nothing */
    0,  /*        AND => nothing */
    0,  /*         IS => nothing */
   55,  /*      MATCH => ID */
   55,  /*    LIKE_KW => ID */
    0,  /*    BETWEEN => nothing */
    0,  /*         IN => nothing */
    0,  /*     ISNULL => nothing */
    0,  /*    NOTNULL => nothing */
    0,  /*         NE => nothing */
    0,  /*         EQ => nothing */
    0,  /*         GT => nothing */
    0,  /*         LE => nothing */
    0,  /*         LT => nothing */
    0,  /*         GE => nothing */
    0,  /*     ESCAPE => nothing */
    0,  /*     BITAND => nothing */
    0,  /*      BITOR => nothing */
    0,  /*     LSHIFT => nothing */
    0,  /*     RSHIFT => nothing */
    0,  /*       PLUS => nothing */
    0,  /*      MINUS => nothing */
    0,  /*       STAR => nothing */
    0,  /*      SLASH => nothing */
    0,  /*        REM => nothing */
    0,  /*     CONCAT => nothing */
    0,  /*    COLLATE => nothing */
    0,  /*     BITNOT => nothing */
    0,  /*         ID => nothing */
    0,  /*    INDEXED => nothing */
   55,  /*      ABORT => ID */
   55,  /*     ACTION => ID */
   55,  /*      AFTER => ID */
   55,  /*    ANALYZE => ID */
   55,  /*        ASC => ID */
   55,  /*     ATTACH => ID */
   55,  /*     BEFORE => ID */
   55,  /*         BY => ID */
   55,  /*    CASCADE => ID */
   55,  /*       CAST => ID */
   55,  /*   COLUMNKW => ID */
   55,  /*   CONFLICT => ID */
   55,  /*   DATABASE => ID */
   55,  /*       DESC => ID */
   55,  /*     DETACH => ID */
   55,  /*       EACH => ID */
   55,  /*       FAIL => ID */
   55,  /*        FOR => ID */
   55,  /*     IGNORE => ID */
   55,  /*  INITIALLY => ID */
   55,  /*    INSTEAD => ID */
   55,  /*         NO => ID */
   55,  /*        KEY => ID */
   55,  /*         OF => ID */
   55,  /*     OFFSET => ID */
   55,  /*     PRAGMA => ID */
   55,  /*      RAISE => ID */
   55,  /*  RECURSIVE => ID */
   55,  /*    REPLACE => ID */
   55,  /*   RESTRICT => ID */
   55,  /*        ROW => ID */
   55,  /*    TRIGGER => ID */
   55,  /*     VACUUM => ID */
   55,  /*       VIEW => ID */
   55,  /*    VIRTUAL => ID */
   55,  /*       WITH => ID */
   55,  /*    REINDEX => ID */
   55,  /*     RENAME => ID */
   55,  /*   CTIME_KW => ID */
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
**
** After the "shift" half of a SHIFTREDUCE action, the stateno field
** actually contains the reduce action for the second half of the
** SHIFTREDUCE.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  yyStackEntry *yytos;          /* Pointer to top element of the stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyhwm;                    /* High-water mark of the stack */
#endif
#ifndef YYNOERRORRECOVERY
  int yyerrcnt;                 /* Shifts left before out of the error */
#endif
  sqlite3ParserARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
  yyStackEntry yystk0;          /* First stack entry */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void sqlite3ParserTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "SEMI",          "EXPLAIN",       "QUERY",       
  "PLAN",          "BEGIN",         "TRANSACTION",   "DEFERRED",    
  "IMMEDIATE",     "EXCLUSIVE",     "COMMIT",        "END",         
  "ROLLBACK",      "SAVEPOINT",     "RELEASE",       "TO",          
  "TABLE",         "CREATE",        "IF",            "NOT",         
  "EXISTS",        "TEMP",          "LP",            "RP",          
  "AS",            "WITHOUT",       "COMMA",         "OR",          
  "AND",           "IS",            "MATCH",         "LIKE_KW",     
  "BETWEEN",       "IN",            "ISNULL",        "NOTNULL",     
  "NE",            "EQ",            "GT",            "LE",          
  "LT",            "GE",            "ESCAPE",        "BITAND",      
  "BITOR",         "LSHIFT",        "RSHIFT",        "PLUS",        
  "MINUS",         "STAR",          "SLASH",         "REM",         
  "CONCAT",        "COLLATE",       "BITNOT",        "ID",          
  "INDEXED",       "ABORT",         "ACTION",        "AFTER",       
  "ANALYZE",       "ASC",           "ATTACH",        "BEFORE",      
  "BY",            "CASCADE",       "CAST",          "COLUMNKW",    
  "CONFLICT",      "DATABASE",      "DESC",          "DETACH",      
  "EACH",          "FAIL",          "FOR",           "IGNORE",      
  "INITIALLY",     "INSTEAD",       "NO",            "KEY",         
  "OF",            "OFFSET",        "PRAGMA",        "RAISE",       
  "RECURSIVE",     "REPLACE",       "RESTRICT",      "ROW",         
  "TRIGGER",       "VACUUM",        "VIEW",          "VIRTUAL",     
  "WITH",          "REINDEX",       "RENAME",        "CTIME_KW",    
  "ANY",           "STRING",        "JOIN_KW",       "CONSTRAINT",  
  "DEFAULT",       "NULL",          "PRIMARY",       "UNIQUE",      
  "CHECK",         "REFERENCES",    "AUTOINCR",      "ON",          
  "INSERT",        "DELETE",        "UPDATE",        "SET",         
  "DEFERRABLE",    "FOREIGN",       "DROP",          "UNION",       
  "ALL",           "EXCEPT",        "INTERSECT",     "SELECT",      
  "VALUES",        "DISTINCT",      "DOT",           "FROM",        
  "JOIN",          "USING",         "ORDER",         "GROUP",       
  "HAVING",        "FORECAST",      "LIMIT",         "WHERE",       
  "INTO",          "FLOAT",         "BLOB",          "INTEGER",     
  "VARIABLE",      "CASE",          "WHEN",          "THEN",        
  "ELSE",          "INDEX",         "ALTER",         "ADD",         
  "error",         "input",         "cmdlist",       "ecmd",        
  "explain",       "cmdx",          "cmd",           "transtype",   
  "trans_opt",     "nm",            "savepoint_opt",  "create_table",
  "create_table_args",  "createkw",      "temp",          "ifnotexists", 
  "dbnm",          "columnlist",    "conslist_opt",  "table_options",
  "select",        "columnname",    "carglist",      "typetoken",   
  "typename",      "signed",        "plus_num",      "minus_num",   
  "ccons",         "term",          "expr",          "onconf",      
  "sortorder",     "autoinc",       "eidlist_opt",   "refargs",     
  "defer_subclause",  "refarg",        "refact",        "init_deferred_pred_opt",
  "conslist",      "tconscomma",    "tcons",         "sortlist",    
  "eidlist",       "defer_subclause_opt",  "orconf",        "resolvetype", 
  "raisetype",     "ifexists",      "fullname",      "selectnowith",
  "oneselect",     "with",          "multiselect_op",  "distinct",    
  "selcollist",    "from",          "where_opt",     "groupby_opt", 
  "having_opt",    "forecast_opt",  "orderby_opt",   "limit_opt",   
  "values",        "nexprlist",     "exprlist",      "sclp",        
  "as",            "seltablist",    "stl_prefix",    "joinop",      
  "indexed_opt",   "on_opt",        "using_opt",     "idlist",      
  "setlist",       "insert_cmd",    "idlist_opt",    "likeop",      
  "between_op",    "in_op",         "paren_exprlist",  "case_operand",
  "case_exprlist",  "case_else",     "uniqueflag",    "collate",     
  "nmnum",         "trigger_decl",  "trigger_cmd_list",  "trigger_time",
  "trigger_event",  "foreach_clause",  "when_clause",   "trigger_cmd", 
  "trnm",          "tridxby",       "database_kw_opt",  "key_opt",     
  "add_column_fullname",  "kwcolumn_opt",  "create_vtab",   "vtabarglist", 
  "vtabarg",       "vtabargtoken",  "lp",            "anylist",     
  "wqlist",      
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "explain ::= EXPLAIN",
 /*   1 */ "explain ::= EXPLAIN QUERY PLAN",
 /*   2 */ "cmdx ::= cmd",
 /*   3 */ "cmd ::= BEGIN transtype trans_opt",
 /*   4 */ "transtype ::=",
 /*   5 */ "transtype ::= DEFERRED",
 /*   6 */ "transtype ::= IMMEDIATE",
 /*   7 */ "transtype ::= EXCLUSIVE",
 /*   8 */ "cmd ::= COMMIT trans_opt",
 /*   9 */ "cmd ::= END trans_opt",
 /*  10 */ "cmd ::= ROLLBACK trans_opt",
 /*  11 */ "cmd ::= SAVEPOINT nm",
 /*  12 */ "cmd ::= RELEASE savepoint_opt nm",
 /*  13 */ "cmd ::= ROLLBACK trans_opt TO savepoint_opt nm",
 /*  14 */ "create_table ::= createkw temp TABLE ifnotexists nm dbnm",
 /*  15 */ "createkw ::= CREATE",
 /*  16 */ "ifnotexists ::=",
 /*  17 */ "ifnotexists ::= IF NOT EXISTS",
 /*  18 */ "temp ::= TEMP",
 /*  19 */ "temp ::=",
 /*  20 */ "create_table_args ::= LP columnlist conslist_opt RP table_options",
 /*  21 */ "create_table_args ::= AS select",
 /*  22 */ "table_options ::=",
 /*  23 */ "table_options ::= WITHOUT nm",
 /*  24 */ "columnname ::= nm typetoken",
 /*  25 */ "typetoken ::=",
 /*  26 */ "typetoken ::= typename LP signed RP",
 /*  27 */ "typetoken ::= typename LP signed COMMA signed RP",
 /*  28 */ "typename ::= typename ID|STRING",
 /*  29 */ "ccons ::= CONSTRAINT nm",
 /*  30 */ "ccons ::= DEFAULT term",
 /*  31 */ "ccons ::= DEFAULT LP expr RP",
 /*  32 */ "ccons ::= DEFAULT PLUS term",
 /*  33 */ "ccons ::= DEFAULT MINUS term",
 /*  34 */ "ccons ::= DEFAULT ID|INDEXED",
 /*  35 */ "ccons ::= NOT NULL onconf",
 /*  36 */ "ccons ::= PRIMARY KEY sortorder onconf autoinc",
 /*  37 */ "ccons ::= UNIQUE onconf",
 /*  38 */ "ccons ::= CHECK LP expr RP",
 /*  39 */ "ccons ::= REFERENCES nm eidlist_opt refargs",
 /*  40 */ "ccons ::= defer_subclause",
 /*  41 */ "ccons ::= COLLATE ID|STRING",
 /*  42 */ "autoinc ::=",
 /*  43 */ "autoinc ::= AUTOINCR",
 /*  44 */ "refargs ::=",
 /*  45 */ "refargs ::= refargs refarg",
 /*  46 */ "refarg ::= MATCH nm",
 /*  47 */ "refarg ::= ON INSERT refact",
 /*  48 */ "refarg ::= ON DELETE refact",
 /*  49 */ "refarg ::= ON UPDATE refact",
 /*  50 */ "refact ::= SET NULL",
 /*  51 */ "refact ::= SET DEFAULT",
 /*  52 */ "refact ::= CASCADE",
 /*  53 */ "refact ::= RESTRICT",
 /*  54 */ "refact ::= NO ACTION",
 /*  55 */ "defer_subclause ::= NOT DEFERRABLE init_deferred_pred_opt",
 /*  56 */ "defer_subclause ::= DEFERRABLE init_deferred_pred_opt",
 /*  57 */ "init_deferred_pred_opt ::=",
 /*  58 */ "init_deferred_pred_opt ::= INITIALLY DEFERRED",
 /*  59 */ "init_deferred_pred_opt ::= INITIALLY IMMEDIATE",
 /*  60 */ "conslist_opt ::=",
 /*  61 */ "tconscomma ::= COMMA",
 /*  62 */ "tcons ::= CONSTRAINT nm",
 /*  63 */ "tcons ::= PRIMARY KEY LP sortlist autoinc RP onconf",
 /*  64 */ "tcons ::= UNIQUE LP sortlist RP onconf",
 /*  65 */ "tcons ::= CHECK LP expr RP onconf",
 /*  66 */ "tcons ::= FOREIGN KEY LP eidlist RP REFERENCES nm eidlist_opt refargs defer_subclause_opt",
 /*  67 */ "defer_subclause_opt ::=",
 /*  68 */ "onconf ::=",
 /*  69 */ "onconf ::= ON CONFLICT resolvetype",
 /*  70 */ "orconf ::=",
 /*  71 */ "orconf ::= OR resolvetype",
 /*  72 */ "resolvetype ::= IGNORE",
 /*  73 */ "resolvetype ::= REPLACE",
 /*  74 */ "cmd ::= DROP TABLE ifexists fullname",
 /*  75 */ "ifexists ::= IF EXISTS",
 /*  76 */ "ifexists ::=",
 /*  77 */ "cmd ::= createkw temp VIEW ifnotexists nm dbnm eidlist_opt AS select",
 /*  78 */ "cmd ::= DROP VIEW ifexists fullname",
 /*  79 */ "cmd ::= select",
 /*  80 */ "select ::= with selectnowith",
 /*  81 */ "selectnowith ::= selectnowith multiselect_op oneselect",
 /*  82 */ "multiselect_op ::= UNION",
 /*  83 */ "multiselect_op ::= UNION ALL",
 /*  84 */ "multiselect_op ::= EXCEPT|INTERSECT",
 /*  85 */ "oneselect ::= SELECT distinct selcollist from where_opt groupby_opt having_opt forecast_opt orderby_opt limit_opt",
 /*  86 */ "values ::= VALUES LP nexprlist RP",
 /*  87 */ "values ::= values COMMA LP exprlist RP",
 /*  88 */ "distinct ::= DISTINCT",
 /*  89 */ "distinct ::= ALL",
 /*  90 */ "distinct ::=",
 /*  91 */ "sclp ::=",
 /*  92 */ "selcollist ::= sclp expr as",
 /*  93 */ "selcollist ::= sclp STAR",
 /*  94 */ "selcollist ::= sclp nm DOT STAR",
 /*  95 */ "as ::= AS nm",
 /*  96 */ "as ::=",
 /*  97 */ "from ::=",
 /*  98 */ "from ::= FROM seltablist",
 /*  99 */ "stl_prefix ::= seltablist joinop",
 /* 100 */ "stl_prefix ::=",
 /* 101 */ "seltablist ::= stl_prefix nm dbnm as indexed_opt on_opt using_opt",
 /* 102 */ "seltablist ::= stl_prefix nm dbnm LP exprlist RP as on_opt using_opt",
 /* 103 */ "seltablist ::= stl_prefix LP select RP as on_opt using_opt",
 /* 104 */ "seltablist ::= stl_prefix LP seltablist RP as on_opt using_opt",
 /* 105 */ "dbnm ::=",
 /* 106 */ "dbnm ::= DOT nm",
 /* 107 */ "fullname ::= nm dbnm",
 /* 108 */ "joinop ::= COMMA|JOIN",
 /* 109 */ "joinop ::= JOIN_KW JOIN",
 /* 110 */ "joinop ::= JOIN_KW nm JOIN",
 /* 111 */ "joinop ::= JOIN_KW nm nm JOIN",
 /* 112 */ "on_opt ::= ON expr",
 /* 113 */ "on_opt ::=",
 /* 114 */ "indexed_opt ::=",
 /* 115 */ "indexed_opt ::= INDEXED BY nm",
 /* 116 */ "indexed_opt ::= NOT INDEXED",
 /* 117 */ "using_opt ::= USING LP idlist RP",
 /* 118 */ "using_opt ::=",
 /* 119 */ "orderby_opt ::=",
 /* 120 */ "orderby_opt ::= ORDER BY sortlist",
 /* 121 */ "sortlist ::= sortlist COMMA expr sortorder",
 /* 122 */ "sortlist ::= expr sortorder",
 /* 123 */ "sortorder ::= ASC",
 /* 124 */ "sortorder ::= DESC",
 /* 125 */ "sortorder ::=",
 /* 126 */ "groupby_opt ::=",
 /* 127 */ "groupby_opt ::= GROUP BY nexprlist",
 /* 128 */ "having_opt ::=",
 /* 129 */ "having_opt ::= HAVING expr",
 /* 130 */ "forecast_opt ::=",
 /* 131 */ "forecast_opt ::= FORECAST expr",
 /* 132 */ "limit_opt ::=",
 /* 133 */ "limit_opt ::= LIMIT expr",
 /* 134 */ "limit_opt ::= LIMIT expr OFFSET expr",
 /* 135 */ "limit_opt ::= LIMIT expr COMMA expr",
 /* 136 */ "cmd ::= with DELETE FROM fullname indexed_opt where_opt",
 /* 137 */ "where_opt ::=",
 /* 138 */ "where_opt ::= WHERE expr",
 /* 139 */ "cmd ::= with UPDATE orconf fullname indexed_opt SET setlist where_opt",
 /* 140 */ "setlist ::= setlist COMMA nm EQ expr",
 /* 141 */ "setlist ::= setlist COMMA LP idlist RP EQ expr",
 /* 142 */ "setlist ::= nm EQ expr",
 /* 143 */ "setlist ::= LP idlist RP EQ expr",
 /* 144 */ "cmd ::= with insert_cmd INTO fullname idlist_opt select",
 /* 145 */ "cmd ::= with insert_cmd INTO fullname idlist_opt DEFAULT VALUES",
 /* 146 */ "insert_cmd ::= INSERT orconf",
 /* 147 */ "insert_cmd ::= REPLACE",
 /* 148 */ "idlist_opt ::=",
 /* 149 */ "idlist_opt ::= LP idlist RP",
 /* 150 */ "idlist ::= idlist COMMA nm",
 /* 151 */ "idlist ::= nm",
 /* 152 */ "expr ::= LP expr RP",
 /* 153 */ "term ::= NULL",
 /* 154 */ "expr ::= ID|INDEXED",
 /* 155 */ "expr ::= JOIN_KW",
 /* 156 */ "expr ::= nm DOT nm",
 /* 157 */ "expr ::= nm DOT nm DOT nm",
 /* 158 */ "term ::= FLOAT|BLOB",
 /* 159 */ "term ::= STRING",
 /* 160 */ "term ::= INTEGER",
 /* 161 */ "expr ::= VARIABLE",
 /* 162 */ "expr ::= expr COLLATE ID|STRING",
 /* 163 */ "expr ::= CAST LP expr AS typetoken RP",
 /* 164 */ "expr ::= ID|INDEXED LP distinct exprlist RP",
 /* 165 */ "expr ::= ID|INDEXED LP STAR RP",
 /* 166 */ "term ::= CTIME_KW",
 /* 167 */ "expr ::= LP nexprlist COMMA expr RP",
 /* 168 */ "expr ::= expr AND expr",
 /* 169 */ "expr ::= expr OR expr",
 /* 170 */ "expr ::= expr LT|GT|GE|LE expr",
 /* 171 */ "expr ::= expr EQ|NE expr",
 /* 172 */ "expr ::= expr BITAND|BITOR|LSHIFT|RSHIFT expr",
 /* 173 */ "expr ::= expr PLUS|MINUS expr",
 /* 174 */ "expr ::= expr STAR|SLASH|REM expr",
 /* 175 */ "expr ::= expr CONCAT expr",
 /* 176 */ "likeop ::= LIKE_KW|MATCH",
 /* 177 */ "likeop ::= NOT LIKE_KW|MATCH",
 /* 178 */ "expr ::= expr likeop expr",
 /* 179 */ "expr ::= expr likeop expr ESCAPE expr",
 /* 180 */ "expr ::= expr ISNULL|NOTNULL",
 /* 181 */ "expr ::= expr NOT NULL",
 /* 182 */ "expr ::= expr IS expr",
 /* 183 */ "expr ::= expr IS NOT expr",
 /* 184 */ "expr ::= NOT expr",
 /* 185 */ "expr ::= BITNOT expr",
 /* 186 */ "expr ::= MINUS expr",
 /* 187 */ "expr ::= PLUS expr",
 /* 188 */ "between_op ::= BETWEEN",
 /* 189 */ "between_op ::= NOT BETWEEN",
 /* 190 */ "expr ::= expr between_op expr AND expr",
 /* 191 */ "in_op ::= IN",
 /* 192 */ "in_op ::= NOT IN",
 /* 193 */ "expr ::= expr in_op LP exprlist RP",
 /* 194 */ "expr ::= LP select RP",
 /* 195 */ "expr ::= expr in_op LP select RP",
 /* 196 */ "expr ::= expr in_op nm dbnm paren_exprlist",
 /* 197 */ "expr ::= EXISTS LP select RP",
 /* 198 */ "expr ::= CASE case_operand case_exprlist case_else END",
 /* 199 */ "case_exprlist ::= case_exprlist WHEN expr THEN expr",
 /* 200 */ "case_exprlist ::= WHEN expr THEN expr",
 /* 201 */ "case_else ::= ELSE expr",
 /* 202 */ "case_else ::=",
 /* 203 */ "case_operand ::= expr",
 /* 204 */ "case_operand ::=",
 /* 205 */ "exprlist ::=",
 /* 206 */ "nexprlist ::= nexprlist COMMA expr",
 /* 207 */ "nexprlist ::= expr",
 /* 208 */ "paren_exprlist ::=",
 /* 209 */ "paren_exprlist ::= LP exprlist RP",
 /* 210 */ "cmd ::= createkw uniqueflag INDEX ifnotexists nm dbnm ON nm LP sortlist RP where_opt",
 /* 211 */ "uniqueflag ::= UNIQUE",
 /* 212 */ "uniqueflag ::=",
 /* 213 */ "eidlist_opt ::=",
 /* 214 */ "eidlist_opt ::= LP eidlist RP",
 /* 215 */ "eidlist ::= eidlist COMMA nm collate sortorder",
 /* 216 */ "eidlist ::= nm collate sortorder",
 /* 217 */ "collate ::=",
 /* 218 */ "collate ::= COLLATE ID|STRING",
 /* 219 */ "cmd ::= DROP INDEX ifexists fullname",
 /* 220 */ "cmd ::= VACUUM",
 /* 221 */ "cmd ::= VACUUM nm",
 /* 222 */ "cmd ::= PRAGMA nm dbnm",
 /* 223 */ "cmd ::= PRAGMA nm dbnm EQ nmnum",
 /* 224 */ "cmd ::= PRAGMA nm dbnm LP nmnum RP",
 /* 225 */ "cmd ::= PRAGMA nm dbnm EQ minus_num",
 /* 226 */ "cmd ::= PRAGMA nm dbnm LP minus_num RP",
 /* 227 */ "plus_num ::= PLUS INTEGER|FLOAT",
 /* 228 */ "minus_num ::= MINUS INTEGER|FLOAT",
 /* 229 */ "cmd ::= createkw trigger_decl BEGIN trigger_cmd_list END",
 /* 230 */ "trigger_decl ::= temp TRIGGER ifnotexists nm dbnm trigger_time trigger_event ON fullname foreach_clause when_clause",
 /* 231 */ "trigger_time ::= BEFORE",
 /* 232 */ "trigger_time ::= AFTER",
 /* 233 */ "trigger_time ::= INSTEAD OF",
 /* 234 */ "trigger_time ::=",
 /* 235 */ "trigger_event ::= DELETE|INSERT",
 /* 236 */ "trigger_event ::= UPDATE",
 /* 237 */ "trigger_event ::= UPDATE OF idlist",
 /* 238 */ "when_clause ::=",
 /* 239 */ "when_clause ::= WHEN expr",
 /* 240 */ "trigger_cmd_list ::= trigger_cmd_list trigger_cmd SEMI",
 /* 241 */ "trigger_cmd_list ::= trigger_cmd SEMI",
 /* 242 */ "trnm ::= nm DOT nm",
 /* 243 */ "tridxby ::= INDEXED BY nm",
 /* 244 */ "tridxby ::= NOT INDEXED",
 /* 245 */ "trigger_cmd ::= UPDATE orconf trnm tridxby SET setlist where_opt",
 /* 246 */ "trigger_cmd ::= insert_cmd INTO trnm idlist_opt select",
 /* 247 */ "trigger_cmd ::= DELETE FROM trnm tridxby where_opt",
 /* 248 */ "trigger_cmd ::= select",
 /* 249 */ "expr ::= RAISE LP IGNORE RP",
 /* 250 */ "expr ::= RAISE LP raisetype COMMA nm RP",
 /* 251 */ "raisetype ::= ROLLBACK",
 /* 252 */ "raisetype ::= ABORT",
 /* 253 */ "raisetype ::= FAIL",
 /* 254 */ "cmd ::= DROP TRIGGER ifexists fullname",
 /* 255 */ "cmd ::= ATTACH database_kw_opt expr AS expr key_opt",
 /* 256 */ "cmd ::= DETACH database_kw_opt expr",
 /* 257 */ "key_opt ::=",
 /* 258 */ "key_opt ::= KEY expr",
 /* 259 */ "cmd ::= REINDEX",
 /* 260 */ "cmd ::= REINDEX nm dbnm",
 /* 261 */ "cmd ::= ANALYZE",
 /* 262 */ "cmd ::= ANALYZE nm dbnm",
 /* 263 */ "cmd ::= ALTER TABLE fullname RENAME TO nm",
 /* 264 */ "cmd ::= ALTER TABLE add_column_fullname ADD kwcolumn_opt columnname carglist",
 /* 265 */ "add_column_fullname ::= fullname",
 /* 266 */ "cmd ::= create_vtab",
 /* 267 */ "cmd ::= create_vtab LP vtabarglist RP",
 /* 268 */ "create_vtab ::= createkw VIRTUAL TABLE ifnotexists nm dbnm USING nm",
 /* 269 */ "vtabarg ::=",
 /* 270 */ "vtabargtoken ::= ANY",
 /* 271 */ "vtabargtoken ::= lp anylist RP",
 /* 272 */ "lp ::= LP",
 /* 273 */ "with ::=",
 /* 274 */ "with ::= WITH wqlist",
 /* 275 */ "with ::= WITH RECURSIVE wqlist",
 /* 276 */ "wqlist ::= nm eidlist_opt AS LP select RP",
 /* 277 */ "wqlist ::= wqlist COMMA nm eidlist_opt AS LP select RP",
 /* 278 */ "input ::= cmdlist",
 /* 279 */ "cmdlist ::= cmdlist ecmd",
 /* 280 */ "cmdlist ::= ecmd",
 /* 281 */ "ecmd ::= SEMI",
 /* 282 */ "ecmd ::= explain cmdx SEMI",
 /* 283 */ "explain ::=",
 /* 284 */ "trans_opt ::=",
 /* 285 */ "trans_opt ::= TRANSACTION",
 /* 286 */ "trans_opt ::= TRANSACTION nm",
 /* 287 */ "savepoint_opt ::= SAVEPOINT",
 /* 288 */ "savepoint_opt ::=",
 /* 289 */ "cmd ::= create_table create_table_args",
 /* 290 */ "columnlist ::= columnlist COMMA columnname carglist",
 /* 291 */ "columnlist ::= columnname carglist",
 /* 292 */ "nm ::= ID|INDEXED",
 /* 293 */ "nm ::= STRING",
 /* 294 */ "nm ::= JOIN_KW",
 /* 295 */ "typetoken ::= typename",
 /* 296 */ "typename ::= ID|STRING",
 /* 297 */ "signed ::= plus_num",
 /* 298 */ "signed ::= minus_num",
 /* 299 */ "carglist ::= carglist ccons",
 /* 300 */ "carglist ::=",
 /* 301 */ "ccons ::= NULL onconf",
 /* 302 */ "conslist_opt ::= COMMA conslist",
 /* 303 */ "conslist ::= conslist tconscomma tcons",
 /* 304 */ "conslist ::= tcons",
 /* 305 */ "tconscomma ::=",
 /* 306 */ "defer_subclause_opt ::= defer_subclause",
 /* 307 */ "resolvetype ::= raisetype",
 /* 308 */ "selectnowith ::= oneselect",
 /* 309 */ "oneselect ::= values",
 /* 310 */ "sclp ::= selcollist COMMA",
 /* 311 */ "as ::= ID|STRING",
 /* 312 */ "expr ::= term",
 /* 313 */ "exprlist ::= nexprlist",
 /* 314 */ "nmnum ::= plus_num",
 /* 315 */ "nmnum ::= nm",
 /* 316 */ "nmnum ::= ON",
 /* 317 */ "nmnum ::= DELETE",
 /* 318 */ "nmnum ::= DEFAULT",
 /* 319 */ "plus_num ::= INTEGER|FLOAT",
 /* 320 */ "foreach_clause ::=",
 /* 321 */ "foreach_clause ::= FOR EACH ROW",
 /* 322 */ "trnm ::= nm",
 /* 323 */ "tridxby ::=",
 /* 324 */ "database_kw_opt ::= DATABASE",
 /* 325 */ "database_kw_opt ::=",
 /* 326 */ "kwcolumn_opt ::=",
 /* 327 */ "kwcolumn_opt ::= COLUMNKW",
 /* 328 */ "vtabarglist ::= vtabarg",
 /* 329 */ "vtabarglist ::= vtabarglist COMMA vtabarg",
 /* 330 */ "vtabarg ::= vtabarg vtabargtoken",
 /* 331 */ "anylist ::=",
 /* 332 */ "anylist ::= anylist LP anylist RP",
 /* 333 */ "anylist ::= anylist ANY",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int yyGrowStack(yyParser *p){
  int newSize;
  int idx;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  idx = p->yytos ? (int)(p->yytos - p->yystack) : 0;
  if( p->yystack==&p->yystk0 ){
    pNew = malloc(newSize*sizeof(pNew[0]));
    if( pNew ) pNew[0] = p->yystk0;
  }else{
    pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  }
  if( pNew ){
    p->yystack = pNew;
    p->yytos = &p->yystack[idx];
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows from %d to %d entries.\n",
              yyTracePrompt, p->yystksz, newSize);
    }
#endif
    p->yystksz = newSize;
  }
  return pNew==0; 
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to sqlite3ParserAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef YYMALLOCARGTYPE
# define YYMALLOCARGTYPE size_t
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to sqlite3Parser and sqlite3ParserFree.
*/
void *sqlite3ParserAlloc(void *(*mallocProc)(YYMALLOCARGTYPE)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (YYMALLOCARGTYPE)sizeof(yyParser) );
  if( pParser ){
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyhwm = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yytos = NULL;
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    if( yyGrowStack(pParser) ){
      pParser->yystack = &pParser->yystk0;
      pParser->yystksz = 1;
    }
#endif
#ifndef YYNOERRORRECOVERY
    pParser->yyerrcnt = -1;
#endif
    pParser->yytos = pParser->yystack;
    pParser->yystack[0].stateno = 0;
    pParser->yystack[0].major = 0;
  }
  return pParser;
}

/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "yymajor" is the symbol code, and "yypminor" is
** a pointer to the value to be deleted.  The code used to do the 
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  sqlite3ParserARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are *not* used
    ** inside the C code.
    */
/********* Begin destructor definitions ***************************************/
    case 164: /* select */
    case 195: /* selectnowith */
    case 196: /* oneselect */
    case 208: /* values */
{
#line 396 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
sqlite3SelectDelete(pParse->db, (yypminor->yy3));
#line 1582 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
}
      break;
    case 173: /* term */
    case 174: /* expr */
{
#line 828 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
sqlite3ExprDelete(pParse->db, (yypminor->yy346).pExpr);
#line 1590 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
}
      break;
    case 178: /* eidlist_opt */
    case 187: /* sortlist */
    case 188: /* eidlist */
    case 200: /* selcollist */
    case 203: /* groupby_opt */
    case 206: /* orderby_opt */
    case 209: /* nexprlist */
    case 210: /* exprlist */
    case 211: /* sclp */
    case 220: /* setlist */
    case 226: /* paren_exprlist */
    case 228: /* case_exprlist */
{
#line 1276 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
sqlite3ExprListDelete(pParse->db, (yypminor->yy14));
#line 1608 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
}
      break;
    case 194: /* fullname */
    case 201: /* from */
    case 213: /* seltablist */
    case 214: /* stl_prefix */
{
#line 628 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
sqlite3SrcListDelete(pParse->db, (yypminor->yy65));
#line 1618 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
}
      break;
    case 197: /* with */
    case 252: /* wqlist */
{
#line 1553 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
sqlite3WithDelete(pParse->db, (yypminor->yy59));
#line 1626 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
}
      break;
    case 202: /* where_opt */
    case 204: /* having_opt */
    case 205: /* forecast_opt */
    case 217: /* on_opt */
    case 227: /* case_operand */
    case 229: /* case_else */
    case 238: /* when_clause */
    case 243: /* key_opt */
{
#line 749 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
sqlite3ExprDelete(pParse->db, (yypminor->yy132));
#line 1640 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
}
      break;
    case 218: /* using_opt */
    case 219: /* idlist */
    case 222: /* idlist_opt */
{
#line 662 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
sqlite3IdListDelete(pParse->db, (yypminor->yy408));
#line 1649 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
}
      break;
    case 234: /* trigger_cmd_list */
    case 239: /* trigger_cmd */
{
#line 1390 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
sqlite3DeleteTriggerStep(pParse->db, (yypminor->yy473));
#line 1657 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
}
      break;
    case 236: /* trigger_event */
{
#line 1376 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
sqlite3IdListDelete(pParse->db, (yypminor->yy378).b);
#line 1664 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
}
      break;
/********* End destructor definitions *****************************************/
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
*/
static void yy_pop_parser_stack(yyParser *pParser){
  yyStackEntry *yytos;
  assert( pParser->yytos!=0 );
  assert( pParser->yytos > pParser->yystack );
  yytos = pParser->yytos--;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yy_destructor(pParser, yytos->major, &yytos->minor);
}

/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the YYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void sqlite3ParserFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
#ifndef YYPARSEFREENEVERNULL
  if( pParser==0 ) return;
#endif
  while( pParser->yytos>pParser->yystack ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  if( pParser->yystack!=&pParser->yystk0 ) free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int sqlite3ParserStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyhwm;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static unsigned int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yytos->stateno;
 
  if( stateno>=YY_MIN_REDUCE ) return stateno;
  assert( stateno <= YY_SHIFT_COUNT );
  do{
    i = yy_shift_ofst[stateno];
    assert( iLookAhead!=YYNOCODE );
    i += iLookAhead;
    if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        assert( yyFallback[iFallback]==0 ); /* Fallback loop must terminate */
        iLookAhead = iFallback;
        continue;
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( 
#if YY_SHIFT_MIN+YYWILDCARD<0
          j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
          j<YY_ACTTAB_COUNT &&
#endif
          yy_lookahead[j]==YYWILDCARD && iLookAhead>0
        ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead],
               yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
      return yy_default[stateno];
    }else{
      return yy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser){
   sqlite3ParserARG_FETCH;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
#line 37 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"

  sqlite3ErrorMsg(pParse, "parser stack overflow");
#line 1837 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
/******** End %stack_overflow code ********************************************/
   sqlite3ParserARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void yyTraceShift(yyParser *yypParser, int yyNewState){
  if( yyTraceFILE ){
    if( yyNewState<YYNSTATE ){
      fprintf(yyTraceFILE,"%sShift '%s', go to state %d\n",
         yyTracePrompt,yyTokenName[yypParser->yytos->major],
         yyNewState);
    }else{
      fprintf(yyTraceFILE,"%sShift '%s'\n",
         yyTracePrompt,yyTokenName[yypParser->yytos->major]);
    }
  }
}
#else
# define yyTraceShift(X,Y)
#endif

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  sqlite3ParserTOKENTYPE yyMinor        /* The minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yytos++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
    yypParser->yyhwm++;
    assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack) );
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yytos>=&yypParser->yystack[YYSTACKDEPTH] ){
    yypParser->yytos--;
    yyStackOverflow(yypParser);
    return;
  }
#else
  if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz] ){
    if( yyGrowStack(yypParser) ){
      yypParser->yytos--;
      yyStackOverflow(yypParser);
      return;
    }
  }
#endif
  if( yyNewState > YY_MAX_SHIFT ){
    yyNewState += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
  }
  yytos = yypParser->yytos;
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor.yy0 = yyMinor;
  yyTraceShift(yypParser, yyNewState);
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 148, 1 },
  { 148, 3 },
  { 149, 1 },
  { 150, 3 },
  { 151, 0 },
  { 151, 1 },
  { 151, 1 },
  { 151, 1 },
  { 150, 2 },
  { 150, 2 },
  { 150, 2 },
  { 150, 2 },
  { 150, 3 },
  { 150, 5 },
  { 155, 6 },
  { 157, 1 },
  { 159, 0 },
  { 159, 3 },
  { 158, 1 },
  { 158, 0 },
  { 156, 5 },
  { 156, 2 },
  { 163, 0 },
  { 163, 2 },
  { 165, 2 },
  { 167, 0 },
  { 167, 4 },
  { 167, 6 },
  { 168, 2 },
  { 172, 2 },
  { 172, 2 },
  { 172, 4 },
  { 172, 3 },
  { 172, 3 },
  { 172, 2 },
  { 172, 3 },
  { 172, 5 },
  { 172, 2 },
  { 172, 4 },
  { 172, 4 },
  { 172, 1 },
  { 172, 2 },
  { 177, 0 },
  { 177, 1 },
  { 179, 0 },
  { 179, 2 },
  { 181, 2 },
  { 181, 3 },
  { 181, 3 },
  { 181, 3 },
  { 182, 2 },
  { 182, 2 },
  { 182, 1 },
  { 182, 1 },
  { 182, 2 },
  { 180, 3 },
  { 180, 2 },
  { 183, 0 },
  { 183, 2 },
  { 183, 2 },
  { 162, 0 },
  { 185, 1 },
  { 186, 2 },
  { 186, 7 },
  { 186, 5 },
  { 186, 5 },
  { 186, 10 },
  { 189, 0 },
  { 175, 0 },
  { 175, 3 },
  { 190, 0 },
  { 190, 2 },
  { 191, 1 },
  { 191, 1 },
  { 150, 4 },
  { 193, 2 },
  { 193, 0 },
  { 150, 9 },
  { 150, 4 },
  { 150, 1 },
  { 164, 2 },
  { 195, 3 },
  { 198, 1 },
  { 198, 2 },
  { 198, 1 },
  { 196, 10 },
  { 208, 4 },
  { 208, 5 },
  { 199, 1 },
  { 199, 1 },
  { 199, 0 },
  { 211, 0 },
  { 200, 3 },
  { 200, 2 },
  { 200, 4 },
  { 212, 2 },
  { 212, 0 },
  { 201, 0 },
  { 201, 2 },
  { 214, 2 },
  { 214, 0 },
  { 213, 7 },
  { 213, 9 },
  { 213, 7 },
  { 213, 7 },
  { 160, 0 },
  { 160, 2 },
  { 194, 2 },
  { 215, 1 },
  { 215, 2 },
  { 215, 3 },
  { 215, 4 },
  { 217, 2 },
  { 217, 0 },
  { 216, 0 },
  { 216, 3 },
  { 216, 2 },
  { 218, 4 },
  { 218, 0 },
  { 206, 0 },
  { 206, 3 },
  { 187, 4 },
  { 187, 2 },
  { 176, 1 },
  { 176, 1 },
  { 176, 0 },
  { 203, 0 },
  { 203, 3 },
  { 204, 0 },
  { 204, 2 },
  { 205, 0 },
  { 205, 2 },
  { 207, 0 },
  { 207, 2 },
  { 207, 4 },
  { 207, 4 },
  { 150, 6 },
  { 202, 0 },
  { 202, 2 },
  { 150, 8 },
  { 220, 5 },
  { 220, 7 },
  { 220, 3 },
  { 220, 5 },
  { 150, 6 },
  { 150, 7 },
  { 221, 2 },
  { 221, 1 },
  { 222, 0 },
  { 222, 3 },
  { 219, 3 },
  { 219, 1 },
  { 174, 3 },
  { 173, 1 },
  { 174, 1 },
  { 174, 1 },
  { 174, 3 },
  { 174, 5 },
  { 173, 1 },
  { 173, 1 },
  { 173, 1 },
  { 174, 1 },
  { 174, 3 },
  { 174, 6 },
  { 174, 5 },
  { 174, 4 },
  { 173, 1 },
  { 174, 5 },
  { 174, 3 },
  { 174, 3 },
  { 174, 3 },
  { 174, 3 },
  { 174, 3 },
  { 174, 3 },
  { 174, 3 },
  { 174, 3 },
  { 223, 1 },
  { 223, 2 },
  { 174, 3 },
  { 174, 5 },
  { 174, 2 },
  { 174, 3 },
  { 174, 3 },
  { 174, 4 },
  { 174, 2 },
  { 174, 2 },
  { 174, 2 },
  { 174, 2 },
  { 224, 1 },
  { 224, 2 },
  { 174, 5 },
  { 225, 1 },
  { 225, 2 },
  { 174, 5 },
  { 174, 3 },
  { 174, 5 },
  { 174, 5 },
  { 174, 4 },
  { 174, 5 },
  { 228, 5 },
  { 228, 4 },
  { 229, 2 },
  { 229, 0 },
  { 227, 1 },
  { 227, 0 },
  { 210, 0 },
  { 209, 3 },
  { 209, 1 },
  { 226, 0 },
  { 226, 3 },
  { 150, 12 },
  { 230, 1 },
  { 230, 0 },
  { 178, 0 },
  { 178, 3 },
  { 188, 5 },
  { 188, 3 },
  { 231, 0 },
  { 231, 2 },
  { 150, 4 },
  { 150, 1 },
  { 150, 2 },
  { 150, 3 },
  { 150, 5 },
  { 150, 6 },
  { 150, 5 },
  { 150, 6 },
  { 170, 2 },
  { 171, 2 },
  { 150, 5 },
  { 233, 11 },
  { 235, 1 },
  { 235, 1 },
  { 235, 2 },
  { 235, 0 },
  { 236, 1 },
  { 236, 1 },
  { 236, 3 },
  { 238, 0 },
  { 238, 2 },
  { 234, 3 },
  { 234, 2 },
  { 240, 3 },
  { 241, 3 },
  { 241, 2 },
  { 239, 7 },
  { 239, 5 },
  { 239, 5 },
  { 239, 1 },
  { 174, 4 },
  { 174, 6 },
  { 192, 1 },
  { 192, 1 },
  { 192, 1 },
  { 150, 4 },
  { 150, 6 },
  { 150, 3 },
  { 243, 0 },
  { 243, 2 },
  { 150, 1 },
  { 150, 3 },
  { 150, 1 },
  { 150, 3 },
  { 150, 6 },
  { 150, 7 },
  { 244, 1 },
  { 150, 1 },
  { 150, 4 },
  { 246, 8 },
  { 248, 0 },
  { 249, 1 },
  { 249, 3 },
  { 250, 1 },
  { 197, 0 },
  { 197, 2 },
  { 197, 3 },
  { 252, 6 },
  { 252, 8 },
  { 145, 1 },
  { 146, 2 },
  { 146, 1 },
  { 147, 1 },
  { 147, 3 },
  { 148, 0 },
  { 152, 0 },
  { 152, 1 },
  { 152, 2 },
  { 154, 1 },
  { 154, 0 },
  { 150, 2 },
  { 161, 4 },
  { 161, 2 },
  { 153, 1 },
  { 153, 1 },
  { 153, 1 },
  { 167, 1 },
  { 168, 1 },
  { 169, 1 },
  { 169, 1 },
  { 166, 2 },
  { 166, 0 },
  { 172, 2 },
  { 162, 2 },
  { 184, 3 },
  { 184, 1 },
  { 185, 0 },
  { 189, 1 },
  { 191, 1 },
  { 195, 1 },
  { 196, 1 },
  { 211, 2 },
  { 212, 1 },
  { 174, 1 },
  { 210, 1 },
  { 232, 1 },
  { 232, 1 },
  { 232, 1 },
  { 232, 1 },
  { 232, 1 },
  { 170, 1 },
  { 237, 0 },
  { 237, 3 },
  { 240, 1 },
  { 241, 0 },
  { 242, 1 },
  { 242, 0 },
  { 245, 0 },
  { 245, 1 },
  { 247, 1 },
  { 247, 3 },
  { 248, 2 },
  { 251, 0 },
  { 251, 4 },
  { 251, 2 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  unsigned int yyruleno        /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  sqlite3ParserARG_FETCH;
  yymsp = yypParser->yytos;
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    yysize = yyRuleInfo[yyruleno].nrhs;
    fprintf(yyTraceFILE, "%sReduce [%s], go to state %d.\n", yyTracePrompt,
      yyRuleName[yyruleno], yymsp[-yysize].stateno);
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if( yyRuleInfo[yyruleno].nrhs==0 ){
#ifdef YYTRACKMAXSTACKDEPTH
    if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
      yypParser->yyhwm++;
      assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack));
    }
#endif
#if YYSTACKDEPTH>0 
    if( yypParser->yytos>=&yypParser->yystack[YYSTACKDEPTH-1] ){
      yyStackOverflow(yypParser);
      return;
    }
#else
    if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz-1] ){
      if( yyGrowStack(yypParser) ){
        yyStackOverflow(yypParser);
        return;
      }
      yymsp = yypParser->yytos;
    }
#endif
  }

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
/********** Begin reduce actions **********************************************/
        YYMINORTYPE yylhsminor;
      case 0: /* explain ::= EXPLAIN */
#line 113 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ pParse->explain = 1; }
#line 2311 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 1: /* explain ::= EXPLAIN QUERY PLAN */
#line 114 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ pParse->explain = 2; }
#line 2316 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 2: /* cmdx ::= cmd */
#line 116 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ sqlite3FinishCoding(pParse); }
#line 2321 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 3: /* cmd ::= BEGIN transtype trans_opt */
#line 121 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3BeginTransaction(pParse, yymsp[-1].minor.yy328);}
#line 2326 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 4: /* transtype ::= */
#line 126 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy328 = TK_DEFERRED;}
#line 2331 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 5: /* transtype ::= DEFERRED */
      case 6: /* transtype ::= IMMEDIATE */ yytestcase(yyruleno==6);
      case 7: /* transtype ::= EXCLUSIVE */ yytestcase(yyruleno==7);
#line 127 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = yymsp[0].major; /*A-overwrites-X*/}
#line 2338 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 8: /* cmd ::= COMMIT trans_opt */
      case 9: /* cmd ::= END trans_opt */ yytestcase(yyruleno==9);
#line 130 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3CommitTransaction(pParse);}
#line 2344 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 10: /* cmd ::= ROLLBACK trans_opt */
#line 132 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3RollbackTransaction(pParse);}
#line 2349 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 11: /* cmd ::= SAVEPOINT nm */
#line 136 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3Savepoint(pParse, SAVEPOINT_BEGIN, &yymsp[0].minor.yy0);
}
#line 2356 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 12: /* cmd ::= RELEASE savepoint_opt nm */
#line 139 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3Savepoint(pParse, SAVEPOINT_RELEASE, &yymsp[0].minor.yy0);
}
#line 2363 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 13: /* cmd ::= ROLLBACK trans_opt TO savepoint_opt nm */
#line 142 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3Savepoint(pParse, SAVEPOINT_ROLLBACK, &yymsp[0].minor.yy0);
}
#line 2370 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 14: /* create_table ::= createkw temp TABLE ifnotexists nm dbnm */
#line 149 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
   sqlite3StartTable(pParse,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy0,yymsp[-4].minor.yy328,0,0,yymsp[-2].minor.yy328);
}
#line 2377 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 15: /* createkw ::= CREATE */
#line 152 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{disableLookaside(pParse);}
#line 2382 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 16: /* ifnotexists ::= */
      case 19: /* temp ::= */ yytestcase(yyruleno==19);
      case 22: /* table_options ::= */ yytestcase(yyruleno==22);
      case 42: /* autoinc ::= */ yytestcase(yyruleno==42);
      case 57: /* init_deferred_pred_opt ::= */ yytestcase(yyruleno==57);
      case 67: /* defer_subclause_opt ::= */ yytestcase(yyruleno==67);
      case 76: /* ifexists ::= */ yytestcase(yyruleno==76);
      case 90: /* distinct ::= */ yytestcase(yyruleno==90);
      case 217: /* collate ::= */ yytestcase(yyruleno==217);
#line 155 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy328 = 0;}
#line 2395 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 17: /* ifnotexists ::= IF NOT EXISTS */
#line 156 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-2].minor.yy328 = 1;}
#line 2400 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 18: /* temp ::= TEMP */
      case 43: /* autoinc ::= AUTOINCR */ yytestcase(yyruleno==43);
#line 159 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = 1;}
#line 2406 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 20: /* create_table_args ::= LP columnlist conslist_opt RP table_options */
#line 162 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3EndTable(pParse,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0,yymsp[0].minor.yy328,0);
}
#line 2413 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 21: /* create_table_args ::= AS select */
#line 165 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3EndTable(pParse,0,0,0,yymsp[0].minor.yy3);
  sqlite3SelectDelete(pParse->db, yymsp[0].minor.yy3);
}
#line 2421 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 23: /* table_options ::= WITHOUT nm */
#line 171 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  if( yymsp[0].minor.yy0.n==5 && sqlite3_strnicmp(yymsp[0].minor.yy0.z,"rowid",5)==0 ){
    yymsp[-1].minor.yy328 = TF_WithoutRowid | TF_NoVisibleRowid;
  }else{
    yymsp[-1].minor.yy328 = 0;
    sqlite3ErrorMsg(pParse, "unknown table option: %.*s", yymsp[0].minor.yy0.n, yymsp[0].minor.yy0.z);
  }
}
#line 2433 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 24: /* columnname ::= nm typetoken */
#line 181 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3AddColumn(pParse,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy0);}
#line 2438 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 25: /* typetoken ::= */
      case 60: /* conslist_opt ::= */ yytestcase(yyruleno==60);
      case 96: /* as ::= */ yytestcase(yyruleno==96);
#line 246 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy0.n = 0; yymsp[1].minor.yy0.z = 0;}
#line 2445 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 26: /* typetoken ::= typename LP signed RP */
#line 248 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-3].minor.yy0.n = (int)(&yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n] - yymsp[-3].minor.yy0.z);
}
#line 2452 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 27: /* typetoken ::= typename LP signed COMMA signed RP */
#line 251 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-5].minor.yy0.n = (int)(&yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n] - yymsp[-5].minor.yy0.z);
}
#line 2459 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 28: /* typename ::= typename ID|STRING */
#line 256 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-1].minor.yy0.n=yymsp[0].minor.yy0.n+(int)(yymsp[0].minor.yy0.z-yymsp[-1].minor.yy0.z);}
#line 2464 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 29: /* ccons ::= CONSTRAINT nm */
      case 62: /* tcons ::= CONSTRAINT nm */ yytestcase(yyruleno==62);
#line 265 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{pParse->constraintName = yymsp[0].minor.yy0;}
#line 2470 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 30: /* ccons ::= DEFAULT term */
      case 32: /* ccons ::= DEFAULT PLUS term */ yytestcase(yyruleno==32);
#line 266 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3AddDefaultValue(pParse,&yymsp[0].minor.yy346);}
#line 2476 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 31: /* ccons ::= DEFAULT LP expr RP */
#line 267 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3AddDefaultValue(pParse,&yymsp[-1].minor.yy346);}
#line 2481 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 33: /* ccons ::= DEFAULT MINUS term */
#line 269 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  ExprSpan v;
  v.pExpr = sqlite3PExpr(pParse, TK_UMINUS, yymsp[0].minor.yy346.pExpr, 0, 0);
  v.zStart = yymsp[-1].minor.yy0.z;
  v.zEnd = yymsp[0].minor.yy346.zEnd;
  sqlite3AddDefaultValue(pParse,&v);
}
#line 2492 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 34: /* ccons ::= DEFAULT ID|INDEXED */
#line 276 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  ExprSpan v;
  spanExpr(&v, pParse, TK_STRING, yymsp[0].minor.yy0);
  sqlite3AddDefaultValue(pParse,&v);
}
#line 2501 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 35: /* ccons ::= NOT NULL onconf */
#line 286 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3AddNotNull(pParse, yymsp[0].minor.yy328);}
#line 2506 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 36: /* ccons ::= PRIMARY KEY sortorder onconf autoinc */
#line 288 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3AddPrimaryKey(pParse,0,yymsp[-1].minor.yy328,yymsp[0].minor.yy328,yymsp[-2].minor.yy328);}
#line 2511 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 37: /* ccons ::= UNIQUE onconf */
#line 289 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3CreateIndex(pParse,0,0,0,0,yymsp[0].minor.yy328,0,0,0,0,
                                   SQLITE_IDXTYPE_UNIQUE);}
#line 2517 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 38: /* ccons ::= CHECK LP expr RP */
#line 291 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3AddCheckConstraint(pParse,yymsp[-1].minor.yy346.pExpr);}
#line 2522 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 39: /* ccons ::= REFERENCES nm eidlist_opt refargs */
#line 293 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3CreateForeignKey(pParse,0,&yymsp[-2].minor.yy0,yymsp[-1].minor.yy14,yymsp[0].minor.yy328);}
#line 2527 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 40: /* ccons ::= defer_subclause */
#line 294 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3DeferForeignKey(pParse,yymsp[0].minor.yy328);}
#line 2532 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 41: /* ccons ::= COLLATE ID|STRING */
#line 295 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3AddCollateType(pParse, &yymsp[0].minor.yy0);}
#line 2537 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 44: /* refargs ::= */
#line 308 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[1].minor.yy328 = OE_None*0x0101; /* EV: R-19803-45884 */}
#line 2542 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 45: /* refargs ::= refargs refarg */
#line 309 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[-1].minor.yy328 = (yymsp[-1].minor.yy328 & ~yymsp[0].minor.yy429.mask) | yymsp[0].minor.yy429.value; }
#line 2547 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 46: /* refarg ::= MATCH nm */
#line 311 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[-1].minor.yy429.value = 0;     yymsp[-1].minor.yy429.mask = 0x000000; }
#line 2552 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 47: /* refarg ::= ON INSERT refact */
#line 312 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[-2].minor.yy429.value = 0;     yymsp[-2].minor.yy429.mask = 0x000000; }
#line 2557 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 48: /* refarg ::= ON DELETE refact */
#line 313 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[-2].minor.yy429.value = yymsp[0].minor.yy328;     yymsp[-2].minor.yy429.mask = 0x0000ff; }
#line 2562 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 49: /* refarg ::= ON UPDATE refact */
#line 314 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[-2].minor.yy429.value = yymsp[0].minor.yy328<<8;  yymsp[-2].minor.yy429.mask = 0x00ff00; }
#line 2567 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 50: /* refact ::= SET NULL */
#line 316 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[-1].minor.yy328 = OE_SetNull;  /* EV: R-33326-45252 */}
#line 2572 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 51: /* refact ::= SET DEFAULT */
#line 317 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[-1].minor.yy328 = OE_SetDflt;  /* EV: R-33326-45252 */}
#line 2577 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 52: /* refact ::= CASCADE */
#line 318 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[0].minor.yy328 = OE_Cascade;  /* EV: R-33326-45252 */}
#line 2582 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 53: /* refact ::= RESTRICT */
#line 319 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[0].minor.yy328 = OE_Restrict; /* EV: R-33326-45252 */}
#line 2587 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 54: /* refact ::= NO ACTION */
#line 320 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[-1].minor.yy328 = OE_None;     /* EV: R-33326-45252 */}
#line 2592 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 55: /* defer_subclause ::= NOT DEFERRABLE init_deferred_pred_opt */
#line 322 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-2].minor.yy328 = 0;}
#line 2597 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 56: /* defer_subclause ::= DEFERRABLE init_deferred_pred_opt */
      case 71: /* orconf ::= OR resolvetype */ yytestcase(yyruleno==71);
      case 146: /* insert_cmd ::= INSERT orconf */ yytestcase(yyruleno==146);
#line 323 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-1].minor.yy328 = yymsp[0].minor.yy328;}
#line 2604 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 58: /* init_deferred_pred_opt ::= INITIALLY DEFERRED */
      case 75: /* ifexists ::= IF EXISTS */ yytestcase(yyruleno==75);
      case 189: /* between_op ::= NOT BETWEEN */ yytestcase(yyruleno==189);
      case 192: /* in_op ::= NOT IN */ yytestcase(yyruleno==192);
      case 218: /* collate ::= COLLATE ID|STRING */ yytestcase(yyruleno==218);
#line 326 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-1].minor.yy328 = 1;}
#line 2613 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 59: /* init_deferred_pred_opt ::= INITIALLY IMMEDIATE */
#line 327 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-1].minor.yy328 = 0;}
#line 2618 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 61: /* tconscomma ::= COMMA */
#line 333 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{pParse->constraintName.n = 0;}
#line 2623 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 63: /* tcons ::= PRIMARY KEY LP sortlist autoinc RP onconf */
#line 337 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3AddPrimaryKey(pParse,yymsp[-3].minor.yy14,yymsp[0].minor.yy328,yymsp[-2].minor.yy328,0);}
#line 2628 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 64: /* tcons ::= UNIQUE LP sortlist RP onconf */
#line 339 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3CreateIndex(pParse,0,0,0,yymsp[-2].minor.yy14,yymsp[0].minor.yy328,0,0,0,0,
                                       SQLITE_IDXTYPE_UNIQUE);}
#line 2634 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 65: /* tcons ::= CHECK LP expr RP onconf */
#line 342 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3AddCheckConstraint(pParse,yymsp[-2].minor.yy346.pExpr);}
#line 2639 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 66: /* tcons ::= FOREIGN KEY LP eidlist RP REFERENCES nm eidlist_opt refargs defer_subclause_opt */
#line 344 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
    sqlite3CreateForeignKey(pParse, yymsp[-6].minor.yy14, &yymsp[-3].minor.yy0, yymsp[-2].minor.yy14, yymsp[-1].minor.yy328);
    sqlite3DeferForeignKey(pParse, yymsp[0].minor.yy328);
}
#line 2647 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 68: /* onconf ::= */
      case 70: /* orconf ::= */ yytestcase(yyruleno==70);
#line 358 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy328 = OE_Default;}
#line 2653 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 69: /* onconf ::= ON CONFLICT resolvetype */
#line 359 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-2].minor.yy328 = yymsp[0].minor.yy328;}
#line 2658 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 72: /* resolvetype ::= IGNORE */
#line 363 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = OE_Ignore;}
#line 2663 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 73: /* resolvetype ::= REPLACE */
      case 147: /* insert_cmd ::= REPLACE */ yytestcase(yyruleno==147);
#line 364 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = OE_Replace;}
#line 2669 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 74: /* cmd ::= DROP TABLE ifexists fullname */
#line 368 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3DropTable(pParse, yymsp[0].minor.yy65, 0, yymsp[-1].minor.yy328);
}
#line 2676 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 77: /* cmd ::= createkw temp VIEW ifnotexists nm dbnm eidlist_opt AS select */
#line 379 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3CreateView(pParse, &yymsp[-8].minor.yy0, &yymsp[-4].minor.yy0, &yymsp[-3].minor.yy0, yymsp[-2].minor.yy14, yymsp[0].minor.yy3, yymsp[-7].minor.yy328, yymsp[-5].minor.yy328);
}
#line 2683 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 78: /* cmd ::= DROP VIEW ifexists fullname */
#line 382 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3DropTable(pParse, yymsp[0].minor.yy65, 1, yymsp[-1].minor.yy328);
}
#line 2690 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 79: /* cmd ::= select */
#line 389 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  SelectDest dest = {SRT_Output, 0, 0, 0, 0, 0};
  sqlite3Select(pParse, yymsp[0].minor.yy3, &dest);
  sqlite3SelectDelete(pParse->db, yymsp[0].minor.yy3);
}
#line 2699 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 80: /* select ::= with selectnowith */
#line 426 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  Select *p = yymsp[0].minor.yy3;
  if( p ){
    p->pWith = yymsp[-1].minor.yy59;
    parserDoubleLinkSelect(pParse, p);
  }else{
    sqlite3WithDelete(pParse->db, yymsp[-1].minor.yy59);
  }
  yymsp[-1].minor.yy3 = p; /*A-overwrites-W*/
}
#line 2713 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 81: /* selectnowith ::= selectnowith multiselect_op oneselect */
#line 439 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  Select *pRhs = yymsp[0].minor.yy3;
  Select *pLhs = yymsp[-2].minor.yy3;
  if( pRhs && pRhs->pPrior ){
    SrcList *pFrom;
    Token x;
    x.n = 0;
    parserDoubleLinkSelect(pParse, pRhs);
    pFrom = sqlite3SrcListAppendFromTerm(pParse,0,0,0,&x,pRhs,0,0);
    pRhs = sqlite3SelectNew(pParse,0,pFrom,0,0,0,0,0,0,0,0);
  }
  if( pRhs ){
    pRhs->op = (u8)yymsp[-1].minor.yy328;
    pRhs->pPrior = pLhs;
    if( ALWAYS(pLhs) ) pLhs->selFlags &= ~SF_MultiValue;
    pRhs->selFlags &= ~SF_MultiValue;
    if( yymsp[-1].minor.yy328!=TK_ALL ) pParse->hasCompound = 1;
  }else{
    sqlite3SelectDelete(pParse->db, pLhs);
  }
  yymsp[-2].minor.yy3 = pRhs;
}
#line 2739 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 82: /* multiselect_op ::= UNION */
      case 84: /* multiselect_op ::= EXCEPT|INTERSECT */ yytestcase(yyruleno==84);
#line 462 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = yymsp[0].major; /*A-overwrites-OP*/}
#line 2745 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 83: /* multiselect_op ::= UNION ALL */
#line 463 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-1].minor.yy328 = TK_ALL;}
#line 2750 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 85: /* oneselect ::= SELECT distinct selcollist from where_opt groupby_opt having_opt forecast_opt orderby_opt limit_opt */
#line 467 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
#if SELECTTRACE_ENABLED
  Token s = yymsp[-9].minor.yy0; /*A-overwrites-S*/
#endif
  yymsp[-9].minor.yy3 = sqlite3SelectNew(pParse,yymsp[-7].minor.yy14,yymsp[-6].minor.yy65,yymsp[-5].minor.yy132,yymsp[-4].minor.yy14,yymsp[-3].minor.yy132,yymsp[-2].minor.yy132,yymsp[-1].minor.yy14,yymsp[-8].minor.yy328,yymsp[0].minor.yy476.pLimit,yymsp[0].minor.yy476.pOffset);
#if SELECTTRACE_ENABLED
  /* Populate the Select.zSelName[] string that is used to help with
  ** query planner debugging, to differentiate between multiple Select
  ** objects in a complex query.
  **
  ** If the SELECT keyword is immediately followed by a C-style comment
  ** then extract the first few alphanumeric characters from within that
  ** comment to be the zSelName value.  Otherwise, the label is #N where
  ** is an integer that is incremented with each SELECT statement seen.
  */
  if( yymsp[-9].minor.yy3!=0 ){
    const char *z = s.z+6;
    int i;
    sqlite3_snprintf(sizeof(yymsp[-9].minor.yy3->zSelName), yymsp[-9].minor.yy3->zSelName, "#%d",
                     ++pParse->nSelect);
    while( z[0]==' ' ) z++;
    if( z[0]=='/' && z[1]=='*' ){
      z += 2;
      while( z[0]==' ' ) z++;
      for(i=0; sqlite3Isalnum(z[i]); i++){}
      sqlite3_snprintf(sizeof(yymsp[-9].minor.yy3->zSelName), yymsp[-9].minor.yy3->zSelName, "%.*s", i, z);
    }
  }
#endif /* SELECTRACE_ENABLED */
}
#line 2784 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 86: /* values ::= VALUES LP nexprlist RP */
#line 501 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-3].minor.yy3 = sqlite3SelectNew(pParse,yymsp[-1].minor.yy14,0,0,0,0,0,0,SF_Values,0,0);
}
#line 2791 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 87: /* values ::= values COMMA LP exprlist RP */
#line 504 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  Select *pRight, *pLeft = yymsp[-4].minor.yy3;
  pRight = sqlite3SelectNew(pParse,yymsp[-1].minor.yy14,0,0,0,0,0,0,SF_Values|SF_MultiValue,0,0);
  if( ALWAYS(pLeft) ) pLeft->selFlags &= ~SF_MultiValue;
  if( pRight ){
    pRight->op = TK_ALL;
    pRight->pPrior = pLeft;
    yymsp[-4].minor.yy3 = pRight;
  }else{
    yymsp[-4].minor.yy3 = pLeft;
  }
}
#line 2807 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 88: /* distinct ::= DISTINCT */
#line 521 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = SF_Distinct;}
#line 2812 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 89: /* distinct ::= ALL */
#line 522 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = SF_All;}
#line 2817 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 91: /* sclp ::= */
      case 119: /* orderby_opt ::= */ yytestcase(yyruleno==119);
      case 126: /* groupby_opt ::= */ yytestcase(yyruleno==126);
      case 205: /* exprlist ::= */ yytestcase(yyruleno==205);
      case 208: /* paren_exprlist ::= */ yytestcase(yyruleno==208);
      case 213: /* eidlist_opt ::= */ yytestcase(yyruleno==213);
#line 535 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy14 = 0;}
#line 2827 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 92: /* selcollist ::= sclp expr as */
#line 536 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
   yymsp[-2].minor.yy14 = sqlite3ExprListAppend(pParse, yymsp[-2].minor.yy14, yymsp[-1].minor.yy346.pExpr);
   if( yymsp[0].minor.yy0.n>0 ) sqlite3ExprListSetName(pParse, yymsp[-2].minor.yy14, &yymsp[0].minor.yy0, 1);
   sqlite3ExprListSetSpan(pParse,yymsp[-2].minor.yy14,&yymsp[-1].minor.yy346);
}
#line 2836 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 93: /* selcollist ::= sclp STAR */
#line 541 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  Expr *p = sqlite3Expr(pParse->db, TK_ASTERISK, 0);
  yymsp[-1].minor.yy14 = sqlite3ExprListAppend(pParse, yymsp[-1].minor.yy14, p);
}
#line 2844 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 94: /* selcollist ::= sclp nm DOT STAR */
#line 545 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  Expr *pRight = sqlite3PExpr(pParse, TK_ASTERISK, 0, 0, 0);
  Expr *pLeft = sqlite3ExprAlloc(pParse->db, TK_ID, &yymsp[-2].minor.yy0, 1);
  Expr *pDot = sqlite3PExpr(pParse, TK_DOT, pLeft, pRight, 0);
  yymsp[-3].minor.yy14 = sqlite3ExprListAppend(pParse,yymsp[-3].minor.yy14, pDot);
}
#line 2854 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 95: /* as ::= AS nm */
      case 106: /* dbnm ::= DOT nm */ yytestcase(yyruleno==106);
      case 227: /* plus_num ::= PLUS INTEGER|FLOAT */ yytestcase(yyruleno==227);
      case 228: /* minus_num ::= MINUS INTEGER|FLOAT */ yytestcase(yyruleno==228);
#line 556 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-1].minor.yy0 = yymsp[0].minor.yy0;}
#line 2862 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 97: /* from ::= */
#line 570 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy65 = sqlite3DbMallocZero(pParse->db, sizeof(*yymsp[1].minor.yy65));}
#line 2867 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 98: /* from ::= FROM seltablist */
#line 571 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-1].minor.yy65 = yymsp[0].minor.yy65;
  sqlite3SrcListShiftJoinType(yymsp[-1].minor.yy65);
}
#line 2875 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 99: /* stl_prefix ::= seltablist joinop */
#line 579 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
   if( ALWAYS(yymsp[-1].minor.yy65 && yymsp[-1].minor.yy65->nSrc>0) ) yymsp[-1].minor.yy65->a[yymsp[-1].minor.yy65->nSrc-1].fg.jointype = (u8)yymsp[0].minor.yy328;
}
#line 2882 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 100: /* stl_prefix ::= */
#line 582 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy65 = 0;}
#line 2887 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 101: /* seltablist ::= stl_prefix nm dbnm as indexed_opt on_opt using_opt */
#line 584 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-6].minor.yy65 = sqlite3SrcListAppendFromTerm(pParse,yymsp[-6].minor.yy65,&yymsp[-5].minor.yy0,&yymsp[-4].minor.yy0,&yymsp[-3].minor.yy0,0,yymsp[-1].minor.yy132,yymsp[0].minor.yy408);
  sqlite3SrcListIndexedBy(pParse, yymsp[-6].minor.yy65, &yymsp[-2].minor.yy0);
}
#line 2895 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 102: /* seltablist ::= stl_prefix nm dbnm LP exprlist RP as on_opt using_opt */
#line 589 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-8].minor.yy65 = sqlite3SrcListAppendFromTerm(pParse,yymsp[-8].minor.yy65,&yymsp[-7].minor.yy0,&yymsp[-6].minor.yy0,&yymsp[-2].minor.yy0,0,yymsp[-1].minor.yy132,yymsp[0].minor.yy408);
  sqlite3SrcListFuncArgs(pParse, yymsp[-8].minor.yy65, yymsp[-4].minor.yy14);
}
#line 2903 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 103: /* seltablist ::= stl_prefix LP select RP as on_opt using_opt */
#line 595 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
    yymsp[-6].minor.yy65 = sqlite3SrcListAppendFromTerm(pParse,yymsp[-6].minor.yy65,0,0,&yymsp[-2].minor.yy0,yymsp[-4].minor.yy3,yymsp[-1].minor.yy132,yymsp[0].minor.yy408);
  }
#line 2910 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 104: /* seltablist ::= stl_prefix LP seltablist RP as on_opt using_opt */
#line 599 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
    if( yymsp[-6].minor.yy65==0 && yymsp[-2].minor.yy0.n==0 && yymsp[-1].minor.yy132==0 && yymsp[0].minor.yy408==0 ){
      yymsp[-6].minor.yy65 = yymsp[-4].minor.yy65;
    }else if( yymsp[-4].minor.yy65->nSrc==1 ){
      yymsp[-6].minor.yy65 = sqlite3SrcListAppendFromTerm(pParse,yymsp[-6].minor.yy65,0,0,&yymsp[-2].minor.yy0,0,yymsp[-1].minor.yy132,yymsp[0].minor.yy408);
      if( yymsp[-6].minor.yy65 ){
        struct SrcList_item *pNew = &yymsp[-6].minor.yy65->a[yymsp[-6].minor.yy65->nSrc-1];
        struct SrcList_item *pOld = yymsp[-4].minor.yy65->a;
        pNew->zName = pOld->zName;
        pNew->zDatabase = pOld->zDatabase;
        pNew->pSelect = pOld->pSelect;
        pOld->zName = pOld->zDatabase = 0;
        pOld->pSelect = 0;
      }
      sqlite3SrcListDelete(pParse->db, yymsp[-4].minor.yy65);
    }else{
      Select *pSubquery;
      sqlite3SrcListShiftJoinType(yymsp[-4].minor.yy65);
      pSubquery = sqlite3SelectNew(pParse,0,yymsp[-4].minor.yy65,0,0,0,0,0,SF_NestedFrom,0,0);
      yymsp[-6].minor.yy65 = sqlite3SrcListAppendFromTerm(pParse,yymsp[-6].minor.yy65,0,0,&yymsp[-2].minor.yy0,pSubquery,yymsp[-1].minor.yy132,yymsp[0].minor.yy408);
    }
  }
#line 2936 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 105: /* dbnm ::= */
      case 114: /* indexed_opt ::= */ yytestcase(yyruleno==114);
#line 624 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy0.z=0; yymsp[1].minor.yy0.n=0;}
#line 2942 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 107: /* fullname ::= nm dbnm */
#line 630 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-1].minor.yy65 = sqlite3SrcListAppend(pParse->db,0,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy0); /*A-overwrites-X*/}
#line 2947 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 108: /* joinop ::= COMMA|JOIN */
#line 633 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[0].minor.yy328 = JT_INNER; }
#line 2952 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 109: /* joinop ::= JOIN_KW JOIN */
#line 635 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-1].minor.yy328 = sqlite3JoinType(pParse,&yymsp[-1].minor.yy0,0,0);  /*X-overwrites-A*/}
#line 2957 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 110: /* joinop ::= JOIN_KW nm JOIN */
#line 637 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-2].minor.yy328 = sqlite3JoinType(pParse,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0,0); /*X-overwrites-A*/}
#line 2962 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 111: /* joinop ::= JOIN_KW nm nm JOIN */
#line 639 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-3].minor.yy328 = sqlite3JoinType(pParse,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0);/*X-overwrites-A*/}
#line 2967 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 112: /* on_opt ::= ON expr */
      case 129: /* having_opt ::= HAVING expr */ yytestcase(yyruleno==129);
      case 131: /* forecast_opt ::= FORECAST expr */ yytestcase(yyruleno==131);
      case 138: /* where_opt ::= WHERE expr */ yytestcase(yyruleno==138);
      case 201: /* case_else ::= ELSE expr */ yytestcase(yyruleno==201);
#line 643 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-1].minor.yy132 = yymsp[0].minor.yy346.pExpr;}
#line 2976 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 113: /* on_opt ::= */
      case 128: /* having_opt ::= */ yytestcase(yyruleno==128);
      case 130: /* forecast_opt ::= */ yytestcase(yyruleno==130);
      case 137: /* where_opt ::= */ yytestcase(yyruleno==137);
      case 202: /* case_else ::= */ yytestcase(yyruleno==202);
      case 204: /* case_operand ::= */ yytestcase(yyruleno==204);
#line 644 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy132 = 0;}
#line 2986 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 115: /* indexed_opt ::= INDEXED BY nm */
#line 658 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-2].minor.yy0 = yymsp[0].minor.yy0;}
#line 2991 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 116: /* indexed_opt ::= NOT INDEXED */
#line 659 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-1].minor.yy0.z=0; yymsp[-1].minor.yy0.n=1;}
#line 2996 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 117: /* using_opt ::= USING LP idlist RP */
#line 663 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-3].minor.yy408 = yymsp[-1].minor.yy408;}
#line 3001 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 118: /* using_opt ::= */
      case 148: /* idlist_opt ::= */ yytestcase(yyruleno==148);
#line 664 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy408 = 0;}
#line 3007 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 120: /* orderby_opt ::= ORDER BY sortlist */
      case 127: /* groupby_opt ::= GROUP BY nexprlist */ yytestcase(yyruleno==127);
#line 678 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-2].minor.yy14 = yymsp[0].minor.yy14;}
#line 3013 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 121: /* sortlist ::= sortlist COMMA expr sortorder */
#line 679 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-3].minor.yy14 = sqlite3ExprListAppend(pParse,yymsp[-3].minor.yy14,yymsp[-1].minor.yy346.pExpr);
  sqlite3ExprListSetSortOrder(yymsp[-3].minor.yy14,yymsp[0].minor.yy328);
}
#line 3021 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 122: /* sortlist ::= expr sortorder */
#line 683 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-1].minor.yy14 = sqlite3ExprListAppend(pParse,0,yymsp[-1].minor.yy346.pExpr); /*A-overwrites-Y*/
  sqlite3ExprListSetSortOrder(yymsp[-1].minor.yy14,yymsp[0].minor.yy328);
}
#line 3029 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 123: /* sortorder ::= ASC */
#line 690 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = SQLITE_SO_ASC;}
#line 3034 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 124: /* sortorder ::= DESC */
#line 691 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = SQLITE_SO_DESC;}
#line 3039 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 125: /* sortorder ::= */
#line 692 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy328 = SQLITE_SO_UNDEFINED;}
#line 3044 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 132: /* limit_opt ::= */
#line 722 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy476.pLimit = 0; yymsp[1].minor.yy476.pOffset = 0;}
#line 3049 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 133: /* limit_opt ::= LIMIT expr */
#line 723 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-1].minor.yy476.pLimit = yymsp[0].minor.yy346.pExpr; yymsp[-1].minor.yy476.pOffset = 0;}
#line 3054 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 134: /* limit_opt ::= LIMIT expr OFFSET expr */
#line 725 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-3].minor.yy476.pLimit = yymsp[-2].minor.yy346.pExpr; yymsp[-3].minor.yy476.pOffset = yymsp[0].minor.yy346.pExpr;}
#line 3059 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 135: /* limit_opt ::= LIMIT expr COMMA expr */
#line 727 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-3].minor.yy476.pOffset = yymsp[-2].minor.yy346.pExpr; yymsp[-3].minor.yy476.pLimit = yymsp[0].minor.yy346.pExpr;}
#line 3064 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 136: /* cmd ::= with DELETE FROM fullname indexed_opt where_opt */
#line 741 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3WithPush(pParse, yymsp[-5].minor.yy59, 1);
  sqlite3SrcListIndexedBy(pParse, yymsp[-2].minor.yy65, &yymsp[-1].minor.yy0);
  sqlite3DeleteFrom(pParse,yymsp[-2].minor.yy65,yymsp[0].minor.yy132);
}
#line 3073 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 139: /* cmd ::= with UPDATE orconf fullname indexed_opt SET setlist where_opt */
#line 768 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3WithPush(pParse, yymsp[-7].minor.yy59, 1);
  sqlite3SrcListIndexedBy(pParse, yymsp[-4].minor.yy65, &yymsp[-3].minor.yy0);
  sqlite3ExprListCheckLength(pParse,yymsp[-1].minor.yy14,"set list"); 
  sqlite3Update(pParse,yymsp[-4].minor.yy65,yymsp[-1].minor.yy14,yymsp[0].minor.yy132,yymsp[-5].minor.yy328);
}
#line 3083 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 140: /* setlist ::= setlist COMMA nm EQ expr */
#line 779 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-4].minor.yy14 = sqlite3ExprListAppend(pParse, yymsp[-4].minor.yy14, yymsp[0].minor.yy346.pExpr);
  sqlite3ExprListSetName(pParse, yymsp[-4].minor.yy14, &yymsp[-2].minor.yy0, 1);
}
#line 3091 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 141: /* setlist ::= setlist COMMA LP idlist RP EQ expr */
#line 783 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-6].minor.yy14 = sqlite3ExprListAppendVector(pParse, yymsp[-6].minor.yy14, yymsp[-3].minor.yy408, yymsp[0].minor.yy346.pExpr);
}
#line 3098 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 142: /* setlist ::= nm EQ expr */
#line 786 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yylhsminor.yy14 = sqlite3ExprListAppend(pParse, 0, yymsp[0].minor.yy346.pExpr);
  sqlite3ExprListSetName(pParse, yylhsminor.yy14, &yymsp[-2].minor.yy0, 1);
}
#line 3106 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
  yymsp[-2].minor.yy14 = yylhsminor.yy14;
        break;
      case 143: /* setlist ::= LP idlist RP EQ expr */
#line 790 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-4].minor.yy14 = sqlite3ExprListAppendVector(pParse, 0, yymsp[-3].minor.yy408, yymsp[0].minor.yy346.pExpr);
}
#line 3114 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 144: /* cmd ::= with insert_cmd INTO fullname idlist_opt select */
#line 796 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3WithPush(pParse, yymsp[-5].minor.yy59, 1);
  sqlite3Insert(pParse, yymsp[-2].minor.yy65, yymsp[0].minor.yy3, yymsp[-1].minor.yy408, yymsp[-4].minor.yy328);
}
#line 3122 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 145: /* cmd ::= with insert_cmd INTO fullname idlist_opt DEFAULT VALUES */
#line 801 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3WithPush(pParse, yymsp[-6].minor.yy59, 1);
  sqlite3Insert(pParse, yymsp[-3].minor.yy65, 0, yymsp[-2].minor.yy408, yymsp[-5].minor.yy328);
}
#line 3130 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 149: /* idlist_opt ::= LP idlist RP */
#line 816 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-2].minor.yy408 = yymsp[-1].minor.yy408;}
#line 3135 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 150: /* idlist ::= idlist COMMA nm */
#line 818 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-2].minor.yy408 = sqlite3IdListAppend(pParse->db,yymsp[-2].minor.yy408,&yymsp[0].minor.yy0);}
#line 3140 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 151: /* idlist ::= nm */
#line 820 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy408 = sqlite3IdListAppend(pParse->db,0,&yymsp[0].minor.yy0); /*A-overwrites-Y*/}
#line 3145 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 152: /* expr ::= LP expr RP */
#line 870 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{spanSet(&yymsp[-2].minor.yy346,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0); /*A-overwrites-B*/  yymsp[-2].minor.yy346.pExpr = yymsp[-1].minor.yy346.pExpr;}
#line 3150 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 153: /* term ::= NULL */
      case 158: /* term ::= FLOAT|BLOB */ yytestcase(yyruleno==158);
      case 159: /* term ::= STRING */ yytestcase(yyruleno==159);
#line 871 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{spanExpr(&yymsp[0].minor.yy346,pParse,yymsp[0].major,yymsp[0].minor.yy0);/*A-overwrites-X*/}
#line 3157 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 154: /* expr ::= ID|INDEXED */
      case 155: /* expr ::= JOIN_KW */ yytestcase(yyruleno==155);
#line 872 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{spanExpr(&yymsp[0].minor.yy346,pParse,TK_ID,yymsp[0].minor.yy0); /*A-overwrites-X*/}
#line 3163 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 156: /* expr ::= nm DOT nm */
#line 874 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  Expr *temp1 = sqlite3ExprAlloc(pParse->db, TK_ID, &yymsp[-2].minor.yy0, 1);
  Expr *temp2 = sqlite3ExprAlloc(pParse->db, TK_ID, &yymsp[0].minor.yy0, 1);
  spanSet(&yymsp[-2].minor.yy346,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0); /*A-overwrites-X*/
  yymsp[-2].minor.yy346.pExpr = sqlite3PExpr(pParse, TK_DOT, temp1, temp2, 0);
}
#line 3173 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 157: /* expr ::= nm DOT nm DOT nm */
#line 880 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  Expr *temp1 = sqlite3ExprAlloc(pParse->db, TK_ID, &yymsp[-4].minor.yy0, 1);
  Expr *temp2 = sqlite3ExprAlloc(pParse->db, TK_ID, &yymsp[-2].minor.yy0, 1);
  Expr *temp3 = sqlite3ExprAlloc(pParse->db, TK_ID, &yymsp[0].minor.yy0, 1);
  Expr *temp4 = sqlite3PExpr(pParse, TK_DOT, temp2, temp3, 0);
  spanSet(&yymsp[-4].minor.yy346,&yymsp[-4].minor.yy0,&yymsp[0].minor.yy0); /*A-overwrites-X*/
  yymsp[-4].minor.yy346.pExpr = sqlite3PExpr(pParse, TK_DOT, temp1, temp4, 0);
}
#line 3185 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 160: /* term ::= INTEGER */
#line 890 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yylhsminor.yy346.pExpr = sqlite3ExprAlloc(pParse->db, TK_INTEGER, &yymsp[0].minor.yy0, 1);
  yylhsminor.yy346.zStart = yymsp[0].minor.yy0.z;
  yylhsminor.yy346.zEnd = yymsp[0].minor.yy0.z + yymsp[0].minor.yy0.n;
  if( yylhsminor.yy346.pExpr ) yylhsminor.yy346.pExpr->flags |= EP_Leaf;
}
#line 3195 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
  yymsp[0].minor.yy346 = yylhsminor.yy346;
        break;
      case 161: /* expr ::= VARIABLE */
#line 896 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  if( !(yymsp[0].minor.yy0.z[0]=='#' && sqlite3Isdigit(yymsp[0].minor.yy0.z[1])) ){
    u32 n = yymsp[0].minor.yy0.n;
    spanExpr(&yymsp[0].minor.yy346, pParse, TK_VARIABLE, yymsp[0].minor.yy0);
    sqlite3ExprAssignVarNumber(pParse, yymsp[0].minor.yy346.pExpr, n);
  }else{
    /* When doing a nested parse, one can include terms in an expression
    ** that look like this:   #1 #2 ...  These terms refer to registers
    ** in the virtual machine.  #N is the N-th register. */
    Token t = yymsp[0].minor.yy0; /*A-overwrites-X*/
    assert( t.n>=2 );
    spanSet(&yymsp[0].minor.yy346, &t, &t);
    if( pParse->nested==0 ){
      sqlite3ErrorMsg(pParse, "near \"%T\": syntax error", &t);
      yymsp[0].minor.yy346.pExpr = 0;
    }else{
      yymsp[0].minor.yy346.pExpr = sqlite3PExpr(pParse, TK_REGISTER, 0, 0, 0);
      if( yymsp[0].minor.yy346.pExpr ) sqlite3GetInt32(&t.z[1], &yymsp[0].minor.yy346.pExpr->iTable);
    }
  }
}
#line 3221 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 162: /* expr ::= expr COLLATE ID|STRING */
#line 917 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-2].minor.yy346.pExpr = sqlite3ExprAddCollateToken(pParse, yymsp[-2].minor.yy346.pExpr, &yymsp[0].minor.yy0, 1);
  yymsp[-2].minor.yy346.zEnd = &yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n];
}
#line 3229 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 163: /* expr ::= CAST LP expr AS typetoken RP */
#line 922 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  spanSet(&yymsp[-5].minor.yy346,&yymsp[-5].minor.yy0,&yymsp[0].minor.yy0); /*A-overwrites-X*/
  yymsp[-5].minor.yy346.pExpr = sqlite3ExprAlloc(pParse->db, TK_CAST, &yymsp[-1].minor.yy0, 1);
  sqlite3ExprAttachSubtrees(pParse->db, yymsp[-5].minor.yy346.pExpr, yymsp[-3].minor.yy346.pExpr, 0);
}
#line 3238 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 164: /* expr ::= ID|INDEXED LP distinct exprlist RP */
#line 928 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  if( yymsp[-1].minor.yy14 && yymsp[-1].minor.yy14->nExpr>pParse->db->aLimit[SQLITE_LIMIT_FUNCTION_ARG] ){
    sqlite3ErrorMsg(pParse, "too many arguments on function %T", &yymsp[-4].minor.yy0);
  }
  yylhsminor.yy346.pExpr = sqlite3ExprFunction(pParse, yymsp[-1].minor.yy14, &yymsp[-4].minor.yy0);
  spanSet(&yylhsminor.yy346,&yymsp[-4].minor.yy0,&yymsp[0].minor.yy0);
  if( yymsp[-2].minor.yy328==SF_Distinct && yylhsminor.yy346.pExpr ){
    yylhsminor.yy346.pExpr->flags |= EP_Distinct;
  }
}
#line 3252 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
  yymsp[-4].minor.yy346 = yylhsminor.yy346;
        break;
      case 165: /* expr ::= ID|INDEXED LP STAR RP */
#line 938 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yylhsminor.yy346.pExpr = sqlite3ExprFunction(pParse, 0, &yymsp[-3].minor.yy0);
  spanSet(&yylhsminor.yy346,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);
}
#line 3261 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
  yymsp[-3].minor.yy346 = yylhsminor.yy346;
        break;
      case 166: /* term ::= CTIME_KW */
#line 942 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yylhsminor.yy346.pExpr = sqlite3ExprFunction(pParse, 0, &yymsp[0].minor.yy0);
  spanSet(&yylhsminor.yy346, &yymsp[0].minor.yy0, &yymsp[0].minor.yy0);
}
#line 3270 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
  yymsp[0].minor.yy346 = yylhsminor.yy346;
        break;
      case 167: /* expr ::= LP nexprlist COMMA expr RP */
#line 971 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  ExprList *pList = sqlite3ExprListAppend(pParse, yymsp[-3].minor.yy14, yymsp[-1].minor.yy346.pExpr);
  yylhsminor.yy346.pExpr = sqlite3PExpr(pParse, TK_VECTOR, 0, 0, 0);
  if( yylhsminor.yy346.pExpr ){
    yylhsminor.yy346.pExpr->x.pList = pList;
    spanSet(&yylhsminor.yy346, &yymsp[-4].minor.yy0, &yymsp[0].minor.yy0);
  }else{
    sqlite3ExprListDelete(pParse->db, pList);
  }
}
#line 3285 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
  yymsp[-4].minor.yy346 = yylhsminor.yy346;
        break;
      case 168: /* expr ::= expr AND expr */
      case 169: /* expr ::= expr OR expr */ yytestcase(yyruleno==169);
      case 170: /* expr ::= expr LT|GT|GE|LE expr */ yytestcase(yyruleno==170);
      case 171: /* expr ::= expr EQ|NE expr */ yytestcase(yyruleno==171);
      case 172: /* expr ::= expr BITAND|BITOR|LSHIFT|RSHIFT expr */ yytestcase(yyruleno==172);
      case 173: /* expr ::= expr PLUS|MINUS expr */ yytestcase(yyruleno==173);
      case 174: /* expr ::= expr STAR|SLASH|REM expr */ yytestcase(yyruleno==174);
      case 175: /* expr ::= expr CONCAT expr */ yytestcase(yyruleno==175);
#line 982 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{spanBinaryExpr(pParse,yymsp[-1].major,&yymsp[-2].minor.yy346,&yymsp[0].minor.yy346);}
#line 3298 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 176: /* likeop ::= LIKE_KW|MATCH */
#line 995 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy0=yymsp[0].minor.yy0;/*A-overwrites-X*/}
#line 3303 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 177: /* likeop ::= NOT LIKE_KW|MATCH */
#line 996 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-1].minor.yy0=yymsp[0].minor.yy0; yymsp[-1].minor.yy0.n|=0x80000000; /*yymsp[-1].minor.yy0-overwrite-yymsp[0].minor.yy0*/}
#line 3308 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 178: /* expr ::= expr likeop expr */
#line 997 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  ExprList *pList;
  int bNot = yymsp[-1].minor.yy0.n & 0x80000000;
  yymsp[-1].minor.yy0.n &= 0x7fffffff;
  pList = sqlite3ExprListAppend(pParse,0, yymsp[0].minor.yy346.pExpr);
  pList = sqlite3ExprListAppend(pParse,pList, yymsp[-2].minor.yy346.pExpr);
  yymsp[-2].minor.yy346.pExpr = sqlite3ExprFunction(pParse, pList, &yymsp[-1].minor.yy0);
  exprNot(pParse, bNot, &yymsp[-2].minor.yy346);
  yymsp[-2].minor.yy346.zEnd = yymsp[0].minor.yy346.zEnd;
  if( yymsp[-2].minor.yy346.pExpr ) yymsp[-2].minor.yy346.pExpr->flags |= EP_InfixFunc;
}
#line 3323 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 179: /* expr ::= expr likeop expr ESCAPE expr */
#line 1008 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  ExprList *pList;
  int bNot = yymsp[-3].minor.yy0.n & 0x80000000;
  yymsp[-3].minor.yy0.n &= 0x7fffffff;
  pList = sqlite3ExprListAppend(pParse,0, yymsp[-2].minor.yy346.pExpr);
  pList = sqlite3ExprListAppend(pParse,pList, yymsp[-4].minor.yy346.pExpr);
  pList = sqlite3ExprListAppend(pParse,pList, yymsp[0].minor.yy346.pExpr);
  yymsp[-4].minor.yy346.pExpr = sqlite3ExprFunction(pParse, pList, &yymsp[-3].minor.yy0);
  exprNot(pParse, bNot, &yymsp[-4].minor.yy346);
  yymsp[-4].minor.yy346.zEnd = yymsp[0].minor.yy346.zEnd;
  if( yymsp[-4].minor.yy346.pExpr ) yymsp[-4].minor.yy346.pExpr->flags |= EP_InfixFunc;
}
#line 3339 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 180: /* expr ::= expr ISNULL|NOTNULL */
#line 1035 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{spanUnaryPostfix(pParse,yymsp[0].major,&yymsp[-1].minor.yy346,&yymsp[0].minor.yy0);}
#line 3344 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 181: /* expr ::= expr NOT NULL */
#line 1036 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{spanUnaryPostfix(pParse,TK_NOTNULL,&yymsp[-2].minor.yy346,&yymsp[0].minor.yy0);}
#line 3349 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 182: /* expr ::= expr IS expr */
#line 1057 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  spanBinaryExpr(pParse,TK_IS,&yymsp[-2].minor.yy346,&yymsp[0].minor.yy346);
  binaryToUnaryIfNull(pParse, yymsp[0].minor.yy346.pExpr, yymsp[-2].minor.yy346.pExpr, TK_ISNULL);
}
#line 3357 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 183: /* expr ::= expr IS NOT expr */
#line 1061 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  spanBinaryExpr(pParse,TK_ISNOT,&yymsp[-3].minor.yy346,&yymsp[0].minor.yy346);
  binaryToUnaryIfNull(pParse, yymsp[0].minor.yy346.pExpr, yymsp[-3].minor.yy346.pExpr, TK_NOTNULL);
}
#line 3365 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 184: /* expr ::= NOT expr */
      case 185: /* expr ::= BITNOT expr */ yytestcase(yyruleno==185);
#line 1085 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{spanUnaryPrefix(&yymsp[-1].minor.yy346,pParse,yymsp[-1].major,&yymsp[0].minor.yy346,&yymsp[-1].minor.yy0);/*A-overwrites-B*/}
#line 3371 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 186: /* expr ::= MINUS expr */
#line 1089 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{spanUnaryPrefix(&yymsp[-1].minor.yy346,pParse,TK_UMINUS,&yymsp[0].minor.yy346,&yymsp[-1].minor.yy0);/*A-overwrites-B*/}
#line 3376 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 187: /* expr ::= PLUS expr */
#line 1091 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{spanUnaryPrefix(&yymsp[-1].minor.yy346,pParse,TK_UPLUS,&yymsp[0].minor.yy346,&yymsp[-1].minor.yy0);/*A-overwrites-B*/}
#line 3381 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 188: /* between_op ::= BETWEEN */
      case 191: /* in_op ::= IN */ yytestcase(yyruleno==191);
#line 1094 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = 0;}
#line 3387 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 190: /* expr ::= expr between_op expr AND expr */
#line 1096 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  ExprList *pList = sqlite3ExprListAppend(pParse,0, yymsp[-2].minor.yy346.pExpr);
  pList = sqlite3ExprListAppend(pParse,pList, yymsp[0].minor.yy346.pExpr);
  yymsp[-4].minor.yy346.pExpr = sqlite3PExpr(pParse, TK_BETWEEN, yymsp[-4].minor.yy346.pExpr, 0, 0);
  if( yymsp[-4].minor.yy346.pExpr ){
    yymsp[-4].minor.yy346.pExpr->x.pList = pList;
  }else{
    sqlite3ExprListDelete(pParse->db, pList);
  } 
  exprNot(pParse, yymsp[-3].minor.yy328, &yymsp[-4].minor.yy346);
  yymsp[-4].minor.yy346.zEnd = yymsp[0].minor.yy346.zEnd;
}
#line 3403 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 193: /* expr ::= expr in_op LP exprlist RP */
#line 1112 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
    if( yymsp[-1].minor.yy14==0 ){
      /* Expressions of the form
      **
      **      expr1 IN ()
      **      expr1 NOT IN ()
      **
      ** simplify to constants 0 (false) and 1 (true), respectively,
      ** regardless of the value of expr1.
      */
      sqlite3ExprDelete(pParse->db, yymsp[-4].minor.yy346.pExpr);
      yymsp[-4].minor.yy346.pExpr = sqlite3ExprAlloc(pParse->db, TK_INTEGER,&sqlite3IntTokens[yymsp[-3].minor.yy328],1);
    }else if( yymsp[-1].minor.yy14->nExpr==1 ){
      /* Expressions of the form:
      **
      **      expr1 IN (?1)
      **      expr1 NOT IN (?2)
      **
      ** with exactly one value on the RHS can be simplified to something
      ** like this:
      **
      **      expr1 == ?1
      **      expr1 <> ?2
      **
      ** But, the RHS of the == or <> is marked with the EP_Generic flag
      ** so that it may not contribute to the computation of comparison
      ** affinity or the collating sequence to use for comparison.  Otherwise,
      ** the semantics would be subtly different from IN or NOT IN.
      */
      Expr *pRHS = yymsp[-1].minor.yy14->a[0].pExpr;
      yymsp[-1].minor.yy14->a[0].pExpr = 0;
      sqlite3ExprListDelete(pParse->db, yymsp[-1].minor.yy14);
      /* pRHS cannot be NULL because a malloc error would have been detected
      ** before now and control would have never reached this point */
      if( ALWAYS(pRHS) ){
        pRHS->flags &= ~EP_Collate;
        pRHS->flags |= EP_Generic;
      }
      yymsp[-4].minor.yy346.pExpr = sqlite3PExpr(pParse, yymsp[-3].minor.yy328 ? TK_NE : TK_EQ, yymsp[-4].minor.yy346.pExpr, pRHS, 0);
    }else{
      yymsp[-4].minor.yy346.pExpr = sqlite3PExpr(pParse, TK_IN, yymsp[-4].minor.yy346.pExpr, 0, 0);
      if( yymsp[-4].minor.yy346.pExpr ){
        yymsp[-4].minor.yy346.pExpr->x.pList = yymsp[-1].minor.yy14;
        sqlite3ExprSetHeightAndFlags(pParse, yymsp[-4].minor.yy346.pExpr);
      }else{
        sqlite3ExprListDelete(pParse->db, yymsp[-1].minor.yy14);
      }
      exprNot(pParse, yymsp[-3].minor.yy328, &yymsp[-4].minor.yy346);
    }
    yymsp[-4].minor.yy346.zEnd = &yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n];
  }
#line 3458 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 194: /* expr ::= LP select RP */
#line 1163 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
    spanSet(&yymsp[-2].minor.yy346,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0); /*A-overwrites-B*/
    yymsp[-2].minor.yy346.pExpr = sqlite3PExpr(pParse, TK_SELECT, 0, 0, 0);
    sqlite3PExprAddSelect(pParse, yymsp[-2].minor.yy346.pExpr, yymsp[-1].minor.yy3);
  }
#line 3467 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 195: /* expr ::= expr in_op LP select RP */
#line 1168 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
    yymsp[-4].minor.yy346.pExpr = sqlite3PExpr(pParse, TK_IN, yymsp[-4].minor.yy346.pExpr, 0, 0);
    sqlite3PExprAddSelect(pParse, yymsp[-4].minor.yy346.pExpr, yymsp[-1].minor.yy3);
    exprNot(pParse, yymsp[-3].minor.yy328, &yymsp[-4].minor.yy346);
    yymsp[-4].minor.yy346.zEnd = &yymsp[0].minor.yy0.z[yymsp[0].minor.yy0.n];
  }
#line 3477 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 196: /* expr ::= expr in_op nm dbnm paren_exprlist */
#line 1174 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
    SrcList *pSrc = sqlite3SrcListAppend(pParse->db, 0,&yymsp[-2].minor.yy0,&yymsp[-1].minor.yy0);
    Select *pSelect = sqlite3SelectNew(pParse, 0,pSrc,0,0,0,0,0,0,0,0);
    if( yymsp[0].minor.yy14 )  sqlite3SrcListFuncArgs(pParse, pSelect ? pSrc : 0, yymsp[0].minor.yy14);
    yymsp[-4].minor.yy346.pExpr = sqlite3PExpr(pParse, TK_IN, yymsp[-4].minor.yy346.pExpr, 0, 0);
    sqlite3PExprAddSelect(pParse, yymsp[-4].minor.yy346.pExpr, pSelect);
    exprNot(pParse, yymsp[-3].minor.yy328, &yymsp[-4].minor.yy346);
    yymsp[-4].minor.yy346.zEnd = yymsp[-1].minor.yy0.z ? &yymsp[-1].minor.yy0.z[yymsp[-1].minor.yy0.n] : &yymsp[-2].minor.yy0.z[yymsp[-2].minor.yy0.n];
  }
#line 3490 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 197: /* expr ::= EXISTS LP select RP */
#line 1183 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
    Expr *p;
    spanSet(&yymsp[-3].minor.yy346,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0); /*A-overwrites-B*/
    p = yymsp[-3].minor.yy346.pExpr = sqlite3PExpr(pParse, TK_EXISTS, 0, 0, 0);
    sqlite3PExprAddSelect(pParse, p, yymsp[-1].minor.yy3);
  }
#line 3500 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 198: /* expr ::= CASE case_operand case_exprlist case_else END */
#line 1192 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  spanSet(&yymsp[-4].minor.yy346,&yymsp[-4].minor.yy0,&yymsp[0].minor.yy0);  /*A-overwrites-C*/
  yymsp[-4].minor.yy346.pExpr = sqlite3PExpr(pParse, TK_CASE, yymsp[-3].minor.yy132, 0, 0);
  if( yymsp[-4].minor.yy346.pExpr ){
    yymsp[-4].minor.yy346.pExpr->x.pList = yymsp[-1].minor.yy132 ? sqlite3ExprListAppend(pParse,yymsp[-2].minor.yy14,yymsp[-1].minor.yy132) : yymsp[-2].minor.yy14;
    sqlite3ExprSetHeightAndFlags(pParse, yymsp[-4].minor.yy346.pExpr);
  }else{
    sqlite3ExprListDelete(pParse->db, yymsp[-2].minor.yy14);
    sqlite3ExprDelete(pParse->db, yymsp[-1].minor.yy132);
  }
}
#line 3515 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 199: /* case_exprlist ::= case_exprlist WHEN expr THEN expr */
#line 1205 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-4].minor.yy14 = sqlite3ExprListAppend(pParse,yymsp[-4].minor.yy14, yymsp[-2].minor.yy346.pExpr);
  yymsp[-4].minor.yy14 = sqlite3ExprListAppend(pParse,yymsp[-4].minor.yy14, yymsp[0].minor.yy346.pExpr);
}
#line 3523 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 200: /* case_exprlist ::= WHEN expr THEN expr */
#line 1209 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-3].minor.yy14 = sqlite3ExprListAppend(pParse,0, yymsp[-2].minor.yy346.pExpr);
  yymsp[-3].minor.yy14 = sqlite3ExprListAppend(pParse,yymsp[-3].minor.yy14, yymsp[0].minor.yy346.pExpr);
}
#line 3531 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 203: /* case_operand ::= expr */
#line 1219 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy132 = yymsp[0].minor.yy346.pExpr; /*A-overwrites-X*/}
#line 3536 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 206: /* nexprlist ::= nexprlist COMMA expr */
#line 1230 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-2].minor.yy14 = sqlite3ExprListAppend(pParse,yymsp[-2].minor.yy14,yymsp[0].minor.yy346.pExpr);}
#line 3541 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 207: /* nexprlist ::= expr */
#line 1232 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy14 = sqlite3ExprListAppend(pParse,0,yymsp[0].minor.yy346.pExpr); /*A-overwrites-Y*/}
#line 3546 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 209: /* paren_exprlist ::= LP exprlist RP */
      case 214: /* eidlist_opt ::= LP eidlist RP */ yytestcase(yyruleno==214);
#line 1240 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-2].minor.yy14 = yymsp[-1].minor.yy14;}
#line 3552 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 210: /* cmd ::= createkw uniqueflag INDEX ifnotexists nm dbnm ON nm LP sortlist RP where_opt */
#line 1247 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3CreateIndex(pParse, &yymsp[-7].minor.yy0, &yymsp[-6].minor.yy0, 
                     sqlite3SrcListAppend(pParse->db,0,&yymsp[-4].minor.yy0,0), yymsp[-2].minor.yy14, yymsp[-10].minor.yy328,
                      &yymsp[-11].minor.yy0, yymsp[0].minor.yy132, SQLITE_SO_ASC, yymsp[-8].minor.yy328, SQLITE_IDXTYPE_APPDEF);
}
#line 3561 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 211: /* uniqueflag ::= UNIQUE */
      case 252: /* raisetype ::= ABORT */ yytestcase(yyruleno==252);
#line 1254 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = OE_Abort;}
#line 3567 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 212: /* uniqueflag ::= */
#line 1255 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy328 = OE_None;}
#line 3572 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 215: /* eidlist ::= eidlist COMMA nm collate sortorder */
#line 1305 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-4].minor.yy14 = parserAddExprIdListTerm(pParse, yymsp[-4].minor.yy14, &yymsp[-2].minor.yy0, yymsp[-1].minor.yy328, yymsp[0].minor.yy328);
}
#line 3579 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 216: /* eidlist ::= nm collate sortorder */
#line 1308 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-2].minor.yy14 = parserAddExprIdListTerm(pParse, 0, &yymsp[-2].minor.yy0, yymsp[-1].minor.yy328, yymsp[0].minor.yy328); /*A-overwrites-Y*/
}
#line 3586 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 219: /* cmd ::= DROP INDEX ifexists fullname */
#line 1319 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3DropIndex(pParse, yymsp[0].minor.yy65, yymsp[-1].minor.yy328);}
#line 3591 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 220: /* cmd ::= VACUUM */
#line 1325 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3Vacuum(pParse,0);}
#line 3596 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 221: /* cmd ::= VACUUM nm */
#line 1326 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3Vacuum(pParse,&yymsp[0].minor.yy0);}
#line 3601 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 222: /* cmd ::= PRAGMA nm dbnm */
#line 1333 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3Pragma(pParse,&yymsp[-1].minor.yy0,&yymsp[0].minor.yy0,0,0);}
#line 3606 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 223: /* cmd ::= PRAGMA nm dbnm EQ nmnum */
#line 1334 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3Pragma(pParse,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0,0);}
#line 3611 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 224: /* cmd ::= PRAGMA nm dbnm LP nmnum RP */
#line 1335 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3Pragma(pParse,&yymsp[-4].minor.yy0,&yymsp[-3].minor.yy0,&yymsp[-1].minor.yy0,0);}
#line 3616 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 225: /* cmd ::= PRAGMA nm dbnm EQ minus_num */
#line 1337 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3Pragma(pParse,&yymsp[-3].minor.yy0,&yymsp[-2].minor.yy0,&yymsp[0].minor.yy0,1);}
#line 3621 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 226: /* cmd ::= PRAGMA nm dbnm LP minus_num RP */
#line 1339 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3Pragma(pParse,&yymsp[-4].minor.yy0,&yymsp[-3].minor.yy0,&yymsp[-1].minor.yy0,1);}
#line 3626 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 229: /* cmd ::= createkw trigger_decl BEGIN trigger_cmd_list END */
#line 1355 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  Token all;
  all.z = yymsp[-3].minor.yy0.z;
  all.n = (int)(yymsp[0].minor.yy0.z - yymsp[-3].minor.yy0.z) + yymsp[0].minor.yy0.n;
  sqlite3FinishTrigger(pParse, yymsp[-1].minor.yy473, &all);
}
#line 3636 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 230: /* trigger_decl ::= temp TRIGGER ifnotexists nm dbnm trigger_time trigger_event ON fullname foreach_clause when_clause */
#line 1364 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3BeginTrigger(pParse, &yymsp[-7].minor.yy0, &yymsp[-6].minor.yy0, yymsp[-5].minor.yy328, yymsp[-4].minor.yy378.a, yymsp[-4].minor.yy378.b, yymsp[-2].minor.yy65, yymsp[0].minor.yy132, yymsp[-10].minor.yy328, yymsp[-8].minor.yy328);
  yymsp[-10].minor.yy0 = (yymsp[-6].minor.yy0.n==0?yymsp[-7].minor.yy0:yymsp[-6].minor.yy0); /*A-overwrites-T*/
}
#line 3644 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 231: /* trigger_time ::= BEFORE */
#line 1370 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[0].minor.yy328 = TK_BEFORE; }
#line 3649 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 232: /* trigger_time ::= AFTER */
#line 1371 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[0].minor.yy328 = TK_AFTER;  }
#line 3654 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 233: /* trigger_time ::= INSTEAD OF */
#line 1372 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[-1].minor.yy328 = TK_INSTEAD;}
#line 3659 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 234: /* trigger_time ::= */
#line 1373 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[1].minor.yy328 = TK_BEFORE; }
#line 3664 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 235: /* trigger_event ::= DELETE|INSERT */
      case 236: /* trigger_event ::= UPDATE */ yytestcase(yyruleno==236);
#line 1377 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy378.a = yymsp[0].major; /*A-overwrites-X*/ yymsp[0].minor.yy378.b = 0;}
#line 3670 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 237: /* trigger_event ::= UPDATE OF idlist */
#line 1379 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-2].minor.yy378.a = TK_UPDATE; yymsp[-2].minor.yy378.b = yymsp[0].minor.yy408;}
#line 3675 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 238: /* when_clause ::= */
      case 257: /* key_opt ::= */ yytestcase(yyruleno==257);
#line 1386 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[1].minor.yy132 = 0; }
#line 3681 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 239: /* when_clause ::= WHEN expr */
      case 258: /* key_opt ::= KEY expr */ yytestcase(yyruleno==258);
#line 1387 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[-1].minor.yy132 = yymsp[0].minor.yy346.pExpr; }
#line 3687 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 240: /* trigger_cmd_list ::= trigger_cmd_list trigger_cmd SEMI */
#line 1391 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  assert( yymsp[-2].minor.yy473!=0 );
  yymsp[-2].minor.yy473->pLast->pNext = yymsp[-1].minor.yy473;
  yymsp[-2].minor.yy473->pLast = yymsp[-1].minor.yy473;
}
#line 3696 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 241: /* trigger_cmd_list ::= trigger_cmd SEMI */
#line 1396 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ 
  assert( yymsp[-1].minor.yy473!=0 );
  yymsp[-1].minor.yy473->pLast = yymsp[-1].minor.yy473;
}
#line 3704 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 242: /* trnm ::= nm DOT nm */
#line 1407 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-2].minor.yy0 = yymsp[0].minor.yy0;
  sqlite3ErrorMsg(pParse, 
        "qualified table names are not allowed on INSERT, UPDATE, and DELETE "
        "statements within triggers");
}
#line 3714 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 243: /* tridxby ::= INDEXED BY nm */
#line 1419 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3ErrorMsg(pParse,
        "the INDEXED BY clause is not allowed on UPDATE or DELETE statements "
        "within triggers");
}
#line 3723 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 244: /* tridxby ::= NOT INDEXED */
#line 1424 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3ErrorMsg(pParse,
        "the NOT INDEXED clause is not allowed on UPDATE or DELETE statements "
        "within triggers");
}
#line 3732 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 245: /* trigger_cmd ::= UPDATE orconf trnm tridxby SET setlist where_opt */
#line 1437 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-6].minor.yy473 = sqlite3TriggerUpdateStep(pParse->db, &yymsp[-4].minor.yy0, yymsp[-1].minor.yy14, yymsp[0].minor.yy132, yymsp[-5].minor.yy328);}
#line 3737 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 246: /* trigger_cmd ::= insert_cmd INTO trnm idlist_opt select */
#line 1441 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-4].minor.yy473 = sqlite3TriggerInsertStep(pParse->db, &yymsp[-2].minor.yy0, yymsp[-1].minor.yy408, yymsp[0].minor.yy3, yymsp[-4].minor.yy328);/*A-overwrites-R*/}
#line 3742 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 247: /* trigger_cmd ::= DELETE FROM trnm tridxby where_opt */
#line 1445 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[-4].minor.yy473 = sqlite3TriggerDeleteStep(pParse->db, &yymsp[-2].minor.yy0, yymsp[0].minor.yy132);}
#line 3747 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 248: /* trigger_cmd ::= select */
#line 1449 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy473 = sqlite3TriggerSelectStep(pParse->db, yymsp[0].minor.yy3); /*A-overwrites-X*/}
#line 3752 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 249: /* expr ::= RAISE LP IGNORE RP */
#line 1452 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  spanSet(&yymsp[-3].minor.yy346,&yymsp[-3].minor.yy0,&yymsp[0].minor.yy0);  /*A-overwrites-X*/
  yymsp[-3].minor.yy346.pExpr = sqlite3PExpr(pParse, TK_RAISE, 0, 0, 0); 
  if( yymsp[-3].minor.yy346.pExpr ){
    yymsp[-3].minor.yy346.pExpr->affinity = OE_Ignore;
  }
}
#line 3763 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 250: /* expr ::= RAISE LP raisetype COMMA nm RP */
#line 1459 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  spanSet(&yymsp[-5].minor.yy346,&yymsp[-5].minor.yy0,&yymsp[0].minor.yy0);  /*A-overwrites-X*/
  yymsp[-5].minor.yy346.pExpr = sqlite3ExprAlloc(pParse->db, TK_RAISE, &yymsp[-1].minor.yy0, 1); 
  if( yymsp[-5].minor.yy346.pExpr ) {
    yymsp[-5].minor.yy346.pExpr->affinity = (char)yymsp[-3].minor.yy328;
  }
}
#line 3774 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 251: /* raisetype ::= ROLLBACK */
#line 1469 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = OE_Rollback;}
#line 3779 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 253: /* raisetype ::= FAIL */
#line 1471 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[0].minor.yy328 = OE_Fail;}
#line 3784 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 254: /* cmd ::= DROP TRIGGER ifexists fullname */
#line 1476 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3DropTrigger(pParse,yymsp[0].minor.yy65,yymsp[-1].minor.yy328);
}
#line 3791 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 255: /* cmd ::= ATTACH database_kw_opt expr AS expr key_opt */
#line 1483 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3Attach(pParse, yymsp[-3].minor.yy346.pExpr, yymsp[-1].minor.yy346.pExpr, yymsp[0].minor.yy132);
}
#line 3798 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 256: /* cmd ::= DETACH database_kw_opt expr */
#line 1486 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3Detach(pParse, yymsp[0].minor.yy346.pExpr);
}
#line 3805 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 259: /* cmd ::= REINDEX */
#line 1501 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3Reindex(pParse, 0, 0);}
#line 3810 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 260: /* cmd ::= REINDEX nm dbnm */
#line 1502 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3Reindex(pParse, &yymsp[-1].minor.yy0, &yymsp[0].minor.yy0);}
#line 3815 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 261: /* cmd ::= ANALYZE */
#line 1507 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3Analyze(pParse, 0, 0);}
#line 3820 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 262: /* cmd ::= ANALYZE nm dbnm */
#line 1508 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3Analyze(pParse, &yymsp[-1].minor.yy0, &yymsp[0].minor.yy0);}
#line 3825 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 263: /* cmd ::= ALTER TABLE fullname RENAME TO nm */
#line 1513 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  sqlite3AlterRenameTable(pParse,yymsp[-3].minor.yy65,&yymsp[0].minor.yy0);
}
#line 3832 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 264: /* cmd ::= ALTER TABLE add_column_fullname ADD kwcolumn_opt columnname carglist */
#line 1517 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-1].minor.yy0.n = (int)(pParse->sLastToken.z-yymsp[-1].minor.yy0.z) + pParse->sLastToken.n;
  sqlite3AlterFinishAddColumn(pParse, &yymsp[-1].minor.yy0);
}
#line 3840 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 265: /* add_column_fullname ::= fullname */
#line 1521 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  disableLookaside(pParse);
  sqlite3AlterBeginAddColumn(pParse, yymsp[0].minor.yy65);
}
#line 3848 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 266: /* cmd ::= create_vtab */
#line 1531 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3VtabFinishParse(pParse,0);}
#line 3853 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 267: /* cmd ::= create_vtab LP vtabarglist RP */
#line 1532 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3VtabFinishParse(pParse,&yymsp[0].minor.yy0);}
#line 3858 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 268: /* create_vtab ::= createkw VIRTUAL TABLE ifnotexists nm dbnm USING nm */
#line 1534 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
    sqlite3VtabBeginParse(pParse, &yymsp[-3].minor.yy0, &yymsp[-2].minor.yy0, &yymsp[0].minor.yy0, yymsp[-4].minor.yy328);
}
#line 3865 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 269: /* vtabarg ::= */
#line 1539 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3VtabArgInit(pParse);}
#line 3870 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 270: /* vtabargtoken ::= ANY */
      case 271: /* vtabargtoken ::= lp anylist RP */ yytestcase(yyruleno==271);
      case 272: /* lp ::= LP */ yytestcase(yyruleno==272);
#line 1541 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{sqlite3VtabArgExtend(pParse,&yymsp[0].minor.yy0);}
#line 3877 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 273: /* with ::= */
#line 1556 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{yymsp[1].minor.yy59 = 0;}
#line 3882 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 274: /* with ::= WITH wqlist */
#line 1558 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[-1].minor.yy59 = yymsp[0].minor.yy59; }
#line 3887 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 275: /* with ::= WITH RECURSIVE wqlist */
#line 1559 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{ yymsp[-2].minor.yy59 = yymsp[0].minor.yy59; }
#line 3892 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 276: /* wqlist ::= nm eidlist_opt AS LP select RP */
#line 1561 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-5].minor.yy59 = sqlite3WithAdd(pParse, 0, &yymsp[-5].minor.yy0, yymsp[-4].minor.yy14, yymsp[-1].minor.yy3); /*A-overwrites-X*/
}
#line 3899 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      case 277: /* wqlist ::= wqlist COMMA nm eidlist_opt AS LP select RP */
#line 1564 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"
{
  yymsp[-7].minor.yy59 = sqlite3WithAdd(pParse, yymsp[-7].minor.yy59, &yymsp[-5].minor.yy0, yymsp[-4].minor.yy14, yymsp[-1].minor.yy3);
}
#line 3906 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
        break;
      default:
      /* (278) input ::= cmdlist */ yytestcase(yyruleno==278);
      /* (279) cmdlist ::= cmdlist ecmd */ yytestcase(yyruleno==279);
      /* (280) cmdlist ::= ecmd (OPTIMIZED OUT) */ assert(yyruleno!=280);
      /* (281) ecmd ::= SEMI */ yytestcase(yyruleno==281);
      /* (282) ecmd ::= explain cmdx SEMI */ yytestcase(yyruleno==282);
      /* (283) explain ::= */ yytestcase(yyruleno==283);
      /* (284) trans_opt ::= */ yytestcase(yyruleno==284);
      /* (285) trans_opt ::= TRANSACTION */ yytestcase(yyruleno==285);
      /* (286) trans_opt ::= TRANSACTION nm */ yytestcase(yyruleno==286);
      /* (287) savepoint_opt ::= SAVEPOINT */ yytestcase(yyruleno==287);
      /* (288) savepoint_opt ::= */ yytestcase(yyruleno==288);
      /* (289) cmd ::= create_table create_table_args */ yytestcase(yyruleno==289);
      /* (290) columnlist ::= columnlist COMMA columnname carglist */ yytestcase(yyruleno==290);
      /* (291) columnlist ::= columnname carglist */ yytestcase(yyruleno==291);
      /* (292) nm ::= ID|INDEXED */ yytestcase(yyruleno==292);
      /* (293) nm ::= STRING */ yytestcase(yyruleno==293);
      /* (294) nm ::= JOIN_KW */ yytestcase(yyruleno==294);
      /* (295) typetoken ::= typename */ yytestcase(yyruleno==295);
      /* (296) typename ::= ID|STRING */ yytestcase(yyruleno==296);
      /* (297) signed ::= plus_num (OPTIMIZED OUT) */ assert(yyruleno!=297);
      /* (298) signed ::= minus_num (OPTIMIZED OUT) */ assert(yyruleno!=298);
      /* (299) carglist ::= carglist ccons */ yytestcase(yyruleno==299);
      /* (300) carglist ::= */ yytestcase(yyruleno==300);
      /* (301) ccons ::= NULL onconf */ yytestcase(yyruleno==301);
      /* (302) conslist_opt ::= COMMA conslist */ yytestcase(yyruleno==302);
      /* (303) conslist ::= conslist tconscomma tcons */ yytestcase(yyruleno==303);
      /* (304) conslist ::= tcons (OPTIMIZED OUT) */ assert(yyruleno!=304);
      /* (305) tconscomma ::= */ yytestcase(yyruleno==305);
      /* (306) defer_subclause_opt ::= defer_subclause (OPTIMIZED OUT) */ assert(yyruleno!=306);
      /* (307) resolvetype ::= raisetype (OPTIMIZED OUT) */ assert(yyruleno!=307);
      /* (308) selectnowith ::= oneselect (OPTIMIZED OUT) */ assert(yyruleno!=308);
      /* (309) oneselect ::= values */ yytestcase(yyruleno==309);
      /* (310) sclp ::= selcollist COMMA */ yytestcase(yyruleno==310);
      /* (311) as ::= ID|STRING */ yytestcase(yyruleno==311);
      /* (312) expr ::= term (OPTIMIZED OUT) */ assert(yyruleno!=312);
      /* (313) exprlist ::= nexprlist */ yytestcase(yyruleno==313);
      /* (314) nmnum ::= plus_num (OPTIMIZED OUT) */ assert(yyruleno!=314);
      /* (315) nmnum ::= nm (OPTIMIZED OUT) */ assert(yyruleno!=315);
      /* (316) nmnum ::= ON */ yytestcase(yyruleno==316);
      /* (317) nmnum ::= DELETE */ yytestcase(yyruleno==317);
      /* (318) nmnum ::= DEFAULT */ yytestcase(yyruleno==318);
      /* (319) plus_num ::= INTEGER|FLOAT */ yytestcase(yyruleno==319);
      /* (320) foreach_clause ::= */ yytestcase(yyruleno==320);
      /* (321) foreach_clause ::= FOR EACH ROW */ yytestcase(yyruleno==321);
      /* (322) trnm ::= nm */ yytestcase(yyruleno==322);
      /* (323) tridxby ::= */ yytestcase(yyruleno==323);
      /* (324) database_kw_opt ::= DATABASE */ yytestcase(yyruleno==324);
      /* (325) database_kw_opt ::= */ yytestcase(yyruleno==325);
      /* (326) kwcolumn_opt ::= */ yytestcase(yyruleno==326);
      /* (327) kwcolumn_opt ::= COLUMNKW */ yytestcase(yyruleno==327);
      /* (328) vtabarglist ::= vtabarg */ yytestcase(yyruleno==328);
      /* (329) vtabarglist ::= vtabarglist COMMA vtabarg */ yytestcase(yyruleno==329);
      /* (330) vtabarg ::= vtabarg vtabargtoken */ yytestcase(yyruleno==330);
      /* (331) anylist ::= */ yytestcase(yyruleno==331);
      /* (332) anylist ::= anylist LP anylist RP */ yytestcase(yyruleno==332);
      /* (333) anylist ::= anylist ANY */ yytestcase(yyruleno==333);
        break;
/********** End reduce actions ************************************************/
  };
  assert( yyruleno<sizeof(yyRuleInfo)/sizeof(yyRuleInfo[0]) );
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact <= YY_MAX_SHIFTREDUCE ){
    if( yyact>YY_MAX_SHIFT ){
      yyact += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
    }
    yymsp -= yysize-1;
    yypParser->yytos = yymsp;
    yymsp->stateno = (YYACTIONTYPE)yyact;
    yymsp->major = (YYCODETYPE)yygoto;
    yyTraceShift(yypParser, yyact);
  }else{
    assert( yyact == YY_ACCEPT_ACTION );
    yypParser->yytos -= yysize;
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  sqlite3ParserARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
/************ End %parse_failure code *****************************************/
  sqlite3ParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  sqlite3ParserTOKENTYPE yyminor         /* The minor type of the error token */
){
  sqlite3ParserARG_FETCH;
#define TOKEN yyminor
/************ Begin %syntax_error code ****************************************/
#line 32 "/Users/walterstevenbabcock/Desktop/lemon/parse.y"

  UNUSED_PARAMETER(yymajor);  /* Silence some compiler warnings */
  assert( TOKEN.z[0] );  /* The tokenizer always gives us a token */
  sqlite3ErrorMsg(pParse, "near \"%T\": syntax error", &TOKEN);
#line 4026 "/Users/walterstevenbabcock/Desktop/lemon/parse.c"
/************ End %syntax_error code ******************************************/
  sqlite3ParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  sqlite3ParserARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
#ifndef YYNOERRORRECOVERY
  yypParser->yyerrcnt = -1;
#endif
  assert( yypParser->yytos==yypParser->yystack );
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
/*********** End %parse_accept code *******************************************/
  sqlite3ParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "sqlite3ParserAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void sqlite3Parser(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  sqlite3ParserTOKENTYPE yyminor       /* The value for the token */
  sqlite3ParserARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  unsigned int yyact;   /* The parser action. */
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  int yyendofinput;     /* True if we are at the end of input */
#endif
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  yypParser = (yyParser*)yyp;
  assert( yypParser->yytos!=0 );
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  yyendofinput = (yymajor==0);
#endif
  sqlite3ParserARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput '%s'\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact <= YY_MAX_SHIFTREDUCE ){
      yy_shift(yypParser,yyact,yymajor,yyminor);
#ifndef YYNOERRORRECOVERY
      yypParser->yyerrcnt--;
#endif
      yymajor = YYNOCODE;
    }else if( yyact <= YY_MAX_REDUCE ){
      yy_reduce(yypParser,yyact-YY_MIN_REDUCE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
      yyminorunion.yy0 = yyminor;
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminor);
      }
      yymx = yypParser->yytos->major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
        yymajor = YYNOCODE;
      }else{
        while( yypParser->yytos >= yypParser->yystack
            && yymx != YYERRORSYMBOL
            && (yyact = yy_find_reduce_action(
                        yypParser->yytos->stateno,
                        YYERRORSYMBOL)) >= YY_MIN_REDUCE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yytos < yypParser->yystack || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
          yypParser->yyerrcnt = -1;
#endif
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          yy_shift(yypParser,yyact,YYERRORSYMBOL,yyminor);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor, yyminor);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor, yyminor);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
        yypParser->yyerrcnt = -1;
#endif
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yytos>yypParser->yystack );
#ifndef NDEBUG
  if( yyTraceFILE ){
    yyStackEntry *i;
    char cDiv = '[';
    fprintf(yyTraceFILE,"%sReturn. Stack=",yyTracePrompt);
    for(i=&yypParser->yystack[1]; i<=yypParser->yytos; i++){
      fprintf(yyTraceFILE,"%c%s", cDiv, yyTokenName[i->major]);
      cDiv = ' ';
    }
    fprintf(yyTraceFILE,"]\n");
  }
#endif
  return;
}
