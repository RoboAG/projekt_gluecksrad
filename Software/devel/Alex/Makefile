###############################################################################
#                                                                             #
# devel/.../Makefile                                                          #
# ==================                                                          #
#                                                                             #
# Version: 1.0.2                                                              #
# Date   : 07.02.18                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# Source code was taken from:                                                 #
#   https://github.com/RoboSAX/avr_spielfeld                                  #
#                                                                             #
# See also:                                                                   #
#   https://github.com/RoboAG/projekt_gluecksrad                              #
###############################################################################

CURRENTDIR = Alex

SUBDIRS = buttons timer eeprom  \
          LED_API_2 LED_API_1 \
          Fruehlingsmarkt_03_23
# unfixed build error for test

###############################################################################
# define phony targets for make commands
.PHONY: all all_init  $(SUBDIRS)  clean

all: all_init $(SUBDIRS)

all_init:
	@echo "# building $(CURRENTDIR) #"


$(SUBDIRS):
	$(MAKE) -C $@ -f "$(ROBOLIB_MAKEFILE)"


clean:
	@echo
	@echo "# clean $(CURRENTDIR) #"
	@for dir in $(SUBDIRS); do \
	    $(MAKE) -C $$dir -f "$(ROBOLIB_MAKEFILE)" clean; \
	done

###############################################################################
