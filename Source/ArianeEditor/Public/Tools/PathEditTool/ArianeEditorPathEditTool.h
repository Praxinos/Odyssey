// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
// Ariane Editor Headers
#include "ArianeEditorTool.h"
#include "ArianeEditorPathEditToolBuilder.h" // that way only this header needs to be included by files using this tool
// Ariane Headers
#include "ArianePainting3DComponent.h" // for EArianePainting3DGeometryMode
#include "ArianePath.h"

// Odyssey
#include "OdysseyPainterEditorColorType.h"

#include "ArianeEditorPathEditTool.generated.h"

class FArianeEditor;
struct FArianePath;
struct FArianePoint;
struct FArianeVertex;
struct FArianeSegment;
struct FArianeHandleSegment;

UENUM()
enum class EArianePathEditToolEditionMode : uint8
{
    Vertex = 0,
    VertexHandle = 1,
    SegmentHandle = 2,
    Alter = 3 // Add, Remove or cut
};

UENUM(BlueprintType)
enum class EArianeEditorPathEditToolSegmentType : uint8
{
    Polyline = 0,
    CubicBezier = 1,
};

UCLASS()
class ARIANEEDITOR_API UArianeEditorPathEditTool : public UArianeEditorTool
{
GENERATED_BODY()

    // struct that stores the ratio of handleLength / segmentLength at mouseDown
    // This allows us to adjust the handle length when moving vertices.
    struct FSegmentAdjustment
    {
        double HandleRatio[2];
        FArianeSegment* Segment;

        FSegmentAdjustment( FArianeSegment* InSegment );
        void Adjust();
    };

    // Struct that stores the displacement plane and the vertex' original position, so that we don't have to recompute each time.
    struct FPointDisplacement
    {
        ~FPointDisplacement();
        FPointDisplacement( FArianePoint* Point
                          , const FVector& InWorldPlaneNormal
                          , const FTransform& Transform
                          , const FVector& RayOrigin
                          , const FVector& RayDirection );


    public:
        FVector LocalPosition;
        FVector WorldPosition;
        FVector WorldRayPositionAtDown;
        FPlane WorldPlane;
    };

public:
    static FString GetStaticType() { return "ArianeEditor_PathEditTool"; };
    virtual FString GetType() override { return GetStaticType(); };

public:
    // Destructor
    virtual ~UArianeEditorPathEditTool();

    //Constructor
    UArianeEditorPathEditTool();

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
    virtual bool SupportsColorType( EOdysseyPainterEditorColorType ColorType ) override;

    virtual void Render(IToolsContextRenderAPI* RenderAPI) override;

    void Activate();
    void Inactivate();
    virtual void DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI ) override;
    virtual bool OnKeyUpGlobal( const FKeyEvent& InKeyEvent ) override;
    virtual bool OnKeyDownGlobal( const FKeyEvent& InKeyEvent ) override;

    virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;

protected:
    virtual void ExtendContextMenu( FMenuBuilder& menu ) override;

    /**
     * @brief Add a new vertex to the current Path. Create a segment between this vertex and the previously created one.
     * @param ViewportClient
     * @param State the state of the input device (mouse or stylus)
     */
    void PlotVertex( FEditorViewportClient* ViewportClient
                   , const FArianePointerState& State
                   , bool bInteractive );

    FArianeEditorHUD::FDrawingFlags EditonModeToHUDDrawingFlags();
    FPickingFlags EditonModeToPickingFlags();

    static void BuildSegmentAdjustments( const TArray<FArianeSegment*>& Segments
                                       , TArray<FSegmentAdjustment>& OutSegmentAdjustments );
    void OnMouseDownPickPoint( FEditorViewportClient* ViewportClient
                             , FSceneView* View
                             , const FKey& iKey
                             , const FArianePointerState& PointerState
                             , bool iRepeat );
    void OnMouseUpDeletePoint( const TArray<FArianePoint*>& PickedPoints );

    static void DisplacePoint( FArianePoint* Point
                             , const FPointDisplacement& PointDisplacment
                             , const FTransform& Transform
                             , const FVector& RayOrigin
                             , const FVector& RayDirection );
    static void DragVertexHandle( FArianeVertex* Point
                                , const TArray<FArianePath*>& SelectedPaths
                                , const FPointDisplacement& PointDisplacment
                                , const FTransform& Transform
                                , const FVector& RayOrigin
                                , const FVector& RayDirection
                                , bool bInWidenAllAlong );

    const FSlateBrush* GetBackgroundBrush( EArianePathEditToolEditionMode iMode ) const;
    void SetEditionMode( EArianePathEditToolEditionMode iMode );
    EArianePathEditToolEditionMode GetEditionMode();
    TSharedRef<SWidget> CreateModifierSegmentControl();
    void RebuildQuadTree( FEditorViewportClient* ViewportClient
                        , FSceneView* View );
    void OnPostUpdate( bool bInteractive );
    void Reset();
    void ResetQuadTree();
    void BindDelegates();
    void UnbindDelegates();
    void OnPreLayerStackSelectionChanged();
    void OnPostLayerStackSelectionChanged();
    void OnPostImageChanged();

public:
    UPROPERTY( EditAnywhere
             , Category=PathEditTool
             , meta = ( ToolTip  = "Picking Radius"
                      , ClampMin = "0"
                      , UIMin    = "0"
                      , LinearDeltaSensitivity = "15"
                      , Delta = "1" ) )
    uint32 PickingRadius;

    UPROPERTY( EditAnywhere
             , Category=PathEditTool
             , meta = ( ToolTip = "Widen All Along" ) )
    bool bWidenAllAlong;

protected:
    TArray<FArianeVertex*> PickedVertices;
    TArray<FPointDisplacement> PickedVertexDisplacements;
    TArray<FArianeHandleSegment*> PickedHandles;
    TArray<FPointDisplacement> PickedHandleDisplacements;
    TArray<FSegmentAdjustment> SegmentAdjustments;
    TArray<FArianePath*> SelectedPaths;
    EArianePathEditToolEditionMode EditionMode;
    TArray<FArianePoint*> HoveredPoints;
    TArray<FArianeObject*> SelectedTrees;
};
