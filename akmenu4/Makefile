#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

export TARGET		:=	$(shell basename $(CURDIR))
export TOPDIR		:=	$(CURDIR)


#---------------------------------------------------------------------------------
# path to tools - this can be deleted if you set the path in windows
#---------------------------------------------------------------------------------
export PATH		:=	$(DEVKITARM)/bin:$(PATH)

.PHONY: all rpg r4 ak2i r4idsn clean
.PHONY: arm7/rpg/$(TARGET).arm7 arm9/rpg/$(TARGET).arm9
.PHONY: arm7/r4/$(TARGET).arm7 arm9/r4/$(TARGET).arm9
.PHONY: arm7/ak2i/$(TARGET).arm7 arm9/ak2i/$(TARGET).arm9
.PHONY: arm7/r4idsn/$(TARGET).arm7 arm9/r4idsn/$(TARGET).arm9

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: rpg r4

#---------------------------------------------------------------------------------
# rpg target
#---------------------------------------------------------------------------------
rpg: $(TARGET).nds

#---------------------------------------------------------------------------------
# r4 target
#---------------------------------------------------------------------------------
r4: _DS_MENU.DAT

#---------------------------------------------------------------------------------
# ak2i target
#---------------------------------------------------------------------------------
ak2i: ak2i/$(TARGET).nds

#---------------------------------------------------------------------------------
# r4idsn target
#---------------------------------------------------------------------------------
r4idsn: _DSMENU.DAT

#---------------------------------------------------------------------------------
$(TARGET).nds	:	arm7/rpg/$(TARGET).arm7 arm9/rpg/$(TARGET).arm9
	ndstool	-c $(TARGET).nds -7 arm7/rpg/$(TARGET).arm7 -9 arm9/rpg/$(TARGET).arm9 -e9 0x02000450 -e7 0x02380000 -b $(TARGET).bmp "acekard;Real Play Gear;www.acekard.com"

#---------------------------------------------------------------------------------
arm7/rpg/$(TARGET).arm7:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
arm9/rpg/$(TARGET).arm9:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
_DS_MENU.DAT	:	$(TARGET)_r4.nds
	r4denc $(TARGET)_r4.nds _DS_MENU.DAT

#---------------------------------------------------------------------------------
$(TARGET)_r4.nds	:	arm7/r4/$(TARGET).arm7 arm9/r4/$(TARGET).arm9
	ndstool	-c $(TARGET)_r4.nds -7 arm7/r4/$(TARGET).arm7 -9 arm9/r4/$(TARGET).arm9 -e9 0x02000450 -e7 0x02380000 -b $(TARGET).bmp "wood r4"

#---------------------------------------------------------------------------------
arm7/r4/$(TARGET).arm7:
	$(MAKE) -C arm7 STORAGE=r4

#---------------------------------------------------------------------------------
arm9/r4/$(TARGET).arm9:
	$(MAKE) -C arm9 STORAGE=r4

#---------------------------------------------------------------------------------
ak2i/$(TARGET).nds	:	arm7/ak2i/$(TARGET).arm7 arm9/ak2i/$(TARGET).arm9
	mkdir -p ak2i
	ndstool	-c ak2i/$(TARGET).nds -7 arm7/ak2i/$(TARGET).arm7 -9 arm9/ak2i/$(TARGET).arm9 -e9 0x02000450 -e7 0x02380000 -b $(TARGET).bmp "wood ak2i"

#---------------------------------------------------------------------------------
_DSMENU.DAT	:	arm7/r4idsn/$(TARGET).arm7 arm9/r4idsn/$(TARGET).arm9
	ndstool	-c _DSMENU.DAT -7 arm7/r4idsn/$(TARGET).arm7 -9 arm9/r4idsn/$(TARGET).arm9 -e9 0x02000000 -e7 0x02380000 -b $(TARGET).bmp "wood r4idsn"

#---------------------------------------------------------------------------------
arm7/ak2i/$(TARGET).arm7:
	$(MAKE) -C arm7 STORAGE=ak2i

#---------------------------------------------------------------------------------
arm9/ak2i/$(TARGET).arm9:
	$(MAKE) -C arm9 STORAGE=ak2i

#---------------------------------------------------------------------------------
arm7/r4idsn/$(TARGET).arm7:
	$(MAKE) -C arm7 STORAGE=r4idsn

#---------------------------------------------------------------------------------
arm9/r4idsn/$(TARGET).arm9:
	$(MAKE) -C arm9 STORAGE=r4idsn

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	$(MAKE) -C arm9 STORAGE=r4 clean
	$(MAKE) -C arm7 STORAGE=r4 clean
	$(MAKE) -C arm9 STORAGE=ak2i clean
	$(MAKE) -C arm7 STORAGE=ak2i clean
	$(MAKE) -C arm9 STORAGE=r4idsn clean
	$(MAKE) -C arm7 STORAGE=r4idsn clean
	rm -f $(TARGET).nds _DS_MENU.DAT $(TARGET)_r4.nds _DSMENU.DAT
	rm -rf ak2i
