/*
    patches_cm.cpp
    Copyright (C) 2008 somebody
    Copyright (C) 2009-2010 yellow wood goblin

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

#include "patches.h"
#include <string.h>
#include "repatch_bin.h"
#include "repatch_pokemon_bin.h"
#include "repatch9_bin.h"
#include "unprot_001_bin.h"
#include "unprot_002_bin.h"
#include "unprot_003_bin.h"
#include "unprot_004_bin.h"
#include "unprot_005_bin.h"
#include "unprot_006_bin.h"
#include "unprot_007_bin.h"
#include "unprot_008_bin.h"
#include "unprot_011_bin.h"
#include "unprot_012_bin.h"
#include "unpatch9_bin.h"

void CDsPatcher::PatchDumbSave(void)
{
  switch(GameCode())
  {
    case 0x4a334259: //YB3J: "2048 - Bokujou Monogatari - KiraKira Taiyou to Nakama-tachi (Japan)"
      //the game give 10s to save. increase this to 20s.
      PatchMem(KArm9,0x020955ec,0xe3500f96); //cmp r0, #0x258
      break;
    case 0x45334259: //YB3E: 4439 - Harvest Moon - Sunshine Islands (USA)
      //the game give 10s to save. increase this to 20s.
      PatchMem(KArm9,0x0207d498,0xe3500f96); //cmp r0, #0x258
      break;
    case 0x50334259: //YB3P: 5359 - Harvest Moon DS - Sunshine Islands (Europe) (En,Fr,De,Es,It)
      //the game give 10s to save. increase this to 20s.
      PatchMem(KArm9,0x02072da8,0xe3500f96); //cmp r0, #0x258
      break;
    case 0x4a4b5049: //IPKJ: "4168 - Pocket Monster - Heart Gold (Japan)"
    case 0x4a475049: //IPGJ: "4169 - Pocket Monster - Soul Silver (Japan)"
      //the game use uncommon backup command 0x08 for identify.
      NeedUncompressed();
      PatchMem(KArm9,0x020dda98,0xe3a000aa); //mov r0, #0xaa
      break;
    case 0x4b475049: //IPGK: 4698 - Pocket Monsters - Soul Silver (Korea)
      //the game use uncommon backup command 0x08 for identify.
      NeedUncompressed();
      PatchMem(KArm9,0x020de90c,0xe3a000aa); //mov r0, #0xaa
      break;
    case 0x4b4b5049: //IPKK: 4701 - Pocket Monsters - Heart Gold (Korea)
      //the game use uncommon backup command 0x08 for identify.
      NeedUncompressed();
      PatchMem(KArm9,0x020de914,0xe3a000aa); //mov r0, #0xaa
      break;
    case 0x464b5049: //IPKF: 4786 - Pokemon - Version or HeartGold (France)
    case 0x454b5049: //IPKE: 4787 - Pokemon - HeartGold Version (USA) & 4839 - Pokemon - HeartGold (Europe) (Rev 10)
    case 0x45475049: //IPGE: 4788 - Pokemon - SoulSilver Version (USA) & 4840 - Pokemon - SoulSilver (Europe) (Rev 10)
    case 0x46475049: //IPGF: 4791 - Pokemon - Version Argent SoulSilver (France)
    case 0x444b5049: //IPKD: 4827 - Pokemon - Goldene Edition HeartGold (Germany)
    case 0x44475049: //IPGD: 4828 - Pokemon - Silberne Edition SoulSilver (Germany)
    case 0x53475049: //IPGS: 4833 - Pokemon - Edicion Plata SoulSilver (Spain)
    case 0x494b5049: //IPGI: 4848 - Pokemon - Versione Argento SoulSilver (Italy)
    case 0x49475049: //IPKI: 4851 - Pokemon - Versione Oro HeartGold (Italy)
      //the game use uncommon backup command 0x08 for identify.
      NeedUncompressed();
      PatchMem(KArm9,0x020de220,0xe3a000aa); //mov r0, #0xaa
      break;
    //IPKS: 4832 - Pokemon - Edicion Oro HeartGold (Spain)
    case 0x534b5049:
      //the game use uncommon backup command 0x08 for identify.
      NeedUncompressed();
      PatchMem(KArm9,0x020de218,0xe3a000aa); //mov r0, #0xaa
      break;
  }
}

void CDsPatcher::PatchFlashCardCheck(void)
{
  const size_t C3JJ_index=0;
  const size_t CLJJ_index=1;
  const size_t YNOK_index=2;
  const size_t C36J_index=3;
  const size_t YKGJ_index=4;
  const size_t C52J_index=5;
  const size_t C5IJ_index=6;
  const size_t YRBK_index=7;
  const size_t YDQJ_index=8;
  const size_t B62J_index=9;
  const size_t CYJJ_index=10;
  const size_t YW4K_index=11;
  const size_t C4WK_index=12;
  const size_t YG4K_index=13;
  const size_t CSGJ_index=14;
  const size_t A3YK_index=15;
  const size_t YLZK_index=16;
  const size_t YKGE_index=17;
  const size_t YKGP_index=18;
  const size_t BMTJ_index=19;
  const size_t COLE_COLP_index=20;
  const size_t CS7J_index=21;
  const size_t COLJ_COLK_index=22;
  static SOverlayInfo info[]=
  {
    {0x0000000a,0x020107fc,KMemProtMe,{0x0211eb08,0x0211ee68}}, //000
    {0x00000004,0x0203e898,KMemProtMe,{0x020d4104,0x020d446c}}, //001
    {0x0000005a,0x02037698,KMemProtMe,{0x021b85e8,0x021b8948}}, //002
    {0x00000000,0x0205fdb8,KMemProtMe,{0x0211b748,0x0211baa8}}, //003
    {0x0000001c,0x0200b6f4,KMemProtMe,{0x0208a988,0x0208ace8}}, //004
    {0x00000001,0x0200b150,KMemProtMe,{0x020be250,0x020be5b0}}, //005
    {0x00000001,0x02050258,KMemProtMe,{0x020bf5d0,0x020bf930}}, //006
    {0x00000040,0x0200e03c,KMemProtMe,{0x021ded68,0x021df0c8}}, //007
    {0x0000001d,0x020a378c,KMemProtMe,{0x021d96f0,0x021d9a50}}, //008
    {0x00000006,0x02122020,KMemProtMe,{0x0224c110,0x0224c470}}, //009
    {0x00000002,0x0200cc98,KMemProtMe,{0x020c7308,0x020c7670}}, //010
    {0x00000000,0x0208a78c,KMemProtMe,{0x0216c6a8,0x0216ca10}}, //011
    {0x00000000,0x0200a754,KMemProtMe,{0x020aa808,0x020aab68}}, //012
    {0x00000002,0x0200701c,KMemProtMe,{0x02118d08,0x02119068}}, //013
    {0x00000002,0x0205ef80,KMemProtMe,{0x0215edd4,0x0215f134}}, //014
    {0x00000006,0x020f0f54,KMemProtMe,{0x02158f64,0x021592c4}}, //015
    {0x000000a6,0x02036c78,KMemProtMe,{0x0229d290,0x0229d5f0}}, //016
    {0x0000001c,0x0200b6d8,KMemProtMe,{0x0208abb0,0x0208af10}}, //017
    {0x0000001c,0x0200b6f8,KMemProtMe,{0x0208abd0,0x0208af30}}, //018
    {0x00000000,0x0200c7b0,KMemProtMe,{0x02177890,0x02177bf0}}, //019
    {0x00000029,0x020143e0,KMemProtMe,{0x023df7f0,0x023dfb50}}, //020
    {0x00000005,0x0200d2dc,KMemProtMe,{0x0210f670,0x0210f9d0}}, //021
    {0x00000029,0x02014400,KMemProtMe,{0x023df7f0,0x023dfb50}}  //022
  };
  //flash card check
  switch(GameCode())
  {
    //patch "2472 - Dragon Quest V - Tenkuu no Hanayome (Japan)"
    case 0x4a355659: //YV5J
      NeedUncompressed();
      PatchMem(KArm9,0x02085fd4,0xe12fff1e); //bx lr
      break;
    //patch "2540 - Maeilmaeil Deoukdeo! DS Dunoe Training (Korea)"
    case 0x4b4d4e41: //ANMK
      NeedUncompressed();
      PatchMem(KArm9,0x0210631c,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x02106320,0xe12fff1e); //bx lr
      break;
    //patch "2796 - DS Vitamin - Widaehan Bapsang - Malhaneun! Geongangyori Giljabi (Korea)"
    case 0x4b4b5659: //YVKK
      NeedUncompressed();
      PatchMem(KArm9,0x020f7458,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020f745c,0xe12fff1e); //bx lr
      break;
    //patch "2838 - Cid to Chocobo no Fushigi na Dungeon - Toki Wasure no Meikyuu DS+ (Japan)"
    case 0x4a544343: //CCTJ
      PatchMem(KArm9,0x0203d1e8,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0203d1ec,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x0203ce88,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0203ce8c,0xe12fff1e); //bx lr
      break;
    //patch "2949 - Chrono Trigger (Japan) (En,Ja)"
    case 0x4a555159: //YQUJ
      NeedUncompressed();
      PatchMem(KArm9,0x0204e364,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0204e368,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x0204e6c4,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0204e6c8,0xe12fff1e); //bx lr
      break;
    //patch "3049 - Layton Kyouju to Saigo no Jikan Ryokou (Japan)"
    case 0x4a4a3343: //C3JJ
      NeedUncompressed();
      PatchOverlay(info[C3JJ_index]);
      break;
    //patch "3055 - Chrono Trigger (USA) (En,Fr)"
    case 0x45555159: //YQUE
    //patch "3351 - Chrono Trigger (Europe) (En,Fr)"
    case 0x50555159: //YQUP
      NeedUncompressed();
      PatchMem(KArm9,0x0204e334,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0204e338,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x0204e694,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0204e698,0xe12fff1e); //bx lr
      break;
    //patch "3070 - TOEIC DS - Haru 10-Bun Yakjeomgeukbog +200 (Korea)"
    case 0x4b355443: //CT5K
      NeedUncompressed();
      PatchMem(KArm9,0x020c6d9c,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020c6da0,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x020c6a3c,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020c6a40,0xe12fff1e); //bx lr
      break;
    //patch "3149 - Pokemon Mystery Dungeon - Explorers of Time (Korea)"
    case 0x4b544659: //YFTK
      NeedUncompressed();
      PatchMem(KArm9,0x0206a2dc,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0206a2e0,0xe12fff1e); //bx lr
      break;
    //patch "3151 - Chocobo to Mahou no Ehon - Majo to Shoujo to 5-nin no Yuusha (Japan)"
    case 0x4a593543: //C5YJ
      NeedUncompressed();
      PatchMem(KArm9,0x02015a58,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x02015a5c,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x02015db8,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x02015dbc,0xe12fff1e); //bx lr
      break;
    //patch "3191 - Phantasy Star Zero (Japan)"
    case 0x4a343243: //C24J
      NeedUncompressed();
      PatchMem(KArm9,0x020c5690,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020c5694,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x020c59f0,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020c59f4,0xe12fff1e); //bx lr
      break;
    //patch "3332 - Final Fantasy Crystal Chronicles - Echoes of Time (Japan)"
    case 0x4a494643: //CFIJ
      NeedUncompressed();
      PatchMem(KArm9,0x020cad90,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020cad94,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x020cb0f8,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020cb0fc,0xe12fff1e); //bx lr
      break;
    //patch "3369 - Mario & Luigi RPG 3!!! (Japan)"
    case 0x4a4a4c43: //CLJJ
      NeedUncompressed();
      PatchMem(KArm9,0x02047c50,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x02047c54,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x02047fb8,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x02047fbc,0xe12fff1e); //bx lr
      PatchOverlay(info[CLJJ_index]);
      break;
    //patch "3380 - Hotel Dusk - Room 215 (Korea)"
    case 0x4b495741: //AWIK
      NeedUncompressed();
      PatchMem(KArm9,0x020cd45c,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020cd460,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x020cd7c4,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020cd7c8,0xe12fff1e); //bx lr
      break;
    //patch "3396 - Dragon Quest - The Hand of the Heavenly Bride (Europe) (En,Fr,De,Es,It)"
    case 0x50355659: //YV5P
    //patch "3424 - Dragon Quest V - Hand of the Heavenly Bride (USA) (En,Fr,Es)"
    case 0x45355659: //YV5E
      NeedUncompressed();
      PatchMem(KArm9,0x0200819c,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020081a0,0xe12fff1e); //bx lr
      break;
    //patch "3498 - Princess Maker 4 - Special Edition (Korea)"
    case 0x4b345043: //CP4K
      NeedUncompressed();
      PatchMem(KArm9,0x020692f4,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020692f8,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x0206965c,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x02069660,0xe12fff1e); //bx lr
      break;
    //patch "3533 - Pokemon Bulgasaui Dungeon - Eodumui Tamheomdae (Korea)"
    case 0x4b594659: //YFYK
      NeedUncompressed();
      PatchMem(KArm9,0x0206a2dc,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0206a2e0,0xe12fff1e); //bx lr
      break;
    //patch "3570 - TOEIC Test DS Training (Korea)"
    case 0x4b594541: //AEYK
      NeedUncompressed();
      PatchMem(KArm9,0x0205d0c4,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0205d0c8,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x0205d42c,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0205d430,0xe12fff1e); //bx lr
      break;
    //patch "3574 - Final Fantasy Crystal Chronicles - Echoes of Time (USA) (En,Fr,Es)"
    case 0x45494643: //CFIE
      NeedUncompressed();
      PatchMem(KArm9,0x020cae48,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020cae4c,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x020cb1b0,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020cb1b4,0xe12fff1e); //bx lr
      break;
    //patch "3580 - Final Fantasy Crystal Chronicles - Echoes of Time (Europe) (En,Fr,De,Es)"
    case 0x50494643: //CFIP
      NeedUncompressed();
      PatchMem(KArm9,0x020cae64,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020cae68,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x020cb1cc,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020cb1d0,0xe12fff1e); //bx lr
      break;
    //patch "3595 - Spectral Force - Genesis (Korea)"
    case 0x4b345759: //YW4K
      NeedUncompressed();
      PatchOverlay(info[YW4K_index]);
      break;
    //patch "3607 - Pro Yakyuu Famista DS 2009 (Japan)"
    case 0x4a393243: //C29J
      NeedUncompressed();
      PatchMem(KArm9,0x0217e58c,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0217e590,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x0217e8ec,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0217e8f0,0xe12fff1e); //bx lr
      break;
    //patch "3659 - Mabeopcheonjamun DS (Korea)"
    case 0x4b4f4e59: //YNOK
      NeedUncompressed();
      PatchOverlay2(info[YNOK_index],0);
      break;
    //patch "3733 - Princess Bakery (Korea)"
    case 0x4b573443: //C4WK
      NeedUncompressed();
      PatchOverlay(info[C4WK_index]);
      break;
    //patch "3782 - Sloan to Michael - Nazo no Monogatari (Japan)"
    case 0x4a363343: //C36J
      NeedUncompressed();
      PatchOverlay(info[C36J_index]);
      break;
    //patch "3790 - Pro Yakyuu Team o Tsukurou! 2 (Japan)"
    case 0x4a393842: //B89J
      NeedUncompressed();
      PatchMem(KArm9,0x02114994,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x02114998,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x02114cf4,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x02114cf8,0xe12fff1e); //bx lr
      break;
    //patch "3791 - Yuuzai x Muzaii (Japan)"
    case 0x4a454243: //CBEJ
      NeedUncompressed();
      PatchMem(KArm9,0x020318c4,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020318c8,0xe12fff1e); //bx lr
      PatchMem(KArm9,0x02031c24,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x02031c28,0xe12fff1e); //bx lr
      break;
    //patch "3819 - Kingdom Hearts 358-2 Days (Japan)"
    case 0x4a474b59: //YKGJ
      NeedUncompressed();
      PatchOverlay(info[YKGJ_index]);
      break;
    //patch "3872 - Tago Akira no Atama no Taisou Dai-2-Shuu - Ginga Oudan Nazotoki Adventure (Japan)"
    case 0x4a323543: //C52J
      NeedUncompressed();
      PatchOverlay(info[C52J_index]);
      break;
    //patch "3873 - Tomodachi Collection (Japan)"
    //patch "5297 - Tomodachi Collection (Japan) (Rev 1)"
    case 0x4a554343: //CCUJ
      NeedUncompressed();
      if(0==__NDSHeader->romversion)
      {
        PatchMem(KArm9,0x020af9a4,0xe3a00000); //mov r0, #0
        PatchMem(KArm9,0x020af9a8,0xe12fff1e); //bx lr
        PatchMem(KArm9,0x020afd0c,0xe3a00000); //mov r0, #0
        PatchMem(KArm9,0x020afd10,0xe12fff1e); //bx lr
      }
      else if(1==__NDSHeader->romversion)
      {
        PatchMem(KArm9,0x020afdd8,0xe3a00000); //mov r0, #0
        PatchMem(KArm9,0x020afddc,0xe12fff1e); //bx lr
        PatchMem(KArm9,0x020b0140,0xe3a00000); //mov r0, #0
        PatchMem(KArm9,0x020b0144,0xe12fff1e); //bx lr
      }
      break;
    //patch "3874 - Tago Akira no Atama no Taisou Dai-1-Shuu - Nazotoki Sekai Isshuu Ryokou (Japan)"
    case 0x4a493543: //C5IJ
      NeedUncompressed();
      PatchOverlay(info[C5IJ_index]);
      break;
    //patch "3909 - Ragnarok Online DS (Korea)"
    case 0x4b425259: //YRBK
      NeedUncompressed();
      PatchOverlay(info[YRBK_index]);
      break;
    //patch "3966 - Dragon Quest IX - Hoshizora no Mamoribito (Japan)"
    case 0x4a514459: //YDQJ
      NeedUncompressed();
      PatchOverlay3(info[YDQJ_index]);
      break;
    //patch "3982 - Hwansangsuhojeon - Tierkreis (Korea)"
    case 0x4b344759: //YG4K
      NeedUncompressed();
      PatchOverlay(info[YG4K_index]);
      break;
    //patch "4013 - Okaeri! Chibi-Robo! Happy Rich Oosouji (Japan)"
    case 0x4a323642: //B62J
      NeedUncompressed();
      PatchOverlay4(info[B62J_index]);
      break;
    //patch "4036 - Puyo Puyo 7 (Japan)"
    case 0x4a4f5942: //BYOJ
      NeedUncompressed();
      PatchOverlay5(0x0200b1dc);
      break;
    //patch "4085 - Blood of Bahamut (Japan)"
    case 0x4a4a5943: //CYJJ
      NeedUncompressed();
      PatchOverlay(info[CYJJ_index]);
      break;
    //patch "4090 - Tsukibito (Japan)"
    case 0x4a563243: //C2VJ
      NeedUncompressed();
      PatchMem(KArm9,0x02069ed8,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0206a25c,0xe3a00000); //mov r0, #0
      break;
    //patch "4133 - Nanashi no Game Me (Japan)"
    case 0x4a343742: //B74J
      NeedUncompressed();
      PatchOverlay6(0x0200bf0c,KMemProtMe);
      break;
    //patch "4134 - Super Robo Gakuen (Japan)"
    case 0x4a593243: //C2YJ
      NeedUncompressed();
      PatchMem(KArm9,0x0202abc8,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0202af4c,0xe3a00001); //mov r0, #1
      break;
    //patch "4148 - Nihon Keizai Shimbunsha Kanshu Shiranai Mama dewa Son wo Suru - Mono ya Okane no Shikumi DS (Japan)"
    case 0x4a544542: //BETJ
      NeedUncompressed();
      PatchOverlay6(0x0207ceac,KMemProtMe);
      break;
    //patch "4151 - Sloane to MacHale no Nazo no Story 2 (Japan)"
    case 0x4a513242: //B2QJ
      NeedUncompressed();
      PatchOverlay6(0x0208fb7c,KMemProtMe);
      break;
    //patch "4168 - Pocket Monster - Heart Gold (Japan)"
    case 0x4a4b5049: //IPKJ
    //patch "4169 - Pocket Monster - Soul Silver (Japan)"
    case 0x4a475049: //IPGJ
      NeedUncompressed();
      PatchOverlay6(0x020d8624,KMemProtMe);
      break;
    //patch "4171 - Mario & Luigi - Bowser's Inside Story (USA)"
    case 0x454a4c43: //CLJE
    //patch "4268 - Mario & Luigi - Bowser's Inside Story (Europe) (En,Fr,De,Es,It)"
    case 0x504a4c43: //CLJP
      NeedUncompressed();
      PatchOverlay6(0x0203e68c,KMemProtMe);
      break;
    //patch "4183 - SaGa 2 - Hihou Densetsu - Goddess of Destiny (Japan)"
    case 0x4a475343: //CSGJ
      //not compressed
      PatchOverlay2(info[CSGJ_index],1);
      FixProtectionRegion2();
      break;
    //patch "4188 - Anpanman to Touch de Waku Waku Training (Japan)"
    case 0x4a503341: //A3PJ
      NeedUncompressed();
      PatchMem(KArm9,0x02029684,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x02029688,0xe12fff1e); //bx lr
      break;
    //patch "4189 - Kindaichi Shounen no Jikenbo - Akuma no Satsujin Koukai (Japan)"
    case 0x4a444b43: //CKDJ
      NeedUncompressed();
      PatchMem(KArm9,0x0206cab0,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0206cbb8,0xe3a00001); //mov r0, #1
      break;
    //patch "4191 - Sonic Rush Adventure (Korea)"
    case 0x4b593341: //A3YK
      //not compressed
      PatchOverlay(info[A3YK_index]);
      FixProtectionRegion2();
      break;
    //patch "4194 - Jjangguneun Mosmallyeo - Cinemaland Chalkak Chalkak Daesodong! (Korea)"
    case 0x4b435259: //YRCK
      NeedUncompressed();
      PatchOverlay6(0x02074678,KMemProtMe);
      break;
    //patch "4205 - Ookami to Koushinryou - Umi wo Wataru Kaze (Japan)"
    case 0x4a4b4f42: //BOKJ
      NeedUncompressed();
      PatchOverlay6(0x0200aa64,KMemProtMe);
      break;
    //patch "4215 - Rhythm World (Korea)"
    case 0x4b5a4c59: //YLZK
      NeedUncompressed();
      PatchOverlay(info[YLZK_index]);
      break;
    //patch "4223 - Kingdom Hearts - 358-2 Days (USA) (En,Fr)"
    case 0x45474b59: //YKGE
      NeedUncompressed();
      PatchOverlay(info[YKGE_index]);
      break;
    //patch "4246 - Inazuma Eleven 2 - Kyoui no Shinryokusha - Fire (Japan)"
    case 0x4a454542: //BEEJ
    //patch "4247 - Inazuma Eleven 2 - Kyoui no Shinryokusha - Blizzard (Japan)"
    case 0x4a424542: //BEBJ
      NeedUncompressed();
      PatchOverlay6(0x0200d678,KMemProtMe);
      break;
    //patch "4257 - Blue Dragon - Ikai no Kyoujuu (Japan)"
    case 0x4a554259: //YBUJ
    //patch "4956 - Blue Dragon - Awakened Shadow (USA)"
    case 0x45554259: //YBUE
      NeedUncompressed();
      PatchOverlay6(0x0200d8e4,KMemProtMe);
      break;
    //patch "4262 - Kingdom Hearts - 358-2 Days (Europe) (En,Fr,De,Es,It)"
    case 0x50474b59: //YKGP
      NeedUncompressed();
      PatchOverlay(info[YKGP_index]);
      break;
    //patch "4264 - Shin Megami Tensei - Strange Journey (Japan)"
    case 0x4a544d42: //BMTJ
      NeedUncompressed();
      PatchOverlay(info[BMTJ_index]);
      break;
    //patch "4270 - Tago Akira no Atama no Taisou Dai-3-Shuu - Fushigi no Kuni no Nazotoki Otogibanashi (Japan)"
    case 0x4a335142: //BQ3J
      NeedUncompressed();
      PatchOverlay6(0x0200b0f0,KMemProtMe);
      break;
    //patch "4271 - Tago Akira no Atama no Taisou Dai-4-Shuu - Time Machine no Nazotoki Daibouken (Japan)"
    case 0x4a345142: //BQ4J
      NeedUncompressed();
      PatchOverlay6(0x0200e9d0,KMemProtMe);
      break;
    //patch "4272 - Iron Master - The Legendary Blacksmith (Japan)"
    case 0x4a4d4942: //BIMJ
    //patch "4723 - Iron Master - Wanggukui Yusangwa Se Gaeui Yeolsoe (Korea)"
    case 0x4b4f4443: //CDOK
      NeedUncompressed();
      PatchOverlay6(0x0200b890,KMemProtMe);
      break;
    //patch "4274 - Mario & Sonic at the Olympic Winter Games (USA) (En,Fr,Es)"
    case 0x454c4f43: //COLE
    //patch "4292 - Mario & Sonic at the Olympic Winter Games (Europe) (En,Fr,De,Es,It)"
    case 0x504c4f43: //COLP
      NeedUncompressed();
      PatchOverlay(info[COLE_COLP_index]);
      break;
    //patch "4301 - Kimi ni Todoke - Sodateru Omoi (Japan)"
    case 0x4a544b42: //BKTJ
      NeedUncompressed();
      PatchMem(KArm9,0x02061858,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x02061bdc,0xe3a00001); //mov r0, #1
      break;
    //patch "4328 - Element Hunter (Japan)"
    case 0x4a4c4542: //BELJ
      NeedUncompressed();
      PatchOverlay6(0x0214dfcc,KMemProtMe);
      break;
    //patch "4336 - Style Boutique (Europe) (En,Fr,De,Es,It) (Rev 1)"
    case 0x504c5a41: //AZLP
      NeedUncompressed();
      PatchOverlay7(0x020ad378);
      break;
    //patch "4353 - Hikari no 4 Senshi - Final Fantasy Gaiden (Japan)"
    case 0x4a584642: //BFXJ
    //patch "5258 - Final Fantasy - The 4 Heroes of Light (USA)"
    case 0x45584642: //BFXE
    //patch "5259 - Final Fantasy - The 4 Heroes of Light (Europe)"
    case 0x50584642: //BFXP
      NeedUncompressed();
      PatchOverlay6(0x0200b3a8,KMemProtMe);
      break;
    //patch "4354 - Umihara Kawase Shun - Second Edition Kanzenban (Japan)"
    case 0x4a485543: //CUHJ
      NeedUncompressed();
      PatchOverlay6(0x02008f60,KMemProtMe);
      break;
    //patch "4355 - Fresh PreCure! - Asobi Collection (Japan)"
    case 0x4a504642: //BFPJ
      NeedUncompressed();
      PatchOverlay7(0x02032ca4);
      break;
    //patch "4370 - Summon Night X - Tears Crown (Japan)"
    case 0x4a375343: //CS7J
      NeedUncompressed();
      PatchOverlay(info[CS7J_index]);
      break;
    //patch "4429 - Phantasy Star 0 (USA)"
    case 0x45343243: //C24E
    //patch "4708 - Phantasy Star 0 (Europe)"
    case 0x50343243: //C24P
      NeedUncompressed();
      PatchMem(KArm9,0x020c5a08,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x020c5b10,0xe3a00001); //mov r0, #1
      break;
    //patch "4432 - Rockman EXE - Operate Shooting Star (Japan)"
    case 0x4a583642: //B6XJ
      NeedUncompressed();
      PatchOverlay6(0x0200d344,KMemProtMe);
      break;
    //patch "4434 - Tamagotchi no Narikiri Channel (Japan)"
    case 0x4a564742: //BGVJ
      //not compressed
      PatchOverlay6(0x02059bd4,KMemProtMe);
      FixProtectionRegion2();
      break;
    //patch "4467 - Mario & Sonic at Vancouver Olympics (Japan)"
    case 0x4a4c4f43: //COLJ
    //patch "4535 - Mario and Sonic at the Olympic Winter Games (Korea)"
    case 0x4b4c4f43: //COLK
      NeedUncompressed();
      PatchOverlay(info[COLJ_COLK_index]);
      break;
    //patch "4469 - Wizardry - Inochi no Kusabi (Japan)"
    case 0x4a5a5742: //BWZJ
      NeedUncompressed();
      PatchOverlay6(0x0200bd60,KMemProtMe);
      break;
    //patch "4499 - Cooking Mama 3 (Japan)"
    case 0x4a433342: //B3CJ
      NeedUncompressed();
      PatchOverlay6(0x02062cec,KMemProtMe);
      break;
    //patch "4500 - Ochaken no Heya DS 4 - Ochaken Land de Hotto Shiyo (Japan)"
    case 0x4a344f42: //BO4J
      NeedUncompressed();
      PatchOverlay6(0x0203b4e0,KMemProtMe);
      break;
    //patch "4513 - Cross Treasures (Japan)"
    case 0x4a4e4442: //BDNJ
      //not compressed
      PatchOverlay6(0x0200bcb0,KMemProtMe);
      FixProtectionRegion2();
      break;
    //patch "4550 - Kuru Kuru Princess Tokimeki Figure Mezase Vancouver (Japan)"
    case 0x4a505142: //BQPJ
      NeedUncompressed();
      PatchOverlay6(0x02115ec0,KMemProtMe);
      break;
    //patch "4561 - Kyokugen Dasshutsu 9 Jikan 9 Nin 9 no Tobira (Japan)"
    case 0x4a4b5342: //BSKJ
      NeedUncompressed();
      PatchOverlay6(0x02086afc,KMemProtMe);
      break;
    //patch "4591 - Style Savvy (USA)"
    case 0x454c5a41: //AZLE
      NeedUncompressed();
      PatchOverlay7(0x020ad268);
      break;
    //patch "4593 - Naruto Shippuuden - Dairansen! Kagebunshin Emaki (Korea)"
    case 0x4b524e59: //YNRK
      NeedUncompressed();
      PatchOverlay6(0x020095cc,KMemProtMe);
      break;
    //patch "4654 - Last Window - Mayonaka no Yakusoku (Japan)"
    case 0x4a554c59: //YLUJ
      NeedUncompressed();
      PatchMem(KArm9,0x0207add8,0xe3a00000); //mov r0, #0
      PatchMem(KArm9,0x0207aee0,0xe3a00001); //mov r0, #1
      break;
    //patch "4658 - Element Hunters (Korea)"
    case 0x4b4c4542: //BELK
      NeedUncompressed();
      PatchOverlay6(0x0214d6f4,KMemProtMe);
      break;
    //patch "4659 - Jeoldaepiryo Yeongsugeo 1000 DS (Korea)"
    case 0x4b4f4559: //YEOK
      NeedUncompressed();
      PatchMem(KArm9,0x0208ae58,0xe3a00000); //mov r0, #0
      break;
    //patch "4660 - Jeoldaeuwi Yeongdaneo 1900 DS (Korea)"
    case 0x4b394559: //YE9K
      NeedUncompressed();
      PatchMem(KArm9,0x0208d684,0xe3a00000); //mov r0, #0
      break;
    //patch "4680 - Dragon Quest VI - Maboroshi no Daichi (Japan)"
    case 0x4a495659: //YVIJ
      NeedUncompressed();
      PatchOverlay7(0x0200ae68);
      break;
    //patch "4760 - Otona no Renai Shousetsu - DS Harlequin Selection (Japan)"
    case 0x4a514842: //BHQJ
      for(u32* ii=(u32*)unprot_006_bin;ii<(u32*)(unprot_006_bin+unprot_006_bin_size);++ii)
      {
        if(0xe28dd02c==*ii)
        {
          *ii=0xe28dd074;
        }
      }
      NeedUncompressed();
      PatchOverlay6(0x02004654,KMemProtMe);
      break;
    //patch "4761 - Imasugu Tsukaeru Mame Chishiki - Quiz Zatsugaku-ou DS (Japan)"
    case 0x4a325a42: //BZ2J
      NeedUncompressed();
      PatchOverlay6(0x0200bdec,KMemProtMe);
      break;
    //patch "4900 - America's Test Kitchen - Let's Get Cooking (USA)"
    case 0x45434b42: //BKCE
      NeedUncompressed();
      PatchOverlay6(0x0209f5f4,KMemProtMe);
      break;
    //patch "5154 - Kanshuu - Shuukan Pro Wrestling - Pro Wrestling Kentei DS (Japan)"
    case 0x4a504b42: //BKPJ
      NeedUncompressed();
      PatchOverlay6(0x0204df38,KMemProtMe);
      break;
    default:
      PatchFlashCardCheck2();
      break;
  }
}

void CPatcher::PatchOverlay(SOverlayInfo& anInfo)
{
  memcpy((void*)anInfo.iPatchAddress,unprot_001_bin,unprot_001_bin_size);
  u32* data=(u32*)(anInfo.iPatchAddress+unprot_001_bin_size);
  data[0]=anInfo.iFixAddresses[0];
  data[1]=anInfo.iFixAddresses[1];
  data[2]=anInfo.iOverlayId;
  PatchMem(KArm9,anInfo.iLoadAddress,0xe51ff004); //ldr pc, [pc, #-4]
  PatchMem(KArm9,anInfo.iLoadAddress+4,(u32)anInfo.iPatchAddress);
}

void CDsPatcher::PatchOverlay2(SOverlayInfo& anInfo,u32 aStyle)
{
  memcpy((void*)anInfo.iPatchAddress,unprot_002_bin,unprot_002_bin_size);
  u32* data=(u32*)(anInfo.iPatchAddress+unprot_002_bin_size);
  data[0]=anInfo.iFixAddresses[0];
  data[1]=anInfo.iFixAddresses[1];
  data[2]=anInfo.iOverlayId;
  switch(aStyle)
  {
    case 1:
      PatchMem(KArm9,anInfo.iLoadAddress-4,0x46c02000); //movs r0, #0|nop
    case 0:
      PatchMem(KArm9,anInfo.iLoadAddress,GenerateThumbBL(anInfo.iLoadAddress,anInfo.iPatchAddress));
      break;
  }
}

void CDsPatcher::PatchOverlay3(SOverlayInfo& anInfo)
{
  memcpy((void*)anInfo.iPatchAddress,unprot_003_bin,unprot_003_bin_size);
  u32* data=(u32*)(anInfo.iPatchAddress+unprot_003_bin_size);
  data[0]=anInfo.iFixAddresses[0];
  data[1]=anInfo.iFixAddresses[1];
  data[2]=anInfo.iOverlayId;
  PatchMem(KArm9,anInfo.iLoadAddress,0xe51ff004); //ldr pc, [pc, #-4]
  PatchMem(KArm9,anInfo.iLoadAddress+4,(u32)anInfo.iPatchAddress);
}

void CDsPatcher::PatchOverlay4(SOverlayInfo& anInfo)
{
  memcpy((void*)anInfo.iPatchAddress,unprot_004_bin,unprot_004_bin_size);
  u32* data=(u32*)(anInfo.iPatchAddress+unprot_004_bin_size);
  data[0]=anInfo.iFixAddresses[0];
  data[1]=anInfo.iFixAddresses[1];
  data[2]=anInfo.iOverlayId;
  PatchMem(KArm9,anInfo.iLoadAddress,GenerateThumbBL(anInfo.iLoadAddress,anInfo.iPatchAddress));
}

void CDsPatcher::PatchOverlay5(u32 aLoadAddress)
{
  memcpy((void*)KMemProtMe,unprot_005_bin,unprot_005_bin_size);
  PatchMem(KArm9,aLoadAddress,GenerateArmBL(aLoadAddress,KMemProtMe,true));
}

void CPatcher::PatchOverlay6(u32 aLoadAddress,u32 aPatchAddress)
{
  memcpy((void*)aPatchAddress,unprot_006_bin,unprot_006_bin_size);
  PatchMem(KArm9,aLoadAddress,GenerateArmBL(aLoadAddress,aPatchAddress,true));
}

void CPatcher::PatchOverlay7(u32 aLoadAddress)
{
  memcpy((void*)KMemProtMe,unprot_007_bin,unprot_007_bin_size);
  PatchMem(KArm9,aLoadAddress,GenerateThumbBL(aLoadAddress,KMemProtMe));
}

void CPatcher::PatchOverlay8(u32 aLoadAddress,u32 aPatchAddress,u32 aFirst,u32 aSecond)
{
  for(u32* ii=(u32*)unprot_008_bin;ii<(u32*)(unprot_008_bin+unprot_008_bin_size);++ii)
  {
    if(0xefc0de01==*ii)
    {
      *ii=aFirst;
    }
    else if(0xefc0de02==*ii)
    {
      *ii=aSecond;
    }
  }
  memcpy((void*)aPatchAddress,unprot_008_bin,unprot_008_bin_size);
  PatchMem(KArm9,aLoadAddress,GenerateArmBL(aLoadAddress,aPatchAddress,true));
}

u32* CPatcher::PatchRepatch9(void)
{
  u32* func_5_addr=(u32*)iPatchDMA4Ptr;
  memcpy((void*)iPatchDMA4Ptr,repatch9_bin,repatch9_bin_size);
  for(u32* ii=(u32*)iPatchDMA4Ptr;ii<(u32*)(iPatchDMA4Ptr+repatch9_bin_size);ii++)
  {
    if(0xEFC0DE10==*ii)
    {
      if(IsDMA()) *ii=0xe12fff31; //blx r1
      else *ii=0xe1a00000; //nop
    }
    else if(0xEFC0DE11==*ii)
    {
      *ii=iPatchDMA4Entry+1;
    }
    else if(0xEFC0DEC1==*ii)
    {
      if(*ii==*(ii+1))
      {
        *ii=FixedAddr();
        *(ii+1)=FixedData();
      }
    }
  }
  iPatchDMA4Ptr+=repatch9_bin_size;
  return func_5_addr;
}

#include "patches_pop.h"
#include "patches_idr.h"
u32* CPatcher::PushRepatch9(u32* aTarget,bool aType)
{
  if(aType) *aTarget++=CountRepatch9();
  u32 count=iPatchSize[KArm9];
  memcpy(aTarget,iPatchData9,count*sizeof(TPatchInfo));
  aTarget+=count*2;
  u32 more_count=0; const u32* more_ptr=NULL; u32 more_fill=0xe1500000;
  switch(GameCode())
  {
    case 0x56505056: //4951 - Prince of Persia - The Forgotten Sands (Europe) (En,Fr,De,Es,It,Nl) (NDSi Enhanced)
    case 0x45505056: //5017 - Prince of Persia - The Forgotten Sands (USA) (En,Fr,De,Es,It,Nl) (NDSi Enhanced)
      more_ptr=pop_info;
      more_count=ArraySize(pop_info);
      break;
    case 0x56444956: //5225 - Imagine - Dream Resort (Europe) (NDSi Enhanced)
      more_ptr=idr_info;
      more_count=ArraySize(idr_info);
      break;
  }
  if(more_count&&more_ptr)
  {
    for(u32 ii=0;ii<more_count;++ii)
    {
      aTarget[ii*2]=more_ptr[ii];
      aTarget[ii*2+1]=more_fill;
      *(u32*)(more_ptr[ii])=more_fill;
    }
    aTarget+=more_count*2;
  }
  return aTarget;
}

u32 CPatcher::CountRepatch9(void)
{
  u32 add=0;
  switch(GameCode())
  {
    case 0x56505056: //4951 - Prince of Persia - The Forgotten Sands (Europe) (En,Fr,De,Es,It,Nl) (NDSi Enhanced)
    case 0x45505056: //5017 - Prince of Persia - The Forgotten Sands (USA) (En,Fr,De,Es,It,Nl) (NDSi Enhanced)
      add=ArraySize(pop_info);
      break;
  }
  return iPatchSize[KArm9]+add;
}

void CPatcher::PatchRepatch(void)
{
  if(!IsDMA()&&FixedAddr()) //game uncompressed but dma not used.
  {
    u32 gameMain=0;
    u32* gameMainAddr=0;
    for(u32* ii=(u32*)__NDSHeader->arm9executeAddress;ii<(u32*)(__NDSHeader->arm9destination+UncompressedSize());ii++)
    {
      if(*ii==0xffff0000)
      {
        gameMainAddr=ii-1;
        gameMain=*(ii-1);
        PatchMem(KArm9,s32(ii-1),iPatchDMA4Ptr);
        break;
      }
    }
    memcpy((void*)iPatchDMA4Ptr,unpatch9_bin,unpatch9_bin_size);
    for(u32* ii=(u32*)iPatchDMA4Ptr;ii<(u32*)(iPatchDMA4Ptr+unpatch9_bin_size);ii++)
    {
      if(0xEFC0DE01==*ii)
      {
        *ii=FixedAddr();
      }
      else if(0xEFC0DE02==*ii)
      {
        *ii=FixedData();
      }
      else if(0xEFC0DE03==*ii)
      {
        *ii=(u32)gameMainAddr;
      }
      else if(0xEFC0DE04==*ii)
      {
        *ii=gameMain;
      }
    }
    iPatchDMA4Ptr+=unpatch9_bin_size;

    u32* func_5_addr=PatchRepatch9();
    for(u32* ii=(u32*)__NDSHeader->arm9destination;ii<(u32*)(__NDSHeader->arm9destination+UncompressedSize());ii++)
    {
      //12 0F 06 EE 1E FF 2F E1
      if(*ii==0xee060f12) //mcr p15, 0, r0, c6, c2
      {
        if(*(ii+1)==0xe12fff1e) //bx lr
        {
          PatchMPU2(s32(ii));
        }
      }
      //FF 07 9B E8 00 B0 A0 E3 1C FF 2F E1
      else if(*ii==0xe89b07ff) //ldmia r11, {r0-r10}
      {
        if(*(ii+1)==0xe3a0b000&&*(ii+2)==0xe12fff1c) //mov r11, #0|bx r12
        {
          PatchMem(KArm9,s32(ii+1),0xe51ff004); //ldr pc, [pc, #-4]
          PatchMem(KArm9,s32(ii+2),(u32)func_5_addr);
          iUsePatchData=true;
        }
      }
    }
  }
  if(iUsePatchData)
  {
    PushRepatch9((u32*)iPatchDMA4Ptr,true);
  }
  // always patch arm7 reset if we need pokemon linkage
  if(IsWiiLinkage()||(iUsePatchData?iPatchSize[KArm7]:(iPatchSize[KArm7]||iPatchSize[KArm9])))
  {
    PatchArm7Check();
    for(u32* ii=(u32*)__NDSHeader->arm7destination;ii<(u32*)(__NDSHeader->arm7destination+__NDSHeader->arm7binarySize);ii++)
    {
      if(*ii==0xe1a0e00c) // mov lr, r12
      {
        //new reset
        //0C E0 A0 E1 00 00 A0 E3 00 10 A0 E3 00 20 A0 E3 00 30 A0 E3 1C FF 2F E1
        if(*(ii+1)==0xe3a00000&&*(ii+2)==0xe3a01000&&*(ii+3)==0xe3a02000&&*(ii+4)==0xe3a03000&&*(ii+5)==0xe12fff1c) //mov r0, #0| mov r1, #0| mov r2, #0|mov r3, #0|bx r12
        {
          PatchRepatch7(ii+4);
          break;
        }
        //old reset
        //0C E0 A0 E1 01 B3 A0 E3 FF 07 1B E9 00 B0 A0 E3 1C FF 2F E1
        else if(*(ii+1)==0xe3a0b301&&*(ii+2)==0xe91b07ff&&*(ii+3)==0xe3a0b000&&*(ii+4)==0xe12fff1c) //mov r11, #0x4000000|ldmdb r11, {r0-r10}|mov r11, #0|bx r12
        {
          PatchRepatch7(ii+3);
          break;
        }
      }
    }
  }
}

void CPatcher::PatchRepatch7(u32* aPos)
{
  PatchMem(KArm7,s32(aPos),0xe51ff004); //ldr pc, [pc, #-4]
  PatchMem(KArm7,s32(aPos+1),iPatchArm7Entry);
  const u8* repatch_cur;
  u32 repatch_cur_size;
  if(IsPokemonGame()) // if arm7 patched always use pokemon repatcher for pokemon games
  {
    repatch_cur=repatch_pokemon_bin;
    repatch_cur_size=repatch_pokemon_bin_size;
  }
  else
  {
    repatch_cur=repatch_bin;
    repatch_cur_size=repatch_bin_size;
  }
  memcpy((void*)iPatchArm7Entry,repatch_cur,repatch_cur_size);
  for(u32* jj=(u32*)iPatchArm7Entry;jj<(u32*)(iPatchArm7Entry+repatch_cur_size);jj++)
  {
    if(0xefc0def2==*jj)
    {
      *jj=(u32)iDmaFuncs.iFunc2;
    }
    else if(0xefc0def3==*jj)
    {
      *jj=(u32)iDmaFuncs.iFunc3;
    }
    else if(0xefc0de01==*jj)
    {
      if(iSDSaveStart) *jj=0x037f8000;
      else *jj=0;
    }
    else if(0xefc0de02==*jj)
    {
      *jj=iSDSaveStart;
    }
    else if(0xefc0de03==*jj)
    {
      *jj=iSDSaveEnd;
    }
    else if(0xefc0de04==*jj)
    {
      *jj=iSDSaveEnd-iSDSaveStart;
    }
  }
  iPatchArm7Entry+=repatch_cur_size;
  u32* target=(u32*)iPatchArm7Entry;
  *target++=iUsePatchData?iPatchSize[KArm7]:(iPatchSize[KArm7]+CountRepatch9());
  if(!iUsePatchData)
  {
    target=PushRepatch9(target,false);
  }
  memcpy(target,iPatchData7,iPatchSize[KArm7]*sizeof(TPatchInfo));
}

void CPatcher::PatchArm7Check(void)
{
  switch(GameCode())
  {
    case 0x50515143: //"3903 - AFL Mascot Manor (Europe)"
      PatchMem(KArm7,0x02020f80,0xe3a00000); //mov r0,#0
#if defined(_STORAGE_r4) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
      PatchMem(KArm7,0x0206eee8,0xe3a00001); //mov r0, #1
      PatchMem(KArm7,0x0206eeec,0xe12fff1e); //bx lr
#endif
      break;
    case 0x504b5143: //"3906 - NRL Mascot Mania (Europe)"
      PatchMem(KArm7,0x0201d600,0xe3a00000); //mov r0,#0
#if defined(_STORAGE_r4) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
      PatchMem(KArm7,0x0206ff70,0xe3a00001); //mov r0, #1
      PatchMem(KArm7,0x0206ff74,0xe12fff1e); //bx lr
#endif
      break;
  }
}

void CDsPatcher::PatchiDS(void)
{
  switch(GameCode())
  {
    case 0x434e5341: //"x005 - Zhigan Yi Bi (China) (iDS)"
      PatchMem(KArm7,0x02380360,0x027ffe1f);
      break;
    case 0x43575a41: //"x019 - Momo Waliou Zhizao (China) (iDS)"
      PatchMem(KArm7,0x02380358,0x027ffe1f);
      break;
    case 0x43363341: //"x020 - Rentian GouGou (China) (Demo) (Kiosk) (iDS)"
      PatchMem(KArm7,0x02380394,0x027ffe1f);
      break;
    case 0x43495941: //"x022 - Momo Yaoxi - Yun Zhong Manbu (China) (iDS)"
      NeedUncompressed();
      PatchMem(KArm9,0x0201ff38,0x027ffe1f);
      break;
    case 0x434d5341: //"x075 - Shenyou Maliou DS (China) (iDS)"
      NeedUncompressed();
      PatchMem(KArm9,0x0201aebc,0x027ffe1f);
      break;
    case 0x43443241: //"x142 - New Chaoji Maliou Xiongdi (China) (iDS)"
      PatchMem(KArm7,0x02380394,0x027ffe1f);
      break;
  }
}

void CDsPatcher::FixBugs(void)
{
  //"3908 - Catan (Europe) (En,De)"
  //"5204 - Catan (Netherlands)"
  if((0x50374e43==GameCode()||0x48374e43==GameCode())&&0==__NDSHeader->romversion)
  {
    //bug in ai? vector[vector.size()-1] cause crash on empty vector.
    const u32 patch[]=
    {
      0xe3540000, // cmp r4, #0
      0x12441001, // subne r1, r4, #1
      0x112fff1e, // bxne lr
      0xe28dd070, // add sp, sp, #0x70
      0xe3a00000, // mov r0, #0
      0xe8bd8ff8  // ldmfd sp!, {r3-r11, pc}
    };
    memcpy((void*)0x2000bc0,patch,sizeof(patch));
    if(0x50374e43==GameCode())
    {
      PatchMem(KArm9,0x0207a9f0,0xebfe1872);
    }
    else
    {
      PatchMem(KArm9,0x0207af40,0xebfe171e);
    }
  }
  //"0735 - Castlevania - Portrait of Ruin (USA)"
  else if(0x45424341==GameCode())
  {
    PatchMem(KArm9,0x02007910,0xeb02508e);
    PatchMem(KArm9,0x02007918,0xea000004);
    PatchMem(KArm9,0x02007a00,0xeb025052);
    PatchMem(KArm9,0x02007a08,0xe59f1030);
    PatchMem(KArm9,0x02007a0c,0xe59f0028);
    PatchMem(KArm9,0x02007a10,0xe0281097);
    PatchMem(KArm9,0x02007a14,0xea000003);
  }
  //"0676 - Akumajou Dracula - Gallery of Labyrinth (Japan)"
  else if(0x4a424341==GameCode())
  {
    PatchMem(KArm9,0x02007910,0xeb0250b0);
    PatchMem(KArm9,0x02007918,0xea000004);
    PatchMem(KArm9,0x02007a00,0xeb025074);
    PatchMem(KArm9,0x02007a08,0xe59f1030);
    PatchMem(KArm9,0x02007a0c,0xe59f0028);
    PatchMem(KArm9,0x02007a10,0xe0281097);
    PatchMem(KArm9,0x02007a14,0xea000003);
  }
  //"0881 - Castlevania - Portrait of Ruin (Europe) (En,Fr,De,Es,It)"
  else if(0x50424341==GameCode())
  {
    PatchMem(KArm9,0x02007b00,0xeb025370);
    PatchMem(KArm9,0x02007b08,0xea000004);
    PatchMem(KArm9,0x02007bf0,0xeb025334);
    PatchMem(KArm9,0x02007bf8,0xe59f1030);
    PatchMem(KArm9,0x02007bfc,0xe59f0028);
    PatchMem(KArm9,0x02007c00,0xe0281097);
    PatchMem(KArm9,0x02007c04,0xea000003);
  }
  else if(0x50474942==GameCode()||0x45474942==GameCode()) //4800 - Combat of Giants - Mutant Insects (Europe) (En,Fr,De,Es,It,Nl,Sv,No,Da) || 5455 - Battle of Giants - Mutant Insects (USA) (En,Fr,Es)
  {
    if(IsCommon()||IsDMA()||IsSDSave()||Language()) PatchCoG3(0x50474942==GameCode());
  }
  else if(0x45575641==GameCode()) //1958 - Miami Nights - Singles in the City (USA) (En,Fr,Es)
  {
    //fixes not enough memory error
    PatchMem(KArm9,0x0204cccc,0xe1a00000); //nop
  }
  else if(0x50575641==GameCode()) //2010 - Miami Nights - Singles in the City (Europe) (En,Fr,De,Es,It,Nl)
  {
    //fixes not enough memory error
    PatchMem(KArm9,0x0204cdbc,0xe1a00000); //nop
  }
  else if(0x454e4843==GameCode()||0x504e4843==GameCode()) //"Might & Magic - Clash of Heroes (USA)" || "Might & Magic - Clash of Heroes (Europe) (En,Fr,De,Es,It)"
  {
    if(IsDownloadPlay()||IsDMA()||Language()) PatchMandM(0x504e4843==GameCode());
  }
  else if(0x45584759==GameCode()||0x50584759==GameCode()) //"Grand Theft Auto - Chinatown Wars (USA) (En,Fr,De,Es,It)" || "Grand Theft Auto - Chinatown Wars (Europe) (En,Fr,De,Es,It)"
  {
    if(IsSoftReset()||IsDMA()||Language())
    {
      PatchMem(KArm9,-0x02037a34,0x46c0);
      PatchMem(KArm9,0x0216ac0c,0x0001fffb);
    }
  }
  else if(0x50553743==GameCode()||0x45553743==GameCode()) //"Combat of Giants - Dragons (Europe) (En,Fr,De,Es,It)" || "Battle of Giants - Dragons (USA) (En,Fr,Es)"
  {
    if(IsSoftReset()||IsDMA()||Language()) PatchCoG2(0x50553743==GameCode());
  }
  else if(0x50554442==GameCode()||0x45554442==GameCode()) // "C.O.P. - The Recruit (Europe) (En,Fr,De,Es,It)" & "C.O.P. - The Recruit (USA) (En,Fr,De,Es,It)"
  {
    if(IsSoftReset()||IsDMA()||Language()) PatchCOP();
  }
  else if(0x00414441==GameCodeAll()||0x00415041==GameCodeAll()||0x00555043==GameCodeAll()||0x004b5049==GameCodeAll()||0x00475049==GameCodeAll()) //Pokemon - Diamond Version & Pokemon - Pearl Version & Pokemon - Platinum Version & Pokemon - HeartGold Version & Pokemon - SoulSilver Version
  {
    if(IsDMA())
    {
      s32 address=0;
      NeedUncompressed();
      switch(GameCode())
      {
        case 0x4a414441: //Diamond (Japan)
        case 0x4a415041: //Pearl (Japan) & Pearl (Japan) (Rev 5) & Pearl (Japan) (Rev 6)
          if(6==__NDSHeader->romversion) address=0x020168a8;
          else address=0x02016884;
          break;
        case 0x45414441: //Diamond (USA) (Rev 5) & Diamond (Europe) (Rev 13)
        case 0x45415041: //Pearl (USA) (Rev 5) & Pearl (Europe) (Rev 13)
          address=0x020164f0;
          break;
        case 0x44414441: //Diamond (Germany) (Rev 5)
        case 0x53414441: //Diamond (Spain) (Rev 5)
        case 0x46414441: //Diamond (France) (Rev 5)
        case 0x49414441: //Diamond (Italy) (Rev 5)
        case 0x44415041: //Pearl (Germany) (Rev 5)
        case 0x53415041: //Pearl (Spain) (Rev 5)
        case 0x46415041: //Pearl (France) (Rev 5)
        case 0x49415041: //Pearl (Italy) (Rev 5)
          address=0x0201653c;
          break;
        case 0x4b414441: //Diamond (Korea)
        case 0x4b415041: //Pearl (Korea)
          address=0x02016614;
          break;
        case 0x4a555043: //Platinum (Japan)
          address=0x020178d0;
          break;
        case 0x45555043: //Platinum (USA) & Platinum (USA) (Rev 1) & Platinum (Europe) (Rev 10)
          address=0x02017c28;
          break;
        case 0x44555043: //Platinum (Germany)
        case 0x46555043: //Platinum (France)
        case 0x53555043: //Platinum (Spain)
        case 0x49555043: //Platinum (Italy)
          address=0x02017c98;
          break;
        case 0x4b555043: //Platinum (Korea)
          address=0x02017d88;
          break;
        case 0x4a4b5049: //HeartGold (Japan)
        case 0x4a475049: //SoulSilver (Japan)
          address=0x0201a224;
          break;
        case 0x4b4b5049: //HeartGold (Korea)
        case 0x4b475049: //SoulSilver (Korea)
          address=0x0201a508;
          break;
        case 0x464b5049: //HeartGold (France)
        case 0x454b5049: //HeartGold (USA) & HeartGold (Europe) (Rev 10)
        case 0x444b5049: //HeartGold (Germany)
        case 0x534b5049: //HeartGold (Spain)
        case 0x494b5049: //HeartGold (Italy)
        case 0x46475049: //SoulSilver (France)
        case 0x45475049: //SoulSilver (USA) & HeartGold (Europe) (Rev 10)
        case 0x44475049: //SoulSilver (Germany)
        case 0x53475049: //SoulSilver (Spain)
        case 0x49475049: //SoulSilver (Italy)
          address=0x0201a570;
          break;
      }
      if(address) PatchMem(KArm9,-address,0x46c0); // nop
    }
  }
  else if(0x45545559==GameCode()) //"Ultimate Mortal Kombat (USA) (En,Fr,De,Es,It)"
  {
    if(IsDMA())
    {
      PatchMem(KArm9,0x0200b250,0xe5c02002); //strb r2, [r0, #2]
      PatchMem(KArm9,0x0200d718,GenerateArmBL(0x0200d718,0x02000b90,false)); //b xxx
      PatchMem(KArm9,0x02000b90,0xe2500000); //subs r0, r0, #0
      PatchMem(KArm9,0x02000b94,0x13a00001); //movne r0, #1
      PatchMem(KArm9,0x02000b98,0xe5c10002); //strb r0, [r1, #2]
      PatchMem(KArm9,0x02000b9c,0xe8bd8008); //ldmfd  sp!, {r3, pc}
    }
  }
  else if(0x50545559==GameCode()) //"Ultimate Mortal Kombat (Europe) (En,Fr,De,Es,It)"
  {
    if(IsDMA())
    {
      PatchMem(KArm9,0x0200b3c4,0xe5c02000); //strb r2, [r0]
      PatchMem(KArm9,0x0200d920,GenerateArmBL(0x0200d920,0x02000b90,false)); //b xxx
      PatchMem(KArm9,0x02000b90,0xe2500000); //subs r0, r0, #0
      PatchMem(KArm9,0x02000b94,0x13a00001); //movne r0, #1
      PatchMem(KArm9,0x02000b98,0xe5c10000); //strb r0, [r1]
      PatchMem(KArm9,0x02000b9c,0xe8bd8008); //ldmfd  sp!, {r3,pc}
    }
  }
}

// introes cause problems.
// i hope all this trash will redumped and this function will removed.
void CDsPatcher::FixIntros(void)
{
  switch(GameCode())
  {
    case 0x45553543: //4953 - Are You Smarter than a 5th Grader - Game Time (USA)
    case 0x45495342: //4982 - Safecracker - The Ultimate Puzzle Challenge! (USA) (En,Fr,Es)
    case 0x45415343: //5023 - Safari Adventures - Africa (USA)
    case 0x45325342: //5024 - Smart Kid's Mega Game Mix (USA)
    case 0x45324242: //5043 - My Baby - First Steps (USA) (En,Fr,Es)
    case 0x45465042: //5044 - Princess and the Frog, The (USA) (En,Fr,Es)
    case 0x45504442: //5052 - Dora Puppy (USA)
      if(*(u32*)0x02000800!=0xe3a0c301)
      {
        PatchMem(KArm7,0x02380000,0xe3a0c301); //mov r12, #0x4000000
        PatchMem(KArm9,0x02000800,0xe3a0c301); //mov r12, #0x4000000
      }
      break;
  }
}

void CDsiPatcher::FixRam(void)
{
  u32* top=(u32*)__NDSHeader->arm7destination;
  u32* bottom=(u32*)(__NDSHeader->arm7destination+__NDSHeader->arm7binarySize);
  for(u32* ii=top;ii<bottom;++ii)
  {
    if(0x02ffc000==*ii&&0x02fff000==*(ii+1))
    {
      PatchMem(KArm7,s32(ii+1),0x02ffc000);
    }
  }
}

void CDsiPatcher::PatchDumbSave(void)
{
  switch(GameCode())
  {
    case 0x4a415249: //IRAJ: 5216 - Pocket Monsters - White (Japan) (NDSi Enhanced)
      NeedUncompressed();
      PatchOverlay14(0x02079038,KMemProtMeAA,231,0x02180c78);
      break;
    case 0x4a425249: //IRBJ: 5217 - Pocket Monsters - Black (Japan) (NDSi Enhanced)
      NeedUncompressed();
      PatchOverlay14(0x02079020,KMemProtMeAA,231,0x02180c58);
      break;
  }
}

void CDsiPatcher::FixBugs(void)
{
  switch(GameCode())
  {
    case 0x56444956: //5225 - Imagine - Dream Resort (Europe) (NDSi Enhanced)
      if(IsCommon()||IsDMA()||IsSDSave()) NeedUncompressed();
      break;
  }
}

void CDsiPatcher::PatchFlashCardCheck(void)
{
  switch(GameCode())
  {
    //patch "4184 - Idolm@ster, The - Dearly Stars (Japan) (NDSi Enhanced)"
    case 0x4a4d4956: //VIMJ
      NeedUncompressed();
      PatchOverlay6(0x02016e34,KDsiProtMe); //size 1024
      break;
    //patch "4518 - Power Pro Kun Pocket 12 (Japan) (NDSi Enhanced)"
    case 0x4a545056: //VPTJ
      NeedUncompressed();
      PatchOverlay6(0x0200b1c0,KDsiProtMe); //size 1024
      break;
    //patch "4755 - Kira Kira Rhythm Collection (Japan) (NDSi Enhanced)"
    case 0x4a474356: //VCGJ
      NeedUncompressed();
      PatchOverlay9(0x02013868,KDsiProtMe,280); //size 1024
      break;
    //patch "4762 - Alice in Wonderland (USA) (En,Fr,Es) (NDSi Enhanced)"
    case 0x454c4156: //VALE
      NeedUncompressed();
      PatchOverlay11(0x0201ac5c,KDsiProtMe,0x020190e9,true); //size 1024
      break;
    //patch "4789 - RPG Tkool DS (Japan) (NDSi Enhanced)"
    case 0x4a393256: //V29J
      FixPatch01(unprot_011_bin,unprot_011_bin_size,EFix01);
      NeedUncompressed();
      PatchOverlay11(0x020813b0,KDsiProtMe,0x0207f38d,true); //size 1024
      break;
    //patch "4798 - Alice in Wonderland (Europe) (En,Fr,De,Es,It,Nl) (NDSi Enhanced)"
    case 0x564c4156: //VALV
      NeedUncompressed();
      PatchOverlay11(0x0201a1c0,KDsiProtMe,0x0201880d,true); //size 1024
      break;
    //patch "4868 - Korg DS-10 Plus (USA) (NDSi Enhanced)"
    case 0x45474b56: //VKGE
      NeedUncompressed();
      PatchOverlay6(0x020106bc,KDsiProtMe);
      break;
    //patch "4869 - Sonny With a Chance (USA) (NDSi Enhanced)"
    case 0x454e5356: //VSNE
      NeedUncompressed();
      PatchOverlay9(0x02018080,KDsiProtMe,280); //size 1024
      break;
    //patch "4947 - Petz - Fantasy (Europe) (En,Fr,De,Es,It,Nl,Sv,No,Da) (NDSi Enhanced)"
    case 0x565a4656: //VFZV
      NeedUncompressed();
      PatchOverlay9(0x020643d0,KDsiProtMe,280); //size 1024
      break;
    //patch "4949 - Cosmetic Paradise - Kirei no Mahou (Japan) (NDSi Enhanced)"
    case 0x4a504356: //VCPJ
      NeedUncompressed();
      PatchOverlay12(0x020125f0,KDsiProtMe,0x020100fc,280,true); //size 1024
      break;
    //patch "4952 - sonny with a chance (europe) (En,Fr,De,Es,It) (NDSi Enhanced)"
    case 0x564e5356: //VSNV
      NeedUncompressed();
      PatchOverlay9(0x02017a48,KDsiProtMe,280); //size 1024
      break;
    //patch "5016 - Toy Story 3 (Europe) (En,Fr,De,Es,It,Nl) (NDSi Enhanced)"
    case 0x56335456: //VT3V
      NeedUncompressed();
      PatchOverlay12(0x020112e0,KDsiProtMe,0x0200ec74,280,true); //size 1024
      break;
    //patch "5054 - Eigokoro Kyoushitsu DS (Japan) (NDSi Enhanced)"
    case 0x4a414156: //VAAJ
      NeedUncompressed();
      PatchOverlay12(0x0206b378,KDsiProtMe,0x02068e8c,280,true); //size 1024
      break;
    //patch "5055 - Elminage II  - Sousei no Megami to Unmei no Daichi - DS Remix (Japan) (NDSi Enhanced)"
    case 0x4a334c56: //VL3J
      NeedUncompressed();
      PatchOverlay12(0x020152f0,KDsiProtMe,0x02012df0,280,true); //size 1024
      break;
    //patch "5097 - Fire Emblem - Shin Monshou no Nazo Hikari to Kage no Eiyuu (Japan) (Rev 1) (NDSi Enhanced)"
    case 0x4a324956: //VI2J
      FixPatch02(unprot_012_bin,unprot_012_bin_size,EFix01);
      NeedUncompressed();
      PatchOverlay12(0x020c3cf0,KDsiProtMe,0x020c172c,280,true); //size 1024
      break;
    //patch "5127 - Toy Story 3 (USA) (En,Fr,Es) (NDSi Enhanced)"
    case 0x45335456: //VT3E
      NeedUncompressed();
      PatchOverlay12(0x020168d4,KDsiProtMe,0x02013e8c,280,true); //size 1024
      break;
    //patch "5161 - 1000 Cooking Recipes from Elle a Table (Europe) (En,Fr,De,Es,It) (NDSi Enhanced)"
    case 0x50544556: //VETP
      FixPatch02(unprot_012_bin,unprot_012_bin_size,EFix01);
      NeedUncompressed();
      PatchOverlay12(0x020542b4,KDsiProtMe,0x02051e00,280,true); //size 1024
      break;
    //patch "5162 - Art Academy (Europe) (En,Fr,De,Es,It) (NDSi Enhanced)"
    case 0x56414156: //VAAV
      NeedUncompressed();
      PatchOverlay12(0x0206ae60,KDsiProtMe,0x02068974,280,true); //size 1024
      break;
    //patch "5216 - Pocket Monsters - White (Japan) (NDSi Enhanced)"
    case 0x4a415249: //IRAJ
      FixPatch02(unprot_012_bin,unprot_012_bin_size,EFix01);
      NeedUncompressed();
      PatchOverlay12(0x0207906c,KMemProtMe,0x02076ae0,280,true); //size 1024
      break;
    //patch "5217 - Pocket Monsters - Black (Japan) (NDSi Enhanced)"
    case 0x4a425249: //IRBJ
      FixPatch02(unprot_012_bin,unprot_012_bin_size,EFix01);
      NeedUncompressed();
      PatchOverlay12(0x02079054,KMemProtMe,0x02076ac8,280,true); //size 1024
      break;
    //patch "5274 - Camp Rock - The Final Jam (Europe) (En,Fr,De,Es,It,Nl) (NDSi Enhanced)"
    case 0x504d4356: //VCMP
      FixPatch01(unprot_011_bin,unprot_011_bin_size,EFix02);
      NeedUncompressed();
      PatchOverlay11(0x02018850,KDsiProtMe,0x0201bebd,true);
      break;
    //patch "5280 - Crime Lab - Body of Evidence (Europe) (En,Es,It,Nl) (NDSi Enhanced)"
    case 0x504f4156: //VAOP
      FixPatch02(unprot_012_bin,unprot_012_bin_size,EFix01);
      NeedUncompressed();
      PatchOverlay12(0x02136124,KDsiProtMe,0x02133b40,280,true); //size 1024
      break;
    //patch "5289 - Solatorobo (Japan) (NDSi Enhanced)"
    case 0x4a444356: //VCDJ
      FixPatch02(unprot_012_bin,unprot_012_bin_size,EFix01);
      NeedUncompressed();
      PatchOverlay12(0x02010170,KMemProtMe,0x0200da40,280,true); //size 1024
      break;
    //patch "5291 - Sims 3, The (Europe) (En,Fr,De,Es,It,Nl) (NDSi Enhanced)"
    case 0x56335356: //VS3V
      //not compressed
      PatchOverlay13(0x020cb10c,KDsiProtMe,0x020c8cc8,true);
      break;
    //patch "5325 - Mario vs. Donkey Kong - Mini-Land Mayhem (USA) (En,Fr,Es) (Rev 1) (NDSi Enhanced)"
    case 0x45473256: //V2GE
      //not compressed
      PatchOverlay13(0x02050cf4,KDsiProtMe,0x0204e734,true);
      break;
    //patch "5364 - Tron Evolution (Europe) (En,Fr,De,Es,It,Nl) (NDSi Enhanced)"
    case 0x56455456: //VTEV
      NeedUncompressed();
      PatchOverlay13(0x020215f8,KMemProtMe,0x0201e8d4,true);
      break;
    //patch "5369 - Super Kaseki Horider (Japan) (NDSi Enhanced)"
    case 0x4a454456: //VDEJ
      NeedUncompressed();
      PatchOverlay17(0x0207e5d4,KMemProtMe,0x0207cca9,true);
      break;
    //patch "5378 - Power Pro Kun Pocket 13 (Japan) (NDSi Enhanced)"
    case 0x4a4c5056: //VPLJ
      NeedUncompressed();
      PatchOverlay13(0x0200ae98,KDsiProtMe,0x020088b8,true);
      break;
    //patch "5386 - Mario vs. Donkey Kong - Totsugeki! Mini-Land (Japan) (NDSi Enhanced)"
    case 0x4a473256: //V2GJ
      //not compressed
      PatchOverlay13(0x020516ac,KDsiProtMe,0x0204f0e0,true);
      break;
    //patch "5394 - Camp Rock - The Final Jam (Europe) (En,Sv,No,Da) (NDSi Enhanced)"
    case 0x564d4356: //VCMV
      FixPatch01(unprot_011_bin,unprot_011_bin_size,EFix02);
      NeedUncompressed();
      PatchOverlay11(0x02018850,KDsiProtMe,0x0201bebd,true);
      break;
    //patch "5396 - Phineas and Ferb Ride Again (Europe) (En,De,Es,It,Nl) (NDSi Enhanced)"
    case 0x56465056: //VPFV
      FixPatch02(unprot_012_bin,unprot_012_bin_size,EFix01);
      NeedUncompressed();
      PatchOverlay12(0x02009858,KDsiProtMe,0x02007418,280,true); //size 1024
      break;
    //patch "5460 - Art Academy (Korea) (NDSi Enhanced)"
    case 0x4b414156: //VAAK
      NeedUncompressed();
      PatchOverlay12(0x0206bbec,KDsiProtMe,0x02069700,280,true); //size 1024
      break;
    //patch "5547 - Mario vs. Donkey Kong - Mini-Land Mayhem! (Europe) (En,Fr,De,Es,It) (NDSi Enhanced)"
    case 0x56473256: //V2GV
      //not compressed
      PatchOverlay13(0x02052214,KDsiProtMe,0x0204fc48,true);
      break;
    //patch "5557 - Phineas and Ferb - Ride Again (USA) (NDSi Enhanced)"
    case 0x45465056: //VPFE
      NeedUncompressed();
      PatchOverlay12(0x02009cf8,KDsiProtMe,0x0200752c,280,true); //size 1024
      break;
  }
}

void CPatcher::FixPatch01(const u8* aPatch,u32 aSize,TFixType aType)
{
  for(u32* ii=(u32*)aPatch;ii<(u32*)(aPatch+aSize);++ii)
  {
    if(0x3affffd0==*ii)
    {
      switch(aType)
      {
        case EFix01:
          *ii=0x3affffd3; //bcc xxx
          break;
        case EFix02:
          *ii=0x3affffb0; //bcc xxx
          break;
      }
      *(ii+3)=0xe2673b3e; //rsb r3,r7,#0xf800
      *(ii+4)=0xe1a0ed07; //mov lr,r7,lsl #0x1a
      *(ii+5)=0xe0043003; //and r3,r4,r3
    }
    else if(0xe1a00008==*ii)
    {
      switch(aType)
      {
        case EFix01:
          *ii=0xe1a00009; //mov r0, r9
          *(ii+1)=0xe0801008; //add r1, r0, r8
          break;
        case EFix02:
          *ii=0xe1a00006; //mov r0, r6
          *(ii+1)=0xe0801005; //add r1, r0, r5
          break;
      }
    }
  }
}

void CPatcher::FixPatch02(const u8* aPatch,u32 aSize,TFixType aType)
{
  for(u32* ii=(u32*)aPatch;ii<(u32*)(aPatch+aSize);++ii)
  {
    switch(aType)
    {
      case EFix01:
        if(0x3affffb0==*ii)
        {
          *(ii+3)=0xe1a04d04; //mov r4,r4,lsl #0x1A
          *(ii+4)=0xe3a06005; //mov r6,#5
          *(ii+5)=0xe3c33b02; //bic r3,r3,#0x800
        }
        break;
      case EFix02:
        if(0x3affffd0==*ii)
        {
          *ii=0x3affffb0; //bcc xxx
          *(ii+3)=0xe1a04d04; //mov r4,r4,lsl #0x1A
          *(ii+4)=0xe3a06005; //mov r6,#5
          *(ii+5)=0xe3c33b02; //bic r3,r3,#0x800
        }
        else if(0xe1a00008==*ii)
        {
          *ii=0xe1a00006; //mov r0, r6
          *(ii+1)=0xe0801005; //add r1, r0, r5
        }
        break;
    }
  }
}
