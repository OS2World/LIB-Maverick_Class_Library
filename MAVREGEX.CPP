/*  File: MAVREGEX.CPP      Updated: Tue Aug 15 15:54:39 1995
Copyright (c) Fabrizio Aversa
===========================================================*/
#include <os2def.h>

#include <ctype.h>
#include <iostream.h>
#include <istring.hpp>
#include <strstrea.h>

#include <mavregex.hpp>

/*
Regex searches files for lines matching a regular expression.

Flags:
-c only print count of matching lines
-i ignore case in comparisons (GREP is case-sensitive by default)
-n each line is preceded by its line number
-v only print non-matching lines

The regular expression should be quoted if it contains blanks.
Characters are matched by the following rules:

x any ordinary character not mentioned below
'\'escapes (quotes) special characters seen below
'^'beginning-of-line
'$'end-of-line
'.'any character except end-of-line
':a'alphabetic character
':d'digit
':n'alphanumeric
': 'whitespace or other non-printable characters
'*'zero or more occurrences of previous character
'+'one or more occurrences of previous character
'-'optionally match previous character (say what?)
'[]'character classes:
--->match any character in the enumerated set
(the example above matches a run of vowels followed by
whitespace).  Ranges are allowed, as in [a-z], which
would match a lower-case letter.  '^' as the first
character in a class means match anything but what
is in the class, so [^aeiou] would match a consonant.

*/
/* From DECUS C Tools package on DEC systems - for non-commercial
use only.
Modifications by Chuck Allison, April 1985:

- handles quoted args (for embedded spaces)
- expands wildcards against indicated directory
- distinguishes case (ignore with -i)

Modifications by Fabrizio Aversa, Feb 1994:

- C++ porting, stream handling, incapsulation in a class
*/

#define REGEXLMAX 512
#define REGEXPMAX 256
#define CHARAC    1
#define BOL     2
#define EOL     3
#define ANY     4
#define CLASS   5
#define NCLASS  6
#define STAR    7
#define PLUS    8
#define MINUS   9
#define ALPHA   10
#define DIGIT   11
#define NALPHA  12
#define WHITE   13
#define RANGE   14
#define ENDPAT  15

RegEx::RegEx(istream & is1, ostream & os1, IString & str1):
isAct(is1), osAct(os1), strFlags(str1)
{
   parseFlags();
}

void RegEx::parseFlags()
{
   cflag = strFlags.includes("c");
   nflag = strFlags.includes("n");
   vflag = strFlags.includes("v");
   iflag = strFlags.includes("i");
}

int RegEx::compile( char * source)
/* ..Compile the pattern into global pbuf[].. */
{
   register char  *s;         /* Source string pointer     */
   register char  *lp;        /* Last pattern pointer      */
   register int   c;          /* Current character         */
   int            o;          /* Temp                      */
   char           *spp;       /* Save beginning of pattern */
   
   s = source;
   pp = pbuf;
   
   while (c = *s++)
   {
      /* ..STAR, PLUS and MINUS are special.. */
      if (c == '*' || c == '+' || c == '-')
      {
         if ( pp == pbuf ||
         (o=pp[-1]) == BOL ||
         o == EOL ||
         o == STAR ||
         o == PLUS ||
         o == MINUS
         ) {
            badpat("Illegal occurrance op.", source, s);
            return 0;
         }
         if(!store(ENDPAT)) return 0;
         if(!store(ENDPAT)) return 0;
         spp = pp;               /* Save pattern end     */
         while (--pp > lp)       /* Move pattern down    */
         *pp = pp[-1];        /* one byte             */
         *pp =   (c == '*') ? STAR :
         (c == '-') ? MINUS : PLUS;
         pp = spp;               /* Restore pattern end  */
         continue;
      }
      
      /* ..All the rest.. */
      lp = pp;         /* ..Remember start.. */
      switch(c)
      {
         case '^':
         if(!store(BOL)) return 0;
         break;
         case '$':
         if(!store(EOL)) return 0;
         break;
         case '.':
         if(!store(ANY)) return 0;
         break;
         case '[':
         if(!cclass(source, &s)) return 0;
         break;
         case ':':
         if (*s)
         {
            c = *s++;
            switch(tolower(c))
            {
               case 'a':
               if(!store(ALPHA)) return 0;
               break;
               case 'd':
               if(!store(DIGIT)) return 0;
               break;
               case 'n':
               if(!store(NALPHA)) return 0;
               break;
               case ' ':
               if(!store(WHITE)) return 0;
               break;
               default:
               badpat("Unknown : type", source, s);
               return 0;
            }
            break;
         }
         else {
            badpat("No : type", source, s);
            return 0;
         }
         case '\\':
         if (*s)
         c = *s++;
         default:
         if(!store(CHARAC)) return 0;
         if(!store(iflag ? tolower(c) : c)) return 0;
      }
   }
   if(!store(ENDPAT)) return 0;
   if(!store('\0')) return 0;
   return 1;
}


int RegEx::cclass(char * source, char **src)
/* char *source; ..Pattern start.. */
/* char *src;    ..Class start.. */
/* ..Compile a class (within []).. */
{
   register char   *s;        /* Source pointer    */
   register char   *cp;       /* Pattern start     */
   register int    c;         /* Current character */
   int             o;         /* ..Temp.. */
   
   s = *src;
   o = CLASS;
   if (*s == '^')
   {
      ++s;
      o = NCLASS;
   }
   if(!store(o)) return 0;
   cp = pp;
   if(!store(0)) return 0;                         /* ..Byte count.. */
   
   while ((c = *s++) && c!=']')
   {
      if (c == '\\')
      {  /* ..Store quoted char.. */
         if ((c = *s++) == '\0')  {
            /* ..Gotta get something.. */
            badpat("Class terminates badly", source, s);
            return 0;
         }  else
         if(!store(iflag ? tolower(c) : c)) return 0;
      }
      else if (c == '-' && (pp - cp) > 1 && *s != ']' && *s != '\0')
      {
         c = pp[-1];             /* Range start     */
         pp[-1] = RANGE;         /* Range signal    */
         if(!store(c)) return 0;               /* Re-store start  */
         c = *s++;               /* Get end char and*/
         if(!store(iflag ? tolower(c) : c)) return 0;
      }
      else
      if(!store(iflag ? tolower(c) : c)) return 0;
   }
   
   if (c != ']') {
      badpat("Unterminated class", source, s);
      return 0;
   }
   if ((c = (pp - cp)) >= 256) {
      badpat("Class too large", source, s);
      return 0;
   }
   if (c == 0) {
      badpat("Empty class", source, s);
      return 0;
   }
   
   *cp = c;
   
   *src= s;
   return 1;
}

int RegEx::store(int op)
{
   if (pp >= &pbuf[REGEXPMAX]) return 0;
   *pp++ = op;
   return 1;
}

void RegEx::badpat(char * message, char * source, char * s)
/*  char  *message,       ..Error message.. */
/* *source;        ..Pattern start.. */
{
   cerr << "-GREP-E-%s, pattern is " << message
   << ' ' << source << ' ' << s;
}

void RegEx::process(IString & strPattern)
/* ..Scan the file for the pattern in pbuf[].. */
{
   register int lno, count, m;
   
   lno = 0;
   count = 0;
   char c;
   int i;
   
   if(!compile((PSZ)strPattern)) {
      cerr << "can't compile " << endl;
   }
   
   while (!isAct.eof())
   {
      
      for(i=0;i<REGEXLMAX;i++) {
         isAct >> c;
         if (c==10 || isAct.eof()) break;
         *(lbuf+i)= c;
      }
      *(lbuf+i)=0;
      
      ++lno;
      m = match();
      if ((m && !vflag) || (!m && vflag))
      {
         ++count;
         if (!cflag)
         {
            if (nflag)
            osAct << lno << '\t';
            osAct << lbuf << endl;
         }
      }
   }
}

int RegEx::match()
/* ..Match the current line (in lbuf[]), return 1 if it does.. */
{
   register char *l;        /* ..Line pointer.. */
   
   for (l = lbuf; *l; l++)
   if (pmatch(l, pbuf))
   return(1);
   
   return(0);
}

char * RegEx::pmatch(char * line, char *pattern)
/* char *line,     ..(partial) line to match.. */
/* *pattern;   ..(partial) pattern to match.. */
{
   register char   *l;        /* ..Current line pointer.. */
   register char   *p;        /* ..Current pattern pointer.. */
   register char   c;         /* ..Current character.. */
   register char   d;         /* ..Temporary character.. */
   char            *e;        /* ..End for STAR and PLUS match.. */
   int             op;        /* ..Pattern operation.. */
   int             n;         /* ..Class counter.. */
   char            *are;      /* ..Start of STAR match.. */
   
   l = line;
   p = pattern;
   while ((op = *p++) != ENDPAT)
   {
      switch(op)
      {
         case CHARAC:
         c = iflag ? tolower(*l++) : *l++;
         d = iflag ? tolower(*p++) : *p++;
         if (c != d)
         return(0);
         break;
         case BOL:
         if (l != lbuf)
         return(0);
         break;
         case EOL:
         if (*l != '\0' && *l != '\n')
         return(0);
         break;
         case ANY:
         if (*l++ == '\0')
         return(0);
         break;
         case DIGIT:
         if (!isdigit(c = *l++))
         return(0);
         break;
         case ALPHA:
         if (!isalpha(c = *l++))
         return(0);
         break;
         case NALPHA:
         if (!isalnum(c = *l++))
         return(0);
         break;
         case WHITE:
         if (!isspace(c = *l++))
         return(0);
         break;
         case CLASS:
         case NCLASS:
         c = iflag ? tolower(*l++) : *l++;
         n = *p++ & 0x00ff;
         do
         {
            if (*p == RANGE)
            {
               p += 3;
               n -= 2;
               if (c >= p[-2] && c <= p[-1])
               break;
            }
            else if (c == *p++)
            break;
         } while (--n > 1);
         if ((op == CLASS) == (n <= 1))
         return(0);
         if (op == CLASS)
         p += n - 2;
         break;
         case MINUS:
         e = pmatch(l, p);       /* ..Look for a match.. */
         while (*p++ != ENDPAT); /* ..Skip over pattern.. */
         if (e)                  /* ..Got a match?.. */
         l = e;               /* ..Yes, update string.. */
         break;                  /* ..Always succeeds.. */
         case PLUS:                 /* ..One or more.. */
         if ((l = pmatch(l, p)) == 0)
         return(0);           /* ..Gotta have a match.. */
         case STAR:                 /* ..Zero or more.. */
         are = l;                /* ..Remember line start.. */
         while (*l && (e = pmatch(l, p)))
         l = e;               /* ..Get longest match.. */
         while (*p++ != ENDPAT); /* ..Skip over pattern.. */
         while (l >= are)
         {  /* ..Try to match rest.. */
            e = pmatch(l, p);
            if (e)
            return(e);
            --l;                 /* ..Nope, try earlier.. */
         }
         return(0);              /* ..Nothing else worked.. */
         default:
         cerr << "Bad op code " << op << endl;
         cerr << "Cannot happen -- match" << endl;
      }
   }
   return(l);
}

