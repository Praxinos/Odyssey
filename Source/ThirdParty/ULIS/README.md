## ULIS
ULIS is the third party library used for core image processing on GPU in Iliad.  
ULIS is developped and maintained by Praxinos.  
Right now, ULIS sources are private and ULIS is only redistributed in binary form along with Iliad releases.  
This may change in the future so here are the instructions to build ULIS from source, in case you are setting up a development environment for Iliad.  

## How To Build
Software requirements:

        Linux, Windows or MacOS
        CMake
        Git
        C++ compiler with support for C++14 or greater ( MSVC or XCode )

First, clone ULIS on your drive, anywhere outside of Iliad source tree:

        git clone https://github.com/Praxinos/ULIS3

Next look for this file ( .bat for windows, .sh for macos ) and copy it alongside to the ULIS3 folder:

        ULIS3/scripts/UnrealEngine4/ULIS3_UE4_build_redist.bat

Next launch it from command line, if cmake generates an error that states the generator is not found, open the script and change the generator name according to your needs.  
Then, it will produce a directory named ULIS3_FullBuildUE4_XXX_Redist, containing some files and directories. You will copy these in the directory where you found this README file ( You can leave the Generated_XXX folder there. )

        redist
        ULIS.Build.cs

Once it's done, you might want to copy the files in redist/bin into the Binaries folder of your project, or else it will complain that some library are missing at runtime.


## Latest Compatible Commit
ec7659e511aae688b7632865bad1710d0f9a4e25

