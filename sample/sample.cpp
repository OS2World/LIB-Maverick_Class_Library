/*  File: sample.cpp      Updated: Sat Aug 19 09:25:49 1995
Copyright (c) Fabrizio Aversa
===========================================================*/

#define INCL_DEV
#define INCL_PM
#define INCL_DOS
#define INCL_SPLDOSPRINT
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iapp.hpp>                     //IApplication Class
#include <iframe.hpp>                   //IFrameWindow Class
#include <icmdhdr.hpp>                  //ICommandHandler & ICommandEvent
#include <istattxt.hpp>                 //IStaticText Class
#include <istring.hpp>                  //IString Class
#include <idate.hpp>                  //IString Class
#include <ithread.hpp>
#include <ipushbut.hpp>

#include <mavprint.hpp>
#include <mavconv.hpp>
#include <mavfrent.hpp>

#define WND_MAIN         5000           //Main Window Id
#define MI_CLOSE         5001           //Command Id
#define IDM_PRINT         5002           //Command Id
#define IDM_PRINTERSETUP         5003           //Command Id
#define IDE_BUT         5004

class AWindow : public IFrameWindow,    //Define AWindow Class from
public ICommandHandler  //  IFrameWindow & ICommandHandler
{

   private:
   MavPrint printer;
   LONG Test();
   IStaticText * text1, * text2, *text3, *text4;
   NumEntryField * entry1;
   TimeEntryField * entry2;
   DateEntryField * entry3;
   AlphaEntryField * entry4;
   IPushButton * iBut;

   public:
   AWindow(unsigned long);
   ~AWindow();
   virtual  Boolean command(ICommandEvent&);
};


AWindow :: ~AWindow()
{
   printer.cleanupPrinter();
   /* ALWAYS DELETE CREATED FIELDS */
   delete text1;
   delete text2;
   delete text3;
   delete text4;
   delete entry1;
   delete entry2;
   delete entry3;
   delete entry4;
   delete iBut;
}

AWindow :: AWindow(unsigned long windowId)       //Define AWindow Constructor
: IFrameWindow (                    //Call IFrameWindow constructor
IFrameWindow::defaultStyle()      //  Use default styles plus
| IFrameWindow::menuBar,          //  Get Menu Bar from Resource File
windowId)                         //  Main Window ID
{
   handleEventsFor(this);

   /* create static texts and enry fields */

   ISize siz(240,25);
   IPoint pnt(30, 250);

   text1=new IStaticText(100, this, this, IRectangle(pnt, siz));
   text1->setText("Numeric");

   pnt.setY(pnt.y() - 50);
   text2=new IStaticText(101, this, this, IRectangle(pnt, siz));
   text2->setText("Time");

   pnt.setY(pnt.y() - 50);
   text3=new IStaticText(102, this, this, IRectangle(pnt, siz));
   text3->setText("Date");

   pnt.setY(pnt.y() - 50);
   text4=new IStaticText(103, this, this, IRectangle(pnt, siz));
   text4->setText("Alphanumeric");

   pnt.setX(290);
   pnt.setY(250);

   entry1=new NumEntryField(200, this, this, IRectangle(pnt, siz), true, true, 1, 2e5);

   pnt.setY(pnt.y() - 50);
   entry2=new TimeEntryField(201, this, this, IRectangle(pnt, siz));

   pnt.setY(pnt.y() - 50);
   entry3=new DateEntryField(202, this, this, IRectangle(pnt, siz));

   pnt.setY(pnt.y() - 50);
   entry4=new AlphaEntryField(203, this, this, IRectangle(pnt, siz), false, true);

   pnt.setY(pnt.y() - 50);

   ISize sizBut(90,40);
   iBut= new IPushButton(IDE_BUT, this, this,
   IRectangle(pnt, sizBut), IPushButton::defaultStyle() |
   IControl::tabStop);
   iBut->setText("Test");

   /* get default printer */
   printer.setHAB(IThread::current().anchorBlock());
   printer.setWorldCoords(PU_LOMETRIC);
   char psz1[80]="", psz2[80]="";
   printer.setupPrinter(psz1, psz2);

   /* now psz1, psz2 contain printer and file names */

   show();

   entry1->setFocus();

} /* end AWindow :: AWindow(...) */


Boolean AWindow :: command(ICommandEvent& cEvent)//Define command member function
{
    IMessageBox msgbox(this);
   msgbox.setTitle("MAVCL");

   switch (cEvent.commandId())
   {

      case IDE_BUT:

      text1->setText(entry1->text());
      text2->setText(entry2->text());
      text3->setText(entry3->getTextWithDefault());
      text4->setText(entry4->text());

      return true;

      case IDM_PRINTERSETUP:

      printer.setupPrinter(this);

      return true;


      case IDM_PRINT:

   if (msgbox.show( "Print Test ?",
   IMessageBox::yesNoButton | IMessageBox::defButton2
   | IMessageBox::queryIcon | IMessageBox::moveable) != IMessageBox::yes) return true;

      WinSetPointer(HWND_DESKTOP,
      WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE));

      if(!Test()) {
         WinSetPointer(HWND_DESKTOP,
         WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));

         printer.closePrinterJob();

         IMessageBox msgbox(this);
         msgbox.setTitle("Print Job");
         msgbox.show("Print Error",
         IMessageBox::okButton | IMessageBox::warningIcon | IMessageBox::moveable);

      }
      WinSetPointer(HWND_DESKTOP,
      WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));

      return true;

      case MI_CLOSE:
      close();
      return true;

   };
   return false;                       //Return Command not Processed
} /* end AWindow :: command(...) */


void main()                             //Main Procedure with no parameters
{
   AWindow mainWindow(WND_MAIN);         //Create main window on the desktop
   IApplication::current().run();        //Get current application & start runnig
} /* end main */

LONG AWindow :: Test()
{

   POINTL pointl, p1, p2;
   SIZEL sizePage;
   LONG fntHelv14Bold, fntHelv12Bold, fntHelv12, fntHelv10, fntHelv8, fntHelv12Italic, fntTimes8;
   RECTL rec, rec1;
   LONG lngNotWritten;
   USHORT usNoPag = 1;
   USHORT i;
   IString str;
   Boolean booPrintHeader = TRUE;

   if(!printer.openPrinterJob("MAVCL test")) return FALSE;

   HPS hps = printer.getHPS();

   if(!GpiSetTextAlignment(hps, TA_NORMAL_HORIZ, TA_NORMAL_VERT)) return FALSE;

   if(! ( fntHelv14Bold = printer.createFontMatching("Helvetica", 14, FATTR_SEL_BOLD)) ) return FALSE;
   if(! ( fntHelv12Bold = printer.createFontMatching("Helvetica", 12, FATTR_SEL_BOLD)) ) return FALSE;
   if(! ( fntHelv12Italic = printer.createFontMatching("Helvetica", 12, FATTR_SEL_ITALIC)) ) return FALSE;
   if(! ( fntHelv12 = printer.createFontMatching("Helvetica", 12)) ) return FALSE;
   if(! ( fntHelv10 = printer.createFontMatching("Helvetica", 10)) ) return FALSE;
   if(! ( fntHelv8 = printer.createFontMatching("Helvetica", 8)) ) return FALSE;
   if(! ( fntTimes8 = printer.createFontMatching("Times New Roman", 8)) ) return FALSE;

   if(!printer.setMargins(150, 150, 200, 200)) return FALSE;

   sizePage = *(printer.getMarginedPageSize());

   if(! printer.selectLogFont( fntHelv12 ) ) return FALSE;

   if(! printer.selectLogFont( fntHelv12 ) ) return FALSE;
   LONG lineHeigth = printer.getPFM()->lMaxBaselineExt;

   LONG lTab[8];
   lTab[0] = 0;
   lTab[1] = sizePage.cx * 0.08;
   lTab[2] = lTab[1] + sizePage.cx * 0.08;
   lTab[3] = lTab[2] + sizePage.cx * 0.062;
   lTab[4] = lTab[3] + sizePage.cx * 0.3;
   lTab[5] = lTab[4] + sizePage.cx * 0.16;
   lTab[6] = lTab[5] + sizePage.cx * 0.16;
   lTab[7] = sizePage.cx;

   IString strIntest[7];
   strIntest[0] = "Date ";
   strIntest[1] = "Date 1";
   strIntest[2] = "Doc.";
   strIntest[3] = "Description";
   strIntest[4] = "$$";
   strIntest[5] = "$$";
   strIntest[6] = "Total";

   LONG lHeaderLines = 4;

   pointl.x  = 0;
   pointl.y = sizePage.cy - lineHeigth * lHeaderLines;

   for(int j = 1 ; j< 100; j++) {

      if(booPrintHeader) {

         /* reset booPrintHeader */
         booPrintHeader = FALSE;

         IString str;

         /* define box */
         rec.xLeft = 0;
         rec.yTop = sizePage.cy;
         rec.xRight = sizePage.cx;
         rec.yBottom = sizePage.cy - lineHeigth * 1.5;
         /* convert to worl coords */
         printer.rectlInMargins(&rec, &rec1);

         if(!GpiSetTextAlignment(hps, TA_CENTER, TA_NORMAL_VERT)) return FALSE;

         /* intestazione generale */
         str = "What a test !";
         if(! printer.selectLogFont( fntHelv12Bold ) ) return FALSE;
         if(! printer.printTextInBox(&rec1,
         str, lngNotWritten, FALSE, DRO_OUTLINE ) ) return FALSE;

         /* intestazione colonne */
         if(! printer.selectLogFont( fntHelv10 ) ) return FALSE;
         rec.yTop = rec1.yBottom - printer.getMargins()->yBottom;
         rec.yBottom = rec.yTop - lineHeigth * 1.5;

         for(i = 0 ; i < 7 ; i++) {

            rec.xLeft = lTab[i];
            rec.xRight = lTab[i+1];

            printer.rectlInMargins(&rec, &rec1);

            if(! printer.printTextInBox(&rec1,
            strIntest[i], lngNotWritten, FALSE, DRO_OUTLINE ) ) return FALSE;

         }

         /* HEADER */

         /* bitmap (client logo) */
         rec.xLeft= 10;
         rec.yBottom= printer.getPageSize()->cy - 190;
         rec.xRight= 170;
         rec.yTop= printer.getPageSize()->cy-10;
         if(! printer.printBitmapInBox(&rec, "os23.bmp", TRUE ) ) return FALSE;
         if(! printer.selectLogFont( fntHelv14Bold ) ) return FALSE;

         p2.x = printer.getHeaderSize()->cx/2;
         p2.y = printer.getHeaderSize()->cy/2;
         str = "Maverick classes by Fabrizio Aversa";
         if(!GpiSetTextAlignment(hps, TA_CENTER, TA_NORMAL_VERT)) return FALSE;
         if(! printer.printStringAt( printer.coordsInHeader(&p2, &p1), str ) )
         return FALSE;

         /* line (world coords) */
         p2.x = printer.getHeaderSize()->cx * 0.2;
         p2.y = printer.getPageSize()->cy - printer.getHeaderSize()->cy * 0.6;
         p1.x = printer.getHeaderSize()->cx * 0.9;
         p1.y = p2.y;
         GpiMove(hps, &p2);
         GpiLine(hps, &p1);

         /* FOOTER */
         if(! printer.selectLogFont( fntTimes8 ) ) return FALSE;

         p2.x = printer.getFooterSize()->cx/2;
         p2.y = printer.getFooterSize()->cy/2;
         IDate today;
         today.today();
         str = "Pag. " + IString(usNoPag);
         if(!GpiSetTextAlignment(hps, TA_CENTER, TA_NORMAL_VERT)) return FALSE;
         if(! printer.printStringAt( printer.coordsInFooter(&p2, &p1), str ) )
         return FALSE;

         p2.x = printer.getFooterSize()->cx * 0.95;
         p2.y = printer.getFooterSize()->cy/2;
         str = DateToString(today, false);
         if(!GpiSetTextAlignment(hps, TA_RIGHT, TA_NORMAL_VERT)) return FALSE;
         if(! printer.printStringAt( printer.coordsInFooter(&p2, &p1), str ) )
         return FALSE;

         p2.x = printer.getFooterSize()->cx * 0.05;
         p2.y = printer.getFooterSize()->cy/2;
         str = "Mavcl v. 1.00";
         if(!GpiSetTextAlignment(hps, TA_NORMAL_HORIZ, TA_NORMAL_VERT)) return FALSE;
         if(! printer.printStringAt( printer.coordsInFooter(&p2, &p1), str ) )
         return FALSE;

         /* line (world coords) */
         p2.x = printer.getFooterSize()->cx * 0.2;
         p2.y = printer.getFooterSize()->cy * 0.8;
         p1.x = printer.getFooterSize()->cx * 0.9;
         p1.y = p2.y;
         GpiMove(hps, &p2);
         GpiLine(hps, &p1);

      } /* end of if booPrintHeader */


      /* print patient row */

      if(! printer.selectLogFont( fntHelv8 ) ) return FALSE;

      IString strRow[10];
      strRow[0] = "03/01/95";
      strRow[1] = "01/01/95";
      strRow[2] = "1027";
      strRow[4] = "1,500,000";
      strRow[5] = "0";
      strRow[6] = "1,200,000";
      /* change content after 1/4 page */
      if(pointl.y < sizePage.cy / 4) {
         strRow[3] = "This is still a silly test\nbut on\nthree lines";
      } else {
         strRow[3] = "This is a silly test";
      }


      lngNotWritten = 999;

      while (lngNotWritten) {

         rec.yTop = pointl.y + printer.getPFM()->lMaxBaselineExt * 0.3;
         rec.yBottom = 0;

         LONG lngNotWr[11];
         lngNotWritten = 0;

         for(i = 0 ; i < 7 ; i++) {

            rec.xLeft = lTab[i];
            rec.xRight = lTab[i+1];

            if( i == 4 || i == 5 || i == 6)
            if(!GpiSetTextAlignment(hps, TA_RIGHT, TA_NORMAL_VERT)) return FALSE;

            if(! printer.printTextInBox(printer.rectlInMargins(&rec, &rec1),
            strRow[i], lngNotWr[i], 2 ) ) return FALSE;

            if( i == 4 || i == 5 || i == 6)
            if(!GpiSetTextAlignment(hps, TA_NORMAL_HORIZ, TA_NORMAL_VERT)) return FALSE;

            lngNotWritten += lngNotWr[i];

            /* see which box goes lower */
            LONG lsize = rec1.yBottom - printer.getMargins()->yBottom;
            if( lsize < pointl.y ) pointl.y = lsize;

         }

         /* manage partially written boxes */
         if(lngNotWritten) {

            pointl.x  = 0;
            pointl.y = sizePage.cy - lineHeigth * lHeaderLines;

            printer.newPage();

            booPrintHeader = TRUE;

            usNoPag++;

            /* trim strings */
            for(i = 0 ; i < 7 ; i++) {
               if(lngNotWr[i]) {
                  strRow[i] = strRow[i].words(
                  strRow[i].numWords() - lngNotWr[i] + 1, lngNotWr[i]);
               } else {
                  strRow[i] = "-";
               }
            }
         }
      } /* end while lngNotWritten */


      pointl.x = 0;

      /* manage form feed */
      if(pointl.y < 0 ) {

         pointl.x  = 0;
         pointl.y = sizePage.cy - lineHeigth * lHeaderLines;

         printer.newPage();

         booPrintHeader = TRUE;

         usNoPag++;

      }

   } /* end of while on queryarray */

   /* totals */

   LONG lngNotWr[11];
   IString strRow[10];
   strRow[0] = "";
   strRow[1] = "";
   strRow[2] = "";
   strRow[3] = "Subtotals (invented)";
   strRow[4] = "1,178,248,837";
   strRow[5] = "0";
   strRow[6] = "1,178,248,837";

   for(j = 0 ; j < 2 ; j++)
   {
      if( j == 1)
      {
         strRow[3] = "Totals (invented)";
         strRow[4] = "2,778,248,837";
         strRow[5] = "1,879,765,456";
         strRow[6] = "1,178,248,837";

         rec.yTop = pointl.y ;
         rec.yBottom = rec.yTop - lineHeigth * 1;
      }
      else
      {
         rec.yTop = pointl.y - lineHeigth * 1;
         rec.yBottom = rec.yTop - lineHeigth * 1;
      }
      for(i = 3 ; i < 7 ; i++) {

         rec.xLeft = lTab[i];
         rec.xRight = lTab[i+1];

         if(!GpiSetTextAlignment(hps, TA_RIGHT, TA_NORMAL_VERT)) return FALSE;

         if(! printer.printTextInBox(printer.rectlInMargins(&rec, &rec1),
         strRow[i], lngNotWr[i], FALSE, DRO_OUTLINE  ) ) return FALSE;

         if(!GpiSetTextAlignment(hps, TA_NORMAL_HORIZ, TA_NORMAL_VERT)) return FALSE;

         lngNotWritten += lngNotWr[i];

         /* see which box goes lower */
         LONG lsize = rec1.yBottom - printer.getMargins()->yBottom;
         if( lsize < pointl.y ) pointl.y = lsize;

      }
   }

   printer.closePrinterJob();

   return TRUE;

}


