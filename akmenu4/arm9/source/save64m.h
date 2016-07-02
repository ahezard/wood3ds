/*
    save64m.h
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

#ifndef __SAVE64M_H__
#define __SAVE64M_H__

#include <nds.h>
#include <elm.h>

#if defined(_STORAGE_rpg) || defined(_STORAGE_r4)

#include <sys/iosupport.h>
#include <string>

class cSave64m
{
  private:
    enum
    {
      KMaxSave=(32*1024*1024)/512 //32 megabytes
    };
  private:
    static void IterateFileCluster(int aFileDes,u32* aFatMap);
    static bool GenerateSaveMapInternal(const char* aPath,u32* aFatMap);
  public:
#if defined(_STORAGE_rpg)
    static bool GenerateSaveMap(const std::string& aRomFilename,u8 aSlot);
#elif defined(_STORAGE_r4)
    static bool GenerateSaveMap(std::string& aSaveName);
#endif
};

#endif

#endif
