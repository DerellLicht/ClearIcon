//****************************************************************************
//  Copyright (c) 2009-2017  Daniel D Miller
//  
//  Written by:  Dan Miller
//****************************************************************************
//  Filename will be same as executable, but will have .ini extensions.
//  Config file will be stored in same location as executable file.
//  Comments will begin with '#'
//  First line:
//  device_count=%u
//  Subsequent file will have a section for each device.
//****************************************************************************
#include <windows.h>
#include <stdio.h>   //  fopen, etc
#include <stdlib.h>  //  atoi()
#include <limits.h>  //  PATH_MAX

#include "common.h"
#include "ClearIconTray.h"

static char ini_name[PATH_MAX+1] = "" ;

//****************************************************************************
static void strip_comments(char * const bfr)
{
   char * const hd = strchr(bfr, '#') ;
   if (hd != 0)
      *hd = 0 ;
}  //lint !e818  Pointer parameter 'bfr' (line 38) could be declared as pointing to const

//****************************************************************************
static LRESULT save_default_ini_file(void)
{
   FILE *fd = fopen(ini_name, "wt") ;
   if (fd == 0) {
      LRESULT result = (LRESULT) GetLastError() ;
      syslog("%s open: %s\n", ini_name, get_system_message((DWORD) result)) ;
      return result;
   }
   //  save any global vars
   fprintf(fd, "fg_attr=%u\n", fg_attr) ;
   fclose(fd) ;
   return ERROR_SUCCESS;
}

//****************************************************************************
LRESULT save_cfg_file(void)
{
   return save_default_ini_file() ;
}

//****************************************************************************
//  - derive ini filename from exe filename
//  - attempt to open file.
//  - if file does not exist, create it, with device_count=0
//    no other data.
//  - if file *does* exist, open/read it, create initial configuration
//****************************************************************************
LRESULT read_config_file(void)
{
   char inpstr[128] ;
   LRESULT result = derive_filename_from_exec(ini_name, (char *) ".ini") ; //lint !e1773
   if (result != 0)
      return result;

   FILE *fd = fopen(ini_name, "rt") ;
   if (fd == 0)
      return save_default_ini_file() ;

   while (fgets(inpstr, sizeof(inpstr), fd) != 0) {
      strip_comments(inpstr) ;
      strip_newlines(inpstr) ;
      if (strlen(inpstr) == 0)
         continue;

      if (strncmp(inpstr, "fg_attr=", 8) == 0) {
         // syslog("enabling factory mode\n") ;
         fg_attr = (uint) strtoul(&inpstr[8], 0, 0) ;
      } else
      {
         syslog("unknown: [%s]\n", inpstr) ;
      }
   }
   return 0;
}

