#!/bin/bash

set -e

if ! ls visuald64*; then
    echo "visuald64 executable missing. Build it first."
    exit 1
fi

PATH=../:$PATH

mkdir -p tmp
pushd tmp

wget --quiet ftp://arnold.c64.org/pub/games/i/International_Karate_copy3.System_3.zip
unzip International_Karate_copy3.System_3.zip
visuald64 INTERNAT_KARATE.d64
convert INTERNAT_KARATE.bmp INTERNAT_KARATE.png

wget --quiet ftp://arnold.c64.org/pub/games/i/Impossible_Mission.Epyx.+5pd-REM.zip
unzip Impossible_Mission.Epyx.+5pd-REM.zip
visuald64 Impmission.d64
convert Impmission.bmp Impmission.png

popd
