// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "EditorSubsystem.h"

#include "ArianeEditorSubsystem.generated.h"

class UArianeLayer;
struct FArianeObject;

USTRUCT()
struct ARIANEEDITOR_API FArianeClipboard
{
    GENERATED_BODY()

public:
    // Not a UPROPERTY: FArianeObject is not a UObject
    TArray<FArianeObject*> CopiedObjects;

    // UPROPERTY so that the garbage collector keeps the copies alive until they are pasted
    UPROPERTY()
    TArray<TObjectPtr<UArianeLayer>> CopiedLayers;
};

/**
 * Editor-wide Ariane data, shared by every Ariane editor instance and kept alive as long as the Unreal editor.
 */
UCLASS()
class ARIANEEDITOR_API UArianeEditorSubsystem : public UEditorSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FArianeClipboard Clipboard;
};
