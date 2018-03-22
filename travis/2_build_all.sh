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

echo "# test_datenbus"
cd ${PATH_TIM}test_datenbus/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

echo "# test_helligkeit"
cd ${PATH_TIM}test_helligkeit/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi



echo ""
echo "## Richard"
PATH_RICHARD="${PATH_DEVEL}Richard/"

echo "# test_simple"
cd ${PATH_RICHARD}test_simple/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

echo "# test_datenbus"
cd ${PATH_RICHARD}test_datenbus/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi



echo ""
echo "## Alex"
PATH_ALEX="${PATH_DEVEL}Alex/"

echo "# buttons"
cd ${PATH_ALEX}buttons/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

echo "# eeprom"
cd ${PATH_ALEX}eeprom/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

echo "# LED_API_1"
cd ${PATH_ALEX}LED_API_1/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

echo "# LED_API_2"
cd ${PATH_ALEX}LED_API_2/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

echo "# timer"
cd ${PATH_ALEX}timer/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

#cd ${PATH_ALEX}test/
#robolib_all
#if [ $? -ne 0 ]; then exit -1; fi



echo ""
echo "## Peter"
PATH_PETER="${PATH_DEVEL}Peter/tests/"

echo "# leds1"
cd ${PATH_PETER}leds1/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

echo "# leds2"
cd ${PATH_PETER}leds2/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

echo "# cover"
cd ${PATH_PETER}cover/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi

echo "# random"
cd ${PATH_PETER}random/
robolib_all
if [ $? -ne 0 ]; then exit -1; fi
