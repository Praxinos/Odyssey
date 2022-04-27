#!/usr/bin/env python

from enum import Enum, auto
import itertools
from pathlib import Path

#---

## The options
#
#  To select option to check
class eOptions( Enum ):
    kUTF8 = auto()
    kTabs = auto()
    kTrailingSpaces = auto()
    kOnlySpaces = auto()
    kEOL = auto()
    kEOF = auto()
    kIDDN = auto()

#---

## The EOL checker (= no mixing eol)
#
#  @param  iLines   List[str]   The lines to check
#  @return          bool        Check if all lines are ok or not
def CheckEOL( iLines ):
    eol = { 'win': 0, 'unix': 0, 'mac': 0 }
    for line in iLines:
        if line.endswith( '\r\n' ):
            eol['win'] += 1
        elif line.endswith( '\n' ):
            eol['unix'] += 1
        elif line.endswith( '\r' ):
            eol['mac'] += 1
    
    return sum( [ bool(count) for count in eol.values() ] ) <= 1

## The Tabs checker (= no tabs)
#
#  @param  iLines   List[str]               The lines to check
#  @return          Tuple[bool, int, str]   Check if all lines are ok or not, and if not ok, return the line number and the line
def CheckTabs( iLines ):
    for i, line in enumerate( iLines ):
        if '\t' in line:
            return False, i + 1, line
    
    return True, None, None

## The Trailing Spaces checker (= no trailing spaces)
#
#  @param  iLines   List[str]               The lines to check
#  @return          Tuple[bool, int, str]   Check if all lines are ok or not, and if not ok, return the line number and the line
def CheckTrailingSpaces( iLines ):
    for i, line in enumerate( iLines ):
        line_without_eol = line.rstrip( '\n' )
                            # remove starting spaces (to not consider full spaces line as error here)
                                        # check if trailing spaces
        if line_without_eol.lstrip( ' ' ).endswith( ' ' ):
            return False, i + 1, line
    
    return True, None, None

## The Only Spaces checker (= no line with only spaces)
#
#  @param  iLines   List[str]               The lines to check
#  @return          Tuple[bool, int, str]   Check if all lines are ok or not, and if not ok, return the line number and the line
def CheckOnlySpaces( iLines ):
    for i, line in enumerate( iLines ):
        line_without_eol = line.rstrip( '\n' )

        if len( line_without_eol ) and all( c == ' ' for c in line_without_eol ):
            return False, i + 1, line
    
    return True, None, None

## The EOF checker (= only one last empty line)
#
#  @param  iLines   List[str]   The lines to check
#  @return          bool        Check if last line(s) is(are) ok or not
def CheckEOF( iLines ):
    if len( iLines ) == 0:
        return True
    
    return iLines[-1].endswith( '\n' ) and len( iLines[-1] ) > 1

## The IDDN checker (= exist and correct format)
#
#  @param  iLines   List[str]   The lines to check
#  @return          bool        Check if first lines are ok (contain iddn and cpoyright) or not
def CheckIDDN( iLines ):
    if len( iLines ) == 0:
        return True

    if len( iLines ) == 1:
        return False
    
    return iLines[0].startswith( '// IDDN.FR.' ) and ( iLines[1].startswith( '// ' ) and 'copyright' in iLines[1] and 'Praxinos' in iLines[1] and 'publishing' in iLines[1] )

#---

## The file checker
#
#  @param  iPathFile    Path            The file to check
#  @param  iOptions     List[eOptions]  The options to check
#  @return              bool            Check if file is ok or not
def CheckFile( iPathFile, iOptions ):
    if not iPathFile.is_file():
        print( f'Error: not a file: {iPathFile}' )
        return False

    ok = True

    if eOptions.kUTF8 in iOptions:
        with iPathFile.open( 'r', encoding='utf-8' ) as f:
            try:
                lines = f.readlines()
            except UnicodeDecodeError:
                print( f'Error: UTF8 decoding: {iPathFile}' )
                ok = False
        
    if eOptions.kEOL in iOptions:
        with iPathFile.open( 'r', newline='' ) as f:
            lines = f.readlines()

            if not CheckEOL( lines ):
                print( f'Error: mixed eol: {iPathFile}' )
                ok = False

    #---
    
    # To open as utf-8 and with universal line ending (ie. '\n')
    with iPathFile.open( 'r' ) as f:
        lines = f.readlines()
        
        if eOptions.kIDDN in iOptions and not CheckIDDN( lines ):
            print( f'Error: wrong iddn: {iPathFile}' )
            ok = False

        check, no_line, line = CheckTabs( lines )
        if eOptions.kTabs in iOptions and not check:
            print( f'Error: tab inside:' )
            print( f'\t#{no_line:<4} : {iPathFile}' )
            print( f'\t[' + line.rstrip( '\n' ) + ']' )
            ok = False
    
        check, no_line, line = CheckTrailingSpaces( lines )
        if eOptions.kTrailingSpaces in iOptions and not check:
            print( f'Error: trailing spaces:' )
            print( f'\t#{no_line:<4} : {iPathFile}' )
            print( f'\t[' + line.rstrip( '\n' ) + ']' )
            ok = False
        
        check, no_line, line = CheckOnlySpaces( lines )
        if eOptions.kOnlySpaces in iOptions and not check:
            print( f'Error: only spaces:' )
            print( f'\t#{no_line:<4} : {iPathFile}' )
            print( f'\t[' + line.rstrip( '\n' ) + ']' )
            ok = False
        
        if eOptions.kEOF in iOptions and not CheckEOF( lines ):
            print( f'Error: no (or multiple) empty EOF line: {iPathFile}' )
            ok = False
    
    return ok

## The files checker
#
#  @param  iPathFiles   List[Path]      The files to check
#  @param  iOptions     List[eOptions]  The options to check
#  @return              bool            Check if files are ok or not
def Check( iPathFiles, iOptions ):
    ok = [ CheckFile( pathfile, iOptions ) for pathfile in iPathFiles ]
    return all( ok )

#---

## The main
#
#  @return  int     The error code
def main():
    root = Path( '.' ).resolve()
    source = root / 'Source'
    ok = []
    
    # .git*
    options = [ eOptions.kUTF8, eOptions.kTabs, eOptions.kTrailingSpaces, eOptions.kOnlySpaces, eOptions.kEOL, eOptions.kEOF ]
    pathfiles = [ entry for entry in root.glob( '.*' ) if entry.is_file() ]
    ok.append( Check( pathfiles, options ) )

    # *.uplugin
    options = [ eOptions.kUTF8, eOptions.kTabs, eOptions.kTrailingSpaces, eOptions.kOnlySpaces, eOptions.kEOL, eOptions.kEOF ]
    pathfiles = [ entry for entry in root.glob( '*.uplugin' ) if entry.is_file() ]
    ok.append( Check( pathfiles, options ) )

    # source
    options = [ eOptions.kUTF8, eOptions.kTabs, eOptions.kTrailingSpaces, eOptions.kOnlySpaces, eOptions.kEOL, eOptions.kEOF, eOptions.kIDDN ]
    pathfiles = [ entry for entry in source.rglob( '*' ) if entry.is_file() and entry.suffix in ['.h', '.cpp', '.cs'] ]
    ok.append( Check( pathfiles, options ) )

    return 0 if all( ok ) else 1 # return 1 if at least one error in one file occurs

if __name__ == "__main__":
    exit( main() )
