#!/bin/bash

version=$(sed -nE "s/.*VERSION: (.*)/\1/p" LEDController.ino)

mkdir -p build
mkdir -p _buildcache

mv layout.h layout.h.orig

for file in layouts/*.h; do
  layoutname=$(basename ${file} .h)
  outname=${layoutname}_${version}.hex

  echo "----------------- ${layoutname} -----------------"
  cp $file layout.h
  arduino-cli compile --fqbn arduino:avr:nano --output-dir _buildcache
  mv _buildcache/LEDController.ino.hex build/${outname}
  echo -e "------------------${layoutname//?/-}------------------\n\n"
done

mv layout.h.orig layout.h

rm -rf _buildcache
