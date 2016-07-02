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

.PHONY: arm7/rpg/$(TARGET).arm7 arm9/rpg/$(TARGET).arm9
.PHONY: arm7/r4/$(TARGET).arm7 arm9/r4/$(TARGET).arm9
.PHONY: arm7/ak2i/$(TARGET).arm7 arm9/ak2i/$(TARGET).arm9
.PHONY: arm7/r4idsn/$(TARGET).arm7 arm9/r4idsn/$(TARGET).arm9
.PHONY: all rpg r4 ak2i r4idsn clean

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: rpg r4

#---------------------------------------------------------------------------------
# rpg target
#---------------------------------------------------------------------------------
rpg: $(TARGET)_rpg.nds

#---------------------------------------------------------------------------------
# r4 target
#---------------------------------------------------------------------------------
r4: $(TARGET)_r4.nds

#---------------------------------------------------------------------------------
# ak2i target
#---------------------------------------------------------------------------------
ak2i: $(TARGET)_ak2i.nds

#---------------------------------------------------------------------------------
# r4idsn target
#---------------------------------------------------------------------------------
r4idsn: $(TARGET)_r4idsn.nds

#---------------------------------------------------------------------------------
$(TARGET)_rpg.nds	:	arm7/rpg/$(TARGET).arm7 arm9/rpg/$(TARGET).arm9
	ndstool	-c $(TARGET)_rpg.nds -7 arm7/rpg/$(TARGET).arm7 -9 arm9/rpg/$(TARGET).arm9 -e9 0x023c0000 -e7 0x023fa000
#	ndstool -c $(TARGET).nds -9 $(TARGET).arm9 -7 $(TARGET).arm7 -b unknown_nds.bmp "new flash card;www.acekard.com" -r9 0x02200000 -e9 0x02204000 -e7 0x023fc000 -h 0x4000 -g ACEK AK ACEKARDBOOT -n 0x00586400 0x001808F8
	mkdir -p ../akmenu4/arm9/data/rpg
	cp arm7/rpg/$(TARGET).arm7 ../akmenu4/arm9/data/rpg/akloader_arm7.bin
	cp arm9/rpg/$(TARGET).arm9 ../akmenu4/arm9/data/rpg/akloader_arm9.bin

#---------------------------------------------------------------------------------
arm7/rpg/$(TARGET).arm7:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
arm9/rpg/$(TARGET).arm9:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
$(TARGET)_r4.nds	:	arm7/r4/$(TARGET).arm7 arm9/r4/$(TARGET).arm9
	ndstool	-c $(TARGET)_r4.nds -7 arm7/r4/$(TARGET).arm7 -9 arm9/r4/$(TARGET).arm9 -e9 0x023c0000 -e7 0x023fa000
	mkdir -p ../akmenu4/arm9/data/r4
	cp arm7/r4/$(TARGET).arm7 ../akmenu4/arm9/data/r4/akloader_arm7.bin
	cp arm9/r4/$(TARGET).arm9 ../akmenu4/arm9/data/r4/akloader_arm9.bin

#---------------------------------------------------------------------------------
arm7/r4/$(TARGET).arm7:
	$(MAKE) -C arm7 STORAGE=r4

#---------------------------------------------------------------------------------
arm9/r4/$(TARGET).arm9:
	$(MAKE) -C arm9 STORAGE=r4

#---------------------------------------------------------------------------------
$(TARGET)_ak2i.nds	:	arm7/ak2i/$(TARGET).arm7 arm9/ak2i/$(TARGET).arm9
	ndstool	-c $(TARGET)_ak2i.nds -7 arm7/ak2i/$(TARGET).arm7 -9 arm9/ak2i/$(TARGET).arm9 -e9 0x023c0000 -e7 0x023fa000
	mkdir -p ../akmenu4/arm9/data/ak2i
	cp arm7/ak2i/$(TARGET).arm7 ../akmenu4/arm9/data/ak2i/akloader_arm7.bin
	cp arm9/ak2i/$(TARGET).arm9 ../akmenu4/arm9/data/ak2i/akloader_arm9.bin

#---------------------------------------------------------------------------------
arm7/ak2i/$(TARGET).arm7:
	$(MAKE) -C arm7 STORAGE=ak2i

#---------------------------------------------------------------------------------
arm9/ak2i/$(TARGET).arm9:
	$(MAKE) -C arm9 STORAGE=ak2i

#---------------------------------------------------------------------------------
$(TARGET)_r4idsn.nds	:	arm7/r4idsn/$(TARGET).arm7 arm9/r4idsn/$(TARGET).arm9
	ndstool	-c $(TARGET)_r4idsn.nds -7 arm7/r4idsn/$(TARGET).arm7 -9 arm9/r4idsn/$(TARGET).arm9 -e9 0x023c0000 -e7 0x023fa000
	mkdir -p ../akmenu4/arm9/data/r4idsn
	cp arm7/r4idsn/$(TARGET).arm7 ../akmenu4/arm9/data/r4idsn/akloader_arm7.bin
	cp arm9/r4idsn/$(TARGET).arm9 ../akmenu4/arm9/data/r4idsn/akloader_arm9.bin

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
	rm -f $(TARGET)_rpg.nds $(TARGET)_r4.nds $(TARGET)_ak2i.nds $(TARGET)_r4idsn.nds
