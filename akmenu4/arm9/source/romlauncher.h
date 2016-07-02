/*
    romlauncher.h
    Copyright (C) 2010 yellow wood goblin

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

#ifndef __ROMLAUNCHER_H__
#define __ROMLAUNCHER_H__

#include "dsrom.h"

enum TLaunchResult
{
  ELaunchRomOk,
  ELaunchSDOnly,
  ELaunchRestoreFail,
  ELaunchSlowSD,
  ELaunchNoFreeSpace
};

TLaunchResult launchRom(const std::string& aFullPath,DSRomInfo& aRomInfo,bool aMenu);
void autoLaunchRom(const std::string& aFullPath);

#endif
