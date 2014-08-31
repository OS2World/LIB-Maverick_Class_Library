/*  File: MAVPRINT.HPP      Updated: Tue Aug 15 15:55:22 1995
Copyright (c) Fabrizio Aversa
===========================================================*/
#ifndef _MAVPRINT_HPP
#define _MAVPRINT_HPP


// Defines for resource compiler for setup.
#define ID_PICKQ            22000
#define IDC_LISTBOX         22001
#define IDC_JOBPROP         22002
#define IDC_ENTRY           22003
#define IDC_TOFILE          22004
#define ID_PICKQ_IT            22005

typedef struct _PRINTERSETUP {

   // INPUT PARAMETERS: assign these before calling SetupPrinter()
   LONG           lWorldCoordinates; // PU_TWIPS or whatever
   HAB            hab;               // application's anchor block

   // PROFILED PARAMETERS: store these two items on app close.
   // Retrieve them on app open.
   // Note pDriverData->cb is length of DRIVDATA structure.
   CHAR           szPreferredQueue[ 64 ];  // name of user's preferred queue
   PDRIVDATA      pDriverData;             // driver's data

   // OUTPUT PARAMETERS: for use by SetupPrinter() and the application
   HDC            hdcPrinterInfo;    // printer info DC
   HPS            hpsPrinterInfo;    // printer info PS
   LONG           lDCType;           // DC type suitable for DevOpenDC() param 2
   PDEVOPENDATA   pDevOpenData;      // suitable for DevOpenDC() parameter 5
   DEVOPENSTRUC   devopenstruc;      // pdevopendata points to this.
   LONG           cQueues;           // count of queues in PRQINFO3 array
   PPRQINFO3      pQueueInfo;        // pointer to array of PRQINFO3
   BOOL           fToFile;           // printing to file case
   CHAR           szFileName[ CCHMAXPATH ];  // file name for print-to-file

} PRINTERSETUP;
typedef PRINTERSETUP *PPRINTERSETUP;

class _Export MavPrint
{

   private:

   PPRINTERSETUP pSetup;
   FONTMETRICS fm; /* actual font metrics */
   HDC hdc;
   HPS hps;
   LONG lLcIdCount; /* number of log fonts created during this hps life */
   RECTL rctMargins;
   SIZEL sizMarginedPage, sizPage, sizHeader, sizFooter;
   USHORT * usFontSize;

   PPRQINFO3 FindQueue( PPRINTERSETUP );
   char pszQueueProcParams[15];

   public:

HDC getHDC() { return hdc; }
HPS getHPS() { return hps; }
void setHAB(HAB h1) {pSetup->hab = h1;}
void setWorldCoords(LONG l1) {pSetup->lWorldCoordinates = l1;}
RECTL * getMargins() {return &rctMargins;}
SIZEL * getMarginedPageSize() {return & sizMarginedPage;}
SIZEL * getPageSize() {return & sizPage;}
SIZEL * getHeaderSize() {return & sizHeader;}
SIZEL * getFooterSize() {return & sizFooter;}
PDEVOPENDATA getDevOpenData() {return pSetup->pDevOpenData;}
PDRIVDATA getDrivData() {return pSetup->pDriverData;}
FONTMETRICS * getPFM() {return & fm;}

   /*     setupPrinter       fills the PRINTERSETUP structure with relevant data.
   call with empty pszUserQueueName if you want the default queue
   call with pszUserQueueName if you want ta specific queue you know in advance, in
   this case you can also use pszUserPrintFileName.
   call with a valid owner if you want the print queue dialog to be shown.
   returns TRUE if ok, otherwise FALSE.
   Always call with PSZs allocated for the maximum queue name length, they will
   be filled on successfull completion with queue/file names. */
   Boolean setupPrinter(PSZ, PSZ, IWindow * = NULL, int = 1 );
   Boolean setupPrinter(IWindow * = NULL, int = 1 );

   void cleanupPrinter( );
   Boolean openPrinterJob(PSZ);
   void closePrinterJob();
   void abortPrinterJob();
   Boolean newPage();
   Boolean setMargins(LONG, LONG, LONG, LONG);

   Boolean printTextInBox(PRECTL, PSZ, LONG &, USHORT = 1, LONG = 0, LONG = 0);
   Boolean printTextInBox(PRECTL, IString &, LONG &, USHORT = 1, LONG = 0, LONG = 0);

   Boolean printStringAt(PPOINTL, PSZ, LONG = 0);
   Boolean printStringAt(PPOINTL, IString &, LONG = 0);

   PPOINTL coordsInHeader(PPOINTL, PPOINTL);
   PPOINTL coordsInFooter(PPOINTL, PPOINTL);
   PPOINTL coordsInMargins(PPOINTL, PPOINTL);
   PRECTL rectlInMargins(PRECTL, PRECTL);

   LONG createFontMatching(PSZ, LONG, ULONG = 0, PBOOL = NULL, PSIZEF = NULL);
   Boolean selectLogFont(LONG, PSIZEF = NULL);

   Boolean printBitmapInBox(RECTL *, PSZ, Boolean);

   MavPrint();
   ~MavPrint();

};

#endif
