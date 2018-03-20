#!/bin/bash

### travis file for building all software modules



echo ""
echo "### Sourcing robolib"
. ../../robolib/scripts/bashrc.sh



echo ""
echo "### Building"
cd ../Software/
PATH_SOFTWARE="$(pwd)/"
PATH_DEVEL="${PATH_SOFTWARE}devel/"



echo ""
echo "## Tim"
PATH_TIM="${PATH_DEVEL}Tim_atmeg88p_smd/"

cd ${PATH_TIM}test_datenbus/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

cd ${PATH_TIM}test_helligkeit/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi



echo ""
echo "## Richard"
PATH_RICHARD="${PATH_DEVEL}Richard_atmega328p_tht/"

cd ${PATH_RICHARD}test_simple/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

cd ${PATH_RICHARD}test_datenbus/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi



echo ""
echo "## Alex"
PATH_RICHARD="${PATH_DEVEL}Alex_atmega328p_tht/"

cd ${PATH_RICHARD}buttons/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

cd ${PATH_RICHARD}eeprom/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

cd ${PATH_RICHARD}LED_API_1/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

cd ${PATH_RICHARD}LED_API_2/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

cd ${PATH_RICHARD}test/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

