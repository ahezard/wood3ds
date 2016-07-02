/*
    favorites.h
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

#ifndef __FAVORITES_H__
#define __FAVORITES_H__

#include <string>

class cFavorites
{
  public:
    static bool AddToFavorites(const std::string& aFileName);
    static bool UpdateFavorites(const std::string& aOldFileName,const std::string& aNewFileName);
    static bool RemoveFromFavorites(const std::string& aFileName);
};

#endif
