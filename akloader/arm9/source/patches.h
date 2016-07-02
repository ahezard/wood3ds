/*
    patches.h
    Copyright (C) 2007 Acekard, www.acekard.com
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

#ifndef __PATCHES_H__
#define __PATCHES_H__

#include <nds.h>
#include <elm.h>
#include "../../share/flags.h"
#include "msgdisplay.h"

#define ArraySize(a) (sizeof(a)/sizeof(a[0]))

//0x027fff60 - unused.
//0x027fff64 - sdState. used for dma and sdsave.
//0x027fff68 - arm7 softreset/cheats - original vblank handler.
//0x027fff6c - softreset state flag.

class CPatcher
{
  public: //param
    inline bool IsDownloadPlay(void) {return iParam&PATCH_DOWNLOAD_PLAY;};
    inline bool IsCheats(void) {return iParam&PATCH_CHEATS;};
    inline bool IsSoftReset(void) {return iParam&PATCH_SOFT_RESET;};
    inline bool IsDMA(void) {return iParam&PATCH_DMA;};
    inline bool IsSDSave(void) {return iParam&PATCH_SD_SAVE;};
    inline bool IsPSRAM(void) {return iParam&PATCH_PSRAM;};
    inline bool IsProtection(void) {return iParam&PATCH_PROTECTION;};
    inline bool IsLinkage(void) {return iParam&PATCH_LINKAGE;};
    inline bool IsCommon(void) {return (IsCheats()||IsSoftReset());};
    inline u32 CommonParam(void) {return (iParam&(PATCH_SOFT_RESET|PATCH_CHEATS));};
    inline bool IsSoftResetOnly(void) {return CommonParam()==PATCH_SOFT_RESET;};
    inline u32 Language(void) {return ((iParam&PATCH_LANGUAGE_MASK)>>PATCH_LANGUAGE_SHIFT);}
    u32 SaveMask(void);
  public:
    inline const char* FileName(void) {return iFileName;};
    inline u32 FixedAddr(void) { return iFixedAddr;};
    inline u32 FixedData(void) { return iFixedData;};
    inline u32 UncompressedSize(void) { return iUncompressedSize;};
    inline u32 GameCode(void) {return iGameCode;};
    inline u32 GameCodeAll(void) {return iGameCodeAll;};
  protected:
    CPatcher(const char* aFileName,u32* aParams,u32 aSizeMask);
  protected: //arm7 hook
    void SearchArm7(void);
    void PatchSoftReset7(u32 anArm7Address2);
    void PatchCheats(void);
    void PatchArm7E(u32 anArm7Address2);
  protected: //cheats
    u32 LoadARCodeTable(u32 aPatchEntry7,u32 aPatchEntry7End);
  protected:
    struct SOverlayInfo
    {
      u32 iOverlayId;
      s32 iLoadAddress;
      u32 iPatchAddress;
      u32 iFixAddresses[2];
    };
  protected: //common
    void NeedUncompressed(void);
    u32 UncompressRom(u8* aMainMemory,u32 aCodeLength,u32 aMemOffset);
    void PatchMem(u32 aCpu,s32 anOffset,u32 aValue);
    void PatchOverlay6(u32 aLoadAddress,u32 aPatchAddress);
    void PatchOverlay7(u32 aLoadAddress);
    void PatchOverlay8(u32 aLoadAddress,u32 aPatchAddress,u32 aFirst,u32 aSecond);
    void PatchRepatch(void);
    void PatchRepatch7(u32* aPos);
    u32* PatchRepatch9(void);
    u32* PushRepatch9(u32* aTarget,bool aType);
    u32 CountRepatch9(void);
    void PatchArm7Check(void);
    void FixProtectionRegion2(void);
    void PatchLanguage(bool aDSi);
    void PatchMPU2(s32 anAddress);
  protected: //protection
    enum TFixType
    {
      EFix01,
      EFix02
    };
  protected: //protection
    void PatchOverlay(SOverlayInfo& anInfo);
    void PatchOverlay9(u32 aLoadAddress,u32 aPatchAddress,u32 aShift);
    void PatchOverlay10(u32 aFixAddress1,u32 aFixAddress2,u32 aFixAddress3,u32 aPatchAddress,u32 aCode,u32 aRead,bool aNew,u32 aBack);
    void PatchOverlay11(s32 aLoadAddress,u32 aPatchAddress,u32 aRead,bool aNew);
    void PatchOverlay12(u32 aLoadAddress,u32 aPatchAddress,u32 aRead,u32 aShift,bool aNew);
    void PatchOverlay13(u32 aLoadAddress,u32 aPatchAddress,u32 aRead,bool aNew);
    void PatchOverlay14(u32 aLoadAddress,u32 aPatchAddress,u32 anOverlay,u32 anAddress);
    void PatchOverlay15(u32 aLoadAddress,u32 aPatchAddress,u32 aRead,bool aNew);
    void PatchOverlay16(s32 aLoadAddress,u32 aPatchAddress,u32 aRead,bool aNew);
    void PatchOverlay17(s32 aLoadAddress,u32 aPatchAddress,u32 aRead,bool aNew);
    void PatchOverlay18(s32 aLoadAddress,u32 aPatchAddress,u32 aRead,bool aNew);
    void FixPatch01(const u8* aPatch,u32 aSize,TFixType aType);
    void FixPatch02(const u8* aPatch,u32 aSize,TFixType aType);
    void PatchRead(const u8* aPatch,u32 aPatchSize,u32 aRead,bool aNew);
  protected: //sdsave
    enum TOpType
    {
      ESDRead=0,
      ESDWrite=1,
      ESDVerify=2
    };
    enum TSave6Type
    {
      ESave6Type71,
      ESave6Type83,
      ESave6Type88
    };
    enum TSave9Type
    {
      ESave9Type61,
      ESave9Type88
    };
  protected: //sdsave
    void PatchSDSave(void);
    void PatchSDSave0(u32 aPatchMem,u32* aPatchCalls,u32 aPatchInstruction);
    void PatchSDSave1(u32 aPatchMem,u32* aPatchCalls,u32 aPatchInstruction,u32 aShift);
    void PatchSDSave2(u32 aPatchMem,u32* aPatchCalls,u32 aPatchInstruction,u32 aShift);
    void PatchSDSave3(u32 aPatchMem,s32 aPatchCalls,u32 aPatchInstruction);
    void PatchSDSave4(u32 aPatchMem,u32* aPatchCalls,u32 aPatchInstruction,u32 aShift);
    void PatchSDSave5(u32 aPatchMem,u32* aPatchCalls,u32 aPatchInstruction,bool aPatchId);
    void PatchSDSave6(u32 aPatchMem,s32 aPatchCalls,u32 aPatchInstruction,TSave6Type aType);
    void PatchSDSave7(u32 aPatchMem,s32 aPatchCalls,u32 aPatchInstruction);
    void PatchSDSave8(u32 aPatchMem,u32* aPatchCalls,u32 aPatchInstruction);
    void PatchSDSave9(u32 aPatchMem,s32 aPatchCalls,u32 aPatchInstruction,TSave9Type aType);
    void PatchSDSave10(u32 aPatchMem,s32 aPatchCalls,u32 aPatchInstruction);
    void PatchSDSave_Copy(u32 aPatchMem);
    void PatchSDSave_MainFunc(u32 aPatchMem,u32 aPatchInstruction,u32* aCall,TOpType aType,bool aStyle);
    void PatchSDSave_MainFuncThumb(u32 aPatchMem,u32 aPatchInstruction,s32 aCall,TOpType aType,bool anOldStyle);
    void PatchSDSave_UnusedFunc(u32* aCall,u32 aFunc11,u32 aFunc12,u32 aFunc13,u32 aFunc14,u32 aFunc15);
  protected: //dma
    void InitDMA4(void);
    void PatchCardId(s32 aMemory);
#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
    void PatchCardIdThumb(s32 aMemory);
#endif
  private:
    CPatcher();
  private: //compression
    bool iUnCompressed;
    u32 iFixedAddr;
    u32 iFixedData;
    u32 iUncompressedSize;
  private:
    const char* iFileName;
    u32 iParam;
    u32 iGameCode;
    u32 iGameCodeAll;
  protected: //dma
    class CDmaFuncs
    {
      public:
        u32* iFunc2;
        u32* iFunc3;
        u32* iFunc4;
        u32* iFunc5;
        u32* iFunc6;
        u32* iCardId;
        u32* iLastAddr;
      public:
        CDmaFuncs(): iFunc2(NULL),iFunc3(NULL),iFunc4(NULL),iFunc5(NULL),iFunc6(NULL),iCardId(NULL),iLastAddr(NULL) {};
    };
  protected: //dma
    CDmaFuncs iDmaFuncs;
    u32 iPatchDMA4Entry;
    u32 iPatchDMA4Ptr;
    bool iUsePatchData;
    u32 iSizeMask;
  protected: //common
    u32 iPatchArm7Entry;
    bool iProtectionRegion2Fixed;
  protected: //sdsave
    u32 iSDSaveStart;
    u32 iSDSaveEnd;
  protected: //cheats
    u32 iCheatOffset;
    u32 iCheatSize;
  protected: //cheats&softreset
    u32 iVBlank;
    u32 iSoftResetHandler;
    u32 iSoftResetHook;
  protected:
    struct TPatchInfo
    {
      s32 iOffset;
      u32 iValue;
    };
    struct TPatchInfo2
    {
      s32 iOffset[2];
      u32 iValue;
    };
    enum
    {
      KPatchInfoSize7=80,
      KPatchInfoSize9=208,
      KArm7=0,
      KArm9=1,
      KCpus=2
    };
    u32 iPatchSize[KCpus];
    TPatchInfo iPatchData7[KPatchInfoSize7];
    TPatchInfo iPatchData9[KPatchInfoSize9];
  protected: //linkage
    bool IsPokemonGame(void);
    bool IsWiiLinkage(void);
  protected: //memory
    enum
    {
      KMemSoftReset=0x23fc000,
      KMemSoftResetITCM=0x1007c00,
      KMemDma=0x23fc800,
      KMemProtMe=0x23fc400, //only for compressed games!
      KMemProtMeAA=0x23fc780 //only for compressed games!
    };
};

class CDsPatcher: public CPatcher
{
  public:
    CDsPatcher(const char* aFileName,u32* aParams,u32 aSpeed,u32 aSizeMask);
    void Patch(void);
  private: //soft-reset
    u32 SoftResetTarget(void);
    void PatchArm9(u32& anArm7Address);
  private: //dma
    void PatchDMA4(void);
  private: //common
    void PatchDumbSave(void);
    void PatchFlashCardCheck(void);
    void PatchOverlay2(SOverlayInfo& anInfo,u32 aStyle);
    void PatchOverlay3(SOverlayInfo& anInfo);
    void PatchOverlay4(SOverlayInfo& anInfo);
    void PatchOverlay5(u32 aLoadAddress);
    void PatchiDS(void);
    void FixBugs(void);
    void FixIntros(void);
  private: //protection
    void PatchMandM(bool anEuro);
    void PatchCoG2(bool anEuro);
    void PatchCoG3(bool anEuro);
    void PatchCOP(void);
    void PatchFlashCardCheck2(void);
  private: //linkage
    void PatchLinkage(void);
    bool PatchPokemon(void);
    bool PatchIL2(void);
  private: //downloadplay
    void PatchDownloadplay(void);
    void PatchDownloadplayArm(u32* aPtr);
  private:
    CDsPatcher();
  private:
    u32 iSpeed;
  private:
    u32 iGbaGameCode;
    u32 iGbaGameCodeAll;
};

class CDsiPatcher: public CPatcher
{
  public:
    CDsiPatcher(const char* aFileName,u32* aParams,u32 aSizeMask);
    void Patch(void);
  private: //dma
    void PatchDMA4(void);
    void PatchFlashCardCheck3(void);
  private: //soft-reset
    void PatchArm9(u32& anArm7Address);
  private: //common
    enum TType
    {
      EType0,
      EType1,
      EType2
    };
  private: //common
    void PatchFlashCardCheck(void);
    TType Type(void);
    void FixRam(void);
    void PatchDumbSave(void);
    void FixBugs(void);
  private:
    u32 iPatchArm9Entry;
  private: //memory
    enum
    {
      KDsiProtMe=0x02003c00
    };
};

class CHbPatcher
{
  public:
    CHbPatcher() {};
    void Patch(void);
};

void patchDS(const char* aFileName,u32* aParams,u32 aSpeed,u32 aSizeMask);
void patchDSi(const char* aFileName,u32* aParams,u32 aSizeMask);
u32 GenerateThumbBL(u32 aSource,u32 aTarget);
u32 GenerateArmBL(u32 aSource,u32 aTarget,bool aType);
void patchHB(void);

#endif
