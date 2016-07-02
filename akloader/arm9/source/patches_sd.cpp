/*
    patches_sd.cpp
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
#include "save_nand_bin.h"
#include "sd_save_bin.h"
#include "unprot_001_bin.h"

#define CHECK_VALUE(a,b)
#define COMPARE_VALUE(a,b) (a==*(u32*)b)

void CPatcher::PatchSDSave(void)
{
  //patch "3690 - Made in Ore (Japan)" & "4812 - WarioWare D.I.Y. (USA)" & "4913 - WarioWare - Do It Yourself (Europe) (En,Fr,De,Es,It)"
  if(0x4a524f55==GameCode()||0x45524f55==GameCode()||0x50524f55==GameCode()) //UORJ&UORE&UORP
  {
    u32 sdPatchEntry=0x2002be4;
    if(0x45524f55==GameCode()) sdPatchEntry=0x2002c04;
    else if(0x50524f55==GameCode()) sdPatchEntry=0x2002ca4;
    memcpy((void*)sdPatchEntry,save_nand_bin,save_nand_bin_size);

    u32 patchEntry=sdPatchEntry+0x50c;           //u32 gNandInit(void* data)
    PatchMem(KArm9,patchEntry+0,0xe3a00001);     //mov r0, #1
    PatchMem(KArm9,patchEntry+4,0xe12fff1e);     //bx lr

    patchEntry=sdPatchEntry+0x958;               //u32 gNandWrite(void* memory,void* flash,u32 size,u32 dma_channel)
    PatchMem(KArm9,patchEntry+0,0xe3a03001);     //mov r3, #1
    PatchMem(KArm9,patchEntry+4,0xe51ff004);     //ldr pc, [pc, #-4]
    PatchMem(KArm9,patchEntry+8,sdPatchEntry+1);

    patchEntry=sdPatchEntry+0xc9c;               //void gNandWait(void)
    PatchMem(KArm9,patchEntry+0,0xe12fff1e);     //bx lr

    patchEntry=sdPatchEntry+0xd24;               //u32 gNandRead(void* memory,void* flash,u32 size,u32 dma_channel)
    PatchMem(KArm9,patchEntry+0,0xe3a03000);     //mov r3, #0
    PatchMem(KArm9,patchEntry+4,0xe51ff004);     //ldr pc, [pc, #-4]
    PatchMem(KArm9,patchEntry+8,sdPatchEntry+1);

    //patchEntry=sdPatchEntry+0xe84;             //void gNandGetData(void* data)

    patchEntry=sdPatchEntry+0xeb0;               //u32 gNandState(void)
    PatchMem(KArm9,patchEntry+0,0xe3a00003);     //mov r0, #3
    PatchMem(KArm9,patchEntry+4,0xe12fff1e);     //bx lr

    patchEntry=sdPatchEntry+0xec8;               //u32 gNandError(void)
    PatchMem(KArm9,patchEntry+0,0xe3a00000);     //mov r0, #0
    PatchMem(KArm9,patchEntry+4,0xe12fff1e);     //bx lr
  }
  else if(0x50425855==GameCode()) //"4950 - Jam with the Band (Europe) (En,Fr,De,Es,It)"
  {
    NeedUncompressed();
    u32 sdPatchEntry=0x2000c60;
    memcpy((void*)sdPatchEntry,save_nand_bin,save_nand_bin_size);

    u32 patchEntry=0x020613cc;                   //u32 gNandInit(void)
    PatchMem(KArm9,patchEntry+0,0xe3a00001);     //mov r0, #1
    PatchMem(KArm9,patchEntry+4,0xe12fff1e);     //bx lr

    patchEntry=0x0206176c;                       //u32 gNandWrite(void* memory,void* flash,u32 size,u32 dma_channel)
    PatchMem(KArm9,patchEntry+0,0xe3a03001);     //mov r3, #1
    PatchMem(KArm9,patchEntry+4,0xe51ff004);     //ldr pc, [pc, #-4]
    PatchMem(KArm9,patchEntry+8,sdPatchEntry+1);

    patchEntry=0x02061a4c;                       //u32 gNandResume(void)
    PatchMem(KArm9,patchEntry+0,0xe3a00000);     //mov r0, #0
    PatchMem(KArm9,patchEntry+4,0xe12fff1e);     //bx lr

    patchEntry=0x02061ac4;                       //u32 gNandRead(void* memory,void* flash,u32 size,u32 dma_channel)
    PatchMem(KArm9,patchEntry+0,0xe3a03000);     //mov r3, #0
    PatchMem(KArm9,patchEntry+4,0xe51ff004);     //ldr pc, [pc, #-4]
    PatchMem(KArm9,patchEntry+8,sdPatchEntry+1);

    patchEntry=0x02061c24;                       //u32 gNandError(void)
    PatchMem(KArm9,patchEntry+0,0xe3a00000);     //mov r0, #0
    PatchMem(KArm9,patchEntry+4,0xe12fff1e);     //bx lr
  }
  else
  {
    if(COMPARE_VALUE(0x33382e32,0x023a7108)||COMPARE_VALUE(0x33382e32,0x023a70fc)) //2.83
    {
      CHECK_VALUE(0x0238a948,0xe92d41f0);
      CHECK_VALUE(0x0238b5f0,0xe593000c); //6
      CHECK_VALUE(0x0238b62c,0xe5930010); //9
      CHECK_VALUE(0x0238b67c,0xe593000c); //14
      //0x0238a948 - 0x0238b1b8 - 2160 bytes
      PatchSDSave0(0x0238a948,(u32*)0x0238b5cc,0xe1a00003); //mov r0, r3
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a79d0)||COMPARE_VALUE(0x33382e32,0x023a79c4)) //2.83
    {
      CHECK_VALUE(0x02387b00,0xe92d41f0);
      CHECK_VALUE(0x023887a8,0xe593000c); //6
      CHECK_VALUE(0x023887e4,0xe5930010); //9
      CHECK_VALUE(0x02388834,0xe593000c); //14
      //0x02387b00 - 0x02387d1c - 2160 bytes
      PatchSDSave0(0x02387b00,(u32*)0x02388784,0xe1a00003); //mov r0, r3
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a8b5c)||COMPARE_VALUE(0x33382e32,0x023a8990)||COMPARE_VALUE(0x33382e32,0x023a8994)||COMPARE_VALUE(0x33382e32,0x023a8bf4)||COMPARE_VALUE(0x33382e32,0x023a8aec)||COMPARE_VALUE(0x33382e32,0x023a8d4c)||COMPARE_VALUE(0x33382e32,0x023a8dbc)) //2.83
    {
      CHECK_VALUE(0x0238afac,0xe92d41f0);
      CHECK_VALUE(0x0238bc54,0xe593000c); //6
      CHECK_VALUE(0x0238bc90,0xe5930010); //9
      CHECK_VALUE(0x0238bce0,0xe593000c); //14
      //0x0238afac - 0x0238b1c8 - 2160 bytes
      PatchSDSave0(0x0238afac,(u32*)0x0238bc30,0xe1a00003); //mov r0, r3
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a8bd0)||COMPARE_VALUE(0x33382e32,0x023a8e3c)||COMPARE_VALUE(0x33382e32,0x023a8e30)||COMPARE_VALUE(0x33382e32,0x023a8bdc)) //2.83
    {
      CHECK_VALUE(0x0238b020,0xe92d41f0);
      CHECK_VALUE(0x0238bcc8,0xe593000c); //6
      CHECK_VALUE(0x0238bd04,0xe5930010); //9
      CHECK_VALUE(0x0238bd54,0xe593000c); //14
      //0x0238b020 - 0x0238b890 - 2160 bytes
      PatchSDSave0(0x0238b020,(u32*)0x0238bca4,0xe1a00003); //mov r0, r3
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a6f00)||COMPARE_VALUE(0x33382e32,0x023a70c8)||COMPARE_VALUE(0x33382e32,0x023a7058)||COMPARE_VALUE(0x33382e32,0x023a72f0)) //2.83
    {
      CHECK_VALUE(0x0238a914,0xe92d41f0);
      CHECK_VALUE(0x0238b5bc,0xe593000c);
      CHECK_VALUE(0x0238b5f8,0xe5930010);
      CHECK_VALUE(0x0238b648,0xe593000c);
      //0x0238a914 - 0x0238b184 - 2160 bytes
      PatchSDSave0(0x0238a914,(u32*)0x0238b598,0xe1a00003); //mov r0, r3
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a77c8)||COMPARE_VALUE(0x33382e32,0x023a7920)||COMPARE_VALUE(0x33382e32,0x023a7990)) //2.83
    {
      CHECK_VALUE(0x02387acc,0xe92d41f0);
      CHECK_VALUE(0x02388774,0xe593000c); //6
      CHECK_VALUE(0x023887b0,0xe5930010); //9
      CHECK_VALUE(0x02388800,0xe593000c); //14
      //0x02387acc - 0x02387ce8 - 2160 bytes
      PatchSDSave0(0x02387acc,(u32*)0x02388750,0xe1a00003); //mov r0, r3
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a6d90)||COMPARE_VALUE(0x33382e32,0x023a6db0)) //2.83
    {
      CHECK_VALUE(0x0238ab14,0xe59f1058);
      CHECK_VALUE(0x0238b514,0xe5921010); //6
      CHECK_VALUE(0x0238b55c,0xe592100c); //9
      CHECK_VALUE(0x0238b570,0xe5910010); //11
      //0x0238ab14 - 0x0238b0e8 - 1492 bytes
      PatchSDSave1(0x0238ab14,(u32*)0x0238b4e4,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a697c)||COMPARE_VALUE(0x33382e32,0x023a698c)) //2.83
    {
      CHECK_VALUE(0x0238a99c,0xe59f1058);
      CHECK_VALUE(0x0238b39c,0xe5921010); //6
      CHECK_VALUE(0x0238b3e4,0xe592100c); //9
      CHECK_VALUE(0x0238b3f8,0xe5910010); //11
      //0x0238a99c - 0x0238af70 - 1492 bytes
      PatchSDSave1(0x0238a99c,(u32*)0x0238b36c,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a4980)||COMPARE_VALUE(0x33382e32,0x023a4824)||COMPARE_VALUE(0x33382e32,0x023a490c)) //2.83
    {
      CHECK_VALUE(0x023894b8,0x480ab508);
      CHECK_VALUE(0x02389d18,0x69526911); //6
      CHECK_VALUE(0x02389d3c,0x695268d1); //9
      CHECK_VALUE(0x02389d6c,0x780068d0); //14
      //0x023894b8 - 0x02389a0c - 1364 bytes
      PatchSDSave3(0x023894b8,0x02389d00,0x00001c10); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a49c0)||COMPARE_VALUE(0x33382e32,0x023a49b4)) //2.83
    {
      CHECK_VALUE(0x023894ec,0x480ab508);
      CHECK_VALUE(0x02389d4c,0x69526911); //6
      CHECK_VALUE(0x02389d70,0x695268d1); //9
      CHECK_VALUE(0x02389da0,0x780068d0); //14
      //0x023894ec - 0x02389a40 - 1364 bytes
      PatchSDSave3(0x023894ec,0x02389d34,0x00001c10); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38372e32,0x023a6920)||COMPARE_VALUE(0x38372e32,0x023a6914)) //2.78
    {
      CHECK_VALUE(0x0238a9c4,0xe59f1058);
      CHECK_VALUE(0x0238b3c4,0xe5921010); //6
      CHECK_VALUE(0x0238b40c,0xe592100c); //9
      CHECK_VALUE(0x0238b424,0xe5911014); //11
      //0x0238a9c4 - 0x0238af98 - 1492 bytes
      PatchSDSave1(0x0238a9c4,(u32*)0x0238b394,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a840c)) //2.83
    {
      CHECK_VALUE(0x0238bb50,0xe59f1058);
      CHECK_VALUE(0x0238c550,0xe5921010); //6
      CHECK_VALUE(0x0238c598,0xe592100c); //9
      CHECK_VALUE(0x0238c5b0,0xe5911014); //11
      //0x0238bb50 - 0x0238c124 - 1492 bytes
      PatchSDSave1(0x0238bb50,(u32*)0x0238c520,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a7224)) //2.83
    {
      CHECK_VALUE(0x02387b34,0xe59f1058);
      CHECK_VALUE(0x02388534,0xe5921010); //6
      CHECK_VALUE(0x0238857c,0xe592100c); //9
      CHECK_VALUE(0x02388594,0xe5911014); //11
      //0x02387b34 - 0x02388108 - 1492 bytes
      PatchSDSave1(0x02387b34,(u32*)0x02388504,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a8a7c)||COMPARE_VALUE(0x33382e32,0x023a881c)||COMPARE_VALUE(0x33382e32,0x023a8840)||COMPARE_VALUE(0x33382e32,0x023a8aa0)) //2.83
    {
      CHECK_VALUE(0x0238b1ac,0xe59f1058);
      CHECK_VALUE(0x0238bbac,0xe5921010); //6
      CHECK_VALUE(0x0238bbf4,0xe592100c); //9
      CHECK_VALUE(0x0238bc0c,0xe5911014); //11
      //0x0238b1ac - 0x0238b780 - 1492 bytes
      PatchSDSave1(0x0238b1ac,(u32*)0x0238bb7c,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38372e32,0x023a8450)) //2.78
    {
      CHECK_VALUE(0x0238bb90,0xe59f1058);
      CHECK_VALUE(0x0238c590,0xe5921010); //6
      CHECK_VALUE(0x0238c5d8,0xe592100c); //9
      CHECK_VALUE(0x0238c5f0,0xe5911014); //11
      //0x0238bb90 - 0x0238c164 - 1492 bytes
      PatchSDSave1(0x0238bb90,(u32*)0x0238c560,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a7658)||COMPARE_VALUE(0x33382e32,0x023a7678)) //2.83
    {
      CHECK_VALUE(0x02387ccc,0xe59f1058);
      CHECK_VALUE(0x023886cc,0xe5921010); //6
      CHECK_VALUE(0x02388714,0xe592100c); //9
      CHECK_VALUE(0x0238872c,0xe5911014); //11
      //0x02387ccc - 0x023882a0 - 1492 bytes
      PatchSDSave1(0x02387ccc,(u32*)0x0238869c,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a6ea4)) //2.83
    {
      CHECK_VALUE(0x0238ac24,0xe59f1058);
      CHECK_VALUE(0x0238b624,0xe5921010); //6
      CHECK_VALUE(0x0238b66c,0xe592100c); //9
      CHECK_VALUE(0x0238b684,0xe5911014); //11
      //0x0238ac24 - 0x0238b1f8 - 1492 bytes
      PatchSDSave1(0x0238ac24,(u32*)0x0238b5f4,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38372e32,0x023a8998)) //2.78
    {
      CHECK_VALUE(0x0238b5f4,0xe92d47f0);
      CHECK_VALUE(0x0238c204,0xe5921010); //6
      CHECK_VALUE(0x0238c24c,0xe592100c); //9
      CHECK_VALUE(0x0238c260,0xe5910010); //11
      //0x0238b5f4 - 0x0238bc20 - 1580 bytes
      PatchSDSave1(0x0238b5f4,(u32*)0x0238c1d8,0xe1a00002,11); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38372e32,0x023aa2d8)) //2.78
    {
      CHECK_VALUE(0x0238c644,0xe92d47f0);
      CHECK_VALUE(0x0238d254,0xe5921010); //6
      CHECK_VALUE(0x0238d29c,0xe592100c); //9
      CHECK_VALUE(0x0238d2b0,0xe5910010); //11
      //0x0238c644 - 0x0238cc70 - 1580 bytes
      PatchSDSave1(0x0238c644,(u32*)0x0238d228,0xe1a00002,11); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38372e32,0x023a92e8)) //2.78
    {
      CHECK_VALUE(0x023882c0,0xe92d47f0);
      CHECK_VALUE(0x02388ed0,0xe5921010); //6
      CHECK_VALUE(0x02388f18,0xe592100c); //9
      CHECK_VALUE(0x02388f2c,0xe5910010); //11
      //0x023882c0 - 0x023888ec - 1580 bytes
      PatchSDSave1(0x023882c0,(u32*)0x02388ea4,0xe1a00002,11); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x34372e32,0x023a9fc8)||COMPARE_VALUE(0x34372e32,0x023a9fb8)) //2.74
    {
      CHECK_VALUE(0x0238c538,0xe92d47f0);
      CHECK_VALUE(0x0238d144,0xe5921010); //6
      CHECK_VALUE(0x0238d18c,0xe592100c); //9
      CHECK_VALUE(0x0238d1a0,0xe5910010); //11
      //0x0238c538 - 0x0238cb64 - 1580 bytes
      PatchSDSave1(0x0238c538,(u32*)0x0238d118,0xe1a00002,11); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x34362e32,0x023a8970)||COMPARE_VALUE(0x35362e32,0x023a89c4)) //2.64/2.65
    {
      CHECK_VALUE(0x023881e0,0xe92d4010);
      CHECK_VALUE(0x02388fc0,0xe5921010); //6
      CHECK_VALUE(0x02389008,0xe592100c); //9
      CHECK_VALUE(0x0238901c,0xe5900010); //11
      //0x023881e0 - 0x02388a50 - 2160 bytes
      PatchSDSave2(0x023881e0,(u32*)0x02388f88,0xe1a00002,14); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x34362e32,0x023a9d58)) //2.64
    {
      CHECK_VALUE(0x0238c328,0xe92d4010);
      CHECK_VALUE(0x0238d108,0xe5921010); //6
      CHECK_VALUE(0x0238d150,0xe592100c); //9
      CHECK_VALUE(0x0238d164,0xe5900010); //11
      //0x0238c328 - 0x0238cb98 - 2160 bytes
      PatchSDSave2(0x0238c328,(u32*)0x0238d0d0,0xe1a00002,14); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31362e32,0x023a7fb4)||COMPARE_VALUE(0x35362e32,0x023a8068)) //2.61/2.65
    {
      CHECK_VALUE(0x0238b284,0xe92d4010);
      CHECK_VALUE(0x0238c064,0xe5921010); //6
      CHECK_VALUE(0x0238c0ac,0xe592100c); //9
      CHECK_VALUE(0x0238c0c0,0xe5900010); //11
      //0x0238b284 - 0x0238baf4 - 2160 bytes
      PatchSDSave2(0x0238b284,(u32*)0x0238c02c,0xe1a00002,14); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38382e32,0x023aa2d8)) //2.88
    {
      CHECK_VALUE(0x0238f7c0,0xe92d41f0);
      CHECK_VALUE(0x0239025c,0xe5921010); //6
      CHECK_VALUE(0x023902a4,0xe592100c); //9
      CHECK_VALUE(0x023902fc,0xe590000c); //14
      //0x0238f7c0 - 0x02390030 - 2160 bytes
      PatchSDSave4(0x0238f7c0,(u32*)0x0239022c,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a9ea8)) //2.83
    {
      CHECK_VALUE(0x0238f588,0xe92d41f0);
      CHECK_VALUE(0x02390024,0xe5921010); //6
      CHECK_VALUE(0x0239006c,0xe592100c); //9
      CHECK_VALUE(0x023900c4,0xe590000c); //14
      //0x0238f588 - 0x0238fdf8 - 2160 bytes
      PatchSDSave4(0x0238f588,(u32*)0x0238fff4,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38382e32,0x023ac1b0)) //2.88
    {
      CHECK_VALUE(0x023902c8,0xe92d41f0);
      CHECK_VALUE(0x02390d64,0xe5921010); //6
      CHECK_VALUE(0x02390dac,0xe592100c); //9
      CHECK_VALUE(0x02390e04,0xe590000c); //14
      //0x023902c8 - 0x02390b38 - 2160 bytes
      PatchSDSave4(0x023902c8,(u32*)0x02390d34,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38382e32,0x023aa2ac)) //2.88
    {
      CHECK_VALUE(0x0238f7a4,0xe92d41f0);
      CHECK_VALUE(0x02390240,0xe5921010); //6
      CHECK_VALUE(0x02390288,0xe592100c); //9
      CHECK_VALUE(0x023902e0,0xe590000c); //14
      //0x0238f7a4 - 0x02390014 - 2160 bytes
      PatchSDSave4(0x0238f7a4,(u32*)0x02390210,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38382e32,0x023a7604)) //2.88
    {
      CHECK_VALUE(0x0238eacc,0xe92d41f0);
      CHECK_VALUE(0x0238f504,0xe5921010); //6
      CHECK_VALUE(0x0238f54c,0xe592100c); //9
      CHECK_VALUE(0x0238f5a4,0xe590000c); //14
      //0x0238eacc - 0x0238f33c - 2160 bytes
      PatchSDSave4(0x0238eacc,(u32*)0x0238f4d4,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38382e32,0x023a75f0)) //2.88
    {
      CHECK_VALUE(0x0238eaac,0xe92d41f0);
      CHECK_VALUE(0x0238f4e4,0xe5921010); //6
      CHECK_VALUE(0x0238f52c,0xe592100c); //9
      CHECK_VALUE(0x0238f584,0xe590000c); //14
      //0x0238eaac - 0x0238f31c - 2160 bytes
      PatchSDSave4(0x0238eaac,(u32*)0x0238f4b4,0xe1a00002,12); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38382e32,0x023a8f44)) //2.88
    {
      CHECK_VALUE(0x0238efd4,0xe92d41f0);
      CHECK_VALUE(0x0238fb18,0xe5921010); //6
      CHECK_VALUE(0x0238fb60,0xe592100c); //9
      CHECK_VALUE(0x0238fbb8,0xe590000c); //14
      //0x0238efd4 - 0x0238f914 - 2368 bytes
      PatchSDSave4(0x0238efd4,(u32*)0x238faf4,0xe1a00002,9); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38382e32,0x023a6348)) //2.88
    {
      CHECK_VALUE(0x0238e1c8,0xe92d41f0);
      CHECK_VALUE(0x0238ecac,0xe5921010); //6
      CHECK_VALUE(0x0238ecf4,0xe592100c); //9
      CHECK_VALUE(0x0238ed4c,0xe590000c); //14
      //0x0238e1c8 - 0x0238eb08 - 2368 bytes
      PatchSDSave4(0x0238e1c8,(u32*)0x0238ec88,0xe1a00002,9); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38362e32,0x023a843c)) //2.68
    {
      CHECK_VALUE(0x0238b4b8,0xe92d47f0);
      CHECK_VALUE(0x0238c2e4,0xe5921010); //6
      CHECK_VALUE(0x0238c32c,0xe592100c); //9
      CHECK_VALUE(0x0238c340,0xe5900010); //11
      //0x0238b4b8 - 0x0238ba58 - 1440 bytes
      PatchSDSave2(0x0238b4b8,(u32*)0x0238c2ac,0xe1a00002,14); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x37342e32,0x023a6efc)) //2.47
    {
      CHECK_VALUE(0x0238afc8,0xe92d4010);
      CHECK_VALUE(0x0238bb78,0xe5900010); //11
      CHECK_VALUE(0x0238bb90,0xe592100c); //9
      CHECK_VALUE(0x0238bbe4,0xe5921010); //6
      //0x0238afc8 - 0x0238b6ac - 1764 bytes
      PatchSDSave5(0x0238afc8,(u32*)0x0238bb80,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x39332e32,0x023a662c)) //2.39
    {
      CHECK_VALUE(0x0238adac,0xe92d4010);
      CHECK_VALUE(0x0238b95c,0xe5900010); //11
      CHECK_VALUE(0x0238b974,0xe592100c); //9
      CHECK_VALUE(0x0238b9c8,0xe5921010); //6
      //0x0238adac - 0x0238b490 - 1764 bytes
      PatchSDSave5(0x0238adac,(u32*)0x0238b964,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x37342e32,0x023a6f00)||COMPARE_VALUE(0x37342e32,0x023a7244)) //2.47
    {
      CHECK_VALUE(0x0238afcc,0xe92d4010);
      CHECK_VALUE(0x0238bb7c,0xe5900010); //11
      CHECK_VALUE(0x0238bb94,0xe592100c); //9
      CHECK_VALUE(0x0238bbe8,0xe5921010); //6
      //0x0238afcc - 0x0238b6b0 - 1764 bytes
      PatchSDSave5(0x0238afcc,(u32*)0x0238bb84,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x37342e32,0x023a8c18)) //2.47
    {
      CHECK_VALUE(0x0238c054,0xe92d4010);
      CHECK_VALUE(0x0238cc04,0xe5900010); //11
      CHECK_VALUE(0x0238cc1c,0xe592100c); //9
      CHECK_VALUE(0x0238cc70,0xe5921010); //6
      //0x0238c054 - 0x0238c738 - 1764 bytes
      PatchSDSave5(0x0238c054,(u32*)0x0238cc0c,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x37332e32,0x023a6024)||COMPARE_VALUE(0x37332e32,0x023a605c)||COMPARE_VALUE(0x37332e32,0x023a62d8)||COMPARE_VALUE(0x39332e32,0x023a6484)||COMPARE_VALUE(0x39332e32,0x023a64ac)) //2.37/2.39
    {
      CHECK_VALUE(0x0238ab98,0xe92d4010);
      CHECK_VALUE(0x0238b748,0xe5900010); //11
      CHECK_VALUE(0x0238b760,0xe592100c); //9
      CHECK_VALUE(0x0238b7b4,0xe5921010); //6
      //0x0238ab98 - 0x0238b27c - 1764 bytes
      PatchSDSave5(0x0238ab98,(u32*)0x0238b750,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x39332e32,0x023a82cc)) //2.39
    {
      CHECK_VALUE(0x0238be04,0xe92d4010);
      CHECK_VALUE(0x0238c9b4,0xe5900010); //11
      CHECK_VALUE(0x0238c9cc,0xe592100c); //9
      CHECK_VALUE(0x0238ca20,0xe5921010); //6
      //0x0238be04 - 0x0238c4e8 - 1764 bytes
      PatchSDSave5(0x0238be04,(u32*)0x0238c9bc,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x37342e32,0x023a8c14)) //2.47
    {
      CHECK_VALUE(0x0238c050,0xe92d4010);
      CHECK_VALUE(0x0238cc00,0xe5900010); //11
      CHECK_VALUE(0x0238cc18,0xe592100c); //9
      CHECK_VALUE(0x0238cc6c,0xe5921010); //6
      //0x0238c050 - 0x0238c734 - 1764 bytes
      PatchSDSave5(0x0238c050,(u32*)0x0238cc08,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x37342e32,0x023a6ee0)) //2.47
    {
      CHECK_VALUE(0x0238afac,0xe92d4010);
      CHECK_VALUE(0x0238bb5c,0xe5900010); //11
      CHECK_VALUE(0x0238bb74,0xe592100c); //9
      CHECK_VALUE(0x0238bbc8,0xe5921010); //6
      //0x0238afac - 0x0238b690 - 1764 bytes
      PatchSDSave5(0x0238afac,(u32*)0x0238bb64,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x37342e32,0x023a7858)) //2.47
    {
      CHECK_VALUE(0x02388208,0xe92d4010);
      CHECK_VALUE(0x02388db8,0xe5900010); //11
      CHECK_VALUE(0x02388dd0,0xe592100c); //9
      CHECK_VALUE(0x02388e24,0xe5921010); //6
      //0x02388208 - 0x023888ec - 1764 bytes
      PatchSDSave5(0x02388208,(u32*)0x02388dc0,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x39332e32,0x023a8124)) //2.39
    {
      CHECK_VALUE(0x0238bbf0,0xe92d4010);
      CHECK_VALUE(0x0238c7a0,0xe5900010); //11
      CHECK_VALUE(0x0238c7b8,0xe592100c); //9
      CHECK_VALUE(0x0238c80c,0xe5921010); //6
      //0x0238bbf0 - 0x0238c2d4 - 1764 bytes
      PatchSDSave5(0x0238bbf0,(u32*)0x0238c7a8,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x39332e32,0x023a72c4)) //2.39
    {
      CHECK_VALUE(0x02387f4c,0xe92d4010);
      CHECK_VALUE(0x02388afc,0xe5900010); //11
      CHECK_VALUE(0x02388b14,0xe592100c); //9
      CHECK_VALUE(0x02388b68,0xe5921010); //6
      //0x02387f4c - 0x02388630 - 1764 bytes
      PatchSDSave5(0x02387f4c,(u32*)0x02388b04,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a4720)||COMPARE_VALUE(0x33382e32,0x023a4700)) //2.83
    {
      CHECK_VALUE(0x02389470,0x480cb508);
      CHECK_VALUE(0x02389c9c,0x691168d0); //6
      CHECK_VALUE(0x02389cc8,0x695268d1); //9
      CHECK_VALUE(0x02389cd4,0x69496908); //11
      //0x02389470 - 0x023899f8 - 1416 bytes
      PatchSDSave6(0x02389470,0x02389c80,0x00001c10,ESave6Type83); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a4324)) //2.83
    {
      CHECK_VALUE(0x023892f8,0x480cb508);
      CHECK_VALUE(0x02389b24,0x691168d0); //6
      CHECK_VALUE(0x02389b50,0x695268d1); //9
      CHECK_VALUE(0x02389b5c,0x69496908); //11
      //0x023892f8 - 0x02389880 - 1416 bytes
      PatchSDSave6(0x023892f8,0x02389b08,0x00001c10,ESave6Type83); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a3fd8)) //2.83
    {
      CHECK_VALUE(0x02386484,0x480cb508);
      CHECK_VALUE(0x02386cb0,0x691168d0); //6
      CHECK_VALUE(0x02386cdc,0x695268d1); //9
      CHECK_VALUE(0x02386ce8,0x69496908); //11
      //0x02386484 - 0x02386a0c - 1416 bytes
      PatchSDSave6(0x02386484,0x02386c94,0x00001c10,ESave6Type83); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x33382e32,0x023a43d4)) //2.83
    {
      CHECK_VALUE(0x0238661c,0x480cb508);
      CHECK_VALUE(0x02386e48,0x691168d0); //6
      CHECK_VALUE(0x02386e74,0x695268d1); //9
      CHECK_VALUE(0x02386e80,0x69496908); //11
      //0x0238661c - 0x02386ba4 - 1416 bytes
      PatchSDSave6(0x0238661c,0x02386e2c,0x00001c10,ESave6Type83); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x34372e32,0x023a8688)||COMPARE_VALUE(0x34372e32,0x023a8678)) //2.74
    {
      CHECK_VALUE(0x0238b4e8,0xe92d47f0);
      CHECK_VALUE(0x0238c0f4,0xe5921010); //6
      CHECK_VALUE(0x0238c13c,0xe592100c); //9
      CHECK_VALUE(0x0238c150,0xe5910010); //11
      //0x0238b4e8 - 0x0238bb14 - 1580 bytes
      PatchSDSave1(0x0238b4e8,(u32*)0x0238c0c8,0xe1a00002,11); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31372e32,0x023a8678)||COMPARE_VALUE(0x31372e32,0x023a8674)||COMPARE_VALUE(0x31372e32,0x023a89bc)||COMPARE_VALUE(0x31372e32,0x023a8688)) //2.71
    {
      CHECK_VALUE(0x0238b464,0xe92d47f0);
      CHECK_VALUE(0x0238bfcc,0xe5921010); //6
      CHECK_VALUE(0x0238c014,0xe592100c); //9
      CHECK_VALUE(0x0238c028,0xe5900010); //11
      //0x0238b464 - 0x0238ba04 - 1440 bytes
      PatchSDSave2(0x0238b464,(u32*)0x0238bfa0,0xe1a00002,11); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x38362e32,0x023a865c)||COMPARE_VALUE(0x38362e32,0x023a863c)) //2.68
    {
      CHECK_VALUE(0x0238b460,0xe92d47f0);
      CHECK_VALUE(0x0238bfc8,0xe5921010); //6
      CHECK_VALUE(0x0238c010,0xe592100c); //9
      CHECK_VALUE(0x0238c024,0xe5900010); //11
      //0x0238b460 - 0x0238ba00 - 1440 bytes
      PatchSDSave2(0x0238b460,(u32*)0x0238bf9c,0xe1a00002,11); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31372e32,0x023a8fc8)) //2.71
    {
      CHECK_VALUE(0x02388210,0xe92d47f0);
      CHECK_VALUE(0x02388d78,0xe5921010); //6
      CHECK_VALUE(0x02388dc0,0xe592100c); //9
      CHECK_VALUE(0x02388dd4,0xe5900010); //11
      //0x02388210 - 0x023887b0 - 1440 bytes
      PatchSDSave2(0x02388210,(u32*)0x02388d4c,0xe1a00002,11); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31372e32,0x023a9fa0)) //2.71
    {
      CHECK_VALUE(0x0238c4b0,0xe92d47f0);
      CHECK_VALUE(0x0238d018,0xe5921010); //6
      CHECK_VALUE(0x0238d060,0xe592100c); //9
      CHECK_VALUE(0x0238d074,0xe5900010); //11
      //0x0238c4b0 - 0x0238ca50 - 1440 bytes
      PatchSDSave2(0x0238c4b0,(u32*)0x0238cfec,0xe1a00002,11); //mov r0, r2
    }
    //asphalt (u)
    else if(COMPARE_VALUE(0x31312e32,0x023a5f7c)) //2.11
    {
      CHECK_VALUE(0x0238ab70,0xe92d4010);
      CHECK_VALUE(0x0238b79c,0xe5900010); //11
      CHECK_VALUE(0x0238b7b4,0xe592100c); //9
      CHECK_VALUE(0x0238b808,0xe5921010); //6
      //0x0238ab70 - 0x0238b2a4 - 1844 bytes
      PatchSDSave5(0x0238ab70,(u32*)0x0238b7a4,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x35302e32,0x023a646c)) //2.05
    {
      CHECK_VALUE(0x0238ad00,0xe92d4010);
      CHECK_VALUE(0x0238b92c,0xe5900010); //11
      CHECK_VALUE(0x0238b944,0xe592100c); //9
      CHECK_VALUE(0x0238b998,0xe5921010); //6
      //0x0238ad00 - 0x0238b434 - 1844 bytes
      PatchSDSave5(0x0238ad00,(u32*)0x0238b934,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31312e32,0x023a6278)) //2.11
    {
      CHECK_VALUE(0x0238adf0,0xe92d4010);
      CHECK_VALUE(0x0238ba1c,0xe5900010); //11
      CHECK_VALUE(0x0238ba34,0xe592100c); //9
      CHECK_VALUE(0x0238ba88,0xe5921010); //6
      //0x0238adf0 - 0x0238b524 - 1844 bytes
      PatchSDSave5(0x0238adf0,(u32*)0x0238ba24,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31312e32,0x023a7728)) //2.11
    {
      CHECK_VALUE(0x0238bb10,0xe92d4010);
      CHECK_VALUE(0x0238c73c,0xe5900010); //11
      CHECK_VALUE(0x0238c754,0xe592100c); //9
      CHECK_VALUE(0x0238c7a8,0xe5921010); //6
      //0x0238bb10 - 0x0238c244 - 1844 bytes
      PatchSDSave5(0x0238bb10,(u32*)0x0238c744,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31312e32,0x023a6174)) //2.11
    {
      CHECK_VALUE(0x0238ad5c,0xe92d4010);
      CHECK_VALUE(0x0238b988,0xe5900010); //11
      CHECK_VALUE(0x0238b9a0,0xe592100c); //9
      CHECK_VALUE(0x0238b9f4,0xe5921010); //6
      //0x0238ad5c - 0x0238b490 - 1844 bytes
      PatchSDSave5(0x0238ad5c,(u32*)0x0238b990,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31312e32,0x023a779c)) //2.11
    {
      CHECK_VALUE(0x0238bb98,0xe92d4010);
      CHECK_VALUE(0x0238c7c4,0xe5900010); //11
      CHECK_VALUE(0x0238c7dc,0xe592100c); //9
      CHECK_VALUE(0x0238c830,0xe5921010); //6
      //0x0238bb98 - 0x0238c2cc - 1844 bytes
      PatchSDSave5(0x0238bb98,(u32*)0x0238c7cc,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31312e32,0x023a6158)) //2.11
    {
      CHECK_VALUE(0x0238ad48,0xe92d4010);
      CHECK_VALUE(0x0238b974,0xe5900010); //11
      CHECK_VALUE(0x0238b98c,0xe592100c); //9
      CHECK_VALUE(0x0238b9e0,0xe5921010); //6
      //0x0238ad48 - 0x0238b47c - 1844 bytes
      PatchSDSave5(0x0238ad48,(u32*)0x0238b97c,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x35302e32,0x023a7a18)) //2.05
    {
      CHECK_VALUE(0x0238ba2c,0xe92d4010);
      CHECK_VALUE(0x0238c658,0xe5900010); //11
      CHECK_VALUE(0x0238c670,0xe592100c); //9
      CHECK_VALUE(0x0238c6c4,0xe5921010); //6
      //0x0238ba2c - 0x0238c160 - 1844 bytes
      PatchSDSave5(0x0238ba2c,(u32*)0x0238c660,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31312e32,0x023a74a4)) //2.11
    {
      CHECK_VALUE(0x0238b89c,0xe92d4010);
      CHECK_VALUE(0x0238c4c8,0xe5900010); //11
      CHECK_VALUE(0x0238c4e0,0xe592100c); //9
      CHECK_VALUE(0x0238c534,0xe5921010); //6
      //0x0238b89c - 0x0238bfd0 - 1844 bytes
      PatchSDSave5(0x0238b89c,(u32*)0x0238c4d0,0xe1a00002,true); //mov r0, r2
    }
    //nfs:u2 (u)
    else if(COMPARE_VALUE(0x31312e32,0x023a7224)||COMPARE_VALUE(0x31312e32,0x023a71fc)||COMPARE_VALUE(0x31312e32,0x023a71f0)) //2.11
    {
      CHECK_VALUE(0x0238b130,0xe92d4010);
      CHECK_VALUE(0x0238bd74,0xe5900010); //11
      CHECK_VALUE(0x0238bd8c,0xe592100c); //9
      CHECK_VALUE(0x0238bde0,0xe5921010); //6
      //0x0238b130 - 0x0238b864 - 1844 bytes
      PatchSDSave5(0x0238b130,(u32*)0x0238bd7c,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31312e32,0x023a7258)) //2.11
    {
      CHECK_VALUE(0x0238b198,0xe92d4010);
      CHECK_VALUE(0x0238bddc,0xe5900010); //11
      CHECK_VALUE(0x0238bdf4,0xe592100c); //9
      CHECK_VALUE(0x0238be48,0xe5921010); //6
      //0x0238b198 - 0x0238b8cc - 1844 bytes
      PatchSDSave5(0x0238b198,(u32*)0x0238bde4,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31312e32,0x023a8d38)||COMPARE_VALUE(0x31312e32,0x023a8d10)) //2.11
    {
      CHECK_VALUE(0x0238c068,0xe92d4010);
      CHECK_VALUE(0x0238ccac,0xe5900010); //11
      CHECK_VALUE(0x0238ccc4,0xe592100c); //9
      CHECK_VALUE(0x0238cd18,0xe5921010); //6
      //0x0238c068 - 0x0238c79c - 1844 bytes
      PatchSDSave5(0x0238c068,(u32*)0x0238ccb4,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x6c696146,0x0238e1f4)) //Fail
    {
      CHECK_VALUE(0x02387308,0xe92d4010);
      CHECK_VALUE(0x02387f4c,0xe5900010); //11
      CHECK_VALUE(0x02387f64,0xe592100c); //9
      CHECK_VALUE(0x02387fb8,0xe5921010); //6
      //0x02387308 - 0x02387a3c - 1844 bytes
      PatchSDSave5(0x02387308,(u32*)0x02387f54,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31312e32,0x023a72bc)) //2.11
    {
      CHECK_VALUE(0x0238b21c,0xe92d4010);
      CHECK_VALUE(0x0238be60,0xe5900010); //11
      CHECK_VALUE(0x0238be78,0xe592100c); //9
      CHECK_VALUE(0x0238becc,0xe5921010); //6
      //0x0238b21c - 0x0238b950 - 1844 bytes
      PatchSDSave5(0x0238b21c,(u32*)0x0238be68,0xe1a00002,true); //mov r0, r2
    }
    //sm64ds (e)
    else if(COMPARE_VALUE(0x31312e32,0x023a4afc)) //2.11
    {
      CHECK_VALUE(0x0238750c,0xe92d4010);
      CHECK_VALUE(0x02388048,0xe5900010); //11
      CHECK_VALUE(0x02388060,0xe592100c); //9
      CHECK_VALUE(0x023880b4,0xe5921010); //6
      //0x0238750c - 0x02387c40 - 1844 bytes
      PatchSDSave5(0x0238750c,(u32*)0x02388050,0xe1a00002,true); //mov r0, r2
    }
    //sm64ds (u)
    else if(COMPARE_VALUE(0x31312e32,0x023a4b3c)) //2.11
    {
      CHECK_VALUE(0x0238754c,0xe92d4010);
      CHECK_VALUE(0x02388088,0xe5900010); //11
      CHECK_VALUE(0x023880a0,0xe592100c); //9
      CHECK_VALUE(0x023880f4,0xe5921010); //6
      //0x0238754c - 0x02387c80 - 1844 bytes
      PatchSDSave5(0x0238754c,(u32*)0x02388090,0xe1a00002,true); //mov r0, r2
    }
    //wariware: touched (u)
    else if(COMPARE_VALUE(0x31312e32,0x023a6a38)) //2.11
    {
      CHECK_VALUE(0x0238af68,0xe92d4010);
      CHECK_VALUE(0x0238bba0,0xe5900010); //11
      CHECK_VALUE(0x0238bbb8,0xe592100c); //9
      CHECK_VALUE(0x0238bc0c,0xe5921010); //6
      //0x0238af68 - 0x0238b69c - 1844 bytes
      PatchSDSave5(0x0238af68,(u32*)0x0238bba8,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31312e32,0x023a6a50)) //2.11
    {
      CHECK_VALUE(0x0238afb4,0xe92d4010);
      CHECK_VALUE(0x0238bbec,0xe5900010); //11
      CHECK_VALUE(0x0238bc04,0xe592100c); //9
      CHECK_VALUE(0x0238bc58,0xe5921010); //6
      //0x0238afb4 - 0x0238b6e8 - 1844 bytes
      PatchSDSave5(0x0238afb4,(u32*)0x0238bbf4,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31312e32,0x023a6a90)) //2.11
    {
      CHECK_VALUE(0x0238afc0,0xe92d4010);
      CHECK_VALUE(0x0238bbf8,0xe5900010); //11
      CHECK_VALUE(0x0238bc10,0xe592100c); //9
      CHECK_VALUE(0x0238bc64,0xe5921010); //6
      //0x0238afc0 - 0x0238b6f4 - 1844 bytes
      PatchSDSave5(0x0238afc0,(u32*)0x0238bc00,0xe1a00002,true); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31312e32,0x023a6b04)) //2.11
    {
      CHECK_VALUE(0x0238b054,0xe92d4010);
      CHECK_VALUE(0x0238bc8c,0xe5900010); //11
      CHECK_VALUE(0x0238bca4,0xe592100c); //9
      CHECK_VALUE(0x0238bcf8,0xe5921010); //6
      //0x0238b054 - 0x0238b788 - 1844 bytes
      PatchSDSave5(0x0238b054,(u32*)0x0238bc94,0xe1a00002,true); //mov r0, r2
    }
    //yoshi touch&go (e)
    else if(COMPARE_VALUE(0x31322e32,0x023a5f48)||COMPARE_VALUE(0x31322e32,0x023a5f10)||COMPARE_VALUE(0x31322e32,0x023a63e4)) //2.21
    {
      CHECK_VALUE(0x0238aaec,0xe92d4010);
      CHECK_VALUE(0x0238b6c4,0xe5900010); //11
      CHECK_VALUE(0x0238b6dc,0xe592100c); //9
      CHECK_VALUE(0x0238b730,0xe5921010); //6
      //0x0238aaec - 0x0238b1e4 - 1784 bytes
      PatchSDSave5(0x0238aaec,(u32*)0x0238b6cc,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31322e32,0x023a5ffc)||COMPARE_VALUE(0x31322e32,0x023a5fc4)) //2.21
    {
      CHECK_VALUE(0x0238aba0,0xe92d4010);
      CHECK_VALUE(0x0238b778,0xe5900010); //11
      CHECK_VALUE(0x0238b790,0xe592100c); //9
      CHECK_VALUE(0x0238b7e4,0xe5921010); //6
      //0x0238aba0 - 0x0238b298 - 1784 bytes
      PatchSDSave5(0x0238aba0,(u32*)0x0238b780,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x6c696146,0x0238e314)) //Fail
    {
      CHECK_VALUE(0x02387418,0xe92d4010);
      CHECK_VALUE(0x02387ff0,0xe5900010); //11
      CHECK_VALUE(0x02388008,0xe592100c); //9
      CHECK_VALUE(0x0238805c,0xe5921010); //6
      //0x02387418 - 0x02387b10 - 1784 bytes
      PatchSDSave5(0x02387418,(u32*)0x02387ff8,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x6c696146,0x0238e260)) //Fail
    {
      CHECK_VALUE(0x02387364,0xe92d4010);
      CHECK_VALUE(0x02387f3c,0xe5900010); //11
      CHECK_VALUE(0x02387f54,0xe592100c); //9
      CHECK_VALUE(0x02387fa8,0xe5921010); //6
      //0x02387364 - 0x02387a5c - 1784 bytes
      PatchSDSave5(0x02387364,(u32*)0x02387f44,0xe1a00002,false); //mov r0, r2
    }
    else if(COMPARE_VALUE(0x31322e32,0x023a7a68)) //2.21
    {
      CHECK_VALUE(0x0238ba24,0xe92d4010);
      CHECK_VALUE(0x0238c5fc,0xe5900010); //11
      CHECK_VALUE(0x0238c614,0xe592100c); //9
      CHECK_VALUE(0x0238c668,0xe5921010); //6
      //0x0238ba24 - 0x0238c11c - 1784 bytes
      PatchSDSave5(0x0238ba24,(u32*)0x0238c604,0xe1a00002,false); //mov r0, r2
    }
    //tom clancy scct (e/u)
    else if(COMPARE_VALUE(0x31322e32,0x023a35b4)) //2.21
    {
      CHECK_VALUE(0x02389428,0x1c04b510);
      CHECK_VALUE(0x02389c34,0x69006800); //11
      CHECK_VALUE(0x02389c44,0x695268d1); //9
      CHECK_VALUE(0x02389c74,0x69526911); //6
      //0x02389428 - 0x023898ec - 1220 bytes
      PatchSDSave7(0x02389428,0x02389c38,0x00001c10); //mov r0, r2
    }
    //touch kirby (j)
    else if(COMPARE_VALUE(0x31322e32,0x023a3504)) //2.21
    {
      CHECK_VALUE(0x023893b0,0x1c04b510);
      CHECK_VALUE(0x02389bbc,0x69006800); //11
      CHECK_VALUE(0x02389bcc,0x695268d1); //9
      CHECK_VALUE(0x02389bfc,0x69526911); //6
      //0x023893b0 - 0x02389874 - 1220 bytes
      PatchSDSave7(0x023893b0,0x02389bc0,0x00001c10); //mov r0, r2
    }
    //kirby - canvas curse (u)
    else if(COMPARE_VALUE(0x39332e32,0x023a3c84)) //2.39
    {
      CHECK_VALUE(0x023894d0,0x1c04b510);
      CHECK_VALUE(0x02389cb8,0x69006800); //11
      CHECK_VALUE(0x02389cc8,0x695268d1); //9
      CHECK_VALUE(0x02389cf8,0x69526911); //6
      //0x023894d0 - 0x02389984 - 1204 bytes
      PatchSDSave7(0x023894d0,0x02389cbc,0x00001c10); //mov r0, r2
    }
    //kirby - power paintbrush (e)
    else if(COMPARE_VALUE(0x37342e32,0x023a459c)) //2.47
    {
      CHECK_VALUE(0x023897c8,0x1c04b510);
      CHECK_VALUE(0x02389fb0,0x69006800); //11
      CHECK_VALUE(0x02389fc0,0x695268d1); //9
      CHECK_VALUE(0x02389ff0,0x69526911); //6
      //0x023897c8 - 0x02389c7c - 1204 bytes
      PatchSDSave7(0x023897c8,0x02389fb4,0x00001c10); //mov r0, r2
    }
    //tootuff - mission nadia (e)
    else if(COMPARE_VALUE(0x37342e32,0x023a4598)) //2.47
    {
      CHECK_VALUE(0x023897c4,0x1c04b510);
      CHECK_VALUE(0x02389fac,0x69006800); //11
      CHECK_VALUE(0x02389fbc,0x695268d1); //9
      CHECK_VALUE(0x02389fec,0x69526911); //6
      //0x023897c4 - 0x02389c78 - 1204 bytes
      PatchSDSave7(0x023897c4,0x02389fb0,0x00001c10); //mov r0, r2
    }
    //super princess peach
    else if(COMPARE_VALUE(0x037fb5cc,0x0238ca00)) //?.??
    {
      CHECK_VALUE(0x0238603c,0xe92d4010);
      CHECK_VALUE(0x02386d00,0xe5921010); //6
      CHECK_VALUE(0x02386d48,0xe592100c); //9
      CHECK_VALUE(0x02386d5c,0xe5900010); //11
      //0x0238603c - 0x02386784 - 1864 bytes
      PatchSDSave8(0x0238603c,(u32*)0x02386d00,0xe1a00002); //mov r0, r2
    }
    //dragon ball z - supersonic warriors 2 (u)
    else if(COMPARE_VALUE(0x37342e32,0x023a71c0)||COMPARE_VALUE(0x37342e32,0x023a7504)) //2.47
    {
      CHECK_VALUE(0x0238ae48,0xe92d4010);
      CHECK_VALUE(0x0238bbf0,0xe5921010); //6
      CHECK_VALUE(0x0238bc38,0xe592100c); //9
      CHECK_VALUE(0x0238bc4c,0xe5900010); //11
      //0x0238ae48 - 0x0238b680 - 2104 bytes
      PatchSDSave2(0x0238ae48,(u32*)0x0238bbb8,0xe1a00002,14); //mov r0, r2
    }
    //sbk - snowboard kids (u)
    else if(COMPARE_VALUE(0x31362e32,0x023a7750)) //2.61
    {
      CHECK_VALUE(0x0238b0b0,0xe92d4010);
      CHECK_VALUE(0x0238be58,0xe5921010); //6
      CHECK_VALUE(0x0238bea0,0xe592100c); //9
      CHECK_VALUE(0x0238beb4,0xe5900010); //11
      //0x0238b0b0 - 0x0238b8e8 - 2104 bytes
      PatchSDSave2(0x0238b0b0,(u32*)0x0238be20,0xe1a00002,14); //mov r0, r2
    }
    //shanghai (j)
    else if(COMPARE_VALUE(0x37342e32,0x023a8ef4)) //2.47
    {
      CHECK_VALUE(0x0238beec,0xe92d4010);
      CHECK_VALUE(0x0238cc94,0xe5921010); //6
      CHECK_VALUE(0x0238ccdc,0xe592100c); //9
      CHECK_VALUE(0x0238ccf0,0xe5900010); //11
      //0x0238beec - 0x0238c724 - 2104 bytes
      PatchSDSave2(0x0238beec,(u32*)0x0238cc5c,0xe1a00002,14); //mov r0, r2
    }
    //tottoko hamutaro - nazo nazo q - kumo no ue no hatena jou (j)
    else if(COMPARE_VALUE(0x31362e32,0x023a80ac)) //2.61
    {
      CHECK_VALUE(0x02388088,0xe92d4010);
      CHECK_VALUE(0x02388e30,0xe5921010); //6
      CHECK_VALUE(0x02388e78,0xe592100c); //9
      CHECK_VALUE(0x02388e8c,0xe5900010); //11
      //0x02388088 - 0x023888c0 - 2104 bytes
      PatchSDSave2(0x02388088,(u32*)0x02388df8,0xe1a00002,14); //mov r0, r2
    }
    //yakitate!! japan - game ichigou - choujou kessen!! pantasic grand prix! (j)
    else if(COMPARE_VALUE(0x31362e32,0x023a9484)) //2.61
    {
      CHECK_VALUE(0x0238c154,0xe92d4010);
      CHECK_VALUE(0x0238cefc,0xe5921010); //6
      CHECK_VALUE(0x0238cf44,0xe592100c); //9
      CHECK_VALUE(0x0238cf58,0xe5900010); //11
      //0x0238c154 - 0x0238c98c - 2104 bytes
      PatchSDSave2(0x0238c154,(u32*)0x0238cec4,0xe1a00002,14); //mov r0, r2
    }
    //ice age 2 (e)
    else if(COMPARE_VALUE(0x38362e32,0x023a77b8)) //2.68
    {
      CHECK_VALUE(0x0238b0f8,0xe92d4010);
      CHECK_VALUE(0x0238bde8,0xe5921010); //6
      CHECK_VALUE(0x0238be30,0xe592100c); //9
      CHECK_VALUE(0x0238be44,0xe5900010); //11
      //0x0238b0f8 - 0x0238b644 - 1356 bytes
      PatchSDSave2(0x0238b0f8,(u32*)0x0238bdb0,0xe1a00002,14); //mov r0, r2
    }
    //over the hedge (u)
    else if(COMPARE_VALUE(0x38362e32,0x023a94e8)) //2.68
    {
      CHECK_VALUE(0x0238c19c,0xe92d4010);
      CHECK_VALUE(0x0238ce8c,0xe5921010); //6
      CHECK_VALUE(0x0238ced4,0xe592100c); //9
      CHECK_VALUE(0x0238cee8,0xe5900010); //11
      //0x0238c19c - 0x0238c6e8 - 1356 bytes
      PatchSDSave2(0x0238c19c,(u32*)0x0238ce54,0xe1a00002,14); //mov r0, r2
    }
    //sudoku gridmaster (u)
    else if(COMPARE_VALUE(0x31362e32,0x023a7670)) //2.61
    {
      CHECK_VALUE(0x0238b0b0,0xe92d4010);
      CHECK_VALUE(0x0238bda0,0xe5921010); //6
      CHECK_VALUE(0x0238bde8,0xe592100c); //9
      CHECK_VALUE(0x0238bdfc,0xe5900010); //11
      //0x0238b0b0 - 0x0238b5fc - 1356 bytes
      PatchSDSave2(0x0238b0b0,(u32*)0x0238bd68,0xe1a00002,14); //mov r0, r2
    }
    //monster bomber (j)
    else if(COMPARE_VALUE(0x38362e32,0x023a8114)) //2.68
    {
      CHECK_VALUE(0x02388088,0xe92d4010);
      CHECK_VALUE(0x02388d78,0xe5921010); //6
      CHECK_VALUE(0x02388dc0,0xe592100c); //9
      CHECK_VALUE(0x02388dd4,0xe5900010); //11
      //0x02388088 - 0x023885d4 - 1356 bytes
      PatchSDSave2(0x02388088,(u32*)0x02388d40,0xe1a00002,14); //mov r0, r2
    }
    //bleach ds - souten ni kakeru unmei (j)
    else if(COMPARE_VALUE(0x35362e32,0x023a82e4)) //2.65
    {
      CHECK_VALUE(0x0238b4f4,0xe92d4010);
      CHECK_VALUE(0x0238c284,0xe5921010); //6
      CHECK_VALUE(0x0238c2cc,0xe592100c); //9
      CHECK_VALUE(0x0238c2e0,0xe5900010); //11
      //0x0238b4f4 - 0x0238bd14 - 2080 bytes
      PatchSDSave2(0x0238b4f4,(u32*)0x0238c24c,0xe1a00002,14); //mov r0, r2
    }
    //eigo ga nigate na otona no ds training - eigo zuke (j)
    else if(COMPARE_VALUE(0x31362e32,0x023a4d80)) //2.61
    {
      CHECK_VALUE(0x02389960,0x1c04b510);
      CHECK_VALUE(0x0238a278,0x69526911); //6
      CHECK_VALUE(0x0238a2a0,0x68d16910); //9
      CHECK_VALUE(0x0238a2ac,0x69006820); //11
      //0x02389960 - 0x02389cd0 - 880 bytes
      PatchSDSave9(0x02389960,0x0238a254,0x00001c10,ESave9Type61); //mov r0, r2
    }
    //eigo ga nigate na otona no ds training - eigo zuke (j) (rev 1)
    else if(COMPARE_VALUE(0x38362e32,0x023a4f28)) //2.68
    {
      CHECK_VALUE(0x023899a8,0xb081b5f0);
      CHECK_VALUE(0x0238a320,0x69526911); //6
      CHECK_VALUE(0x0238a348,0x68d16910); //9
      CHECK_VALUE(0x0238a354,0x69006820); //11
      //0x023899a8 - 0x02389d58 - 944 bytes
      PatchSDSave9(0x023899a8,0x0238a2fc,0x00001c10,ESave9Type61); //mov r0, r2
    }
    //nintendo ds browser (ue)
    else if(COMPARE_VALUE(0x31372e32,0x023a5d6c)||COMPARE_VALUE(0x31372e32,0x023a5d68)) //2.71
    {
      CHECK_VALUE(0x02389d44,0xb081b5f0);
      CHECK_VALUE(0x0238a544,0x691168d0); //6
      CHECK_VALUE(0x0238a570,0x695268d1); //9
      CHECK_VALUE(0x0238a584,0x20036821); //11
      //0x02389d44 - 0x0238a0f4 - 944 bytes
      PatchSDSave6(0x02389d44,0x0238a528,0x00001c10,ESave6Type71); //mov r0, r2
    }
    //dead'n'furious (e)
    else if(COMPARE_VALUE(0x34372e32,0x023a5cd8)||COMPARE_VALUE(0x34372e32,0x023a5cdc)) //2.74
    {
      CHECK_VALUE(0x02389da8,0xb081b5f0);
      CHECK_VALUE(0x0238a610,0x691168d0); //6
      CHECK_VALUE(0x0238a63c,0x695268d1); //9
      CHECK_VALUE(0x0238a648,0x69496908); //11
      //0x02389da8 - 0x0238a1ac - 1028 bytes
      PatchSDSave10(0x02389da8,0x0238a5f8,0x00001c10); //mov r0, r2
    }
    //diddy kong racing ds (u)
    else if(COMPARE_VALUE(0x38372e32,0x023a5fd4)) //2.78
    {
      CHECK_VALUE(0x02389eb0,0xb081b5f0);
      CHECK_VALUE(0x0238a71c,0x691168d0); //6
      CHECK_VALUE(0x0238a748,0x695268d1); //9
      CHECK_VALUE(0x0238a754,0x69496908); //11
      //0x02389eb0 - 0x0238a2b4 - 1028 bytes
      PatchSDSave10(0x02389eb0,0x0238a704,0x00001c10); //mov r0, r2
    }
    //pokemon pearl/diamond (j)
    else if(COMPARE_VALUE(0x34372e32,0x023a8fd8)) //2.74
    {
      CHECK_VALUE(0x02388294,0xe92d47f0);
      CHECK_VALUE(0x02388ea0,0xe5921010); //6
      CHECK_VALUE(0x02388ee8,0xe592100c); //9
      CHECK_VALUE(0x02388efc,0xe5910010); //11
      //0x02388294 - 0x023888c0 - 1580 bytes
      PatchSDSave1(0x02388294,(u32*)0x02388e74,0xe1a00002,11); //mov r0, r2
    }
    //pokemon gold/silver
    else if(COMPARE_VALUE(0x33382e32,0x023a76a4)) //2.83
    {
      CHECK_VALUE(0x02387930,0xe92d41f0);
      CHECK_VALUE(0x02388730,0xe593000c); //6
      CHECK_VALUE(0x0238876c,0xe5930010); //9
      CHECK_VALUE(0x023887bc,0xe593000c); //14
      //0x02387930 - 0x023882f4 - 2500 bytes
      PatchSDSave0(0x02387930,(u32*)0x0238870c,0xe1a00003); //mov r0, r3
    }
    //hoshizora navi (j)
    else if(COMPARE_VALUE(0x33382e32,0x023a6dd0)) //2.83
    {
      CHECK_VALUE(0x0238a808,0xe92d41f0);
      CHECK_VALUE(0x0238b590,0xe593000c); //6
      CHECK_VALUE(0x0238b5cc,0xe5930010); //9
      CHECK_VALUE(0x0238b61c,0xe593000c); //14
      //0x0238a808 - 0x0238b158 - 2384 bytes
      PatchSDSave0(0x0238a808,(u32*)0x0238b56c,0xe1a00003); //mov r0, r3
    }
    //sonic classic collection (u)
    else if(COMPARE_VALUE(0x38382e32,0x023aaa7c)) //2.88
    {
      CHECK_VALUE(0x0238f818,0xe92d41f0);
      CHECK_VALUE(0x02390358,0xe592000c); //6
      CHECK_VALUE(0x023903a0,0xe5920010); //9
      CHECK_VALUE(0x023903fc,0xe590000c); //14
      //0x0238f818 - 0x02390158 - 2368 bytes
      PatchSDSave4(0x0238f818,(u32*)0x02390338,0xe1a00002,9); //mov r0, r2
    }
    //estpolis - the lands cursed by the gods (j)
    else if(COMPARE_VALUE(0x38382e32,0x023a6bb8)) //2.88
    {
      CHECK_VALUE(0x0238c77c,0xe92d41f0);
      CHECK_VALUE(0x0238d25c,0xe592000c); //6
      CHECK_VALUE(0x0238d2a4,0xe5920010); //9
      CHECK_VALUE(0x0238d300,0xe590000c); //14
      //0x0238c77c - 0x0238d0bc - 2368 bytes
      PatchSDSave4(0x0238c77c,(u32*)0x0238d23c,0xe1a00002,9); //mov r0, r2
    }
    //super robot taisen og saga - masou kishin - the lord of elemental (j)
    else if(COMPARE_VALUE(0x38382e32,0x023a6404)) //2.88
    {
      CHECK_VALUE(0x0238e1d0,0xe92d41f0);
      CHECK_VALUE(0x0238ecb0,0xe592000c); //6
      CHECK_VALUE(0x0238ecf8,0xe5920010); //9
      CHECK_VALUE(0x0238ed54,0xe590000c); //14
      //0x0238e1d0 - 0x0238eb10 - 2368 bytes
      PatchSDSave4(0x0238e1d0,(u32*)0x0238ec90,0xe1a00002,9); //mov r0, r2
    }
    //world of zoo
    else if(COMPARE_VALUE(0x38382e32,0x023a4540)) //2.88
    {
      CHECK_VALUE(0x0238c684,0x480ab508);
      CHECK_VALUE(0x0238cce8,0x691168d0); //6
      CHECK_VALUE(0x0238cd14,0x695268d1); //9
      CHECK_VALUE(0x0238cd48,0x68c06820); //14
      //0x0238c684 - 0x0238cb98 - 1300 bytes
      PatchSDSave6(0x0238c684,0x0238cccc,0x00001c10,ESave6Type88); //mov r0, r2
    }
    //sonic & sega all-stars racing
    else if(COMPARE_VALUE(0x38382e32,0x023a36e0)) //2.88
    {
      CHECK_VALUE(0x0238c07c,0x480ab508);
      CHECK_VALUE(0x0238c794,0x69526911); //6
      CHECK_VALUE(0x0238c7bc,0x68d16910); //9
      CHECK_VALUE(0x0238c7f4,0x780068c0); //14
      //0x0238c07c - 0x0238c648 - 1484 bytes
      PatchSDSave9(0x0238c07c,0x0238c77c,0x00001c10,ESave9Type88); //mov r0, r2
    }
    //eigokoro kyoushitsu ds (j)
    else if(COMPARE_VALUE(0x38382e32,0x023a905c)) //2.88
    {
      CHECK_VALUE(0x0238f038,0xe92d41f0);
      CHECK_VALUE(0x0238fb78,0xe592000c); //6
      CHECK_VALUE(0x0238fbc0,0xe5920010); //9
      CHECK_VALUE(0x0238fc1c,0xe590000c); //14
      //0x0238f038 - 0x0238f978 - 2368 bytes
      PatchSDSave4(0x0238f038,(u32*)0x0238fb58,0xe1a00002,9); //mov r0, r2
    }
    //heartcatch pretty cure! - oshare collection (j)
    else if(COMPARE_VALUE(0x38382e32,0x023a8610)) //2.88
    {
      CHECK_VALUE(0x0238ebcc,0xe92d41f0);
      CHECK_VALUE(0x0238f6ac,0xe592000c); //6
      CHECK_VALUE(0x0238f6f4,0xe5920010); //9
      CHECK_VALUE(0x0238f750,0xe590000c); //14
      //0x0238ebcc - 0x0238f50c - 2368 bytes
      PatchSDSave4(0x0238ebcc,(u32*)0x0238f68c,0xe1a00002,9); //mov r0, r2
    }
    //jaka jaka music! (j)
    else if(COMPARE_VALUE(0x38382e32,0x023a6460)) //2.88
    {
      CHECK_VALUE(0x0238e1cc,0xe92d41f0);
      CHECK_VALUE(0x0238ecec,0xe592000c); //6
      CHECK_VALUE(0x0238ed34,0xe5920010); //9
      CHECK_VALUE(0x0238ed90,0xe590000c); //14
      //0x0238e1cc - 0x0238eb0c - 2368 bytes
      PatchSDSave4(0x0238e1cc,(u32*)0x0238eccc,0xe1a00002,9); //mov r0, r2
    }
    //camp rock (e)
    else if(COMPARE_VALUE(0x38382e32,0x023a2b04)) //2.88
    {
      CHECK_VALUE(0x0238c0d0,0x480ab508);
      CHECK_VALUE(0x0238c814,0x69526911); //6
      CHECK_VALUE(0x0238c83c,0x68d16910); //9
      CHECK_VALUE(0x0238c874,0x780068c0); //14
      //0x0238c0d0 - 0x0238c684 - 1460 bytes
      PatchSDSave9(0x0238c0d0,0x0238c7fc,0x00001c10,ESave9Type88); //mov r0, r2
    }
    //quest game (n)
    else if(COMPARE_VALUE(0x38382e32,0x023a3764)) //2.88
    {
      CHECK_VALUE(0x0238c084,0x480ab508);
      CHECK_VALUE(0x0238c79c,0x69526911); //6
      CHECK_VALUE(0x0238c7c4,0x68d16910); //9
      CHECK_VALUE(0x0238c7fc,0x780068c0); //14
      //0x0238c084 - 0x0238c650 - 1484 bytes
      PatchSDSave9(0x0238c084,0x0238c784,0x00001c10,ESave9Type88); //mov r0, r2
    }
    //12 card games (e)
    else if(COMPARE_VALUE(0x38382e32,0x023a8370)) //2.88
    {
      CHECK_VALUE(0x0238ebc8,0xe92d41f0);
      CHECK_VALUE(0x0238f6a8,0xe592000c); //6
      CHECK_VALUE(0x0238f6f0,0xe5920010); //9
      CHECK_VALUE(0x0238f74c,0xe590000c); //14
      //0x0238ebc8 - 0x0238f508 - 2368 bytes
      PatchSDSave4(0x0238ebc8,(u32*)0x0238f688,0xe1a00002,9); //mov r0, r2
    }
    //power pro kun pocket 13 (j)
    else if(COMPARE_VALUE(0x38382e32,0x023aace8)) //2.88
    {
      CHECK_VALUE(0x0238f93c,0xe92d41f0);
      CHECK_VALUE(0x0239047c,0xe592000c); //6
      CHECK_VALUE(0x023904c4,0xe5920010); //9
      CHECK_VALUE(0x02390520,0xe590000c); //14
      //0x0238f93c - 0x0239027c - 2368 bytes
      PatchSDSave4(0x0238f93c,(u32*)0x0239045c,0xe1a00002,9); //mov r0, r2
    }
    //saga 3 - jikuu no hasha - shadow or light (j)
    else if(COMPARE_VALUE(0x38382e32,0x023a866c)) //2.88
    {
      CHECK_VALUE(0x0238ebc8,0xe92d41f0);
      CHECK_VALUE(0x0238f6e8,0xe592000c); //6
      CHECK_VALUE(0x0238f730,0xe5920010); //9
      CHECK_VALUE(0x0238f78c,0xe590000c); //14
      //0x0238ebc8 - 0x0238f508 - 2368 bytes
      PatchSDSave4(0x0238ebc8,(u32*)0x0238f6c8,0xe1a00002,9); //mov r0, r2
    }
    //monster jam - path of destruction (u)
    else if(COMPARE_VALUE(0x38382e32,0x023a84f4)) //2.88
    {
      CHECK_VALUE(0x0238eb7c,0xe92d41f0);
      CHECK_VALUE(0x0238f65c,0xe592000c); //6
      CHECK_VALUE(0x0238f6a4,0xe5920010); //9
      CHECK_VALUE(0x0238f700,0xe590000c); //14
      //0x0238eb7c - 0x0238f4bc - 2368 bytes
      PatchSDSave4(0x0238eb7c,(u32*)0x0238f63c,0xe1a00002,9); //mov r0, r2
    }
    //sawaru - made in wario (j) (rev 2)
    else if(COMPARE_VALUE(0x31312e32,0x023a6324)) //2.11
    {
      CHECK_VALUE(0x0238aecc,0xe92d4010);
      CHECK_VALUE(0x0238baf8,0xe5900010); //11
      CHECK_VALUE(0x0238bb10,0xe592100c); //9
      CHECK_VALUE(0x0238bb64,0xe5921010); //6
      //0x0238aecc - 0x0238b600 - 1844 bytes
      PatchSDSave5(0x0238aecc,(u32*)0x0238bb00,0xe1a00002,true); //mov r0, r2
    }
    //cabela's dangerous hunts 2011 (u)
    else if(COMPARE_VALUE(0x38382e32,0x023a8254)) //2.88
    {
      CHECK_VALUE(0x0238eb78,0xe92d41f0);
      CHECK_VALUE(0x0238f658,0xe592000c); //6
      CHECK_VALUE(0x0238f6a0,0xe5920010); //9
      CHECK_VALUE(0x0238f6fc,0xe590000c); //14
      //0x0238eb78 - 0x0238f4b8 - 2368 bytes
      PatchSDSave4(0x0238eb78,(u32*)0x0238f638,0xe1a00002,9); //mov r0, r2
    }
    //gyakuten kenji 2 (j)
    else if(COMPARE_VALUE(0x38382e32,0x023a37c8)) //2.88
    {
      CHECK_VALUE(0x0238c080,0x480ab508);
      CHECK_VALUE(0x0238c7dc,0x69526911); //6
      CHECK_VALUE(0x0238c804,0x68d16910); //9
      CHECK_VALUE(0x0238c83c,0x780068c0); //14
      //0x0238c080 - 0x0238c64c - 1484 bytes
      PatchSDSave9(0x0238c080,0x0238c7c4,0x00001c10,ESave9Type88); //mov r0, r2
    }
    //18 classic card games (u)
    else if(COMPARE_VALUE(0x38382e32,0x023a83cc)) //2.88
    {
      CHECK_VALUE(0x0238ebc4,0xe92d41f0);
      CHECK_VALUE(0x0238f6e4,0xe592000c); //6
      CHECK_VALUE(0x0238f72c,0xe5920010); //9
      CHECK_VALUE(0x0238f788,0xe590000c); //14
      //0x0238ebc4 - 0x0238f504 - 2368 bytes
      PatchSDSave4(0x0238ebc4,(u32*)0x0238f6c4,0xe1a00002,9); //mov r0, r2
    }
    else
    {
      showMsgandWait(MSGID_LOADING,MSG_ERROR_SD_COLOR);
    }
    //patch "1981 - Houkago Shounen (Japan)"
    if(0x4a474859==GameCode()) //YHGJ
    {
      u32* ptr=(u32*)(unprot_001_bin+0x2c);
      ptr[0]=0xe1a00000;
      ptr[1]=0xe1a00000;
      static SOverlayInfo info={0x00000001,0x0203c448,KMemProtMe,{0x0209c1c8,0x0209c1d8}};
      NeedUncompressed();
      PatchOverlay(info);
    }
    //patch "4931 - Dementium II (USA) (En,Fr,Es)" & "5246 - Dementium II (Europe) (En,Fr,De,Es,It)"
    if(0x45454442==GameCode()||0x50454442==GameCode()) //BDEE|BDEP
    {
      PatchMem(KArm9,0x020e9120,0xe3a00002); //mov r0, #2
      PatchMem(KArm9,0x020e9124,0xea000029); //b xxx
    }
    //patch "5240 - Dimentium II - Tozasareta Byoutou (Japan)"
    if(0x4a454442==GameCode()) //BDEJ
    {
      PatchMem(KArm9,0x020d9f64,0xe3a00005); //mov r0, #5
      PatchMem(KArm9,0x020d9f68,0xea000029); //b xxx
    }
    //there is not CARDi_WaitAsync after CARDi_RequestStreamCommand after backup init.
    //so sometime game freezes at startup then launched 1st time.
    if(0x00455341==GameCodeAll()) //children of mana
    {
      NeedUncompressed();
      switch(GameCode())
      {
        //0338 - Seiken Densetsu DS - Children of Mana (Japan)
        case 0x4a455341: //ASEJ
          PatchMem(KArm9,0x02090454,0xeaff00d8); //b 0x020507bc
          break;
        //0613 - Children of Mana (USA)
        case 0x45455341: //ASEE
          PatchMem(KArm9,0x0209149c,0xeafefd6f); //b 0x02050a60
          break;
        //0809 - Children of Mana (Europe) (En,Fr,De,Es,It)
        case 0x50455341: //ASEP
          PatchMem(KArm9,0x02091700,0xeafefd33); //b 0x02050bd4
          break;
      }
    }
  }
}

void CPatcher::PatchSDSave0(u32 aPatchMem,u32* aPatchCalls,u32 aPatchInstruction)
{
  PatchSDSave_Copy(aPatchMem);
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+10,ESDRead,true); //6
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+15,ESDWrite,true); //7
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+20,ESDWrite,true); //8
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+25,ESDVerify,true); //9

  PatchSDSave_UnusedFunc(aPatchCalls,31,37,39,46,35);
}

void CPatcher::PatchSDSave1(u32 aPatchMem,u32* aPatchCalls,u32 aPatchInstruction,u32 aShift)
{
  PatchSDSave_Copy(aPatchMem);
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+aShift,ESDRead,true); //6
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+aShift+6,ESDWrite,true); //7
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+aShift+12,ESDWrite,true); //8
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+aShift+18,ESDVerify,true); //9

  u32* patchEntry=aPatchCalls; //2
  PatchMem(KArm7,s32(patchEntry),0xe1a00000); // nop
  patchEntry=aPatchCalls+aShift+25; //11
  PatchMem(KArm7,s32(patchEntry),0xe1a00000); // nop
  patchEntry=aPatchCalls+aShift+27; //12
  PatchMem(KArm7,s32(patchEntry),0xe1a00000); // nop
}

void CPatcher::PatchSDSave2(u32 aPatchMem,u32* aPatchCalls,u32 aPatchInstruction,u32 aShift)
{
  PatchSDSave_Copy(aPatchMem);
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+aShift,ESDRead,true); //6
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+aShift+6,ESDWrite,true); //7
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+aShift+12,ESDWrite,true); //8
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+aShift+18,ESDVerify,true); //9

  u32* patchEntry=aPatchCalls; //2
  PatchMem(KArm7,s32(patchEntry),0xe1a00000); // nop
  patchEntry=aPatchCalls+aShift+24; //11
  PatchMem(KArm7,s32(patchEntry),0xe1a00000); // nop
}

void CPatcher::PatchSDSave3(u32 aPatchMem,s32 aPatchCalls,u32 aPatchInstruction)
{
  PatchSDSave_Copy(aPatchMem);
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+24,ESDRead,false); //6
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+36,ESDWrite,false); //7
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+48,ESDWrite,false); //8
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+60,ESDVerify,false); //9
  //2
  PatchMem(KArm7,aPatchCalls,0x46c046c0); // nop|nop
}

void CPatcher::PatchSDSave4(u32 aPatchMem,u32* aPatchCalls,u32 aPatchInstruction,u32 aShift)
{
  PatchSDSave_Copy(aPatchMem);
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+aShift,ESDRead,true); //6
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+aShift+6,ESDWrite,true); //7
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+aShift+12,ESDWrite,true); //8
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+aShift+18,ESDVerify,true); //9
  PatchSDSave_UnusedFunc(aPatchCalls,aShift+25,aShift+32,aShift+34,aShift+42,aShift+30);
}

void CPatcher::PatchSDSave5(u32 aPatchMem,u32* aPatchCalls,u32 aPatchInstruction,bool aPatchId)
{
  PatchSDSave_Copy(aPatchMem);
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+25,ESDRead,false); //6
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+11,ESDWrite,false); //7
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+18,ESDWrite,false); //8
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+4,ESDVerify,false); //9
  PatchMem(KArm7,s32(aPatchCalls),0xe12fff1e); //bx lr
  if(aPatchId)
  {
    PatchMem(KArm7,s32(aPatchCalls+48),0xe12fff1e); //bx lr
  }
}

void CPatcher::PatchSDSave6(u32 aPatchMem,s32 aPatchCalls,u32 aPatchInstruction,TSave6Type aType)
{
  PatchSDSave_Copy(aPatchMem);
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+28,ESDRead,false); //6
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+44,ESDWrite,false); //7
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+56,ESDWrite,false); //8
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+72,ESDVerify,false); //9

  //2
  PatchMem(KArm7,-(aPatchCalls+2),0x46c0); // nop
  PatchMem(KArm7,-(aPatchCalls+4),0x46c0); // nop
  //6
  PatchMem(KArm7,-(aPatchCalls+36),0x46c0); // nop
  //8
  PatchMem(KArm7,-(aPatchCalls+64),0x46c0); // nop

  switch(aType)
  {
    case ESave6Type71:
      //11
      PatchMem(KArm7,aPatchCalls+84,0x46c046c0); // nop|nop
      PatchMem(KArm7,-(aPatchCalls+88),0x46c0); // nop
      break;
    case ESave6Type83:
    case ESave6Type88:
      //11
      PatchMem(KArm7,aPatchCalls+88,0x46c046c0); // nop|nop
      if(aType==ESave6Type83) break;
      //15
      PatchMem(KArm7,aPatchCalls+100,0x46c046c0); // nop|nop
      //12
      PatchMem(KArm7,-(aPatchCalls+106),0x46c0); // nop
      PatchMem(KArm7,-(aPatchCalls+108),0x46c0); // nop
      //13
      PatchMem(KArm7,aPatchCalls+112,0x46c02000); // mov r0,#0|nop
      //14
      PatchMem(KArm7,-(aPatchCalls+130),0x46c0); // nop
      PatchMem(KArm7,-(aPatchCalls+132),0x46c0); // nop
      break;
  }
}

void CPatcher::PatchSDSave7(u32 aPatchMem,s32 aPatchCalls,u32 aPatchInstruction)
{
  PatchSDSave_Copy(aPatchMem);
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+56,ESDRead,true); //6
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+24,ESDWrite,true); //7
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+40,ESDWrite,true); //8
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+8,ESDVerify,true); //9
  //11
  PatchMem(KArm7,aPatchCalls,0x477046c0); // nop|bx lr
}

void CPatcher::PatchSDSave8(u32 aPatchMem,u32* aPatchCalls,u32 aPatchInstruction)
{
  PatchSDSave_Copy(aPatchMem);
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls,ESDRead,true); //6
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+6,ESDWrite,true); //7
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+12,ESDWrite,true); //8
  PatchSDSave_MainFunc(aPatchMem,aPatchInstruction,aPatchCalls+18,ESDVerify,true); //9

  //11
  PatchMem(KArm7,s32(aPatchCalls+24),0xe1a00000); // nop
}

void CPatcher::PatchSDSave9(u32 aPatchMem,s32 aPatchCalls,u32 aPatchInstruction,TSave9Type aType)
{
  s32 shift=(aType==ESave9Type61)?12:0;
  PatchSDSave_Copy(aPatchMem);
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+shift+24,ESDRead,false); //6
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+shift+36,ESDWrite,false); //7
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+shift+52,ESDWrite,false); //8
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+shift+64,ESDVerify,false); //9
  //2
  PatchMem(KArm7,-(aPatchCalls+2),0x46c0); // nop
  PatchMem(KArm7,-(aPatchCalls+4),0x46c0); // nop
  //7
  PatchMem(KArm7,-(aPatchCalls+shift+44),0x46c0); // nop
  //9
  PatchMem(KArm7,-(aPatchCalls+shift+72),0x46c0); // nop
  //11
  PatchMem(KArm7,aPatchCalls+shift+80,0x46c046c0); // nop|nop
  if(aType==ESave9Type88)
  {
    //11
    PatchMem(KArm7,-(aPatchCalls+shift+84),0x46c0); // nop
    //15
    PatchMem(KArm7,-(aPatchCalls+shift+94),0x46c0); // nop
    PatchMem(KArm7,-(aPatchCalls+shift+96),0x46c0); // nop
    //12
    PatchMem(KArm7,aPatchCalls+shift+100,0x46c046c0); // nop|nop
    //13
    PatchMem(KArm7,-(aPatchCalls+shift+106),0x46c0); // nop
    PatchMem(KArm7,-(aPatchCalls+shift+108),0x2000); // mov r0, #0
    //14
    PatchMem(KArm7,aPatchCalls+shift+124,0x46c046c0); // nop|nop
  }
}

void CPatcher::PatchSDSave10(u32 aPatchMem,s32 aPatchCalls,u32 aPatchInstruction)
{
  PatchSDSave_Copy(aPatchMem);
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+24,ESDRead,false); //6
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+40,ESDWrite,false); //7
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+52,ESDWrite,false); //8
  PatchSDSave_MainFuncThumb(aPatchMem,aPatchInstruction,aPatchCalls+68,ESDVerify,false); //9
  //2
  PatchMem(KArm7,aPatchCalls,0x46c046c0); // nop|nop
  //6
  PatchMem(KArm7,-(aPatchCalls+32),0x46c0); // nop
  //8
  PatchMem(KArm7,-(aPatchCalls+60),0x46c0); // nop
  //11
  PatchMem(KArm7,aPatchCalls+84,0x46c046c0); // nop|nop
}

void CPatcher::PatchSDSave_Copy(u32 aPatchMem)
{
  for(u32* ii=(u32*)sd_save_bin;ii<(u32*)(sd_save_bin+sd_save_bin_size);++ii)
  {
    if(0xefc0de01==*ii) //size
    {
      *ii=SaveMask();
    }
  }
  memcpy((void*)aPatchMem,sd_save_bin,sd_save_bin_size);
  iSDSaveStart=aPatchMem;
  iSDSaveEnd=aPatchMem+sd_save_bin_size;
}

void CPatcher::PatchSDSave_MainFunc(u32 aPatchMem,u32 aPatchInstruction,u32* aCall,TOpType aType,bool aStyle)
{
  PatchMem(KArm7,s32(aCall+0),aPatchInstruction);
  PatchMem(KArm7,s32(aCall+1),0xe3a01000|aType); //mov r1, #?
  PatchMem(KArm7,s32(aCall+2),GenerateArmBL((u32)(aCall+2),aPatchMem,aStyle));
}

void CPatcher::PatchSDSave_MainFuncThumb(u32 aPatchMem,u32 aPatchInstruction,s32 aCall,TOpType aType,bool anOldStyle)
{
  u32 instr2=(0x2100|aType)<<16,shift=0;
  if(anOldStyle)
  {
    PatchMem(KArm7,aCall+0,0x6802b500); //push {lr}|ldr r2, [r0]
    shift=4;
  }
  PatchMem(KArm7,aCall+shift+0,aPatchInstruction|instr2);
  PatchMem(KArm7,aCall+shift+4,GenerateThumbBL(aCall+shift+4,aPatchMem+8));
  if(anOldStyle) PatchMem(KArm7,aCall+shift+8,0x4700bc01); // pop {r0}|bx r0
}

void CPatcher::PatchSDSave_UnusedFunc(u32* aCall,u32 aFunc11,u32 aFunc12,u32 aFunc13,u32 aFunc14,u32 aFunc15)
{
  u32* patchEntry=aCall; //2
  PatchMem(KArm7,s32(patchEntry),0xe1a00000); // nop
  patchEntry=aCall+aFunc11; //11
  PatchMem(KArm7,s32(patchEntry),0xe1a00000); // nop
  patchEntry=aCall+aFunc12; //12
  PatchMem(KArm7,s32(patchEntry),0xe1a00000); // nop
  patchEntry=aCall+aFunc13; //13
  PatchMem(KArm7,s32(patchEntry),0xe3a00000); // mov r0, #0
  patchEntry=aCall+aFunc14; //14
  PatchMem(KArm7,s32(patchEntry),0xe1a00000); // nop
  patchEntry=aCall+aFunc15; //15
  PatchMem(KArm7,s32(patchEntry),0xe1a00000); // nop
}
