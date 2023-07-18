#!/bin/bash

if ! command -v arduino-cli >/dev/null 2>&1; then
  echo "Installing arduino-cli"
  mkdir -p build/arduino-cli
  if command -v "curl" >/dev/null 2>&1; then
    curl_cmd="curl -fsSL"
  elif command -v "wget" >/dev/null 2>&1; then
    curl_cmd="wget -q -O -"
  else
    echo "You need curl or wget"
    exit 1
  fi
  $curl_cmd https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=./build/arduino-cli sh
  PATH=./build/arduino-cli:$PATH
else
  echo "arduino-cli found at $(which arduino-cli)"
fi

arduino_cli_cmd="arduino-cli --config-file arduino-cli.yaml"

if ! $arduino_cli_cmd core list |grep -q rp2040:rp2040; then
  $arduino_cli_cmd core update-index
fi

$arduino_cli_cmd core install rp2040:rp2040@3.3.0 arduino:avr@1.8.6

sel=${1:-"both"}

ver=$(awk '/VERSION:/ {print $3}' LEDController/LEDController.ino)

if [[ $sel == "old" || $sel == "both" ]]; then
  $arduino_cli_cmd compile -b arduino:avr:nano --output-dir build/rev1 LEDController
  cp build/rev1/LEDController.ino.hex build/firmware_${ver}.hex
fi

if [[ $sel == "new" || $sel == "both" ]]; then
  $arduino_cli_cmd compile -b rp2040:rp2040:generic --output-dir build/rev2 LEDController
  cp build/rev2/LEDController.ino.uf2 build/firmware_${ver}.uf2
fi