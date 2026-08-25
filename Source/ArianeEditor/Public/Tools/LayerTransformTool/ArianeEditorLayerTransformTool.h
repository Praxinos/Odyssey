// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
// Ariane
#include "ArianeEditorTool.h"

#include "ArianeEditorLayerTransformTool.generated.h"

class FArianeEditor;
class UArianePainting3DComponent;
class UCanvasRenderTarget2D;
struct FArianeVertex;
class FEditorViewportClient;
class UArianeLayerDrawing;
class UTransformProxy;
class UCombinedTransformGizmo;
class UArianeLayer;

UCLASS()
class ARIANEEDITOR_API UArianeEditorLayerTransformTool : public UArianeEditorTool
{
    GENERATED_BODY()

    struct FTransformSnapshot
    {
        FTransformSnapshot( UArianeLayer* InLayer, const FTransform& InTransform )
            : Layer ( InLayer )
            , Transform ( InTransform )
        {
        }

        UArianeLayer* Layer;
        FTransform Transform;
    };

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
                            , FSceneView* View
                            , const FKey& iKey
                            , const FArianePointerState& State
                            , bool iRepeat = false ) override;
    virtual void OnMouseHover( FEditorViewportClient* iViewportClient
                             , FSceneView* View
                             , const FArianePointerState& State ) override;
    virtual bool OnMouseDrag( FEditorViewportClient* iViewportClient
                            , FSceneView* View
                            , const FKey& iKey
                            , const FArianePointerState& State ) override;
    virtual bool OnMouseUp( FEditorViewportClient* iViewportClient
                          , FSceneView* View
                          , const FKey& iKey
                          , const FArianePointerState& State ) override;

    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void Init( FArianeEditor* InEditor ) override;

    void SetPreviousTool( UArianeEditorTool* InPreviousTool );

protected:
    virtual void ExtendContextMenu( FMenuBuilder& menu ) override;
    void OnTransformChanged( UTransformProxy* Proxy, FTransform NewTransform );
    void BindComponentDelegates();
    void UnbindComponentDelegates();
    void ResetGizmo();
    void ClearGizmo();
    void CreateGizmo();
    void CreateOverlayWidget();
    FReply OnAccept();
    FReply OnCancel();
    void OnRootFolderUpdate( bool Interactive );
    void OnPre3DPaintingComponentSelection();
    void OnPost3DPaintingComponentSelection();

protected:
    UPROPERTY() // to prevent GC
    UTransformProxy* TransformProxy;

    UPROPERTY() // to prevent GC
    UCombinedTransformGizmo* Gizmo;

    TSharedPtr<SWidget> OverlayWidget;
    TArray<FTransformSnapshot> TransformSnapshots;

    UArianeEditorTool* PreviousTool;
};
