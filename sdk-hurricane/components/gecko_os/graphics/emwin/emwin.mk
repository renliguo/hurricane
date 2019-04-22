NAME := lib_graphics_emwin

$(NAME)_PREBUILT_LIBRARIES := lib_graphics_emwin.a

$(NAME)_GLOBAL_INCLUDES := GUI/Core \
                           Config


# uncomment to build library
# $(NAME)_SOURCES := GUI/AntiAlias/GUIAA_Arc.c \
# 					GUI/AntiAlias/GUIAA_Char.c \
# 					GUI/AntiAlias/GUIAA_Char2.c \
# 					GUI/AntiAlias/GUIAA_Char2Ext.c \
# 					GUI/AntiAlias/GUIAA_Char2Ext_XBF.c \
# 					GUI/AntiAlias/GUIAA_Char4.c \
# 					GUI/AntiAlias/GUIAA_Char4Ext.c \
# 					GUI/AntiAlias/GUIAA_Char4Ext_XBF.c \
# 					GUI/AntiAlias/GUIAA_Char8.c \
# 					GUI/AntiAlias/GUIAA_Circle.c \
# 					GUI/AntiAlias/GUIAA_DrawRoundedRect.c \
# 					GUI/AntiAlias/GUIAA_FillRoundedRect.c \
# 					GUI/AntiAlias/GUIAA_Lib.c \
# 					GUI/AntiAlias/GUIAA_Line.c \
# 					GUI/AntiAlias/GUIAA_Poly.c \
# 					GUI/AntiAlias/GUIAA_PolyOut.c \
# 					GUI/AntiAlias/GUIAA_SIF2.c \
# 					GUI/AntiAlias/GUIAA_SIF2Ext.c \
# 					GUI/AntiAlias/GUIAA_SIF4.c \
# 					GUI/AntiAlias/GUIAA_SIF4Ext.c \
# 					GUI/ConvertColor/GUICC_111.c \
# 					GUI/ConvertColor/GUICC_16.c \
# 					GUI/ConvertColor/GUICC_1616I.c \
# 					GUI/ConvertColor/GUICC_222.c \
# 					GUI/ConvertColor/GUICC_233.c \
# 					GUI/ConvertColor/GUICC_323.c \
# 					GUI/ConvertColor/GUICC_332.c \
# 					GUI/ConvertColor/GUICC_444_12.c \
# 					GUI/ConvertColor/GUICC_444_12_1.c \
# 					GUI/ConvertColor/GUICC_444_16.c \
# 					GUI/ConvertColor/GUICC_555.c \
# 					GUI/ConvertColor/GUICC_666.c \
# 					GUI/ConvertColor/GUICC_666_9.c \
# 					GUI/ConvertColor/GUICC_822216.c \
# 					GUI/ConvertColor/GUICC_84444.c \
# 					GUI/ConvertColor/GUICC_8666.c \
# 					GUI/ConvertColor/GUICC_8666_1.c \
# 					GUI/ConvertColor/GUICC_88666I.c \
# 					GUI/ConvertColor/GUICC_M1555I.c \
# 					GUI/ConvertColor/GUICC_M233.c \
# 					GUI/ConvertColor/GUICC_M323.c \
# 					GUI/ConvertColor/GUICC_M332.c \
# 					GUI/ConvertColor/GUICC_M4444I.c \
# 					GUI/ConvertColor/GUICC_M444_12.c \
# 					GUI/ConvertColor/GUICC_M444_12_1.c \
# 					GUI/ConvertColor/GUICC_M444_16.c \
# 					GUI/ConvertColor/GUICC_M555.c \
# 					GUI/ConvertColor/GUICC_M666.c \
# 					GUI/ConvertColor/GUICC_M666_9.c \
# 					GUI/ConvertColor/GUICC_M8565.c \
# 					GUI/ConvertColor/GUICC_M8888I.c \
# 					GUI/ConvertMono/GUICC_2.c \
# 					GUI/ConvertMono/GUICC_4.c \
# 					GUI/ConvertMono/GUICC_5.c \
# 					GUI/ConvertMono/GUICC_6.c \
# 					GUI/ConvertMono/GUICC_8.c \
# 					GUI/Core/GUI_2DLib.c \
# 					GUI/Core/GUI__Alpha.c \
# 					GUI/Core/GUI__Arabic.c \
# 					GUI/Core/GUI__BiDi.c \
# 					GUI/Core/GUI__BiDi2.c \
# 					GUI/Core/GUI__BiDiRouting.c \
# 					GUI/Core/GUI__CompactPixelIndices.c \
# 					GUI/Core/GUI__DivTbl.c \
# 					GUI/Core/GUI__DrawBitmap16bpp.c \
# 					GUI/Core/GUI__DrawTwinArc.c \
# 					GUI/Core/GUI__ExpandPixelIndices.c \
# 					GUI/Core/GUI__GetCursorPos.c \
# 					GUI/Core/GUI__ManageCache.c \
# 					GUI/Core/GUI__Private.c \
# 					GUI/Core/GUI__SinHQ.c \
# 					GUI/Core/GUI__Wrap.c \
# 					GUI/Core/GUI_Alloc.c \
# 					GUI/Core/GUI_ANIM.c \
# 					GUI/Core/GUI_Arc.c \
# 					GUI/Core/GUI_BMP.c \
# 					GUI/Core/GUI_BMP_DrawScaled.c \
# 					GUI/Core/GUI_BMP_EnableAlpha.c \
# 					GUI/Core/GUI_BMP_Serialize.c \
# 					GUI/Core/GUI_Char.c \
# 					GUI/Core/GUI_CharLine.c \
# 					GUI/Core/GUI_CharM.c \
# 					GUI/Core/GUI_CharP.c \
# 					GUI/Core/GUI_CharPExt.c \
# 					GUI/Core/GUI_CharPFrm.c \
# 					GUI/Core/GUI_Circ.c \
# 					GUI/Core/GUI_Common.c \
# 					GUI/Core/GUI_CopyRect.c \
# 					GUI/Core/GUI_Core.c \
# 					GUI/Core/GUI_Cursor.c \
# 					GUI/Core/GUI_CursorResource.c \
# 					GUI/Core/GUI_Device.c \
# 					GUI/Core/GUI_DirtyDevice.c \
# 					GUI/Core/GUI_DispChar.c \
# 					GUI/Core/GUI_DispString.c \
# 					GUI/Core/GUI_DispStringInRectEx.c \
# 					GUI/Core/GUI_DrawBitmap.c \
# 					GUI/Core/GUI_DrawBitmap_24.c \
# 					GUI/Core/GUI_DrawBitmap_444.c \
# 					GUI/Core/GUI_DrawBitmap_555.c \
# 					GUI/Core/GUI_DrawBitmap_565.c \
# 					GUI/Core/GUI_DrawBitmap_888.c \
# 					GUI/Core/GUI_DrawBitmap_8888.c \
# 					GUI/Core/GUI_DrawBitmap_A16.c \
# 					GUI/Core/GUI_DrawBitmapEx.c \
# 					GUI/Core/GUI_DrawGradient.c \
# 					GUI/Core/GUI_DrawGradientRounded.c \
# 					GUI/Core/GUI_DrawGraph.c \
# 					GUI/Core/GUI_DrawGraphEx.c \
# 					GUI/Core/GUI_DrawLib.c \
# 					GUI/Core/GUI_DrawLine.c \
# 					GUI/Core/GUI_DrawRoundedFrame.c \
# 					GUI/Core/GUI_DrawRoundedRect.c \
# 					GUI/Core/GUI_EnableAlpha.c \
# 					GUI/Core/GUI_EncExt.c \
# 					GUI/Core/GUI_EncJS.c \
# 					GUI/Core/GUI_ErrorOut.c \
# 					GUI/Core/GUI_FillPolygon.c \
# 					GUI/Core/GUI_FillRoundedRect.c \
# 					GUI/Core/GUI_GCache.c \
# 					GUI/Core/GUI_GCache_4.c \
# 					GUI/Core/GUI_GetBlankCols.c \
# 					GUI/Core/GUI_GIF.c \
# 					GUI/Core/GUI_GIF_DrawScaled.c \
# 					GUI/Core/GUI_JPEG.c \
# 					GUI/Core/GUI_JPEG_DrawScaled.c \
# 					GUI/Core/GUI_Lang.c \
# 					GUI/Core/GUI_Log.c \
# 					GUI/Core/GUI_MOUSE.c \
# 					GUI/Core/GUI_MOUSE_DriverPS2.c \
# 					GUI/Core/GUI_MOVIE.c \
# 					GUI/Core/GUI_MULTIBUF.c \
# 					GUI/Core/GUI_OnKey.c \
# 					GUI/Core/GUI_PID.c \
# 					GUI/Core/GUI_PolyEnlarge.c \
# 					GUI/Core/GUI_QR.c \
# 					GUI/Core/GUI_SelectLayer.c \
# 					GUI/Core/GUI_SetAlpha.c \
# 					GUI/Core/GUI_SetOrientation.c \
# 					GUI/Core/GUI_SetOrientationC0.c \
# 					GUI/Core/GUI_SetOrientationC16.c \
# 					GUI/Core/GUI_SetOrientationC32.c \
# 					GUI/Core/GUI_SetOrientationC8.c \
# 					GUI/Core/GUI_SIF.c \
# 					GUI/Core/GUI_SIF_Prop.c \
# 					GUI/Core/GUI_SIF_PropExt.c \
# 					GUI/Core/GUI_SIF_PropExtFrmInfo.c \
# 					GUI/Core/GUI_SIF_PropFrm.c \
# 					GUI/Core/GUI_SOFTLAYER.c \
# 					GUI/Core/GUI_SPLINE.c \
# 					GUI/Core/GUI_SPRITE.c \
# 					GUI/Core/GUI_SPRITE_Anim.c \
# 					GUI/Core/GUI_SPY.c \
# 					GUI/Core/GUI_Stream.c \
# 					GUI/Core/GUI_StreamCreate.c \
# 					GUI/Core/GUI_Task.c \
# 					GUI/Core/GUI_Time.c \
# 					GUI/Core/GUI_Timer.c \
# 					GUI/Core/GUI_TOUCH.c \
# 					GUI/Core/GUI_TOUCH_Calibrate.c \
# 					GUI/Core/GUI_TOUCH_DriverAnalog.c \
# 					GUI/Core/GUI_TOUCH_StoreState.c \
# 					GUI/Core/GUI_TOUCH_StoreUnstable.c \
# 					GUI/Core/GUI_UC.c \
# 					GUI/Core/GUI_UC0.c \
# 					GUI/Core/GUI_UC_ConvertUC2UTF8.c \
# 					GUI/Core/GUI_UC_ConvertUTF82UC.c \
# 					GUI/Core/GUI_UC_DispString.c \
# 					GUI/Core/GUI_UC_EnableBIDI.c \
# 					GUI/Core/GUI_UC_EncodeNone.c \
# 					GUI/Core/GUI_UC_EncodeUTF8.c \
# 					GUI/Core/GUI_Val.c \
# 					GUI/Core/GUI_Valf.c \
# 					GUI/Core/GUI_WaitEvent.c \
# 					GUI/Core/GUI_Warn.c \
# 					GUI/Core/GUI_XBF.c \
# 					GUI/Core/GUI_XBF_Prop.c \
# 					GUI/Core/GUI_XBF_PropExt.c \
# 					GUI/Core/GUI_XBF_PropFrm.c \
# 					GUI/Core/GUICC_0.c \
# 					GUI/Core/GUICC_1.c \
# 					GUI/Core/GUICC_444_12_1_Index2Color.c \
# 					GUI/Core/GUICC_444_12_Index2Color.c \
# 					GUI/Core/GUICC_444_16_Index2Color.c \
# 					GUI/Core/GUICC_555_Index2Color.c \
# 					GUI/Core/GUICC_565.c \
# 					GUI/Core/GUICC_888.c \
# 					GUI/Core/GUICC_8888.c \
# 					GUI/Core/GUICC_8888_Index2Color.c \
# 					GUI/Core/GUICC_888_Index2Color.c \
# 					GUI/Core/GUICC_BW.c \
# 					GUI/Core/GUICC_M444_12_1_Index2Color.c \
# 					GUI/Core/GUICC_M444_12_Index2Color.c \
# 					GUI/Core/GUICC_M444_16_Index2Color.c \
# 					GUI/Core/GUICC_M555_Index2Color.c \
# 					GUI/Core/GUICC_M565.c \
# 					GUI/Core/GUICC_M888.c \
# 					GUI/Core/GUICC_M8888.c \
# 					GUI/Core/GUICC_M8888I_Index2Color.c \
# 					GUI/Core/GUICC_M888_Index2Color.c \
# 					GUI/Core/LCD.c \
# 					GUI/Core/LCD_AA.c \
# 					GUI/Core/LCD_Color.c \
# 					GUI/Core/LCD_ControlCache.c \
# 					GUI/Core/LCD_GetColorIndex.c \
# 					GUI/Core/LCD_GetEx.c \
# 					GUI/Core/LCD_GetNumDisplays.c \
# 					GUI/Core/LCD_GetP.c \
# 					GUI/Core/LCD_GetPixelColor.c \
# 					GUI/Core/LCD_Index2ColorEx.c \
# 					GUI/Core/LCD_Info.c \
# 					GUI/Core/LCD_Mirror.c \
# 					GUI/Core/LCD_MixColors256.c \
# 					GUI/Core/LCD_NextPixelAPI.c \
# 					GUI/Core/LCD_ReadRect.c \
# 					GUI/Core/LCD_RLE16.c \
# 					GUI/Core/LCD_RLE32.c \
# 					GUI/Core/LCD_RLE4.c \
# 					GUI/Core/LCD_RLE8.c \
# 					GUI/Core/LCD_RLEAlpha.c \
# 					GUI/Core/LCD_Rotate180.c \
# 					GUI/Core/LCD_RotateCCW.c \
# 					GUI/Core/LCD_RotateCW.c \
# 					GUI/Core/LCD_SelectLCD.c \
# 					GUI/Core/LCD_Set.c \
# 					GUI/Core/LCD_SetClipRectEx.c \
# 					GUI/Core/LCD_UpdateColorIndices.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_1.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_16.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_2.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_24.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_32.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_4.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_8.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OS_16.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OS_24.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OS_32.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OS_8.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OSX_16.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OSX_24.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OSX_32.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OSX_8.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OSXY_16.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OSY_16.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OSY_24.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OSY_32.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OSY_8.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OX_1.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OX_16.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OX_24.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OX_32.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OX_8.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OXY_1.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OXY_16.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OXY_24.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OXY_32.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OXY_8.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OY_1.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OY_16.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OY_24.c \
# 					GUI/DisplayDriver/GUIDRV_Lin_OY_32.c \
# 					GUI/Font/F08_1.c \
# 					GUI/Font/F08_ASCII.c \
# 					GUI/Font/F10_1.c \
# 					GUI/Font/F10_ASCII.c \
# 					GUI/Font/F10S_1.c \
# 					GUI/Font/F10S_ASCII.c \
# 					GUI/Font/F13_1.c \
# 					GUI/Font/F13_ASCII.c \
# 					GUI/Font/F13B_1.c \
# 					GUI/Font/F13B_ASCII.c \
# 					GUI/Font/F13H_1.c \
# 					GUI/Font/F13H_ASCII.c \
# 					GUI/Font/F13HB_1.c \
# 					GUI/Font/F13HB_ASCII.c \
# 					GUI/Font/F16_1.c \
# 					GUI/Font/F16_1HK.c \
# 					GUI/Font/F16_ASCII.c \
# 					GUI/Font/F16_HK.c \
# 					GUI/Font/F16B_1.c \
# 					GUI/Font/F16B_ASCII.c \
# 					GUI/Font/F20_1.c \
# 					GUI/Font/F20_ASCII.c \
# 					GUI/Font/F20B_1.c \
# 					GUI/Font/F20B_ASCII.c \
# 					GUI/Font/F20F_ASCII.c \
# 					GUI/Font/F24_1.c \
# 					GUI/Font/F24_ASCII.c \
# 					GUI/Font/F24B_1.c \
# 					GUI/Font/F24B_ASCII.c \
# 					GUI/Font/F32_1.c \
# 					GUI/Font/F32_ASCII.c \
# 					GUI/Font/F32B_1.c \
# 					GUI/Font/F32B_ASCII.c \
# 					GUI/Font/F4x6.c \
# 					GUI/Font/F6x8.c \
# 					GUI/Font/F6x8_1.c \
# 					GUI/Font/F6x8_ASCII.c \
# 					GUI/Font/F8x10_ASCII.c \
# 					GUI/Font/F8x12_ASCII.c \
# 					GUI/Font/F8x13_1.c \
# 					GUI/Font/F8x13_ASCII.c \
# 					GUI/Font/F8x15B_1.c \
# 					GUI/Font/F8x15B_ASCII.c \
# 					GUI/Font/F8x16.c \
# 					GUI/Font/F8x16_1.c \
# 					GUI/Font/F8x16_ASCII.c \
# 					GUI/Font/F8x8.c \
# 					GUI/Font/F8x8_1.c \
# 					GUI/Font/F8x8_ASCII.c \
# 					GUI/Font/FComic18B_1.c \
# 					GUI/Font/FComic18B_ASCII.c \
# 					GUI/Font/FComic24B_1.c \
# 					GUI/Font/FComic24B_ASCII.c \
# 					GUI/Font/FD24x32.c \
# 					GUI/Font/FD32.c \
# 					GUI/Font/FD36x48.c \
# 					GUI/Font/FD48.c \
# 					GUI/Font/FD48x64.c \
# 					GUI/Font/FD60x80.c \
# 					GUI/Font/FD64.c \
# 					GUI/Font/FD80.c \
# 					GUI/MemDev/GUIDEV.c \
# 					GUI/MemDev/GUIDEV_1.c \
# 					GUI/MemDev/GUIDEV_16.c \
# 					GUI/MemDev/GUIDEV_32.c \
# 					GUI/MemDev/GUIDEV_8.c \
# 					GUI/MemDev/GUIDEV_AA.c \
# 					GUI/MemDev/GUIDEV_Auto.c \
# 					GUI/MemDev/GUIDEV_Banding.c \
# 					GUI/MemDev/GUIDEV_Blur.c \
# 					GUI/MemDev/GUIDEV_BlurAndBlendWin.c \
# 					GUI/MemDev/GUIDEV_Clear.c \
# 					GUI/MemDev/GUIDEV_ClearAlpha.c \
# 					GUI/MemDev/GUIDEV_CmpWithLCD.c \
# 					GUI/MemDev/GUIDEV_CopyFromLCD.c \
# 					GUI/MemDev/GUIDEV_CreateFixed.c \
# 					GUI/MemDev/GUIDEV_Dither.c \
# 					GUI/MemDev/GUIDEV_GetDataPtr.c \
# 					GUI/MemDev/GUIDEV_GetPos.c \
# 					GUI/MemDev/GUIDEV_GetXSize.c \
# 					GUI/MemDev/GUIDEV_GetYSize.c \
# 					GUI/MemDev/GUIDEV_Measure.c \
# 					GUI/MemDev/GUIDEV_MoveAndFade.c \
# 					GUI/MemDev/GUIDEV_MoveAndFadeWin.c \
# 					GUI/MemDev/GUIDEV_Perspective.c \
# 					GUI/MemDev/GUIDEV_PunchOutDevice.c \
# 					GUI/MemDev/GUIDEV_ReadLine.c \
# 					GUI/MemDev/GUIDEV_ReduceYSize.c \
# 					GUI/MemDev/GUIDEV_Rotate.c \
# 					GUI/MemDev/GUIDEV_RotateTrans.c \
# 					GUI/MemDev/GUIDEV_Serialize.c \
# 					GUI/MemDev/GUIDEV_SetColorConv.c \
# 					GUI/MemDev/GUIDEV_SetOrg.c \
# 					GUI/MemDev/GUIDEV_StaticDevices.c \
# 					GUI/MemDev/GUIDEV_Usage.c \
# 					GUI/MemDev/GUIDEV_UsageBM.c \
# 					GUI/MemDev/GUIDEV_Write.c \
# 					GUI/MemDev/GUIDEV_WriteAlpha.c \
# 					GUI/MemDev/GUIDEV_WriteEx.c \
# 					GUI/MemDev/GUIDEV_WriteToActiveAlpha.c \
# 					GUI/MemDev/GUIDEV_WriteToActiveOpaque.c \
# 					GUI/MemDev/GUIDEV_XY2PTR.c \
# 					GUI/TouchDriver/GUITDRV_ADS7846.c \
# 					GUI/Widget/BUTTON.c \
# 					GUI/Widget/BUTTON__SetDrawObj.c \
# 					GUI/Widget/BUTTON_Bitmap.c \
# 					GUI/Widget/BUTTON_BMP.c \
# 					GUI/Widget/BUTTON_Default.c \
# 					GUI/Widget/BUTTON_GetBitmap.c \
# 					GUI/Widget/BUTTON_SelfDraw.c \
# 					GUI/Widget/BUTTON_Skin.c \
# 					GUI/Widget/BUTTON_SkinClassic.c \
# 					GUI/Widget/BUTTON_SkinFlex.c \
# 					GUI/Widget/BUTTON_StreamedBitmap.c \
# 					GUI/Widget/CALENDAR.c \
# 					GUI/Widget/CHECKBOX.c \
# 					GUI/Widget/CHECKBOX_Default.c \
# 					GUI/Widget/CHECKBOX_Image.c \
# 					GUI/Widget/CHECKBOX_SetNumStates.c \
# 					GUI/Widget/CHECKBOX_Skin.c \
# 					GUI/Widget/CHECKBOX_SkinClassic.c \
# 					GUI/Widget/CHECKBOX_SkinFlex.c \
# 					GUI/Widget/CHOOSECOLOR.c \
# 					GUI/Widget/CHOOSEFILE.c \
# 					GUI/Widget/DIALOG.c \
# 					GUI/Widget/DROPDOWN.c \
# 					GUI/Widget/DROPDOWN_Default.c \
# 					GUI/Widget/DROPDOWN_Expanded.c \
# 					GUI/Widget/DROPDOWN_ItemDisabled.c \
# 					GUI/Widget/DROPDOWN_Skin.c \
# 					GUI/Widget/DROPDOWN_SkinClassic.c \
# 					GUI/Widget/DROPDOWN_SkinFlex.c \
# 					GUI/Widget/EDIT.c \
# 					GUI/Widget/EDIT_Default.c \
# 					GUI/Widget/EDITBin.c \
# 					GUI/Widget/EDITDec.c \
# 					GUI/Widget/EDITFloat.c \
# 					GUI/Widget/EDITHex.c \
# 					GUI/Widget/EDITUlong.c \
# 					GUI/Widget/FRAMEWIN.c \
# 					GUI/Widget/FRAMEWIN__UpdateButtons.c \
# 					GUI/Widget/FRAMEWIN_AddButton.c \
# 					GUI/Widget/FRAMEWIN_AddMenu.c \
# 					GUI/Widget/FRAMEWIN_ButtonClose.c \
# 					GUI/Widget/FRAMEWIN_ButtonMax.c \
# 					GUI/Widget/FRAMEWIN_ButtonMin.c \
# 					GUI/Widget/FRAMEWIN_Default.c \
# 					GUI/Widget/FRAMEWIN_MinMaxRest.c \
# 					GUI/Widget/FRAMEWIN_SetResizeable.c \
# 					GUI/Widget/FRAMEWIN_Skin.c \
# 					GUI/Widget/FRAMEWIN_SkinClassic.c \
# 					GUI/Widget/FRAMEWIN_SkinFlex.c \
# 					GUI/Widget/GRAPH.c \
# 					GUI/Widget/GRAPH_DATA_XY.c \
# 					GUI/Widget/GRAPH_DATA_YT.c \
# 					GUI/Widget/GRAPH_SCALE.c \
# 					GUI/Widget/GUI_ARRAY.c \
# 					GUI/Widget/GUI_ARRAY_DeleteItem.c \
# 					GUI/Widget/GUI_ARRAY_InsertItem.c \
# 					GUI/Widget/GUI_ARRAY_ResizeItem.c \
# 					GUI/Widget/GUI_DRAW.c \
# 					GUI/Widget/GUI_DRAW_BITMAP.c \
# 					GUI/Widget/GUI_DRAW_BMP.c \
# 					GUI/Widget/GUI_DRAW_Self.c \
# 					GUI/Widget/GUI_DRAW_STREAMED.c \
# 					GUI/Widget/GUI_EditBin.c \
# 					GUI/Widget/GUI_EditDec.c \
# 					GUI/Widget/GUI_EditFloat.c \
# 					GUI/Widget/GUI_EditHex.c \
# 					GUI/Widget/GUI_EditString.c \
# 					GUI/Widget/GUI_HOOK.c \
# 					GUI/Widget/HEADER.c \
# 					GUI/Widget/HEADER__SetDrawObj.c \
# 					GUI/Widget/HEADER_Bitmap.c \
# 					GUI/Widget/HEADER_BMP.c \
# 					GUI/Widget/HEADER_Default.c \
# 					GUI/Widget/HEADER_Skin.c \
# 					GUI/Widget/HEADER_SkinClassic.c \
# 					GUI/Widget/HEADER_SkinFlex.c \
# 					GUI/Widget/HEADER_StreamedBitmap.c \
# 					GUI/Widget/ICONVIEW.c \
# 					GUI/Widget/ICONVIEW_BMP.c \
# 					GUI/Widget/ICONVIEW_SetWrapMode.c \
# 					GUI/Widget/IMAGE.c \
# 					GUI/Widget/IMAGE_Bitmap.c \
# 					GUI/Widget/IMAGE_BMP.c \
# 					GUI/Widget/IMAGE_DTA.c \
# 					GUI/Widget/IMAGE_GIF.c \
# 					GUI/Widget/IMAGE_JPEG.c \
# 					GUI/Widget/KNOB.c \
# 					GUI/Widget/LISTBOX.c \
# 					GUI/Widget/LISTBOX_Default.c \
# 					GUI/Widget/LISTVIEW.c \
# 					GUI/Widget/LISTVIEW_Default.c \
# 					GUI/Widget/LISTVIEW_DeleteColumn.c \
# 					GUI/Widget/LISTVIEW_DeleteRow.c \
# 					GUI/Widget/LISTVIEW_GetItemText.c \
# 					GUI/Widget/LISTVIEW_InsertRow.c \
# 					GUI/Widget/LISTVIEW_SetItemColor.c \
# 					GUI/Widget/LISTVIEW_SetSort.c \
# 					GUI/Widget/LISTWHEEL.c \
# 					GUI/Widget/LISTWHEEL_Create.c \
# 					GUI/Widget/MENU.c \
# 					GUI/Widget/MENU__FindItem.c \
# 					GUI/Widget/MENU_Default.c \
# 					GUI/Widget/MENU_Skin.c \
# 					GUI/Widget/MENU_SkinClassic.c \
# 					GUI/Widget/MENU_SkinFlex.c \
# 					GUI/Widget/MESSAGEBOX.c \
# 					GUI/Widget/MULTIEDIT.c \
# 					GUI/Widget/MULTIPAGE.c \
# 					GUI/Widget/MULTIPAGE_Default.c \
# 					GUI/Widget/MULTIPAGE_SetRotation.c \
# 					GUI/Widget/MULTIPAGE_Skin.c \
# 					GUI/Widget/MULTIPAGE_SkinClassic.c \
# 					GUI/Widget/MULTIPAGE_SkinFlex.c \
# 					GUI/Widget/PROGBAR.c \
# 					GUI/Widget/PROGBAR_Skin.c \
# 					GUI/Widget/PROGBAR_SkinClassic.c \
# 					GUI/Widget/PROGBAR_SkinFlex.c \
# 					GUI/Widget/RADIO.c \
# 					GUI/Widget/RADIO_Default.c \
# 					GUI/Widget/RADIO_Image.c \
# 					GUI/Widget/RADIO_Skin.c \
# 					GUI/Widget/RADIO_SkinClassic.c \
# 					GUI/Widget/RADIO_SkinFlex.c \
# 					GUI/Widget/SCROLLBAR.c \
# 					GUI/Widget/SCROLLBAR_Defaults.c \
# 					GUI/Widget/SCROLLBAR_Skin.c \
# 					GUI/Widget/SCROLLBAR_SkinClassic.c \
# 					GUI/Widget/SCROLLBAR_SkinFlex.c \
# 					GUI/Widget/SLIDER.c \
# 					GUI/Widget/SLIDER_Default.c \
# 					GUI/Widget/SLIDER_Skin.c \
# 					GUI/Widget/SLIDER_SkinClassic.c \
# 					GUI/Widget/SLIDER_SkinFlex.c \
# 					GUI/Widget/SPINBOX.c \
# 					GUI/Widget/SPINBOX_Default.c \
# 					GUI/Widget/SPINBOX_Skin.c \
# 					GUI/Widget/SPINBOX_SkinClassic.c \
# 					GUI/Widget/SPINBOX_SkinFlex.c \
# 					GUI/Widget/SWIPELIST.c \
# 					GUI/Widget/SWIPELIST_Default.c \
# 					GUI/Widget/TEXT.c \
# 					GUI/Widget/TEXT_Default.c \
# 					GUI/Widget/TREEVIEW.c \
# 					GUI/Widget/TREEVIEW_Default.c \
# 					GUI/Widget/TREEVIEW_Image.c \
# 					GUI/Widget/WIDGET.c \
# 					GUI/Widget/WIDGET_Effect_3D.c \
# 					GUI/Widget/WIDGET_Effect_3D1L.c \
# 					GUI/Widget/WIDGET_Effect_3D2L.c \
# 					GUI/Widget/WIDGET_Effect_None.c \
# 					GUI/Widget/WIDGET_Effect_Simple.c \
# 					GUI/Widget/WINDOW.c \
# 					GUI/Widget/WINDOW_Default.c \
# 					GUI/WM/WM.c \
# 					GUI/WM/WM__Private.c \
# 					GUI/WM/WM_BringToTop.c \
# 					GUI/WM/WM_DIAG.c \
# 					GUI/WM/WM_GetScrollPos.c \
# 					GUI/WM/WM_GetWindowSize.c \
# 					GUI/WM/WM_Hide.c \
# 					GUI/WM/WM_IsCompletelyCovered.c \
# 					GUI/WM/WM_IsCompletelyVisible.c \
# 					GUI/WM/WM_Lib.c \
# 					GUI/WM/WM_MakeModal.c \
# 					GUI/WM/WM_MemDev.c \
# 					GUI/WM/WM_Motion.c \
# 					GUI/WM/WM_Move.c \
# 					GUI/WM/WM_MULTIBUF.c \
# 					GUI/WM/WM_PaintWindowAndDescs.c \
# 					GUI/WM/WM_PID__GetPrevState.c \
# 					GUI/WM/WM_Screen2Win.c \
# 					GUI/WM/WM_SetCapture.c \
# 					GUI/WM/WM_SetCaptureMove.c \
# 					GUI/WM/WM_SetFocusOnNextChild.c \
# 					GUI/WM/WM_SetFocusOnPrevChild.c \
# 					GUI/WM/WM_SetScrollbar.c \
# 					GUI/WM/WM_SetScrollPos.c \
# 					GUI/WM/WM_SetTrans.c \
# 					GUI/WM/WM_SetTransState.c \
# 					GUI/WM/WM_SIM.c \
# 					GUI/WM/WM_Timer.c \
# 					GUI/WM/WM_ToolTip.c \
# 					GUI/WM/WM_Touch.c \
# 					GUI/WM/WM_UpdateWindowAndDescs.c \
# 					GUI/WM/WM_UserData.c \
# 					GUI/WM/WM_Validate.c

$(NAME)_INCLUDES := GUI \
					GUI/AntiAlias \
					GUI/ConvertColor \
					GUI/ConvertMono \
					GUI/Core \
					GUI/DisplayDriver \
					GUI/Font \
					GUI/MemDev \
					GUI/TouchDriver \
					GUI/Widget \
					GUI/WM \

