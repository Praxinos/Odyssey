#!/usr/bin/env python3
# Copyright 2019-2020 Praxinos, Inc. All Rights Reserved.

import argparse
from datetime import datetime
import itertools
import json
from pathlib import Path
import platform
import re
import shutil
import subprocess
import sys

from colorama import init, Fore, Back, Style
init( autoreset=True )

#---

gVersionUE = '4.26'

gOperatingSystem = platform.system().lower() # 'windows', 'darwin', 'linux', ...
if gOperatingSystem != 'windows' and gOperatingSystem != 'darwin':
    print( Fore.RED + f'This platform is not supported: {gOperatingSystem}' )
    sys.exit( 5 )

#---

def GetArguments():
    global gOperatingSystem
    
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
                        help += '\n' + Fore.BLACK + Style.BRIGHT + '(default: %(default)s)' + Style.RESET_ALL
            return help

    default_input_path = Path.cwd().resolve()
    default_output_path = ( default_input_path / '..' / 'package' ).resolve()
    default_upload_path = Path( 'P:\\' ) / 'Praxinos' / 'Developpement' / 'Package'
    if gOperatingSystem == 'darwin':
        default_upload_path = Path.home() / 'pCloud Drive' / 'Praxinos' / 'Developpement' / 'Package'
    
    # Example with full parameters used
    # $(WORK)> 4.25\Iliad\build-package.py --input-dir "C:\Users\Mike\work\4.25\Iliad" --output-dir "C:\Users\Mike\work\4.25\package2" -u --upload-dir "C:\Users\Mike\work\4.25\upload2" -s test-script-parameters

    parser = argparse.ArgumentParser( description='Build package.', formatter_class=CustomArgumentDefaultsHelpFormatter )
    parser.add_argument( '-i', '--input-dir', default=f'{default_input_path}', help=f'the input path\nit must contains a uplugin file' )
    parser.add_argument( '-o', '--output-dir', default=f'{default_output_path}', help=f'the output path\nsuffix folders will be append to it' )
    parser.add_argument( '-u', '--upload', action="store_true", help=f'start uploading after building' )
    parser.add_argument( '-p', '--upload-dir', default=f'{default_upload_path}', help=f'the upload path\nsuffix folders will be append to it' )
    parser.add_argument( '-s', '--suffix', help=f'a suffix to the output directory name' )
    parser.add_argument( '-m', '--marketplace', action="store_true", help=f'package for the marketplace (without binaries)' )
    args = parser.parse_args()
    
    return args

#---

# Get input uplugin file
def ProcessArgumentForInputPath( iArgs ):
    input_path = Path( iArgs.input_dir ).resolve()

    uplugin_pathfiles = [ entry for entry in input_path.glob( '*.uplugin' ) if entry.is_file() ]
    if not uplugin_pathfiles:
        print( Fore.RED + f'no uplugin file in: {input_path}' )
        sys.exit( 10 )
        
    uplugin_pathfile = uplugin_pathfiles[0]
    print( Fore.GREEN + f'Input uplugin file: {uplugin_pathfile}' )

    plugin_name = uplugin_pathfile.stem

    # Check Content filenames validity

    content_path = input_path / 'Content'
    invalid_subpathfiles = []
    for pathfile in content_path.rglob( '*' ):
        subpathfile = pathfile.relative_to( content_path )
       # Same regex as https://www.unrealengine.com/en-US/marketplace-guidelines#271c
        if re.search( r'[^a-zA-Z0-9_]', str( subpathfile.stem ) ):
            invalid_subpathfiles.append( subpathfile )

    if invalid_subpathfiles:
        for invalid_subpathfile in invalid_subpathfiles:
            print( f'Invalid characters: {content_path} {Fore.RED}{invalid_subpathfile}' )
            
        sys.exit( 12 )
    
    return uplugin_pathfile, plugin_name

# Get intermediate folders (with date/version/...)
def GetIntermediateFolders( iArgs, iUPluginPathFile, iPluginName ):
    global gVersionUE
    global gOperatingSystem
    
    uplugin_data = {}
    with iUPluginPathFile.open() as infile:
        uplugin_data = json.load( infile )
        
    now = datetime.now()

    date_folder = []
    date_folder.append( now.strftime( '%Y%m%d.%H%M%S' ) )
    date_folder.append( gVersionUE )
    date_folder.append( uplugin_data["VersionName"] )
    date_folder.append( 'beta' if uplugin_data['IsBetaVersion'] else '' )
    if not iArgs.marketplace:
        date_folder.append( gOperatingSystem )
    date_folder.append( iArgs.suffix )
    date_folder = list( filter( None, date_folder ) )
    date_folder = '-'.join( date_folder )

    main_folder = []
    main_folder.append( iPluginName )
    main_folder.append( gVersionUE )
    main_folder.append( 'marketplace' if iArgs.marketplace else '' )
    main_folder = list( filter( None, main_folder ) )
    main_folder = '-'.join( main_folder )

    return Path( main_folder ) / date_folder / iPluginName

# Get the name of the zip (with version/...)
def GetZipName( iArgs, iUPluginPathFile, iPluginName ):
    global gOperatingSystem
    
    uplugin_data = {}
    with iUPluginPathFile.open() as infile:
        uplugin_data = json.load( infile )
        
    zip_name = []
    zip_name.append( iPluginName )
    zip_name.append( uplugin_data["VersionName"] )
    zip_name.append( 'beta' if uplugin_data['IsBetaVersion'] else '' )
    if not iArgs.marketplace:
        zip_name.append( gOperatingSystem )
    zip_name = list( filter( None, zip_name ) )
    zip_name = '-'.join( zip_name )

    return zip_name

# Get output package directory
def ProcessArgumentForOutputPath( iArgs, iIntermediateFolders ):
    output_path = Path( iArgs.output_dir ).resolve()

    output_path = ( output_path / iIntermediateFolders )
    output_path.mkdir( parents=True, exist_ok=True )

    print( Fore.GREEN + f'Output path: {output_path}' )
    
    return output_path

# Get upload package directory
def ProcessArgumentForUploadPath( iArgs, iIntermediateFolders ):
    if not iArgs.upload:
        print( Fore.GREEN + f'NO upload' )
        return None
        
    upload_path = Path( iArgs.upload_dir ).resolve()
        
    upload_path.mkdir( parents=True, exist_ok=True )
    upload_path = upload_path / iIntermediateFolders

    print( Fore.GREEN + f'Upload path: {upload_path}' )

    return upload_path

# Print info for marketplace
def ProcessArgumentForMarketplace( iArgs ):
    if not iArgs.marketplace:
        return
        
    print( Fore.GREEN + f'Build for marketplace' )

#---

# Process the compilation
def Build( iUPluginPathFile, iOutputPath ):
    global gVersionUE

    # Backup uplugin file
    uplugin_backup_pathfile = iUPluginPathFile.with_suffix( iUPluginPathFile.suffix + '.backup' )
    if not uplugin_backup_pathfile.exists(): # Otherwise, an already modified file will be copied
        shutil.copyfile( iUPluginPathFile, uplugin_backup_pathfile )

    # Add platform specification
    uplugin_data = {}
    with iUPluginPathFile.open() as infile:
        uplugin_data = json.load( infile )
        
    if not uplugin_data:
        print( Fore.RED + f'Empty uplugin_data: {iUPluginPathFile}' )
        sys.exit( 20 )

    for module in uplugin_data['Modules']:
        if gOperatingSystem == 'windows':
            module['WhitelistPlatforms'] = [ 'Win64' ] # https://www.unrealengine.com/en-US/marketplace-guidelines#261b
        elif gOperatingSystem == 'darwin':
            module['WhitelistPlatforms'] = [ 'Mac' ]

    with iUPluginPathFile.open( 'w' ) as outfile:
        json.dump( uplugin_data, outfile )

    #---

    if gOperatingSystem == 'windows':
        uat = [ str( Path( 'C:\\' ) / 'Program Files' / 'Epic Games' / f'UE_{gVersionUE}' / 'Engine' / 'Build' / 'BatchFiles' / 'RunUAT.bat' ) ]
    elif gOperatingSystem == 'darwin':
        uat = [ str( Path( '/' ) / 'Users' / 'Shared' / 'Epic Games' / f'UE_{gVersionUE}' / 'Engine' / 'Build' / 'BatchFiles' / 'RunUAT.sh' ) ]
    uat_args = [ 'BuildPlugin', '-Plugin=' + str( iUPluginPathFile ) + '', '-Package=' + str( iOutputPath ) + '', '-CreateSubFolder', '-Rocket' ]
        
    # Run packaging script
    process = subprocess.run( uat + uat_args )

    #---

    shutil.copyfile( uplugin_backup_pathfile, iUPluginPathFile ) # Must be done before any sys.exit()
    uplugin_backup_pathfile.unlink()

    if process.returncode != 0:
        print( Fore.RED + f'An error occurs during processing...' )
        sys.exit( 30 )

#---

# Add platform specification (all platforms) for marketplace
def PostBuildFixPlatforms( iArgs, iOutputPath ):
    if not iArgs.marketplace:
        return

    uplugin_pathfiles = [ entry for entry in iOutputPath.glob( '*.uplugin' ) if entry.is_file() ]
    if not uplugin_pathfiles:
        print( Fore.RED + f'no uplugin file in: {iOutputPath}' )
        sys.exit( 40 )

    uplugin_pathfile = uplugin_pathfiles[0]
        
    uplugin_data = {}
    with uplugin_pathfile.open() as infile:
        uplugin_data = json.load( infile )
        
    for module in uplugin_data['Modules']:
        module['WhitelistPlatforms'] = [ 'Win64', 'Mac' ] # https://www.unrealengine.com/en-US/marketplace-guidelines#261b

    with uplugin_pathfile.open( 'w' ) as outfile:
        json.dump( uplugin_data, outfile )

# Cleaning
def Clean( iArgs, iOutputPath ):
    # Remove binaries only for marketplace, otherwise it's for internal testing and binaries are needed to not have to compile the plugin again
    if iArgs.marketplace:
        binaries = iOutputPath / 'Binaries'
        print( Fore.GREEN + f'Removing: {binaries}' )
        shutil.rmtree( binaries, ignore_errors=True )

    intermediate = iOutputPath / 'Intermediate'
    print( Fore.GREEN + f'Removing: {intermediate}' )
    shutil.rmtree( intermediate, ignore_errors=True )

#---

# Zipping
def Zip( iArgs, iPathToZip, iZipName ):
    folder_to_zip = iPathToZip
    pathfile_zip = iPathToZip.parent / iZipName # extension added by make_archive
    print( Fore.GREEN + f'Zipping: {folder_to_zip} -> {pathfile_zip}.zip' )
    pathfile_zip = shutil.make_archive( pathfile_zip, 'zip', folder_to_zip.parents[0], folder_to_zip.name )

#---

# Uploading
def Upload( iArgs, iOutputPath, iUploadPath ):
    if not iArgs.upload:
        return

    src_path = iOutputPath.parents[0]
    dst_path = iUploadPath.parents[0]
    print( Fore.GREEN + f'Copying/Uploading: {src_path} -> {dst_path}' )
    shutil.copytree( src_path, dst_path )
    
#---
#---
#---

args = GetArguments()

uplugin_pathfile, plugin_name   = ProcessArgumentForInputPath( args )
intermediate_folders            = GetIntermediateFolders( args, uplugin_pathfile, plugin_name )
zip_name                        = GetZipName( args, uplugin_pathfile, plugin_name )
output_path                     = ProcessArgumentForOutputPath( args, intermediate_folders )
upload_path                     = ProcessArgumentForUploadPath( args, intermediate_folders )
ProcessArgumentForMarketplace( args )

#---

Build( uplugin_pathfile, output_path )

#---

PostBuildFixPlatforms( args, output_path )                              # For marketplace package, the uplugin file must know all the os supported

#---

Clean( args, output_path )

Zip( args, output_path, zip_name )

Upload( args, output_path, upload_path )
