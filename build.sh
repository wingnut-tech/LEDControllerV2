#!/bin/bash

version=$(sed -nE "s/.*VERSION: (.*)/\1/p" LEDController.ino)

mkdir -p build
mkdir -p _buildcache

build() {
  if [[ ${2} == "" ]]; then
    layoutname=$(basename ${1} .h)
  else
    layoutname=${2}
  fi
  outname=${layoutname}_v${version}.hex
  cachedir=_buildcache/${layoutname}

  mkdir -p ${cachedir}

  cp LEDController.ino ${cachedir}/${layoutname}.ino
  cp -r LEDController.h src ${cachedir}
  cp ${1} ${cachedir}/layout.h
  
  cd ${cachedir}

  echo "Queueing up ${layoutname}..."

  out=$(arduino-cli compile --fqbn arduino:avr:nano --output-dir _build)
  mv _build/${layoutname}.ino.hex ../../build/${outname}

  echo -e "--- ${layoutname} ---\n${out}\n\n"
}

if [[ $1 != "--all" ]]; then
  build layout.h default
else
  for file in layouts/*.h; do
    build $file &
  done
fi

wait
rm -rf _buildcache