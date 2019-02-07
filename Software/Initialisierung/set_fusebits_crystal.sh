echo ""
echo "setting Fusebits of atmega328p"
echo ""
echo "Clock source: external full swing crystal"
echo "Frequency   : 16MHz (depends on crystal)"
echo "EEPROM      : preserve through chip erase"
echo "Bootloader  : none (boot from 0x0000)"
echo ""
echo "WARNING: If no external crystal is present the mcu"
echo "         might not be programmed again!"
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
  -U lfuse:w:0xF7:m

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
