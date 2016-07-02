/*
    patches_pr.cpp
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

#include "patches.h"
#include "unprot_009_bin.h"
#include "unprot_010_bin.h"
#include "unprot_011_bin.h"
#include "unprot_012_bin.h"
#include "unprot_013_bin.h"
#include "unprot_014_bin.h"
#include "unprot_015_bin.h"
#include "unprot_016_bin.h"
#include "unprot_017_bin.h"
#include "unprot_018_bin.h"

void CPatcher::PatchRead(const u8* aPatch,u32 aPatchSize,u32 aRead,bool aNew)
{
  for(u32* ii=(u32*)aPatch;ii<(u32*)(aPatch+aPatchSize);++ii)
  {
    if(0xefc0def1==*ii)
    {
      *ii=aRead;
    }
    else if(0xefc0def2==*ii)
    {
      *ii=((__NDSHeader->cardControl13)&~0x7000000)|0xa1000000;
    }
    else if(0xefc0def3==*ii)
    {
      *ii=aNew?0xe3a000b7:0xe3a004b7; //mov r0, #0xb7 : mov r0, #0xb7000000
    }
    else if(0xefc0def4==*ii)
    {
      *ii=aNew?0xe1a00000:0xe1800421; //nop : orr r0, r1, lsr#8
    }
    else if(0xefc0def5==*ii)
    {
      *ii=aNew?0xe1a00000:0xe3a01000; //nop : mov r1, #0
    }
  }
}

void CPatcher::PatchOverlay9(u32 aLoadAddress,u32 aPatchAddress,u32 aShift)
{
  u32 aLoadAddress_s=aLoadAddress-aShift;
  memcpy((void*)aPatchAddress,unprot_009_bin,unprot_009_bin_size);
  PatchMem(KArm9,aLoadAddress_s,GenerateArmBL(aLoadAddress_s,aPatchAddress,true));
  PatchMem(KArm9,aLoadAddress,GenerateArmBL(aLoadAddress,aPatchAddress+16,false));
}

void CPatcher::PatchOverlay10(u32 aFixAddress1,u32 aFixAddress2,u32 aFixAddress3,u32 aPatchAddress,u32 aCode,u32 aRead,bool aNew,u32 aBack)
{
  PatchRead(unprot_010_bin,unprot_010_bin_size,aRead,aNew);
  for(u32* ii=(u32*)unprot_010_bin;ii<(u32*)(unprot_010_bin+unprot_010_bin_size);++ii)
  {
    if(0xefc0de01==*ii)
    {
      *ii=GenerateArmBL(aFixAddress2,aPatchAddress+12,true);
    }
    else if(0xefc0de02==*ii)
    {
      *ii=aFixAddress2;
    }
    else if(0xefc0de03==*ii)
    {
      *ii=GenerateArmBL(aFixAddress3,aPatchAddress+40,false);
    }
    else if(0xefc0de04==*ii)
    {
      *ii=aFixAddress3;
    }
    else if(0xefc0de05==*ii)
    {
      *ii=aCode;
    }
    else if(0xefc0de06==*ii)
    {
      *ii=aBack;
    }
  }
  memcpy((void*)aPatchAddress,unprot_010_bin,unprot_010_bin_size);
  PatchMem(KArm9,aFixAddress1,GenerateArmBL(aFixAddress1,aPatchAddress,false));
}

void CPatcher::PatchOverlay11(s32 aLoadAddress,u32 aPatchAddress,u32 aRead,bool aNew)
{
  PatchRead(unprot_011_bin,unprot_011_bin_size,aRead,aNew);
  memcpy((void*)aPatchAddress,unprot_011_bin,unprot_011_bin_size);
  u32 jump=GenerateThumbBL(aLoadAddress,aPatchAddress);
  PatchMem(KArm9,-aLoadAddress,jump&0xffff);
  PatchMem(KArm9,-(aLoadAddress+2),jump>>16);
}

void CPatcher::PatchOverlay12(u32 aLoadAddress,u32 aPatchAddress,u32 aRead,u32 aShift,bool aNew)
{
  PatchRead(unprot_012_bin,unprot_012_bin_size,aRead,aNew);
  u32 aLoadAddress_s=aLoadAddress-aShift;
  memcpy((void*)aPatchAddress,unprot_012_bin,unprot_012_bin_size);
  PatchMem(KArm9,aLoadAddress_s,GenerateArmBL(aLoadAddress_s,aPatchAddress,true));
  PatchMem(KArm9,aLoadAddress,GenerateArmBL(aLoadAddress,aPatchAddress+16,false));
}

void CPatcher::PatchOverlay13(u32 aLoadAddress,u32 aPatchAddress,u32 aRead,bool aNew)
{
  PatchRead(unprot_013_bin,unprot_013_bin_size,aRead,aNew);
  memcpy((void*)aPatchAddress,unprot_013_bin,unprot_013_bin_size);
  PatchMem(KArm9,aLoadAddress,GenerateArmBL(aLoadAddress,aPatchAddress,true));
}

void CPatcher::PatchOverlay14(u32 aLoadAddress,u32 aPatchAddress,u32 anOverlay,u32 anAddress)
{
  for(u32* ii=(u32*)unprot_014_bin;ii<(u32*)(unprot_014_bin+unprot_014_bin_size);++ii)
  {
    if(0xefc0de01==*ii)
    {
      *ii=anOverlay;
    }
    else if(0xefc0de02==*ii)
    {
      *ii=anAddress;
    }
  }
  memcpy((void*)aPatchAddress,unprot_014_bin,unprot_014_bin_size);
  PatchMem(KArm9,aLoadAddress,GenerateArmBL(aLoadAddress,aPatchAddress,true));
}

void CPatcher::PatchOverlay15(u32 aLoadAddress,u32 aPatchAddress,u32 aRead,bool aNew)
{
  PatchRead(unprot_015_bin,unprot_015_bin_size,aRead,aNew);
  memcpy((void*)aPatchAddress,unprot_015_bin,unprot_015_bin_size);
  PatchMem(KArm9,aLoadAddress,GenerateArmBL(aLoadAddress,aPatchAddress,true));
}

void CPatcher::PatchOverlay16(s32 aLoadAddress,u32 aPatchAddress,u32 aRead,bool aNew)
{
  PatchRead(unprot_016_bin,unprot_016_bin_size,aRead,aNew);
  memcpy((void*)aPatchAddress,unprot_016_bin,unprot_016_bin_size);
  u32 jump=GenerateThumbBL(aLoadAddress,aPatchAddress);
  PatchMem(KArm9,-aLoadAddress,jump&0xffff);
  PatchMem(KArm9,-(aLoadAddress+2),jump>>16);
}

void CPatcher::PatchOverlay17(s32 aLoadAddress,u32 aPatchAddress,u32 aRead,bool aNew)
{
  PatchRead(unprot_017_bin,unprot_017_bin_size,aRead,aNew);
  memcpy((void*)aPatchAddress,unprot_017_bin,unprot_017_bin_size);
  u32 jump=GenerateThumbBL(aLoadAddress,aPatchAddress);
  PatchMem(KArm9,-aLoadAddress,jump&0xffff);
  PatchMem(KArm9,-(aLoadAddress+2),jump>>16);
}

void CPatcher::PatchOverlay18(s32 aLoadAddress,u32 aPatchAddress,u32 aRead,bool aNew)
{
  PatchRead(unprot_018_bin,unprot_018_bin_size,aRead,aNew);
  memcpy((void*)aPatchAddress,unprot_018_bin,unprot_018_bin_size);
  PatchMem(KArm9,aLoadAddress,GenerateArmBL(aLoadAddress,aPatchAddress,true));
}

void CDsPatcher::PatchFlashCardCheck2(void)
{
  struct SPatchInfo
  {
    u32 iGameCode;
    u32 iPatch;
    u32 iLoad;
  };
  static const SPatchInfo info[]=
  {
    {0x4a464c42,0x02009304,KMemProtMe}, //BLFJ: 4492 - Layton Kyouju to Majin no Fue (Japan)
    {0x4a584b42,0x020a15d4,KMemProtMe}, //BKXJ: 4510 - Kaijuu Busters (Japan)
    {0x4a4d4b42,0x020332f4,KMemProtMe}, //BKMJ: 4514 - Rilakkuma Rhythm - Mattari Kibun de Dararan Ran (Japan)
    {0x45494b42,0x0202dacc,KMemProtMe}, //BKIE: 4527 - Legend of Zelda, The - Spirit Tracks (USA) (En,Fr,Es)
    {0x4a484b42,0x0200ef6c,KMemProtMe}, //BKHJ: 4536 - Kamonohashikamo. - Aimai Seikatsu no Susume (Japan)
    {0x50494b42,0x0202daf4,KMemProtMe}, //BKIP: 4544 - Legend of Zelda, The - Spirit Tracks (Europe) (En,Fr,De,Es,It)
    {0x4a374d42,0x0200cd34,KMemProtMe}, //BM7J: 4551 - Gokujou!! Mecha Mote Iinchou - MM Town de Miracle Change! (Japan)
    {0x4a554242,0x020b5958,KMemProtMe}, //BBUJ: 4554 - Metal Fight Beyblade - Bakutan Cyber Pegasis (Japan)
    {0x50554359,0x0205921c,KMemProtMe}, //YCUP: 4572 - Nintendo Presents - Crossword Collection (Europe)
    {0x4a504942,0x020c2acc,KMemProtMe}, //BIPJ: 4579 - Pen 1 Grand Prix - Penguin no Mondai Special (Japan)
    {0x4a343343,0x0200cd18,KMemProtMe}, //C34J: 4588 - Mini Yonku DS (Japan)
    {0x4a415442,0x0200abb4,KMemProtMe}, //BTAJ: 4590 - Lina no Atelier - Strahl no Renkinjutsushi (Japan)
    {0x4a494b42,0x0202f024,KMemProtMe}, //BKIJ: 4594 - Zelda no Densetsu - Daichi no Kiteki (Japan)
    {0x4b4e3442,0x0200a330,KMemProtMe}, //B4NK: 4603 - Princess Angel (Korea)
    {0x4a505042,0x02088760,KMemProtMe}, //BPPJ: 4604 - Post Pet DS - Yume Miru Momo to Fushigi no Pen (Japan)
    {0x4a474442,0x0200cf44,KMemProtMe}, //BDGJ: 4611 - Dear Girl - Stories Hibiki - Hibiki Tokkun Daisakusen! (Japan)
    {0x4a324442,0x0200f528,KMemProtMe}, //BD2J: 4617 - Deca Sporta DS de Sports 10 Shumoku (Japan)
    //not compressed
    {0x4a473542,0x0200a720,KMemProtMe}, //B5GJ: 4692 - Shin Sengoku Tenkatouitsu - Gunyuu Tachi no Souran (Japan)
    {0x4b475049,0x020d9338,KMemProtMe}, //IPGK: 4698 - Pocket Monsters - Soul Silver (Korea)
    {0x4b465241,0x020a5640,KMemProtMe}, //ARFK: 4699 - Rune Factory - Shin Bokujou Monogatari (Korea)
    {0x4b4b5049,0x020d9340,KMemProtMe}, //IPKK: 4701 - Pocket Monsters - Heart Gold (Korea)
    {0x4a334c42,0x02078c44,KMemProtMe}, //BL3J: 4706 - Lupin Sansei - Shijou Saidai no Zunousen (Japan)
    {0x4a325142,0x020e4ce4,KMemProtMe}, //BQ2J: 4707 - Quiz Magic Academy DS - Futatsu no Jikuuseki (Japan)
    {0x4a424442,0x020b3624,KMemProtMe}, //BDBJ: 4712 - Dragon Ball DS 2 - Totsugeki! Red Ribbon Gun (Japan)
    {0x4a434e42,0x0200c938,KMemProtMe}, //BNCJ: 4720 - Nodame Cantabile - Tanoshii Ongaku no Jikan Desu (Japan)
    {0x4a353342,0x0200aec4,KMemProtMe}, //B35J: 4724 - Sangokushi DS 3 (Japan)
    {0x4a445342,0x02106904,KMemProtMe}, //BSDJ: 4737 - Estpolis - The Lands Cursed by the Gods (Japan)
    {0x4a463643,0x02029998,KMemProtMe}, //C6FJ: 4738 - Mugen no Frontier Super Robot Taisen OG Saga Exceed (Japan)
    {0x4a575742,0x0201ed78,KMemProtMe}, //BWWJ: 4739 - WiZmans World (Japan)
    {0x4a4f5042,0x020275f8,KMemProtMe}, //BPOJ: 4742 - Utacchi (Japan)
    {0x4a515242,0x0200aaf4,KMemProtMe}, //BRQJ: 4746 - Gendai Daisenryaku DS - Isshoku Sokuhatsu, Gunji Balance Houkai (Japan)
    {0x4a495142,0x020d0970,KMemProtMe}, //BQIJ: 4747 - Cooking Idol - I! My! Mine! - Game de Hirameki! Kirameki! Cooking (Japan)
    {0x4a4a4b42,0x020097fc,KMemProtMe}, //BKJJ: 4751 - Nazotte Oboeru Otona no Kanji Renshuu Kaiteiban (Japan)
    {0x4a355442,0x0209046c,KMemProtMe}, //BT5J: 4752 - TOEIC Test Super Coach @DS (Japan)
    {0x4a475442,0x0200a79c,KMemProtMe}, //BTGJ: 4756 - Tennis no Ouji-sama - Motto Gakuensai no Ouji-sama - More Sweet Edition (Japan)
    {0x4a523342,0x0207d39c,KMemProtMe}, //B3RJ: 4768 - Pokemon Ranger - Hikari no Kiseki (Japan)
    {0x444e4259,0x020c9928,KMemProtMe}, //YBND: 4770 - Bibliothek der Klassischen Buecher (Germany)
    {0x464e4259,0x020c9b1c,KMemProtMe}, //YBNF: 4783 - 100 Livres Classiques (France)
    {0x464b5049,0x020d8c4c,KMemProtMe}, //IPKF: 4786 - Pokemon - Version or HeartGold (France)
    {0x454b5049,0x020d8c4c,KMemProtMe}, //IPKE: 4787 - Pokemon - HeartGold Version (USA) & 4839 - Pokemon - HeartGold (Europe) (Rev 10)
    {0x45475049,0x020d8c4c,KMemProtMe}, //IPGE: 4788 - Pokemon - SoulSilver Version (USA) & 4840 - Pokemon - SoulSilver (Europe) (Rev 10)
    {0x46475049,0x020d8c4c,KMemProtMe}, //IPGF: 4791 - Pokemon - Version Argent SoulSilver (France)
    {0x45324442,0x0200f528,KMemProtMe}, //BD2E: 4797 - Deca Sports DS (USA) (En,Fr,Es)
    {0x4a555042,0x02006de8,KMemProtMe}, //BPUJ: 4807 - Nettou! Powerful Koushien (Japan)
    {0x4a334b42,0x0200ebc4,KMemProtMe}, //BK3J: 4809 - Kawaii Koinu DS 3 (Japan)
    {0x4a425142,0x020973fc,KMemProtMe}, //BQBJ: 4810 - Crayon Shin-Chan - Obaka Daininden - Susume! Kasukabe Ninja Tai! (Japan)
    {0x454d5242,0x0200df18,KMemProtMe}, //BRME: 4813 - Rooms - The Main Building (USA) (En,Fr,Es)
    {0x45544d42,0x0200c650,KMemProtMe}, //BMTE: 4821 - Shin Megami Tensei - Strange Journey (USA)
    {0x444b5049,0x020d8c4c,KMemProtMe}, //IPKD: 4827 - Pokemon - Goldene Edition HeartGold (Germany)
    {0x44475049,0x020d8c4c,KMemProtMe}, //IPGD: 4828 - Pokemon - Silberne Edition SoulSilver (Germany)
    {0x534b5049,0x020d8c44,KMemProtMe}, //IPKS: 4832 - Pokemon - Edicion Oro HeartGold (Spain)
    {0x53475049,0x020d8c4c,KMemProtMe}, //IPGS: 4833 - Pokemon - Edicion Plata SoulSilver (Spain)
    {0x4a573242,0x0200abbc,KMemProtMe}, //B2WJ: 4843 - Moe Moe 2-Ji Daisenryaku 2 - Yamato Nadeshiko (Japan)
    {0x49475049,0x020d8c4c,KMemProtMe}, //IPGI: 4848 - Pokemon - Versione Argento SoulSilver (Italy)
    {0x4a334a42,0x02009244,KMemProtMe}, //BJ3J: 4849 - Sekaiju no Meikyuu III - Seikai no Raihousha (Japan)
    {0x494b5049,0x020d8c4c,KMemProtMe}, //IPKI: 4851 - Pokemon - Versione Oro HeartGold (Italy)
    //not compressed
    {0x4a335143,0x020098a8,KMemProtMe}, //CQ3J: 4860 - Sengoku Spirits - Moushouden (Japan)
    //not compressed
    {0x4a345259,0x020098a8,KMemProtMe}, //YR4J: 4861 - Sengoku Spirits - Shukunden (Japan)
    {0x4b585a43,0x02058e04,KMemProtMe}, //CZXK: 4862 - Touch Man to Man - Gicho Yeongeo (Korea)
    {0x4a393542,0x0201f064,KMemProtMe}, //B59J: 4870 - Saku Saku Jinkou Kokyuu Care Training DS (Japan)
    {0x4a395142,0x0201e778,KMemProtMe}, //BQ9J: 4871 - Tekipaki Kyuukyuu Kyuuhen Training DS (Japan)
    {0x50324442,0x0200f528,KMemProtMe}, //BD2P: 4890 - Sports Island DS (Europe) (En,Fr,De,Es,It)
    {0x4a4d5242,0x0200df18,KMemProtMe}, //BRMJ: 4912 - Rooms - Fushigi na Ugoku Heya (Japan)
    {0x504d5242,0x0200df18,KMemProtMe}, //BRMP: 4985 - Rooms - The Main Building (Europe) (En,Fr,De,Es,It)
    {0x45424442,0x020b3590,KMemProtMe}, //BDBE: 5030 - Dragon Ball - Origins 2 (USA) (En,Fr,Es)
    {0x50424442,0x020b35c4,KMemProtMe}, //BDBP: 5037 - Dragon Ball - Origins 2 (Europe) (En,Fr,De,Es,It)
    //not compressed
    {0x4a325143,0x020098a8,KMemProtMe}, //CQ2J: 5068 - Sengoku Spirits - Gunshiden (Japan)
    {0x4a554b42,0x02020e70,KMemProtMe}, //BKUJ: 5074 - Bokujou Monogatari - Futago no Mura (Japan)
    {0x4b4d5241,0x0203af68,KMemProtMe}, //ARMK: 5075 - Mario Luigi - Partners in Time (Korea)
    //not compressed
    {0x4b554242,0x020b5344,KMemProtMe}, //BBUK: 5122 - Metal Beyblade - Cyber Pegasis (Korea)
    {0x45334a42,0x020090d4,KMemProtMe}  //BJ3E: 5233 - Etrian Odyssey 3 - Drowned City (USA)
  };
  struct SPatchInfo2
  {
    u32 iGameCode;
    u32 iPatch1;
    u32 iPatch2;
    u32 iPatch3;
    u32 iLoad;
    u32 iCode;
    u32 iRead;
    bool iNew;
    u32 iBack;
  };
  static const SPatchInfo2 info2[]=
  {
    {0x4a435742,0x020fd010,0x020fcf44,0x020fc6d0,KMemProtMe,0xe1510008,0x0200ca10,false,0xe8bd8000}, //cmp r1, r8 //ldmfd sp!, {pc}     //BWCJ: 4704 - Minna no Conveni (Japan)
    {0x4a4b4b42,0x0205c160,0x0205c084,0x0205b68c,KMemProtMe,0xe1510009,0x0200dba8,false,0xe8bd8000}, //cmp r1, r9 //ldmfd sp!, {pc}     //BKKJ: 4764 - Keroro RPG - Kishi to Musha to Densetsu no Kaizoku (Japan)
    {0x4a324642,0x021b36d4,0x021b35f8,0x021b2c00,KMemProtMe,0xe1510009,0x021b4300,false,0xe8bd8000}, //cmp r1, r9 //ldmfd sp!, {pc}     //BF2J: 4823 - Pro Yakyuu Famista DS 2010 (Japan)
    {0x4a595242,0x02026a8c,0x020269b0,0x02025fb8,KMemProtMe,0xe1510009,0x020397e0,false,0xe8bd8000}, //cmp r1, r9 //ldmfd sp!, {pc}     //BRYJ: 4863 - Hudson x GReeeeN Live! DeeeeS! (Japan)
    {0x4b505142,0x020f42a8,0x020f41cc,0x020f36dc,KMemProtMe,0xe1510006,0x0211bff8,false,0xe8bd8000}, //cmp r1, r6 //ldmfd sp!, {pc}     //BQPK: 4939 - Figure Princess (Korea)
    {0x50554c59,0x02085894,0x0207ee9c,0x0207e508,KMemProtMe,0xe1510006,0x0207a308,false,0xe8bd8010}  //cmp r1, r6 //ldmfd sp!, {r4, pc} //YLUP: 5212 - Last Window - The Secret of Cape West (Europe) (En,Fr,De,Es,It)
  };
  struct SPatchInfo3
  {
    u32 iGameCode;
    u32 iPatch;
    u32 iLoad;
    u32 iRead;
    bool iNew;
  };
  static const SPatchInfo3 info3[]=
  {
    {0x4a585942,0x0207cdc2,KMemProtMe,0x0207fea9,false}, //BYXJ: 4721 - Yu-Gi-Oh! 5D's World Championship 2010 - Reverse of Arcadia (Japan) (En,Ja,Fr,De,Es,It)
    {0x4a463442,0x0208b496,KMemProtMe,0x0208d969,false}, //B4FJ: 4736 - Fushigi no Dungeon - Fuurai no Shiren 4 (Japan)
    {0x45585942,0x0207cdc2,KMemProtMe,0x0207fea9,false}, //BYXE: 4781 - Yu-Gi-Oh! 5D's - World Championship 2010 - Reverse of Arcadia (USA) (En,Ja,Fr,De,Es,It)
    {0x50585942,0x0207cdce,KMemProtMe,0x0207feb5,false}  //BYXP: 4852 - Yu-Gi-Oh! 5D's - World Championship 2010 - Reverse of Arcadia (Europe) (En,Ja,Fr,De,Es,It)
  };
  static const SPatchInfo info4[]=
  {
    {0x4a555142,0x020661f0,KMemProtMe}, //BQUJ: 4824 - Minna no Suizokukan (Japan)
    {0x4a584342,0x020d1140,KMemProtMe}, //BCXJ: 4856 - Kodawari Saihai Simulation - Ochanoma Pro Yakyuu DS 2010 Nendoban (Japan)
    {0x4a323442,0x0202b0cc,KMemProtMe}, //B42J: 4857 - Pet Shop Monogatari DS 2 (Japan)
    {0x45354143,0x02092fd0,KMemProtMe}, //CA5E: 4876 - Again (USA)
    {0x4a583342,0x02070110,KMemProtMe}, //B3XJ: 4974 - Kunio-Kun no Chou Nekketsu! - Soccer League Plus - World Hyper Cup Hen (Japan)
    {0x45583342,0x020702ac,KMemProtMe}, //B3XE: 5010 - River City - Soccer Hooligans (USA)
    {0x4a4b4342,0x0200b728,KMemProtMe}, //BCKJ: 5047 - Kochira Katsushikaku Kameari Kouenmae Hashutsujo - Machiteba Tengoku! Makereba Jigoku! Ryoutsuryuu Ikkakusenkin Daisakusen! (Japan)
    {0x4b5a5a43,0x02008c18,KMemProtMe}, //CZZK: 5049 - Motmallineun Samgongjuwa Hamkkehaneun Geurimyeonsang Yeongdaneo Amgibeop (Korea)
    {0x4a454c42,0x0200bb18,KMemProtMe}  //BLEJ: 5056 - Digimon Story - Lost Evolution (Japan)
  };
  static const SPatchInfo3 info5[]=
  {
    {0x4a583742,0x0200ad28,KMemProtMe,0x0200f698,true }, //B7XJ: 4829 - Seitokai no Ichizon - DS-suru Seitokai (Japan)
    {0x4a524a43,0x0200c970,KMemProtMe,0x020109a8,false}, //CJRJ: 4898 - Dragon Quest Monsters - Joker 2 (Japan)
    {0x50425855,0x020d9838,KMemProtMe,0x020dd890,false}, //UXBP: 4950 - Jam with the Band (Europe) (En,Fr,De,Es,It)
    {0x4a423242,0x020300b8,KMemProtMe,0x0203400c,false}, //B2BJ: 4958 - Captain Tsubasa - Gekitou no Kiseki (Japan)
    {0x4a4f5242,0x0200af60,KMemProtMe,0x0200f774,true }, //BROJ: 4962 - Super Robot Taisen OG Saga - Masou Kishin - The Lord of Elemental (Japan)
    {0x4a4f3643,0x02016b08,KMemProtMe,0x02014578,true }, //C6OJ: 4963 - Medarot DS - Kabuto Ver. (Japan)
    {0x4a575142,0x02016b08,KMemProtMe,0x02014578,true }, //BQWJ: 4964 - Medarot DS - Kuwagata Ver. (Japan)
    {0x4a334742,0x02048efc,KMemProtMe,0x02046c70,true }, //BG3J: 4965 - G.G Series - Collection+ (Japan)
    {0x4a575342,0x0202e5a0,KMemProtMe,0x0202b66c,true }, //BSWJ: 4966 - Saka Tsuku DS - World Challenge 2010 (Japan)
    {0x45543442,0x0201060c,KMemProtMe,0x020157a4,true }, //B4TE: 4968 - Tetris Party Deluxe (USA) (En,Fr,Es)
    {0x4a4d5942,0x0200ccc4,KMemProtMe,0x02010f00,false}, //BYMJ: 4976 - Yumeiro Patissiere - My Sweets Cooking (Japan)
    {0x4a5a3642,0x0201f4e8,KMemProtMe,0x02022ab0,false}, //B6ZJ: 4993 - Rockman Zero Collection (Japan)
    {0x455a3642,0x0201f4e8,KMemProtMe,0x02022ab0,false}, //B6ZE: 4996 - Mega Man Zero Collection (USA)
    {0x505a3642,0x0201f4e8,KMemProtMe,0x02022ab0,false}, //B6ZP: 5006 - Mega Man Zero Collection (Europe)
    {0x4a354742,0x0200cb7c,KMemProtMe,0x020119d0,true }, //BG5J: 5009 - Tamagotchi no Pichi Pichi Omisecchi (Japan)
    {0x454e4259,0x020cc9bc,KMemProtMe,0x020d0aec,false}, //YBNE: 5014 - 100 Classic Books (USA)
    {0x4a544742,0x0208e9ec,KMemProtMe,0x0208c648,true }, //BGTJ: 5018 - Ghost Trick (Japan)
    {0x4a4b4a42,0x02009798,KMemProtMe,0x0200c688,false}, //BJKJ: 5046 - Kanjukuken DS (Japan)
    {0x4a424f42,0x020096e4,KMemProtMe,0x0200e674,true }, //BOBJ: 5069 - Kaidan Restaurant Ura Menu 100-Sen (Japan)
    {0x45514459,0x020cd480,KMemProtMe,0x020d08d8,false}, //YDQE: 5076 - Dragon Quest IX - Sentinels of the Starry Skies (USA) (En,Fr,Es)
    {0x45385342,0x0201bc54,KMemProtMe,0x02019560,true }, //BS8E: 5095 - Sorcerer's Apprentice, The (USA) (En,Es)
    {0x50514459,0x020cd490,KMemProtMe,0x020d08e8,false}, //YDQP: 5124 - Dragon Quest IX - Sentinels of the Starry Skies (Europe) (En,Fr,De,Es,It)
    {0x50543442,0x0201060c,KMemProtMe,0x020157a4,true }, //B4TP: 5188 - Tetris Party Deluxe (Europe) (En,Fr,De,Es,It)
    {0x50385342,0x02018288,KMemProtMe,0x02015f84,true }, //BS8P: 5189 - Sorcerer's Apprentice, The (Europe) (En,De,Es,It)
    //not compressed
    {0x45324842,0x020fa040,KMemProtMe,0x020f3690,true }, //BH2E: 5264 - Super Scribblenauts (USA) (En,Fr,Es,Pt)
    //not compressed
    {0x50324842,0x020fa4f4,KMemProtMe,0x020f3b44,true }, //BH2P: 5281 - Super Scribblenauts (Europe) (En,Fr,De,Es,It,Nl)
    {0x58423242,0x0203021c,KMemProtMe,0x02034170,false}, //B2BX: 5301 - Captain Tsubasa - New Kick Off (Europe) (Fr,De,Es,It)
    {0x4b413242,0x0200b444,KMemProtMe,0x0200e724,false}, //B2AK: 5459 - Aranuri (Korea)
    {0x4b593242,0x02009bd4,KMemProtMe,0x0200d1b8,false}, //B2YK: 5463 - Piyodamari DS (Korea)
    {0x4b4f5259,0x0200ada8,KMemProtMe,0x0200f778,true }  //YROK: 5466 - Princess in Wonderland (Korea)
  };
  static const SPatchInfo3 info6[]=
  {
    {0x4b504d59,0x020c8736,KMemProtMe,0x020ca8b1,false}  //YMPK: 4893 - MapleStory DS (Korea)
  };
  static const SPatchInfo3 info7[]=
  {
    {0x4b585942,0x0207cf32,KMemProtMe,0x02080019,false}, //BYXK: 4940 - Yu-Gi-Oh! 5D's - Stardust Accelerator - World Championship 2010 (Korea)
    {0x45395943,0x020383c2,KMemProtMe,0x02036a49,true }, //CY9E: 4969 - Club Penguin - Elite Penguin Force - Herbert's Revenge (USA) (En,Fr,De)
    {0x50395943,0x020383c2,KMemProtMe,0x02036a49,true }, //CY9P: 5114 - Club Penguin - EPF - Herbert's Revenge (Europe) (En,Fr)
    //not compressed
    {0x504c4642,0x020958ce,KMemProtMe,0x02093c19,true }, //BFLP: 5237 - MySims - SkyHeroes (Europe) (En,Fr,De,Es,It,Nl)
    {0x50483742,0x02090e6a,KMemProtMe,0x0208e969,true }, //B7HP: 5341 - Harry Potter and the Deathly Hallows - Part 1 (Europe) (En,Fr,De,Es,It)
    {0x45483742,0x02090bf2,KMemProtMe,0x0208e6f1,true }, //B7HE: 5429 - Harry Potter and The Deathly Hallows - Part 1 (USA) (En,Fr,Es)
    {0x4b4c5a41,0x020ae1de,KMemProtMe,0x020ab381,false}, //AZLK: 5464 - My Fashion - Girls Style (Korea)
    //not compressed
    {0x454c4642,0x020958ce,KMemProtMe,0x02093c19,true }  //BFLE: 5509 - MySims - SkyHeroes (USA)
  };
  static const SPatchInfo3 info8[]=
  {
    {0x4a584f42,0x0201727c,KMemProtMe,0x020145b8,true }, //BOXJ: 5027 - Love Plus Plus (Japan)
    {0x4a443342,0x0205f610,KMemProtMe,0x0205cacc,true }, //B3DJ: 5051 - Taiko no Tatsujin DS - Dororon! Yokai Daikessen!! (Japan)
    {0x4a5a4542,0x0200e450,KMemProtMe,0x02010c74,true }, //BEZJ: 5057 - Inazuma Eleven 3 - Sekai heno Chosen!! Bomber (Japan)
    {0x4a384542,0x0200e450,KMemProtMe,0x02010c74,true }, //BE8J: 5058 - Inazuma Eleven 3 - Sekai heno Chosen!! Spark (Japan)
    {0x4a594f42,0x020362fc,KMemProtMe,0x02033dcc,true }, //BOYJ: 5098 - Odoru Daisousasen The Game - Sensuikan ni Sennyuu Seyo! (Japan)
    {0x4a4a4242,0x020ae530,KMemProtMe,0x020ac0b8,true }, //BBJJ: 5110 - Metal Fight Beyblade - Bakugami Susanoh Shuurai! (Japan)
    {0x4a534b42,0x0208093c,KMemProtMe,0x02085594,true }, //BKSJ: 5116 - Keshikasu-kun Battle Castival (Japan)
    {0x4a584842,0x020b3438,KMemProtMe,0x020b817c,true }, //BHXJ: 5123 - Quiz! Hexagon II (Japan)
    {0x4a364842,0x020392fc,KMemProtMe,0x02036e44,true }, //BH6J: 5131 - TV Anime Fairy Tail - Gekitou! Madoushi Kessen (Japan)
    {0x4a523842,0x0200c2cc,KMemProtMe,0x02010ab8,true }, //B8RJ: 5135 - Kamen Rider Battle - GanbaRide - Card Battle Taisen (Japan)
    {0x4a595742,0x020a0e00,KMemProtMe,0x0209e378,true }, //BWYJ: 5143 - Wizardry - Boukyaku no Isan (Japan)
    {0x4a543442,0x020105f0,KMemProtMe,0x02015788,true }, //B4TJ: 5155 - Tetris Party Premium (Japan)
    {0x4a344242,0x0200aa78,KMemProtMe,0x0200f6d0,true }, //BB4J: 5157 - Battle Spirits Digital Starter (Japan)
    {0x4a465142,0x0200bd88,KMemProtMe,0x02010c30,true }, //BQFJ: 5164 - HeartCatch Pretty Cure! - Oshare Collection (Japan)
    {0x50555442,0x0203a664,KMemProtMe,0x02038178,true }, //BTUP: 5202 - Tinker Bell and the Great Fairy Rescue (Europe)
    {0x44554259,0x0200d784,KMemProtMe,0x020117a4,false}, //YBUD: 5232 - Blue Dragon - Awakened Shadow (Germany)
    {0x4a4f4f42,0x0200d11c,KMemProtMe,0x02013cd8,true }, //BOOJ: 5239 - Okamiden Chiisaki Taiyou (Japan)
    {0x58555442,0x02039c28,KMemProtMe,0x02037748,true }, //BTUX: 5247 - Tinker Bell and the Great Fairy Rescue (Europe) (En,Fr,De,Es,It,Nl)
    {0x50553342,0x0212fdec,KMemProtMe,0x0212d758,true }, //B3UP: 5251 - Petz - Playschool (Europe)
    {0x49554259,0x0200d784,KMemProtMe,0x020117a4,false}, //YBUI: 5252 - Blue Dragon - Awakened Shadow (Italy)
    {0x45523342,0x0207de68,KMemProtMe,0x02083c98,false}, //B3RE: 5253 - Pokemon Ranger - Guardian Signs (USA)
    {0x4a4b3542,0x0200af78,KMemProtMe,0x0200f948,true }, //B5KJ: 5266 - Tenkaichi Sengoku Lovers DS (Japan)
    //not compressed
    {0x4a4a5242,0x0200c098,KMemProtMe,0x020106b8,true }, //BRJJ: 5298 - Radiant Historia (Japan)
    {0x50523342,0x0207e020,KMemProtMe,0x02083e50,false}, //B3RP: 5306 - Pokemon Ranger - Guardian Signs (Europe) (En,Fr,De,Es,It)
    {0x46554259,0x0200d784,KMemProtMe,0x020117a4,false}, //YBUF: 5326 - Blue Dragon - Awakened Shadow (France)
    {0x50554242,0x020b4bf4,KMemProtMe,0x020b91fc,false}, //BBUP: 5342 - Beyblade Metal Fusion - Cyber Pegasus (Europe)
    {0x454b5342,0x020826e4,KMemProtMe,0x02085c50,false}, //BSKE: 5355 - Nine Hours, Nine Persons, Nine Doors (USA)
    {0x59554242,0x020b4c34,KMemProtMe,0x020b923c,false}, //BBUY: 5366 - Beyblade Metal Fusion (USA) (En,Fr,Es)
    {0x50563342,0x0210d6cc,KMemProtMe,0x0210b3dc,true }, //B3VP: 5399 - Vampire Moon - The Mystery of the Hidden Sun (Europe) (En,Fr,De,Es,It,Nl)
    {0x45553342,0x0212fd48,KMemProtMe,0x0212d6b4,true }, //B3UE: 5432 - Petz Nursery 2 (USA)
    {0x50354142,0x02025758,KMemProtMe,0x020234d4,true }, //BA5P: 5442 - Marie-Antoinette and the American War of Independence - Episode 1 - The Brotherhood of the Wolf (Europe) (En,Fr,De,Es,It)
    {0x4b514b42,0x02010620,KMemProtMe,0x0200e350,true }, //BKQK: 5465 - Pucca - Power Up (Korea)
    {0x4b483643,0x0200b1a8,KMemProtMe,0x0200f6e0,true }, //C6HK: 5467 - Tomicar - Hero Rescue Force DS (Korea)
    {0x4b534e43,0x0200a62c,KMemProtMe,0x0200efb4,true }, //CNSK: 5468 - Naruto Shippuden - Naruto vs Sasuke (Korea)
    {0x50554259,0x0200d784,KMemProtMe,0x020117a4,false}, //YBUP: 5511 - Blue Dragon - Awakened Shadow (Europe)
    {0x45563342,0x0210d27c,KMemProtMe,0x0210af8c,true }, //B3VE: 5528 - Vampire Moon - The Mystery of the Hidden Sun (USA)
    {0x53554259,0x0200d784,KMemProtMe,0x020117a4,false}  //YBUS: 5544 - Blue Dragon - Awakened Shadow (Spain)
  };
  static const SPatchInfo3 info9[]=
  {
    {0x4a394d42,0x0200711a,KMemProtMe,0x0200a301,true }, //BM9J: 5134 - Metal Max 3 (Japan)
    {0x4a4a4f42,0x0201b82a,KMemProtMe,0x02019bcd,true }  //BOJJ: 5197 - One Piece - Gigant Battle (Japan)
  };
  static const SPatchInfo3 info10[]=
  {
    {0x454a3343,0x02010638,KMemProtMe,0x0201471c,false}, //C3JE: 5200 - Professor Layton and the Unwound Future (USA)
    {0x4a394b42,0x0200baf8,KMemProtMe,0x02009a19,true }, //BK9J: 5255 - Kingdom Hearts - Re-coded (Japan)
    {0x45445342,0x0210776c,KMemProtMe,0x021054b0,true }, //BSDE: 5265 - Lufia - Curse of the Sinistrals (USA)
    {0x464a3343,0x02010638,KMemProtMe,0x0201471c,false}, //C3JF: 5277 - Professeur Layton et le Destin Perdu (France)
    {0x444a3343,0x02010638,KMemProtMe,0x0201471c,false}, //C3JD: 5278 - Professor Layton und die Verlorene Zukunft (Germany)
    {0x494a3343,0x02010638,KMemProtMe,0x0201471c,false}, //C3JI: 5284 - Professor Layton e il Futuro Perduto, Il (Italy)
    {0x4a323242,0x0200acf8,KMemProtMe,0x0200f754,true }, //B22J: 5285 - Strike Witches 2 - Iyasu Naosu Punipunisuru (Japan)
    {0x484a3343,0x02010638,KMemProtMe,0x0201471c,false}, //C3JH: 5287 - Professor Layton en de Verloren Toekomst (Netherlands)
    {0x534a3343,0x02010638,KMemProtMe,0x0201471c,false}, //C3JS: 5288 - Profesor Layton y el Futuro Perdido, El (Spain)
    {0x504a3343,0x02010638,KMemProtMe,0x0201471c,false}, //C3JP: 5294 - Professor Layton and the Lost Future (Europe)
    {0x50434a42,0x020fb03c,KMemProtMe,0x020f8148,true }, //BJCP: 5302 - Goldeneye 007 (Europe)
    {0x45434a42,0x020fb03c,KMemProtMe,0x020f8148,true }, //BJCE: 5299 - Goldeneye 007 (USA)
    {0x4a515a42,0x0200d128,KMemProtMe,0x02011e48,true }, //BZQJ: 5303 - Zac to Ombra - Maboroshi no Yuuenchi (Japan)
    {0x49434a42,0x020fb03c,KMemProtMe,0x020f8148,true }, //BJCI: 5309 - Goldeneye 007 (Italy)
    {0x50535842,0x020cb1c0,KMemProtMe,0x020c8d0c,true }, //BXSP: 5318 - Sonic Colors (Europe) (En,Ja,Fr,De,Es,It)
    {0x4a4b3742,0x02013bd0,KMemProtMe,0x02007cd8,true }, //B7KJ: 5322 - Tamagotchi no Narikiri Challenge (Japan)
    {0x50384a42,0x0200afe4,KMemProtMe,0x02025b08,true }, //BJ8P: 5323 - Cooking Mama World - Hobbies and Fun (Europe) (En,Fr,De,Es,It)
    {0x4a575242,0x02039c34,KMemProtMe,0x02037790,true }, //BRWJ: 5336 - Super Robot Taisen L (Japan)
    {0x45535842,0x020cb1c0,KMemProtMe,0x020c8d0c,true }, //BXSE: 5368 - Sonic Colors (USA)
    {0x4a394f42,0x02084b58,KMemProtMe,0x020826a4,true }, //BO9J: 5374 - Penguin no Mondai - The World (Japan)
    {0x504e5642,0x020526fc,KMemProtMe,0x02057258,true }, //BVNP: 5381 - Michael Jackson - The Experience (Europe) (En,Fr,De,Es,It)
    {0x4a495242,0x0200c134,KMemProtMe,0x02010b5c,true }, //BRIJ: 5384 - Nori Nori Relaxuma - Hit Song Ongakusai (Japan)
    {0x4a4a5142,0x0200c634,KMemProtMe,0x020114a0,true }, //BQJJ: 5385 - Kawaii Koneko DS 3 (Japan)
    {0x4a495642,0x02075bd8,KMemProtMe,0x0207a90c,true }, //BVIJ: 5388 - Do-Konjou Shougakussei - Bon Bita - Hadaka no Choujou Ketsusen!! Bita vs. Dokuro Dei! (Japan)
    {0x4a535842,0x020cb1bc,KMemProtMe,0x020c8d08,true }, //BXSJ: 5401 - Sonic Colors (Japan) (En,Ja,Fr,De,Es,It)
    {0x4a4b5542,0x0200b794,KMemProtMe,0x0201049c,true }, //BUKJ: 5404 - Rekishi Taisen Gettenka - Tenkaichi Battle Royale (Japan)
    {0x4a454f42,0x0200e394,KMemProtMe,0x02010c94,true }, //BOEJ: 5421 - Inazuma Eleven 3 - Sekai heno Chousen! The Ogre (Japan)
    {0x44434a42,0x020fb03c,KMemProtMe,0x020f8148,true }, //BJCD: 5428 - GoldenEye 007 (Germany)
    {0x45385142,0x0200afe4,KMemProtMe,0x02025b08,true }, //BQ8E: 5431 - Crafting Mama (USA)
    {0x4b344c43,0x02092940,KMemProtMe,0x0209590c,false}, //CL4K: 5462 - Zzang-Gu Rubber Clay (Korea)
    {0x46434a42,0x020fb03c,KMemProtMe,0x020f8148,true }, //BJCF: 5472 - GoldenEye 007 (France)
    {0x45394b42,0x0200bb78,KMemProtMe,0x02009a99,true }, //BK9E: 5473 - Kingdom Hearts - Re-coded (USA)
    {0x50394b42,0x0200bb8c,KMemProtMe,0x02009aad,true }, //BK9P: 5476 - Kingdom Hearts - Re-coded (Europe) (En,Fr,De,Es,It)
    {0x45544742,0x0206cb90,KMemProtMe,0x0206a8c8,true }, //BGTE: 5478 - Ghost Trick - Phantom Detective (USA) (En,Fr,De,Es,It)
    {0x4a425a42,0x02059c1c,KMemProtMe,0x020579dc,true }, //BZBJ: 5501 - Zombie Daisuki (Japan)
    {0x44454559,0x0200cdb4,KMemProtMe,0x02011aa0,true }, //YEED: 5516 - Inazuma Eleven (Germany)
    {0x46454559,0x0200cdb4,KMemProtMe,0x02011aa0,true }, //YEEF: 5522 - Inazuma Eleven (France)
    {0x454e5642,0x020526b4,KMemProtMe,0x02057210,true }, //BVNE: 5529 - Michael Jackson - The Experience (USA) (En,Fr,Es)
    {0x49454559,0x0200cdb4,KMemProtMe,0x02011aa0,true }, //YEEI: 5533 - Inazuma Eleven (Italy)
    {0x53454559,0x0200cdb4,KMemProtMe,0x02011aa0,true }, //YEES: 5534 - Inazuma Eleven (Spain)
    {0x50454559,0x0200cdb4,KMemProtMe,0x02011aa0,true }, //YEEP: 5535 - Inazuma Eleven (Europe)
    {0x50544742,0x0206cb78,KMemProtMe,0x0206a8b0,true }, //BGTP: 5537 - Ghost Trick - Phantom Detective (Europe) (En,Fr,De,Es,It)
    {0x45534e43,0x0200a550,KMemProtMe,0x0200efb4,true }, //CNSE: Naruto Shippuden - Naruto vs Sasuke (USA)
    {0x454a5242,0x0200bfbc,KMemProtMe,0x020106b8,true }  //BRJE: Radiant Historia (USA)
  };
  static const SPatchInfo3 info11[]=
  {
    {0x4a354f42,0x0204b868,KMemProtMe,0x02049654,true }, //BO5J: 5292 - Ougon no Taiyou - Shikkokunaru Yoake (Japan)
    {0x45354f42,0x0204ba64,KMemProtMe,0x02049850,true }, //BO5E: 5367 - Golden Sun - Dark Dawn (USA)
    {0x4a473342,0x0200c9e4,KMemProtMe,0x02011464,true }, //B3GJ: 5370 - SD Gundam Sangokuden - Brave Battle Warriors (Japan)
    {0x4a435542,0x020dd7e4,KMemProtMe,0x020e20ec,true }, //BUCJ: 5375 - Crayon Shin-Chan Shokkugan! Densetsu o Yobu Omake Daiketsusen!! (Japan)
    {0x4a525742,0x0209f318,KMemProtMe,0x0209cba0,true }, //BWRJ: 5376 - Momotarou Dentetsu World (Japan)
    {0x4a5a5242,0x020300fc,KMemProtMe,0x0202dd08,true }, //BRZJ: 5382 - Metal Fight Bayblade - Choujou Kessen! Big Bang Bladers (Japan)
    {0x50354f42,0x0204bb24,KMemProtMe,0x02049910,true }, //BO5P: 5387 - Golden Sun - Dark Dawn (Europe) (En,Fr,De,Es,It)
    {0x4a4b3242,0x0200d968,KMemProtMe,0x0201688c,true }, //B2KJ: 5400 - Ninokuni - Shikkoku no Madoushi (Japan)
    {0x4a565942,0x0209499c,KMemProtMe,0x02091e20,true }, //BYVJ: 5500 - Kaiju Busters Powered (Japan)
    {0x4a4c3342,0x0202a1d4,KMemProtMe,0x02027770,true }, //B3LJ: 5508 - Eigo de Tabisutu - Little Charo (Japan)
    {0x4b4a4342,0x0205f4c4,KMemProtMe,0x0205ca68,true }, //BCJK: 5523 - Mabeop Cheonjamun DS2 - Choehuui hanjamabeop (Korea)
    {0x4a4c4654,0x0200a844,KMemProtMe,0x0200f3bc,true }, //TFLJ: 5530 - Sora no Otoshimono Forte - Dreamy Season (Japan)
    {0x4a424454,0x020bbb4c,KMemProtMe,0x020b967c,true }, //TDBJ: 5531 - Dragon Ball Kai - Ultimate Butouden (Japan)
    {0x4a4d5a42,0x020091dc,KMemProtMe,0x0200719c,true }, //BZMJ: 5549 - KORG M01 Music Workstation (Japan) (En)
    {0x4a475054,0x02009c7c,KMemProtMe,0x0200e7e4,true }  //TPGJ: 5562 - Dengeki Gakuen RPG - Cross of Venus Special (Japan)
  };
  static const SPatchInfo3 info12[]=
  {
    {0x50594442,0x02153896,KMemProtMe,0x0215194d,true }, //BDYP: 5314 - Call of Duty - Black Ops (Europe)
    {0x44594442,0x02153892,KMemProtMe,0x02151949,true }, //BDYD: 5327 - Call of Duty - Black Ops (Germany)
    {0x45594442,0x02153892,KMemProtMe,0x02151949,true }, //BDYE: 5360 - Call of Duty - Black Ops (USA)
    {0x4a463542,0x0209c696,KMemProtMe,0x0209fd79,true }, //B5FJ: 5402 - Fuurai no Shiren 5 - Fortun Tower to Unmei no Dice (Japan)
    {0x46594442,0x02153b76,KMemProtMe,0x02151c2d,true }, //BDYF: 5407 - Call of Duty - Black Ops (France)
    {0x45495659,0x0200ad02,KMemProtMe,0x0200852d,false}  //YVIE: 5560 - Dragon Quest VI - Realms of Revelation (USA) (En,Fr,Es)
  };
  static const SPatchInfo3 info13[]=
  {
    {0x4a493342,0x0201e95e,KMemProtMe,0x02032285,true }, //B3IJ: 5377 - Motto! Stitch! DS Rhythm de Rakugaki Daisakusen (Japan)
    //not compressed
    {0x4a335a42,0x02019522,KMemProtMe,0x0201b631,true }, //BZ3J: 5450 - SaGa 3 - Jikuu no Hasha - Shadow or Light (Japan)
    {0x4a4f5842,0x02026b5e,KMemProtMe,0x02025425,true }  //BXOJ: 5532 - Gyakuten Kenji 2 (Japan)
  };
  static const SPatchInfo3 info14[]=
  {
    {0x50543642,0x020b559c,KMemProtMe,0x02010694,true }  //B6TP: 5424 - Disney Tangled - The Video Game (Europe) (En,Fr,De,Es,It,Nl)
  };
  for(u32 ii=0;ii<sizeof(info)/sizeof(info[0]);++ii)
  {
    if(info[ii].iGameCode==GameCode())
    {
      NeedUncompressed();
      PatchOverlay9(info[ii].iPatch,info[ii].iLoad,220);
      switch(GameCode())
      {
        case 0x4a473542: //B5GJ: 4692 - Shin Sengoku Tenkatouitsu - Gunyuu Tachi no Souran (Japan)
        case 0x4a335143: //CQ3J: 4860 - Sengoku Spirits - Moushouden (Japan)
        case 0x4a345259: //YR4J: 4861 - Sengoku Spirits - Shukunden (Japan)
        case 0x4a325143: //CQ2J: 5068 - Sengoku Spirits - Gunshiden (Japan)
        case 0x4b554242: //BBUK: 5122 - Metal Beyblade - Cyber Pegasis (Korea)
          FixProtectionRegion2();
          break;
      }
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info2)/sizeof(info2[0]);++ii)
  {
    if(info2[ii].iGameCode==GameCode())
    {
      NeedUncompressed();
      PatchOverlay10(info2[ii].iPatch1,info2[ii].iPatch2,info2[ii].iPatch3,info2[ii].iLoad,info2[ii].iCode,info2[ii].iRead,info2[ii].iNew,info2[ii].iBack);
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info3)/sizeof(info3[0]);++ii)
  {
    if(info3[ii].iGameCode==GameCode())
    {
      NeedUncompressed();
      PatchOverlay11(info3[ii].iPatch,info3[ii].iLoad,info3[ii].iRead,info3[ii].iNew);
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info4)/sizeof(info4[0]);++ii)
  {
    if(info4[ii].iGameCode==GameCode())
    {
      FixPatch01(unprot_009_bin,unprot_009_bin_size,EFix01);
      NeedUncompressed();
      PatchOverlay9(info4[ii].iPatch,info4[ii].iLoad,220);
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info5)/sizeof(info5[0]);++ii)
  {
    if(info5[ii].iGameCode==GameCode())
    {
      NeedUncompressed();
      PatchOverlay12(info5[ii].iPatch,info5[ii].iLoad,info5[ii].iRead,220,info5[ii].iNew);
      switch(GameCodeAll())
      {
        case 0x00324842: //BH2*: Super Scribblenauts
          FixProtectionRegion2();
          break;
      }
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info6)/sizeof(info6[0]);++ii)
  {
    if(info6[ii].iGameCode==GameCode())
    {
      FixPatch01(unprot_011_bin,unprot_011_bin_size,EFix01);
      NeedUncompressed();
      PatchOverlay11(info6[ii].iPatch,info6[ii].iLoad,info6[ii].iRead,info6[ii].iNew);
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info7)/sizeof(info7[0]);++ii)
  {
    if(info7[ii].iGameCode==GameCode())
    {
      FixPatch01(unprot_011_bin,unprot_011_bin_size,EFix02);
      NeedUncompressed();
      PatchOverlay11(info7[ii].iPatch,info7[ii].iLoad,info7[ii].iRead,info7[ii].iNew);
      switch(GameCodeAll())
      {
        case 0x004c4642: //BFL*: 5237 - MySims - SkyHeroes
          FixProtectionRegion2();
          break;
      }
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info8)/sizeof(info8[0]);++ii)
  {
    if(info8[ii].iGameCode==GameCode())
    {
      FixPatch02(unprot_012_bin,unprot_012_bin_size,EFix01);
      NeedUncompressed();
      PatchOverlay12(info8[ii].iPatch,info8[ii].iLoad,info8[ii].iRead,220,info8[ii].iNew);
      switch(GameCode())
      {
        case 0x4a4a5242: //BRJJ: 5298 - Radiant Historia (Japan)
          FixProtectionRegion2();
          break;
      }
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info9)/sizeof(info9[0]);++ii)
  {
    if(info9[ii].iGameCode==GameCode())
    {
      FixPatch02(unprot_011_bin,unprot_011_bin_size,EFix02);
      NeedUncompressed();
      PatchOverlay11(info9[ii].iPatch,info9[ii].iLoad,info9[ii].iRead,info9[ii].iNew);
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info10)/sizeof(info10[0]);++ii)
  {
    if(info10[ii].iGameCode==GameCode())
    {
      NeedUncompressed();
      PatchOverlay13(info10[ii].iPatch,info10[ii].iLoad,info10[ii].iRead,info10[ii].iNew);
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info11)/sizeof(info11[0]);++ii)
  {
    if(info11[ii].iGameCode==GameCode())
    {
      NeedUncompressed();
      PatchOverlay15(info11[ii].iPatch,info11[ii].iLoad,info11[ii].iRead,info11[ii].iNew);
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info12)/sizeof(info12[0]);++ii)
  {
    if(info12[ii].iGameCode==GameCode())
    {
      NeedUncompressed();
      PatchOverlay16(info12[ii].iPatch,info12[ii].iLoad,info12[ii].iRead,info12[ii].iNew);
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info13)/sizeof(info13[0]);++ii)
  {
    if(info13[ii].iGameCode==GameCode())
    {
      NeedUncompressed();
      PatchOverlay17(info13[ii].iPatch,info13[ii].iLoad,info13[ii].iRead,info13[ii].iNew);
      switch(GameCodeAll())
      {
        case 0x00335a42: //BZ3*: SaGa 3 - Jikuu no Hasha - Shadow or Light
          FixProtectionRegion2();
          break;
      }
      return;
    }
  }
  for(u32 ii=0;ii<sizeof(info14)/sizeof(info14[0]);++ii)
  {
    if(info14[ii].iGameCode==GameCode())
    {
      NeedUncompressed();
      PatchOverlay18(info14[ii].iPatch,info14[ii].iLoad,info14[ii].iRead,info14[ii].iNew);
      return;
    }
  }
}

void CDsPatcher::PatchMandM(bool anEuro)
{
  //0xe1500000 cmp r0,r0
  static const u32 info1[]=
  {
    0x020047dc,
    0x020050d0,
    0x0200635c,
    0x02007184,
    0x020074fc,
    0x020077d0,
    0x0200803c,
    0x0200859c,
    0x02008e60,
    0x02009404,
    0x020095f4,
    0x02009840,
    0x0200a5f4,
    0x0200a6fc,
    0x0200a898,
    0x0200afb8,
    0x0200b530,
    0x0200b73c,
    0x0200cf98,
    0x0200d0e8,
    0x0200d304,
    0x0200d440,
    0x0200d758,
    0x0200d8fc,
    0x0200dacc,
    0x0200dc14,
    0x0200ddf8,
    0x0200e3a0,
    0x0200e77c,
    0x0200ea5c,
    0x0200ecbc,
    0x0200ee44,
    0x0200efac,
    0x02010dcc,
    0x02010fb4,
    0x02011438,
    0x020115a8,
    0x020116f0,
    0x0201188c,
    0x02011ddc,
    0x020153a8,
    0x02015518,
    0x0201592c,
    0x02015ad4,
    0x02015e7c,
    0x02018590,
    0x02018f78,
    0x020191a0,
    0x02019d10,
    0x02019e88,
    0x0201ba9c,
    0x0201bbd8,
    0x0201bd14,
    0x0201c760,
    0x0201c9a4,
    0x0201caa4,
    0x0201d9e0,
    0x0201db8c,
    0x0201df00,
    0x0201e47c,
    0x0201e8e8,
    0x0201eaf4,
    0x0201f41c,
    0x02020e24,
    0x02021228,
    0x02021578,
    0x02021a04,
    0x02021b64,
    0x02021c68,
    0x020220b8,
    0x02022600,
    0x020227fc,
    0x02023730,
    0x02023868,
    0x02023a0c,
    0x02023b7c,
    0x02023cc4,
    0x0202436c,
    0x020245b0,
    0x02025944,
    0x02026860,
    0x02026998,
    0x0202702c,
    0x02029e20,
    0x0202aa4c,
    0x0202af70,
    0x0202b13c,
    0x0202b584,
    0x0202bc04,
    0x0202bf54,
    0x0202c05c,
    0x0202c98c,
    0x0202cbdc,
    0x0202ccf0,
    0x0202d0a0,
    0x0202d1a8,
    0x0202d4c0,
    0x0202dab0,
    0x0202fd00,
    0x020301cc,
    0x02030800,
    0x02031fb0,
    0x020321fc,
    0x02032c34,
    0x020335f8,
    0x02033ab4,
    0x02034430,
    0x0203475c,
    0x0203556c,
    0x02035c4c,
    0x020360c0,
    0x020371ac,
    0x02037320,
    0x02038db4,
    0x020399a8,
    0x0203a954,
    0x0203ab88,
    0x0203aff4,
    0x0203b148,
    0x0203b6b8,
    0x0203b8e8,
    0x0203c0cc,
    0x0203c1e8,
    0x0203c2e0,
    0x0203c4f0,
    0x0203c6d8,
    0x0203c830,
    0x0203ca68,
    0x0203cbd8,
    0x0203cd58,
    0x0203d094,
    0x0203d20c,
    0x0203d448,
    0x0203eb2c,
    0x0203ec70,
    0x0203edb4,
    0x0203efb8,
    0x0203fabc,
    0x0203ff24,
    0x02040090,
    0x0204085c,
    0x02040dc0,
    0x0204127c,
    0x02041524,
    0x020416bc,
    0x02041810,
    0x02041bb4,
    0x02041fa8,
    0x0204286c,
    0x020429e8,
    0x020453d8,
    0x02045610,
    0x02045c88,
    0x02045f44,
    0x0204637c,
    0x02047898,
    0x02047b28,
    0x020481f4,
    0x0204851c,
    0x02048740,
    0x020488e0,
    0x02049b64,
    0x0204b978,
    0x0204bd3c,
    0x0204be58,
    0x0204c944,
    0x0204e194,
    0x0204ef94,
    0x0204fd7c,
    0x020502f8,
    0x02050b44,
    0x02050c94,
    0x020516a4,
    0x02051800,
    0x02051f58,
    0x02052498,
    0x02052d80,
    0x02053b74,
    0x0205406c,
    0x02055718,
    0x02055940,
    0x02055a54,
    0x02055df8,
    0x02055fc0,
    0x020562b4,
    0x020564c4,
    0x02056618,
    0x0205708c,
    0x020571ec,
    0x02057774,
    0x02057ad0,
    0x02057be0,
    0x02057e24,
    0x02057f38,
    0x020582b4,
    0x020585c8,
    0x02058a48,
    0x0205a130,
    0x0205a618,
    0x0205aaf4,
    0x0205be48,
    0x0205bfa4,
    0x0205e4a8,
    0x0205e9fc,
    0x02060208,
    0x02060a34,
    0x02060c60,
    0x020616f8,
    0x020640ec,
    0x02064548,
    0x02065dc8,
    0x02067830,
    0x0206af58,
    0x0206b180,
    0x0206bf30,
    0x0206c86c,
    0x0206cdfc,
    0x0206d08c,
    0x0206d5c8,
    0x0206dac4,
    0x0206dc10,
    0x0206dd7c,
    0x0206e1bc,
    0x0206e534,
    0x0206f700,
    0x0206f8b0,
    0x02070174,
    0x02070d84,
    0x020713ec,
    0x020718e0,
    0x02071cc0,
    0x020725e0,
    0x020755ec,
    0x02075c74,
    0x02075e04,
    0x02075f54,
    0x02076708,
    0x0207687c,
    0x020769b8,
    0x02076acc,
    0x02078090,
    0x02079100,
    0x02079738,
    0x02079960,
    0x02079ac0,
    0x02079be8,
    0x02079cf4,
    0x02079e48,
    0x02079f80,
    0x0207a31c,
    0x0207a4c8,
    0x0207c53c,
    0x0207c7ec,
    0x0207da18,
    0x0207dc68,
    0x0207df70,
    0x0207e1b0,
    0x0207e580,
    0x0207e830,
    0x0207eb7c,
    0x0207ecdc,
    0x0207edf4,
    0x0207fc64,
    0x02080308,
    0x02082f08,
    0x02083038,
    0x02083e88,
    0x0208408c,
    0x020841e8,
    0x020843a0,
    0x02084808,
    0x02084d9c,
    0x02084f38,
    0x02085064,
    0x02085254,
    0x02085408,
    0x02085690,
    0x020866d0,
    0x02087358,
    0x02087554,
    0x02087800,
    0x020879b8,
    0x02087b00,
    0x02087c3c,
    0x02087d44,
    0x02087e78,
    0x02088734,
    0x02088918,
    0x02088fa4,
    0x020893dc,
    0x02089570,
    0x0208a328,
    0x0208add0,
    0x0208af98,
    0x0208b160,
    0x0208c4cc,
    0x0208d240,
    0x0208d56c,
    0x0208db98,
    0x0208dd80,
    0x020909a8,
    0x02090be0,
    0x02091138,
    0x02091424,
    0x020918cc,
    0x02092918,
    0x020931d0,
    0x0209341c,
    0x0209370c,
    0x02093a0c,
    0x02096464,
    0x02096574,
    0x02096754,
    0x02096b28,
    0x02096d74,
    0x02096eac,
    0x020971ac,
    0x0209acfc,
    0x0209ae1c,
    0x0209af88,
    0x0209bd10,
    0x0209be7c,
    0x0209c1fc,
    0x0209c32c,
    0x0209c438,
    0x0209c558,
    0x0209c680,
    0x0209d6b0,
    0x020a1098,
    0x020a123c,
    0x020a1460,
    0x020a173c,
    0x020a1ba0,
    0x020a1ce0,
    0x020a1f74,
    0x020a2148,
    0x020a2344,
    0x020a34f4,
    0x020a448c,
    0x020a4740,
    0x020a4dc0,
    0x020a4f50,
    0x020a50c4,
    0x020a51cc,
    0x020a5a30,
    0x020a5ba8,
    0x020a5e3c,
    0x020a5f50,
    0x020a6534,
    0x020a6d40,
    0x020a74e8,
    0x020a75f4,
    0x020a7710,
    0x020a8344,
    0x020a8764,
    0x020a89bc,
    0x020a8b5c,
    0x020a955c,
    0x020a96cc
  };
  static const u8 info1s[]=
  {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x12,0x12,0x12,
    0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
    0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
    0x12,0x12,0x12,0x12,0x12,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,
    0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,
    0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,
    0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x2f,0x2f,0x2f,0x2f,0x2f,0x2f,
    0x2f,0x3b,0x3b,0x3b,0x3b,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,
    0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,
    0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,
    0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,0x4b,
    0x4b,0x4d,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x1f,
    0x1c,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,
    0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,
    0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,
    0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,
    0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,
    0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f
  };
  for(size_t ii=0;ii<(sizeof(info1)/sizeof(info1[0]));++ii)
  {
    *(u32*)(info1[ii]+(anEuro?(info1s[ii]<<2):0))=0xe1500000; //cmp r0,r0
  }
}

void CDsPatcher::PatchCoG2(bool anEuro)
{
  u32 shift=anEuro?0:1;
  static const s32 info[][2]=
  {
    {0x02008a3a,0x02008912},
    {0x0200e1dc,0x0200e0b4},
    {0x0200e2e6,0x0200e1be},
    {0x0200eef8,0x0200edd0},
    {0x0200efc8,0x0200eea0},
    {0x0200f852,0x0200f72a},
    {0x0200f9e0,0x0200f8b8},
    {0x0200fac2,0x0200f99a},
    {0x0200fba8,0x0200fa80},
    {0x0200fd8c,0x0200fc64},
    {0x0200fece,0x0200fda6},
    {0x020107fe,0x020106d6},
    {0x02013ffc,0x02013eb4},
    {0x02016ed0,0x02016d88},
    {0x02016fe2,0x02016e9a},
    {0x02017250,0x02017108},
    {0x0201742e,0x020172e6},
    {0x0201b700,0x0201b5b8},
    {0x0201c352,0x0201c20a},
    {0x0201cd98,0x0201cc50},
    {0x0201ff7c,0x0201fe34},
    {0x0202013c,0x0201fff4},
    {0x02020446,0x020202fe},
    {0x02020a7e,0x02020936},
    {0x02020fa4,0x02020e5c},
    {0x02027620,0x020274d8},
    {0x0202782e,0x020276e6},
    {0x02028782,0x0202863a},
    {0x0202ca9e,0x0202c956},
    {0x0202cf48,0x0202ce00},
    {0x0202d08e,0x0202cf46},
    {0x0202d154,0x0202d00c},
    {0x0202ff2c,0x0202fde4},
    {0x02030090,0x0202ff48},
    {0x020301ea,0x020300a2},
    {0x020345fe,0x020344b6},
    {0x020356ec,0x020355a4},
    {0x020357a6,0x0203565e},
    {0x020358b8,0x02035770},
    {0x02035a1a,0x020358d2},
    {0x020364d6,0x0203638e},
    {0x020365c4,0x0203647c},
    {0x02036878,0x02036730},
    {0x02036f6c,0x02036e24},
    {0x02037048,0x02036f00},
    {0x02037558,0x02037410},
    {0x02037bba,0x02037a72},
    {0x02039636,0x020394ee},
    {0x02039768,0x02039620},
    {0x02041210,0x020410c8},
    {0x020416d8,0x02041590},
    {0x02042fb8,0x02042e70},
    {0x02043104,0x02042fbc},
    {0x0204435e,0x02044216},
    {0x02044746,0x020445fe},
    {0x02047550,0x02047408},
    {0x0204834c,0x02048204},
    {0x0204962c,0x020494e4},
    {0x02049884,0x0204973c},
    {0x0204a61c,0x0204a4d4},
    {0x0204a84c,0x0204a704},
    {0x0204ecdc,0x0204eb94},
    {0x0205227e,0x02052136},
    {0x02054d7a,0x02054c32},
    {0x02054f62,0x02054e1a},
    {0x02056358,0x02056210},
    {0x02056e24,0x02056cdc},
    {0x02089f14,0x02089dcc},
    {0x0208a1e8,0x0208a0a0},
    {0x0208fb5e,0x0208fa16},
    {0x02092cf6,0x02092bae},
    {0x02096950,0x02096808},
    {0x0209858a,0x02098442},
    {0x0209d1e0,0x0209d098},
    {0x020a9268,0x020a9120},
    {0x020b25c4,0x020b247c},
    {0x020b6f30,0x020b6de8},
    {0x020b7c1c,0x020b7ad4},
    {0x020bc7d8,0x020bc690},
    {0x020bcc14,0x020bcacc},
    {0x020bd6b6,0x020bd56e}
  };
  for(size_t ii=0;ii<sizeof(info)/sizeof(info[0]);++ii)
  {
    PatchMem(KArm9,-info[ii][shift],0x4280); //cmp r0, r0
  }
}

void CDsPatcher::PatchCoG3(bool anEuro)
{
  static const s32 info[]=
  {
    0x02000e0c,
    0x020021c8,
    0x020029d6,
    0x02004b5c,
    0x02004f6c,
    0x020051aa,
    0x02007942,
    0x0200bb50,
    0x0200bcee,
    0x0200be0e,
    0x0200c134,
    0x0200cbd8,
    0x0200d032,
    0x0200d41e,
    0x0200d776,
    0x0200d890,
    0x0200d960,
    0x0200da3e,
    0x0200db4c,
    0x02011190,
    0x0201147e,
    0x0201172e,
    0x02011f06,
    0x020133f2,
    0x0201366c,
    0x02018e7e,
    0x02019fd6,
    0x0201aff6,
    0x0201b0f0,
    0x0201b462,
    0x0201b57c,
    0x0201b6bc,
    0x0201ba2e,
    0x0201bb44,
    0x0201bce8,
    0x0201bdd2,
    0x0201c140,
    0x0201c602,
    0x0201c6c6,
    0x0201c826,
    0x0201cd12,
    0x0201ce28,
    0x0201cef4,
    0x0201d1f4,
    0x0201d3e0,
    0x0201d844,
    0x0201ddd2,
    0x0201eee6,
    0x0201f264,
    0x0201f4a8,
    0x0201f59c,
    0x02020184,
    0x0202031e,
    0x020215bc,
    0x02021792,
    0x02021a8c,
    0x02021bbc,
    0x02022c22,
    0x02022f08,
    0x02024fd0,
    0x02025c72,
    0x02025f9e,
    0x02026154,
    0x02026226,
    0x0202630a,
    0x02026fde,
    0x020273e0,
    0x02027c2a,
    0x02027eca,
    0x02028498,
    0x020286c4,
    0x02028784,
    0x02028928,
    0x020289f6,
    0x02028bf2,
    0x0202a05e,
    0x0202b28a,
    0x0202b36e,
    0x0202b88c,
    0x0202b97a,
    0x0202badc,
    0x0202d0a6,
    0x0202d1c6,
    0x0202d3ce,
    0x0202d4da,
    0x0202d686,
    0x0202d78a,
    0x0202d866,
    0x0202d9d8,
    0x0202dd22,
    0x0202e45c,
    0x0202ee16,
    0x0202eff0,
    0x0202f0b2,
    0x0202f436,
    0x0202f5b4,
    0x0202f716,
    0x0202f926,
    0x0202f9e8,
    0x02030de0,
    0x0203241c,
    0x02032d36,
    0x02032e50,
    0x0203300a,
    0x020334d0,
    0x0203377c,
    0x02033880,
    0x02033944,
    0x02033a2e,
    0x02033caa,
    0x020346dc,
    0x02034812,
    0x0203518a,
    0x020352ac,
    0x0203568e,
    0x02037334,
    0x02037c70,
    0x020387bc,
    0x020388c4,
    0x02038ff0,
    0x020390f8,
    0x0203945c,
    0x02039596,
    0x02039d30,
    0x02039e74,
    0x0203ba2c,
    0x0203bf16,
    0x0203df82,
    0x0203e23c,
    0x0203e390,
    0x0203eb7c,
    0x0203fbea,
    0x0203fd5e,
    0x020413c6,
    0x020416aa,
    0x020419d0,
    0x02042c36,
    0x02044dba,
    0x020464aa,
    0x02046c74,
    0x02046dcc,
    0x02046f0a,
    0x02047226,
    0x0204734a,
    0x02048230,
    0x02048c58,
    0x02048f78,
    0x0204906c,
    0x02049130,
    0x020499fc,
    0x0204b4aa,
    0x0204c058,
    0x0204caac,
    0x02077262,
    0x02077494,
    0x02077616,
    0x02078e42,
    0x02078fbc,
    0x0207a524,
    0x0207c170,
    0x0207c364,
    0x0207c428,
    0x0207c524,
    0x0207c5e4,
    0x0207c6a4,
    0x0207cba2,
    0x0207cc8a,
    0x0207cd84,
    0x0207d74a,
    0x0207eb5a,
    0x020818de,
    0x0208325a,
    0x020838ae,
    0x02095cbe,
    0x0209616e,
    0x02096382,
    0x0209650a,
    0x020a9968,
    0x020aaa20,
    0x020aaba8
  };
  static const s8 info_s[]=
  {
    0x00,0x00,0x00,0xfe,0xfe,0xfe,0xfe,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,
    0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,0x3e,0x39,0x3e,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,
    0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,
    0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,
    0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,
    0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,
    0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,
    0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,
    0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,
    0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,0x3c,
    0x58,0x58,0x58,0x58,0x58,0x58,0x58,0x58,0x58,0x58,0x58,0x58,0x58,0x58,0x58,0x58,
    0x58,0x58,0x58,0x58
  };
  for(size_t ii=0;ii<sizeof(info)/sizeof(info[0]);++ii)
  {
    PatchMem(KArm9,-(info[ii]-(anEuro?0:(info_s[ii]<<1))),0x4280); //cmp r0, r0
  }
  PatchMem(KArm9,-(0x0207d8ea-(anEuro?0:0xb0)),0x2500); //mov r5, #0
  PatchMem(KArm9,-(0x0204ecba-(anEuro?0:0x78)),0x46c0); //nop
}

void CDsPatcher::PatchCOP(void)
{
  s32 data[]=
  {
    0x02022284,
    0x020224c8,
    0x02022540,
    0x020225ac,
    0x02022620,
    0x02022690,
    0x0202270c,
    0x02022770,
    0x02023774,
    0x0202389c,
    0x02023b6c,
    0x02023bd8,
    0x02023c4c,
    0x02023cbc,
    0x02023d38,
    0x02023d9c,
    0x02023e18,
    0x02024424,
    0x02025f48,
    0x02026408,
    0x02026480,
    0x020278e8,
    0x02028468,
    0x02028a5c,
    0x02028b94,
    0x02028c44,
    0x02028d00,
    0x02028da4,
    0x02028e5c,
    0x0202c4fc,
    0x0202c578,
    0x0202c60c,
    0x0202c688,
    0x0202c7d8,
    0x0202cd60,
    0x0202d3cc,
    0x0202d920,
    0x0202da80,
    0x0202dafc,
    0x0202e2c8,
    0x0202e5cc,
    0x0202e638,
    0x0202e78c,
    0x0202eb98,
    0x0202ece4,
    0x0202ed54,
    0x0202f42c,
    0x0202f9f8,
    0x02030128,
    0x02030194,
    0x020304f8,
    0x02030564,
    0x020305d8,
    0x02030aa4,
    0x02030cf0,
    0x02030d9c,
    0x02030e18,
    0x02031234,
    0x020312ac,
    0x02031348,
    0x020313bc,
    0x0203145c,
    0x02031520,
    0x02031584,
    0x02031600,
    0x020316e8,
    0x020318b8,
    0x02031974,
    0x020319e8,
    0x02031af8,
    0x02031b74,
    0x02031bd8,
    0x02031c8c,
    0x02031d30,
    0x02031da8,
    0x02031f80,
    0x02031ff4,
    0x02032114,
    0x02032190,
    0x02032318,
    0x020323b8,
    0x02032514,
    0x0203258c,
    0x02032764,
    0x02032810,
    0x0203288c,
    0x020329b4,
    0x020330ec,
    0x02033168,
    0x020332f0,
    0x0203ba64,
    0x0203c6ec,
    0x0203c760,
    0x0203d540,
    0x0203ebb0,
    0x0203ec14,
    0x0203f4cc,
    0x0203f538,
    0x0203fa44,
    0x0203fab0,
    0x0203ffbc,
    0x02040a84,
    0x02040b00,
    0x02041664,
    0x020416e0,
    0x02041b24,
    0x02041b9c,
    0x020423f8,
    0x02042870,
    0x020428e0,
    0x02042ea0,
    0x02044128,
    0x020441a4,
    0x020447b8,
    0x02048e10,
    0x02049764,
    0x020497d8,
    0x02049848,
    0x0204a1a0,
    0x0204a204,
    0x0205b5c4,
    0x0205b630,
    0x0205f030,
    0x0205f09c,
    0x0205f110,
    0x0208c5ec,
    0x0208c668
  };
  for(size_t ii=0;ii<(sizeof(data)/sizeof(data[0]));++ii)
  {
    PatchMem(KArm9,data[ii],0xe1500000); //cmp r0,r0
  }
  PatchMem(KArm9,0x020657fc,2); //end of list
}
