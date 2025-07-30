#!/bin/bash
# IDDN.FR.001.060015.000.S.X.2019.000.00000
# Installers is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#---

# .command extension is to be able to double-click on this script on MacOSX

# Wrapper around Setup.py
# To have all logic inside 1 file

(
# On mac, when double clicking on .command file, the `pwd` (always?) returns the home directory
# So, go inside the repository directory (which should be `dirname`) with `cd`
cd "`dirname "$0"`"

python3 ./Setup.py
)
