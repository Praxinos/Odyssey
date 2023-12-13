#!/usr/bin/env python
# IDDN.FR.001.220036.001.S.P.2021.000.00000
# EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

import argparse
import locale
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys
import zipfile

# [BEFORE] > must work without venv
from venv_setup import init
init()
# [AFTER]  > venv is valid (with all its modules)

from colorama import init, Fore, Back, Style
init( autoreset=True )
from dotenv import load_dotenv
load_dotenv()
import requests

#---

gOperatingSystem = platform.system().lower() # 'windows', 'darwin', 'linux', ...
if gOperatingSystem not in [ 'windows', 'darwin' ]:
    raise NotImplementedError( Fore.RED + f'This platform is not supported: {gOperatingSystem}' )

script_path = Path( __file__ ).resolve().parent
intermediate_path = script_path / 'Intermediate'
intermediate_path.mkdir( exist_ok=True )

#---

# Get the root directory in a zip file and check there is only one
def FindSingleRootDirectoryInZip( iZipPathFile ):
    with zipfile.ZipFile( iZipPathFile, 'r' ) as zip_ref:
        # Get all inner directories
        inner_full_paths = [ info.filename for info in zip_ref.infolist() if info.is_dir() ]

        inner_firstpart_paths = set( [ inner_full_path.split( '/' )[0] for inner_full_path in inner_full_paths ] )

        # If more than 1 directories in the root, make an error
        if len( inner_firstpart_paths ) != 1:
            raise ValueError( Fore.RED + f'More than 1 root directory in zip file: {iZipPathFile}' )

        return list(inner_firstpart_paths)[0]

#---
#--- Download/Unzip libharu source files
#---

def DownloadAndUnzip_LibharuSources( iScriptPath: Path, iIntermediatePath: Path ) -> Path:
    # Download libharu source files
    libharu_src_zip_file = 'RELEASE_2_3_0.zip'
    # libharu_src_zip_file = 'libharu-2.4.0-rc1' # error on configuring...
    libharu_src_zip_pathfile = iIntermediatePath / f'libharu_{libharu_src_zip_file}'
    libharu_src_url = f'https://github.com/libharu/libharu/archive/refs/tags/{libharu_src_zip_file}'
    if not libharu_src_zip_pathfile.exists():
        print( f'Request libharu source zip file... : {libharu_src_url}')
        r = requests.get( libharu_src_url )
        with libharu_src_zip_pathfile.open( 'wb' ) as file:
            file.write( r.content )

    inner_root_path = FindSingleRootDirectoryInZip( libharu_src_zip_pathfile )
    src_path = iScriptPath / inner_root_path

    # Unzip libharu source files
    if not src_path.exists():
        print( f'Extract libharu zip file... : {libharu_src_zip_pathfile}')
        with zipfile.ZipFile( libharu_src_zip_pathfile, 'r' ) as zip_ref:
            zip_ref.extractall( iScriptPath )

    if not src_path.exists():
        raise FileNotFoundError( Fore.RED + f'libharu src dir doesn\'t exist: {src_path}' )

    return src_path

#---
#--- Download/Unzip cmake source files (if not already in the PATH)
#---

def DownloadAndUnzip_CMakeSources( iPlatform: str, iIntermediatePath: Path ) -> list[str]:
    cmake_cmd = [ 'cmake' ]

    if shutil.which( cmake_cmd[0] ) is not None:
        return cmake_cmd

    cmake_version = '3.23.2'
    if iPlatform == 'windows':
        cmake_zip_file = f'cmake-{cmake_version}-windows-x86_64.zip'
    elif iPlatform == 'darwin':
        cmake_zip_file = f'cmake-{cmake_version}-macos10.10-universal.tar.gz'

    # Download cmake source files
    cmake_zip_pathfile = iIntermediatePath / cmake_zip_file
    cmake_url = f'https://github.com/Kitware/CMake/releases/download/v{cmake_version}/{cmake_zip_file}'
    if not cmake_zip_pathfile.exists():
        print( f'Request cmake zip file... : {cmake_url}')
        r = requests.get( cmake_url )
        with cmake_zip_pathfile.open( 'wb' ) as file:
            file.write( r.content )

    inner_root_path = FindSingleRootDirectoryInZip( cmake_zip_pathfile )
    cmake_path = iIntermediatePath / inner_root_path

    # Unzip cmake source files
    if not cmake_path.exists():
        print( f'Extract cmake zip file... : {cmake_zip_pathfile}')
        with zipfile.ZipFile( cmake_zip_pathfile, 'r' ) as zip_ref:
            zip_ref.extractall( iIntermediatePath )

    #-

    # Check the cmake binary exists
    if iPlatform == 'windows':
        cmake_pathfile = cmake_path / 'bin' / 'cmake.exe'
    elif iPlatform == 'darwin':
        cmake_pathfile = cmake_path / 'bin' / 'cmake'

    if not cmake_pathfile.exists():
        raise FileNotFoundError( Fore.RED + 'cmake file doesn\'t exist, add it to the current directory or install it' )

    cmake_cmd = [ str( cmake_pathfile ) ]

    return cmake_cmd

#---
#--- Download
#---

src_path = DownloadAndUnzip_LibharuSources( script_path, intermediate_path )

cmake_cmd = DownloadAndUnzip_CMakeSources( gOperatingSystem, intermediate_path )

#---
#--- Define all configuration parameters
#---

configuration = 'Release'
target = 'install'

if gOperatingSystem == 'windows':
    generator = 'Visual Studio 17 2022'
    compiler = 'vs2022'
    architecture = 'x64'
elif gOperatingSystem == 'darwin':
    generator = 'Xcode'
    compiler = 'xcode'
    architecture = ''

#-

# final_*_path are the ones used in the ue .Build.cs file
if gOperatingSystem == 'windows':
    final_include_path = script_path / 'include' / 'win64' / 'vs2022'
    final_lib_path = script_path / 'lib' / 'win64' / 'vs2022'
elif gOperatingSystem == 'darwin':
    final_include_path = script_path / 'include' / 'macosx'
    final_lib_path = script_path / 'lib' / 'macosx'

#-

# Get the zlib/libpng stuff from ue5 thirdparty
zlib = {
    'include' : Path( os.getenv( 'UE_THIRDPARTY_ZLIB_INCLUDE' ) ),
    'lib'     : Path( os.getenv( 'UE_THIRDPARTY_ZLIB_LIB' ) ),
}

libpng = {
    'include' : Path( os.getenv( 'UE_THIRDPARTY_LIBPNG_INCLUDE' ) ),
    'lib'     : Path( os.getenv( 'UE_THIRDPARTY_LIBPNG_LIB' ) ),
}

if not zlib['include'].exists():
    raise FileNotFoundError( Fore.RED + f'zlib include dir doesn\'t exist: {zlib["include"]}' )
if not zlib['lib'].exists():
    raise FileNotFoundError( Fore.RED + f'zlib lib file doesn\'t exist: {zlib["lib"]}' )
if not libpng['lib'].exists():
    raise FileNotFoundError( Fore.RED + f'libpng lib dir doesn\'t exist: {libpng["include"]}' )
if not libpng['lib'].exists():
    raise FileNotFoundError( Fore.RED + f'libpng lib file doesn\'t exist: {libpng["lib"]}' )

#---

# Intermediate file for configuring/building libharu via cmake
suffixes = [
    gOperatingSystem,
    architecture,
    compiler,
    configuration.lower(),
]
suffixes = list( filter( None, suffixes ) )
build_path = intermediate_path / ( f'_build-' + '-'.join( suffixes ) )
install_path = intermediate_path / ( f'_install-' + '-'.join( suffixes ) )

#---
#--- Displaying/Validation of all parameters
#---

print( Fore.GREEN + f'{"OS":20}: {gOperatingSystem}' )
print( Fore.GREEN + f'{"source":20}: {src_path}' )
print( Fore.GREEN + f'{"build":20}: {build_path}' )
print( Fore.GREEN + f'{"install":20}: {install_path}' )
print()
print( Fore.GREEN + f'{"final include":20}: {final_include_path}' )
print( Fore.GREEN + f'{"final lib":20}: {final_lib_path}' )
print()
print( Fore.GREEN + f'{"cmake":20}: {cmake_cmd[0]}' )
print( Fore.GREEN + f'{"generator":20}: {generator}' )
print( Fore.GREEN + f'{"architecture":20}: {architecture}' )
print( Fore.GREEN + f'{"zlib include":20}: {zlib["include"]}' )
print( Fore.GREEN + f'{"zlib lib":20}: {zlib["lib"]}' )
print( Fore.GREEN + f'{"libpng include":20}: {libpng["include"]}' )
print( Fore.GREEN + f'{"libpng lib":20}: {libpng["lib"]}' )
print()
print( Fore.GREEN + f'{"configuration":20}: {configuration}' )

print( 'Everything\'s ok ? [Y/n]: ' )
choice = input().lower()
if choice not in [ 'yes', 'y', '' ]:
    sys.exit()

#---
#---
#---

if build_path.exists():
    shutil.rmtree( build_path )
build_path.mkdir( parents=True, exist_ok=True)

if install_path.exists():
    shutil.rmtree( install_path )
install_path.mkdir( parents=True, exist_ok=True)

#---

def RunCommandAndOutput( iCommand ):
    # https://stackoverflow.com/questions/2715847/read-streaming-input-from-subprocess-communicate/17698359#17698359
    with subprocess.Popen( iCommand, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, bufsize=1, universal_newlines=True ) as proc:
        for line in proc.stdout:
            print( line, end='' )

    # stdout and stderr are closed at this point
    return proc

#---

print( Fore.GREEN + f'CMake Configuring... {build_path}' )

cmake_args = [
    '-S', str( src_path ),
    '-B', str( build_path ),
    '-G', generator,
]

if architecture:
    cmake_args += [ '-A', architecture ]

if gOperatingSystem == 'darwin':
    cmake_args += [ '-DCMAKE_OSX_DEPLOYMENT_TARGET=10.15' ]
    cmake_args += [ '-CMAKE_OSX_ARCHITECTURES=x86_64;arm64' ]

cmake_args += [
    '-DCMAKE_INSTALL_PREFIX=' + str( install_path ),
    '-DLIBHPDF_SHARED=' + 'OFF',
    # Use as_posix to keep slash on windows, otherwise there will be warning with cmake during the install ...
    '-DZLIB_INCLUDE_DIR=' + zlib['include'].as_posix(),
    '-DZLIB_LIBRARY=' + zlib['lib'].as_posix(),
    '-DPNG_PNG_INCLUDE_DIR=' + libpng['include'].as_posix(),
    '-DPNG_LIBRARY=' + libpng['lib'].as_posix(),
]

process = RunCommandAndOutput( cmake_cmd + cmake_args )
# print( process.returncode )

#---

print()
print( Fore.GREEN + f'CMake Building/Installing... {install_path}' )

cmake_args = [
    '--build', str( build_path ),
    '--config', configuration,
    '--target', target,
]

process = RunCommandAndOutput( cmake_cmd + cmake_args )
# print( process.returncode )

#---
#--- Copy/Clean all generated stuff to final path
#---

final_include_path.mkdir( parents=True, exist_ok=True )
final_lib_path.mkdir( parents=True, exist_ok=True )

shutil.copytree( install_path / 'include', final_include_path, dirs_exist_ok=True )
shutil.copytree( install_path / 'lib', final_lib_path, dirs_exist_ok=True )

lib_to_remove = final_lib_path / zlib['lib'].name
if lib_to_remove.exists():
    lib_to_remove.unlink()
lib_to_remove = final_lib_path / libpng['lib'].name
if lib_to_remove.exists():
    lib_to_remove.unlink()

#---
#---
#---

print()
print( Fore.YELLOW + 'Don\'t forget to check/update .cs file with the path(s)' )
print()
