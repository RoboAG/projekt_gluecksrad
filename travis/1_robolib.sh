#!/bin/bash

### travis file for downloading and configuring "robolib"

echo "### Downloading robolib"
cd ../../
git clone https://github.com/RoboAG/avr_robolib robolib

echo "### Configuring robolib"
cd robolib
make install_prerequisites
