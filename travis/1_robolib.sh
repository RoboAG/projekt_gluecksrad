#!/bin/bash

### travis file for downloading and configuring "robolib"



echo ""
echo "### Downloading robolib"
cd ../../
git clone https://github.com/RoboAG/avr_robolib robolib
if [ $? -ne 0 ]; then exit -1; fi



echo ""
echo "### Configuring robolib"
cd robolib
make install_prerequisites
if [ $? -ne 0 ]; then exit -1; fi
