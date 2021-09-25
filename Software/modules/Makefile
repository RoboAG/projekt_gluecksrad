###############################################################################
#                                                                             #
# Makefile (LowLevel2)                                                        #
# ====================                                                        #
#                                                                             #
# Version: 1.0.3                                                              #
# Date   : 25.09.21                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# Source code was taken from:                                                 #
#   https://github.com/RoboSAX/avr_spielfeld                                  #
#                                                                             #
# See also:                                                                   #
#   https://github.com/RoboAG/projekt_gluecksrad                              #
###############################################################################

CURRENTDIR = $(notdir $(patsubst %/,%, \
  $(dir $(abspath $(lastword $(MAKEFILE_LIST))))))

SUBDIRS = $(patsubst %/.,%,$(wildcard */.))

###############################################################################
# define phony targets for make commands
.PHONY: all all_init  $(SUBDIRS)  clean

all: all_init $(SUBDIRS)

all_init:
	@echo ""
	@echo ""
	@echo "## building $(CURRENTDIR) ##"


$(SUBDIRS):
	@echo ""
	@echo "# building $@ #"
	$(MAKE) -C $@ -f "$(ROBOLIB_MAKEFILE)"


clean:
	@echo
	@echo "# clean $(CURRENTDIR) #"
	@for dir in $(SUBDIRS); do \
	    $(MAKE) -C $$dir -f "$(ROBOLIB_MAKEFILE)" clean; \
	done

###############################################################################
