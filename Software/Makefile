###############################################################################
#                                                                             #
# Makefile                                                                    #
# ========                                                                    #
#                                                                             #
# Version: 1.2.5                                                              #
# Date   : 07.02.18                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# Source code was taken from:                                                 #
#   https://github.com/RoboSAX/avr_spielfeld                                  #
#                                                                             #
# See also:                                                                   #
#   https://github.com/RoboAG/projekt_gluecksrad                              #
###############################################################################

NAME_GIT_THIS=gluecksrad

SUBDIRS = Fruehlingsmarkt_2018 devel

###############################################################################
# define phony targets for make commands
.PHONY: all all_init warn  $(SUBDIRS) clean \
        update status push pull


# build specific commands
all: all_init $(SUBDIRS)

all_init:
	@echo
	@echo "### building $(NAME_GIT_THIS) ###"

warn:
	@echo
	@echo "###### reducing output to warnings ######"
	$(MAKE) | grep -A 3 -B 3 -i warn; dummy=$?

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	@echo
	@echo "### clean $(NAME_GIT_THIS) ###"
	@for dir in $(SUBDIRS); do \
	    $(MAKE) -C $$dir clean; \
	done


# git specific commands
update: pull
	$(MAKE) -C bin/ update

status:
	@echo ""
	@echo "### status of $(NAME_GIT_THIS) ###"
	@git status --untracked-files

push:
	@echo ""
	@echo "### pushing of $(NAME_GIT_THIS) ###"
	git push

pull:
	@echo ""
	@echo "### update $(NAME_GIT_THIS) ###"
	git pull


###############################################################################
