echo ""
echo "setting Fusebits of atmega328p"
echo ""
echo "Clock source: internal RC-oscillator"
echo "Frequency   : 8MHz"
echo "EEPROM      : preserve through chip erase"
echo "Bootloader  : none (boot from 0x0000)"
echo ""
echo "Are you really sure to change all fusebits ? (y/N)"
read -n 1 answer

if [ "$answer" != "y" ]; then
    echo "cancelled"
    return -1
    exit
fi

#// fuse bits (low)
avrdude -v \
  -p  atmega328p  \
  -P /dev/ttyACM0 \
  -c AVRISP2      \
  -B 50           \
  -U lfuse:w:0xE2:m

if [ $? -ne 0 ]; then
    echo "errors occured :-("
    return -1
    exit
fi


#// fuse bits (high)
avrdude -v \
  -p  atmega328p  \
  -P /dev/ttyACM0 \
  -c AVRISP2      \
  -B 50           \
  -U hfuse:w:0xD1:m

if [ $? -eq 0 ]; then
    echo "all done :-)"
else
    echo "errors occured :-("
fi
