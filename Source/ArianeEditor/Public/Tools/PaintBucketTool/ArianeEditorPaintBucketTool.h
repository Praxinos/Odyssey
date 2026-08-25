// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
// Ariane
#include "ArianeEditorTool.h"
#include "ArianePath.h"
#include "ArianeSegment.h"
#include "ArianeVertex.h"
#include "ArianeGraph.h"
#include "ArianeCycle.h" // for enums

#include "ArianeEditorPaintBucketTool.generated.h"


class FArianeEditor;
class UArianePainting3DComponent;
class UCanvasRenderTarget2D;
struct FArianeVertex;
class FEditorViewportClient;
class UArianeLayerDrawing;

UCLASS()
class ARIANEEDITOR_API UArianeEditorPaintBucketTool : public UArianeEditorTool
{
    GENERATED_BODY()



public:
/*
    enum class ESegmentAdditionFlags : uint8
    {
        None                  =        0  ,
        KeepOriginalSegment   = ( 1 << 0 ),
        RemoveOriginalSegment = ( 1 << 1 ),
        CreateDerivedSegment  = ( 1 << 2 ),
        CreateNewPath         = ( 1 << 3 )
    };
*/
public:
    static FString GetStaticType() { return "ArianeEditor_PaintBucketTool"; };
    virtual FString GetType() override { return GetStaticType(); };

public:
    // Destructor
    virtual ~UArianeEditorPaintBucketTool();

    //Constructor
    UArianeEditorPaintBucketTool();

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

    virtual void DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI ) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void Init( FArianeEditor* InEditor ) override;
    virtual void Activate() override;
    virtual void Inactivate() override;


protected:
    virtual void ExtendContextMenu( FMenuBuilder& menu ) override;
    void DrawCycleHUD ( FCanvas* HUDCanvas
                      , IToolsContextRenderAPI* RenderAPI
                      , FEditorViewportClient* ViewportClient
                      , FSceneView* View
                      , const FLinearColor& HcColor
                      , FArianeGraph::FCycle* Cycle );
    void DrawSectionsHUD ( FCanvas* HUDCanvas
                         , IToolsContextRenderAPI* RenderAPI
                         , FEditorViewportClient* ViewportClient
                         , FSceneView* View
                         , const FLinearColor& HcColor
                         , const TArray<FArianeGraph::FSection*>& Sections );
    void Reset();
    void OnCameraMoved( const FVector& Location, const FRotator& Rotation, ELevelViewportType ViewportType, int32 ViewIndex );
    virtual void Render(IToolsContextRenderAPI* RenderAPI) override;
    void UnbindDelegates();
    void BindDelegates();
    void OnPreUpdate( bool bInteractive );
    void OnPostUpdate( bool bInteractive );

public:
    UPROPERTY( EditAnywhere
             , Category = PaintBucketTool )
    UMaterialInterface* MaterialInterface;

    UPROPERTY( EditAnywhere
             , Category = PaintBucketTool )
    double GapTolerance;

    UPROPERTY( EditAnywhere
             , Category = PaintBucketTool )
    EArianeCycleFittingRule FittingRule;

protected:
    FArianeGraph* Graph;
    FArianeGraph::FCycle* PickedCycle;
    bool bGraphNeedsUpdate;
    FTSTicker::FDelegateHandle CameraMoveHandle;
    bool bShowGrid;
};

// define bitwise op
//ENUM_CLASS_FLAGS(UArianeEditorPaintBucketTool::ESegmentAdditionFlags)
