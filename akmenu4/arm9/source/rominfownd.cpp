/*
    rominfownd.cpp
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

#include <sys/stat.h>
#include "rominfownd.h"
#include "systemfilenames.h"
#include "msgbox.h"
#include "windowmanager.h"
#include "globalsettings.h"
#include "uisettings.h"
#include "language.h"
#include "unicode.h"
#include "files.h"
#include "gbaloader.h"
#include "cheatwnd.h"
#include "ui/binaryfind.h"

using namespace akui;

cRomInfoWnd::cRomInfoWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cForm( x, y, w, h, parent, text ),
_buttonOK(0,0,46,18,this,"\x01 OK"),
_buttonSaveType(0,0,76,18,this,"\x04 Save Type"),
_buttonFlash(0,0,46,18,this,"\x03 to NOR"),
_buttonCopy(0,0,46,18,this,"\x05 to RAM"),
_buttonCheats(0,0,46,18,this,"\x03 Cheats"),
_settingWnd(NULL),
_saves(NULL)
{
    s16 buttonY = size().y - _buttonOK.size().y - 4;

    _buttonOK.setStyle( cButton::press );
    _buttonOK.setText( "\x01 " + LANG( "setting window", "ok" ) );
    _buttonOK.setTextColor( uis().buttonTextColor );
    _buttonOK.loadAppearance( SFN_BUTTON3 );
    _buttonOK.clicked.connect( this, &cRomInfoWnd::onOK );
    addChildWindow( &_buttonOK );

    s16 nextButtonX = size().x;

    s16 buttonPitch = _buttonOK.size().x + 8;
    nextButtonX -= buttonPitch;

    _buttonOK.setRelativePosition( cPoint(nextButtonX, buttonY) );

    _buttonSaveType.setStyle( cButton::press );
    _buttonSaveType.setText( "\x04 " + LANG( "setting window", "savetype" ) );
    _buttonSaveType.setTextColor( uis().buttonTextColor );
    _buttonSaveType.loadAppearance( SFN_BUTTON4 );
    _buttonSaveType.clicked.connect( this, &cRomInfoWnd::pressSaveType );
    addChildWindow( &_buttonSaveType );

    buttonPitch = _buttonSaveType.size().x + 8;
    s16 nextButtonXone = nextButtonX - buttonPitch;

    _buttonSaveType.setRelativePosition( cPoint(nextButtonXone, buttonY) );

    _buttonCheats.setStyle( cButton::press );
    _buttonCheats.setText( "\x03 " + LANG( "cheats", "title" ) );
    _buttonCheats.setTextColor( uis().buttonTextColor );
    _buttonCheats.loadAppearance( SFN_BUTTON3 );
    _buttonCheats.clicked.connect( this, &cRomInfoWnd::pressCheats );
    addChildWindow( &_buttonCheats );

    buttonPitch = _buttonCheats.size().x + 8;
    nextButtonXone -= buttonPitch;

    _buttonCheats.setRelativePosition( cPoint(nextButtonXone, buttonY) );

    _buttonFlash.setStyle( cButton::press );
    _buttonFlash.setText( "\x03 " + LANG( "exp window", "flash to nor" ) );
    _buttonFlash.setTextColor( uis().buttonTextColor );
    _buttonFlash.loadAppearance( SFN_BUTTON3 );
    _buttonFlash.clicked.connect( this, &cRomInfoWnd::pressFlash );
    addChildWindow( &_buttonFlash );

    buttonPitch = _buttonFlash.size().x + 8;
    nextButtonX -= buttonPitch;

    _buttonFlash.setRelativePosition( cPoint(nextButtonX, buttonY) );

    _buttonCopy.setStyle( cButton::press );
    _buttonCopy.setText( "\x05 " + LANG( "exp window", "copy to psram" ) );
    _buttonCopy.setTextColor( uis().buttonTextColor );
    _buttonCopy.loadAppearance( SFN_BUTTON3 );
    _buttonCopy.clicked.connect( this, &cRomInfoWnd::pressCopy );
    addChildWindow( &_buttonCopy );

    buttonPitch = _buttonCopy.size().x + 8;
    nextButtonX -= buttonPitch;

    _buttonCopy.setRelativePosition( cPoint(nextButtonX, buttonY) );

    loadAppearance( "" );
    arrangeChildren();
}

cRomInfoWnd::~cRomInfoWnd()
{}


void cRomInfoWnd::draw()
{
    _renderDesc.draw( windowRectangle(), _engine );

    _romInfo.drawDSRomIcon( position().x + 8, position().y + 24, selectedEngine() );

    gdi().setPenColor( uiSettings().formTextColor, selectedEngine() );
    gdi().textOutRect( position().x + 48, position().y + 22, size().x - 40, 40, _romInfoText.c_str(), selectedEngine() );

    gdi().textOutRect( position().x + 8, position().y + 64 , size().x - 8, 40, _filenameText.c_str(), selectedEngine() );
    gdi().textOutRect( position().x + 8, position().y + 64 + 14, size().x - 8, 40, _fileDateText.c_str(), selectedEngine() );
    gdi().textOutRect( position().x + 8, position().y + 64 + 14 + 14, size().x - 8, 40, _fileSizeText.c_str(), selectedEngine() );
    gdi().textOutRect( position().x + 8, position().y + 64 + 14 + 14 + 14, size().x - 8, 40, _saveTypeText.c_str(), selectedEngine() );

    cForm::draw();

}

bool cRomInfoWnd::process( const akui::cMessage & msg )
{
    bool ret = false;

    ret = cForm::process( msg );

    if( !ret ) {
        if( msg.id() > cMessage::keyMessageStart && msg.id()
            < cMessage::keyMessageEnd )
        {
            ret = processKeyMessage( (cKeyMessage &)msg );
        }
    }
    return ret;
}

bool cRomInfoWnd::processKeyMessage( const cKeyMessage & msg )
{
    bool ret = false;
    if( msg.id() == cMessage::keyDown )
    {
        switch( msg.keyCode() )
        {
        case cKeyMessage::UI_KEY_A:
        case cKeyMessage::UI_KEY_B:
            onOK();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_Y:
            pressSaveType();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_X:
            if(_buttonCheats.isVisible())
            {
              pressCheats();
            }
            else if(_buttonFlash.isVisible())
            {
              pressFlash();
            }
            ret = true;
            break;
        case cKeyMessage::UI_KEY_L:
            pressCopy();
            ret = true;
            break;
        default:
            {}
        };
    }

    return ret;
}


cWindow& cRomInfoWnd::loadAppearance(const std::string& aFileName )
{
    _renderDesc.loadData( SFN_FORM_TITLE_L, SFN_FORM_TITLE_R, SFN_FORM_TITLE_M );
    _renderDesc.setTitleText( _text );
    return *this;
}

static std::string getFriendlyFileSizeString( u64 size )
{
  std::string fileSize;
  std::string sizeUnit;
  if( size<1024)
  {
    fileSize=formatString( "%d", size);
    sizeUnit=" Byte";
  }
  else
  {
    u32 divider;
    if(size<1024*1024)
    {
      divider=1024;
      sizeUnit=" KB";
    }
    else if(size<1024*1024*1024)
    {
        divider=1024*1024;
        sizeUnit=" MB";
    }
    else
    {
      divider=1024*1024*1024;
      sizeUnit=" GB";
    }
    fileSize=formatString("%d.%02d",(u32)(size/divider),(u32)((size*100+(divider>>1))/divider%100));
  }
  return fileSize+sizeUnit;
}

void cRomInfoWnd::setDiskInfo(void)
{
    u64 total = 0;
    u64 used = 0;
    u64 freeSpace = 0;

    if( !getDiskSpaceInfo( _fullName, total, used, freeSpace ) )
        return;

    _filenameText = formatString( LANG("disk info", "total").c_str(), getFriendlyFileSizeString(total).c_str() );
    _fileDateText = formatString( LANG("disk info", "used").c_str(), getFriendlyFileSizeString(used).c_str() );
    _fileSizeText = formatString( LANG("disk info", "free").c_str(), getFriendlyFileSizeString(freeSpace).c_str() );

}

void cRomInfoWnd::setFileInfo( const std::string & fullName, const std::string & showName )
{
    _fullName=fullName;

    if( "" == showName ) {
        dbg_printf("show name %s\n", showName.c_str() );
        return;
    }

    struct stat st;
    if( -1 == stat( fullName.c_str(), &st ) ) {
        return;
    }

    if( "fat0:/" == fullName || "fat1:/" == fullName ) {
        setDiskInfo();
        return;
    }

    if( showName.size() > 0 && '/' == showName[showName.size()-1] )
        _filenameText = showName.substr( 0, showName.size() - 1 );
    else
        _filenameText = showName;

    //dbg_printf("st.st_mtime %d\n", st.st_mtime );
    //struct tm * filetime = localtime(&st.st_mtime);

    struct tm * filetime = gmtime(&st.st_mtime);

    _fileDateText = formatString( LANG("rom info", "file date").c_str(),
        filetime->tm_year + 1900, filetime->tm_mon+1, filetime->tm_mday,
        filetime->tm_hour, filetime->tm_min, filetime->tm_sec );

    dbg_printf("st.st_mtime %d\n", (u32)st.st_mtime );
    dbg_printf( "%d-%d-%d %02d:%02d:%02d\n",
        filetime->tm_year + 1900, filetime->tm_mon+1, filetime->tm_mday,
        filetime->tm_hour, filetime->tm_min, filetime->tm_sec );

    _fileSizeText = formatString( LANG("rom info", "file size").c_str(), getFriendlyFileSizeString(st.st_size).c_str() );
    _size = st.st_size;
}

void cRomInfoWnd::setRomInfo( const DSRomInfo & romInfo )
{
    _romInfo = romInfo;

    _romInfoText = unicode_to_local_string( _romInfo.banner().titles[gs().language], 128, NULL );

    _buttonSaveType.hide();
    _buttonFlash.hide();
    _buttonCopy.hide();
    _buttonCheats.hide();
    if( _romInfo.isDSRom()&&!_romInfo.isHomebrew()  )
    {
        const char * stLangStrings[] = { "Unknown", "No Save", "4K", "64K", "512K", "2M", "4M", "8M", "Unknown", "Unknown", "1M", "16M", "32M", "64M" };
        if( _romInfo.saveInfo().saveType < sizeof(stLangStrings)/sizeof(char*) )
        {
            _saveTypeText = formatString( LANG("rom info", "save type").c_str(),
                LANG("save type", stLangStrings[_romInfo.saveInfo().saveType]).c_str() );
        }
        else _saveTypeText = "";
        addCode();
        _buttonSaveType.show();
        if(gs().cheatDB) _buttonCheats.show();
    }
    else if(_romInfo.isGbaRom())
    {
        _buttonFlash.show();
        _buttonSaveType.setText( "\x03 " + LANG( "exp window", "flash to nor" ) );
        if(CGbaLoader::CheckPSRAM(_size)) _buttonCopy.show();
        addCode();
    }
}

const DSRomInfo & cRomInfoWnd::getRomInfo()
{
    return _romInfo;
}

void cRomInfoWnd::setSaves(const std::vector<std::string>* saves)
{
  _saves=saves;
}

void cRomInfoWnd::onOK()
{
    cForm::onOK();
}


void cRomInfoWnd::onShow()
{
    centerScreen();
}

#if defined(_STORAGE_rpg)
#define ITEM_SAVETYPE     0,0
#define ITEM_DOWNLOADPLAY 0,1
#define ITEM_DMA          0,2
#define ITEM_PROTECTION   0,3
#define ITEM_LINKAGE      0,4
#define ITEM_RUMBLE       0,5

#define ITEM_CHEATS       1,0
#define ITEM_SOFTRESET    1,1
#define ITEM_SDSAVE       1,2
#define ITEM_SAVESLOT     1,3
#define ITEM_ICON         1,4
#define ITEM_LANGUAGE     1,5
#elif defined(_STORAGE_r4) || defined(_STORAGE_r4idsn)
#define ITEM_SAVETYPE     0,0
#define ITEM_LINKAGE      0,1
#define ITEM_RUMBLE       0,2

#define ITEM_CHEATS       1,0
#define ITEM_SOFTRESET    1,1
#define ITEM_SAVESLOT     1,2
#define ITEM_ICON         1,3
#define ITEM_LANGUAGE     1,4
#elif defined(_STORAGE_ak2i)
#define ITEM_SAVETYPE     0,0
#define ITEM_DOWNLOADPLAY 0,1
#define ITEM_DMA          0,2
#define ITEM_PROTECTION   0,3
#define ITEM_LINKAGE      0,4
#define ITEM_RUMBLE       0,5

#define ITEM_CHEATS       1,0
#define ITEM_SOFTRESET    1,1
#define ITEM_SAVESLOT     1,2
#define ITEM_ICON         1,3
#define ITEM_LANGUAGE     1,4
#endif

void cRomInfoWnd::pressSaveType(void)
{
    if(!_romInfo.isDSRom()||_romInfo.isHomebrew()) return;

    cSettingWnd settingWnd( 0,0, 252, 188, this, LANG("save type", "tab1" ) );
    std::vector< std::string > _values;
    _values.push_back( LANG("save type", "Unknown" ) );
    _values.push_back( LANG("save type", "No Save" ) );
    _values.push_back( LANG("save type", "4K" ) );
    _values.push_back( LANG("save type", "64K" ) );
    _values.push_back( LANG("save type", "512K" ) );
    _values.push_back( LANG("save type", "1M" ) );
    _values.push_back( LANG("save type", "2M" ) );
    _values.push_back( LANG("save type", "4M" ) );
    _values.push_back( LANG("save type", "8M" ) );
    _values.push_back( LANG("save type", "16M" ) );
    _values.push_back( LANG("save type", "32M" ) );
    _values.push_back( LANG("save type", "64M" ) );
    settingWnd.addSettingItem( LANG("save type", "text" ), _values, cSaveManager::SaveTypeToDisplaySaveType((SAVE_TYPE)_romInfo.saveInfo().saveType) );

#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i)
    _values.clear();
    _values.push_back( LANG("switches", "Disable" ) );
    _values.push_back( LANG("switches", "Enable" ) );
    settingWnd.addSettingItem( LANG("patches", "download play" ), _values, _romInfo.saveInfo().getDownloadPlay() );

    _values.clear();
    _values.push_back( LANG("switches", "Disable" ) );
    _values.push_back( LANG("switches", "Enable" ) );
    _values.push_back(formatString(LANG("switches","Global").c_str(),gs().dma?LANG("switches","Enable").c_str():LANG("switches","Disable").c_str()));
    settingWnd.addSettingItem( LANG("patches", "dma" ), _values, _romInfo.saveInfo().getDMA() );
#endif

    _values.clear();
    _values.push_back( LANG("switches", "Disable" ) );
    _values.push_back( LANG("switches", "Enable" ) );
#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i)
    settingWnd.addSettingItem( LANG("patches", "protection" ), _values, _romInfo.saveInfo().getProtection() );
#endif
    settingWnd.addSettingItem( LANG("patches", "linkage" ), _values, _romInfo.saveInfo().getLinkage() );

    _values.clear();
    const char* rumbleLang[]={"off","low","mid","high"};
    for( size_t i = 0; i < 4; ++i ) {
        _values.push_back( LANG( "exp window", rumbleLang[i] ) );
    }
    settingWnd.addSettingItem( LANG( "exp window","strength"), _values, _romInfo.saveInfo().getRumble() );

    settingWnd.addSettingTab(LANG("save type","tab2"));

    _values.clear();
    _values.push_back( LANG("switches", "Disable" ) );
    _values.push_back( LANG("switches", "Enable" ) );
    _values.push_back(formatString(LANG("switches","Global").c_str(),gs().cheats?LANG("switches","Enable").c_str():LANG("switches","Disable").c_str()));
    settingWnd.addSettingItem( LANG("patches", "cheating system" ), _values, _romInfo.saveInfo().getCheat() );

    _values.clear();
    _values.push_back( LANG("switches", "Disable" ) );
    _values.push_back( LANG("switches", "Enable" ) );
    _values.push_back(formatString(LANG("switches","Global").c_str(),gs().softreset?LANG("switches","Enable").c_str():LANG("switches","Disable").c_str()));
    settingWnd.addSettingItem( LANG("patches", "reset in game" ), _values, _romInfo.saveInfo().getSoftReset() );

#if defined(_STORAGE_rpg)
    _values.clear();
    _values.push_back( LANG("switches", "Disable" ) );
    _values.push_back( LANG("switches", "Enable" ) );
    _values.push_back(formatString(LANG("switches","Global").c_str(),gs().sdsave?LANG("switches","Enable").c_str():LANG("switches","Disable").c_str()));
    settingWnd.addSettingItem( LANG("patches", "sd save" ), _values, _romInfo.saveInfo().getSDSave() );
#endif

    _values.clear();
    std::string slotValue;
    for(size_t ii=0;ii<4;++ii)
    {
      if(ii) slotValue='0'+ii;
      else slotValue=LANG("save type","default");
      if(SlotExists(ii)) slotValue+="*";
      _values.push_back(slotValue);
    }
    settingWnd.addSettingItem( LANG("save type", "save slot" ), _values, _romInfo.saveInfo().getSlot() );

    _values.clear();
    _values.push_back( LANG("icon", "transparent" ) );
    _values.push_back( LANG("icon", "as is" ) );
    _values.push_back( LANG("icon", "firmware" ) );
    settingWnd.addSettingItem( LANG("icon", "icon" ), _values, _romInfo.saveInfo().getIcon() );

    _values.clear();
    _values.push_back( LANG("save type", "default" ) );
    _values.push_back( LANG("language", "ja" ) );
    _values.push_back( LANG("language", "en" ) );
    _values.push_back( LANG("language", "fr" ) );
    _values.push_back( LANG("language", "de" ) );
    _values.push_back( LANG("language", "it" ) );
    _values.push_back( LANG("language", "es" ) );
    settingWnd.addSettingItem( LANG("language", "text" ), _values, _romInfo.saveInfo().getLanguage() );

    _settingWnd = &settingWnd;
    settingWnd.addYButton(LANG("rom info","copy slot"),this,&cRomInfoWnd::pressCopySlot);

    u32 ret = settingWnd.doModal();
    _settingWnd = NULL;
    if( ID_CANCEL == ret )
        return;

    _romInfo.saveInfo().saveType = cSaveManager::DisplaySaveTypeToSaveType((DISPLAY_SAVE_TYPE)settingWnd.getItemSelection(ITEM_SAVETYPE));

    const char * stLangStrings[] = { "Unknown", "No Save", "4K", "64K", "512K", "2M", "4M", "8M", "Unknown", "Unknown", "1M", "16M", "32M", "64M" };
    if( _romInfo.saveInfo().saveType < sizeof(stLangStrings)/sizeof(char*) ) {
        _saveTypeText = formatString( LANG("rom info", "save type").c_str(),
            LANG("save type", stLangStrings[_romInfo.saveInfo().saveType]).c_str() );
        addCode();
    }
    _romInfo.saveInfo().setFlags
    (
      settingWnd.getItemSelection(ITEM_RUMBLE),
#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i)
      settingWnd.getItemSelection(ITEM_DOWNLOADPLAY),
#elif defined(_STORAGE_r4) || defined(_STORAGE_r4idsn)
      0,
#endif
      settingWnd.getItemSelection(ITEM_SOFTRESET),
      settingWnd.getItemSelection(ITEM_CHEATS),
      settingWnd.getItemSelection(ITEM_SAVESLOT),
#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i)
      settingWnd.getItemSelection(ITEM_DMA),
      settingWnd.getItemSelection(ITEM_PROTECTION),
#elif defined(_STORAGE_r4) || defined(_STORAGE_r4idsn)
      2,
      0,
#endif
      settingWnd.getItemSelection(ITEM_LINKAGE),
      settingWnd.getItemSelection(ITEM_ICON),
#if defined(_STORAGE_rpg)
      settingWnd.getItemSelection(ITEM_SDSAVE),
#elif defined(_STORAGE_r4) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
      2,
#endif
      settingWnd.getItemSelection(ITEM_LANGUAGE)
    );

    saveManager().updateCustomSaveList( _romInfo.saveInfo() );
}

void cRomInfoWnd::pressCopySlot(void)
{
  if(_settingWnd)
  {
    u8 slot=_settingWnd->getItemSelection(ITEM_SAVESLOT);
    std::string saveSrc=cSaveManager::generateSaveName(_fullName,slot);
    struct stat st;
    if(0==stat(saveSrc.c_str(),&st))
    {
      std::vector<std::string> values;
      std::string slotValue,srcValue;
      for(size_t ii=0;ii<4;++ii)
      {
        if(ii) slotValue='0'+ii;
        else slotValue=LANG("save type","default");
        if(slot!=ii)
        {
          if(SlotExists(ii)) slotValue+="*";
          values.push_back(slotValue);
        }
        else srcValue=slotValue;
      }
      cSettingWnd settingWnd(0,0,252,188,NULL,LANG("saveslot","title")+": "+srcValue);
      settingWnd.addSettingItem(LANG("saveslot","target slot" ),values,0);
      settingWnd.setConfirmMessage(LANG("saveslot","confirm"));
      u32 ret=settingWnd.doModal();
      if(ret==ID_CANCEL) return;

      u8 new_slot=settingWnd.getItemSelection(0,0);
      if(new_slot>=slot) ++new_slot;
      std::string saveDst=cSaveManager::generateSaveName(_fullName,new_slot);
      copyFile(saveSrc,saveDst,false,st.st_size);
    }
  }
}

void cRomInfoWnd::pressFlash(void)
{
  if(_romInfo.isGbaRom())
  {
    CGbaLoader(_fullName).Load(true,true);
  }
}

void cRomInfoWnd::pressCopy(void)
{
  if(_romInfo.isGbaRom()&&CGbaLoader::CheckPSRAM(_size))
  {
    CGbaLoader(_fullName).Load(false,true);
  }
}

void cRomInfoWnd::pressCheats(void)
{
  if(!_romInfo.isDSRom()||_romInfo.isHomebrew()) return;
  showCheats(_fullName);
}

void cRomInfoWnd::showCheats(const std::string& aFileName)
{
  u32 w=256;
  u32 h=179;
  cCheatWnd cheatWnd((256-w)/2,(192-h)/2,w,h,NULL,LANG("cheats","title"));
  if(cheatWnd.parse(aFileName)) cheatWnd.doModal();
}

void cRomInfoWnd::addCode(void)
{
  char gameCode[5];
  memcpy(gameCode,_romInfo.saveInfo().gameCode,sizeof(_romInfo.saveInfo().gameCode));
  gameCode[4]=0;
  if(_saveTypeText.length()) _saveTypeText+=", ";
  _saveTypeText+=formatString(LANG("rom info","game code").c_str(),gameCode);
  if(_romInfo.version()>0)
  {
    _saveTypeText+=formatString("v%02d",_romInfo.version());
  }
}

bool cRomInfoWnd::SlotExists(u8 slot)
{
  std::string save=cSaveManager::generateSaveName(_fullName,slot);
  if(_saves)
  {
    if(_saves->size()&&akui::binary_find(_saves->begin(),_saves->end(),save,stringComp)!=_saves->end()) return true;
  }
  else
  {
    struct stat st;
    if(0==stat(save.c_str(),&st)) return true;
  }
  return false;
}
