// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
// Ariane
#include "ArianeEditorTool.h"

#include "ArianeEditorToolBuilder.generated.h"

class FArianeEditor;
class UInteractiveToolManager;

UCLASS(Abstract)
class ARIANEEDITOR_API UArianeEditorToolBuilder : public UInteractiveToolBuilder
{
    GENERATED_BODY()

public:
    ~UArianeEditorToolBuilder();
    UArianeEditorToolBuilder();
    UArianeEditorToolBuilder( const FString& InType );

    FString GetType();
    void Init( FArianeEditor* InEditor );
    virtual UArianeEditorTool* CreateTool( UInteractiveToolManager* ToolManager );

protected:
    virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; };
    UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;


protected:
    UPROPERTY() // prevent GC
    mutable UArianeEditorTool* Tool;

    FArianeEditor* Editor;
    FString Type;
};
