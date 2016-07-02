/*
    main.cpp
    Copyright (C) 2007 Acekard, www.acekard.com
    Copyright (C) 2007-2009 somebody
    Copyright (C) 2009 yellow wood goblin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <nds.h>
#include <cstdio>
#include <vector>
#include <map>
#include <list>

#include <elm.h>

#include "dbgtool.h"
#include "gdi.h"
#include "ui.h"

#include "systemfilenames.h"
#include "mainlist.h"
#include "startmenu.h"
#include "mainwnd.h"
#include "timer.h"

#include "datetime.h"
#include "calendarwnd.h"
#include "bigclock.h"
#include "calendar.h"

#include "inifile.h"
#include "irqs.h"

#include "diskicon.h"
#include "progresswnd.h"
#include "language.h"
#include "fontfactory.h"

#include "userwnd.h"
#include "sram.h"
#include "exptools.h"
#include "romlauncher.h"

using namespace akui;

#ifdef __cplusplus
extern "C" {
#endif
void __libnds_exit(int rc) {}
#ifdef __cplusplus
}
#endif

int main(void)
{
    irq().init();

    windowManager();

    // init basic system
    sysSetBusOwners( BUS_OWNER_ARM9, BUS_OWNER_ARM9 );

    // init tick timer/fps counter
    timer().initTimer();

    // init inputs
    initInput();

    //turn led on
    ledBlink(PM_LED_ON);

    // init graphics
    gdi().init();
#ifdef DEBUG
    gdi().switchSubEngineMode();
#endif//DEBUG
    dbg_printf( "gdi ok\n" );

    //wait_press_b();
    // init fat
    //bool succ = fatInitDefault();
    bool succ = (ELM_Mount()&1)?false:true;
    if( !succ )
        dbg_printf( "init fat %d\n", succ );

    //wait_press_b();

    // setting scripts
    gs().loadSettings();

    // init unicode
    //if( initUnicode() )
    //    _FAT_unicode_init( unicodeL2UTable, unicodeU2LTable, unicodeAnkTable );
    cwl();


    lang(); // load language file
    gs().language=lang().GetInt("font","language",gs().language);
    fontFactory().makeFont(); // load font file
    uiSettings().loadSettings();


    bool saveListOK = saveManager().importSaveList( SFN_CUSTOM_SAVELIST, SFN_OFFICIAL_SAVELIST );
    if( !saveListOK ) {
        //messageBox( NULL,
        //    LANG("no savelist","title"),
        //    LANG("no savelist", "text"), MB_OK );
        dbg_printf("WARNING: savelist.bin missed\n");
    }


    gdi().initBg(SFN_LOWER_SCREEN_BG);

    cMainWnd * wnd = new cMainWnd( 0, 0, 256, 192, NULL, "main window" );
    wnd->init();

    progressWnd().init();

    diskIcon().loadAppearance( SFN_CARD_ICON_BLUE );
    diskIcon().show();

    windowManager().update();
    timer().updateFps();

    calendarWnd().init();
    calendarWnd().draw();
    calendar().init();
    calendar().draw();
    bigClock().init();
    bigClock().draw();

    userWindow().draw();

    gdi().present( GE_MAIN );
    gdi().present( GE_SUB );

    dbg_printf( "loop start\n" );

    irq().vblankStart();

    // enter last directory
    std::string lastDirectory="...",lastFile="...";
    if(gs().enterLastDirWhenBoot||gs().autorunWithLastRom)
    {
      CIniFile f;
      if(f.LoadIniFile(SFN_LAST_SAVEINFO))
      {
        lastFile=f.GetString("Save Info","lastLoaded","");
        if(""==lastFile)
        {
          lastFile="...";
        }
        else if(gs().enterLastDirWhenBoot)
        {
          size_t slashPos=lastFile.find_last_of('/');
          if(lastFile.npos!=slashPos) lastDirectory=lastFile.substr(0,slashPos+1);
        }
      }
    }

    { //backup save data from chip to flash. pressing LShift+Up aborts backup.
#if defined(_STORAGE_rpg)
      INPUT & inputs = updateInput();
      if((inputs.keysHeld&(KEY_UP|KEY_L))==(KEY_UP|KEY_L))
      {
        u32 ret = messageBox( NULL, LANG("abort save", "title"), LANG("abort save", "text"), MB_YES | MB_NO );
        if(ret==ID_YES)
        {
          saveManager().clearLastInfo();
          saveManager().clearSaveBlocks();
        }
        else saveManager().backupSaveData();
      }
      else
      {
        saveManager().backupSaveData();
      }
#elif defined(_STORAGE_r4) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
      saveManager().clearLastInfo();
#endif
      //backup gba sram save date to flash.
      if(gs().gbaAutoSave&&expansion().IsValid())
      {
        CIniFile f;
        if(f.LoadIniFile(SFN_LAST_GBA_SAVEINFO))
        {
          std::string psramFile=f.GetString("Save Info","lastLoaded","");
          if(psramFile!="")
          {
            cSram::SaveSramToFile(psramFile.c_str(),cExpansion::EPsramPage);
            f.SetString("Save Info","lastLoaded","");
            f.SaveIniFile(SFN_LAST_GBA_SAVEINFO);
          }
          std::string norFile=f.GetString("Save Info","lastLoadedNOR","");
          if(norFile!="")
          {
            std::string norFileSave=norFile+".sav";
            FILE* saveFile=fopen(norFileSave.c_str(),"rb");
            if(saveFile)
            {
              cSram::sSaveInfo saveInfo;
              cSram::ProcessRAW(saveFile,saveInfo);
              u8* bufFile=(u8*)malloc(saveInfo.size);
              if(bufFile)
              {
                memset(bufFile,0,saveInfo.size);
                fread(bufFile,saveInfo.size,1,saveFile);
                u8* bufData=cSram::SaveSramToMemory(cExpansion::ENorPage,saveInfo,false);
                if(bufData)
                {
                  if(memcmp(bufFile,bufData,saveInfo.size)!=0)
                  {
                    cSram::SaveSramToFile(norFile.c_str(),cExpansion::ENorPage);
                  }
                  free(bufData);
                }
                free(bufFile);
              }
              fclose(saveFile);
            }
          }
        }
      }
    }

    if(gs().autorunWithLastRom&&"..."!=lastFile)
    {
      INPUT & inputs = updateInput();
      if(!(inputs.keysHeld&KEY_B)) autoLaunchRom(lastFile);
    }

    dbg_printf("lastDirectory '%s'\n", lastDirectory.c_str() );
    if(!wnd->_mainList->enterDir("..."!=lastDirectory?lastDirectory:gs().startupFolder)) wnd->_mainList->enterDir("...");

    if(*(u32*)0x04000604) fifoSendValue32(FIFO_USER_01,MENU_MSG_SHUTDOWN);

    while( true )
    {
      timer().updateFps();

      INPUT & inputs = updateInput();

      processInput( inputs );

      //swiWaitForVBlank();

      windowManager().update();

      gdi().present( GE_MAIN );
    }

    return 0;
}
