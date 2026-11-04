// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
// Ariane Editor Headers
#include "ArianeEditorTool.h"
#include "ArianeEditorPathPushToolBuilder.h" // that way only this header needs to be included by files using this tool
// Ariane Headers
#include "ArianePainting3DComponent.h" // for EArianePainting3DGeometryMode
#include "ArianePath.h"
#include "ArianeSegment.h"

// Odyssey
#include "OdysseyPainterEditorColorType.h"

#include "ArianeEditorPathPushTool.generated.h"

class FArianeEditor;
struct FArianePath;
struct FArianePoint;
struct FArianeVertex;
struct FArianeSegment;
struct FArianeHandleSegment;

UCLASS()
class ARIANEEDITOR_API UArianeEditorPathPushTool : public UArianeEditorTool
{
GENERATED_BODY()
    struct FInfluencedPoint
    {
        FArianePoint* Point;
        FArianeObject* Owner;
    };

    // Struct that stores the displacement plane and the vertex' original position, so that we don't have to recompute each time.
    struct FPointDisplacement
    {
        ~FPointDisplacement();
        FPointDisplacement( FArianePoint* InPoint
                          , bool bInMapped
                          , double InScreenDistance
                          , const FVector& InWorldPlaneNormal
                          , const FTransform& Transform
                          , const FVector& RayOrigin
                          , const FVector& RayDirection );


    public:
        FArianePoint* Point;
        bool bMapped;
        double ScreenDistance; // use -1.0f if the point is behind the screen
        FVector LocalPosition;
        FVector WorldPosition;
        FVector WorldRayPositionAtDown;
        FPlane WorldPlane;
    };

    struct FVertexDisplacement : public FPointDisplacement
    {
        ~FVertexDisplacement();
        FVertexDisplacement( FArianeVertex* InVertex
                           , bool bInMapped
                           , double InScreenDistance
                           , const FVector& InWorldPlaneNormal
                           , const FTransform& Transform
                           , const FVector& RayOrigin
                           , const FVector& RayDirection );

    public:
        bool bHandleAligned;
        bool bMapped;
    };

public:
    static FString GetStaticType() { return "ArianeEditor_PathPushTool"; };
    virtual FString GetType() override { return GetStaticType(); };

public:
    // Destructor
    virtual ~UArianeEditorPathPushTool();

    //Constructor
    UArianeEditorPathPushTool();

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

    void Activate();
    void Inactivate();
    virtual void DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI ) override;
    virtual void Render(IToolsContextRenderAPI* RenderAPI) override;
    virtual bool OnKeyUpGlobal( const FKeyEvent& InKeyEvent ) override;
    virtual bool OnKeyDownGlobal( const FKeyEvent& InKeyEvent ) override;

    virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;
    bool IsMouseButtonDown();

protected:
    virtual void ExtendContextMenu( FMenuBuilder& menu ) override;

    static void DisplacePoint( const FPointDisplacement& PointDisplacment
                             , const FTransform& Transform
                             , const FVector& RayOrigin
                             , const FVector& RayDirection );
    static FPointDisplacement* GetPointDisplacement( TArray<FPointDisplacement>& PointDisplacements, FArianePoint* Point );
    static double GetPointMaxDistance( TArray<FArianeSegment*> Segments );
    void ComputeRatios( TArray<FPointDisplacement>& PointDisplacements, double MaxScreenDistance );
    static FPointDisplacement* CreateOrGetPointDisplacement( FSceneView* View
                                                           , const FVector2D& MouseScreenPosition
                                                           , TArray<FPointDisplacement>& PointDisplacements
                                                           , const FPlane& DrawingPlane
                                                           , const FTransform& OwnerTransform
                                                           , const FVector& RayOrigin
                                                           , const FVector& RayDirection
                                                           , FArianePoint* Point );
    double ParseInfluencedPoints( FSceneView* View
                            , const FVector2D& MouseScreenPosition
                            , const FPlane& DrawingPlane
                            , const FVector& RayOrigin
                            , const FVector& RayDirection
                            , const TArray<FInfluencedPoint>& InfluencedPoints );

    TSharedRef<SWidget> CreateModifierSegmentControl();
    void RebuildQuadTree( FEditorViewportClient* ViewportClient
                        , FSceneView* View );
    void OnPostUpdate( bool bInteractive, UArianePainting3DComponent* Painting3DComponent );
    void Reset();
    void ResetQuadTree();
    void BindEditorDelegates();
    void UnbindEditorDelegates();
    void OnPreLayerStackSelectionChanged( UArianeLayerStack* LayerStack );
    void OnPostLayerStackSelectionChanged( UArianeLayerStack* LayerStack );
    void OnPostDrawingChanged( UArianeLayerVector* VectorLayer );
    void BindComponentDelegates( UArianePainting3DComponent* Painting3DComponent );
    void UnbindComponentDelegates( UArianePainting3DComponent* Painting3DComponent );
    void BindLayerStackDelegates( UArianeLayerStack* LayerStack );
    void UnbindLayerStackDelegates( UArianeLayerStack* LayerStack );
    void BindVectorLayerDelegates( UArianeLayerVector* VectorLayer );
    void UnbindVectorLayerDelegates( UArianeLayerVector* VectorLayer );

public:
    UPROPERTY( EditAnywhere
             , Category=PathPushTool
             , meta = ( ToolTip  = "Picking Radius"
                      , ClampMin = "0"
                      , UIMin    = "0"
                      , LinearDeltaSensitivity = "15"
                      , Delta = "1" ) )
    uint32 PickingRadius;

protected:
    bool bMouseButtonDown;
    FVector2D MouseScreenPositionAtDown;
    FVector2D MouseScreenPositionAtDrag;
    TArray<FPointDisplacement> InfluencedHandleDisplacements;
    TArray<FVertexDisplacement> InfluencedVertexDisplacements;
    TArray<FArianeSegment*> PickedSegments;
    TArray<FArianeObject*> SelectedTrees;
    double MaxScreenDistanceToMouse;
};
