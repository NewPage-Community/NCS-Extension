#!/bin/bash
set -eu

NCS_DIR="$(pwd)"
DEPS_DIR="$NCS_DIR/deps"
SMBRANCH="master"

if [[ ! -d "$DEPS_DIR" ]]; then
	mkdir -p "$DEPS_DIR"
fi

cd "$DEPS_DIR" || exit

# SourceMod
echo "Install SourceMod"
if [[ ! -d "sourcemod" ]]; then
	wget -O sourcemod.zip https://raw.new-page.xyz/sourcemod/sourcemod-6d2e0aa.zip
	unzip sourcemod.zip
fi

echo "Building extension"
cd "$NCS_DIR" || exit
make SMSDK="$DEPS_DIR/sourcemod" BOOST="$DEPS_DIR/boost"