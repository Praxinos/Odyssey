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

from colorama import init, Fore, Back, Style
init( autoreset=True )

#---

class CustomArgumentDefaultsHelpFormatter( argparse.RawTextHelpFormatter ):
    """Help message formatter which adds default values to argument help.

    Only the name of this class is considered a public API. All the methods
    provided by the class are considered an implementation detail.
    """

    # From ArgumentDefaultsHelpFormatter, add default value on a new line
    def _get_help_string(self, action):
        help = action.help
        if '%(default)' not in action.help:
            if action.default is not argparse.SUPPRESS:
                defaulting_nargs = [argparse.OPTIONAL, argparse.ZERO_OR_MORE]
                if action.option_strings or action.nargs in defaulting_nargs:
                    help += '\n(default: %(default)s)'
        return help
        
#---

operating_system = platform.system().lower() # 'windows', 'darwin', 'linux', ...
if operating_system != 'windows':
    print( Fore.RED + f'This platform is not supported: {operating_system}' )
    sys.exit( 5 )

input_path = Path.cwd().resolve()
output_path = ( input_path / '..' / 'package' ).resolve()
upload_path = Path( 'P:\\' ) / 'Praxinos' / 'Developpement' / 'Package' / 'Iliad'
   
parser = argparse.ArgumentParser( description='Build package.', formatter_class=CustomArgumentDefaultsHelpFormatter )
parser.add_argument( '-i', '--input-dir', default=f'{input_path}', help=f'the input path\nit must contains a uplugin file' )
parser.add_argument( '-o', '--output-dir', default=f'{output_path}', help=f'the output path\n\'date-time/plugin-name\' folders will be append to it' )
parser.add_argument( '-u', '--upload', action="store_true", help=f'start uploading after building' )
parser.add_argument( '-p', '--upload-dir', default=f'{upload_path}', help=f'the upload path' )
args = parser.parse_args()

#---
 
# Input uplugin file
if args.input_dir:
    input_path = Path( args.input_dir ).resolve()

uplugin_pathfiles = list( input_path.glob( '*.uplugin' ) )
if not uplugin_pathfiles:
    print( Fore.RED + f'uplugin file doesn\'t exist in: {input_path}' )
    sys.exit( 10 )

uplugin_pathfile = uplugin_pathfiles[0]
if not uplugin_pathfile.is_file():
    print( Fore.RED + f'uplugin file is not a file: {uplugin_pathfile}' )
    sys.exit( 12 )
    
print( Fore.GREEN + f'Input uplugin file: {uplugin_pathfile}' )

plugin_name = uplugin_pathfile.stem

# Output package directory
if args.output_dir:
    output_path = Path( args.output_dir ).resolve()

now = datetime.now()
date_folder = now.strftime( '%Y%m%d-%H%M%S-' + operating_system )
output_path = ( output_path / date_folder / plugin_name )
output_path.mkdir( parents=True, exist_ok=True )

print( Fore.GREEN + f'Output path: {output_path}' )

# Upload package directory
if args.upload:
    if args.upload_dir:
        upload_path = Path( args.upload_dir ).resolve()
        
    upload_path.mkdir( parents=True, exist_ok=True )

    print( Fore.GREEN + f'Upload path: {upload_path}' )
else:
    print( Fore.GREEN + f'NO upload' )

#---

# Backup uplugin file
uplugin_backup_pathfile = uplugin_pathfile.with_suffix( uplugin_pathfile.suffix + '.backup' )
if not uplugin_backup_pathfile.exists(): # Otherwise, an already modified file will be copied
    shutil.copyfile( uplugin_pathfile, uplugin_backup_pathfile )

# Add platform specification
uplugin_data = {}
with uplugin_pathfile.open() as infile:
    uplugin_data = json.load( infile )
    
if not uplugin_data:
    print( Fore.RED + f'Empty uplugin_data: {uplugin_pathfile}' )
    sys.exit( 20 )

for module in uplugin_data['Modules']:
    if operating_system == 'windows':
        module['WhitelistPlatforms'] = [ 'Win64' ] # https://www.unrealengine.com/en-US/marketplace-guidelines#261b

with uplugin_pathfile.open( 'w' ) as outfile:
    json.dump( uplugin_data, outfile )

#---

if operating_system == 'windows':
    uat = [ str( Path( 'C:\\' ) / 'Program Files' / 'Epic Games' / 'UE_4.23' / 'Engine' / 'Build' / 'BatchFiles' / 'RunUAT.bat' ) ]
    uat_args = [ 'BuildPlugin', '-Plugin=' + str( uplugin_pathfile ) + '', '-Package=' + str( output_path ) + '', '-CreateSubFolder', '-Rocket' ]
    
# Run packaging script
process = subprocess.run( uat + uat_args )

shutil.copyfile( uplugin_backup_pathfile, uplugin_pathfile )
uplugin_backup_pathfile.unlink()

if process.returncode != 0:
    print( Fore.RED + f'An error occurs during processing...' )
    sys.exit( 30 )

#---

# Cleaning
intermediate = output_path / 'Intermediate'
print( Fore.GREEN + f'Removing: {intermediate}' )
shutil.rmtree( intermediate, ignore_errors=True )

source = output_path / 'Source'
print( Fore.GREEN + f'Removing: {source}' )
shutil.rmtree( source, ignore_errors=True )

#---

# Uploading
if args.upload:
    src_path = ( output_path / '..' ).resolve() # '..' to move to the directory with the date
    dst_path = ( upload_path / date_folder ).resolve()
    print( Fore.GREEN + f'Copying/Uploading: {src_path} -> {dst_path}' )
    shutil.copytree( src_path, dst_path )
