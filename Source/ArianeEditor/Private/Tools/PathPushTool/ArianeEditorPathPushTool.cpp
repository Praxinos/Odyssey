// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "PathPushTool/ArianeEditorPathPushTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"
// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianeDrawing.h"
#include "ArianeLayerVector.h"
#include "ArianeLayerFolder.h"
#include "ArianePath.h"
#include "ArianeGroup.h"
#include "ArianeCore.h"
#include "ArianeVertex.h"
#include "ArianeLayerStack.h"
#include "ArianeSegmentCubic.h"
// Odyssey Widgets
#include "SOdysseySinglePropertyView.h"
// Unreal headers
#include "Subsystems/EditorActorSubsystem.h"
#include "SceneView.h"
#include "Math/UnrealMathUtility.h"
#include "IStylusState.h"
#include "InteractiveToolManager.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "Math/Plane.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPathPushTool::FPointDisplacement::~FPointDisplacement()
{
}

UArianeEditorPathPushTool::FPointDisplacement::FPointDisplacement( FArianePoint* InPoint
                                                                 , bool bInMapped
                                                                 , double InScreenDistance
                                                                 , const FVector& InWorldPlaneNormal
                                                                 , const FTransform& Transform
                                                                 , const FVector& RayOrigin
                                                                 , const FVector& RayDirection )
{
    Point = InPoint;
    bMapped = bInMapped;
    LocalPosition = Point->GetPosition();
    WorldPosition = Transform.TransformPosition( LocalPosition );
    ScreenDistance = InScreenDistance;
    WorldPlane = FPlane( WorldPosition, InWorldPlaneNormal );

    FArianeCore::IntersectPlane( WorldPlane, RayOrigin, RayDirection, WorldRayPositionAtDown );
}

UArianeEditorPathPushTool::FVertexDisplacement::~FVertexDisplacement()
{
}

UArianeEditorPathPushTool::FVertexDisplacement::FVertexDisplacement( FArianeVertex* InVertex
                                                                   , bool bInMapped
                                                                   , double InScreenDistance
                                                                   , const FVector& InWorldPlaneNormal
                                                                   , const FTransform& Transform
                                                                   , const FVector& RayOrigin
                                                                   , const FVector& RayDirection )
    : FPointDisplacement( InVertex, bInMapped, InScreenDistance, InWorldPlaneNormal, Transform, RayOrigin, RayDirection )
    , bHandleAligned ( InVertex->IsHandleAligned() )
{
}

UArianeEditorPathPushTool::~UArianeEditorPathPushTool()
{

}

UArianeEditorPathPushTool::UArianeEditorPathPushTool()
    : PickingRadius( 25.0f )
    , bMouseButtonDown ( false )
{
    Icon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.PathPush64");

    bHasContextMenu = true;
}

void
UArianeEditorPathPushTool::OnPostUpdate( bool bInteractive, UArianePainting3DComponent* Painting3DComponent )
{
    if( bInteractive == false )
    {
        Reset();
    }
}

void
UArianeEditorPathPushTool::ResetQuadTree()
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

    if( ViewportClient )
    {

        FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                                , ViewportClient->GetScene()
                                                                                , ViewportClient->EngineShowFlags ) );
        // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
        FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );

        RebuildQuadTree( ViewportClient, View );
    }
}

void
UArianeEditorPathPushTool::Reset()
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(GetCurrentLayer());

    SelectedTrees.Empty();

    if( VectorLayer )
    {
        VectorLayer->GetDrawing()->GetSelectedTrees( SelectedTrees );

        // if nothing is selected, select the whole root group
        if( SelectedTrees.IsEmpty() )
        {
            SelectedTrees.Add( VectorLayer->GetDrawing()->GetRootGroup() );
        }
    }

    ResetQuadTree();
}

void
UArianeEditorPathPushTool::OnPostDrawingChanged( UArianeLayerVector* VectorLayer )
{
    Reset();
}

void
UArianeEditorPathPushTool::OnPreLayerStackSelectionChanged( UArianeLayerStack* LayerStack )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

    if( VectorLayer )
    {
        UnbindVectorLayerDelegates( VectorLayer );
    }
}

void
UArianeEditorPathPushTool::OnPostLayerStackSelectionChanged( UArianeLayerStack* LayerStack )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

    if( VectorLayer )
    {
        BindVectorLayerDelegates( VectorLayer );
    }
}

void
UArianeEditorPathPushTool::BindVectorLayerDelegates( UArianeLayerVector* VectorLayer )
{
    // commented-out: nothing to do on Pre. Left there for consistency
    //VectorLayer->OnPreImageChangedDelegate().AddSP( this, &UArianeEditorPathPushTool::OnPreImageChanged );
    VectorLayer->OnPostDrawingChangedDelegate().AddUObject( this, &UArianeEditorPathPushTool::OnPostDrawingChanged, VectorLayer );
}

void
UArianeEditorPathPushTool::UnbindVectorLayerDelegates( UArianeLayerVector* VectorLayer )
{
    // commented-out: nothing to do on Pre. Left there for consistency
    //VectorLayer->OnPreImageChangedDelegate().RemoveAll( this );
    VectorLayer->OnPostDrawingChangedDelegate().RemoveAll( this );
}

void
UArianeEditorPathPushTool::BindLayerStackDelegates( UArianeLayerStack* LayerStack )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

    LayerStack->OnPreSelectionChangedDelegate().AddUObject( this, &UArianeEditorPathPushTool::OnPreLayerStackSelectionChanged, LayerStack );
    LayerStack->OnPostSelectionChangedDelegate().AddUObject( this, &UArianeEditorPathPushTool::OnPostLayerStackSelectionChanged, LayerStack );

    if( VectorLayer )
    {
        BindVectorLayerDelegates ( VectorLayer );
    }
}

void
UArianeEditorPathPushTool::UnbindLayerStackDelegates( UArianeLayerStack* LayerStack )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

    LayerStack->OnPreSelectionChangedDelegate().RemoveAll( this );
    LayerStack->OnPostSelectionChangedDelegate().RemoveAll( this );

    if( VectorLayer )
    {
        BindVectorLayerDelegates ( VectorLayer );
    }
}

void
UArianeEditorPathPushTool::BindComponentDelegates( UArianePainting3DComponent* Painting3DComponent )
{
    Painting3DComponent->OnPostUpdateDelegate().AddUObject( this, &UArianeEditorPathPushTool::OnPostUpdate, Painting3DComponent );

    BindLayerStackDelegates ( Painting3DComponent->GetLayerStack() );
}

void
UArianeEditorPathPushTool::UnbindComponentDelegates( UArianePainting3DComponent* Painting3DComponent )
{
    Painting3DComponent->OnPostUpdateDelegate().RemoveAll( this );

    UnbindLayerStackDelegates ( Painting3DComponent->GetLayerStack() );
}

void
UArianeEditorPathPushTool::BindEditorDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().AddUObject( this, &UArianeEditorPathPushTool::Reset );

    if( Painting3DComponent )
    {
        BindComponentDelegates ( Painting3DComponent );
    }
}

void
UArianeEditorPathPushTool::UnbindEditorDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().RemoveAll( this );

    if( Painting3DComponent )
    {
        UnbindComponentDelegates( Painting3DComponent );
    }
}

void
UArianeEditorPathPushTool::Activate()
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

    Super::Activate();

    // ViewportClient can be nullptr when closing the editor
    if( ViewportClient )
    {
        ViewportClient->EngineShowFlags.SetSelectionOutline(false);
        ViewportClient->Invalidate();

        GEditor->RedrawAllViewports();
    }

    Reset();

    BindEditorDelegates();
}

void
UArianeEditorPathPushTool::Inactivate()
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

    UnbindEditorDelegates();

    // ViewportClient can be nullptr when closing the editor
    if( ViewportClient )
    {
        ViewportClient->EngineShowFlags.SetSelectionOutline(true);
        ViewportClient->Invalidate();

        GEditor->RedrawAllViewports();
    }

    Super::Inactivate();
}

bool
UArianeEditorPathPushTool::OnKeyDownGlobal( const FKeyEvent& InKeyEvent )
{
    return Super::OnKeyDownGlobal( InKeyEvent );
}

bool
UArianeEditorPathPushTool::OnKeyUpGlobal( const FKeyEvent& InKeyEvent )
{
    return Super::OnKeyUpGlobal( InKeyEvent );
}

UArianeEditorPathPushTool::FPointDisplacement*
UArianeEditorPathPushTool::GetPointDisplacement( TArray<FPointDisplacement>& PointDisplacements, FArianePoint* Point )
{
    return PointDisplacements.FindByPredicate( [Point] ( FPointDisplacement& PointDisplacement )
                                               {
                                                   return ( PointDisplacement.Point == Point );
                                               } );
}

bool
UArianeEditorPathPushTool::OnMouseDown( FEditorViewportClient* ViewportClient
                                      , FSceneView* View
                                      , const FKey& Key
                                      , const FArianePointerState& PointerState
                                      , bool iRepeat )
{
    FVector RayOrigin, RayDirection;
    double ScreenMaxDistance = 0.0f;
    TArray<FArianeVertex*> InfluencedVertices;
    TArray<FArianeHandleSegment*> InfluencedHandles;

    MouseScreenPositionAtDown = MouseScreenPositionAtDrag = FVector2D( PointerState.ViewportX, PointerState.ViewportY );

    bMouseButtonDown = true;

    PickedSegments.Empty();
    InfluencedHandleDisplacements.Empty();
    InfluencedVertexDisplacements.Empty();
    MaxScreenDistanceToMouse = 0.0f;

    View->DeprojectFVector2D( FVector2D( PointerState.ViewportX, PointerState.ViewportY ), RayOrigin, RayDirection );

    if( Key == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(LayerStack->GetCurrentLayer());

            if( VectorLayer )
            {
                FPlane DrawingPlane = GetDrawingPlane( ViewportClient, VectorLayer );

                VectorLayer->GetDrawing()->Modify();

                SegmentQuadTree->PickSegments( ViewportClient, View, MouseScreenPositionAtDown, PickingRadius, PickedSegments );

                InfluencedVertices.Reserve( PickedSegments.Num() * 2 );
                InfluencedHandles.Reserve( PickedSegments.Num() * 2 );

                for( FArianeSegment* PickedSegment : PickedSegments )
                {
                    FArianeObject* Owner = PickedSegment->GetOwner();
                    const FTransform& OwnerTransform = Owner->GetTransform();
                    FArianeVertex* Vertex0 = PickedSegment->GetVertex(0);
                    FArianeVertex* Vertex1 = PickedSegment->GetVertex(1);

                    InfluencedVertices.AddUnique( Vertex0 );
                    InfluencedVertices.AddUnique( Vertex1 );

                    if( PickedSegment->GetClass() == FArianeSegmentCubic::StaticClass() )
                    {
                        FArianeSegmentCubic* CubicSegment = static_cast<FArianeSegmentCubic*>(PickedSegment);
                        FArianeHandleSegment* Handle0 = CubicSegment->GetHandle((uint32)0);
                        FArianeHandleSegment* Handle1 = CubicSegment->GetHandle((uint32)1);

                        InfluencedHandles.AddUnique( Handle0 );
                        InfluencedHandles.AddUnique( Handle1 );
                    }
                }

                InfluencedVertexDisplacements.Reserve( InfluencedVertices.Num() );

                for( FArianeVertex* InfluencedVertex : InfluencedVertices )
                {
                    const FTransform& OwnerTransform = InfluencedVertex->GetOwner()->GetTransform();
                    FVector2D VertexScreenPosition;
                    bool bMapped = WorldToScreen( View
                                                , OwnerTransform.TransformPosition( InfluencedVertex->GetPosition() )
                                                , VertexScreenPosition );
                    double ScreenDistanceToMouse = bMapped ? ( VertexScreenPosition - MouseScreenPositionAtDown ).Length() : -1.0f;

                    InfluencedVertexDisplacements.Emplace( InfluencedVertex
                                                         , bMapped
                                                         , ScreenDistanceToMouse
                                                         , DrawingPlane.GetNormal()
                                                         , OwnerTransform
                                                         , RayOrigin
                                                         , RayDirection
                                                         /*, InfluencedVertex->IsHandleAligned()*/ );

                    InfluencedVertex->DisableHandleAlignment();

                    MaxScreenDistanceToMouse = FMath::Max( MaxScreenDistanceToMouse, ScreenDistanceToMouse );
                }

                InfluencedHandleDisplacements.Reserve( InfluencedHandles.Num() );

                for( FArianeHandleSegment* InfluencedHandle : InfluencedHandles )
                {
                    const FTransform& OwnerTransform = InfluencedHandle->GetOwnerSegment()->GetOwner()->GetTransform();
                    FVector2D HandleScreenPosition;
                    bool bMapped = WorldToScreen( View
                                                , OwnerTransform.TransformPosition( InfluencedHandle->GetPosition() )
                                                , HandleScreenPosition );
                    double ScreenDistanceToMouse = bMapped ? ( HandleScreenPosition - MouseScreenPositionAtDown ).Length() : -1.0f;

                    InfluencedHandleDisplacements.Emplace( InfluencedHandle
                                                         , bMapped
                                                         , ScreenDistanceToMouse
                                                         , DrawingPlane.GetNormal()
                                                         , OwnerTransform
                                                         , RayOrigin
                                                         , RayDirection );

                    MaxScreenDistanceToMouse = FMath::Max( MaxScreenDistanceToMouse, ScreenDistanceToMouse );
                }
            }
        }

        return true;
    }

    return false;
}

void
UArianeEditorPathPushTool::OnMouseHover( FEditorViewportClient* ViewportClient
                                       , FSceneView* View
                                       , const FArianePointerState& State )
{
}

void
UArianeEditorPathPushTool::DisplacePoint( const FPointDisplacement& PointDisplacment
                                        , const FTransform& Transform
                                        , const FVector& RayOrigin
                                        , const FVector& RayDirection )
{
    FVector IntersectAt;

    if( FArianeCore::IntersectPlane( PointDisplacment.WorldPlane, RayOrigin, RayDirection, IntersectAt ) > 0.0f )
    {
        FVector WorldDelta = IntersectAt - PointDisplacment.WorldRayPositionAtDown;

        PointDisplacment.Point->SetPosition( PointDisplacment.LocalPosition + Transform.InverseTransformVector( WorldDelta ) );
    }
}

bool
UArianeEditorPathPushTool::IsMouseButtonDown()
{
    return bMouseButtonDown;
}

bool
UArianeEditorPathPushTool::OnMouseDrag( FEditorViewportClient* ViewportClient
                                      , FSceneView* View
                                      , const FKey& iKey
                                      , const FArianePointerState& PointerState )
{
    MouseScreenPositionAtDrag = FVector2D( PointerState.ViewportX, PointerState.ViewportY );

    if( iKey == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {
            FVector2D MouseScreenVector = ( MouseScreenPositionAtDrag - MouseScreenPositionAtDown );

            for( FVertexDisplacement& InfluencedVertexDisplacement : InfluencedVertexDisplacements )
            {
                FArianeVertex* InfluencedVertex = static_cast<FArianeVertex*>(InfluencedVertexDisplacement.Point);

                if( InfluencedVertexDisplacement.bMapped )
                {
                    double Weight = 1.0f - ( InfluencedVertexDisplacement.ScreenDistance / MaxScreenDistanceToMouse );
                    const FTransform& OwnerTransform = InfluencedVertex->GetOwner()->GetTransform();
                    FVector RayOrigin;
                    FVector RayDirection;

                    View->DeprojectFVector2D( MouseScreenPositionAtDown + ( MouseScreenVector  * Weight )
                                            , RayOrigin
                                            , RayDirection );

                    DisplacePoint( InfluencedVertexDisplacement
                                 , OwnerTransform
                                 , RayOrigin
                                 , RayDirection );
                }
            }

            for( FPointDisplacement& InfluencedHandleDisplacement : InfluencedHandleDisplacements )
            {
                FArianeHandleSegment* InfluencedHandle = static_cast<FArianeHandleSegment*>(InfluencedHandleDisplacement.Point);

                if( InfluencedHandleDisplacement.bMapped )
                {
                    double Weight = 1.0f - ( InfluencedHandleDisplacement.ScreenDistance / MaxScreenDistanceToMouse );
                    const FTransform& OwnerTransform = InfluencedHandle->GetOwnerSegment()->GetOwner()->GetTransform();
                    FVector RayOrigin;
                    FVector RayDirection;

                    View->DeprojectFVector2D( MouseScreenPositionAtDown + ( MouseScreenVector  * Weight )
                                            , RayOrigin
                                            , RayDirection );

                    DisplacePoint( InfluencedHandleDisplacement
                                 , OwnerTransform
                                 , RayOrigin
                                 , RayDirection );
                }
            }

            // re-align tangents if needed
            for( FVertexDisplacement& InfluencedVertexDisplacement : InfluencedVertexDisplacements )
            {
                FArianeVertex* InfluencedVertex = static_cast<FArianeVertex*>(InfluencedVertexDisplacement.Point);

                if( InfluencedVertexDisplacement.bHandleAligned )
                {
                    InfluencedVertex->AlignHandles( FArianeVertex::EHandleAlignmentPolicy::TangentVector );
                }
            }

            // Update. Note: as the seen is invalidated, it will request a redraw
            Painting3DComponent->Update( true );
        }
    }

    return false;
}

void
UArianeEditorPathPushTool::RebuildQuadTree( FEditorViewportClient* ViewportClient
                                          , FSceneView* View )
{
    UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(GetCurrentLayer());

    if( VectorLayer )
    {
        MakeSegmentQuadTree( ViewportClient, View, SelectedTrees, true );
    }
}

bool
UArianeEditorPathPushTool::OnMouseUp( FEditorViewportClient* ViewportClient
                                    , FSceneView* View
                                    , const FKey& iKey
                                    , const FArianePointerState& PointerState )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    bMouseButtonDown = false;

    if ( Painting3DComponent )
    {
        for( FVertexDisplacement& InfluencedVertexDisplacement : InfluencedVertexDisplacements )
        {
            FArianeVertex* InfluencedVertex = static_cast<FArianeVertex*>(InfluencedVertexDisplacement.Point);

            if( InfluencedVertexDisplacement.bHandleAligned )
            {
                InfluencedVertex->EnableHandleAlignment( FArianeVertex::EHandleAlignmentPolicy::TangentVector );
            }
        }

        Painting3DComponent->Update( false );

        InfluencedVertexDisplacements.Empty();
        InfluencedHandleDisplacements.Empty();
    }

    return false;
}

void
UArianeEditorPathPushTool::Render(IToolsContextRenderAPI* RenderAPI)
{
    if  ( Editor->GetShowGrid() )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>( LayerStack->GetCurrentLayer() );

            if( VectorLayer )
            {
                DrawLayerOrientationGrid( RenderAPI, VectorLayer );
            }
        }
    }
}

void
UArianeEditorPathPushTool::DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI )
{
    //UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                            , ViewportClient->GetScene()
                                                                            , ViewportClient->EngineShowFlags ) );
    // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
    FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );
    //FLinearColor FgColor = GetForegroundColor();
    //FLinearColor BgColor = GetBackgroundColor();
    FLinearColor HcColor = GetHighlightColor();
    //FArianeEditorHUD::FDrawingFlags HUDDrawingFlags = Editor->GetHUDDrawingFlags();
    FVector2D MousePosition = FVector2D( ViewportClient->GetCachedMouseX()
                                       , ViewportClient->GetCachedMouseY() );
    FVector2D HUDPosition = ScreenToHUD( ViewportClient, MousePosition );

        // picking circle
    DrawCircleHUD( Canvas
                 , ViewportClient
                 , View
                 , HUDPosition
                 , PickingRadius
                 , HcColor
                 , 1.0f );
}

bool
UArianeEditorPathPushTool::SupportsColorType( EOdysseyPainterEditorColorType ColorType )
{
    return true;
}

void
UArianeEditorPathPushTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UArianeEditorPathPushTool, PickingRadius ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );
}

void
UArianeEditorPathPushTool::ExtendContextMenu( FMenuBuilder& menu )
{
/*
    UEditorActorSubsystem* editorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

    menu.AddMenuEntry(
          LOCTEXT("ariane-path-drawing-tool.context-menu.add-painting3D-component.name", "Add Painting3D component")
        , LOCTEXT("ariane-path-drawing-tool.context-menu.add-painting3D-component.tooltip", "Add Painting3D component")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateRaw( Editor
                                              , &FArianeEditor::AddPainting3DComponent
                                              , editorActorSubsystem->GetSelectedLevelActors() ) ) );
*/
}

#undef LOCTEXT_NAMESPACE
