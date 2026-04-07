// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
// Ariane
#include "ArianeEditorTool.h"
#include "ArianeEditorLayerTransformToolBuilder.h" // that way only this header needs to be included by files using this tool

#include "ArianeEditorLayerTransformTool.generated.h"

class FArianeEditor;
class UArianePainting3DComponent;
class UCanvasRenderTarget2D;
struct FArianeVertex;
class FEditorViewportClient;
class UArianeLayerDrawing;
class UTransformProxy;
class UCombinedTransformGizmo;

UCLASS()
class ARIANEEDITOR_API UArianeEditorLayerTransformTool : public UArianeEditorTool
{
    GENERATED_BODY()

public:
    static FString GetStaticType() { return "ArianeEditor_LayerTransformTool"; };
    virtual FString GetType() override { return GetStaticType(); };

public:
    // Destructor
    virtual ~UArianeEditorLayerTransformTool();

    //Constructor
    UArianeEditorLayerTransformTool();

    virtual void Activate() override;
    virtual void Inactivate() override;
    //Mouse events overrides
    virtual bool OnMouseDown( FEditorViewportClient* iViewportClient
                            , const FKey& iKey
                            , const FArianePointerState& State
                            , bool iRepeat = false ) override;
    virtual void OnMouseHover( FEditorViewportClient* iViewportClient
                             , const FArianePointerState& State ) override;
    virtual bool OnMouseDrag( FEditorViewportClient* iViewportClient
                            , const FKey& iKey
                            , const FArianePointerState& State ) override;
    virtual bool OnMouseUp( FEditorViewportClient* iViewportClient
                          , const FKey& iKey
                          , const FArianePointerState& State ) override;

    virtual void DrawHUD ( FEditorViewportClient* ViewportClient
                         , FViewport* Viewport
                         , const FSceneView* View
                         , FCanvas* Canvas ) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void Init( FArianeEditor* InEditor ) override;

protected:
    virtual void ExtendContextMenu( FMenuBuilder& menu ) override;
    void OnTransformChanged( UTransformProxy* Proxy, FTransform NewTransform );
    void BindDelegates();
    void UnbindDelegates();
    void ClearGizmo();
    void CreateGizmo();

protected:
    UPROPERTY() // to prevent GC
    UTransformProxy* TransformProxy;

    UPROPERTY() // to prevent GC
    UCombinedTransformGizmo* Gizmo;
};
