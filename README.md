# Epos

# Setting Hooks (after cloning)

- go to `.git/hooks` folder
- rename `pre-commit.sample` to `pre-commit`
- open `pre-commit` file
- add the line at the end of the file: `exec .githooks/check.py`

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
    - MAY be used for referencing issues like: `close #234` or `close #123, #245, #992`
    - MAY be used for BREAKING-CHANGE

## Example

```
refactor(EposTracksEditor): move Board* headers to private
```
```
feat: add key thumbnail section for single cameracut track

this may be improved because base class FTrackEditorThumbnail is not virtualized and FThumbnailSection class owns a plain object of FTrackEditorThumbnail class instead of a pointer
```
</details>

## References

- http://karma-runner.github.io/4.0/dev/git-commit-msg.html
- https://github.com/angular/angular/blob/master/CONTRIBUTING.md#commit-body
- https://www.conventionalcommits.org/en/v1.0.0/

# Module hierarchy

```
    ┌──────────────────────────────────────────────────────────────────────────────────┐
    │                             EposMovieScene                                       │
    └───────────────────┬───────────────┬────────────────────┬─────────────────────┬───┘
                        │               │                    │                     │
    ┌───────────────────┴──────────┐    │                    │                     │
    │          EposTracks          │    │                    │                     │
    └───┬───────┬───────┬──────────┘    │                    │                     │
        │       │       │               │                    │                     │
        │       │   ┌───┴───────────────┴──────────┐         │                     │
        │       │   │         EposSequence         │         │                     │
        │       │   └─┬─────────────┬──────────┬───┘         │                     │
        │       │     │             │          └─────────────│─────────────┐       │
        │       │     │             │                        │             │       │
        │       │     │             │                        │             │       │
        │       │     │             │                        │             │       │
        │       │     │   ┌─────────┴────────────────────┐   │             │       │
        │       │     │   │     EposMovieSceneTools      │   │             │       │
        │       │     │   └─────────────────┬────────┬───┘   │             │       │
        │       │     │                     │        └───────│──────┐      │       │
        │   ┌───┴─────┴─────────────────────┴────────────────┴─┐    │      │       │
        │   │                 EposTracksEditor                 │    │      │       │
        │   └───────────────────────────┬──────────────────────┘    │      │       │
        │                               │                           │      │       │
    ┌───┴───────────────────────────────┴───────────────────────────┴──────┴───────┴───┐
    │                               EposSequenceEditor                                 │
    └──────────────────────────────────────────────────────────────────────────────────┘
```