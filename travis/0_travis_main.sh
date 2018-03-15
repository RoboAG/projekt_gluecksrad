#!/bin/bash

### main travis file



PWD_temp=$(pwd)

. 1_robolib.sh

cd ${PWD_temp}
. 2_build_all.sh

cd ${PWD_temp}
. 3_final.sh
