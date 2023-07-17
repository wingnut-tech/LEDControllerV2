#!/bin/bash

if ! command -v arduino-cli >/dev/null 2>&1; then
  echo "Installing arduino-cli"
  mkdir -p build/arduino-cli
  if command -v "curl" >/dev/null 2>&1; then
    curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=./build/arduino-cli sh
  elif command -v "wget" >/dev/null 2>&1; then
    wget -q -O - https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=./build/arduino-cli sh
  else
    echo "You need curl or wget"
    exit 1
  fi
  PATH=./build/arduino-cli:$PATH
else
  echo "arduino-cli found at $(which arduino-cli)"
fi

if ! arduino-cli --config-file arduino-cli.yaml core list |grep -q rp2040:rp2040; then
  arduino-cli --config-file arduino-cli.yaml core update-index
fi

arduino-cli --config-file arduino-cli.yaml core install rp2040:rp2040@3.3.0 arduino:avr@1.8.6

sel=${1:-"both"}

if [[ $sel == "old" || $sel == "both" ]]; then
  arduino-cli --config-file arduino-cli.yaml compile -b arduino:avr:nano --output-dir build/old
fi

if [[ $sel == "new" || $sel == "both" ]]; then
  arduino-cli --config-file arduino-cli.yaml compile -b rp2040:rp2040:generic --output-dir build/new
fi
