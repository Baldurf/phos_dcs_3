// $Id: mrshellprim.c,v 1.18 2006/04/19 15:05:22 richter Exp $

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

#include <stdio.h>
#include <string.h>
#include <errno.h>
// the fix came in when merging branch chc into the main trunk on April 19 2006
// memory guard should be enabled during configure in the future 
#ifdef HAVE_MEMORYGUARD
#include "memoryguard.h"
#endif //HAVE_MEMORYGUARD

#include "mrshellprim.h"

unsigned int g_mrShellPrimDbg = 0x0;

char* removePrecAndTrailingSpecChars(char* pCmd)
{
  char* pResult=NULL;
  if (pCmd) {
    int i=0;
    int iStart=-1;
    for (i=0; pCmd[i]!=0; i++) {
      if (pCmd[i]!=' ' && iStart<0) iStart=i;
      if (pCmd[i]=='\n') {
	pCmd[i]=0;
	break;
      }
    }
    if (iStart>=0 && i-iStart>0)
      strcpy(pCmd, &pCmd[iStart]);
    else 
      *pCmd=0;
    pResult=pCmd;
  }
  return pResult;
}

char* extractFileNameFromArg(char* pFileName)
{
  char* pResult=NULL;
  if (pFileName) {
    int iLen=strlen(pFileName);
    int i=0;
    int iFileNameStart=0;
    for (i=0; i<iLen; i++) {
      if (pFileName[i]=='\'' || pFileName[i]=='\"' || pFileName[i]==' ') {
	if (iFileNameStart>0) {
	  pFileName[i]=0;
	  break;
	}
      } else if (iFileNameStart==0) {
	iFileNameStart=i;
      }
    }
    fprintf(stderr, "extractFileNameFromArg input: %s ", pFileName);
    if (iFileNameStart>0) {
      memcpy(pFileName, &pFileName[iFileNameStart], i-iFileNameStart);
    }
    fprintf(stderr, "output %s\n", pFileName); 
  } else {
    fprintf(stderr, "extractFileNameFromArg internal error: invalid argument\n");
  }
  return pResult;
}

int getHexNumberFromArg(const char* arg, unsigned int* pNumber, int bWarning) {
  int iResult=0;
  if (arg && pNumber && strlen(arg)>2) {
    unsigned int number=0;
    if (*arg=='0' && *(arg+1)=='x' && sscanf(arg, " 0x%x", &number)==1) {
      *pNumber = number;
    } else {
      if (bWarning)
	fprintf(stderr, "wrong argument format (%s), expect hex value preceeded by 0x\n", arg);
      iResult=-EINVAL;
    } 
  } else {
    iResult=-EINVAL;
  } 
  return iResult;
}

int getDecNumberFromArg(const char* arg, int* pNumber, int bWarning) {
  int iResult=0;
  if (arg && pNumber && strlen(arg)>0) {
    int number=0;
    if (sscanf(arg, " %d", &number)==1) {
      *pNumber = number;
    } else {
      if (bWarning)
	fprintf(stderr, "wrong argument format (%s), expect decimal value\n", arg);
      iResult=-EINVAL;
    } 
  } else {
    iResult=-EINVAL;
  } 
  return iResult;
}

int getFloatNumberFromArg(const char* arg, float* pNumber, int bWarning) {
  int iResult=0;
  if (arg && pNumber && strlen(arg)>0) {
    float number=0;
    if (sscanf(arg, " %f", &number)==1) {
      *pNumber = number;
    } else {
      if (bWarning)
	fprintf(stderr, "wrong argument format (%s), expect float value\n", arg);
      iResult=-EINVAL;
    } 
  } else {
    iResult=-EINVAL;
  } 
  return iResult;
}

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
int buildArgumentsFromCommandLine(char* pCmdLine, char** pTargetArray, int iArraySize, int bSeparate, int iDebugFlag)
{
  int iResult=0;
  if (pCmdLine) {
    int iLen=strlen(pCmdLine);
    if (pCmdLine) {
      char* pLastArg=NULL;
      int bQuotation=-1;
      if (pTargetArray) {
	memset(pTargetArray, 0, iArraySize*sizeof(char*));
      }
      int iNofArgs=0;
      iResult=0;
      int iPos=0;
      // loop over all characters in the command line until a '#' as beginning of comment is found
      for (iPos=0; iPos<iLen+1 && (bQuotation>=0 || pCmdLine[iPos]!='#') ; iPos++) {
	// commands separated by blanks or other special chars
	if (bQuotation<0 && (pCmdLine[iPos]==' ' || pCmdLine[iPos]<16)){
	  if (bSeparate>0) {
	    pCmdLine[iPos]=0; // delete the blank and cut the string
	  }
	  if (pLastArg!=NULL) {
	    // this is the end of a command parameter, store it in the array
	    if (iNofArgs<iArraySize && pTargetArray!=NULL) {
	      pTargetArray[iNofArgs]=pLastArg;
	    }
	    iNofArgs++;
	    pLastArg=NULL;
	  }
	} else if (pLastArg==NULL && (pCmdLine[iPos]=='\'' || pCmdLine[iPos]=='\"')) {
	  if (bSeparate>0) {
	    pCmdLine[iPos]=0;
	  }
	  if (bQuotation<0) {
	    bQuotation=iPos;
	  } else {
	    if (iPos-bQuotation>1) {
	      if (iNofArgs<iArraySize && pTargetArray!=NULL) {
		pTargetArray[iNofArgs]=&pCmdLine[bQuotation+1];
	      }
	      iNofArgs++;
	    }
	    bQuotation=-1;
	  }
	} else if (bQuotation<0){
	  // is this a new command parameter?
	  if (pLastArg==NULL) { 
	    // store position of the command parameter
	    pLastArg=&(pCmdLine[iPos]);
	  }
	} else if (pLastArg!=NULL && bQuotation>=0) {
	  // you should never end up here
	  fprintf(stderr, "internal error while building the arguments\n");
	  pLastArg=NULL;
	}
      }
      if (bQuotation>=0)
	fprintf(stderr, "missing terminating quotation mark, the last argument might be missing\n");
      if (iDebugFlag&DBG_ARGUMENT_CONVERT) {
	if (iNofArgs<=iArraySize)
	  printf("found %d argument(s)\n", iNofArgs);
	else 
	  printf("found %d argument(s), %d stored in array\n", iNofArgs, iArraySize);
	if (pTargetArray!=NULL) {
	  for (iPos=0; iPos<iNofArgs && iPos<iArraySize; iPos++) {
	    printf("arg %d: %s\n", iPos, pTargetArray[iPos]);
	  }
	}
      }
      iResult=iNofArgs;
    } else {
      iResult=-EINVAL;
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int IsSeparator(char c, const char* pSeparators)
{
  int iResult=0;
  if (c==0) {
    // the standard separator, this means the end of the element in the array of arguments 
    if ((g_mrShellPrimDbg&DBG_SCAN_ARG_DEF_DETAIL)==DBG_SCAN_ARG_DEF_DETAIL)
      fprintf(stderr, "IsSeparator: found standard separator\n");
    iResult=1;
  } else if (pSeparators) {
    // additional separators, for more than one argument in one element of the array of arguments
    int i=0;
    for (i=0; pSeparators[i]!=0; i++) {
      if ((iResult=(c==pSeparators[i]))) {
	if ((g_mrShellPrimDbg&DBG_SCAN_ARG_DEF_DETAIL)==DBG_SCAN_ARG_DEF_DETAIL)
	  fprintf(stderr, "IsSeparator: found separator %c\n", pSeparators[i]);
      }
    }
  }
  return iResult;
}

/* read arguments for the current definition
 */
int ReadArgument(const char** arrayArg, int iNofArgs, int iFirstArgStart, TArgDef* pDef, unsigned int flags)
{
  int iResult=0;
  if (arrayArg && arrayArg[0] && pDef) {
    // the global variable errno (set by system calls) indicates errors
    // since it is used later to check for errors in the number conversion,
    // we need to zero 'errno' to clear pending error indications
    errno=0; 
    const char* pArg=arrayArg[0]+iFirstArgStart;
    const char* pMessage="";
    int iLen=strlen(pArg);
    if (g_mrShellPrimDbg&(DBG_ARGUMENT_READ|DBG_SCAN_ARG_DEF))
      fprintf(stderr, "ReadArgument: scanning \'%s\' length=%d\n", pArg, iLen);
    switch (pDef->data.type) {
    case eInteger:
      {
	char* pRemaining=NULL;
	long int value=strtol(pArg, &pRemaining, 10);
	if (errno==0) {
	  if (pRemaining==NULL || *pRemaining==0) {
	    pDef->data.Int=value;
	    iResult=1;
	  }
	} else { 
	  iResult=-errno;
	}
      }
      pMessage="integer value expected";
      break;
    case eFloat:
      {
	char* pRemaining=NULL;
	double value=strtod(pArg, &pRemaining);
	if (errno==0) {
	  if (pRemaining==NULL || *pRemaining==0) {
	    pDef->data.Float=value;
	    iResult=1;
	  }
	} else { 
	  iResult=-errno;
	}
      }
      pMessage="float value expected";
      break;
    case eHex:
      {
	char* pRemaining=NULL;
	char* pStart=strstr(pArg, "0x");
	if (pStart && strlen(pStart)>0) {
	  long int value=strtol(pStart+2, &pRemaining, 16);
	  if (errno==0) {
	    if (pRemaining==NULL || *pRemaining==0) {
	      pDef->data.Hex=value;
	      iResult=1;
	    }
	  } else { 
	    iResult=-errno;
	  }
	}
      }
      pMessage="hexadecimal value expected, use 0x to indicate hex";
      break;
    case eHexArray:
      if (pDef->data.pHex) {
	iResult=sscanf(pArg, " 0x%x", pDef->data.pHex);
	if (g_mrShellPrimDbg&DBG_ARGUMENT_READ)
	  fprintf(stderr, "set %p with %s\n", pDef->data.pHex, pArg);
      }
      pMessage="hexadecimal value expected, use 0x to indicate hex";
      break;
    case eConstString:
      pDef->data.pString=pArg;
      if (g_mrShellPrimDbg&DBG_ARGUMENT_READ)
	fprintf(stderr, "ReadArgument: const string %p\n", pArg);
      iResult=1;
      break;
    case eFctArgDef:
      if (pDef->pFctArg && pDef->pFctArg->pDef) {
	iResult=ScanArguments(arrayArg, iNofArgs, iFirstArgStart, pDef->pFctArg->pDef, pDef->pFctArg->pMode);
      } else {
	fprintf(stderr, "ReadArgument error: invalid parameter to type eFctArgDef (pFctArg=%p)\n", pDef->pFctArg);
	iResult=-EFAULT;
      }
      break;
    case eIntegerArray:
    case eFloatArray:
    case eCharArray:
    case eComposite:
    case eFctIntegerArgs:  
    case eFctFloatArgs:    
    case eFctHexArgs: 
    case eFctCompositeArgs:
      fprintf(stderr, "ReadArgument: not yet implemented\n");
      break;
    case eFctNoArg: 
    case eFctIndex: 
    case eFctRemaining:
    case eFctInclusive:
    case eFctUserScan:
    case eBool:
    case eFlag:
      // no arguments to read
      break;
    default:
      fprintf(stderr, "ReadArgument: unknown argument type\n");
      break;
    }
    if (iResult>0) {
      if (pDef->data.type!=eFctArgDef) { // the result is already set correctly since ScanArguments was used for the scan
	iResult=(iLen<<SCANRET_BITSHIFT_OFFSET_LAST_ARG);
	if (g_mrShellPrimDbg&(DBG_ARGUMENT_READ|DBG_SCAN_ARG_DEF))
	  fprintf(stderr, "ReadArgument: 1 argument scanned, length=%d\n", iLen);
      }
    } else {
      const char* pSrcFct=NULL;
      if (g_mrShellPrimDbg&(DBG_ARGUMENT_READ|DBG_SCAN_ARG_DEF))
	pSrcFct="ReadArgument: ";
      if ((flags&SCANMODE_SILENT)==0)
	fprintf(stderr, "%serror scanning argument \'%s\': %s\n", pSrcFct==NULL?"":pSrcFct,pArg, pMessage);
      iResult=-ENODATA;
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int callCommandHandler(const char** arrayArg, int iNofArgs, int iFirstArgStart, TArgDef* pDef, unsigned int flags)
{
  int iResult=0;
  if (pDef) {
    if (g_mrShellPrimDbg&DBG_SCAN_ARG_DEF)
      fprintf(stderr, "callCommandHandler: %s, %s\n", pDef->s?pDef->s:"<empty>", pDef->l?pDef->l:"<empty>");
    switch (pDef->data.type) {
    case eInteger:
    case eFloat:
    case eHex:
    case eConstString:
    case eIntegerArray:
    case eFloatArray:
    case eHexArray:
    case eCharArray:
    case eComposite:
      // no sub function to call
      break;
    case eBool:
      pDef->data.Int=1;
      break;
    case eFlag:
      if (pDef->data.pInt) {
	*pDef->data.pInt|=pDef->setFlagPattern;
      }
      break;
    case eFctIntegerArgs:  
    case eFctFloatArgs:    
    case eFctHexArgs: 
    case eFctCompositeArgs:
      fprintf(stderr, "callCommandHandler: not yet implemented for type %d\n", pDef->data.type);
      break;
    case eFctNoArg:
      if (pDef->data.pFctNoArg) {
	iResult=(*pDef->data.pFctNoArg)();
      } else {
	fprintf(stderr, "callCommandHandler: warning no pointer of type FctNoArg\n");
      }
      break;
    case eFctRemaining:
    case eFctInclusive: // gets other arguments
      if (pDef->data.pSubArgDef) {
	const char* pNext="<empty>";
	if (arrayArg && arrayArg[0]) {
	  if (strlen(arrayArg[0])>iFirstArgStart)
	    pNext=arrayArg[0]+iFirstArgStart;
	  else if (iNofArgs>1 && strlen(arrayArg[0])==iFirstArgStart && arrayArg[1])
	    pNext=arrayArg[1];
	}
	if (g_mrShellPrimDbg&DBG_SCAN_ARG_DEF)
	  fprintf(stderr, "callCommandHandler: step into next iteration with argument \'%s\'\n", pNext);
	iResult=ScanArguments(arrayArg, iNofArgs, iFirstArgStart, pDef->data.pSubArgDef, pDef->pFctMode);
      } else {
	fprintf(stderr, "callCommandHandler: error, no sub level argument definitions\n");
      }
      break;
    case eFctUserScan:
      if (pDef->data.pFctUser) {
	iResult=(*pDef->data.pFctUser)(arrayArg[0]+iFirstArgStart, arrayArg,iNofArgs, pDef->pUser, NULL);
      } else {
	fprintf(stderr, "callCommandHandler: warning no pointer of type FctUserScan\n");
      }
      break;
    case eFctIndex:
      if (pDef->data.pFctIndex) {
	iResult=(*pDef->data.pFctIndex)(pDef->size);
      } else {
	fprintf(stderr, "callCommandHandler: warning no pointer of type FctIndex\n");
      }
      break;
    case eFctArgDef:
      if (pDef->data.pFctArgDef && pDef->pFctArg) {
	iResult=(*pDef->data.pFctArgDef)(pDef->pFctArg->pDef, pDef->pFctArg->pMode, pDef->pFctArg->pUser);
      } else {
	fprintf(stderr, "callCommandHandler: warning no pointer of type FctArgDef\n");
      }
      break;
    default:
      fprintf(stderr, "unknown argument type\n");
      break;
    }
    if (iResult==-ENODATA) {
      const char* pArgName = "empty";
      if (pDef->s) pArgName=pDef->s;
      else if (pDef->l) pArgName=pDef->l;
      fprintf(stderr, "missing parameter for command \'%s\'\n", pArgName);
    }
  }
  return iResult;
}

int SearchDef(const char* pArg, TArgDef* arrayDef, TFctMode *pMode)
{
  int iResult=0;
  if (pArg && arrayDef) {
    const char* pSeparators=NULL;
    unsigned long flags=0;
    if (pMode) {
      pSeparators=pMode->pSeparators;
      flags=pMode->flags;
    }
    int i=0;
    for (i=0; arrayDef[i].data.type!=eUnknownType && iResult==0; i++) {
      if (g_mrShellPrimDbg&DBG_SEARCH_ARG_DEF_DETAIL)
	fprintf(stderr, "SearchDef: check for argument no %d, flags %#x \n", i, arrayDef[i].flags);
      if ((arrayDef[i].flags&ARGPROC_FAILED)!=0 ||
	  ((arrayDef[i].flags&ARGPROC_FOUND)!=0 && (arrayDef[i].flags&ARGDEF_ONLY_ONCE)!=0)) {
	if (g_mrShellPrimDbg&DBG_SEARCH_ARG_DEF_DETAIL)
	  fprintf(stderr, "SearchDef: previously %s argument %d skipped\n", (arrayDef[i].flags&ARGPROC_FAILED)!=0?"failed":"found", i);
	continue;
      }
      const char* pMatch=NULL;
      int bUntermAllowed=0;
      int iLen=0;
      int bSeparated=1;
      if ((arrayDef[i].s==NULL || arrayDef[i].s[0]==0) &&
	  (arrayDef[i].l==NULL || arrayDef[i].l[0]==0)) {
	// special case for 'any argument'
	if (g_mrShellPrimDbg&DBG_SEARCH_ARG_DEF)
	  fprintf(stderr, "SearchDef: \'any argument\' type matched\n");
	if (i>0) {
	  pMatch="";
	} else {
	  fprintf(stderr, "SearchDef: there has to be a meaningfull argument definition in the first element\n");
	}
	bUntermAllowed=1;
      } else if ((arrayDef[i].flags&ARGDEF_KEYWORDLESS)!=0) {
	// special case for keywordless arguments
	if (g_mrShellPrimDbg&DBG_SEARCH_ARG_DEF)
	  fprintf(stderr, "SearchDef: keyword-less argument matched at position %d\n", i);
	pMatch="";
	if (arrayDef[i].l!=NULL && arrayDef[i].l[0]==0) pMatch=arrayDef[i].l; 
	else if (arrayDef[i].s!=NULL && arrayDef[i].s[0]==0) pMatch=arrayDef[i].s; 
      } else {
	/* 	if (arrayDef[i].l && arrayDef[i].l[0]!=0 && (strncmp(pArg, arrayDef[i].l, strlen(arrayDef[i].l))==0) && */
	/* 	    (arrayDef[i].s && strlen(arrayDef[i].l)<strlen(arrayDef[i].s) &&  */
	/* 	      strncmp(arrayDef[i].l, arrayDef[i].s, strlen(arrayDef[i].l))==0)==0) { */
	/* 	  pMatch=arrayDef[i].l; */
	/* 	  bUntermAllowed=arrayDef[i].flags&ARGDEF_UNTERM_LONG; */
	/* 	} else if (arrayDef[i].s && arrayDef[i].s[0]!=0 && (strncmp(pArg, arrayDef[i].s, strlen(arrayDef[i].s))==0)) { */
	/* 	  pMatch=arrayDef[i].s; */
	/* 	  bUntermAllowed=arrayDef[i].flags&ARGDEF_UNTERM_SHORT; */
	/* 	} */
	int bShortMatched=arrayDef[i].s!=NULL;
	int bLongMatched=arrayDef[i].l!=NULL;
	do {
	  if ((g_mrShellPrimDbg&DBG_SEARCH_ARG_DEF_DETAIL)==DBG_SEARCH_ARG_DEF_DETAIL)
	    fprintf(stderr, "SearchDef: iteration %d of definition %d (%s)\n", iLen, i, &pArg[iLen]);
	  bSeparated=IsSeparator(pArg[iLen], pSeparators);
	  if (bShortMatched==0 || arrayDef[i].s==NULL || arrayDef[i].s[0]==0) {}     // nothing to do, priviously detected missmatch
	  else if (bShortMatched==1 && arrayDef[i].s[iLen]=='*') {bShortMatched++;} // wildcard, from now on all the characters allowd
	  else if ((bShortMatched==1 && arrayDef[i].s[iLen]==0)||(bShortMatched==2 && bSeparated)) { // end of this definition string
	    bUntermAllowed=arrayDef[i].flags&ARGDEF_UNTERM_SHORT;
	    if (bSeparated || bUntermAllowed) {
	      if ((g_mrShellPrimDbg&DBG_SEARCH_ARG_DEF_DETAIL)==DBG_SEARCH_ARG_DEF_DETAIL)
		fprintf(stderr, "SearchDef: set pMatch to short version (%s)\n", arrayDef[i].s);
	      pMatch=arrayDef[i].s;
	    }
	    bShortMatched=0;
	  } else if (bShortMatched==1) {bShortMatched=(pArg[iLen]==arrayDef[i].s[iLen]);}
	  if (bLongMatched==0 || arrayDef[i].l==NULL || arrayDef[i].l[0]==0) {}     // nothing to do, priviously detected missmatch
	  else if (bLongMatched==1 && arrayDef[i].l[iLen]=='*') {bLongMatched++;} // wildcard, from now on all the characters allowd
	  else if ((bLongMatched==1 && arrayDef[i].l[iLen]==0)||(bLongMatched==2 && bSeparated)) {
	    bUntermAllowed=arrayDef[i].flags&ARGDEF_UNTERM_LONG;
	    if (bSeparated || bUntermAllowed) {
	      if ((g_mrShellPrimDbg&DBG_SEARCH_ARG_DEF_DETAIL)==DBG_SEARCH_ARG_DEF_DETAIL)
		fprintf(stderr, "SearchDef: set pMatch to long version (%s)\n", arrayDef[i].l);
	      pMatch=arrayDef[i].l;
	    }
	    bLongMatched=0;
	  } else if (bLongMatched==1) {bLongMatched=(pArg[iLen]==arrayDef[i].l[iLen]);}
	  if ((g_mrShellPrimDbg&DBG_SEARCH_ARG_DEF_DETAIL)==DBG_SEARCH_ARG_DEF_DETAIL)
	    fprintf(stderr, "SearchDef: end iteration %d short=%d long=%d\n", iLen, bShortMatched, bLongMatched);
	  iLen++;
	} while ((bShortMatched || bLongMatched) && bSeparated==0);
	iLen--;
      }
      if (pMatch) {
	if ((g_mrShellPrimDbg&DBG_SEARCH_ARG_DEF_DETAIL)==DBG_SEARCH_ARG_DEF_DETAIL)
	  fprintf(stderr, "check termination conditions for pMatch (%s)\n", pMatch);
	if (bSeparated || bUntermAllowed) {
	  //if (bSeparated==0 && iLen>0) iLen--;
	  const char* pPrint=pMatch;
	  if (pMatch[0]==0) pPrint="<empty>";
	  if (g_mrShellPrimDbg&DBG_SEARCH_ARG_DEF)
	    fprintf(stderr, "SearchDef: found argument %s index %d\n", pPrint, i);
	  // build the return value (containing, offset in argument array and last element and index
	  arrayDef[i].flags|=ARGPROC_FOUND;
	  iResult|=(i<<SCANRET_BITSHIFT_INDEX);
	  iResult|=(iLen<<SCANRET_BITSHIFT_OFFSET_LAST_ARG);
	  /* no offset in argument array!!
	     if (pArg[iLen]==0)
	     iResult|=(1<<SCANRET_BITSHIFT_PROCESSED_ARGS);
	  */
	  if ((g_mrShellPrimDbg&DBG_SEARCH_ARG_DEF_DETAIL)==DBG_SEARCH_ARG_DEF_DETAIL)
	    fprintf(stderr, "SearchDef: termination with pMatch (%s) iResult=%#x\n", pMatch, iResult);
	} else {
	  fprintf(stderr, "SearchDef: terminated argument reqired, set ARGDEF_UNTERM_SHORT/LONG flag(s) to allow unterminated argument\n");
	}
	break;
      }
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int PrintArgumentDefinition(TArgDef* arrayDef, int bAll)
{
  int iResult=0;
  if (arrayDef) {
    int i=0;
    for (i=0; arrayDef[i].data.type!=eUnknownType && iResult==0; i++) {
      if (bAll==1 || arrayDef[i].flags&ARGPROC_FOUND) {
	const char* pStrFound="";
	if (arrayDef[i].flags&ARGPROC_FOUND) pStrFound=" - found";
	const char* pArg = "empty";
	if (arrayDef[i].s) pArg=arrayDef[i].s;
	else if (arrayDef[i].l) pArg=arrayDef[i].l;
	switch (arrayDef[i].data.type) {
	case eInteger:
	  fprintf(stderr, "argument %d (%s) type integer: %d%s\n", i, pArg, arrayDef[i].data.Int, pStrFound);
	  break;
	case eFloat:
	  fprintf(stderr, "argument %d (%s) type float: %f%s\n", i, pArg, arrayDef[i].data.Float, pStrFound);
	  break;
	case eHex:
	  fprintf(stderr, "argument %d (%s) type hexadecimal: %#x%s\n", i, pArg, arrayDef[i].data.Hex, pStrFound);
	  break;
	case eConstString:
	  fprintf(stderr, "argument %d (%s) type const string: %p %s%s\n", i, pArg, arrayDef[i].data.pString, arrayDef[i].data.pString, pStrFound);
	  break;
	case eIntegerArray:
	  fprintf(stderr, "argument %d (%s) type integer array:%s\n", i, pArg, pStrFound);
	  break;
	case eFloatArray:
	  fprintf(stderr, "argument %d (%s) type float array:%s\n", i, pArg, pStrFound);
	  break;
	case eHexArray:
	  fprintf(stderr, "argument %d (%s) type hexadecimal array:%s\n", i, pArg, pStrFound);
	  break;
	case eCharArray:
	  fprintf(stderr, "argument %d (%s) type char array:%s\n", i, pArg, pStrFound);
	  break;
	case eComposite:
	  fprintf(stderr, "argument %d (%s) type composite array:%s\n", i, pArg, pStrFound);
	  break;
	case eFctNoArg: 
	  fprintf(stderr, "argument %d (%s) type function (no args):%s\n", i, pArg, pStrFound);
	  break;
	case eFctIndex: 
	  fprintf(stderr, "argument %d (%s) type function (index):%s\n", i, pArg, pStrFound);
	  break;
	case eFctRemaining:
	  fprintf(stderr, "argument %d (%s) type function (remaining args):%s\n", i, pArg, pStrFound);
	  break;
	case eFctInclusive:
	  fprintf(stderr, "argument %d (%s) type function (remaining inclusive args):%s\n", i, pArg, pStrFound);
	  break;
	case eFctUserScan:
	  fprintf(stderr, "argument %d (%s) type function (user scan):%s\n", i, pArg, pStrFound);
	  break;
	case eFctIntegerArgs:  
	  fprintf(stderr, "argument %d (%s) type function (int args):%s\n", i, pArg, pStrFound);
	  break;
	case eFctFloatArgs:    
	  fprintf(stderr, "argument %d (%s) type function (float args):%s\n", i, pArg, pStrFound);
	  break;
	case eFctHexArgs: 
	  fprintf(stderr, "argument %d (%s) type function (hex args):%s\n", i, pArg, pStrFound);
	  break;
	case eFctCompositeArgs:
	  fprintf(stderr, "argument %d (%s) type function (composite args):%s\n", i, pArg, pStrFound);
	  break;
	case eFctArgDef:
	  fprintf(stderr, "argument %d (%s) type function (argument def):%s\n", i, pArg, pStrFound);
	  break;
	case eBool:
	  fprintf(stderr, "argument %d (%s) type bool: %s%s\n", i, pArg, arrayDef[i].data.Int?"true":"false", pStrFound);
	  break;
	case eFlag:
	  fprintf(stderr, "argument %d (%s) type flag: %#x%s\n", i, pArg, arrayDef[i].data.Int, pStrFound);
	  break;
	default:
	  fprintf(stderr, "unknown argument\n");
	  break;
	}
      }
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int GetNofRequiredArgs(TArgDef* pDef)
{
  int iResult=0;
  if (pDef) {
    switch (pDef->data.type) {
    case eInteger:
    case eFloat:
    case eHex:
    case eConstString:
    case eFctArgDef:
      iResult=1;
      break;
    case eIntegerArray:
    case eFloatArray:
    case eHexArray:
    case eCharArray:
      iResult=pDef->size;
      break;
    case eFctIntegerArgs:  
    case eFctFloatArgs:    
    case eFctHexArgs: 
    case eFctCompositeArgs:
      {
	// the array is supposed to be terminated by a NULL element
	TTaggedData* arrayAddArgs = pDef->arrayComposite;
	for (iResult=0; arrayAddArgs[iResult].type!=eUnknownType; iResult++);
      }
      break;
    default:
      iResult=0;
    }
  } else {
    iResult = -EINVAL;
  }
  if ((g_mrShellPrimDbg&DBG_SCAN_ARG_DEF_DETAIL)==DBG_SCAN_ARG_DEF_DETAIL)
    fprintf(stderr, "GetNofRequiredArgs: iResult=%d\n", iResult);
  return iResult;
}

int ScanArguments(const char** arrayArg, int iNofArgs, int iFirstArgStart, TArgDef* arrayDef, TFctMode *pMode)
{
  int iResult=0;
  if (arrayArg && arrayDef) {
    if (pMode==NULL || (pMode->flags&SCANMODE_PERSISTENT)==0) mrShellPrimResetVolatileFlags(arrayDef);
    if ((g_mrShellPrimDbg&DBG_SCAN_ARG_DEF)==DBG_SCAN_ARG_DEF) {
      fprintf(stderr, "entering ScanArguments: %d arguments (first %s, offset %d)\n", iNofArgs, arrayArg[0], iFirstArgStart);
    }
    if (iNofArgs==0 || iNofArgs==1 && *(arrayArg[0]+iFirstArgStart)==0) return -ENODATA;

    int iCurrentArg=0;  // element index of the current command
    int iArg=0;         // loop index
    int iProcessedArgs=0; // updated at each successfully processed argument -> calculation of return value
    int iProcessedChars=0;// updated at each successfully processed argument -> calculation of return value
    int iCurrentCmd=-1; // definition array index of current command
    int iNofRequiredArgs=0; // number of required arguments for the current command
    int iProcArgStart=0;   // start of the candidates for the required arguments
    int iInitialPos=0; // initial position for scan within the element
    const char* pSeparators=NULL;
    unsigned int flags=0;
    if (pMode) {
      pSeparators=pMode->pSeparators;
      flags=pMode->flags;
    }
    for (iArg=0; iArg<iNofArgs && iResult==0; iArg++) {
      int iCurrentPos=iInitialPos; // start of the current command within the current element
      int iPos=iInitialPos;        // loop index
      iInitialPos=0;
      if (iProcArgStart>0) iProcArgStart=0; // processing is actcive, but there was an element switch 
      if ((g_mrShellPrimDbg&DBG_SCAN_ARG_DEF_DETAIL)==DBG_SCAN_ARG_DEF_DETAIL) {
	fprintf(stderr, "ScanArguments: scanning iArg=%d (%s)\n", iArg, arrayArg[iArg]);
	fprintf(stderr, "ScanArguments: iCurrentCmd=%d iNofRequiredArgs=%d iProcArgStart=%d\n", iCurrentCmd, iNofRequiredArgs, iProcArgStart);
      }
      const char* pArg=arrayArg[iArg];
      char* pWork=NULL;
      if (pSeparators) {
	// we need a working copy of the buffer since we have to temporarily alter it
	pWork=(char*)malloc(strlen(arrayArg[iArg])+1);
	if (pWork) {
	  strcpy(pWork, arrayArg[iArg]);
	  pArg=(const char*)pWork;
	}
      }
      if (pArg) {
	int iSize=strlen(pArg);
	if (iArg==0 && iFirstArgStart>0) {
	  iPos=iFirstArgStart;
	  iProcArgStart=iFirstArgStart;
	}
	for (; iPos<=iSize && iResult==0; iPos++) {
	  if ((g_mrShellPrimDbg&DBG_SCAN_ARG_DEF_DETAIL)==DBG_SCAN_ARG_DEF_DETAIL)
	    fprintf(stderr, "ScanArguments: iPos=%d (%s) iCurrentPos=%d (%s)\n", iPos, &pArg[iPos], iCurrentPos, &pArg[iCurrentPos]);
	  if (IsSeparator(pArg[iPos], pSeparators)) {
	    // terminate the string
	    int iPosBackup=iPos;
	    if (pWork) pWork[iPos]=0;
	    if (iCurrentCmd>=0) {
	      if (iNofRequiredArgs>0) {
		// there is a command active which needs to read the next argument
		if (iPos-iProcArgStart>0) {
		  if (arrayDef[iCurrentCmd].data.type==eConstString) pArg=arrayArg[iArg];
		  // read the actual argument
		  if (arrayDef[iCurrentCmd].data.type==eFctArgDef) {
		    iResult=ReadArgument(&arrayArg[iArg], iNofArgs-iArg, iProcArgStart, &arrayDef[iCurrentCmd], 0);
		  } else {
		    iResult=ReadArgument(&pArg, 1, iProcArgStart, &arrayDef[iCurrentCmd], 0);
		  }
		  if (iResult>=0) {
/* 		    iPos+=(iResult&SCANRET_MASK_OFFSET_LAST_ARG)>>SCANRET_BITSHIFT_OFFSET_LAST_ARG; */
/* 		    iArg+=(iResult&SCANRET_MASK_PROCESSED_ARGS)>>SCANRET_BITSHIFT_PROCESSED_ARGS; */
		    iProcArgStart=iPos;
		    iNofRequiredArgs--;
		    iResult=0; // keep the loop going
		  } else {
		    // handling of failed scan
		    if ((g_mrShellPrimDbg&DBG_SCAN_ARG_DEF_DETAIL)==DBG_SCAN_ARG_DEF_DETAIL)
		      fprintf(stderr, "error scanning argument for command \'%s\' at index %d (still %d required)\n", arrayDef[iCurrentCmd].l, iCurrentCmd, iNofRequiredArgs);
		    if (arrayDef[iCurrentCmd].flags&ARGDEF_RESUME) {
		      // forget about this command and try to find another matching sequence
		      if ((g_mrShellPrimDbg&DBG_SCAN_ARG_DEF_DETAIL)==DBG_SCAN_ARG_DEF_DETAIL)
			fprintf(stderr, "resume from failed command \'%s\' at index %d\n", arrayDef[iCurrentCmd].l, iCurrentCmd);
		      arrayDef[iCurrentCmd].flags|=ARGPROC_FAILED;
		      arrayDef[iCurrentCmd].flags&=~ARGPROC_FOUND;
		      if (iCurrentPos==0) {
		      iArg=iCurrentArg;
		      iPos=iCurrentPos-1;
		      iCurrentCmd=-1;
		      }
		      // keep the loop going
		      iResult=0;
		      continue;
		    } else if (flags&SCANMODE_SKIP_UKWN_SEQU) {
		      // update the values for calculation of result
		      arrayDef[iCurrentCmd].flags&=~ARGPROC_FOUND;
		      iProcessedArgs=iArg;
		      iProcessedChars=iPos;
		      iCurrentCmd=-1;
		      // keep the loop going
		      iResult=0;
		      continue;
		    }
		  }
		} else {
		  // just skip this separator
		  iProcArgStart=iPos+1;
		  if ((g_mrShellPrimDbg&DBG_SCAN_ARG_DEF_DETAIL)==DBG_SCAN_ARG_DEF_DETAIL)
		    fprintf(stderr, "ScanArguments: skip this separator\n");
		}
	      }
	      if (iNofRequiredArgs==0) {
		if ((arrayDef[iCurrentCmd].flags&ARGDEF_DELAY_EXECUTE)==0) {
		  if (arrayDef[iCurrentCmd].data.type==eFctInclusive)
		    iResult=callCommandHandler(&arrayArg[iCurrentArg], iNofArgs-iCurrentArg, iCurrentPos, &arrayDef[iCurrentCmd],0);
		  else
		    iResult=callCommandHandler(&arrayArg[iArg], iNofArgs-iArg, iProcArgStart, &arrayDef[iCurrentCmd],0);
		  if (iResult>=0) {
		    int iArgShift=(iResult&SCANRET_MASK_PROCESSED_ARGS)>>SCANRET_BITSHIFT_PROCESSED_ARGS;
		    if (g_mrShellPrimDbg&DBG_SCAN_ARG_DEF_DETAIL)
		      fprintf(stderr, "ScanArguments: callCommandHandler result %#x\n", iResult);
		    if (iArgShift>0) {
		      // iArg is set to the last processed command
		      iArg+=iArgShift-1;
		      // iPos is set to the iSize to terminate the inner loop, the next loop will start at
		      // iInitialPos, which is set to the start of the next argument
		      iPos=iSize;
		      iInitialPos=(iResult&SCANRET_MASK_OFFSET_LAST_ARG)>>SCANRET_BITSHIFT_OFFSET_LAST_ARG;
		      // update the values for calculation of result
		      if ((arrayDef[iCurrentCmd].flags&(ARGDEF_EXIT|ARGDEF_BREAK))==0) {
			iProcessedArgs=iArg+1;
			iProcessedChars=iInitialPos;
		      }
		    } else {
		      // only lPos has to be incremented by the number of processed characters
		      iPos+=(iResult&SCANRET_MASK_OFFSET_LAST_ARG)>>SCANRET_BITSHIFT_OFFSET_LAST_ARG;
		      // update the values for calculation of result
		      if ((arrayDef[iCurrentCmd].flags&(ARGDEF_EXIT|ARGDEF_BREAK))==0) {
			iProcessedArgs=iArg;
			iProcessedChars=iPos;
		      }
		    }
		    iResult=0;
		    // there are two modes of termination: SCANMODE_READ_ONE_CMD is a mode to the scan function which
		    // causes it to terminate after one processed entry. ARGDEF_TERMINATE, ARGDEF_EXIT and ARGDEF_BREAK 
		    // are options to an argument definition, which cause the function to return with either no error
		    // in case of ARGDEF_BREAK or error code -EINTR for the two others indicating that it was terminated
		    if (flags&SCANMODE_READ_ONE_CMD || arrayDef[iCurrentCmd].flags&(ARGDEF_TERMINATE|ARGDEF_EXIT|ARGDEF_BREAK)) {
		      if (g_mrShellPrimDbg&DBG_SCAN_ARG_DEF)
			fprintf(stderr, "ScanArguments: terminate at cmd=%d flags=%#x\n", iCurrentCmd, flags);
		      iArg=iNofArgs;
		      iPos=iSize;
		      if (arrayDef[iCurrentCmd].flags&(ARGDEF_TERMINATE|ARGDEF_EXIT)) {
			iResult=-EINTR;
			if (g_mrShellPrimDbg&DBG_SCAN_ARG_DEF)
			  fprintf(stderr, "ScanArguments: set result to -EINTR to indicate termination\n", iCurrentCmd, flags);
		      }
		    }
		  }
		}
		// we are done with this command
		iCurrentPos=iPos+1;
		iCurrentArg=iArg;
		iProcArgStart=iCurrentPos;
		if (g_mrShellPrimDbg&DBG_SCAN_ARG_DEF)
		  fprintf(stderr, "ScanArguments: finished %d, iCurrentPos=%d iCurrentArg=%d\n", iCurrentCmd, iCurrentPos, iCurrentArg);
		iCurrentCmd=-1;
	      }
	    } else if (iProcArgStart<iPos) {
	      // no command active, output the char sequence if enabled
	      if (flags&SCANMODE_PRINT_UKWN_SEQU)
		fprintf(stderr, &pArg[iProcArgStart]);
	      if ((flags&SCANMODE_SILENT)==0) {
		fprintf(stderr, "unknown argument (%s)\n", &pArg[iProcArgStart]);
	      }
	      if (flags&(SCANMODE_READ_ONE_CMD|SCANMODE_FORCE_TERMINATION)) {
		iPos=iSize;
		iArg=iNofArgs;
	      } else {
		iProcArgStart=iPos; // processing finished
	      }
	    }
	    // set the string back
	    if (pWork) pWork[iPosBackup]=arrayArg[iArg][iPosBackup];
	  } else if (iCurrentCmd>=0) {
	    // command active, continue iteration
	  } else if (iCurrentPos<iPos) {
	    // there is no command active but we are not right behind a separator or at beginning of element
	  } else if ((iResult=SearchDef((const char*)&pArg[iPos], arrayDef, pMode))>0) {
	    // a definition is matched
	    iCurrentCmd=(iResult&SCANRET_MASK_INDEX)>>SCANRET_BITSHIFT_INDEX;
	    if (iCurrentCmd!=0xff) {
		iCurrentPos=iPos;
		iCurrentArg=iArg;
		iPos+=(iResult&SCANRET_MASK_OFFSET_LAST_ARG)>>SCANRET_BITSHIFT_OFFSET_LAST_ARG;
		iArg+=(iResult&SCANRET_MASK_PROCESSED_ARGS)>>SCANRET_BITSHIFT_PROCESSED_ARGS;
		iProcArgStart=iPos--;
		iNofRequiredArgs=GetNofRequiredArgs(&arrayDef[iCurrentCmd]);
		if ((g_mrShellPrimDbg&DBG_SCAN_ARG_DEF_DETAIL)==DBG_SCAN_ARG_DEF_DETAIL)
		  fprintf(stderr, "ScanArguments: iCurentPos=%d iPos=%d iCurrentArg=%d iArg=%d\n", iCurrentPos, iPos, iCurrentArg, iArg);
		iResult=0;
	    } else {
	      // bad internal error
	      fprintf(stderr, "ScanArguments: internal error\n");
	      iCurrentCmd=-1;
	      iResult=-EFAULT;
	    }
	  } else if (iResult==-ENOENT && flags&SCANMODE_FORCE_TERMINATION) {
	    // the argument was not found in the list, termination is forced
	    if (g_mrShellPrimDbg&DBG_SCAN_ARG_DEF) {
	      fprintf(stderr, "ScanArguments: no entry found, terminate ...\n");
	    }
	  } else if (iResult==-ENOENT) {
	    //if (iProcArgStart<0) {
	      // neither separator nor command, but start of a normal char sequence, save the position
	    // iProcArgStart=iPos;
	    //}
	    iResult=0; // keep the loop going
	  }
	}
      } else {
	fprintf(stderr, "ScanArguments warning: ignore NULL argument at position %d\n", iPos);
      }
      if (pWork) free(pWork);
    }
    if (iNofRequiredArgs>0 && iResult>=0) {
      if ((flags&SCANMODE_SILENT)==0) {
	const char* pArg = "empty";
	if (iCurrentCmd>=0) {
	  if (arrayDef[iCurrentCmd].s) pArg=arrayDef[iCurrentCmd].s;
	  else if (arrayDef[iCurrentCmd].l) pArg=arrayDef[iCurrentCmd].l;
	}
	fprintf(stderr, "missing parameter for command \'%s\'\n", pArg);
      }
      iResult=-ENODATA;
    }
    if (iResult>=0) {
      // build the return value. it contains element offset, position within the last element and a command
      // index (not used outside, invalid so far)
      // iProcessedArgs points to the last array element which was processed, iProcessedChars points to the 
      // position within this element. The character at this position is supposed to be a separator
      // If it is also the end of the element and there are more elements available, the variables are set 
      // to the beginning of the next element, iProcessedChars incremented otherwize
      if (iProcessedArgs<iNofArgs && *(arrayArg[iProcessedArgs]+iProcessedChars)==0) {
	iProcessedArgs++; iProcessedChars=0;
      } else if (iProcessedArgs<iNofArgs && iProcessedChars>0) {
	// 
	//
	iProcessedChars++;
      }
      iResult=(iProcessedArgs<<SCANRET_BITSHIFT_PROCESSED_ARGS);
      iResult+=(iProcessedChars<<SCANRET_BITSHIFT_OFFSET_LAST_ARG);
      iResult+=(SCANRET_INVAL_INDEX<<SCANRET_BITSHIFT_INDEX);
      if (g_mrShellPrimDbg&DBG_SCAN_ARG_DEF) {
	fprintf(stderr, "finalize ScanArguments: iResult=%#x, %d argument(s) required\n", iResult, iNofRequiredArgs);
	PrintArgumentDefinition(arrayDef, 1);
      }
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

unsigned int mrShellPrimSetDebugFlag(unsigned int flag)
{
  g_mrShellPrimDbg|=flag;
  return g_mrShellPrimDbg;
}

unsigned int mrShellPrimClearDebugFlag(unsigned int flag)
{
  g_mrShellPrimDbg&=~flag;
  return g_mrShellPrimDbg;
}

int mrShellPrimPrintDbgFlags()
{
  fprintf(stderr, "mr shell primitives help:\n");
  fprintf(stderr, "use '--mrsdbg <hex number>' to set the following flags\n");
  fprintf(stderr, "DBG_SEARCH_ARG_DEF       : %#x\n",DBG_SEARCH_ARG_DEF);
  fprintf(stderr, "DBG_SEARCH_ARG_DEF_DETAIL: %#x\n",DBG_SEARCH_ARG_DEF_DETAIL);
  fprintf(stderr, "DBG_SCAN_ARG_DEF         : %#x\n",DBG_SCAN_ARG_DEF);
  fprintf(stderr, "DBG_SCAN_ARG_DEF_DETAIL  : %#x\n",DBG_SCAN_ARG_DEF_DETAIL);
  fprintf(stderr, "DBG_ARGUMENT_READ        : %#x\n",DBG_ARGUMENT_READ);
  fprintf(stderr, "DBG_ARGUMENT_CONVERT     : %#x\n",DBG_ARGUMENT_CONVERT);
  return 0;
}

int mrShellPrimGetIndex(TArgDef* arrayDef, const char* pCmd, int iType) {
  int iResult=-ENOENT;
  if (arrayDef && pCmd) {
    int i=0;
    for (i=0; arrayDef[i].data.type!=eUnknownType; i++) {
      if ((arrayDef[i].s && strcmp(arrayDef[i].s, pCmd)==0) || 
	  (arrayDef[i].l && strcmp(arrayDef[i].l, pCmd)==0)) {
	// type check if argument not eUnknownType
	if (iType==arrayDef[i].data.type || iType==eUnknownType) {
	    iResult=i;
	} else {
	  iResult=-EBADF;
	  fprintf(stderr, "mrShellPrim: type missmatch for element %d (%s)\n", i, pCmd);
	}
	break;
      }
    }
    if (iResult==-ENOENT) {
      fprintf(stderr, "mrShellPrim: can not find element %s\n", pCmd);
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int mrShellPrimSetData(TArgDef* arrayDef, const char* pCmd, void* pData, int iType) {
  int iResult=mrShellPrimGetIndex(arrayDef, pCmd, iType);
  if (iResult>=0) {
    if (arrayDef[iResult].data.type!=eInteger && arrayDef[iResult].data.type!=eFloat && 
	arrayDef[iResult].data.type!=eHex && arrayDef[iResult].data.type!=eBool) {
      arrayDef[iResult].data.pVoid=pData;
    } else {
      fprintf(stderr, "mrShellPrim: set data function only available for pointer types (%s)\n", pCmd);
      iResult=-ENOSYS;
    }
  }
  return iResult;
}

int mrShellPrimGetData(TArgDef* arrayDef, const char* pCmd, void** ppData, int iType) {
  int iResult=mrShellPrimGetIndex(arrayDef, pCmd, iType);
  if (iResult>=0) {
    int idx=iResult;
    if (arrayDef[idx].data.type!=eInteger && arrayDef[idx].data.type!=eFloat && 
	arrayDef[idx].data.type!=eHex && arrayDef[idx].data.type!=eBool) {
      if (arrayDef[idx].flags&ARGPROC_FOUND) {
	iResult|=0x1<<ARGPROC_EXISTS_BITSHIFT;
	if (ppData) {
	  *ppData=arrayDef[idx].data.pVoid;
	} else
	  iResult=-EINVAL;
      }
    } else {
      fprintf(stderr, "mrShellPrim: function only available for pointer types (%s)\n", pCmd);
      iResult=-ENOSYS;
    }
  }
  return iResult;
}

int mrShellPrimGetFloat(TArgDef* arrayDef, const char* pCmd, float* pFloat) {
  int iResult=mrShellPrimGetIndex(arrayDef, pCmd, eFloat);
  if (iResult>=0) {
    int idx=iResult;
    if (arrayDef[idx].flags&ARGPROC_FOUND) {
      iResult|=0x1<<ARGPROC_EXISTS_BITSHIFT;
      if (pFloat) *pFloat=arrayDef[idx].data.Float;
    }
  }
  return iResult;
}

int mrShellPrimGetInt(TArgDef* arrayDef, const char* pCmd, int* pInt) {
  // call with disabled type check, since two types are possible 
  int iResult=mrShellPrimGetIndex(arrayDef, pCmd, eUnknownType);
  if (iResult>=0) {
    int idx=iResult;
    if (arrayDef[idx].data.type==eInteger || arrayDef[idx].data.type==eBool) {
      if (arrayDef[idx].flags&ARGPROC_FOUND) {
	iResult|=0x1<<ARGPROC_EXISTS_BITSHIFT;
	if (pInt) *pInt=arrayDef[idx].data.Int;
      }
    } else {
      iResult=-EBADF;
      fprintf(stderr, "mrShellPrim: type missmatch for element %d (%s)\n", iResult, pCmd);
    }
  }
  return iResult;
}

int mrShellPrimGetHex(TArgDef* arrayDef, const char* pCmd, unsigned int* pHex) {
  int iResult=mrShellPrimGetIndex(arrayDef, pCmd, eHex);
  if (iResult>=0) {
    int idx=iResult;
    if (arrayDef[idx].flags&ARGPROC_FOUND) {
      iResult|=0x1<<ARGPROC_EXISTS_BITSHIFT;
      if (pHex) *pHex=arrayDef[idx].data.Hex;
    }
  }
  return iResult;
}

TArgDef* mrShellPrimCloneDef(TArgDef* arrayDef) {
  TArgDef* pResult=NULL;
  if (arrayDef) {
    int i=0;
    for (i=0; arrayDef[i].data.type!=eUnknownType; i++);
    if (i>0) {
      i++;
      pResult=(TArgDef*)malloc(i*sizeof(TArgDef));
      if (pResult) {
	memcpy(pResult, arrayDef, i*sizeof(TArgDef));
      }
    }
  }
  return pResult;
}

int mrShellPrimResetVolatileFlags(TArgDef* arrayDef) {
  int iResult=0;
  if (arrayDef) {
    int i=0;
    for (i=0; arrayDef[i].data.type!=eUnknownType; i++) {
      arrayDef[i].flags&=~(ARGPROC_FOUND|ARGPROC_FAILED);
    }
  }
  return iResult;
}

