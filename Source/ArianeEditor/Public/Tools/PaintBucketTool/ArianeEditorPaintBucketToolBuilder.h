// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
// Ariane
#include "ArianeEditorToolBuilder.h"

#include "ArianeEditorPaintBucketToolBuilder.generated.h"

class UArianeEditorTool;
class UInteractiveToolManager;

UCLASS()
class ARIANEEDITOR_API UArianeEditorPaintBucketToolBuilder : public UArianeEditorToolBuilder
{
    GENERATED_BODY()

public:
    UArianeEditorPaintBucketToolBuilder();

    virtual UArianeEditorTool* CreateTool( UInteractiveToolManager* ToolManager ) override;
};
