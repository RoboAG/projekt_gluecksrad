#!/bin/bash

### travis file for final output



if [ $? -ne 0 ]; then
    echo ""
    echo "### errors occurred  :-("
    return -1
else
    echo ""
    echo "### all done :-)"
fi
