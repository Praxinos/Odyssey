# IDDN.FR.001.060015.014.S.X.2019.000.00000
# ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#---

from pathlib import Path
import platform
import shutil
import subprocess
import sys

#---

def _RunCommand( iCommand: list[str] ):
    completed_process = subprocess.run( iCommand, capture_output=True, text=True )
    if completed_process.stdout:
        print( completed_process.stdout )

    completed_process.check_returncode()

def _GetPython( iVEnvPath: Path=None ):
    python = {
        'windows': 'py',
        'darwin': 'python3',
    }

    if iVEnvPath is not None:
        python = {
            'windows': iVEnvPath / 'Scripts' / 'python.exe' ,
            'darwin': iVEnvPath / 'bin' / 'python',
        }

    return python[ platform.system().lower() ]

#---

def main():
    root = Path( __file__ ).parent.resolve()
    venv_path = root / '.venv'
    requirements_pathfile = root / '.requirements.txt'

    if venv_path.exists():
        print( f'*** Removing existing venv... {venv_path}' )
        shutil.rmtree( venv_path )

    print( f'*** Creating venv... {venv_path}' )
    _RunCommand( [ _GetPython(), '-m', 'venv', venv_path ] )

    print( f'*** Updating venv: pip...' )
    _RunCommand( [ _GetPython( venv_path ), '-m', 'pip', 'install', '--upgrade', 'pip' ] )

    print( f'*** Updating venv: requirements...' )
    _RunCommand( [ _GetPython( venv_path ), '-m', 'pip', 'install', '--upgrade', '-r', requirements_pathfile ] )

    print( f'*** Setup pre-commit...' )
    _RunCommand( [ _GetPython( venv_path ), '-m', 'pre_commit', 'install', '--overwrite' ] )

    print( f'*** Run pre-commit on all files...' )
    _RunCommand( [ _GetPython( venv_path ), '-m', 'pre_commit', 'run', '--all-files' ] )

#-

if __name__ == '__main__':
    sys.exit( main() )
