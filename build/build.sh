#!/bin/bash
set -eu

NCS_DIR="$(pwd)"
DEPS_DIR="$NCS_DIR/deps"
SMBRANCH="master"

if [[ ! -d "$DEPS_DIR" ]]; then
	mkdir -p "$DEPS_DIR"
fi

cd "$DEPS_DIR" || exit

# Boost
echo "Install Boost"
if [[ ! -d "boost" ]]; then
	if [[ ! -d "boost_1_69_0" ]]; then
		tar -xzf boost_1_69_0.tar.gz
	fi
	cd boost_1_69_0
	./bootstrap.sh --with-libraries=date_time,random,regex,system --with-toolset=gcc
	./b2 install --prefix="$DEPS_DIR/boost"
	cd "$NCS_DIR" || exit
fi

# SourceMod
echo "Install SourceMod"
if [[ ! -d "sourcemod" ]]; then
	tar -xzf sourcemod.tar.gz
fi

echo "Building extension"
cd "$NCS_DIR" || exit
make SMSDK="$DEPS_DIR/sourcemod" BOOST="$DEPS_DIR/boost"