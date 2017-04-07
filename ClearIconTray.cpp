//**************************************************************************************
//  Copyright (c) 2017  Daniel D Miller
//  ShowWinMsgs.exe - Another WinBar application
//  
//  This program, its source code and executables, are Copyrighted in their
//  unmodified form by Daniel D Miller, and are distributed as free
//  software, with only one restriction:
//  
//  Any modified version of the program cannot be distributed with
//  the original name.
//  
//  Other than this, the source code, executables, help files, and any
//  other related files are provided with absolutely no restriction 
//  on use, distribution, modification, commercial adaptation, 
//  or any other conditions.
//  
//  Written by:   Daniel D. Miller
//**************************************************************************************
//  version		changes
//	 =======		======================================
// 	1.00		Initial release
//**************************************************************************************
const char *VerNum = "V1.00" ;
static char szClassName[] = "ClearIcon" ;

//lint -esym(767, _WIN32_WINNT)
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>   //  for sprintf, for %f support

#include "resource.h"
#include "common.h"
#include "systray.h"
#include "ClearIconTray.h"
#include "winmsgs.h"

//***********************************************************************

HINSTANCE g_hinst = 0;

static UINT timerID = 0 ;

//lint -esym(843, show_winmsgs)
static bool show_winmsgs = false ;

//*******************************************************************
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   //  debug function: see what messages are being received
   if (show_winmsgs) {
      switch (message) {
      //  list messages to be ignored
      case WM_CTLCOLORSTATIC:
      case WM_NCHITTEST:
      case WM_SETCURSOR:
      case WM_TIMER:
      case WM_MOUSEMOVE:
         break;
      default:
         // syslog("MON: [%s]\n", get_winmsg_name(result));
         syslog("SWMsg: [%s]\n", lookup_winmsg_name((int) message)) ;
         break;
      }
   }

   switch (message) {
   // case WM_CREATE:
   case WM_INITDIALOG:
      SendMessage(hwnd, WM_SETICON, ICON_BIG,   (LPARAM) LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_MAINICON)));
      SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_MAINICON)));

      //  load system-tray popup menu
      load_tray_menu();
      attach_tray_icon(hwnd, szClassName);

      timerID = SetTimer(hwnd, IDT_TIMER, 20, (TIMERPROC) NULL) ;
      return TRUE;

   case WM_TIMER:
      switch (wParam) {
      case IDT_TIMER:
         if (timerID != 0) {
            KillTimer(hwnd, timerID) ;
            timerID = 0 ;
         }
         ShowWindow (hwnd, SW_HIDE);
         reset_icon_colors(false);
         return TRUE;
      }  //lint !e744
      break;

   case WM_COMMAND:
      {
      DWORD cmd = HIWORD (wParam) ;
      DWORD target = LOWORD(wParam) ;
      // If a button is clicked...
      if (cmd == BN_CLICKED) {
         switch (target) {
         case IDM_SET_COLOR:   
            reset_icon_colors(true);
            return TRUE;
            
         case IDM_ABOUT:
            CmdAbout(hwnd);
            return TRUE;

         case IDM_SET_NOW:
            reset_icon_colors(false);
            return TRUE;

         case IDM_DEBUG:
            show_winmsgs = (show_winmsgs) ? false : true ;
            syslog("ClearIconTray: debug=%s\n", show_bool_str(show_winmsgs));
            return TRUE;

         case IDM_APP_EXIT:
            DestroyWindow(hwnd);
            return TRUE;
         }  //lint !e744
      } 
      }
      break;

   //  We cannot trigger on WM_ERASEBKGND or WM_CTLCOLORDLG,
   //  because we want to hide the main window.
   //  Thus, we trigger on WM_SYSCOLORCHANGE instead.
   // 00000002 11:37:16.650   [6380] SWMsg: [WM_SYSCOLORCHANGE]
   // 00000003 11:37:16.650   [6380] SWMsg: [WM_PAINT]   
   // 00000004 11:37:16.650   [6380] SWMsg: [WM_NCPAINT] 
   // 00000005 11:37:16.655   [6380] SWMsg: [WM_ERASEBKGND] 
   // 00000006 11:37:16.655   [6380] SWMsg: [WM_CTLCOLORDLG]   
   case WM_SYSCOLORCHANGE:
   // 00000826 13:10:04.124   [5108] SWMsg: [WM_DISPLAYCHANGE] 
   // 00000827 13:10:04.125   [4104] 2017-04-07 13:10:10.824 (  87756.863) |    
   //    DEBUG: [UXDriver.ApiX.MessageTranslator] 325@Nvidia::UXDriver::ApiX::MessageTranslator::Translate : 
   //           message(0x7e) wparam(0x20) lParam(0x6400a00): 9, translated(1).   
   case WM_DISPLAYCHANGE:
      reset_icon_colors(false);
      break;

   //  handle system-tray messages
   case WM_USER:
      respond_to_tray_clicks(hwnd, lParam);
      break;
   
   //********************************************************************
   //  application shutdown handlers
   //********************************************************************
   case WM_CLOSE:
      DestroyWindow(hwnd);
      return TRUE;

   case WM_DESTROY:
      release_systray_resource();
      PostQuitMessage(0);
      return TRUE;

   default:
      break;
   }  //lint !e744
   return FALSE;
}  //lint !e715

//***********************************************************************
static BOOL WeAreAlone(LPSTR szName)
{
   HANDLE hMutex = CreateMutex (NULL, TRUE, szName);
   if (GetLastError() == ERROR_ALREADY_EXISTS)
   {
      CloseHandle(hMutex);
      return FALSE;
   }
   return TRUE;
}  //lint !e818

//*********************************************************************
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpszArgument, int nFunsterStil)
{
   if (!WeAreAlone (szClassName)) {
      MessageBox(NULL, "ClearIcon is already running!!", "collision", MB_OK | MB_ICONEXCLAMATION) ;
      return 2;
   }

   g_hinst = hInstance;
   load_exec_filename() ;     //  get our executable name, reqd by read_config_file()
   read_config_file();

   int result = DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC) WndProc);
   if (result == 0) {
      syslog("DialogBox (main): %s [%u]\n", get_system_message(), GetLastError()) ;
      return 1;
   }
   return 0;
}  //lint !e715 !e818

