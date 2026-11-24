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
#include "ArianeGraphWorker.h"
#include "ArianeCycle.h" // for enums

#include "ArianeEditorPaintBucketTool.generated.h"


class FArianeEditor;
class UArianePainting3DComponent;
class UCanvasRenderTarget2D;
struct FArianeVertex;
class FEditorViewportClient;
class UArianeLayerVector;

UENUM(BlueprintType)
enum class EArianeEditorPaintBucketToolLayerPolicy : uint8
{
    CurrentAndAbove,
    Selected,
    CurrentAndBelow,
    All
};

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
    void DrawExtensionHUD ( FCanvas* HUDCanvas
                          , IToolsContextRenderAPI* RenderAPI
                          , FEditorViewportClient* ViewportClient
                          , FSceneView* View
                          , const FLinearColor& HcColor );
    void DrawProgressBarHUD( FCanvas* HUDCanvas
                           , FEditorViewportClient* ViewportClient
                           , const FLinearColor& BgColor
                           , const FLinearColor& HcColor );
    /** The drawing or an import setting changed: re-import, then solve. Never touches the graph itself. */
    void RequestImport();
    /** Launches a solve if the worker is idle, importing first if needed. */
    void RequestSolve();
    void OnSolveFinished();
    void StartSolveTicker();
    void GatherGraphedObjects( TArray<FArianeObject*>& OutObjects );
    void OnCameraMoved( const FVector& Location, const FRotator& Rotation, ELevelViewportType ViewportType, int32 ViewIndex );
    virtual void Render(IToolsContextRenderAPI* RenderAPI) override;
    void UnbindDelegates();
    void BindDelegates();
    void OnPreUpdate( bool bInteractive );
    void OnPostUpdate( bool bInteractive );

    void GetPaintedLayers( TArray<UArianeLayer*>& OutPaintedLayers );

public:
    UPROPERTY( EditAnywhere
             , Category = PaintBucketTool
             , meta = ( ToolTip  = "Increase or decrease the tolerance of a gap between 2 paths."
                      , ClampMin = "0.0"
                      , UIMin    = "0.0"
                      , LinearDeltaSensitivity = "15"
                      , Delta = "1" ) )
    double GapTolerance;

    UPROPERTY( EditAnywhere
             , Category = PaintBucketTool )
    EArianeGraphSegmentExtensionScheme SegmentExtensionScheme;

    UPROPERTY( EditAnywhere
             , Category = PaintBucketTool )
    EArianeCycleFittingRule FittingRule;

    UPROPERTY( EditAnywhere
             , Category = PaintBucketTool )
    EArianeEditorPaintBucketToolLayerPolicy LayerPolicy;

    UPROPERTY( EditAnywhere
             , Category = PaintBucketTool )
    UMaterialInterface* MaterialInterface;

protected:
    // shared with the worker task, so it outlives the tool if a solve is still running
    TSharedPtr<FArianeGraph> Graph;
    FArianeGraphWorker Worker;
    FArianeGraph::FCycle* PickedCycle;
    // the graph is being solved or is stale: do not read it
    bool bGraphNeedsUpdate;
    // incremented on each camera or drawing change, a solve with an older value is stale
    uint32 Generation;
    uint32 SolvedGeneration;
    bool bGeometryNeedsImport;
    // a new graph arrived: pick at the last cursor position on next DrawHUD
    bool bNeedsPick;
    bool bIsActive;
    FVector2D LastCursorViewportPosition;
    FTSTicker::FDelegateHandle CameraMoveHandle;
    FTSTicker::FDelegateHandle SolveTickerHandle;
};

// define bitwise op
//ENUM_CLASS_FLAGS(UArianeEditorPaintBucketTool::ESegmentAdditionFlags)
