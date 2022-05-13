[![UE4](https://img.shields.io/badge/UE-5.0+-lightgrey?style=flat&logo=unrealengine&logoColor=whitesmoke)](https://www.unrealengine.com/marketplace/en-US/product/iliad-intelligent-layered-imaging-architecture-for-drawing-beta-version)
[![Release](https://img.shields.io/github/release/Praxinos/Epos)](https://github.com/Praxinos/Epos/releases)
[![License](https://img.shields.io/badge/License-CC_Attribution_NoDerivatives_4.0_International-steelblue?style=flat)](https://creativecommons.org/licenses/by-nd/4.0/legalcode)
[![Discord](https://img.shields.io/discord/639891086369882112?label=Discord&color=mediumslateblue&style=flat&logo=Discord&logoColor=whitesmoke)](https://discordapp.com/invite/gEd6pj7)
[![Patreon](https://img.shields.io/badge/Patreon-Donate-tomato.svg?style=flat&logo=Patreon)](https://www.patreon.com/praxinos)

<br>
<p align="center">
    <img src="https://praxinos.coop/images/epos-logo.png" alt="Epos">
</p>

# Overview

**Extended Production Of Storyboard** - or **EPOS** - is a storyboard manager plugin in Unreal Engine Editor.

**EPOS** is designed for **2D storyboard artists** ; its use is simple and intuitive, yet powerful and complete:
- use 3D environment and create shots, cameras and planes in a few clicks directly from the Sequencer
- adjust the angle and the perspective
- install and enable the [plugin ILIAD](https://www.unrealengine.com/marketplace/en-US/product/iliad-intelligent-layered-imaging-architecture-for-drawing-beta-version)
- and ... **start drawing**! :writing_hand:

**EPOS** is **source-control and multi-user compatible**: storyboard artists, previz artists, background artists, film directors and all other people involved in preproduction can work together in real-time to share and enhance their vision of the art.

# Installation

Install the plugin directly to your Unreal Engine 5 via the [EpicGames Marketplace](https://www.unrealengine.com/marketplace/en-US/product/epos-2d-storyboard-animatic-plugin-for-unreal-engine) ... **it's free !** :heart_eyes::+1:

# Documentation

Read the [:blue_book:User Documentation](https://praxinos.coop/epos-user-doc) to learn how to use the plugin

# Links

[Sample Projects](https://praxinos.coop/epos-projects): Charpy, Bear (Parallax), ...  
[Official Repository](https://github.com/Praxinos/Epos)  
[Official Homepage :house:](https://praxinos.coop/epos)  
[Praxinos Website](https://praxinos.coop)  

# Module hierarchy

```
    ┌──────────────────────┐   ┌──────────────────────┐    ┌──────────────────┐       ╮
    │     EposMovieScene   │   │       EposNote       │    │     EposActors   │       │
    └───────────┬──────────┘   └──────────┬───────────┘    └───────┬──────────┘       │
                │                         │                        │                  │
    ┌───────────┴─────────────────────────┴───────────┐            │                  │
    │                    EposTracks                   │            │                  │  Runtime
    └─────────────────────────┬───────────────────────┘            │                  │
                              │                                    │                  │
                ┌─────────────┴────────────────────────────────────┴───┐              │
                │                      EposSequence                    │              │
                └───────────────────────────┬──────────────────────────┘              ╯
                                            │
                                            │
                                            │
                                            │
                   ┌────────────────────────┴───────────────────────┐                 ╮
                   │                EposNamingConvention            │                 │
                   └────────────────────────┬───────────────────────┘                 │
                                            │                                         │
                   ┌────────────────────────┴───────────────────────┐                 │
                   │                EposTracksEditor                │                 │  Editor
                   └────────────────────────┬───────────────────────┘                 │
                                            │                                         │
                   ┌────────────────────────┴───────────────────────┐                 │
                   │                EposSequenceEditor              │                 │
                   └────────────────────────────────────────────────┘                 ╯
```

# Setting Hooks (after cloning)

- go to `.git/hooks` folder
- rename `pre-commit.sample` to `pre-commit`
- open `pre-commit` file
- add this line after the last `fi`: `exec .githooks/check.py`

# Making a Package

- open a shell/cmd at the root of the plugin
- execute `build-package.py ...`

## Examples

| Command                               | Description                                       |
| :---                                  | :---                                              |
| `build-package.py -h`                 | display all options                               |
| `build-package.py -t beta`            | build for beta (local)                            |
| `build-package.py -t beta -u`         | build for beta (with upload on pcloud)            |
| `build-package.py -t marketplace -u`  | build for marketplace (with upload on pcloud)     |

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
