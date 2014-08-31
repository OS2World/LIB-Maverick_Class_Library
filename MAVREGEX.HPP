/*  File: MAVREGEX.HPP      Updated: Tue Aug 15 15:55:27 1995
Copyright (c) Fabrizio Aversa
===========================================================*/
#ifndef MAVREGEX_HPP
#define MAVREGEX_HPP

#define REGEXLMAX 512
#define REGEXPMAX 256

class _Export RegEx {
   public:
   RegEx(istream &, ostream &, IString&);
   void process(IString &);
   
   private:
   
   int  cflag, nflag,vflag,iflag;
   char *pp, lbuf[REGEXLMAX], pbuf[REGEXPMAX];
   IString strPattern, strFlags;
   istream & isAct;
   ostream & osAct;
   
   void parseFlags();
   int compile( char *);
   int cclass(char *, char **);
   int store(int);
   void badpat(char *, char *, char *);
   int match();
   char *pmatch(char *, char *);
};

#endif
