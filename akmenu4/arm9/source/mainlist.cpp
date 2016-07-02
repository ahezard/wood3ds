/*
    mainlist.cpp
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

//à

#include <sys/dir.h>
#include <elm.h>
#define ATTRIB_HID 0x02
#include "mainlist.h"
#include "files.h"
#include "dbgtool.h"
#include "startmenu.h"
#include "systemfilenames.h"
#include "romloader.h"
#include "windowmanager.h"
#include "../../share/timetool.h"
#include "../../share/memtool.h"
#include "inifile.h"
#include "unknown_banner_bin.h"
#include "nds_save_banner_bin.h"
#include "nand_banner_bin.h"
#include "microsd_banner_bin.h"
#include "gba_banner_bin.h"
#include "folder_banner_bin.h"
#include "progresswnd.h"
#include "language.h"
#include "unicode.h"

using namespace akui;

cMainList::cMainList( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cListView( x, y, w, h, parent, text ),_showAllFiles(false),
#if defined(_STORAGE_rpg)
_topCount(4),_topuSD(1),_topSlot2(2),_topFavorites(3)
#elif defined(_STORAGE_r4) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
_topCount(3),_topuSD(0),_topSlot2(1),_topFavorites(2)
#endif
{
    _viewMode = VM_LIST;
    _activeIconScale = 1;
    _activeIcon.hide();
    _activeIcon.update();
    animationManager().addAnimation( &_activeIcon );
    dbg_printf("_activeIcon.init\n");
#if defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
  fifoSendValue32(FIFO_USER_01,MENU_MSG_SYSTEM);
  while(!fifoCheckValue32(FIFO_USER_02));
  u32 system=fifoGetValue32(FIFO_USER_02);
  if(2==system) //dsi
  {
    _topCount=2;
    _topSlot2=2;
    _topFavorites=1;
  }
#endif
}

cMainList::~cMainList()
{}

int cMainList::init()
{
  CIniFile ini(SFN_UI_SETTINGS);
  _textColor=ini.GetInt("main list","textColor",RGB15(7,7,7));
  _textColorHilight=ini.GetInt("main list","textColorHilight",RGB15(31,0,31));
  _selectionBarColor1=ini.GetInt("main list","selectionBarColor1",RGB15(16,20,24));
  _selectionBarColor2=ini.GetInt("main list","selectionBarColor2",RGB15(20,25,0));
  _selectionBarOpacity=ini.GetInt("main list","selectionBarOpacity",100);

  //selectedRowClicked.connect(this,&cMainList::executeSelected);

  insertColumn(ICON_COLUMN,"icon",0);
  insertColumn(SHOWNAME_COLUMN,"showName",0);
  insertColumn(INTERNALNAME_COLUMN,"internalName",0);
  insertColumn(REALNAME_COLUMN,"realName",0); // hidden column for contain real filename
  insertColumn(SAVETYPE_COLUMN,"saveType",0);
  insertColumn(FILESIZE_COLUMN,"fileSize",0);

  setViewMode((cMainList::VIEW_MODE)gs().viewMode);

  _activeIcon.hide();

  return 1;
}

static bool itemSortComp( const cListView::itemVector & item1, const cListView::itemVector & item2 ) {
    const std::string & fn1 = item1[cMainList::SHOWNAME_COLUMN].text();
    const std::string & fn2 = item2[cMainList::SHOWNAME_COLUMN].text();
    if( "../" == fn1 )
        return true;
    if( "../" == fn2 )
        return false;
    if( '/' == fn1[fn1.size()-1] && '/' == fn2[fn2.size()-1] )
        return fn1 < fn2;
    if( '/' == fn1[fn1.size()-1] )
        return true;
    if( '/' == fn2[fn2.size()-1] )
        return false;

    return fn1 < fn2;
}

static bool extnameFilter( const std::vector<std::string> & extNames, std::string extName )
{
    if( 0 == extNames.size() )
        return true;

    for( size_t i = 0; i < extName.size(); ++i )
        extName[i] = tolower( extName[i] );

    for( size_t i = 0; i < extNames.size(); ++i ) {
        if( extName == extNames[i] ) {
            return true;
        }
    }
    return false;
}

bool cMainList::enterDir( const std::string & dirName )
{
    _saves.clear();
    if( "..." == dirName ) // select RPG or SD card
    {
        removeAllRows();
        _romInfoList.clear();
        for( size_t i = 0; i < _topCount; ++i ) {
            std::vector< std::string > a_row;
            a_row.push_back( "" ); // make a space for icon
            DSRomInfo rominfo;
#if defined(_STORAGE_rpg)
            if( 0 == i ) {
                a_row.push_back( LANG("mainlist","flash memory") );
                a_row.push_back( "" );
                a_row.push_back( "fat0:/" );
                rominfo.setBanner("nand",nand_banner_bin);
            }
            else
#endif
            if( _topuSD == i ) {
                a_row.push_back( LANG("mainlist","microsd card") );
                a_row.push_back( "" );
                a_row.push_back( SD_ROOT );
                rominfo.setBanner("usd",microsd_banner_bin);
            } else if( _topSlot2 == i ) {
                a_row.push_back( LANG("mainlist","slot2 card") );
                a_row.push_back( "" );
                a_row.push_back( "slot2:/" );
                rominfo.setBanner("slot2",gba_banner_bin);
            } else if( _topFavorites == i ) {
                a_row.push_back( LANG("mainlist","favorites") );
                a_row.push_back( "" );
                a_row.push_back( "favorites:/" );
                rominfo.setBanner("folder",folder_banner_bin);
            }
            insertRow( i, a_row );
            _romInfoList.push_back( rominfo );
        }
        _currentDir = "";
        directoryChanged();
        return true;
    }

    if( "slot2:/" == dirName ) {
        _currentDir = "";
        directoryChanged();
        return true;
    }

    bool favorites=("favorites:/"==dirName);
    DIR_ITER* dir=NULL;

    if(!favorites)
    {
      dir = diropen( dirName.c_str() );

      if (dir == NULL) {
          if( SD_ROOT == dirName ) {
              std::string title = LANG( "sd card error", "title" );
              std::string sdError = LANG( "sd card error", "text" );
              messageBox( NULL, title, sdError, MB_OK );
          }
          dbg_printf ("Unable to open directory<%s>.\n", dirName.c_str() );
          return false;
      }
    }

    removeAllRows();
    _romInfoList.clear();

    std::vector< std::string > extNames;
    extNames.push_back( ".nds" );
    extNames.push_back( ".ids" );
    if( gs().showGbaRoms > 0 )
        extNames.push_back( ".gba" );
    if( gs().fileListType > 0 )
        extNames.push_back( ".sav" );
    if( _showAllFiles || gs().fileListType > 1 )
        extNames.clear();
    std::vector< std::string > savNames;
    savNames.push_back( ".sav" );

    // insert Ò»¶ÑÎÄ¼þ, Á½ÁÐ£¬Ò»ÁÐ×÷ÎªÏÔÊ¾£¬Ò»ÁÐ×÷ÎªÕæÊµÎÄ¼þÃû
    struct stat st;
    char longFilename[MAX_FILENAME_LENGTH];
    std::string extName;
    u8 attr=0;

    // list dir
    {
        cwl();
        if(favorites)
        {
          CIniFile ini(SFN_FAVORITES);

          std::vector<std::string> items;
          ini.GetStringVector("main","list",items,'|');
          for(size_t ii=0;ii<items.size();++ii)
          {
            u32 row_count=getRowCount();
            std::vector<std::string> a_row;
            a_row.push_back(""); // make a space for icon

            size_t pos=items[ii].rfind('/',items[ii].length()-2);
            if(pos==items[ii].npos)
            {
              a_row.push_back(items[ii]); //show name
            }
            else
            {
              a_row.push_back(items[ii].substr(pos+1,items[ii].npos)); //show name
            }
            a_row.push_back(""); // make a space for internal name

            a_row.push_back(items[ii]); //real name
            size_t insertPos(row_count);
            insertRow(insertPos,a_row);
            DSRomInfo rominfo;
            _romInfoList.push_back(rominfo);
          }
        }
        else if(dir)
        {
          while(dirnext(dir,longFilename,&st)==0)
          {
              attr=st.st_spare1;
              std::string lfn( longFilename );

              // st.st_mode & S_IFDIR indicates a directory
              size_t lastDotPos = lfn.find_last_of( '.' );
              if( lfn.npos != lastDotPos )
                  extName = lfn.substr( lastDotPos );
              else
                  extName = "";

              dbg_printf( "%s: %s %s\n", (st.st_mode & S_IFDIR ? " DIR" : "FILE"), longFilename, extName.c_str() );
              bool showThis=(st.st_mode & S_IFDIR)?(strcmp(longFilename,".")&&strcmp(longFilename,"..")):extnameFilter( extNames, extName );
              showThis=showThis&&(_showAllFiles||gs().showHiddenFiles||!(attr&ATTRIB_HID));

              // Èç¹ûÓÐºó×ºÃû£¬»òÕßÊÇ¸öÄ¿Â¼£¬¾Ípush½øÈ¥
              if(showThis) {
                  u32 row_count = getRowCount();
                  std::vector< std::string > a_row;
                  a_row.push_back( "" ); // make a space for icon
                  a_row.push_back(lfn); //show name
                  a_row.push_back( "" ); // make a space for internal name

                  a_row.push_back(dirName+lfn); //real name
                  if( st.st_mode & S_IFDIR ) {
                      a_row[SHOWNAME_COLUMN] += "/";
                      a_row[REALNAME_COLUMN] += "/";
                  }
                  size_t insertPos( row_count );
                  insertRow( insertPos, a_row );
                  DSRomInfo rominfo;
                  _romInfoList.push_back( rominfo );

              }
              if(extnameFilter(savNames,extName))
              {
                _saves.push_back(dirName+lfn);
              }
          }
          dirclose( dir );
        }
        std::sort( _rows.begin(), _rows.end(), itemSortComp );
        std::sort(_saves.begin(),_saves.end(),stringComp);

        for( size_t ii = 0; ii < _rows.size(); ++ii )
        {
            ////_romInfoList.push_back( rominfo );

            // Õâ¶Î´úÂë»áÒýÆð¿½±´ÎÄ¼þÍê³ÉºóµÄÍ¼±êÏÔÊ¾²»ÕýÈ·£¬ÒòÎªÍ¼±êµÄÄÚÈÝ»¹Ã»ÓÐ±»¶ÁÈë£¬¾ÍÈ¥¸üÐÂÁËactive iconµÄÄÚÈÝ
            //u8 percent = ii * 100 / _rows.size();
            //if( !(percent & 0x07) )
            //    progressWnd().setPercent( percent );


            DSRomInfo & rominfo = _romInfoList[ii];
            std::string filename = _rows[ii][REALNAME_COLUMN].text();
            size_t lastDotPos = filename.find_last_of( '.' );
            if( filename.npos != lastDotPos )
                extName = filename.substr( lastDotPos );
            else
                extName = "";
            for( size_t jj = 0; jj < extName.size(); ++jj )
                extName[jj] = tolower( extName[jj] );


            if( '/' == filename[filename.size()-1] )
            {
                rominfo.setBanner("folder",folder_banner_bin);
            }
            else
            {
              bool allowExt=true,allowUnknown=false;
              if( ".sav" == extName ) {
                  memcpy( &rominfo.banner(), nds_save_banner_bin, sizeof(tNDSBanner) );
              } else if( ".gba" == extName ) {
                  rominfo.MayBeGbaRom(filename);
              } else if( ".nds" != extName && ".ids" != extName ) {
                  memcpy( &rominfo.banner(), unknown_banner_bin, sizeof(tNDSBanner) );
                  allowUnknown=true;
              } else {
                  rominfo.MayBeDSRom(filename);
                  allowExt=false;
              }
              rominfo.setExtIcon(_rows[ii][SHOWNAME_COLUMN].text());
              if(allowExt&&extName.length()&&!rominfo.isExtIcon()) rominfo.setExtIcon(extName.substr(1));
              if(allowUnknown&&!rominfo.isExtIcon()) rominfo.setExtIcon("unknown");
            }
        }
        _currentDir = dirName;
    }

    directoryChanged();

    return true;
}

void cMainList::onSelectChanged( u32 index )
{
    dbg_printf( "%s\n", _rows[index][3].text().c_str() );
}

void cMainList::onSelectedRowClicked( u32 index )
{
    const INPUT & input = getInput();
    //dbg_printf("%d %d", input.touchPt.px, _position.x );
    if( input.touchPt.px > _position.x && input.touchPt.px < _position.x + 32 )
        selectedRowHeadClicked( index );
}

void cMainList::onScrolled( u32 index )
{
    _activeIconScale = 1;
    //updateActiveIcon( CONTENT );
}

void cMainList::backParentDir()
{
    if( "..." == _currentDir )
        return;

    bool fat1=(SD_ROOT==_currentDir),favorites=("favorites:/"==_currentDir);
    if( "fat0:/" == _currentDir || fat1 || favorites || "/" == _currentDir ) {
        enterDir( "..." );
        if(fat1) selectRow(_topuSD);
        if(favorites) selectRow(_topFavorites);
        return;
    }

    size_t pos = _currentDir.rfind( "/", _currentDir.size() - 2 );
    std::string parentDir = _currentDir.substr( 0, pos + 1 );
    dbg_printf( "%s->%s\n", _currentDir.c_str(), parentDir.c_str() );

    std::string oldCurrentDir = _currentDir;

    if( enterDir( parentDir ) ) { // select last entered director
        for( size_t i = 0; i < _rows.size(); ++i ){
            if( parentDir + _rows[i][SHOWNAME_COLUMN].text() == oldCurrentDir ) {
                selectRow( i );
            }
        }
    }
}

std::string cMainList::getSelectedFullPath()
{
    if( !_rows.size() )
        return std::string("");
    return _rows[_selectedRowId][REALNAME_COLUMN].text();
}

std::string cMainList::getSelectedShowName()
{
    if( !_rows.size() )
        return std::string("");
    return _rows[_selectedRowId][SHOWNAME_COLUMN].text();
}

bool cMainList::getRomInfo( u32 rowIndex, DSRomInfo & info ) const
{
    if( rowIndex < _romInfoList.size() ) {
        info = _romInfoList[rowIndex];
        return true;
    }
    else {
        return false;
    }
}

void cMainList::setRomInfo( u32 rowIndex, const DSRomInfo & info )
{
    if( !_romInfoList[rowIndex].isDSRom() )
        return;

    if( rowIndex < _romInfoList.size() ) {
        _romInfoList[rowIndex] = info;
    }
}

void cMainList::draw()
{
    updateInternalNames();
    cListView::draw();
    updateActiveIcon( POSITION );
    drawIcons();
}

void cMainList::drawIcons()         // Ö±½Ó»­¼ÒËã·¨»­ icons
{
    if( VM_LIST != _viewMode ) {
        size_t total = _visibleRowCount;
        if(  total > _rows.size() - _firstVisibleRowId )
            total = _rows.size() - _firstVisibleRowId;

        for( size_t i = 0; i < total; ++i ) {
            // ÕâÀïÍ¼Ïñ³ÊÏÖ±ÈÕæÕýµÄ MAIN buffer ·­×ªÒªÔç£¬ËùÒÔ»áÉÁÒ»ÏÂ
            // ½â¾ö·½·¨ÊÇÔÚ gdi().present Àï±ßÍ³Ò»³ÊÏÖ·­×ª
            if( _firstVisibleRowId + i == _selectedRowId ) {
                if( _activeIcon.visible() ) {
                    continue;
                }
            }
            s32 itemX = _position.x + 1;
            s32 itemY = _position.y + i * _rowHeight + ((_rowHeight-32)>>1)  - 1;
            _romInfoList[_firstVisibleRowId+i].drawDSRomIcon( itemX, itemY, _engine );
        }
    }
}

void cMainList::setViewMode(VIEW_MODE mode)
{
  if(!_columns.size()) return;
  _viewMode=mode;
  switch(_viewMode)
  {
    case VM_LIST:
      _columns[ICON_COLUMN].width=0;
      _columns[SHOWNAME_COLUMN].width=250;
      _columns[INTERNALNAME_COLUMN].width=0;
      arangeColumnsSize();
      setRowHeight(15);
      break;
    case VM_ICON:
      _columns[ICON_COLUMN].width=36;
      _columns[SHOWNAME_COLUMN].width=214;
      _columns[INTERNALNAME_COLUMN].width=0;
      arangeColumnsSize();
      setRowHeight(38);
      break;
    case VM_INTERNAL:
      _columns[ICON_COLUMN].width=36;
      _columns[SHOWNAME_COLUMN].width=0;
      _columns[INTERNALNAME_COLUMN].width=214;
      arangeColumnsSize();
      setRowHeight(38);
      break;
  }
  scrollTo(_selectedRowId-_visibleRowCount+1);
}

void cMainList::updateActiveIcon( bool updateContent )
{
    const INPUT & temp =  getInput();
    bool allowAnimation = true;
    animateIcons(allowAnimation);

    //do not show active icon when hold key to list files. Otherwise the icon will not show correctly.
    if( getInputIdleMs() > 1000 && VM_LIST != _viewMode && allowAnimation && _romInfoList.size() && 0 == temp.keysHeld && gs().Animation)
    {
        if( !_activeIcon.visible()  ) {
            u8 backBuffer[32*32*2];
            zeroMemory( backBuffer, 32 * 32 * 2 );
            _romInfoList[_selectedRowId].drawDSRomIconMem( backBuffer );
            memcpy( _activeIcon.buffer(), backBuffer, 32 * 32 * 2 );
            _activeIcon.setBufferChanged();

            s32 itemX = _position.x;
            s32 itemY = _position.y + (_selectedRowId - _firstVisibleRowId) * _rowHeight + ((_rowHeight-32)>>1) - 1;
            _activeIcon.setPosition( itemX, itemY );
            _activeIcon.show();
            dbg_printf("sel %d ac ico x %d y %d\n", _selectedRowId, itemX, itemY );
            for( u8 i = 0; i < 8; ++i )
                dbg_printf("%02x", backBuffer[i] );
            dbg_printf("\n");
        }
    } else {
        if( _activeIcon.visible() ) {
            _activeIcon.hide();
            cwl();
        }
    }
}

std::string cMainList::getCurrentDir()
{
    return _currentDir;
}

void cMainList::updateInternalNames(void)
{
  if(_viewMode==VM_INTERNAL)
  {
    size_t total=_visibleRowCount;
    if(total>_rows.size()-_firstVisibleRowId) total=_rows.size()-_firstVisibleRowId;
    for(size_t ii=0;ii<total;++ii)
    {
      if(0==_rows[_firstVisibleRowId+ii][INTERNALNAME_COLUMN].text().length())
      {
        if(_romInfoList[_firstVisibleRowId+ii].isDSRom())
        {
          _rows[_firstVisibleRowId+ii][INTERNALNAME_COLUMN].setText(unicode_to_local_string(_romInfoList[_firstVisibleRowId+ii].banner().titles[gs().language],128,NULL));
        }
        else
        {
          _rows[_firstVisibleRowId+ii][INTERNALNAME_COLUMN].setText(_rows[_firstVisibleRowId+ii][SHOWNAME_COLUMN].text());
        }
      }
    }
  }
}

bool cMainList::IsFavorites(void)
{
  return ("favorites:/"==_currentDir);
}

const std::vector<std::string>* cMainList::Saves(void)
{
  return IsFavorites()?NULL:&_saves;
}

void cMainList::SwitchShowAllFiles(void)
{
  _showAllFiles=!_showAllFiles;
  enterDir(getCurrentDir());
}
