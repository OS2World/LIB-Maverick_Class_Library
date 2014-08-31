/*  File: MAVFRENT.HPP      Updated: Tue Aug 15 15:55:13 1995
Copyright (c) Fabrizio Aversa
===========================================================*/
#ifndef MAVFRENT_HPP
#define MAVFRENT_HPP

#define INCL_DOSNLS        /* National Language Support values */
#include <os2.h>

#include <ikeyevt.hpp>
#include <ictlevt.hpp>
#include <ikeyhdr.hpp>
#include <ifocshdr.hpp>
#include <idate.hpp>
#include <itime.hpp>
#include <istring.hpp>
#include <ientryfd.hpp>
#include <icmdevt.hpp>
#include <icolor.hpp>
#include <strstrea.h>
#include <imsgbox.hpp>

class _Export FormEntryField: public IEntryField,
public IKeyboardHandler,
public IFocusHandler
{
   public:
   FormEntryField(unsigned long, IWindow *);
   FormEntryField(unsigned long, IWindow *, IWindow *, const IRectangle &);
   FormEntryField(unsigned long id, IWindow * parent,
   IWindow * owner, const IRectangle & rect, const IEntryField::Style & style);
   virtual IWindow & setFocus();
   COUNTRYINFO  CountryInfo; /* Buffer for country-specific information */

   private:
   virtual Boolean characterKeyPress(IKeyboardEvent&)= 0;

   protected:
   virtual Boolean gotFocus(IControlEvent&);
   virtual Boolean lostFocus(IControlEvent&);
   IWindow * piwParent;
};

class _Export NumEntryField: public FormEntryField
{
   public:
   NumEntryField(unsigned long id, IWindow * parent,
   IWindow * owner, const IRectangle & rect, const IEntryField::Style & style);
   NumEntryField(unsigned long, IWindow *, IWindow *,const IRectangle &,
   Boolean= true, Boolean= false, double = 0.0, double  = 0.0);
   NumEntryField(unsigned long, IWindow *, Boolean= true, Boolean= false,
   double = 0.0, double = 0.0);
   double getDouble();
   NumEntryField & setDouble(double);

   Boolean isThousDisplayed() const  {return booThous;};
   void thousDisplayed(Boolean booThousDisplayed) {booThous = booThousDisplayed;}
   Boolean isShowZero() const  {return booShowZero;};
   void showZero(Boolean showZero) {booShowZero = showZero;}
   double minAllowed() const {return dblMin;}
   void setMinAllowed (const double minAllowed) {dblMin = minAllowed;}
   double maxAllowed() const {return dblMax;}
   void setMaxAllowed (const double maxAllowed) {dblMax = maxAllowed;}

   private:

   double dblMin, dblMax;
   IString strValidChars;
   Boolean booThous; /* true if user wants thousands separators */
   Boolean booShowZero; /* true if user wants to show '0' when field=0 */

   virtual Boolean characterKeyPress(IKeyboardEvent&);
   virtual Boolean lostFocus(IControlEvent&);
   virtual Boolean gotFocus(IControlEvent&);

   IString format(double, Boolean);
};

class _Export AlphaEntryField: public FormEntryField
{
   public:
   AlphaEntryField(unsigned long, IWindow *, IWindow *,
   const IRectangle &, Boolean= false, Boolean= false, IString="", IString="");
   AlphaEntryField(unsigned long, IWindow *,
   Boolean= false, Boolean= false, IString="", IString="");
   AlphaEntryField(unsigned long id, IWindow * parent,
   IWindow * owner, const IRectangle & rect,
   const IEntryField::Style & style);

   Boolean isNoNumberAllowed() const  {return booNoNum;};
   void noNumberAllowed(Boolean noNumberAllowed) {booNoNum = noNumberAllowed;}
   Boolean isCapsLock() const  {return booCaps;};
   void capsLock(Boolean capsLock) {booCaps = capsLock;}
   IString& allowedChars () const {return (IString&)strAllowed;}
   void setAllowedChars (const IString& allowedChars) {strAllowed = allowedChars;}
   IString& regex () const {return (IString&)strRegex;}
   void setRegex (const IString& regex) {strRegex = regex;}

   private:
   virtual Boolean characterKeyPress(IKeyboardEvent&);
   virtual Boolean lostFocus(IControlEvent&);
   Boolean booCaps, booNoNum;
   IString strAllowed, strRegex;

};

class _Export DateEntryField: public FormEntryField
{
   public:
   DateEntryField(unsigned long id, IWindow * parent,
   IWindow * owner, const IRectangle & rect, const IEntryField::Style & style);
   DateEntryField(unsigned long, IWindow *, IWindow *,
   const IRectangle &, Boolean= false);
   DateEntryField(unsigned long, IWindow *, Boolean= false);
   IDate getDate();
   DateEntryField & setDate(IDate &);
   Boolean isNoYearDisplayed() const  {return booNoYear;};
   void noYearDisplayed(Boolean noYearDisplayed) {booNoYear = noYearDisplayed;}
   DateEntryField & setTextWithDefault(IString);
   IString getTextWithDefault();

   private:
   virtual Boolean characterKeyPress(IKeyboardEvent&);
   virtual Boolean lostFocus(IControlEvent&);
   Boolean booNoYear;

};

class _Export TimeEntryField: public FormEntryField
{
   public:
   TimeEntryField(unsigned long id, IWindow * parent,
   IWindow * owner, const IRectangle & rect, const IEntryField::Style & style);
   TimeEntryField(unsigned long, IWindow *, IWindow *, const IRectangle &);
   TimeEntryField(unsigned long, IWindow *);
   ITime getTime();
   TimeEntryField & setTime(ITime &);
   IString getTextWithDefault();

   private:
   virtual Boolean characterKeyPress(IKeyboardEvent&);
   virtual Boolean lostFocus(IControlEvent&);

};

#endif
