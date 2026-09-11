// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "GenericPlatform/ICursor.h"

/**
 * Mouse cursor custom types
 */
enum class EMouseCursorCustom: uint8
{
    None,
    Cross,
    Dot,
    Circle,
    CircleClockwise,
    CircleAntiClockwise,

    // When adding new entry, don't forget to add the corresponding cursor file in FMouseCursor::InitializeCustomCursorMap()
};

class ODYSSEYCOREEDITOR_API FMouseCursor
{
public:
    // Avoid using it, prefer to use the one with parameter !
    FMouseCursor();
    FMouseCursor( EMouseCursor::Type iMouseCursor );
    FMouseCursor( EMouseCursorCustom iMouseCursor );

    ~FMouseCursor();

public:
    bool IsMouseCursorNative() const;

    EMouseCursor::Type              GetMouseCursorNative() const;
    TOptional<EMouseCursorCustom>   GetMouseCursorCustom() const;

    FMouseCursor& SetMouseCursorNative( EMouseCursor::Type iMouseCursor );
    FMouseCursor& SetMouseCursorCustom( EMouseCursorCustom iMouseCursorCustom );

    void UpdateCursor() const;

public:
    //TODO: Should not be used, must be fixed in some vector tools by adding a state variable (?)
    friend bool operator==( FMouseCursor A, FMouseCursor B )
    {
        return A.mMouseCursorNative == B.mMouseCursorNative
            && A.mMouseCursorCustom == B.mMouseCursorCustom;
    }

private:
    static TMap<EMouseCursorCustom, void*> smMouseCursorCustomMap;
    static void InitializeCustomCursorMap();

private:
    EMouseCursor::Type                  mMouseCursorNative;
    TOptional<EMouseCursorCustom>       mMouseCursorCustom;
};
