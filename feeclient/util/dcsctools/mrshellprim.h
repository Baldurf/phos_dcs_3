// $Id: mrshellprim.h,v 1.14 2006/02/13 11:10:24 richter Exp $

/************************************************************************
**
**
** This file is property of and copyright by the Experimental Nuclear 
** Physics Group, Dep. of Physics and Technology
** University of Bergen, Norway, 2004
** This file has been written by Matthias Richter,
** Matthias.Richter@ift.uib.no
**
** Permission to use, copy, modify and distribute this software and its  
** documentation strictly for non-commercial purposes is hereby granted  
** without fee, provided that the above copyright notice appears in all  
** copies and that both the copyright notice and this permission notice  
** appear in the supporting documentation. The authors make no claims    
** about the suitability of this software for any purpose. It is         
** provided "as is" without express or implied warranty.                 
**
*************************************************************************/
//har ikke funksjon i prosjektet - ole - 23.04.13


#ifndef _MRSHELLPRIM_H
#define _MRSHELLPRIM_H

#include <stdio.h>

#define DBG_SHELLPRIM_MASK          0xff0000
#define DBG_SEARCH_ARG_DEF          0x010000 // debug output of the SearchDef function
#define DBG_SEARCH_ARG_DEF_DETAIL   0x030000 // detailed debug output of the SearchDef function
#define DBG_SCAN_ARG_DEF            0x040000 // debug output of the ScanArguments function
#define DBG_SCAN_ARG_DEF_DETAIL     0x0c0000 // detailed debug output of the ScanArguments function
#define DBG_ARGUMENT_READ           0x100000 // debug output of the ReadArgument function
#define DBG_ARGUMENT_CONVERT        0x800000 // print debug information concerning argument conversion

#define PRIVATE_ERRORS          0x1000
#define EINTERNAL              PRIVATE_ERRORS+1
#define EOPERATION             PRIVATE_ERRORS+2

#ifdef __cplusplus
extern "C" { 
#endif

char* removePrecAndTrailingSpecChars(char* pCmd);

char* extractFileNameFromArg(char* pFileName);

int getHexNumberFromArg(const char* arg, unsigned int* pNumber, int bWarning);

int getDecNumberFromArg(const char* arg, int* pNumber, int bWarning);

int getFloatNumberFromArg(const char* arg, float* pNumber, int bWarning);

/* scan a buffer of characters, separate commands and extract pointers to single command parameters
parameter:
  pCmdLine - pointer to zero terminated input char buffer, the content is changed if bSeparate==1!!!
  pTargetArray - array to receive pointers to separated strings
  iArraySize - size of the target array
  bSeparate - blanks and special characters set to zero to separate to command parameters
result:
  number of parameters if succeeded, pointers to separated strings in taget array
  error code <0 if failed
*/
int buildArgumentsFromCommandLine(char* pCmdLine, char** pTargetArray, int iArraySize, int bSeparate, int iDebugFlag);

enum {
  eUnknownType = 0,
  eInteger,         // expect decimal value as next argument and read value into variable
                    // TaggedData_t.Int

  eIntegerArray,    // some integers, size of array specified in user.size
                    // TaggedData_t.pInt & ArgDef_t.size

  eFloat,           // expect float value as next argument and read value into variable
                    // TaggedData_t.Float

  eFloatArray,      // some float, size of array specified in user.size
                    // TaggedData_t.pFloat & ArgDef_t.size

  eHex,             // expect hexadecimal value as next argument and read value into variable
                    // TaggedData_t.Hex

  eHexArray,        // some hexadecimal arguments, size of array specified in user.size
                    // TaggedData_t.pHex & ArgDef_t.size

  eConstString,     // expect const char string as next argument, only applicable if all arguments zero seperated
                    // the pString pointer of the TaggedData_t structure is set to the string
                    // TaggedData_t.pString

  eCharArray,       // character array to receive the argument, size of array specified in user.size
                    // the string is copied into the provided buffer
                    // TaggedData_t.arrayChar & ArgDef_t.size

  eComposite,       // array of different data types, zero terminated
  eFctNoArg,        // call an argument-less function
  eFctIndex,        // call a function with an integer argument
                    // the user data is treated as integer and passed to the function.

  eFctRemaining,    // recurse scan function with remaining arguments
  eFctInclusive,    // recurse scan function with arguments including the current
  eFctUserScan,     // a user defined scan function, the current argument and the remaining arguments are passed
  eFctIntegerArgs,  // a function with an array of integers
  eFctFloatArgs,    // a function with an array of floats
  eFctHexArgs,      // a function with an array of hexadecimal values
  eFctCompositeArgs,// a function with an array of TTagged data members, terminated by an eUnknownType element
  eFctArgDef,       // a recursive scan is done with the argument definition structure and passed to the function 
  eBool,            // the Int member of the tagged data is set if the argument was found
  eFlag,            // the pInt member of the tagged data is set with the pattern setflag if the argument was found
};

typedef struct ArgDef_t  TArgDef;
typedef struct FctMode_t TFctMode;
typedef struct FctArg_t TFctArg;
typedef struct TaggedData_t TTaggedData;

typedef int (*FunctionNoArg)();
typedef int (*FctIndex)(int majorNo);
typedef int (*FctRecurseArg)(const char** arrayArg, int iNofArgs, int iFirstArgStart, TArgDef** arrayDef, TFctMode *pMode);
typedef int (*FctUserScan)(const char* currentArg, const char** arrayArg, int iNofArgs, void* pUser, FILE* pOut);
typedef int (*FctInteger)(int* array, int iNofArgs, FILE* pOut);
typedef int (*FctFloat)(float* array, int iNofArgs, FILE* pOut);
typedef int (*FctHex)(unsigned long* array, int iNofArgs, FILE* pOut);
typedef int (*FctComposite)(TTaggedData* array, int iNofArgs, FILE* pOut);
typedef int (*FctArgDef)(TArgDef* pDef, void* pUser, FILE* pOut);

// flags for scan function
#define SCANMODE_STRICT_SEQUENCE   0x0001 // require the sequence of all mandatory arguments
#define SCANMODE_FORCE_TERMINATION 0x0002 // terminate at the first not-recognized argument 
#define SCANMODE_PRINT_UKWN_SEQU   0x0004 // write an unknown sequence to the output 
#define SCANMODE_SILENT            0x0008 // dont report an error in case of unknown argument
#define SCANMODE_SKIP_UKWN_SEQU    0x0010 // skip an unknown sequence and continue
#define SCANMODE_READ_ONE_CMD      0x0020 // read only one command with all the arguments and terminate
#define SCANMODE_PERSISTENT        0x0040 // do not clean volatile flags at the beginning of the argument scan

// bitshifts and masks for return value of scan function
#define SCANRET_BITSHIFT_PROCESSED_ARGS  8
#define SCANRET_BITSHIFT_OFFSET_LAST_ARG 0
#define SCANRET_BITSHIFT_INDEX           16
#define SCANRET_MASK_PROCESSED_ARGS      0x0000ff00
#define SCANRET_MASK_OFFSET_LAST_ARG     0x000000ff
#define SCANRET_MASK_INDEX               0x00ff0000
#define SCANRET_INVAL_INDEX              0x000000ff

#define SCANRET_GET_PROCESSED_ARGS ((x&SCANRET_MASK_PROCESSED_ARGS)>>SCANRET_BITSHIFT_PROCESSED_ARGS)
#define SCANRET_GET_OFFSET_LAST_ARG ((x&SCANRET_MASK_OFFSET_LAST_ARG)>>SCANRET_BITSHIFT_OFFSET_LAST_ARG)

// flags for argument definition
#define ARGDEF_MANDATORY      0x0001 // mark the argument as mandatory
#define ARGDEF_OPTIONAL       0x0002 // mark the argument as optional
#define ARGDEF_ONLY_ONCE      0x0004 // the argument will be ignored for further check if read once
#define ARGDEF_SKIP_IF_FAILED 0x0008 // skip the argument if the required additional argument scan failed
#define ARGDEF_RESUME         0x0010 // if additional scans fail resume and try to find another entry in the definition array
#define ARGDEF_UNTERM_SHORT   0x0020 // the short argument does not need to be terminated by a separator
#define ARGDEF_UNTERM_LONG    0x0040 // the long argument does not need to be terminated by a separator
#define ARGDEF_TERMINATE      0x0080 // terminate the argument scan after this argument was processed
#define ARGDEF_EXIT           0x0100 // terminate without processing with -EINTR if this argument was found
#define ARGDEF_BREAK          0x0200 // terminate without processing if this argument was found
#define ARGDEF_DELAY_EXECUTE  0x0400 // dont call subfunctions execute
#define ARGDEF_KEYWORDLESS    0x0800 // this is a keywordless argument, start scanning the parameters from the argument itself
// flags used internally
#define ARGPROC_FOUND         0x1000 // the argument was found
#define ARGPROC_FAILED        0x2000 // the argument was found, but additional argument scan failed

#define ARGPROC_INDEX_BITSHIFT   0
#define ARGPROC_INDEX_WIDTH     16
#define ARGPROC_EXISTS_BITSHIFT 16
#define ARGPROC_EXISTS_WIDTH     1

#define ARGPROC_EXISTS(x) ((x>>ARGPROC_EXISTS_BITSHIFT)&((0x1<<ARGPROC_EXISTS_WIDTH)-1))
#define ARGPROC_INDEX(x) ((x>>ARGPROC_INDEX_BITSHIFT)&((0x1<<ARGPROC_INDEX_WIDTH)-1))

struct TaggedData_t {
  int   type;     // type of the data pointer
  union  {    // type dependend data structure
    void*         pVoid;
    int           Int;
    int*          pInt;
    float         Float;
    float*        pFloat;
    unsigned int  Hex;
    unsigned int* pHex;
    const char*   pString;
    char*         arrayChar;
    TArgDef*      pSubArgDef;
    FunctionNoArg pFctNoArg;
    FctIndex      pFctIndex;
    FctUserScan   pFctUser;
    FctInteger    pFctInteger;
    FctFloat      pFctFloat;
    FctHex        pFctHex;
    FctComposite  pFctComposite;
    FctArgDef     pFctArgDef;
  };
};

struct ArgDef_t {
  const char* s;    // short version of the argument
  const char* l;    // long version of the argument
  TTaggedData data; // type dependend data structure
  union {    // additional type dependend data
    void*         pUser;    // pointer to user defined data passed to a sub-function
    TFctMode*     pFctMode; // mode for the next recursion
    TTaggedData*  arrayComposite;
    TFctArg*      pFctArg;  // parameter to the eFctArgDef type
    int           size;    // size of arrays
    unsigned int  setFlagPattern; // the flag pattern for the eFlag type
  };
  unsigned int    flags;
};

struct FctMode_t {
  unsigned int flags;
  const char* pSeparators;
  FILE* pOutput;
};

struct FctArg_t {
  TArgDef*  pDef;
  TFctMode* pMode;
  void*     pUser;
};

/* scan a list or single string of arguments
   with respect to the definitions of arguments as entries in the definition array, sub-functions can be called and float,
   decimal and hexadecimal values can be read
   parameters:
     arrayArg      : array of argument strings
     iNofArgs      : size of the array
     iFirstArgStart: start of scan within the first element of the array
     pSeparator    : a zero terminated string with additional separators, the \0 value is always treated as separator
     arrayDef      : an array of TArgDef elements, each defining a known argument type
     flags         : 
   result: >=0 successfull, offset coded
     bit 8-15: number of scanned elements of argArray
     bit  0-7: offset in last processed element
     -EINTR if an argument with ARGDEF_TERMINATE or ARGDEF_BREAK was found
 */
int ScanArguments(const char** arrayArg, int iNofArgs, int iFirstArgStart, TArgDef* arrayDef, TFctMode *pMode);

int PrintArgumentDefinition(TArgDef* arrayDef, int bAll);

unsigned int mrShellPrimSetDebugFlag(unsigned int flag);
unsigned int mrShellPrimClearDebugFlag(unsigned int flag);
int mrShellPrimPrintDbgFlags();

/* search in the argument definition for a command
 * parameters:
 *   arrayDef      : argument definition array, terminated by an element of type eUnknownType
 *   pCmd          : command string, both the short and the long definition are compared
 *   iType         : if not equal to eUnknownType a type check is done
 * result:
 *   >=0           : success, index of the element
 *   -ENOENT       : entry not found
 *   -EBADF        : entry found, but of wrong type
 *   -EINVAL       : invalid argument
 */
int mrShellPrimGetIndex(TArgDef* arrayDef, const char* pCmd, int iType);

/* set data pointer for an argument definition element
 * the function is only available for other arguments than of type eInteger, eBool, iFloat and iHex
 * parameters:
 *   arrayDef      : argument definition array, terminated by an element of type eUnknownType
 *   pCmd          : command string, both the short and the long definition are compared
 *   pData         : void pointer to data
 *   iType         : if not equal to eUnknownType a type check is done
 * result:
 *   >=0           : success, index of the element in lower 16bit
 *   -ENOENT       : entry not found
 *   -EBADF        : entry found, but of wrong type
 *   -EINVAL       : invalid argument
 *   -ENOSYS       : function not available for this type
 */
int mrShellPrimSetData(TArgDef* arrayDef, const char* pCmd, void* pData, int iType);

/* get data pointer for an argument definition element
 * the function is only available for other arguments than of type eInteger, eBool, iFloat and iHex
 * parameters:
 *   arrayDef      : argument definition array, terminated by an element of type eUnknownType
 *   pCmd          : command string, both the short and the long definition are compared
 *   ppData        : pointer to void pointer to receive data pointer
 *   iType         : if not equal to eUnknownType a type check is done
 * result:
 *   >=0           : success, index of the element in lower 16bit; bit 16 set if this argument was found
 *                   use the ARGPROC_INDEX and ARGPROC_EXISTS macros to extract to parts
 *   -ENOENT       : entry not found
 *   -EBADF        : entry found, but of wrong type
 *   -EINVAL       : invalid argument
 *   -ENOSYS       : function not available for this type
 */
int mrShellPrimGetData(TArgDef* arrayDef, const char* pCmd, void** ppData, int iType);

/* get float value for an argument definition element
 * the function is only available arguments of type iFloat
 * parameters:
 *   arrayDef      : argument definition array, terminated by an element of type eUnknownType
 *   pCmd          : command string, both the short and the long definition are compared
 *   pFloat        : data target
 * result:
 *   >=0           : success, index of the element in lower 16bit; bit 16 set if this argument was found
 *                   use the ARGPROC_INDEX and ARGPROC_EXISTS macros to extract to parts
 *   -ENOENT       : entry not found
 *   -EBADF        : entry found, but of wrong type
 *   -EINVAL       : invalid argument
 *   -ENOSYS       : function not available for this type
 */
int mrShellPrimGetFloat(TArgDef* arrayDef, const char* pCmd, float* pFloat);

/* get integer value for an argument definition element
 * the function is only available arguments of type iInteger and eBool
 * parameters:
 *   arrayDef      : argument definition array, terminated by an element of type eUnknownType
 *   pCmd          : command string, both the short and the long definition are compared
 *   pInt          : data target
 * result:
 *   >=0           : success, index of the element in lower 16bit; bit 16 set if this argument was found
 *                   use the ARGPROC_INDEX and ARGPROC_EXISTS macros to extract to parts
 *   -ENOENT       : entry not found
 *   -EBADF        : entry found, but of wrong type
 *   -EINVAL       : invalid argument
 *   -ENOSYS       : function not available for this type
 */
int mrShellPrimGetInt(TArgDef* arrayDef, const char* pCmd, int* pInt);

/* get hexadecimal value for an argument definition element
 * the function is only available arguments of type iHex
 * parameters:
 *   arrayDef      : argument definition array, terminated by an element of type eUnknownType
 *   pCmd          : command string, both the short and the long definition are compared
 *   pHex          : data target
 * result:
 *   >=0           : success, index of the element in lower 16bit; bit 16 set if this argument was found
 *                   use the ARGPROC_INDEX and ARGPROC_EXISTS macros to extract to parts
 *   -ENOENT       : entry not found
 *   -EBADF        : entry found, but of wrong type
 *   -EINVAL       : invalid argument
 *   -ENOSYS       : function not available for this type
 */
int mrShellPrimGetHex(TArgDef* arrayDef, const char* pCmd, unsigned int* pHex);

/* clone an argument definition
 * the allocated memory has to be released by the caller
 */
TArgDef* mrShellPrimCloneDef(TArgDef* arrayDef);

/* reset volatile processing flags of the definition array
 *  reseted flags: ARGPROC_FOUND
 */
int mrShellPrimResetVolatileFlags(TArgDef* arrayDef);

#ifdef __cplusplus
}
#endif

#endif // _MRSHELLPRIM_H
