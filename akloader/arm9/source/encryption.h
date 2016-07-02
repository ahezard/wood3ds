/*
    encryption.h
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

#ifndef __ENCRYPTION_H__
#define __ENCRYPTION_H__

#include <nds.h>

class CEncryption
{
  private:
    u32 iKeyCode[3];
    u32 iGameCode;
    u32* iCardHash;
  private: //encryption
    void InitKey(u32 aGameCode,bool aType);
    void ApplyKey(void);
    void CryptUp(u32* aPtr);
    void CryptDown(u32* aPtr);
  public:
    CEncryption();
    ~CEncryption();
    void DecryptSecureArea(void);
};

#ifdef __cplusplus
extern "C" {
#endif
u32 bswap(u32 in);
#ifdef __cplusplus
}
#endif

void Decrypt(void);

#endif
