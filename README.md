<!-- IDDN.FR.001.060015.014.S.X.2019.000.00000 -->
<!-- ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023 -->

[![UE5](https://img.shields.io/badge/UE5-5.6+-orange)](https://www.fab.com/listings/c2a71aa9-998c-4286-9d5b-df91d9cc4034)
[![Discord](https://img.shields.io/discord/639891086369882112?label=Discord&style=flat&logo=Discord&logoColor=whitesmoke&color=mediumslateblue)](https://discordapp.com/invite/gEd6pj7)

# Odyssey

<p align="center">
    <img src="https://praxinos.coop/images/odyssey-logo.png" alt="Odyssey">
</p>

<!-- -->

# Overview

**Odyssey** is a digital-painting + storyboard manager plugin for Unreal Engine 5.

**Odyssey**  comes with a dedicated interface to create and edit animations and textures directly within the engine, eliminating the need for a workflow with multiple software.
Express your art and experience new creative possibilities with a powerful blueprint-based nodal brush engine.
Create brushes to make traditional tools ( crayons, paintbrushes, pencils... ).

**Odyssey** can be used for many purposes, including:
- Creating 2D animations and drop them in the viewport
- Editing 2D textures used in materials ( as diffuse, specular, normal, etc. ) and see the result in real-time on 3D assets in the viewport.
- Creating 2D images like tile sets or sprites for 2D video games.
- Drawing sketches for storyboard, design or illustration.

**Odyssey** is also designed for **2D storyboard artists** ; its use is simple and intuitive, yet powerful and complete:
- use 3D environment and create shots, cameras and animations in a few clicks directly from the Sequencer
- adjust the angle and the perspective
- and ... **start drawing and animating**! :writing_hand:

**Odyssey** is **source-control and multi-user compatible**: storyboard artists, previz artists, background artists, film directors and all other people involved in preproduction can work together in real-time to share and enhance their vision of the art.

<!-- -->

# Installation

Install the plugin directly to your Unreal Engine 5 via [Fab](https://www.fab.com/listings/c2a71aa9-998c-4286-9d5b-df91d9cc4034) ... **it's free !** :heart_eyes::+1:

<!-- -->

# Documentation

Read the [:blue_book:User Documentation](https://praxinos.coop/odyssey-user-doc) to learn how to use the plugin

<!-- -->

# Links

[Official Repository](https://github.com/Praxinos/Odyssey-Plugin)  
[Official Homepage :house:](https://praxinos.coop/odyssey)  
[Praxinos Website](https://praxinos.coop)  
[Odyssey on Discord](https://discordapp.com/invite/gEd6pj7)  
[Fab](https://www.fab.com/listings/c2a71aa9-998c-4286-9d5b-df91d9cc4034)  

<!-- -->

# Setup (after cloning)

- ![Static Badge](https://img.shields.io/badge/Windows-aaaaff) Double-click on `Setup.bat`
- ![Static Badge](https://img.shields.io/badge/MacOSX-ffaaaa) Double-click on `Setup.command`

> [!TIP]  
> Don't hesitate to double-click on `Setup.bat` | `Setup.command` from time to time

<!-- -->

# Making a Package

See the [README](https://github.com/Praxinos/Installers#plugins-odyssey-iliad--epos) of [Installers](https://github.com/Praxinos/Installers) repository

<!-- -->

# Commit comment rules

## Minimal Structure

```
<type>: <description>
```

- `<description>`
    - MUST be a single line
    - MUST follow a space (after the semi-colon)
    - MUST start with a verb in imperative, present tense
        - like `add`, `remove`, `replace`, `improve`, `move`, `create`, `set`, `check`, `use`, `manage`, ... but **NEVER** ~~`fix`~~ !!!
    - MUST be in lowercase, **even the verb**
        - an exception is allowed for words used in the code (function/class/... names)
    - MUST **NOT** end with a period (`.`)

- `<type>`
    - MUST be **only** one of:
        - **`feat`**: new feature for the user (not a new feature for build script)
        - **`fix`**: bug fix for the user (not a fix to a build script)
        - **`chore`**: updating grunt tasks etc (no production code change)
        - **`refactor`**: refactoring production code, eg. renaming a variable
        - *`style`*: formatting, missing semi colons, etc (no production code change)
        - *`docs`*: changes to the documentation
        - *`test`*: adding missing tests, refactoring tests (no production code change)

- examples
    ```
    feat: compute the scale size of the mesh to be inside the camera frustrum
    ```
    ```
    fix: use the real Size<>() function for section
    ```
    ```
    chore: remove dependency between EposTracksEditor and EposEditor by adding its own style
    ```
    Multiple lines MAY be used for one commit, but use it sparingly
    ```
    feat: manage single camera and multiple planes inside shot sequence
    feat: add a playback context inside toolkit
    ```

## Verbose Structure

<details>
  <summary>Show</summary>

```
<type>(<optional-scope>): <description>

<optional-body>

<optional-footer>
```

- Blank lines between each part are mandatory
- `<optional-scope>`
    - MAY be 1 module name like: `EposMovieScene`, `EposSequence`, ... if modifications are only contained inside a single module
    - in the case that 2 (or 3 but no more) modules are concerned, they will be separated by a comma (`,`) (without a space after)
- `<optional-body>`
    - just as in the description, use the imperative, present tense
- `<optional-footer>`
    - MAY be used for referencing issues like: `close #234` or `close #123, close #245, close #992`
    - MAY be used for BREAKING-CHANGE

## Example

```
refactor(EposTracksEditor): move Board* headers to private
```
```
feat: add key thumbnail section for single cameracut track

this may be improved because base class FTrackEditorThumbnail is not virtualized and FThumbnailSection class owns a plain object of FTrackEditorThumbnail class instead of a pointer

close #12345
```
</details>

## References

- http://karma-runner.github.io/4.0/dev/git-commit-msg.html
- https://github.com/angular/angular/blob/master/CONTRIBUTING.md#commit-body
- https://www.conventionalcommits.org/en/v1.0.0/
