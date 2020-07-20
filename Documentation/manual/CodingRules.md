# Coding Rules
The following documents provides the **Coding Rules** for this project.

#### Classes
|Type                       |Prefix                 |
|---------------------------|-----------------------|
|Model / Utility class      |**F**Class             |
|Model / Utility struct     |**F**Struct            |
|GUI class                  |**S**Class             |
|Abstract / Interface class |**I**Class             |
|Template class             |**T**Class             |

#### Namespaces
|Type           |Prefix                                 |
|---------------|---------------------------------------|
|Library        |5-char max name ( `ULIS, Rivet` )      |
|Application    |**n**Example (`nWindow, nApplication`) |
- Namespace begin: `namespace nExample {`
- Namespace end: `}; // namespace nExample`
- Explicit access from Global namespace: `::nApplication::nWindow:: ... `
- Never use the `using` directive, unless in a local function or function-like scope, especially for `std::`

### Include Directives
- Use `#pragma once`
- Include directives should be sorted first by type, then by name in alphanumeric order:
    - Application
    - Library1
    - Library2
    - C++ Standard Library Headers
    - C Headers
    - Platform-Specific
- Includes of Application files use double quotes:`""`
- Includes of Library files use brackets:`<>`

### Member Variables
- **Member variables** of class should generally be private and start with lowercase prefix "**m**":
    ```
    uint32 mCount;
    ```
- **Member variables** of struct should generally be public and be named with a short explicit name, you can ommit any prefix:
    ```
    int width;
    ```

### Typedefs
- **`typedef`** on template class instanciations can be named with uppercase prefix "**F**":
    ```
    typedef TNode< int > FNodeInt; // OK.
    ```

- **`typedef`** of a semantic nature about inheritance and object identity inside classes and structs should be defined as private as such:
    ```
    template< typename T >
    class  TAbstractNode< T > : public INode
    {
    private:
        typedef  INode                  tSuperClass;
        typedef  TAbstractNode< T >     tSelf;
    };
    ``` 

### Forward Declarations
- You should use forward declarations as much as possible:
    ```
    namespace nApplication { namespace nWindow { class SCanvas; } }
    ```

### return
- You should leave two space after a `return` statement:
    ```
    return  nullptr;
    ```

### nullptr
- Prefere `nullptr` to `NULL` or `0`

### delete / new
- You should leave two space after a `delete` or `new` statement:
    ```
    new  int[8];
    new  SCanvas();
    delete  []  mVar;
    delete  mVar;
    ```

### Ternary Operator
- Avoid clustering too much with ternary conditional operators ( 1 level max ):
    ```
    int var1 = condition_A ? 0 : 1;                     // Yes !
    int var2 = condition_A ? condition_B ? 0 : 1 : 2;   // Clumsy !
    ```

### Spacing
- One space beetween a variable type and the variable name:
- One space beetween a function type and the function name:
    ```
    int var;
    void Foo();
    ```
- Avoid trailing spaces.

### Templates
- As powerful as it can be, use templates sparingly as it can lead to unmanageable code or template bloating very quickly.

### License Header
- Comment headers:
```
// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
```

### Type Consistency And Possible Loss Of Data.
- Solve all warnings of the form "about C4244: conversion from 'X' to 'Y', possible loss of data".

### Casts
- Always use C++ style cast such as static_cast, dynamic_cast, and such. Never use c-style casts.

### Cross-Platform
- Always target the broadest set of platforms. Never make assumptions about the target OS ( the only exception is to always target 64-bit, we don't care about 32-bit ). If some feature need platform specific implementation, use the available macros to dispatch the appropriate implementation at compile time.

### Aim For Zero Warnings
- Aim for zero compile time warnings at all times.

### Lines Limit Per File
- Whenever possible, keep the line count per file as low as possible. A file that has over 500 lines of code is considered bloated.
