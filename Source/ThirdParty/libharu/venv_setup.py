# IDDN.FR.001.060015.004.S.X.2019.000.00000
# venv_setup.py is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#---

# This file manage the venv checking
# Copy this file as-is near the (future) .venv folder
# Then just import it at the begining of the main script like:
"""
# [BEFORE] > must work without venv
import venv_setup
# [AFTER]  > venv is valid (with all its modules)
"""
# So the script will check if a venv is activate or not

#---

from pathlib import Path
import platform
import subprocess
import sys

#---

def ConvertCommandToShell( iCommand: list[str] ) -> str:
    return ' '.join( f'"{argument}"' if ' ' in argument else argument for argument in iCommand )

def _RunCommand( iCommand ):
    command = ConvertCommandToShell( iCommand )
    completed_process = subprocess.run( command, shell=True, capture_output=True, text=True )
    if completed_process.stdout:
        print( completed_process.stdout )

def _IsWindows():
    return platform.system().lower() == 'windows'
def _IsMacOSX():
    return platform.system().lower() == 'darwin'

def _CreateAndUpgradeVenv():
    root_path = Path( __file__ ).parent.resolve()
    venv_path = root_path / '.venv'
    requirements_pathfile = root_path / 'requirements.txt'

    if _IsWindows():
        venv_activation = [ str(venv_path / 'Scripts' / 'activate.bat') ]
        py = 'python'
    elif _IsMacOSX():
        venv_activation = [ 'source', str(venv_path / 'bin' / 'activate') ]
        py = 'python3'
    else:
        raise ValueError( f'{platform.system().lower()} not supported' )

    create_venv =           [ py, '-m', 'venv', str( venv_path ) ]
    install_requirements =  [ py, '-m', 'pip', 'install', '-q', '-r', str( requirements_pathfile ) ]
    upgrade_pip =           [ py, '-m', 'pip', 'install', '-q', '--upgrade', 'pip' ]
    upgrade_requirements =  [ py, '-m', 'pip', 'install', '-q', '--upgrade', '-r', str( requirements_pathfile ) ]

    if venv_path.exists():
        print( f'Upgrading venv: pip...' )
        _RunCommand( venv_activation + [ '&&' ] + upgrade_pip )
        print( f'Upgrading venv: requirements...' )
        _RunCommand( venv_activation + [ '&&' ] + upgrade_requirements )
    else:
        print( f'Creating venv...' )
        _RunCommand( create_venv )

        print( f'Updating venv: pip...' )
        _RunCommand( venv_activation + [ '&&' ] + upgrade_pip )
        print( f'Updating venv: requirements...' )
        _RunCommand( venv_activation + [ '&&' ] + install_requirements )

    print()
    print( f'Now, activate the virtual environement with: ' )
    venv_activation_str = ConvertCommandToShell( venv_activation )
    print( '\tcmd       : ' + '\033[32m' + f'{venv_activation_str}' + '\033[0m' )           # colorama is not imported here, so hardcode values
    if _IsWindows():
        print( '\tpowershell: ' + '\033[32m' + f'{venv_activation_str.replace( "activate.bat", "Activate.ps1" )}' + '\033[0m' )

    print( f'Then, once in the venv, execute your command again.' )

#---
#---
#---

def init():
    _valid_venv_path = Path( __file__ ).parent.resolve() / '.venv'
    _prefix_path = Path( sys.prefix )
    _base_prefix_path = Path( sys.base_prefix )

    # print( f'_prefix_path     : {_prefix_path}' )
    # print( f'_base_prefix_path: {_base_prefix_path}' )
    # print( f'_valid_venv_path : {_valid_venv_path}' )

    # "a venv is NOT activated"
    if _prefix_path == _base_prefix_path:
        print( 'Not in a venv, setting up venv.' )
        print()
        _CreateAndUpgradeVenv()
        sys.exit()

    # "activated venv is NOT the one in the script folder"
    elif _prefix_path != _valid_venv_path:
        print( 'Wrong venv, deactivate it and execute your command again.' )
        sys.exit()

    #else:
        # Let's see if we need to also manage the upgrade of pip and requirements when we are already in the venv
        # But in this case, this will be called at the startup of each script
