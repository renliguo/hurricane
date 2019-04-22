/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.44 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to Silicon Labs Norway, a subsidiary
of Silicon Labs Inc. whose registered office is 400 West Cesar Chavez,
Austin,  TX 78701, USA solely for  the purposes of creating  libraries 
for its  ARM Cortex-M3, M4F  processor-based devices,  sublicensed and 
distributed  under the  terms and conditions  of the  End User License  
Agreement supplied by Silicon Labs.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              Silicon Laboratories Norway
Licensed SEGGER software: emWin
License number:           GUI-00140
License model:            See Agreement, dated 20th April 2012
Licensed product:         - 
Licensed platform:        Cortex M3, Cortex M4F
Licensed number of seats: -
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2012-04-24 - 2018-05-08
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : GUIDRV_Lin_Opt_32.h
Purpose     : Optimized routines, included by GUIDRV_Lin_..._32.c
---------------------------END-OF-HEADER------------------------------
*/

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _FillRectOpt32
*
* Purpose:
*   Optimized filling routine for 32 bpp
*/
static void _FillRectOpt32(GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1) {
  DRIVER_CONTEXT * pContext;
  register LCD_PIXELINDEX ColorIndex;
  U32 * pDest;
  U32 * pDest0;
  U32 Off, Off0, RemPixels, NumLines, RemLines, OffLine, RemItems;
  LCD_PIXELINDEX IndexMask;

  pContext  = (DRIVER_CONTEXT *)pDevice->u.pContext;
  RemPixels = x1 - x0 + 1;
  NumLines  = y1 - y0 + 1;
  OffLine   = pContext->vxSizePhys;
  pDest     = NULL;
  if (GUI_pContext->DrawMode & LCD_DRAWMODE_XOR) {
    IndexMask = pDevice->pColorConvAPI->pfGetIndexMask();
    Off0      = XY2OFF32(pContext->vxSizePhys, x0, y0);
    if (RemPixels) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        Off = Off0 + OffLine * (RemLines - 1);
        do {
          ColorIndex  = READ_MEM32(pContext->VRAMAddr, Off);
          ColorIndex ^= IndexMask;
          WRITE_MEM32(pContext->VRAMAddr, Off, ColorIndex);
          Off++;
        } while (--RemItems);
      }
    }
  } else {
    Off        = XY2OFF32(pContext->vxSizePhys, x0, y0);
    pDest0     = OFF2PTR32(pContext->VRAMAddr, Off);
    ColorIndex = LCD__GetColorIndex();
    if (RemPixels >= 16) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        pDest    = pDest0 + OffLine * (RemLines - 1);
        do {
          WRITE_MEM32P(pDest     , ColorIndex);
          WRITE_MEM32P(pDest +  1, ColorIndex);
          WRITE_MEM32P(pDest +  2, ColorIndex);
          WRITE_MEM32P(pDest +  3, ColorIndex);
          WRITE_MEM32P(pDest +  4, ColorIndex);
          WRITE_MEM32P(pDest +  5, ColorIndex);
          WRITE_MEM32P(pDest +  6, ColorIndex);
          WRITE_MEM32P(pDest +  7, ColorIndex);
          WRITE_MEM32P(pDest +  8, ColorIndex);
          WRITE_MEM32P(pDest +  9, ColorIndex);
          WRITE_MEM32P(pDest + 10, ColorIndex);
          WRITE_MEM32P(pDest + 11, ColorIndex);
          WRITE_MEM32P(pDest + 12, ColorIndex);
          WRITE_MEM32P(pDest + 13, ColorIndex);
          WRITE_MEM32P(pDest + 14, ColorIndex);
          WRITE_MEM32P(pDest + 15, ColorIndex);
          pDest     += 16;
          RemItems -= 16;
        } while (RemItems >= 16);
      }
      pDest0 = pDest;
      RemPixels -= (RemPixels >> 4) << 4;
    }
    if (RemPixels) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        pDest    = pDest0 + OffLine * (RemLines - 1);
        do {
          WRITE_MEM32P(pDest, ColorIndex);
          pDest++;
        } while (--RemItems);
      }
    }
  }
}

/*************************** End of file ****************************/