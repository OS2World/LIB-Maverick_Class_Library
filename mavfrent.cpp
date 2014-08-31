/*  File: mavfrent.cpp      Updated: Sun Dec 17 19:26:32 1995
Copyright (c) Fabrizio Aversa
===========================================================*/

#include <mavfrent.hpp>
#include <mavconv.hpp>
#include <mavregex.hpp>

#define FORM_DATE_SEP " ,/-.=*_:"
#define FORM_TIME_SEP " ,/-.=*_:"

/* ====================== FormEntryField =================== */

FormEntryField::FormEntryField(unsigned long l, IWindow * iw, IWindow * iw1, const IRectangle & r)
:IEntryField(l, iw, iw1, r, IEntryField::defaultStyle() | IControl::tabStop
| IEntryField::margin),
piwParent(iw), IFocusHandler(), IKeyboardHandler()
{

   IFocusHandler::handleEventsFor(this);
   IKeyboardHandler::handleEventsFor(this);

   getCountryInfo(&CountryInfo);
}

FormEntryField::FormEntryField(unsigned long l, IWindow * iw)
:IEntryField(l, iw), piwParent(iw), IFocusHandler(), IKeyboardHandler()
{

   IKeyboardHandler::handleEventsFor(this);
   IFocusHandler::handleEventsFor(this);

   getCountryInfo(&CountryInfo);
}

FormEntryField::FormEntryField(unsigned long id, IWindow * parent,
IWindow * owner, const IRectangle & rect,
const IEntryField::Style & style)
:IEntryField(id, parent, owner, rect, style),
piwParent(parent), IFocusHandler(), IKeyboardHandler()
{

   IKeyboardHandler::handleEventsFor(this);
   IFocusHandler::handleEventsFor(this);

   getCountryInfo(&CountryInfo);
}

IWindow & FormEntryField::setFocus()
/* just to change colors of first field in a dialog when getting focus,
strangely gotfocus is not called automatically */
{
   setColor(border, IColor(IColor::red));
   setColor(background, IColor(IColor::white));
   setColor(foreground, IColor(IColor::black));

   return IEntryField::setFocus();

}

FormEntryField::gotFocus(IControlEvent&)
{
   setColor(border, IColor(IColor::red));
   setColor(background, IColor(IColor::white));
   setColor(foreground, IColor(IColor::black));

   return false;
}

FormEntryField::lostFocus(IControlEvent&)
{
   setColor(border, IColor(IColor::darkGray));
   setColor(background, IColor(IColor::white));

   return false;
}

/* ====================== AlphaEntryField =================== */

AlphaEntryField::AlphaEntryField(unsigned long l,
IWindow * iw, IWindow * iw1, const IRectangle & r,
Boolean b1, Boolean b2, IString s, IString s1)
:FormEntryField(l, iw, iw1, r)
{
   booCaps= b1;
   booNoNum= b2;
   strAllowed= s;
   strRegex= s1;

}

AlphaEntryField::AlphaEntryField(unsigned long l, IWindow * iw,
Boolean b1, Boolean b2, IString s, IString s1)
:FormEntryField(l, iw)
{
   booCaps= b1;
   booNoNum= b2;
   strAllowed= s;
   strRegex= s1;

}

AlphaEntryField::AlphaEntryField(unsigned long id, IWindow * parent,
IWindow * owner, const IRectangle & rect,
const IEntryField::Style & style)
:FormEntryField(id, parent, owner, rect, style)
{
   booCaps= false;
   booNoNum= false;
   strAllowed= "";
   strRegex= "";

}

Boolean AlphaEntryField::characterKeyPress(IKeyboardEvent& keyevt)
{
   CHAR ch= keyevt.character();

   if (booNoNum && ch >= '0' && ch <= '9') return true;

   if (strAllowed.length())
   if(!strAllowed.includes(ch)) return true;

   return false;
}


Boolean AlphaEntryField::lostFocus(IControlEvent& ctl)
{
   IString strFlags= "";
   IString strText= text();

   FormEntryField::lostFocus(ctl);

   if(booCaps) {
      setText(strText.upperCase());
      strFlags="i";
   }

   /* change single and double quotes in ` */
   for (USHORT i= 1; i <= strText.length(); i++)
   if (strText[i] == 39 || strText[i] == '"') strText[i]= '`';
   setText(strText);

   if(strRegex.length()) {

      CHAR pszTarget[100];
      strcpy(pszTarget, "");
      istrstream istAct((PSZ)text());
      ostrstream ostAct(pszTarget, 100);
      RegEx rx(istAct, ostAct, strFlags);
      rx.process(strRegex);

      if(!(*pszTarget)) {

         setColor(background, IColor(IColor::red));
         setColor(foreground, IColor(IColor::white));

         return true;
      }

   }

   return false;
}

/* ====================== NumEntryField =================== */

NumEntryField::NumEntryField(unsigned long id, IWindow * parent,
IWindow * owner, const IRectangle & rect,
const IEntryField::Style & style)
:FormEntryField(id, parent, owner, rect, style)
{
   dblMin = 0.0;
   dblMax = 0.0;
   booThous= true;
   booShowZero= false;
   setAlignment(right);

   strValidChars = (IString)"x0123456789e-";
   /* substitute 'x' with actual decimal separator */
   strValidChars[1] = CountryInfo.szDecimal[0];

}

NumEntryField::NumEntryField(unsigned long l,
IWindow * iw, IWindow * iw1, const IRectangle & r, Boolean b1, Boolean b2,
double dblMin1, double dblMax1)
:FormEntryField(l, iw, iw1, r)
{
   dblMin = dblMin1;
   dblMax = dblMax1;
   booThous= b1;
   booShowZero= b2;
   setAlignment(right);

   strValidChars = (IString)"x0123456789e-";
   /* substitute 'x' with actual decimal separator */
   strValidChars[1] = CountryInfo.szDecimal[0];

}

NumEntryField::NumEntryField(unsigned long l, IWindow * iw,
Boolean b1, Boolean b2,
double dblMin1, double dblMax1)
:FormEntryField(l, iw)
{
   dblMin = dblMin1;
   dblMax = dblMax1;
   booThous= b1;
   booShowZero= b2;
   setAlignment(right);

   strValidChars = (IString)"x0123456789e-";
   /* substitute 'x' with actual decimal separator */
   strValidChars[1] = CountryInfo.szDecimal[0];

}

Boolean NumEntryField::characterKeyPress(IKeyboardEvent& keyevt)
{
   USHORT i;
   Boolean booSign, booDecimal, booExp;

   i= text().length();

   if (text().includes(CountryInfo.szDecimal[0]))
   booDecimal= true;
   else
   booDecimal= false;

   if (i && (text()[1] == '-'))
   booSign= true;
   else
   booSign= false;

   if (text().includes('e'))
   booExp= true;
   else
   booExp= false;

   CHAR ch= keyevt.character();

   if ( ! (strValidChars).includes(ch)) return true;

   /* do not allow - if pos is not 1 or immediately after e */
   if (
   (ch == '-') && (booSign && (i && text()[i] != 'e'))
   ) return true;

   if (ch == 'e' && booExp) return true;

   if (ch == CountryInfo.szDecimal[0] && booDecimal) return true;

   return false;
}

IString NumEntryField::format(double d, Boolean booTh)
{
   INT i, iStart;

   IString strRet(d);

   // do not show 0
   if (!d && !booShowZero) return (strRet= "");

   /* replace '.' with country specific decimal separator */
   for (i= strRet.length() ; i; i-- ) {
      if ( strRet[i] == '.' ) strRet[i] =  CountryInfo.szDecimal[0];
   }

   if (booTh) {

      iStart= strRet.length()+1;

      /* find where integer part begins */
      for (i= strRet.length() ; i; i-- ) {
         if ( strRet[i] == CountryInfo.szDecimal[0] || strRet[i] == 'e') iStart= i ;
      }

      for (i= iStart-4 ; i >= (strRet[1] == '-' ? 2:1) ; i -= 3 ) {
         strRet= strRet.subString(1, i)+
         IString(CountryInfo.szThousandsSeparator)+
         strRet.subString(i+1, strRet.length()-i);
      }
   }

   return strRet;
}

double NumEntryField::getDouble()
{
   USHORT i, dotfound= 0 ;

   IString temps= this->text();

   temps.lowerCase().strip();

   if ( temps.length() == 0 ) return 0.0 ;

   /* remove thousands separators */
   for (i= 1 ; i<= temps.length() ; i++ ) {

      if ( temps[i] == CountryInfo.szThousandsSeparator[0] )
      temps= temps.subString(1, i-1)+temps.subString(i+1, temps.length()-i);

   } /* end of for */

   /* replace country specific decimal separator with '.' */
   for (i= 1 ; i<= temps.length() ; i++ ) {

      if ( temps[i] == CountryInfo.szDecimal[0] ) temps[i] = '.';

   } /* end of for */

   return temps.asDouble();

}

NumEntryField & NumEntryField::setDouble(double d)
{
   setText(format(d, booThous));

   /* if dblMin != dblMax means there is a range of validity */
   if(
   (d < dblMin || d > dblMax)
   && dblMin != dblMax
   && text().strip().length()
   ) {
      setColor(background, IColor(IColor::red));
      setColor(foreground, IColor(IColor::white));
   }

   return (NumEntryField &)(*this);
}

Boolean NumEntryField::gotFocus(IControlEvent& evt)
{
   FormEntryField::gotFocus(evt);

   if(text().lowerCase().length()) setText(format(getDouble(), false));

   return false;
}

Boolean NumEntryField::lostFocus(IControlEvent& ctl)
{

   FormEntryField::lostFocus(ctl);

   double dblThis = getDouble();

   if(text().lowerCase().length())
   setText(format(dblThis, booThous));

   /* if dblMin != dblMax means there is a range of validity */
   if(
   (dblThis < dblMin || dblThis > dblMax)
   && dblMin != dblMax
   && text().strip().length()
   ) {
      setColor(background, IColor(IColor::red));
      setColor(foreground, IColor(IColor::white));
   }

   return false;
}

/* ====================== DateEntryField =================== */

DateEntryField::DateEntryField(unsigned long id, IWindow * parent,
IWindow * owner, const IRectangle & rect,
const IEntryField::Style & style)
:FormEntryField(id, parent, owner, rect, style)
{
   booNoYear = false;
   setAlignment(right);
}

DateEntryField::DateEntryField(unsigned long l, IWindow * iw, Boolean b1)
:FormEntryField(l, iw), booNoYear(b1)
{
   setAlignment(right);
}

DateEntryField::DateEntryField(unsigned long l,
IWindow * iw, IWindow * iw1, const IRectangle & r, Boolean b1)
:FormEntryField(l, iw, iw1, r), booNoYear(b1)
{
   setAlignment(right);
}

Boolean DateEntryField::characterKeyPress(IKeyboardEvent& keyevt)
{
   USHORT i, j, SepPos[2], Da, Mo;

   /* if last char is a separator replace with the standard separator */
   if( text().length() > 1)
   if(((IString)FORM_DATE_SEP).includes(text()[text().length()]))
   setText(text().subString(1,text().length()-1)+CountryInfo.szDateSeparator);

   /* search separators */
   j=0;
   for (i= 1 ; (i <= text().length()) && (j < 2); i++ )
   /* check for possible separators */
   if ( strchr(FORM_DATE_SEP, text()[i]) ) SepPos[j++]= i;
   /* j number of separators found */

   CHAR ch= keyevt.character();

   /* only 2 separators allowed */
   if (((IString)FORM_DATE_SEP).includes(ch) && j == 2) return true;

   if (!(
   (ch >= '0' && ch <= '9')
   || ((IString)FORM_DATE_SEP).includes(ch)
   )) return true;

   if ((
   ((cursorPosition() == 2 && ( j == 0 || (j == 1 && SepPos[0] > 2)))
   ||
   (cursorPosition() == SepPos[0]+2 && j == 1))
   && !((IString)FORM_DATE_SEP).includes(ch)
   )) {
      /* user entered a number in a position where a
      separator should be, put separator */
      IString str1 = text();
      short usCurPos = cursorPosition();
      setText(str1.subString(1, usCurPos)
      +CountryInfo.szDateSeparator+
      str1.subString(usCurPos+1, str1.length() - usCurPos));
      setCursorPosition(usCurPos+1);
   }

   return false;
}

IDate DateEntryField::getDate()
{
   USHORT SepPos[2];
   USHORT i, j= 0, Da, Mo, Ye;
   IString temps= this->text();
   IDate dat1= 0;

   temps.strip();
   if((temps.length() < 6) && (!booNoYear)) return dat1;
   if((temps.length() < 4) && booNoYear) return dat1;

   /* search separators */
   for (i= 1 ; (i <= temps.length()) && (j < 2); i++ )
   /* check for possible separators */
   if ( strchr(FORM_DATE_SEP, temps[i]) ) SepPos[j++]= i;

   if ((j != 2) && (!booNoYear)) return dat1;

   /* read as dd/mm/yy or dd/mm if noyear */
   Da= atoi(temps.subString(1, SepPos[0]-1));
   if(booNoYear) {
      /* year = 1500 means this date represent a day/month set only */
      Mo= atoi(temps.subString(SepPos[0]+1,temps.length()-SepPos[0]));
      Ye= 1500;
   } else {
      Mo= atoi(temps.subString(SepPos[0]+1,SepPos[1]-SepPos[0]-1));
      Ye= atoi(temps.subString(SepPos[1]+1,temps.length()-SepPos[1]));
   }

   switch (CountryInfo.fsDateFmt) {

      case(0):
      /* it is to be interpreted as mm/dd/yy or mm/dd */
      /* swap mm with dd */
      i = Mo;
      Mo = Da;
      Da = i;

      break;

      case (1):
      break;

      case (2):
      /* it is to be interpreted as yy/mm/dd, DO NOT ADMIT noYear */

      if(booNoYear) {
         /* invalidate */
         Da = 0;
      }

      /* swap yy with dd */
      i = Ye;
      Ye = Da;
      Da = i;

      break;

   }

   /* year offset is now taken into account if year between 1970 and 2069 */
   /* if year < 70 it's beyond 2000 !!! */
   if(Ye < 70) Ye += 2000;
   else
   /* if year is > 100 user entered in format ccyy */
   if(Ye < 100) Ye += 1900;

   if(IDate::isValid((IDate::Month)Mo,Da,Ye)) {
      dat1= IDate((IDate::Month)Mo,Da,Ye);
   }

   return dat1;

}

DateEntryField & DateEntryField::setDate(IDate& d)
{
   IString strSep= IString(CountryInfo.szDateSeparator);
   IString strYearFormat, strNoYearFormat;

   if(d.julianDate()) {

      switch (CountryInfo.fsDateFmt) {

         case(0):
         strYearFormat= IString("%m") + strSep + IString("%d") + strSep + IString("%Y");
         strNoYearFormat= IString("%m") + strSep + IString("%d");
         break;

         case(1):
         strYearFormat= IString("%d") + strSep + IString("%m") + strSep + IString("%Y");
         strNoYearFormat= IString("%d") + strSep + IString("%m");
         break;

         case(2):
         strYearFormat= IString("%Y") + strSep + IString("%m") + strSep + IString("%d");
         strNoYearFormat= IString("%m") + strSep + IString("%d");
         break;

      } /* endswitch */

      if(booNoYear) {
         setText(d.asString(strNoYearFormat));
      } else {
         setText(d.asString(strYearFormat));
      }

   } else {

      setText("");

   }

   return (DateEntryField &)(*this);
}

IString DateEntryField::getTextWithDefault()
/* get date as string returning 1/1/1700 if
invalid date (for DB) */
{
   IDate dat1 = getDate();

   if( !dat1.julianDate() ) {
      dat1= IDate((IDate::Month)1,1,1700);
   }

   return DateToString(dat1, false, true);
}

DateEntryField & DateEntryField::setTextWithDefault(IString s)
/* set date from string considering 1/1/1700 as
invalid date (for DB) */
{
   IDate d = StringToDate(s);

   /* consider 1/1/1700 as default not to be displayed */
   if(d.dayOfYear() == 1 && d.year() == 1700) {
      d = 0;
   }

   return setDate(d);

}

Boolean DateEntryField::lostFocus(IControlEvent& ctl)
{
   IDate dat1= getDate();

   FormEntryField::lostFocus(ctl);

   if(dat1.julianDate()) {
      setDate(dat1);
   } else {
      if(text().strip().length()) {
         setColor(background, IColor(IColor::red));
         setColor(foreground, IColor(IColor::white));
      }
   }

   return false;
}

/* ====================== TimeEntryField =================== */

TimeEntryField::TimeEntryField(unsigned long id, IWindow * parent,
IWindow * owner, const IRectangle & rect,
const IEntryField::Style & style)
:FormEntryField(id, parent, owner, rect, style)
{
   setAlignment(right);
}

TimeEntryField::TimeEntryField(unsigned long l, IWindow * iw)
:FormEntryField(l, iw)
{
   setAlignment(right);
}

TimeEntryField::TimeEntryField(unsigned long l, IWindow * iw, IWindow * iw1, const IRectangle & r)
:FormEntryField(l, iw, iw1, r)
{
   setAlignment(right);
}

Boolean TimeEntryField::characterKeyPress(IKeyboardEvent& keyevt)
{
   USHORT i, j, SepPos[2];

   /* if last char is a separator replace with the standard separator */
   if( text().length() > 1)
   if(((IString)FORM_TIME_SEP).includes(text()[text().length()]))
   setText(text().subString(1,text().length()-1)+":");

   /* search separators */
   j=0;
   for (i= 1 ; (i <= text().length()) && (j < 2); i++ )
   /* check for possible separators */
   if ( strchr(FORM_TIME_SEP, text()[i]) ) SepPos[j++]= i;
   /* j number of separators found */

   CHAR ch= keyevt.character();

   /* only 2 separators allowed */
   if (((IString)FORM_TIME_SEP).includes(ch) && j == 2) return true;

   if (!(
   (ch >= '0' && ch <= '9')
   || ((IString)FORM_TIME_SEP).includes(ch)
   )) return true;

   if (!(
   (ch >= '0' && ch <= '9')
   || ((IString)FORM_DATE_SEP).includes(ch)
   )) return true;

   if ((
   ((cursorPosition() == 2 && ( j == 0 || (j == 1 && SepPos[0] > 2)))
   ||
   (cursorPosition() == SepPos[0]+2 && j == 1))
   && !((IString)FORM_TIME_SEP).includes(ch)
   )) {
      /* user entered a number in a position where a
      separator should be, put separator */
      IString str1 = text();
      short usCurPos = cursorPosition();
      setText(str1.subString(1, usCurPos)
      +CountryInfo.szTimeSeparator+
      str1.subString(usCurPos+1, str1.length() - usCurPos));
      setCursorPosition(usCurPos+1);
   }

   return false;
}

ITime TimeEntryField::getTime()
{
   USHORT SepPos[2];
   USHORT i, j= 0, Hour, Min, Sec;
   IString temps= this->text();
   ITime tim1= 0;

   temps.strip();
   if(temps.length() < 4) return tim1;

   /* search separators */
   for (i= 1 ; (i <= temps.length()) && (j < 2); i++ )
   /* check for possible separators */
   if ( strchr(FORM_TIME_SEP, temps[i]) ) SepPos[j++]= i;

   if (j == 0) return tim1;

   Hour= atoi(temps.subString(1, SepPos[0]-1));

   if (j == 1) SepPos[1]= temps.length()+1;

   Min= atoi(temps.subString(SepPos[0]+1,SepPos[1]-SepPos[0]-1));

   if(j == 2) {
      Sec= atoi(temps.subString(SepPos[1]+1,temps.length()-SepPos[1]));
   } else {
      Sec= 0;
   }

   tim1= ITime(Hour, Min, Sec);

   return tim1;

}

TimeEntryField & TimeEntryField::setTime(ITime& t)
{
   IString strSep= IString(CountryInfo.szTimeSeparator);
   IString strFormat;

   if(t.asSeconds()) {
      strFormat= IString("%H") + strSep + IString("%M") + strSep + IString("%S");
      setText(t.asString(strFormat));
   }

   return (TimeEntryField &)(*this);
}

Boolean TimeEntryField::lostFocus(IControlEvent& ctl)
{
   ITime tim1= getTime();

   FormEntryField::lostFocus(ctl);

   if(tim1.asSeconds()) {
      setTime(tim1);
   } else {
      if(text().strip().length()) {
         setColor(background, IColor(IColor::red));
         setColor(foreground, IColor(IColor::white));
      }
   }

   return false;
}

IString TimeEntryField::getTextWithDefault()
/* get time as string returning 00:00:00 if
invalid time (for DB) */
{
   ITime tim1 = getTime();

   if( !tim1.asSeconds() ) {
      return (IString)"00:00:00";
   }

   return tim1.asString("%H:%M:%S");
}


