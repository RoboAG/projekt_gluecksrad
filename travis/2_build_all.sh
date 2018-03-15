#!/bin/bash

### travis file for building all software modules



echo "### Sourcing robolib"
. ../../robolib/scripts/bashrc.sh



echo "### Building"
cd ../Software/
PATH_SOFTWARE="$(pwd)/"
PATH_DEVEL="${PATH_SOFTWARE}devel/"



echo "## Tim"
PATH_TIM="${PATH_DEVEL}Tim_atmeg88p_smd/"

cd ${PATH_TIM}test_datenbus/
robolib_all

cd ${PATH_TIM}test_helligkeit/
robolib_all



echo "## Richard"
PATH_RICHARD="${PATH_DEVEL}Richard_atmega328p_tht/"

cd ${PATH_RICHARD}test_simple/
robolib_all

cd ${PATH_RICHARD}test_datenbus/
robolib_all



echo "## Alex"
PATH_RICHARD="${PATH_DEVEL}Alex_atmega328p_tht/"

cd ${PATH_RICHARD}test_datenbus/
robolib_all
