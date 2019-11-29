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

input_path = Path.cwd().resolve()
output_path = ( input_path / '..' / 'package' ).resolve()
   
parser = argparse.ArgumentParser( description='Build package.', formatter_class=CustomArgumentDefaultsHelpFormatter )
parser.add_argument( '-i', '--input', default=f'{input_path}', help=f'The input path' )
parser.add_argument( '-o', '--output', default=f'{output_path}', help=f'The output path\n\'date-time/plugin-name\' folders will be append to it' )
args = parser.parse_args()

operating_system = platform.system().lower() # 'windows', 'darwin', 'linux', ...
if operating_system != 'windows':
    print( Fore.RED + f'This platform is not supported: {operating_system}' )
    sys.exit( 5 )
   
#---
 
# Input uplugin file
if args.input:
    input_path = Path( args.input ).resolve()

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
if args.output:
    output_path = Path( args.output ).resolve()

now = datetime.now()
output_path = ( output_path / now.strftime( '%Y%m%d-%H%M%S-' + operating_system ) / plugin_name )
output_path.mkdir( parents=True, exist_ok=True )

print( Fore.GREEN + f'Output path: {output_path}' )

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

if operating_system == 'windows':
    uplugin_data['SupportedTargetPlatforms'] = [ 'Win64' ]

with uplugin_pathfile.open( 'w' ) as outfile:
    json.dump( uplugin_data, outfile )

#---

if operating_system == 'windows':
    uat = [ str( Path( 'C:\\' ) / 'Program Files' / 'Epic Games' / 'UE_4.23' / 'Engine' / 'Build' / 'BatchFiles' / 'RunUAT.bat' ) ]
    args = [ 'BuildPlugin', '-Plugin=' + str( uplugin_pathfile ) + '', '-Package=' + str( output_path ) + '', '-CreateSubFolder', '-Rocket' ]
    
# Run packaging script
process = subprocess.run( uat + args )

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

# thirdparty = output_path / 'Source' / 'ThirdParty'
# for entry in thirdparty.rglob( '*' ):
    # if entry.is_dir() and entry.name == 'Debug':
        # print( f'Removing: {entry}' )
        # shutil.rmtree( entry, ignore_errors=True )

# glm = output_path / 'Source' / 'ThirdParty' / 'ULIS' / 'Redist' / 'Include' / 'glm'
# print( f'Removing: {glm}/*' )
# shutil.rmtree( glm / '.git', ignore_errors=True )
# shutil.rmtree( glm / 'doc', ignore_errors=True )
# shutil.rmtree( glm / 'test', ignore_errors=True )
# shutil.rmtree( glm / 'util', ignore_errors=True )
# for entry in glm.glob( '*' ):
    # if entry.is_file():
        # print( f'Removing: {entry}' )
        # entry.unlink()

# runtime = output_path / 'Source' / 'Runtime'
# editor = output_path / 'Source' / 'Editor'
# for entry in itertools.chain( runtime.rglob( '*' ), editor.rglob( '*' ) ):
    # if entry.is_dir() and entry.name == 'Private':
        # print( f'Removing: {entry}' )
        # shutil.rmtree( entry, ignore_errors=True )

