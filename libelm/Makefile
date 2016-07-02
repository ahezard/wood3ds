ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro)
endif

export TOPDIR	:=	$(CURDIR)

.PHONY: release clean all docs

all: release

#-------------------------------------------------------------------------------
release: lib
#-------------------------------------------------------------------------------
	$(MAKE) -C build BUILD=native || { exit 1;}
	$(MAKE) -C build BUILD=dldi || { exit 1;}

#-------------------------------------------------------------------------------
lib:
#-------------------------------------------------------------------------------
	mkdir lib

#-------------------------------------------------------------------------------
clean:
#-------------------------------------------------------------------------------
	@$(MAKE) -C build BUILD=native clean
	@$(MAKE) -C build BUILD=dldi clean
	@rm -rf lib
