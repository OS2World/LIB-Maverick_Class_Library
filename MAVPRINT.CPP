/*  File: MAVPRINT.CPP      Updated: Tue Aug 15 15:54:33 1995
Copyright (c) Fabrizio Aversa
===========================================================*/
#define INCL_DEV
#define INCL_PM
#define INCL_SPLDOSPRINT
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <IString.hpp>

#include <iframe.hpp>

#include "mavprint.hpp"

MavPrint::MavPrint()
{
   pSetup = new PRINTERSETUP;
   memset(pSetup, '\0', sizeof(PRINTERSETUP));

   usFontSize = new USHORT[255];
   memset(usFontSize, '\0', 255 * sizeof(USHORT));

   lLcIdCount = 0;

   rctMargins.xLeft = 0;
   rctMargins.xRight = 0;
   rctMargins.yTop = 0;
   rctMargins.yBottom = 0;

   sizMarginedPage.cx = 0;
   sizMarginedPage.cy = 0;

   sizPage.cx = 0;
   sizPage.cy = 0;

   sizHeader.cx = 0;
   sizHeader.cy = 0;

   sizFooter.cx = 0;
   sizFooter.cy = 0;

}

MavPrint::~MavPrint()
{
   delete pSetup;
   delete usFontSize;
}

Boolean MavPrint::setMargins(LONG lxLeft, LONG lxRight, LONG lyTop, LONG lyBottom)
/* set margins, checks that margins are smaller than the page itself / 2 */
{

   sizMarginedPage.cx = 0;
   sizMarginedPage.cy = 0;

   sizPage.cx = 0;
   sizPage.cy = 0;

   if(!GpiQueryPS( hps, &sizPage )) return FALSE;

   /* check if margins are too big */
   if (
   sizPage.cx / 2 - lxRight - lxLeft < 0
   ||
   sizPage.cy / 2 - lyTop - lyBottom < 0
   ) return FALSE;

   rctMargins.xLeft = lxLeft;
   rctMargins.xRight = lxRight;
   rctMargins.yTop = lyTop;
   rctMargins.yBottom = lyBottom;

   sizMarginedPage.cx = sizPage.cx - rctMargins.xLeft - rctMargins.xRight;
   sizMarginedPage.cy = sizPage.cy - rctMargins.yTop - rctMargins.yBottom;

   /* header and footer sizes are defined as the margins */
   sizHeader.cx = sizMarginedPage.cx;
   sizHeader.cy = rctMargins.yTop - 1;

   sizFooter.cx = sizMarginedPage.cx;
   sizFooter.cy = rctMargins.yBottom - 1;

   return TRUE;
}

Boolean MavPrint::openPrinterJob(PSZ pszJobTitle)
{
   hdc = DevOpenDC( pSetup->hab, pSetup->lDCType, "*",
   9, pSetup->pDevOpenData, (HDC)0 );

   if ( hdc == NULLHANDLE )
   return FALSE;

   SIZEL sizel;

   sizel.cx = 0;
   sizel.cy = 0;
   hps = GpiCreatePS( pSetup->hab,
   hdc,
   &sizel,
   pSetup->lWorldCoordinates | GPIF_DEFAULT | GPIT_NORMAL | GPIA_ASSOC );

   if( GPI_ERROR ==  hps ) {
      DevCloseDC( hdc );
      hdc = (HDC)0;
      hps = (HPS)0;
      return FALSE;
   }

   DevEscape(hdc, DEVESC_STARTDOC,
   strlen(pszJobTitle),
   pszJobTitle,
   NULL,
   NULL);

   /* default margins 0 */
   if(!setMargins(0, 0, 0, 0)) return FALSE;

   if(!GpiSetCharMode(hps, CM_MODE2)) return FALSE;
   if(!GpiSetTextAlignment(hps, TA_NORMAL_HORIZ, TA_NORMAL_VERT)) return FALSE;

   return TRUE;
}

void MavPrint::abortPrinterJob()
{
   DevEscape( hdc, DEVESC_ABORTDOC, 0, NULL, NULL, NULL );

   if( hps )
   {
      GpiSetCharSet( hps, 0 );
      while (lLcIdCount) GpiDeleteSetId( hps, lLcIdCount-- );
      GpiAssociate( hps, (HDC)0 );
      GpiDestroyPS( hps );
      hps = (HPS)0;
   }

   if( hdc )
   {
      DevCloseDC( hdc );
      hdc = (HDC)0;
   }

}

void MavPrint::closePrinterJob()
{

   /* end doc */
   DevEscape( hdc, DEVESC_ENDDOC, 0, NULL, NULL, NULL );

   /* Close DC and PS */

   if( hps )
   {
      GpiSetCharSet( hps, 0 );
      while (lLcIdCount) GpiDeleteSetId( hps, lLcIdCount-- );
      GpiAssociate( hps, (HDC)0 );
      GpiDestroyPS( hps );
      hps = (HPS)0;
   }

   if( hdc )
   {
      DevCloseDC( hdc );
      hdc = (HDC)0;
   }

}

Boolean MavPrint::newPage()
{
   if(hdc) {
      if(
      DevEscape( hdc, DEVESC_NEWFRAME, 0, NULL, NULL, NULL ) == DEV_OK
      ) return TRUE;
   }

   return FALSE;

}


PRECTL MavPrint::rectlInMargins(PRECTL recSource, PRECTL rec)
/* recSource is in margin coords, it is returned in page coords */
{
   rec->xLeft = recSource->xLeft + rctMargins.xLeft;
   rec->xRight = recSource->xRight + rctMargins.xLeft;
   rec->yTop = recSource->yTop + rctMargins.yBottom;
   rec->yBottom = recSource->yBottom + rctMargins.yBottom;

   return rec;
}

PPOINTL MavPrint::coordsInMargins(PPOINTL pntSource, PPOINTL pnt)
/* pnt1 is in margin coords, it is returned in page coords */
{
   pnt->x = pntSource->x + rctMargins.xLeft;
   pnt->y = pntSource->y + rctMargins.yBottom;

   return pnt;
}

PPOINTL MavPrint::coordsInHeader(PPOINTL pntSource, PPOINTL pnt)
/* pnt1 is in header coords, it is returned in page coords */
{
   pnt->x = pntSource->x + rctMargins.xLeft;
   pnt->y = pntSource->y + sizPage.cy - rctMargins.yTop;

   return pnt;
}

PPOINTL MavPrint::coordsInFooter(PPOINTL pntSource, PPOINTL pnt)
/* pnt1 is in footer coords, it is returned in page coords */
{
   pnt->x = pntSource->x + rctMargins.xLeft;
   pnt->y = pntSource->y;

   return pnt;
}

Boolean MavPrint::printStringAt(PPOINTL pnt, PSZ string, LONG len)
/* print a string at a point given in page coords
if len is given (world coords) it is used to clip text */
{
   IString str = string;
   return printStringAt(pnt, str, len);
}

Boolean MavPrint::printStringAt(PPOINTL pnt, IString & string, LONG len)
/* print a string at a point given in page coords
if len is given (world coords) it is used to clip text */
{
   /* coords check (rough) */
   if (
   pnt->y < 0 || pnt->y > sizPage.cy ||
   pnt->x < 0 || pnt->x > sizPage.cx
   ) {
      return FALSE;
   }

   if (len == 0) {

      if(GpiCharStringAt( hps, pnt, string.length(), (PSZ)string ) != GPI_OK) {
         return FALSE;
      }

   } else {

      /* define clipping region (bigger than needed at left-top-bottom) */
      RECTL rec;
      rec.xLeft = pnt->x  - len; /* string may be aligned center or right */
      rec.yBottom = pnt->y  - fm.lMaxBaselineExt;
      rec.xRight = pnt->x + len;
      rec.yTop = pnt->y + fm.lMaxBaselineExt;

      if(rec.xLeft < 0) rec.xLeft = 0;
      if(rec.yBottom < 0) rec.yBottom = 0;
      if(rec.yTop > sizPage.cy) rec.yTop = sizPage.cy;
      if(rec.xRight > sizPage.cx) rec.xRight = sizPage.cx;

      if(GpiCharStringPosAt( hps, pnt, &rec, CHS_CLIP,
      string.length(), (PSZ)string, NULL ) != GPI_OK) {
         return FALSE;
      }

   } /* endif */

   return TRUE;

}

Boolean MavPrint::printTextInBox(PRECTL prct,
PSZ string, LONG & lWordsNotWritten, USHORT usShrink,
LONG lControl, LONG lRound)
{
   IString str = string;
   return printTextInBox(prct, str, lWordsNotWritten, usShrink,
   lControl, lRound);
}

Boolean MavPrint::printTextInBox(PRECTL prct,
IString & string, LONG & lWordsNotWritten, USHORT usShrink,
LONG lControl, LONG lRound)
/*
print a text in a rectangle.
\n are considered.
if text does not fit lWordsNotWritten returns the number of words left out.
On exit prct contains the area used (may be less than maximum allowed
depending on text length, in this case rectangle is lowered)
lControl is the parameter of GpiBox
lRound is the corner rounding axis of an ellipse as in GpiBox
All coords in world coords
usShrink = 0: do not shrink rectangle round text
usShrink = 1: shrink rectangle round text
usShrink = 2: shrink rectangle round text only vertically
*/
{

   /* coords check */
   if (
   prct->yBottom >= prct->yTop || prct->xLeft >= prct->xRight ||
   prct->yBottom < 0 || prct->yTop > sizPage.cy ||
   prct->xRight < 0 || prct->xLeft > sizPage.cx
   ) {
      return FALSE;
   }

   POINTL pnt;
   LONG lMargin = (prct->xRight - prct->xLeft) / 30;

   if(string.strip().length()) {

      LONG lAvailableLineLen = prct->xRight - prct->xLeft -2*lMargin;

      /* pnt is beginning of line (x never changes but depends on alignment
      if box is not shrinkable) */
      LONG lHorizAlignment, lVertAlignment;
      if(!GpiQueryTextAlignment(hps, &lHorizAlignment, &lVertAlignment)) return FALSE;

      /* default left margin */
      pnt.x = lMargin + prct->xLeft;

      if(usShrink != 1 && lHorizAlignment != TA_NORMAL_HORIZ) {

         switch(lHorizAlignment) {

            case TA_RIGHT:
            pnt.x = prct->xRight - lMargin;
            break;

            case TA_CENTER:
            pnt.x = prct->xLeft + (prct->xRight - prct->xLeft) / 2;
            break;

         }

      }

      /* top margin = margin + first line space */
      pnt.y = prct->yTop - fm.lMaxBaselineExt;

      IString strLine = "";

      USHORT usNewLineRequest;
      USHORT i = 1;
      LONG lMaxLineWidth = 0;
      LONG lLineWidth;

      for(;;) {

         strLine +=  string.word(i);

         /* copy separators and see if new line
         is requested (\n) at the end of this word */
         USHORT index = string.indexOfWord(i) + string.word(i).length();

         usNewLineRequest = 0;
         while (index < string.indexOfWord(i+1)) {

            if ( string[index] == 13 || string[index] == '\n') {
               index++;
               usNewLineRequest++;
               if(string[index] == 10) index++;
            }

            /* append separators (spaces) up to 1st cr+lf */
            if(!usNewLineRequest) strLine +=  string[index];

            index ++;

         } /* endwhile */

         /* force new line (print) if end of string */
         if(i == string.numWords()) usNewLineRequest = TRUE;

         POINTL ptlString[TXTBOX_COUNT];

         GpiQueryTextBox(hps,
         strLine.length(), (PSZ)strLine,
         TXTBOX_COUNT,  /* return maximum information */
         ptlString);         /* array of coordinates points in world coordinates */

         if( ptlString[3].x -  ptlString[1].x > lAvailableLineLen
         || usNewLineRequest) {
            /* string exeedes available space or new line request */

            if(strLine.numWords() > 1 &&
            ptlString[3].x -  ptlString[1].x > lAvailableLineLen) {
               /* strip last word, if more than one present in strLine */

               i--;

               strLine =strLine.words(1, strLine.numWords() - 1);

               /* cancel any newline request that would be shifted up the
               word we are clipping */
               usNewLineRequest = 0;
            }

            /* calc line width */
            GpiQueryTextBox(hps,
            strLine.length(), (PSZ)strLine,
            TXTBOX_COUNT,  /* return maximum information */
            ptlString);         /* array of coordinates points in world coordinates */

            /* update  lMaxLineWidth */
            if( (lLineWidth = ptlString[3].x -  ptlString[1].x) > lMaxLineWidth)
            lMaxLineWidth = lLineWidth;

            if(! printStringAt(&pnt, strLine, lAvailableLineLen ) ) return FALSE;

            if(i == string.numWords()) {
               break;
            }

            strLine = "";

            LONG lVertMove = fm.lMaxBaselineExt *
            (1 + (usNewLineRequest ? usNewLineRequest -1 : 0));

            if(pnt.y - lVertMove < prct->yBottom + lMargin) break;

            pnt.y -= lVertMove;

         }

         i++;

      }

      lWordsNotWritten = string.numWords() - i;

      if(usShrink == 1) {
         /* horizontal shrink request */
         prct->xRight = prct->xLeft + lMaxLineWidth + lMargin*2;
      }

      if(usShrink != 0) {
         /* update prct->yBottom with actual values after text print */
         prct->yBottom = pnt.y - lMargin;
      }

   } else {

      lWordsNotWritten = 0;

      /* calc a rectangle of the right measures */
      if(usShrink != 0) {
         prct->yBottom = prct->yTop - fm.lMaxBaselineExt;
         if (prct->yBottom < 0) prct->yBottom = 0;
      }

      if(usShrink == 1) {
         /* horizontal shrink request */
         prct->xRight = prct->xLeft+ lMargin*2;
      }

   } /* end of if str len */

   /* draw box if required */
   if (lControl) {

      POINTL pntEnd;

      pnt.x = prct->xLeft;
      pnt.y = prct->yBottom;

      pntEnd.x = prct->xRight;
      pntEnd.y = prct->yTop;

      if(lControl == DRO_FILL || lControl == DRO_OUTLINEFILL ) {
         if(!GpiSetMix(hps, FM_XOR)) return FALSE;
         if(!GpiSetPattern(hps, PATSYM_DENSE8)) return FALSE;
      }

      if(!GpiSetCurrentPosition(hps, &pnt)) return FALSE;

      if(GpiBox(hps, lControl, &pntEnd, lRound, lRound) != GPI_OK) return FALSE;

      if(lControl == DRO_FILL || lControl == DRO_OUTLINEFILL )
      if(!GpiSetMix(hps, FM_DEFAULT)) return FALSE;

   }

   return TRUE;
}

LONG MavPrint::createFontMatching(PSZ pszFacename, LONG lPointSize,
ULONG fl, PBOOL pfScalable, PSIZEF psizfxBox)
/*  This function is used select the requested font. An image font
is first searched for and if one is not found that matches the
font face and size, a scalable font is then searched for */
{
   LONG      cFont;     /* Fonts Count   */
   LONG      lFontsTotal = 0L;    /* Fonts Total Count   */
   LONG      lXDeviceRes;    /* x Device Resolution  */
   LONG      lYDeviceRes;    /* y Device Resolution  */
   PFONTMETRICS pfmSelect;     /* Font Metrics Pointer  */
   FATTRS      fat;     /* Font Attributes   */
   register INT i;      /* Loop Counter   */

   LONG lNominalPointSize = lPointSize * 10;

   /* Get the height of the screen in pels  */

   DevQueryCaps(hdc, CAPS_HORIZONTAL_FONT_RES, 1L, &lXDeviceRes);
   DevQueryCaps(hdc, CAPS_VERTICAL_FONT_RES,   1L, &lYDeviceRes);

   /* Get the number of fonts for the face name */
   /* provided     */

   if ( (cFont = GpiQueryFonts(hps, QF_PUBLIC, pszFacename, &lFontsTotal,
   sizeof(FONTMETRICS), (PFONTMETRICS)NULL)) != 0L )
   {
      /* Allocate space for the font metrics for the */
      /* different font sizes and devices of the font */

      DosAllocMem((PPVOID)(PVOID)&pfmSelect, (ULONG)(sizeof(FONTMETRICS) * cFont),
      PAG_READ | PAG_WRITE | PAG_COMMIT);

      /* Make a pointer for the memory allocated for */
      /* the font metrics and get the font metrics for */
      /* the number of fonts for the face name  */
      /* provided     */

      GpiQueryFonts(hps, QF_PUBLIC, pszFacename, &cFont,
      sizeof(FONTMETRICS), pfmSelect);

      /* Loop through the font metrics returned to */
      /* locate the desired font by matching the x and */
      /* y device resolution of the font and the point */
      /* size      */

      for ( i = 0; i < (INT)cFont; i++ )
      if ( (pfmSelect[i].sXDeviceRes == (SHORT)lXDeviceRes) &&
      (pfmSelect[i].sYDeviceRes == (SHORT)lYDeviceRes) &&
      ((LONG)pfmSelect[i].sNominalPointSize == lNominalPointSize) )
      {
         /* Font found, get the match value to allow the */
         /* exact font to be selected by the calling */
         /* application     */

         memset(&fat, 0, sizeof(FATTRS));
         fat.usRecordLength  = sizeof(FATTRS);
         strcpy(fat.szFacename, pszFacename);
         fat.lMatch        = pfmSelect[i].lMatch;
         fat.fsSelection     = (USHORT)fl;

         DosFreeMem((PVOID)pfmSelect);

         if(pfScalable) *pfScalable = FALSE;
         GpiCreateLogFont(hps, (PSTR8)NULL, lLcIdCount + 1, &fat);
         /* increment local identifier now that GpiCreateLogFont went ok */
         lLcIdCount++;

         /* not scalable */
         usFontSize[lLcIdCount] = 0;

         if(!selectLogFont(lLcIdCount, psizfxBox)) return 0;

         return lLcIdCount;

      }

      /* NOT FOUND, TRY SCALABLE FONT */
      /* Loop through the font metrics returned to */
      /* locate the desired font by matching the x and */
      /* y device resolution == 1000 (means scalable) */

      for ( i = 0; i < (INT)cFont; i++ )
      if ( (pfmSelect[i].sXDeviceRes == 1000) && (pfmSelect[i].sYDeviceRes == 1000) )
      {
         /* Font found, get the match value to allow the */
         /* exact font to be selected by the calling */
         /* application     */

         memset(&fat, 0, sizeof(FATTRS));
         fat.usRecordLength  = sizeof(FATTRS);
         strcpy(fat.szFacename, pszFacename);
         fat.lMatch        = pfmSelect[i].lMatch;
         fat.usCodePage      = 850;
         fat.fsFontUse       = FATTR_FONTUSE_OUTLINE | FATTR_FONTUSE_TRANSFORMABLE;
         fat.fsSelection     = (USHORT)fl;

         DosFreeMem((PVOID)pfmSelect);

         if(pfScalable) *pfScalable = TRUE;
         GpiCreateLogFont(hps, (PSTR8)NULL, lLcIdCount + 1, &fat);
         /* increment local identifier now that GpiCreateLogFont went ok */
         lLcIdCount++;

         /* scalable */
         usFontSize[lLcIdCount] = lNominalPointSize;

         if(!selectLogFont(lLcIdCount, psizfxBox)) return 0;

         return lLcIdCount;

      }

      /* Release the memory allocated for the font */
      /* metrics array     */

      DosFreeMem((PVOID)pfmSelect);
   }

   return(0);
}

Boolean MavPrint::selectLogFont(LONG lLcId, PSIZEF psizfxBox)
/* Font is set to a previously created one and fm (fontmetrics) updated */
{

   LONG lNominalPointSize = usFontSize[lLcId];
   LONG lFontSize;

   if( ! GpiSetCharSet(hps, lLcId) ) return FALSE;

   /* only scalable fonts have usFontSize[i] > 0 as
   set by createlogfont */
   if(lNominalPointSize) {

      switch(pSetup->lWorldCoordinates) {

         case PU_LOMETRIC:
         lFontSize = lNominalPointSize * 2 *254/1440;
         break;

         case PU_HIMETRIC:
         lFontSize = lNominalPointSize * 2 *2540/1440;
         break;

         case PU_LOENGLISH:
         lFontSize = lNominalPointSize * 2 *100/1440;
         break;

         case PU_HIENGLISH:
         lFontSize = lNominalPointSize * 2 *1000/1440;
         break;

         case PU_TWIPS:
         lFontSize = lNominalPointSize * 2;
         break;

         default:
         lFontSize = lNominalPointSize * 2;
         break;

      }

      SIZEF siz;
      siz.cx = siz.cy = MAKEFIXED(lFontSize, 0);

      if(psizfxBox) {
         psizfxBox->cx = psizfxBox->cy = siz.cx;
      }

      if(! GpiSetCharBox( hps, &siz ) ) return FALSE;
   }

   if(! GpiQueryFontMetrics( hps, sizeof( fm ), &fm ) ) return FALSE;

   return TRUE;

}

Boolean MavPrint::printBitmapInBox(RECTL * rectl, PSZ pszFileName,
Boolean booMaintainAspectRatio)
{
   FILESTATUS3 filestatus3;
   HBITMAP hbm, hbm1, hbmOld;
   HFILE hfile;
   PBITMAPFILEHEADER2 pbmpfileheader2;
   PBITMAPFILEHEADER2 pbmpfileheader2Work;
   PBITMAPINFOHEADER2 pbmp2;
   PBYTE pBits;
   PBYTE pBitsAligned;
   ULONG rc, ulAction, cBytes, cScans;
   ULONG cbImageData;
   LONG lRC;
   LONG cxBitmap, cyBitmap;

   /* open bitmap file */
   rc = DosOpen( pszFileName,
   &hfile,
   &ulAction,
   0L,
   FILE_NORMAL,
   FILE_OPEN,
   OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE,
   NULL );
   if( rc != 0 )
   {
      return FALSE;
   }

   /* query file size; returned in filestatus3.cbFile */
   rc = DosQueryFileInfo( hfile, FIL_STANDARD,
   (PVOID)&filestatus3, sizeof( filestatus3 ));
   if(rc) {
      DosClose( hfile );
      return FALSE;
   }

   /* alloc memory for bitmap file read */
   if(!(pbmpfileheader2 =
   (PBITMAPFILEHEADER2) malloc( filestatus3.cbFile ))) {
      DosClose( hfile );
      return FALSE;
   }

   /* read bitmap file into memory */
   rc = DosRead( hfile, (PVOID)pbmpfileheader2, filestatus3.cbFile, &cBytes );
   if( rc != 0  ||  cBytes == 0 )
   {
      free( pbmpfileheader2 );
      DosClose( hfile );
      return FALSE;
   }

   DosClose( hfile );

   switch( pbmpfileheader2->usType )
   {
      case BFT_BITMAPARRAY:
      /* default to the first bitmap in the array */
      pbmpfileheader2Work = &((PBITMAPARRAYFILEHEADER2)pbmpfileheader2)->bfh2;
      pbmp2 = &pbmpfileheader2Work->bmp2;
      cbImageData = filestatus3.cbFile - pbmpfileheader2Work->offBits;
      pBits = (PBYTE)pbmpfileheader2 + pbmpfileheader2Work->offBits;
      break;

      case BFT_BMAP:
      /* set pbmp2 to point to the BITMAPINFO2 structure within the file */
      pbmp2 = &pbmpfileheader2->bmp2;
      cbImageData = filestatus3.cbFile - pbmpfileheader2->offBits;
      pBits = (PBYTE)pbmpfileheader2 + pbmpfileheader2->offBits;
      break;

      default:
      /* invalid file format */
      free( pbmpfileheader2 );
      return FALSE;
   }

   /* pBits should be sensible */
   if( !pbmp2 || !pBits ) {
      free( pbmpfileheader2 );
      return FALSE;
   }

   /* create a compatible memory PS */

   HDC    hdcMemory;       /* Device-context handle                */
   HPS    hpsMemory;       /* presentation-space handle            */
   SIZEL sizel;
   /* create memory device context and presentation space, associating
   DC with the PS */
   hdcMemory = DevOpenDC(pSetup->hab, OD_MEMORY, "*", 0, NULL, hdc);
   if (!hdcMemory)
   {
      free( pbmpfileheader2 );
      return FALSE;
   }

   sizel.cx = 0;
   sizel.cy = 0;
   hpsMemory = GpiCreatePS(pSetup->hab, hdcMemory, &sizel,
   GPIA_ASSOC | PU_PELS | GPIT_NORMAL);
   if (!hpsMemory)
   {
      free( pbmpfileheader2 );
      DevCloseDC( hdcMemory );
      return FALSE;
   }


   /* create a bitmap */

   hbm1 = GpiCreateBitmap( hpsMemory,
   pbmp2,
   0,                  /* options */
   (PBYTE)NULL,        /* init table */
   (PBITMAPINFO2)NULL );  /* bitmap info */

   /* check that bitmap has been created properly */
   if (hbm1 == GPI_ERROR)
   {
      free( pbmpfileheader2 );
      /* Clean up bitmap, memory PS and memory DC */
      GpiAssociate( hpsMemory, (HDC)NULLHANDLE );
      GpiDestroyPS( hpsMemory );
      DevCloseDC( hdcMemory );

      return FALSE;
   }

   /* set bitmap into the memory ps */
   hbm = GpiSetBitmap( hpsMemory, hbm1 );
   if(hbm == HBM_ERROR ) {
      free( pbmpfileheader2 );
      /* Clean up bitmap, memory PS and memory DC */
      GpiDeleteBitmap(hbm1);
      GpiAssociate( hpsMemory, (HDC)NULLHANDLE );
      GpiDestroyPS( hpsMemory );
      DevCloseDC( hdcMemory );

      return FALSE;
   }

   cScans = pbmp2->cy;
   cyBitmap = pbmp2->cy;
   cxBitmap = pbmp2->cx;

   /*
   * Some print drivers expect the bit image data to be dword
   * aligned.  Allocate a new memory object and copy bit data
   * to it.
   */
   pBitsAligned = (PBYTE) malloc( cbImageData );
   if(!pBitsAligned) {
      free( pbmpfileheader2 );
      /* Clean up bitmap, memory PS and memory DC */
      GpiDeleteBitmap(hbm1);
      GpiAssociate( hpsMemory, (HDC)NULLHANDLE );
      GpiDestroyPS( hpsMemory );
      DevCloseDC( hdcMemory );

      return FALSE;
   }

   memcpy( pBitsAligned, pBits, cbImageData );

   /* take bitmap bits from file buffer and place into memory ps for bitmap */
   lRC = GpiSetBitmapBits( hpsMemory,
   0,
   (LONG)cScans,
   pBitsAligned,
   (PBITMAPINFO2)pbmp2  );

   if( lRC == GPI_ERROR ) {
      free( pBitsAligned );
      free( pbmpfileheader2 );
      /* Clean up bitmap, memory PS and memory DC */
      GpiDeleteBitmap(hbm1);
      GpiAssociate( hpsMemory, (HDC)NULLHANDLE );
      GpiDestroyPS( hpsMemory );
      DevCloseDC( hdcMemory );

      return FALSE;
   }

   /* clean up */
   free( pBitsAligned );
   free( pbmpfileheader2 );

   POINTL aptl[4];

   /* setup the source bitblt rectangle */
   aptl[2].x = 0;
   aptl[2].y = 0;
   aptl[3].x = cxBitmap;
   aptl[3].y = cyBitmap;

   /* Set up target bitblt rectangles */
   aptl[0].x = rectl->xLeft;
   aptl[0].y = rectl->yBottom;
   aptl[1].x = rectl->xRight;
   aptl[1].y = rectl->yTop;

   if(booMaintainAspectRatio) {

      /* shrink x or y target rectangle */
      if(cxBitmap < cyBitmap) {
         aptl[1].x = rectl->xLeft + (rectl->xRight - rectl->xLeft)
         * ((double)cxBitmap/(double)cyBitmap);
      } else {
         aptl[1].y = rectl->yBottom + (rectl->yTop - rectl->yBottom)
         * ((double)cyBitmap/(double)cxBitmap);
      }

   }

   hbmOld = GpiSetBitmap( hpsMemory, 0L );
   if(hbmOld == HBM_ERROR){
      /* Clean up bitmap, memory PS and memory DC */
      GpiDeleteBitmap(hbm1);
      GpiAssociate( hpsMemory, (HDC)NULLHANDLE );
      GpiDestroyPS( hpsMemory );
      DevCloseDC( hdcMemory );

      return FALSE;
   }


   lRC = GpiWCBitBlt( hps,
   hbm1,
   4L,
   (PPOINTL)aptl,
   ROP_SRCCOPY,
   BBO_IGNORE );

   /* Clean up bitmap, memory PS and memory DC */
   GpiDeleteBitmap(hbm1);
   GpiAssociate( hpsMemory, (HDC)NULLHANDLE );
   GpiDestroyPS( hpsMemory );
   DevCloseDC( hdcMemory );

   if(lRC != GPI_OK) return FALSE;

   return TRUE;

}

