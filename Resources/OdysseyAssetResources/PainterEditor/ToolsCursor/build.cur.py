# IDDN.FR.001.060015.015.S.X.2019.000.00000
# ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#---

from pathlib import Path
import platform
import re
import subprocess
import sys

#---

def _RunCommand( iCommand: list[str] ):
    completed_process = subprocess.run( iCommand, capture_output=True, text=True )
    if completed_process.stdout:
        print( completed_process.stdout )

    completed_process.check_returncode()

#---

def main():
    if platform.system() == 'Windows':
        raise ValueError(f'script must be run on WSL or Linux')

    root = Path( __file__ ).parent.resolve()

    for input_pathfile in root.glob( '*.png' ):
        filename_without_suffix = input_pathfile.stem

        m = re.search(r'\.([0-9]+)x([0-9]+)\.?', filename_without_suffix)
        if m:
            hotspotX = int(m.group(1))
            hotspotY = int(m.group(2))
        else:
            raise ValueError(f"Pattern '<hotspotX>x<hotspotY>' not found in '{filename_without_suffix}'")

        output_pathfile = input_pathfile.with_suffix( '.cur' )

        print( f'Convert {input_pathfile} -> {output_pathfile}' )
        _RunCommand( [ 'icotool', '--create', '--cursor', f'--hotspot-x={hotspotX}', f'--hotspot-y={hotspotY}', f'--output={output_pathfile}', input_pathfile ] )

#-

if __name__ == '__main__':
    sys.exit( main() )
