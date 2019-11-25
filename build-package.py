#!/usr/bin/env python3

# Copyright 2019 Praxinos, Inc. All Rights Reserved.

import argparse
from datetime import datetime
import itertools
import json
from pathlib import Path
import platform
import shutil
import subprocess
import sys

# from colorama import init, Fore, Back, Style
# init( autoreset=True )

#---

operating_system = platform.system().lower() # 'windows', 'darwin', 'linux', ...
if operating_system != 'windows':
    print( f'This platform is not supported: {operating_system}' )
    sys.exit( 5 )
   
#---
 
# Input uplugin file
input_path = Path.cwd()
uplugin_pathfile = input_path / 'Iliad.uplugin'

if not uplugin_pathfile.is_file():
    print( '' )
    print( f'uplugin file doesn\'t exist: {uplugin_pathfile}' )
    sys.exit( 10 )
    
print( f'Input uplugin file: {uplugin_pathfile}' )

# Output package directory
now = datetime.now()
output_path = input_path / '..' / 'Iliad-package' / now.strftime( '%Y%m%d-%H%M%S-' + operating_system ) / 'Iliad'
output_path.mkdir( parents=True, exist_ok=True )

print( f'Output path: {output_path}' )

#---

# Backup uplugin file
uplugin_backup_pathfile = input_path / 'Iliad.uplugin.backup'
if not uplugin_backup_pathfile.exists():
    shutil.copyfile( uplugin_pathfile, uplugin_backup_pathfile )

# Add platform specification
uplugin_data = {}
with uplugin_pathfile.open() as infile:
    uplugin_data = json.load( infile )
    
if not uplugin_data:
    print( '' )
    print( f'Empty uplugin_data: {uplugin_pathfile}' )
    sys.exit( 20 )

if operating_system == 'windows':
    uplugin_data['SupportedTargetPlatforms'] = [ 'Win64' ]

with uplugin_pathfile.open( 'w' ) as outfile:
    json.dump( uplugin_data, outfile )

#---

if operating_system == 'windows':
    uat = str( Path( 'C:\\' ) / 'Program Files' / 'Epic Games' / 'UE_4.23' / 'Engine' / 'Build' / 'BatchFiles' / 'RunUAT.bat' )
    args = [ 'BuildPlugin', '-Plugin=' + str( uplugin_pathfile ) + '', '-Package=' + str( output_path ) + '', '-CreateSubFolder', '-Rocket' ]
    
# Run packaging script
process = subprocess.run( [ uat ] + args )

shutil.copyfile( uplugin_backup_pathfile, uplugin_pathfile )
uplugin_backup_pathfile.unlink()

if process.returncode != 0:
    print( '' )
    print( f'An error occurs during processing...' )
    sys.exit( 30 )

#---

print( '' )

# Cleaning
intermediate = output_path / 'Intermediate'
print( f'Removing: {intermediate}' )
shutil.rmtree( intermediate, ignore_errors=True )

thirdparty = output_path / 'Source' / 'ThirdParty'
for entry in thirdparty.rglob( '*' ):
    if entry.is_dir() and entry.name == 'Debug':
        print( f'Removing: {entry}' )
        shutil.rmtree( entry, ignore_errors=True )

glm = output_path / 'Source' / 'ThirdParty' / 'ULIS' / 'Redist' / 'Include' / 'glm'
print( f'Removing: {glm}/*' )
shutil.rmtree( glm / '.git', ignore_errors=True )
shutil.rmtree( glm / 'doc', ignore_errors=True )
shutil.rmtree( glm / 'test', ignore_errors=True )
shutil.rmtree( glm / 'util', ignore_errors=True )
for entry in glm.glob( '*' ):
    if entry.is_file():
        print( f'Removing: {entry}' )
        entry.unlink()

runtime = output_path / 'Source' / 'Runtime'
editor = output_path / 'Source' / 'Editor'
for entry in itertools.chain( runtime.rglob( '*' ), editor.rglob( '*' ) ):
    if entry.is_dir() and entry.name == 'Private':
        print( f'Removing: {entry}' )
        shutil.rmtree( entry, ignore_errors=True )

