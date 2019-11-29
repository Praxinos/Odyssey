# Iliad

# Building
- Create a new 'Basic Code' C++ project with official UE4 version (4.23+) `PROJECT_PATH` and create `Plugins` directory
    ```
    cd $PROJECT_PATH
    mkdir Plugins
    cd Plugins
    ```
- Clone Iliad repository
    ```
    git clone https://github.com/Praxinos/Iliad.git
    ```
- *OR you can also clone in `$WORK` (as `$WORK/Iliad`) and make a symlink inside `$PROJECT_PATH/Plugins`*
    ```
    cd $WORK
    git clone https://github.com/Praxinos/Iliad.git

    cd $PROJECT_PATH/Plugins

    ln -s $WORK/Iliad
    # Windows
    # mklink /D Iliad $WORK\Iliad
    ```
- *Clone and compile ULIS anywhere (e.g. `$WORK/ULIS`)*
- Add ULIS to Iliad ThirdParty libraries
    ```
    cd $ILIAD_PATH
    mkdir -p $ILIAD_PATH/Source/ThirdParty/ULIS

    cp -r $ULIS_PATH/Redist        $ILIAD_PATH/Source/ThirdParty/ULIS
    cp    $ULIS_PATH/ULIS.Build.cs $ILIAD_PATH/Source/ThirdParty/ULIS
    ```
- `'Generate Visual Studio project files'` by right-clicking on the *.uproject file inside `$PROJECT_PATH`
- Open the solution (inside `$PROJECT_PATH`) with Visual Studio

# Packaging
*Only windows (for the moment)*
```
usage: build-package.py [-h] [-i INPUT] [-o OUTPUT]

Build package.

optional arguments:
  -h, --help            show this help message and exit
  -i INPUT, --input INPUT
                        The input path
                        (default: *current working directory*)
  -o OUTPUT, --output OUTPUT
                        The output path
                        'date-time/plugin-name' folders will be append to it
                        (default: *current working directory*/../package)
```

Basic example:
```
cd $PROJECT_PATH
build-package.py
```
