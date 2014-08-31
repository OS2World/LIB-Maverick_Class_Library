/*  File: MAVDRSR.HPP      Updated: Tue Aug 15 15:55:07 1995
Copyright (c) Fabrizio Aversa
===========================================================*/
#ifndef MAVDRSR_HPP
#define MAVDRSR_HPP

class _Export DirServ
{
   private:
   HDIR FindHandle;
   INT iCount;
   APIRET rc; /* Return code from various functions */
   IString strPath;
   
   public:
   IString getNext();
   INT errorCode();
   DirServ(IString &);
   
};

#endif
