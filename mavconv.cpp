/*  File: mavconv.cpp      Updated: Sun Dec 17 19:21:04 1995
Copyright (c) Fabrizio Aversa
===========================================================*/
#define INCL_DOSNLS        /* National Language Support values */
#include <os2.h>

#include <istring.hpp>
#include <idate.hpp>

#include <mavconv.hpp>

void getCountryInfo(COUNTRYINFO * CountryInfo)
{
COUNTRYCODE  Country = {0};   /* Country code info (0 = current country) */
   ULONG        ulInfoLen = 0;
   APIRET       rc = 0;

   rc = DosQueryCtryInfo(sizeof(COUNTRYINFO), &Country,
   CountryInfo, &ulInfoLen);

   if (rc) {
      memset(CountryInfo, '\0', sizeof(COUNTRYINFO));
   }

}

/* ===================================================
Conversion function for idate
===================================================  */
IDate StringToDate(const IString & temps1)
/* Converts from IString to IDate
searching for separators and parsing supplied string */
{
   int SepPos[2];
   int i, j= 0, Da, Mo, Ye;
   IString temps= temps1;
   IDate dat1;
   COUNTRYINFO  CountryInfo; /* Buffer for country-specific information */

   getCountryInfo(&CountryInfo);

   temps.strip();
   if(temps.length()<6) {
      dat1= 0;
      return dat1;
   }
   /* try scanning date with separators as 12/10/90 or 12/10/1990 */

   /* search separators */
   for (i= 0 ; (i < temps.length()) && (j < 2); i++ )
   /* check for possible separators */
   if ( strchr(" ,/-.=*_:", temps[i]) ) SepPos[j++]= i;

   if (j == 2 && SepPos[0] > 0 && SepPos[1] - SepPos[0] > 1) { // 2 separ. found
      Da= atoi(temps.subString(1, SepPos[0]-1));
      Mo= atoi(temps.subString(SepPos[0]+1,SepPos[1]-SepPos[0]-1));
      Ye= atoi(temps.subString(SepPos[1]+1,temps.length()-SepPos[1]));
   } else {
      /* try scanning date without separators as 121090 or 12101990 */
      Da= atoi(temps.subString(1, 2));
      Mo= atoi(temps.subString(3, 2));
      Ye= atoi(temps.subString(5, temps.length()-4));
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
   } else {
      dat1= 0;
   }

   return dat1;

}

IString DateToString(const IDate & d, const Boolean booNoYear, const Boolean booFullYear)
{
   COUNTRYINFO  CountryInfo; /* Buffer for country-specific information */
   getCountryInfo(&CountryInfo);

   IString strSep= IString(CountryInfo.szDateSeparator);
   IString strYearFormat, strNoYearFormat;
   IString strYear4;
   if(booFullYear) {
      strYear4 = "%Y";
   } else {
      strYear4 = "%y";
   }

   if(d.julianDate()) {

      switch (CountryInfo.fsDateFmt) {

         case(0):
         strYearFormat= IString("%m") + strSep + IString("%d") + strSep + strYear4;
         strNoYearFormat= IString("%m") + strSep + IString("%d");
         break;

         case(1):
         strYearFormat= IString("%d") + strSep + IString("%m") + strSep + strYear4;
         strNoYearFormat= IString("%d") + strSep + IString("%m");
         break;

         case(2):
         strYearFormat= strYear4 + strSep + IString("%m") + strSep + IString("%d");
         strNoYearFormat= IString("%m") + strSep + IString("%d");
         break;

      } /* endswitch */

      if(booNoYear) {
         return d.asString(strNoYearFormat);
      } else {
         return d.asString(strYearFormat);
      }

   }

   return IString("");

}

double StringToDouble(IString & temps)
/* converts from a string representing a number in national
format to a double */
{
   USHORT i, dotfound= 0 ;

   COUNTRYINFO  CountryInfo; /* Buffer for country-specific information */
   getCountryInfo(&CountryInfo);

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

IString DoubleToString(const double d, const Boolean booTh)
/* formats a number in national format eventually with thousands separator */
{
   INT i, iStart;

   COUNTRYINFO  CountryInfo; /* Buffer for country-specific information */
   getCountryInfo(&CountryInfo);

   IString strRet(d);

   // do not show 0
   if (!d) return (strRet= "");

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




