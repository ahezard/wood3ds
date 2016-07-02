/*
    gbaloader.h
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

#ifndef __GBALOADER_H__
#define __GBALOADER_H__

class CGbaLoader
{
  private:
    std::string iFileName;
    u32 iSize;
  private:
    CGbaLoader();
    static void LoadBorder(void);
    static void BootGBA(void);
    bool LoadPSRAM(void);
    bool LoadNor(void);
    bool LoadInternal(bool nor,bool force);
    void InitNor(void);
    void InitPSRAM(void);
    bool StoreOldSave(std::string& aFileName);
    bool CheckLink(void);
  public:
    CGbaLoader(const std::string& aFileName);
    bool Load(bool aForce,bool aNotStart);
    static bool CheckPSRAM(u32 aSize);
    static void StartGBA(void);
};

#endif
