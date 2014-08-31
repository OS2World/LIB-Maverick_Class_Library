/*  File: MAVDRSR.CPP      Updated: Tue Aug 15 15:53:36 1995
Copyright (c) Fabrizio Aversa
===========================================================*/
#define INCL_DOS
#include <os2.h>

#include <ctype.h>
#include <iostream.h>
#include <istring.hpp>

#include <mavdrsr.hpp>

DirServ::DirServ(IString & str1):
strPath(str1)
{
   rc= 0;
   iCount= 0;
   FindHandle= 0;
}

INT DirServ::errorCode()
{
   return rc;
}

IString DirServ::getNext()
{
   ULONG FindCount= 1;
   FILEFINDBUF3  FindBuffer;
   IString strRet;
   
   switch(iCount) {
      
      case (-1):
      strRet= "";
      return strRet;
      break;
      
      case(0):
      
      FindHandle= 0x0001;
      
      rc = DosFindFirst((PSZ)strPath,     /* File pattern */
      &FindHandle, /* Directory search handle */
      0,     /* Search attribute */
      (PVOID) &FindBuffer,   /* Result buffer */
      sizeof(FindBuffer),  /* Result buffer length */
      &FindCount,  /* # of entries to find */
      FIL_STANDARD); /* Return level 1 file info */
      
      break;
      
      default:
      
      rc = DosFindNext(FindHandle, /* Directory handle */
      (PVOID) &FindBuffer,  /* Result buffer */
      sizeof(FindBuffer), /* Result buffer length */
      &FindCount);        /* Number of entries to find */
      
      break;
      
   }
   
   if(rc) {
      strRet= "";
      DosFindClose(FindHandle);
      iCount= -1;
   } else {
      iCount++;
      strRet= FindBuffer.achName;
   }
   
   return strRet;
}

