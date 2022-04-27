#!/usr/bin/env python3
# IDDN.FR.001.220036.001.S.P.2021.000.00000
# EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

import argparse
from datetime import datetime
from enum import Enum
import itertools
import json
from pathlib import Path
import platform
import re
import shutil
import subprocess
import sys

from pygit2 import Repository
from colorama import init, Fore, Back, Style
init( autoreset=True )

#---

gOperatingSystem = platform.system().lower() # 'windows', 'darwin', 'linux', ...
if gOperatingSystem != 'windows' and gOperatingSystem != 'darwin':
    print( Fore.RED + f'This platform is not supported: {gOperatingSystem}' )
    sys.exit( 5 )

class eTarget(Enum):
    kDev = 'dev'
    kBeta = 'beta'
    kMarketplace = 'marketplace'

    def __str__(self):
        return self.value

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
    # $(WORK)> 4.27\Epos\build-package.py --input-dir "C:\Users\Mike\work\4.27\Epos" --output-dir "C:\Users\Mike\work\4.27\package2" -u --upload-dir "C:\Users\Mike\work\4.27\upload2" -s test-script-parameters

    parser = argparse.ArgumentParser( description='Build package.', formatter_class=CustomArgumentDefaultsHelpFormatter )
    parser.add_argument( '-t', '--target', type=eTarget, choices=eTarget, required=True, help=f'package for the specific target:\n- dev: binaries + sources\n- beta: binaries only\n- marketplace: sources only' )
    parser.add_argument( '-i', '--input-dir', default=f'{default_input_path}', help=f'the input path\nit must contains a uplugin file' )
    parser.add_argument( '-o', '--output-dir', default=f'{default_output_path}', help=f'the output path\nsuffix folders will be append to it' )
    parser.add_argument( '-u', '--upload', action="store_true", help=f'start uploading after building' )
    parser.add_argument( '-p', '--upload-dir', default=f'{default_upload_path}', help=f'the upload path\nsuffix folders will be append to it' )
    parser.add_argument( '-s', '--suffix', help=f'a suffix to the output directory name' )
    parser.add_argument( '-e', '--ue-version', help=f'the version of the ue engine: 4.xx or 5.xx\nonly useful if it can\'t be deduce from the current branch' )
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
    print( Fore.GREEN + f'{"Input uplugin file":20}: {uplugin_pathfile}' )

    plugin_name = uplugin_pathfile.stem

    content_path = input_path / 'Content'

    # Remove Mac .DS_Store files ...

    for pathfile in content_path.rglob( '*' ):
        if pathfile.name == '.DS_Store':
            pathfile.unlink()

    # Check Content filenames validity

    invalid_subpathfiles = []
    for pathfile in content_path.rglob( '*' ):
        subpathfile = pathfile.relative_to( content_path )
       # Same regex as https://www.unrealengine.com/en-US/marketplace-guidelines#271c
        if re.search( r'[^a-zA-Z0-9_]', str( subpathfile.stem ) ):
            invalid_subpathfiles.append( subpathfile )

    if invalid_subpathfiles:
        for invalid_subpathfile in invalid_subpathfiles:
            print( Fore.RED + f'Invalid characters: {content_path} {Fore.RED}{invalid_subpathfile}' )

        sys.exit( 12 )

    return uplugin_pathfile, plugin_name

# Get ue version
def GetUEVersion( iArgs, iUPluginPathFile ):
    repo = Repository( iUPluginPathFile.resolve().parent )

    current_branch = repo.head.shorthand
    print( Fore.GREEN + f'{"Current branch":20}: {current_branch}' )

    match = re.match( "(dev|release)-(?P<version_major>[0-9])\.(?P<version_minor>[0-9]+)", current_branch )

    # Can't deduce ue version from branch name, so try to get it from argument
    if match is None:
        if iArgs.ue_version is None:
            print( Fore.RED + f'Can\'t determine ue version from current branch name "{current_branch}", add ue version as argument: -e 4.xx' )
            sys.exit( 22 )

        match = re.match( "(?P<version_major>[0-9])\.(?P<version_minor>[0-9]+)", iArgs.ue_version )
        if match is None:
            print( Fore.RED + f'Can\'t determine ue version from argument "{iArgs.ue_version}", please check the syntax: -e 4.xx' )
            sys.exit( 23 )
    else:
        if iArgs.ue_version is not None:
            print( Fore.YELLOW + f'ue version is guess from the current branch name "{current_branch}", the ue version as argument "{iArgs.ue_version}" will NOT be used' )

    branch_versions = [ match.group('version_major'), match.group('version_minor') ]
    branch_version = '.'.join( branch_versions )
    branch_version_without_dot = ''.join( branch_versions )

    # Get version number in uplugin file
    uplugin_data = {}
    with iUPluginPathFile.open() as infile:
        uplugin_data = json.load( infile )

    uplugin_version = uplugin_data['VersionName']
    uplugin_versions = uplugin_version.split( '.' )

    # Check version number of uplugin file to match the branch version
    if branch_version_without_dot != uplugin_versions[-1]:
        print( Fore.RED + f'ue version from branch name "{current_branch}" differs from ue version in uplugin file "{uplugin_versions[-1]}", please update uplugin file' )
        sys.exit( 24 )
        
    print( Fore.GREEN + f'{"Build with UE":20}: {branch_version}' )

    return branch_version

# Get intermediate folders (with date/version/...)
def GetIntermediateFolders( iArgs, iUPluginPathFile, iPluginName, iUEVersion ):
    global gOperatingSystem

    uplugin_data = {}
    with iUPluginPathFile.open() as infile:
        uplugin_data = json.load( infile )

    now = datetime.now()

    date_folder = []
    date_folder.append( now.strftime( '%Y%m%d.%H%M%S' ) )
    date_folder.append( iUEVersion )
    date_folder.append( uplugin_data["VersionName"] )
    date_folder.append( 'beta' if uplugin_data['IsBetaVersion'] else '' )
    if iArgs.target in [eTarget.kDev, eTarget.kBeta]:
        date_folder.append( gOperatingSystem )
    date_folder.append( iArgs.suffix )
    date_folder = list( filter( None, date_folder ) )
    date_folder = '-'.join( date_folder )

    main_folder = []
    main_folder.append( iPluginName )
    main_folder.append( iUEVersion )
    main_folder.append( iArgs.target.value )
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
    if iArgs.target in [eTarget.kDev, eTarget.kBeta]:
        zip_name.append( gOperatingSystem )
    zip_name = list( filter( None, zip_name ) )
    zip_name = '-'.join( zip_name )

    return zip_name

# Get output package directory
def ProcessArgumentForOutputPath( iArgs, iIntermediateFolders ):
    output_path = Path( iArgs.output_dir ).resolve()

    output_path = ( output_path / iIntermediateFolders )
    output_path.mkdir( parents=True, exist_ok=True )

    print( Fore.GREEN + f'{"Output path":20}: {output_path}' )

    return output_path

# Get upload package directory
def ProcessArgumentForUploadPath( iArgs, iIntermediateFolders ):
    if not iArgs.upload:
        print( Fore.GREEN + f'{"Upload":20}: NO' )
        return None

    upload_path = Path( iArgs.upload_dir ).resolve()

    upload_path.mkdir( parents=True, exist_ok=True )
    upload_path = upload_path / iIntermediateFolders

    print( Fore.GREEN + f'{"Upload path":20}: {upload_path}' )

    return upload_path

# Print info for target
def ProcessArgumentForTarget( iArgs ):
    print( Fore.GREEN + f'{"Build for target":20}: {iArgs.target.value}', end='' )
    if iArgs.target == eTarget.kDev:
        print( Fore.GREEN + f' (sources & binaries)' )
    elif iArgs.target == eTarget.kBeta:
        print( Fore.GREEN + f' (binaries only)' )
    elif iArgs.target == eTarget.kMarketplace:
        print( Fore.GREEN + f' (sources only)' )

#---

# Process the compilation
def Build( iUPluginPathFile, iUEVersion, iOutputPath ):
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
            module['PlatformAllowList'] = [ 'Win64' ] # https://www.unrealengine.com/en-US/marketplace-guidelines#261b | https://docs.unrealengine.com/5.0/en-US/unreal-engine-5-0-release-notes/
        elif gOperatingSystem == 'darwin':
            module['PlatformAllowList'] = [ 'Mac' ]

    with iUPluginPathFile.open( 'w' ) as outfile:
        json.dump( uplugin_data, outfile, indent=2 )

    #---

    if gOperatingSystem == 'windows':
        bat = Path( 'C:\\' ) / 'Program Files' / 'Epic Games' / f'UE_{iUEVersion}' / 'Engine' / 'Build' / 'BatchFiles' / 'RunUAT.bat'
        if not bat.exists():
            bat = Path( 'C:\\' ) / 'Program Files' / 'Epic Games' / f'UE_{iUEVersion}EA' / 'Engine' / 'Build' / 'BatchFiles' / 'RunUAT.bat'
        if not bat.exists():
            bat = Path( 'D:\\' ) / 'Epic Games' / f'UE_{iUEVersion}' / 'Engine' / 'Build' / 'BatchFiles' / 'RunUAT.bat'
        if not bat.exists():
            bat = Path( 'D:\\' ) / 'Epic Games' / f'UE_{iUEVersion}EA' / 'Engine' / 'Build' / 'BatchFiles' / 'RunUAT.bat'
        uat = [ str( bat ) ]
    elif gOperatingSystem == 'darwin':
        sh = Path( '/' ) / 'Users' / 'Shared' / 'Epic Games' / f'UE_{iUEVersion}' / 'Engine' / 'Build' / 'BatchFiles' / 'RunUAT.sh'
        if not sh.exists():
            sh = Path( '/' ) / 'Users' / 'Shared' / 'Epic Games' / f'UE_{iUEVersion}EA' / 'Engine' / 'Build' / 'BatchFiles' / 'RunUAT.sh'
        uat = [ str( sh ) ]
    uat_args = [ 'BuildPlugin', '-Plugin=' + str( iUPluginPathFile ) + '', '-Package=' + str( iOutputPath ) + '', '-CreateSubFolder', '-Rocket' ]

    # Run packaging script
    process = subprocess.run( uat + uat_args )

    #---

    shutil.copyfile( uplugin_backup_pathfile, iUPluginPathFile ) # Must be done before any sys.exit()
    uplugin_backup_pathfile.unlink()

    if process.returncode != 0:
        print( Fore.RED + f'An error occurs during processing...' )
        sys.exit( 30 )

# Make some fix depending on target (whitelist platform, config, ...)
def PostBuildFix( iArgs, iOutputPath ):
    uplugin_pathfiles = [ entry for entry in iOutputPath.glob( '*.uplugin' ) if entry.is_file() ]
    if not uplugin_pathfiles:
        print( Fore.RED + f'no uplugin file in: {iOutputPath}' )
        sys.exit( 40 )

    if iArgs.target is eTarget.kMarketplace:
        uplugin_pathfile = uplugin_pathfiles[0]

        uplugin_data = {}
        with uplugin_pathfile.open() as infile:
            uplugin_data = json.load( infile )

        for module in uplugin_data['Modules']:
            module['PlatformAllowList'] = [ 'Win64', 'Mac' ] # https://www.unrealengine.com/en-US/marketplace-guidelines#261b

        del uplugin_data['PreBuildSteps'] # Remove pre-build-steps as they are (at least for now) for development stuff: https://udn.unrealengine.com/s/question/0D54z00006tMlfbCAC/plugin-cconfig-how-to-use-config-ini-file-for-a-custom-plugin-

        with uplugin_pathfile.open( 'w' ) as outfile:
            json.dump( uplugin_data, outfile, indent=2 )
    elif iArgs.target is eTarget.kBeta:
        shutil.copyfile( iOutputPath / 'Config' / 'BaseEpos.ini', iOutputPath / 'Config' / 'DefaultEpos.ini' ) # So, people installing plugin inside project, settings will be read from DefaultEpos.ini

#---

# Cleaning
def Clean( iArgs, iOutputPath ):
    # Remove binaries only for marketplace, otherwise it's for internal testing and binaries are needed to not have to compile the plugin again
    if iArgs.target is eTarget.kMarketplace:
        binaries = iOutputPath / 'Binaries'
        print( Fore.GREEN + f'Removing: {binaries}' )
        shutil.rmtree( binaries, ignore_errors=True )

    if iArgs.target is eTarget.kBeta:
        source = iOutputPath / 'Source'
        print( Fore.GREEN + f'Removing: {source}' )
        shutil.rmtree( source, ignore_errors=True )

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
ProcessArgumentForTarget( args )
ue_version                      = GetUEVersion( args, uplugin_pathfile )
intermediate_folders            = GetIntermediateFolders( args, uplugin_pathfile, plugin_name, ue_version )
zip_name                        = GetZipName( args, uplugin_pathfile, plugin_name )
output_path                     = ProcessArgumentForOutputPath( args, intermediate_folders )
upload_path                     = ProcessArgumentForUploadPath( args, intermediate_folders )

#---

print( 'Everything\'s ok ? [Y/n]: ' )
choice = input().lower()
if choice not in [ 'yes', 'y', '' ]:
    sys.exit( 50 )

#---

Build( uplugin_pathfile, ue_version, output_path )

PostBuildFix( args, output_path )

#---

Clean( args, output_path )

Zip( args, output_path, zip_name )

Upload( args, output_path, upload_path )
