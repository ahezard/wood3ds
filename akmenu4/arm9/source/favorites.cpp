/*
    favorites.cpp
    Copyright (C) 2008 somebody
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

#include "favorites.h"
#include "inifile.h"
#include "systemfilenames.h"

bool cFavorites::AddToFavorites(const std::string& aFileName)
{
  CIniFile ini(SFN_FAVORITES);
  std::vector<std::string> items;
  ini.GetStringVector("main","list",items,'|');
  for(size_t ii=0;ii<items.size();++ii)
  {
    if(items[ii]==aFileName) return false;
  }
  items.push_back(aFileName);
  ini.SetStringVector("main","list",items,'|');
  ini.SaveIniFile(SFN_FAVORITES);
  return true;
}

bool cFavorites::UpdateFavorites(const std::string& aOldFileName,const std::string& aNewFileName)
{
  CIniFile ini(SFN_FAVORITES);
  std::vector<std::string> items;
  ini.GetStringVector("main","list",items,'|');
  for(size_t ii=0;ii<items.size();++ii)
  {
    if(items[ii]==aOldFileName)
    {
      items[ii]=aNewFileName;
      ini.SetStringVector("main","list",items,'|');
      ini.SaveIniFile(SFN_FAVORITES);
      return true;
    }
  }
  return false;
}

bool cFavorites::RemoveFromFavorites(const std::string& aFileName)
{
  CIniFile ini(SFN_FAVORITES);
  std::vector<std::string> items;
  ini.GetStringVector("main","list",items,'|');
  for(size_t ii=0;ii<items.size();++ii)
  {
    if(items[ii]==aFileName)
    {
      items.erase(items.begin()+ii);
      ini.SetStringVector("main","list",items,'|');
      ini.SaveIniFile(SFN_FAVORITES);
      return true;
    }
  }
  return false;
}
