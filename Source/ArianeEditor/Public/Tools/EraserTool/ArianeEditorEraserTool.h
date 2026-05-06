// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
// Ariane
#include "ArianeEditorTool.h"
#include "ArianeEditorEraserToolBuilder.h" // that way only this header needs to be included by files using this tool
#include "ArianePath.h"
#include "ArianeSegment.h"
#include "ArianeVertex.h"

#include "ArianeEditorEraserTool.generated.h"


class FArianeEditor;
class UArianePainting3DComponent;
class UCanvasRenderTarget2D;
struct FArianeVertex;
class FEditorViewportClient;
class UArianeLayerDrawing;

UCLASS()
class ARIANEEDITOR_API UArianeEditorEraserTool : public UArianeEditorTool
{
    GENERATED_BODY()

    struct FWayFragment;

    // a waypoint is met at segment vertex or when a constrast is met
    struct FWayPoint
    {
        //FArianeVertex* IntersectionVertex;
        FArianeVertex* OriginalVertex;
        FArianeVertex* AssignedVertex;
        double T;
        uint32 Flags;
        TArray<FWayFragment*> Fragments;

        // FWayPoint flags
        static const uint32 Original           = ( 1 << 0 );
        static const uint32 OutsideErasureArea = ( 1 << 1 );
        static const uint32 BordersErasureArea = ( 1 << 2 );
        static const uint32 InsideErasureArea  = ( 1 << 3 );

        FWayPoint()
        {
            Flags = 0;
        }

        FWayPoint( FArianeVertex* InOriginalVertex, uint32 iWayPointFlags )
        {
            //intersectionVertex = nullptr;
            OriginalVertex = InOriginalVertex;
            AssignedVertex = nullptr;
            T = 0.0f;
            Flags = iWayPointFlags;

            Fragments.Reserve( 2 );
        }

        FWayPoint( double iT
                 , uint32 iWayPointFlags )
        {
            OriginalVertex = nullptr;
            AssignedVertex = nullptr;
            T = iT;
            Flags = iWayPointFlags;

            Fragments.Reserve( 2 );
        }
    };

    struct FMetaFragment
    {
        FArianeSegment* Segment;
        uint32 WayPoint0Index;
        uint32 WayPoint1Index;
        bool bErased;

        FMetaFragment( FArianeSegment* InSegment
                     , uint32 iWayPoint0Index
                     , uint32 iWayPoint1Index
                     , bool bInErased )
            : Segment( InSegment )
            , WayPoint0Index( iWayPoint0Index )
            , WayPoint1Index( iWayPoint1Index )
            , bErased( bInErased )
        {
        }
    };

    struct FWayFragment
    {
        FArianeSegment* Segment;
        FWayPoint* WayPoint0;
        FWayPoint* WayPoint1;
        //::ULIS::FVec2D bezier[4];
        bool bErased;
        float T0;
        float T1;

        FWayFragment( FArianeSegment* InSegment
                    , FWayPoint* InWayPoint0
                    , FWayPoint* InWayPoint1
                    , bool bInErased );

        FWayFragment* GetNext();
        FWayFragment* GetPrev();
    };

    struct ChainProcessor
    {
        TArray<FArianePath*> AddedPaths;
        TArray<FArianeVertex*> AddedVertices;
        TArray<FArianeSegment*> AddedSegments;
        TArray<FArianeVertex*> RemovedVertices;
        TArray<FArianeSegment*> RemovedSegments;
        TArray<FWayPoint> WayPoints;
        TArray<FWayFragment> WayFragments;
    };

public:
    enum class ESegmentAdditionFlags : uint8
    {
        None                  =        0  ,
        KeepOriginalSegment   = ( 1 << 0 ),
        RemoveOriginalSegment = ( 1 << 1 ),
        CreateDerivedSegment  = ( 1 << 2 ),
        CreateNewPath         = ( 1 << 3 )
    };

    enum class EVertexAdditionFlags : uint8
    {
        None                 =        0  ,
        KeepOriginalVertex   = ( 1 << 0 ),
        RemoveOriginalVertex = ( 1 << 1 ),
        CreateDerivedVertex  = ( 1 << 2 ),
        CreateBoundaryVertex = ( 1 << 3 )
    };

public:
    static FString GetStaticType() { return "ArianeEditor_EraserTool"; };
    virtual FString GetType() override { return GetStaticType(); };

public:
    // Destructor
    virtual ~UArianeEditorEraserTool();

    //Constructor
    UArianeEditorEraserTool();

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

    virtual void DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI ) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void Init( FArianeEditor* InEditor ) override;

protected:
    virtual void ExtendContextMenu( FMenuBuilder& menu ) override;

    bool ErasePaths( FEditorViewportClient* ViewportClient
                   , UArianePainting3DComponent* Painting3DComponent );
    bool EraseChainSegments( FEditorViewportClient* ViewportClient
                           , FSceneView* View
                           , UArianeLayerDrawing* DrawingLayer
                           , FArianePath* Path
                           , const FArianePath::Chain& Chain
                           , const TArray<FColor>& Pixels
                           , TArray<FWayPoint>& oWayPointBuffer
                           , TArray<FWayFragment>& oWayFragmentBuffer );
    void VertexToWaypoint( FEditorViewportClient* ViewportClient
                         , FSceneView* View
                         , const FTransform& WorldTransform
                         , FArianeVertex* Vertex
                         , const TArray<FColor>& Pixels
                         , TArray<FWayPoint>& oWayPointBuffer );
    uint8 GetAlpha( int32 X, int32 Y, const TArray<FColor>& Pixels );
    void TraceLine( FArianePath* Path
                  , FArianeSegment* Segment
                  , const FArianeSegment::FractionStep* Step0
                  , int32 ScreenX0
                  , int32 ScreenY0
                  , const FArianeSegment::FractionStep* Step1
                  , int32 ScreenX1
                  , int32 ScreenY1
                  , const TArray<FColor>& Pixels
                  , TArray<FWayPoint>& OutWayPointBuffer
                  , TArray<FMetaFragment>& OutMetaFragmentBuffer
                  , bool iRevert );
    static FVector2D ProjectWorldToScreen( FEditorViewportClient* ViewportClient
                                         , FSceneView* View
                                         , const FVector& WorldPosition );
    static bool CheckContrast( uint8 iAlphaValue0, uint8 iAlphaValue1 );
    void ResizeCanvas( uint32 Width, uint32 Height );
    void UpdateBrush();
    void ResizeBrush();
    void FillBrush();

    void StampBrush();
    void ClearCanvas();
    static FWayFragment* GetStartFragment( FWayFragment* Fragment );
    static ESegmentAdditionFlags SegmentAdditionPolicy( FWayFragment* InFragment, bool bSplit );
    static EVertexAdditionFlags VertexAdditionPolicy( FWayPoint* WayPoint, bool bSplit );
    void ParseChainWayPoints( UArianeLayerDrawing* DrawingLayer
                            , FArianePath* ChainPath
                            , TArray<FWayPoint>& WayPoints
                            , TArray<FWayFragment>& WayFragments
                            , TArray<FArianePath*>& OutAddedPaths
                            , TArray<FArianeVertex*>& OutAddedVertices
                            , TArray<FArianeSegment*>& OutAddedSegments
                            , TArray<FArianeVertex*>& OutRemovedVertices
                            , TArray<FArianeSegment*>& OutRemovedSegments );
    static FArianeVertex* AssignVertex( FArianePath* OwnerPath
                                      , FWayPoint& WayPoint
                                      , TArray<FArianeVertex*>& OutAddedVertices
                                      , TArray<FArianeVertex*>& OutRemovedVertices
                                      , bool bSplit );
    FBox2D GetPathBoundingArea( FEditorViewportClient* ViewportClient, FSceneView* View, FArianePath* Path );
    FBox2D GetErasureBoundingArea( FSceneView* View );

public:
    UPROPERTY( EditAnywhere
             , Category=EraserTool
             , meta = ( ToolTip = "Size"
                      , ClampMin = "0.0"
                      , UIMin = "0.0" ) )
    int Size;

    UPROPERTY( EditAnywhere
             , Category=EraserTool )
    bool bSplit;


protected:
    UPROPERTY() // prevent GC
    UCanvasRenderTarget2D* CanvasRenderTarget;

    UPROPERTY() // prevent GC
    UTexture2D* Brush;

    FVector2D MouseAtDown;
    FVector2D MouseAtUp;
    FIntVector2 MouseRecords[2];
};

// define bitwise op
ENUM_CLASS_FLAGS(UArianeEditorEraserTool::ESegmentAdditionFlags)
// define bitwise op
ENUM_CLASS_FLAGS(UArianeEditorEraserTool::EVertexAdditionFlags)
