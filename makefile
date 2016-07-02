CURDIR				:=	$(shell pwd)
export LIBIOR4		:=	$(CURDIR)/libior4
export LIBIORPG		:=	$(CURDIR)/libiorpg
export LIBIOAK2I	:=	$(CURDIR)/libioak2i
export LIBIOR4IDSN	:=	$(CURDIR)/libior4idsn
export LIBELM  		:=	$(CURDIR)/libelm
export LIBUNDS 		:=	$(CURDIR)/libunds
RELEASE			:=	akmenu_3in1

all: dldi akmenu4 fonts
	mkdir $(RELEASE)
	cp akmenu4/akmenu4.nds $(RELEASE)/
	mkdir $(RELEASE)/__rpg
	mkdir $(RELEASE)/__rpg/fonts
	mkdir $(RELEASE)/__rpg/language
	cp fonts/*.pcf $(RELEASE)/__rpg/fonts
	cp -r ui $(RELEASE)/__rpg/
	cp -r language/English $(RELEASE)/__rpg/language/English
	cp dldi/rpg_nand/rpg_nand.dldi $(RELEASE)/__rpg/
	cp dldi/rpg_sd/rpg_sd.dldi $(RELEASE)/__rpg/
	tar --exclude=\.svn -czf $(RELEASE).tar.gz $(RELEASE)
	rm -rf $(RELEASE)

world: all ak2i r4idsn

ak2i: akmenu4/ak2i/akmenu4.nds dldi/ak2i_sd/ak2i_sd.dldi

r4idsn: akmenu4/_DSMENU.DAT dldi/r4idsn_sd/r4idsn_sd.dldi

clean:
	cd libiorpg && $(MAKE) clean
	cd libior4 && $(MAKE) clean
	cd libior4idsn && $(MAKE) clean
	cd dldi/rpg_nand && $(MAKE) clean
	cd dldi/rpg_sd && $(MAKE) clean
	cd dldi/r4_sd && $(MAKE) clean
	cd dldi/r4idsn_sd && $(MAKE) clean
	cd libelm && $(MAKE) clean
	cd libunds && $(MAKE) clean
	cd akloader && $(MAKE) clean
	cd akmenu4 && $(MAKE) clean
	rm -f fonts/*.pcf
	rm -rf $(RELEASE)
	rm -f $(RELEASE).tar.gz

cleanx: clean
	cd libioak2i && $(MAKE) clean
	cd dldi/ak2i_sd && $(MAKE) clean

kill:
	rm -rf libioak2i
	rm -rf akloader/patches/include/ak2i
	rm -rf dldi/ak2i_sd
	rm -f akloader/arm9/source/ak2imaps.h
	rm -f libior4idsn/source/ioprivate.cpp

akloader/akloader_rpg.nds: libiorpg libelm libunds
	cd akloader && $(MAKE) rpg

akloader/akloader_r4.nds: libior4 libelm libunds
	cd akloader && $(MAKE) r4

akloader/akloader_ak2i.nds: libioak2i libelm libunds
	cd akloader && $(MAKE) ak2i

akloader/akloader_r4idsn.nds: libior4idsn libelm libunds
	cd akloader && $(MAKE) r4idsn

akmenu4: akmenu4/akmenu4.nds akmenu4/_DS_MENU.DAT

akmenu4/akmenu4.nds : akloader/akloader_rpg.nds
	cd akmenu4 && $(MAKE) rpg

akmenu4/_DS_MENU.DAT : akloader/akloader_r4.nds
	cd akmenu4 && $(MAKE) r4

akmenu4/ak2i/akmenu4.nds : akloader/akloader_ak2i.nds
	cd akmenu4 && $(MAKE) ak2i

akmenu4/_DSMENU.DAT : akloader/akloader_r4idsn.nds
	cd akmenu4 && $(MAKE) r4idsn

dldi: dldi/rpg_nand/rpg_nand.dldi dldi/rpg_sd/rpg_sd.dldi dldi/r4_sd/r4_sd.dldi

dldi/rpg_nand/rpg_nand.dldi:
	cd dldi/rpg_nand && $(MAKE)

dldi/rpg_sd/rpg_sd.dldi:
	cd dldi/rpg_sd && $(MAKE)

dldi/r4_sd/r4_sd.dldi:
	cd dldi/r4_sd && $(MAKE)

dldi/ak2i_sd/ak2i_sd.dldi:
	cd dldi/ak2i_sd && $(MAKE)

dldi/r4idsn_sd/r4idsn_sd.dldi:
	cd dldi/r4idsn_sd && $(MAKE)

libelm: libelm/lib/libelm.a

libelm/lib/libelm.a:
	cd libelm && $(MAKE)

libunds: libunds/lib/libunds7.a libunds/lib/libunds9.a

libunds/lib/libunds7.a libunds/lib/libunds9.a:
	cd libunds && $(MAKE)

libiorpg:
	cd libiorpg && $(MAKE)

libior4:
	cd libior4 && $(MAKE)

libioak2i:
	cd libioak2i && $(MAKE)

libior4idsn:
	cd libior4idsn && $(MAKE)

fonts: fonts/tahoma.pcf

fonts/tahoma.pcf:
	cd fonts && $(MAKE)

.PHONY: world all akloader akmenu4 dldi libelm libiorpg libior4 libioak2i libior4idsn libunds fonts clean cleanx kill ak2i r4idsn
