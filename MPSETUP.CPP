/*  File: MPSETUP.CPP      Updated: Tue Aug 15 15:54:49 1995
Copyright (c) Fabrizio Aversa
===========================================================*/
// setup.c: printer setup routine
// Monte Copeland for DevCon 7

#define INCL_DOSNLS        /* National Language Support values */
#define INCL_DEV
#define INCL_PM
#define INCL_SPLDOSPRINT
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iframe.hpp>
#include <icmdhdr.hpp>
#include <istattxt.hpp>
#include <istring.hpp>
#include <imsgbox.hpp>
#include <ilistbox.hpp>
#include <icheckbx.hpp>
#include <icmdhdr.hpp>
#include <iedithdr.hpp>
#include <icheckbx.hpp>
#include <ientryfd.hpp>
#include <iselhdr.hpp>
#include <idate.hpp>

#include "mavprint.hpp"
#include "mavconv.hpp"

class PrinterDialog : public IFrameWindow,
public ICommandHandler,
public ISelectHandler
{
   public:

   PrinterDialog(IWindow *, PPRINTERSETUP, LONG);
   ~PrinterDialog();

   private:

   PRINTERSETUP         backup;
   PPRINTERSETUP       pSetup, pTarget;

   void fillFields();
   virtual Boolean command(ICommandEvent&);
   virtual Boolean selected(IControlEvent& );

   ICheckBox * check;
   IEntryField * entry;
   IListBox * list;

};

PrinterDialog :: PrinterDialog(IWindow* owner, PPRINTERSETUP p1, LONG lngDlgId)
: IFrameWindow(IResourceId(lngDlgId,IDynamicLinkLibrary("mavcl.dll")), owner)
{
   ICommandHandler::handleEventsFor(this);
   ISelectHandler::handleEventsFor(this);

   memcpy( &backup, p1, sizeof( PRINTERSETUP ) );
   pSetup =  &backup; // work on backup copy
   pTarget = p1;

   check= new ICheckBox(IDC_TOFILE, this);
   entry= new IEntryField(IDC_ENTRY, this);
   list= new IListBox(IDC_LISTBOX, this);

   fillFields();
}

PrinterDialog :: ~PrinterDialog()
{
   delete check;
   delete entry;
   delete list;
}

void PrinterDialog :: fillFields()
{
   LONG j, i, selected = 0;
   char * psz;

   for( i = 0; i < pSetup->cQueues; i++ )
   {
      // Display printer comment if possible, else use queue name for display.
      psz = (*pSetup->pQueueInfo[i].pszComment ?
      pSetup->pQueueInfo[i].pszComment : pSetup->pQueueInfo[i].pszName);

      /* subst cr and lf with space */
      for (j = 0 ; j < strlen(psz) ; j++)
      if(psz[j] == 13 || psz[j] == 10) psz[j] = ' ';

      list->addAsLast( psz );

      if( 0 == strcmp( pSetup->pQueueInfo[i].pszName, pSetup->szPreferredQueue )) {
         selected = i;
      }
   }

   // Ensure that one queue is selected.
   if( list->count() ) list->select( selected );

   // check print-to-file button
   if(! pSetup->fToFile )
   {
      check->deselect();
      entry->disable( );
   } else
   check->select();

   // entry field is file name
   if( 0 == strlen( pSetup->szFileName ))
   {
      // put in a default
      strcpy( pSetup->szFileName, "PRINTER.OUT" );
   }
   entry->setText( pSetup->szFileName  );

}

Boolean PrinterDialog :: command(ICommandEvent& cmdevt)
{
   CHAR                szDeviceName[ 48 ];
   CHAR                szDriverName[ 64 ];
   PPRQINFO3           pqi;

   pqi = &pSetup->pQueueInfo[ list->selection() ];
   char * pch;

   switch(cmdevt.commandId()) {

      case DID_OK:

      // New preferred queue. Modify the one in the PRINTERSETUP structure.
      strcpy( pSetup->szPreferredQueue, pqi->pszName );

      // query filename
      if( check->isSelected() ) {
         pSetup->fToFile = TRUE;
         strcpy(pSetup->szFileName, entry->text());
      } else {
         pSetup->fToFile = FALSE;
         *pSetup->szFileName = 0;
      }

      memcpy( pTarget, pSetup, sizeof( PRINTERSETUP ) );

      dismiss(DID_OK);

      break;

      case DID_CANCEL:
      dismiss(DID_CANCEL);
      break;

      case IDC_JOBPROP:

      // Call DevPostDeviceModes() to present the job setup dialog of the printer driver.
      // pqi->pszDriverName is DRIVER.Device format. Separate them.

      strcpy( szDriverName, pqi->pszDriverName );

      pch = strchr( szDriverName, '.' );
      if( pch ) {
         strcpy( szDeviceName, pch+1 );
         *pch = 0;
      }
      else
      {
         *szDeviceName = 0;
      }


      // There may be more than one printer on this print queue
      pch = strchr( pqi->pszPrinters, ',' );
      if( pch ) {
         *pch = 0;
      }

      // Present the job properties dialog to the user.
      if(
      DevPostDeviceModes( pSetup->hab,
      pqi->pDriverData,
      szDriverName,
      szDeviceName,
      pqi->pszPrinters,
      DPDM_POSTJOBPROP )
      == DPDM_ERROR) {

         IMessageBox msgbox(this);
         msgbox.setTitle("Impostazione stampante");
         msgbox.show("DevPostDeviceModes",
         IMessageBox::okButton | IMessageBox::errorIcon | IMessageBox::moveable);

      }

      break;


   }/* end switch */

   return(false); //Allow Default Processing to occur
}

Boolean PrinterDialog::selected(IControlEvent& icEvt)
{
   switch(icEvt.controlId()) {

      case IDC_TOFILE:
      if( check->isSelected() ) {
         entry->enable();
      } else {
         entry->disable();
      }
      break;

   }

   return true;

}

Boolean MavPrint::setupPrinter(IWindow * owner, int iNumCopies)
{

   CHAR psz1[80], psz2[80];

   *psz1 = 0;
   *psz2 = 0;

   return setupPrinter(psz1, psz2, owner, iNumCopies);

}

Boolean MavPrint::setupPrinter(
PSZ pszUserQueueName,
PSZ pszUserPrintFileName,
IWindow * owner,
int iNumCopies
)
/* fills the PRINTERSETUP structure with relevant data.
call with empty pszUserQueueName if you want the default queue
call with pszUserQueueName if you want ta specific queue you know in advance, in
this case you can also use pszUserPrintFileName.
call with a valid owner if you want the print queue dialog to be shown.
returns TRUE if ok, otherwise FALSE.
Always call with PSZs allocated for the maximum queue name length, they will
be filled on successfull completion with queue/file names. */
{
   BOOL            fOK;
   CHAR            szDefaultQueue[ 196 ];
   CHAR            szSavedQueue[ 196 ];
   CHAR            szWork[ 196 ];
   PCHAR           pch;
   PPRQINFO3       pqi;
   SIZEL           sizel;
   ULONG           cReturned;
   ULONG           cTotal;
   ULONG           cbNeeded;
   ULONG           ul;
   ULONG           ulrc;

   // Caller must set these items before calling.
   if(! pSetup->hab ||
   ! pSetup->lWorldCoordinates  ) return FALSE;

   // no good unless I can open a PS
   pSetup->pDevOpenData = NULL;

   // Close the info DC's and PS's from any previous call.
   if( pSetup->hpsPrinterInfo )
   {
      GpiAssociate( pSetup->hpsPrinterInfo, (HDC)0 );
      GpiDestroyPS( pSetup->hpsPrinterInfo );
      pSetup->hpsPrinterInfo = (HPS)0;
   }

   if( pSetup->hdcPrinterInfo )
   {
      DevCloseDC( pSetup->hdcPrinterInfo );
      pSetup->hdcPrinterInfo = (HDC)0;
   }

   if( pSetup->pQueueInfo )
   {
      // Free the array of PRQINFO3 from previous call.
      free( pSetup->pQueueInfo );
      pSetup->pQueueInfo = NULL;
   }

   // Query how many queues exist on this computer and the
   // number of bytes needed to hold the array.
   ul = SplEnumQueue( NULL, 3, NULL, 0, &cReturned, &cTotal, &cbNeeded, NULL );
   if( cTotal == 0 )
   {
      // There are no queues on this computer!
      ulrc = FALSE;
      pSetup->cQueues = 0;
      return ulrc;
   }

   // Allocate memory to store the newly enumerated queue information.
   pSetup->pQueueInfo = (PRQINFO3*)malloc( cbNeeded ) ;
   if( ! pSetup->pQueueInfo )
   {
      ulrc = FALSE;
      return ulrc;
   }

   // Call system again to get the array of PRQINFO3 structures.
   ul = SplEnumQueue( NULL, 3, pSetup->pQueueInfo, cbNeeded, &cReturned, &cTotal, &cbNeeded, NULL );
   if( ul != 0 ||
   cReturned != cTotal ) return FALSE;
   pSetup->cQueues = cReturned;

   // Establish a default queue -- might need it.
   // Profiled queue name ends with a semicolon.
   ul = PrfQueryProfileString( HINI_PROFILE, "PM_SPOOLER", "QUEUE", NULL, szDefaultQueue, 196 );
   if( ul > 1 ) {
      // Trim off semicolon.
      pch = strchr( szDefaultQueue, ';' );
      *pch = 0;
   } else {
      // Hmmmm. Use the first one queue from the enumeration.
      strcpy( szDefaultQueue,  pSetup->pQueueInfo->pszName );
   }
   if(! strlen( szDefaultQueue ) ) return FALSE;

   if( 0 == strlen( pSetup->szPreferredQueue ))
   {
      // No queue preference; use default.
      strcpy( pSetup->szPreferredQueue, szDefaultQueue );

      // Don't expect to see DRIVDATA without queue name.
      // if(! pSetup->pDriverData ) return FALSE;
   }

   // There is a chance that the preferred queue has been recently deleted.
   // Ensure the preferred queue still exists.

   if(*pszUserQueueName) {

      pSetup->fToFile = FALSE;
      strcpy( pSetup->szPreferredQueue, pszUserQueueName );

      if(*pszUserPrintFileName )
      {
         pSetup->fToFile = TRUE;
         strcpy(pSetup->szFileName, pszUserPrintFileName);
      }

   }

   pqi = FindQueue( pSetup );
   if( ! pqi )
   {
      // Not found. Use the default queue.
      strcpy( pSetup->szPreferredQueue, szDefaultQueue );

      if( pSetup->pDriverData )
      {
         free( pSetup->pDriverData );
         pSetup->pDriverData = NULL;
      }
   }
   else
   {
      // Preferred queue was found in the array. Do some additional validation
      // because it may have changed since last time in this function.

      fOK = TRUE;

      if( pSetup->pDriverData )
      {
         // Is driver data the right length?
         fOK = fOK && ( pqi->pDriverData->cb == pSetup->pDriverData->cb );

         // Is this queue still driving the same device?
         fOK = fOK && ( 0 == strcmp( pqi->pDriverData->szDeviceName,  pSetup->pDriverData->szDeviceName ));
      }

      if( !fOK )
      {
         free( pSetup->pDriverData );
         pSetup->pDriverData = NULL;
      }
   }

   // Find the queue again. If the last find failed, preferred queue name
   // was changed to default. This find will absolutely always succeed.

   pqi = FindQueue( pSetup );

   if( !pSetup->pDriverData )
   {
      // Use driver data from the enumeration.
      pSetup->pDriverData = (DRIVDATA*)malloc( pqi->pDriverData->cb );
      if( ! pSetup->pDriverData )
      {
         ulrc = FALSE;
         return ulrc;
      }
      memcpy( pSetup->pDriverData, pqi->pDriverData, pqi->pDriverData->cb );
   }

   if(
   ! pSetup->pDriverData ||
   pSetup->pDriverData->cb <= 0 ||
   pSetup->pDriverData->cb != pqi->pDriverData->cb ||
   strcmp( pqi->pDriverData->szDeviceName,  pSetup->pDriverData->szDeviceName )
   ) return FALSE;

   /* Note that DriverData used on DevPostDeviceModes() called
   from QueryPrintDlgProc() is the one in the enumerated array,
   not the one in pSetup. This way, QueryPrintDlgProc()
   can massage any/all of the queues in the array before dismissing the dialog.
   */
   memcpy( pqi->pDriverData, pSetup->pDriverData, pSetup->pDriverData->cb );

   // Save the name of the preferred queue because the dialogs can change it.
   strcpy( szSavedQueue, pSetup->szPreferredQueue );

   if( owner )
   {

      /* see if user eats spaghetti */
      COUNTRYINFO  CountryInfo; /* Buffer for country-specific information */
      getCountryInfo(&CountryInfo);
      LONG lngDlgId = (CountryInfo.country == 39 ? ID_PICKQ_IT : ID_PICKQ);

      PrinterDialog * printerDialog= new PrinterDialog(owner, pSetup, lngDlgId);
      printerDialog->showModally();
      Boolean booRet= (printerDialog->result() == DID_CANCEL ?  0 : 1);
      delete printerDialog;
      //      if( !booRet ) return TRUE;

   } else {

      /* see if user passed a preferred queue/file name, in this case use it */
      if(*pszUserQueueName) {

         pSetup->fToFile = FALSE;
         strcpy( pSetup->szPreferredQueue, pszUserQueueName );

         if(*pszUserPrintFileName )
         {
            pSetup->fToFile = TRUE;
            strcpy(pSetup->szFileName, pszUserPrintFileName);
         }

      }
   }

   /* return to the user the selected queue/file (for now blank) */
   strcpy(pszUserQueueName,  "" );
   strcpy(pszUserPrintFileName,  "" );

   // QueryPrintDlgProc() may have modified pSetup->szPreferredQueue.
   pqi = FindQueue( pSetup );
   if ( ! pqi ) return FALSE;

   if( 0 != strcmp( szSavedQueue, pSetup->szPreferredQueue ))
   {
      // The user picked a different queue during dialog processing.
      if(! pSetup->pDriverData ) return FALSE;
      free( pSetup->pDriverData );

      pSetup->pDriverData = (DRIVDATA*)malloc( pqi->pDriverData->cb );
      if( ! pSetup->pDriverData )
      {
         ulrc = FALSE;
         return ulrc;
      }
      pSetup->pDriverData->cb = pqi->pDriverData->cb;
   }

   // Copy data from the array back to PRINTERSETUP structure.
   if(! pSetup->pDriverData ||
   ! pSetup->pDriverData->cb == pqi->pDriverData->cb ) return FALSE;
   memcpy( pSetup->pDriverData, pqi->pDriverData, pqi->pDriverData->cb );

   /* Prepare a DEVOPENSTRUC for DevOpenDC(). Use it here to open an OD_INFO
   DC. Caller may use the same DEVOPENSTRUC to open an OD_QUEUED DC when it is
   time to print. There are 9 pointers in the DEVOPENSTRUC. This code
   prepares the first 4. The others should be NULL.
   */

   // Prepare logical address which is preferred queue name.
   if( pSetup->fToFile )
   {
      pSetup->lDCType = OD_DIRECT;
      pSetup->devopenstruc.pszLogAddress = pSetup->szFileName;
   }
   else
   {
      pSetup->lDCType = OD_QUEUED;
      pSetup->devopenstruc.pszLogAddress = pSetup->szPreferredQueue;
   }

   // Prepare .DRV file name. Truncate after the period.
   strcpy( szWork, pqi->pszDriverName );
   pch = strchr( szWork, '.' );
   if( pch ) {
      *pch = 0;
   }
   if( pSetup->devopenstruc.pszDriverName )
   {
      free( pSetup->devopenstruc.pszDriverName );
   }
   pSetup->devopenstruc.pszDriverName = (PSZ)malloc( 1 + strlen( szWork ));
   if( ! pSetup->devopenstruc.pszDriverName )
   {
      ulrc = FALSE;
      return ulrc;
   }
   strcpy( pSetup->devopenstruc.pszDriverName, szWork );

   // Prepare pointer to driver data.
   pSetup->devopenstruc.pdriv = pSetup->pDriverData;

   // Prepare data type. Standard is the preferred way to go.
   pSetup->devopenstruc.pszDataType = "PM_Q_STD";

   if(iNumCopies > 1) {
      sprintf(pszQueueProcParams, "COP=%03d", iNumCopies);
      pSetup->devopenstruc.pszQueueProcParams = pszQueueProcParams;
   }

   // Open an OD_INFO DC.
   pSetup->hdcPrinterInfo = DevOpenDC( pSetup->hab,
   OD_INFO,
   "*",
   4,
   (PDEVOPENDATA)&pSetup->devopenstruc,
   (HDC)0 );
   if( !pSetup->hdcPrinterInfo ) {
      // Unable to open info DC. WinGetLastError() can provide diagnostics.
      ulrc = FALSE;
      return ulrc;
   }

   // Create PS associated with OD_INFO DC.
   sizel.cx = 0;
   sizel.cy = 0;
   pSetup->hpsPrinterInfo = GpiCreatePS( pSetup->hab,
   pSetup->hdcPrinterInfo,
   &sizel,
   pSetup->lWorldCoordinates | GPIA_ASSOC );

   if( GPI_ERROR ==  pSetup->hpsPrinterInfo ) {
      // Problem with this setup.
      DevCloseDC( pSetup->hdcPrinterInfo );
      pSetup->hdcPrinterInfo = (HDC)0;
      pSetup->hpsPrinterInfo = (HPS)0;
      ulrc = FALSE;
      return ulrc;
   }

   // OK to use.
   pSetup->pDevOpenData = (PDEVOPENDATA)&pSetup->devopenstruc;

   /* return to the user the selected queue */
   strcpy(pszUserQueueName, pSetup->szPreferredQueue);
   if(pSetup->fToFile) strcpy(pszUserPrintFileName, pSetup->szFileName);

   // Success.
   ulrc = TRUE;
   return ulrc;
}

// ---------------------------------------------------------------------------------------------------------
// FindQueue finds the preferred queue name in the PRQINFO3 array.
// Returns the index if found; returns -1 if not found.

PPRQINFO3 MavPrint::FindQueue( PPRINTERSETUP pSetup )
{
   LONG   i;

   for( i = 0; i < pSetup->cQueues; i++ )
   {
      if( 0 == strcmp( pSetup->szPreferredQueue, pSetup->pQueueInfo[ i ].pszName ) )
      {
         return &pSetup->pQueueInfo[ i ];
      }
   }
   return NULL;
}

void MavPrint::cleanupPrinter()
{

   // Close DC's and PS's.

   if( pSetup->hpsPrinterInfo )
   {
      GpiAssociate( pSetup->hpsPrinterInfo, (HDC)0 );
      GpiDestroyPS( pSetup->hpsPrinterInfo );
      pSetup->hpsPrinterInfo = (HPS)0;
   }

   if( pSetup->hdcPrinterInfo )
   {
      DevCloseDC( pSetup->hdcPrinterInfo );
      pSetup->hdcPrinterInfo = (HDC)0;
   }


   if( pSetup->pQueueInfo )
   {
      free( pSetup->pQueueInfo );
      pSetup->pQueueInfo = NULL;
   }

   if( pSetup->pDriverData )
   {
      free( pSetup->pDriverData );
      pSetup->pDriverData = NULL;
   }

   if( pSetup->devopenstruc.pszDriverName )
   {
      free( pSetup->devopenstruc.pszDriverName );
      pSetup->devopenstruc.pszDriverName = NULL;
   }

}


