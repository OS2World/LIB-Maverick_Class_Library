
Welcome to MAVERICK CLASS LIBRARY v1.30!

1. WHAT IS IT ?
MAVCL is a 32 bit class library written in IBM C++ (part of IBM VISUALAGE FOR C++)
providing support for those typical business and general purpose application functions
not directly provided by IBM Open Class class library. In particular MAVCL implements
formatted entry fields, quality printer support, regular expressions,
directory services and conversion functions
for date / time with national language support.
A VBE and a VBB file are included, for formatted entry fields support from
VISUAL BUILDER. In the current version IBM Visual Builder lacks the support for
formatted entry fields that VA for Smalltalk has.

SOURCE CODE IS INCLUDED.

I've been using these classes for two years in my OS/2 applications
with high reliability and user satisfaction. I'm distributing the product
as freeware to promote OS/2 application development and to get
comments from other developers that could help me in improving
my final product quality.

CLASSES ARE DOCUMENTED IN THIS FILE (README.TXT).

If I'll receive positive feedback from YOU I'll supply detailed design
in the versions to come. The source code is commented everywhere.

The sample program SAMPLE.EXE prints a typical business report
with data arranged in columns, text break within a column and
a bitmap printed. It also allows the user to enter text, date and numbers in
sample formatted entry fields.

AS EVERYTHING IS LINKED DINAMICALLY YOU MUST HAVE VISUALAGE DLLs
IN YOUR LIBPATH TO RUN THIS SAMPLE.

Otherwise you can recompile linking to static libraries.

2. INSTALLATION

2.1 Development environment
unzip MAVCLnnn.ZIP in a directory which is in named your LIBPATH, INCLUDE, LIB
environment variables.
This directory will contain all relevant source and include files,
the DLL (MAVCL.DLL) and the sample program SAMPLE.EXE.
The subdirectory SAMPLE contains the source code for the sample program.
This release is compiled with IBM Visualage for C++ version 3.0 upgraded
al level CTC306, CTO306.
To get these updates ftp ftp.software.ibm.com /ps/products/visualagecpp/fixes
or ftp.boulder.ibm.com same directory.

2.2 Runtime environment
Your compiled program only needs MAVCL.DLL in a directory named in LIBPATH.
MAVCL.DLL cannot be renamed.

2.3 Visual Builder
Simply load the MAVFRENT.VBB file or import the parts information contained
in MAVFRENT.VBE to recreate MAVFRENT.VBB.
The LIBPATH, INCLUDE, LIB must be as described in 2.1 and 2.2 above.
For usage of formatted entry fields in Visual Builder see section 3.1.3.

If you send me some nice icons for the four entry fields, I'll integrate them
in the DLL to make them show automatically in the VB palette, in next version.

3. FEATURES
In this section a short description is given of the classes and their main methods.

3.1 Formatted entry fields

you must include: MAVFRENT.HPP
source code: MAVFRENT.CPP

3.1.1 Description

The class FormEntryField inherits from IEntryField. Do not create instances
of this class. It is only used as a base class for:
AlphaEntryField: entry fields for alphanumeric (IString) variables
NumEntryField: entry fields for numeric (double) variables
DateEntryField: entry fields for date (IDate) variables
TimeEntryField: entry fields for time (ITime) variables

NumEntryField and DateEntryField take into account national settings of OS/2
to format and check variables.

All classes have a constructor for use with a resource field and a
constructor to specify also coordinates in a window, overriding
corresponding contrustors of IEntryField.

Depending on legal characters for each field and field configuration
illegal keys are inhibited (for example during numeric entry only
numbers, +, -, e characters are allowed).

For fields requiring a separator all of  " ,/-.=*_:" characters are valid
separators. When leaving the field the separator required by national
settings is used to format the string in the field.

When leaving the field with an invalid entry (for example and invalid date
for DateEntryField or a number out of the user-given range for NumEntryField),
the field background turns red to alert the operator.

3.1.2 Main methods

3.1.2.1 AlphaEntryField

Constructors:

AlphaEntryField::AlphaEntryField(unsigned long l,
IWindow * iw, IWindow * iw1, const IRectangle & r,
Boolean b1, Boolean b2, IString s, IString s1)

AlphaEntryField::AlphaEntryField(unsigned long l, IWindow * iw,
Boolean b1, Boolean b2, IString s, IString s1)

where l is the field id, iw and iw1 are parent and owner, r is the
framing rectangle (for these first four variables refer to IEntryField).
b1=true if all capital characters required
b2=true if no numeric characters are allowed
s is a string containing all allowed characters, if empty (default)
no check is made.
s1 is a regular expression that entry field must match, if empty (default)
no check is made.

The constructor
AlphaEntryField(unsigned long id, IWindow * parent,
IWindow * owner, const IRectangle & rect,
const IEntryField::Style & style);
is supplied for Visual Builder.

Main methods:

setText() to set text.
text() to get text().

Instance variable access methods (used also by Visual Builder)
Boolean isNoNumberAllowed() const
void noNumberAllowed(Boolean noNumberAllowed)
Boolean isCapsLock() const
void capsLock(Boolean capsLock)
IString& allowedChars () const
void setAllowedChars (const IString& allowedChars)
IString& regex () const
void setRegex (const IString& regex)

3.1.2.2 NumEntryField

Constructors:

NumEntryField::NumEntryField(unsigned long l,
IWindow * iw, IWindow * iw1, const IRectangle & r, Boolean b1, Boolean b2,
double dblMin1, double dblMax1)

NumEntryField::NumEntryField(unsigned long l, IWindow * iw,
Boolean b1, Boolean b2,
double dblMin1, double dblMax1)

The constructor
NumEntryField(unsigned long id, IWindow * parent,
IWindow * owner, const IRectangle & rect,
const IEntryField::Style & style);
is supplied for Visual Builder.

where l is the field id, iw and iw1 are parent and owner, r is the
framing rectangle (for these first four variables refer to IEntryField).
b1=true thousands separator are to be shown when the field is left
b2=true if "0" is to be shown when the fields is 0, b2=false means
the field is left empty when the content is 0.
if dblMin1 and dblMax1 are indicated a check is made when leaving
the field that the content is in the given range

Main methods:

setDouble(double) to set the field content.
double getDouble() to get the field content.

Instance variable access methods (used also by Visual Builder)
Boolean isThousDisplayed() const
void thousDisplayed(Boolean booThousDisplayed)
Boolean isShowZero() const
void showZero(Boolean showZero)
double minAllowed() const
void setMinAllowed (const double minAllowed)
double maxAllowed() const
void setMaxAllowed (const double maxAllowed)

3.1.2.3 DateEntryField

Constructors:

DateEntryField::DateEntryField(unsigned long l,
IWindow * iw, IWindow * iw1, const IRectangle & r, Boolean b1)

DateEntryField::DateEntryField(unsigned long l, IWindow * iw, Boolean b1)

where l is the field id, iw and iw1 are parent and owner, r is the
framing rectangle (for these first four variables refer to IEntryField).
b1=true means do not display year (only month/day)

The constructor
DateEntryField(unsigned long id, IWindow * parent,
IWindow * owner, const IRectangle & rect,
const IEntryField::Style & style);
is supplied for Visual Builder.

Main methods:

setDate(IDate &) to set the field content.
IDate getDate() to get the field content.
IString getTextWithDefault() get the date as text returning 01/01/1700 if
invalid date entered (for DB default)

Instance variable access methods (used also by Visual Builder)
Boolean isNoYearDisplayed() const
void noYearDisplayed(Boolean noYearDisplayed)

3.1.2.4 TimeEntryField

TimeEntryField::TimeEntryField(unsigned long l, IWindow * iw,
IWindow * iw1, const IRectangle & r)

TimeEntryField::TimeEntryField(unsigned long l, IWindow * iw)

where l is the field id, iw and iw1 are parent and owner, r is the
framing rectangle (for these first four variables refer to IEntryField).

The constructor
TimeEntryField(unsigned long id, IWindow * parent,
IWindow * owner, const IRectangle & rect,
const IEntryField::Style & style);
is supplied for Visual Builder.

Main methods:

setTime(ITime &) to set the field content.
ITime getTime() to get the field content.
IString getTextWithDefault() get the time as text returning 00:00:00 if
invalid time entered (for DB default)

3.1.3 Visual Buider Notes
VBE IMPORTED VISUAL PARTS ARE AVAILABLE FROM LEVEL CTV302 ON.
You will be able to use formatted entry fields using the "Option -> Add part" menu
item and typing, for example, AlphaEntryField* in the type field. Then
place the cursor anywhere in your composer area and click.
Entry field parts shall appear as pale gray boxes in the composition editor,
which means you will not see a thing on a pale grey background.
Attributes and actions have been added to make the connections.
If you plan to use Maverick entry fields often, which is the case for
business application developers, you'll probably like to add four entries
to the data entry palette of Visual Builder using the
"Option -> Modify palette -> Add New Part" menu item.
If you need to connect date, time and numeric entry fields content
to other objects with an attribute-attribute connection, you'll
need attributes of the corresponding type (IDate, ITime, double), which are
included.
If you need to connect date or time to objects created by the Data Access Builder,
you'll find that DAX creates attibutes of type IString for both DATE and TIME DB2
field types. Unfortunately you cannot connect diectly the text attribute of
Formatted entry fields to these DAX object attribute, as, if you enter an invalid
date, an exception is raised (maybe that's what you want, in that case no problem).
The textWithDefault attributes are implemented for date and time entry fields.
These atributes contain default values (01 jan 1700 for dates and 00:00:00 for time)
if the user entry is NULL or invalid, so you can connect them to DAX objects and
NULL values are handled.

3.2 Printer support

you must include: MAVPRINT.HPP
source code: MAVPRINT.CPP MPSETUP.CPP

3.2.1 Description

The MavPrint class supplies support to the activities of printer
selection, printer selection dialog, font selection and retrieving,
basic string printing in a presentation space, text printing in a
defined rectangle in a presentation space with line wrap and
bitmap printing in a defined rectangle in a presentation space with
or without maintaining aspect ratio.
The page is divided in header (space above margins),
 margined page (space inside margins)
and footer (space below margins).
An handle is given to the presentation space
and one to the device context so that the programmer can use all Gpi
OS/2 functions directly.
The printer selection method is taken from the article "One Call Does It All"
by Monte Copeland in Developer Connection 6. I only rewrote the dialog
functions using IBM classes and changed few things for C++ recompilation.

3.2.2 Main methods

All Bollean functions return true on successfull completion,
false on error.

void MavPrint::setHAB(HAB h1)
void MavPrint::setWorldCoords(LONG l1)

You MUST call these two function before calling setupPrinter to
set the anchor block and the World Coordinates unit of measure (for
ex. PU_TWIPS or PU_LOMETRIC)

Boolean MavPrint::setupPrinter(PSZ pszUserQueueName,
PSZ pszUserPrintFileName,
IWindow *, int)

Boolean MavPrint::setupPrinter(IWindow *, int)

setupPrinter is the printer selection function.
It fills the private structure PRINTERSETUP with the selected printer
relevant data.

call with pszUserQueueName if you want a specific queue you know in advance,
in this case you can also use pszUserPrintFileName for file printing.
call with a valid owner (IWindow*) if you want the print queue dialog
to be shown. the last int parameter is the number of copies.
These classes have been written in Italy, so if country code is
039 the printer selection dialog is shown in italian, otherwise in english.
Always call with PSZs allocated for the maximum queue name length, they will
be filled on successfull completion with queue/file names.
Call w/o the string parameters if you are not interested in getting
queue/file names.
Call with strings allocated but empty to select the default queue and
getting its name.

void MavPrint::cleanupPrinter()
frees all resources allocated by setupPrinter.

After you have selected a printer you must open a print job before drawing.
This is done by
Boolean MavPrint::openPrinterJob(PSZ pszJobTitle)
that creates a suitable HPS and HDC you'll be able to retrieve using
HDC MavPrint::getHDC()
HPS MavPrint::getHPS()


Remember to call
void MavPrint::closePrinterJob()
to close HPS and HDC, free resources and send job content to the printer.

or

void MavPrint::abortPrinterJob()
to close HPS and HDC, free resources and ABORT job.

Then define margins using
Boolean MavPrint::setMargins(LONG lxLeft, LONG lxRight,
LONG lyTop, LONG lyBottom)
This will set data needed by the conversion functions

PPOINTL MavPrint::coordsInHeader(PPOINTL p1, PPOINTL p2);
PPOINTL MavPrint::coordsInFooter(PPOINTL p1, PPOINTL p2);
PPOINTL MavPrint::coordsInMargins(PPOINTL p1, PPOINTL p2);
PRECTL MavPrint::rectlInMargins(PRECTL r1, PRECTL r2);

You will use these functions as all the MavPrint drawing functions work in
world coordinates (as if no margin was defined). You probably like more
to think to points in the header/margin/footer frame, so conversions are
required. For example imagine you want to print a string at the lower-left
corner of your header. You store the coords (0,0) in a POINTL p2, declare
a buffer POINTL p1 and call

if(! printer.printStringAt( printer.coordsInHeader(&p2, &p1), string ) ) { Error(); }

In the same way you can call the other conversion functions for Footer and
margins and the rectangle conversion function for margins.

Other useful function for page size and margins handling are the following:

RECTL * MavPrint::getMargins()
SIZEL * MavPrint::getMarginedPageSize()
SIZEL * MavPrint::getPageSize()
SIZEL * MavPrint::getHeaderSize()
SIZEL * MavPrint::getFooterSize()

which simply return the sizes of the various ereas in the logical page.

We're now ready for drawing text and graphics without being concerned with
frames and physical printers. All relevant data have been encapsulated in
the previously defined functions.

If, for special needs, you want to access to the actual printer data you can
retrieve the DEVOPENDATA struct using

PDEVOPENDATA MavPrint::getDevOpenData()

For example I needed this function programming for Workplace Shell, where
the wpPrintPrinterSpecificFile function requires a structure containing data
I retrieved from DEVOPENDATA.

All graphic drawing function can now be done calling the PM Gpi APIs,
supplying as a parameter the HPS you can get calling MavPrint::getHPS().

MAVCL provides methods for drawing text and handle fonts.

LONG MavPrint::createFontMatching(PSZ pszFacename, LONG lPointSize,
ULONG fl, PBOOL pfScalable, PSIZEF psizfxBox)

is used to create a font. An image font
is first searched for and if one is not found that matches the
font face and size, a scalable font is then searched.
When calling this function MavPrint returns a non-zero LONG (zero is returned
on error) that you can store in a local variable for the following font
selection (by the way this LONG corresponds to the PM lLcId). So the
typical use of this function is, as in the SAMPLE program, to build a set of
fonts after job start. Each font can then be selected for actual drawing calling
MavPrint::selectLogFont. When you create a font it is also selected,
so you can use it without calling MavPrint::selectLogFont.
pszFacename is a string containing face name (for ex. "Helvetica");
lPointSize is the required nominal size of the font (for ex. 12). It is
always expressed in points, independent from the world coordinates
unit of measure selected for the presentation space.
The optional parameter fl can be used to ask for font attributes, as italic
or bold, refer to FATTRS structure fsSelection field for allowed values.
The optional parameter pfScalable is a pointer to a boolean that is
returned as TRUE if the created font is scalable or false.
The optional parameter psizfxBox is a pointer to a SIZEF structure
that is filled with the size of the selected font.

Boolean MavPrint::selectLogFont(LONG lLcId, PSIZEF psizfxBox)

This function selects a font previously created with
MavPrint::createFontMatching.
lLcId is the id of the font returned by MavPrint::createFontMatching.
The optional parameter psizfxBox is a pointer to a SIZEF structure
that is filled with the size of the selected font.

Boolean MavPrint::printStringAt(PPOINTL pnt, PSZ string, LONG len)
Boolean MavPrint::printStringAt(PPOINTL pnt, IString & string, LONG len)

These functions print string (either PSZ or IString) at the point
pnt in the selected font. If the optional parameter len is supplied
the string is clipped to the specified len.
Both pnt and len are expressed in world coords units of measure.
pnt is in page coords. Use conversion functions to use
margins/header/footer coordinates as shown above.


Boolean MavPrint::printTextInBox(PRECTL prct,
PSZ string, LONG & lWordsNotWritten, USHORT usShrink,
LONG lControl, LONG lRound)

Boolean MavPrint::printTextInBox(PRECTL prct,
IString & string, LONG & lWordsNotWritten, USHORT usShrink,
LONG lControl, LONG lRound)

These functions print a text inside the rectangle area pointed by prct.
in the selected font. prct is expressed in world coords units of measure.
prct is in page coords. Use conversion functions to use
margins coordinates as shown above.
CRs (\n) are taken into account to split lines.
if text does not fit lWordsNotWritten returns the number of words left out.
On exit prct contains the rectangle actually used
that may be less than maximum allowed in the passed prct rectangle
depending on text length and on usShrink.
In fact  in this case rectangle is lowered.
lControl is the parameter of GpiBox (default 0)
lRound is the corner rounding axis of an ellipse as in GpiBox  (default 0)
usShrink = 0: do not shrink rectangle round text
usShrink = 1: shrink rectangle round text  (default)
usShrink = 2: shrink rectangle round text only vertically
These are the most flexible text print functions of MAVCL, as they allow
to print in a fixed page region, handle text split, can show the rectangular
area if lControl = DRO_OUTLINE.
If the area if too small to fit passed string text the functions return the
number of worlds that did not fit, so that the programmer can perform
the relative actions (as change page and call the function again with the
remaining text).

Boolean MavPrint::printBitmapInBox(RECTL * rectl, PSZ pszFileName,
Boolean booMaintainAspectRatio)

These functions print a text inside the rectangle area pointed by prct.
prct is expressed in world coords units of measure.
prct is in page coords. Use conversion functions to use
margins coordinates as shown above.
The bitmap is idetintified by its file name.
The function can shrink or expand the bitmap to fill the passed rectangular
region.
if booMaintainAspectRatio is set to true the bitmap is sshrinked or expanded
to fit the supplied area, but if the area horizontal/vertical size ratio is
not equal to the bitmap ratio the bitmap is drawn at the lower-left corner
of prct and its apect ratio maintained.

Boolean MavPrint::newPage()
Changes page.


3.3 Date/Number conversions to/from string

you must include: MAVCONV.HPP
source code: MAVCONV.CPP

3.3.1 Description

This is not a class, it is a collection of functions you can use to convert
date/numbers to/from strings taking into account national settings of OS/2
to format and check variables.

For dates all of  " ,/-.=*_:" characters are valid
separators.

3.3.2 Main methods

IDate StringToDate(const IString & temps1)

IString DateToString(const IDate & d, const Boolean booNoYear)
booNoYear = true makes year digits not to be swown.
booFullYear = if true 4 digits swown for year (1987) else 2 = default (87).

double StringToDouble(IString & temps)

IString DoubleToString(const double d, const Boolean booTh)
booTh = true makes thousands separators to be swown.


3.4 regular expressions

you must include: MAVREGEX.HPP
source code: MAVREGEX.CPP

3.4.1 Description

3.4.2 Main methods

Constructor:

RegEx::RegEx(istream & s1, ostream & s2, IString& strFlags);
s1 and s2 are input and output streams.
s2 will be filled after process() call.
for strFlags see MAVREGEX.CPP.

void RegEx::process(IString & strRegex);
fills the output stream with the matching line.

3.5 directory services

you must include: MAVDRSR.HPP
source code: MAVDRSR.CPP

3.5.1 Description

This is a very simple class that returns the files in a directory.

3.5.2 Main methods

Constructor:

DirServ::DirServ(IString & str1)
str1 is the directory name.

IString DirServ::getNext()
returns the next file in the directory.

INT DirServ::errorCode()
read error code.

4. WHAT'S NEW

ver 1.3

- OS/2 Warp 4.0 supported
- abortPrinterJob method added
- number of copies in setupPrinter method added

ver 1.2

- Fomatted entry fields integration in Visual Builder

- Minor bug fixed in time and date entry fields: retyping on the
first part of the field caused a wrong cursor placement at the end
of the field.

ver 1.1

- Visualage C++ compiler ver. 3.01 support

5. REGISTRATION

Please read REGISTER.DOC and LICENSE.TXT

6. CONTACTING THE AUTHOR

If you use MAVCL in your program(s) please send your comments to the
address below using the form in REGISTER.DOC.

Your help and encouragement will ensure future versions of MAVCL.

Fabrizio AVERSA
Via Tangorra 1
Rome 00191
Italy
Tel/Fax +39-6-3297747
internet: faversa@mbox.vol.it

