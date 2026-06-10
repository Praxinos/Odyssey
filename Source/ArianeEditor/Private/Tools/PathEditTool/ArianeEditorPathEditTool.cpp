// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "PathEditTool/ArianeEditorPathEditTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"
// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerFolder.h"
#include "ArianePath.h"
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

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPathEditTool::FSegmentAdjustment::FSegmentAdjustment( FArianeSegment* InSegment )
{
    double SegmentLength = InSegment->GetLength();

    Segment = InSegment;
    HandleRatio[0] = HandleRatio[1] = 0.0f;

    if( SegmentLength )
    {
        if( Segment->HasBaseClass( FArianeSegmentCubic::StaticClass() ) )
        {
            FArianeSegmentCubic* CubicSegment = static_cast<FArianeSegmentCubic*>(InSegment);
            FVector HandleVector0 = CubicSegment->GetHandleVector( (uint32)0, false );
            FVector HandleVector1 = CubicSegment->GetHandleVector( (uint32)1, false );

            HandleRatio[0] = HandleVector0.Length() / SegmentLength;
            HandleRatio[1] = HandleVector1.Length() / SegmentLength;
        }
    }
}

void
UArianeEditorPathEditTool::FSegmentAdjustment::Adjust()
{
    if( Segment->HasBaseClass( FArianeSegmentCubic::StaticClass() ) )
    {
        FArianeSegmentCubic* CubicSegment = static_cast<FArianeSegmentCubic*>(Segment);
        FArianeVertex* Vertex0 = CubicSegment->GetVertex(0);
        FArianeVertex* Vertex1 = CubicSegment->GetVertex(1);
        FArianeHandleSegment* Handle0 = CubicSegment->GetHandle((uint32)0);
        FArianeHandleSegment* Handle1 = CubicSegment->GetHandle((uint32)1);
        FVector HandleVector0 = CubicSegment->GetHandleVector( Vertex0, true );
        FVector HandleVector1 = CubicSegment->GetHandleVector( Vertex1, true );
        double CurrentSegmentLength = CubicSegment->GetLength();

        Handle0->SetPosition( Vertex0->GetPosition() + ( HandleVector0 * CurrentSegmentLength * HandleRatio[0] ) );
        Handle1->SetPosition( Vertex1->GetPosition() + ( HandleVector1 * CurrentSegmentLength * HandleRatio[1] ) );
    }
}


UArianeEditorPathEditTool::FPointDisplacement::~FPointDisplacement()
{

}

UArianeEditorPathEditTool::FPointDisplacement::FPointDisplacement( FArianePoint* Point
                                                                 , const FVector4& InWorldPlane
                                                                 , const FTransform& Transform
                                                                 , const FVector& RayOrigin
                                                                 , const FVector& RayDirection )
{
    LocalPosition = Point->GetPosition();
    WorldPosition = Transform.TransformPosition( LocalPosition );
    WorldPlane = InWorldPlane;
    // Plane equation Ax + By + Cz + D = 0
    WorldPlane.W = - ( ( InWorldPlane.X * WorldPosition.X )
                     + ( InWorldPlane.Y * WorldPosition.Y )
                     + ( InWorldPlane.Z * WorldPosition.Z ) );

    FArianeCore::IntersectPlane( WorldPlane, RayOrigin, RayDirection, WorldRayPositionAtDown );
}

UArianeEditorPathEditTool::~UArianeEditorPathEditTool()
{
}

UArianeEditorPathEditTool::UArianeEditorPathEditTool()
    : PickingRadius( 25.0f )
    , EditionMode  ( EArianePathEditToolEditionMode::Vertex )
{
    Icon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.PathEdit64");

    bHasContextMenu = true;
}

void
UArianeEditorPathEditTool::Activate()
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
}

void
UArianeEditorPathEditTool::Inactivate()
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

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
UArianeEditorPathEditTool::OnKeyDownGlobal( const FKeyEvent& InKeyEvent )
{
    if( InKeyEvent.IsRepeat() == false )
    {
        FKey key = InKeyEvent.GetKey();

        FInputChord chord = FInputChord( ( InKeyEvent.IsControlDown() ? EModifierKey::Control : 0 )
                                       | ( InKeyEvent.IsShiftDown()   ? EModifierKey::Shift   : 0 )
                                       | ( InKeyEvent.IsAltDown()     ? EModifierKey::Alt     : 0 )
                                       , key );

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftControl ) || ( key == EKeys::RightControl )
          || ( key == EKeys::LeftCommand ) || ( key == EKeys::RightCommand ) )
        {
            EditionMode   = EArianePathEditToolEditionMode::SegmentHandle;

            return true;
        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftShift ) || ( key == EKeys::RightShift ) )
        {
            EditionMode  = EArianePathEditToolEditionMode::VertexHandle;

            return true;
        }

        // Note, we could FSlateApplication::Get().GetModifierKeys() as well, but for consistency
        // with the events processing in the OnKeyUpGlobalVector(), we do like that.
        if ( ( key == EKeys::LeftAlt ) || ( key == EKeys::RightAlt ) )
        {
            EditionMode  = EArianePathEditToolEditionMode::Alter;

            return true;
        }
    }

    return false;
}

bool
UArianeEditorPathEditTool::OnKeyUpGlobal( const FKeyEvent& InKeyEvent )
{
    FKey key = InKeyEvent.GetKey();

    // note, we cannot use FSlateApplication::Get().GetModifierKeys()
    // because the keys are already released. For consistency we do
    // the same in the KeyDown event even though we could use
    // FSlateApplication::Get().GetModifierKeys()
    //if ( ( key == EKeys::LeftControl ) || ( key == EKeys::RightControl )
    //  || ( key == EKeys::LeftCommand ) || ( key == EKeys::RightCommand )
    //  || ( key == EKeys::LeftShift   ) || ( key == EKeys::RightShift   )
    //  || ( key == EKeys::LeftAlt     ) || ( key == EKeys::RightAlt     ) )
    //{
    //}

    // first reset display mode
    EditionMode = EArianePathEditToolEditionMode::Vertex;

    return false;
}

// static
void
UArianeEditorPathEditTool::BuildSegmentAdjustments( const TArray<FArianeSegment*>& Segments
                                                  , TArray<FSegmentAdjustment>& OutSegmentAdjustments )
{
    OutSegmentAdjustments.Reserve( Segments.Num() );

    for( const FArianeSegmentID& SegmentID : Segments )
    {
        FArianeSegment* Segment = const_cast<FArianeSegmentID&>(SegmentID).GetSegment();

        OutSegmentAdjustments.Emplace( Segment );
    }
}

void
UArianeEditorPathEditTool::OnMouseDownPickPoint( FEditorViewportClient* ViewportClient
                                               , FSceneView* View
                                               , const FKey& iKey
                                               , const FArianePointerState& PointerState
                                               , bool iRepeat )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    FVector RayOrigin;
    FVector RayDirection;

    View->DeprojectFVector2D( FVector2D( PointerState.ViewportX, PointerState.ViewportY ), RayOrigin, RayDirection );

    SelectedPathArray.Empty();
    PickedVertexArray.Empty();
    PickedVertexDisplacementArray.Empty();
    PickedVertexRadiusArray.Empty();
    PickedHandleArray.Empty();
    PickedHandleDisplacementArray.Empty();
    SegmentAdjustmentArray.Empty();

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        DrawingLayer->GetRootObject()->Traverse
        ( [ this
          , ViewportClient
          , View
          , &PointerState ]( FArianeObject* Object ) -> FArianeObject::TraversalReturnValue
          {
              if( /*iScene->GetCell()->ObjectHasFocus( object, traversalFlags )*/1 )
              {
                  if( Object->HasBaseClass( FArianePath::StaticClass() ) )
                  {
                      FArianePath* Path = static_cast<FArianePath*>(Object);

                      PickPathPoints( ViewportClient
                                    , View
                                    , Path
                                    , PointerState.ViewportX
                                    , PointerState.ViewportY
                                    , PickingRadius
                                    , PickedVertexArray
                                    , PickedHandleArray
                                    , EditonModeToPickingFlags() );
                  }
              }

              return FArianeObject::TraversalReturnValue::Continue;
          } );

        // Link or Unlink segment handles
        if( ( EditionMode == EArianePathEditToolEditionMode::SegmentHandle ) &&  ( PickedVertexArray.Num() == 1 ) )
        {
            FArianeVertex* Vertex = PickedVertexArray[0];

            //-------------- undo ---------------//
            GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.transaction.align-point-selection","Align Point Selection"));

            Vertex->SetHandleAligned( Vertex->IsHandleAligned() ? false : true );

            GetToolManager()->EndUndoTransaction();
        }
        // Else, save point coordinates before changing them
        else
        {
            switch( EditionMode )
            {
                case EArianePathEditToolEditionMode::VertexHandle :
                    GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.edit-vertex-handles","Edit Vertex Handles"));
                break;

                case EArianePathEditToolEditionMode::Vertex :
                {
                    GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.edit-vertices","Edit Vertices"));

                    TArray<FArianeSegment*> AlteredSegmentArray;

                    // static call
                    FArianeVertex::ArrayToSegmentArray( PickedVertexArray, AlteredSegmentArray, false );
                    // static call
                    BuildSegmentAdjustments( AlteredSegmentArray, SegmentAdjustmentArray );

                    // Control points must move with the point. Store them in the iPickedHandleArray
                    for( FArianeVertex* Vertex : PickedVertexArray )
                    {
                        for( const FArianeSegmentID& SegmentID : Vertex->GetSegments() )
                        {
                            FArianeSegment* Segment = const_cast<FArianeSegmentID&>(SegmentID).GetSegment();

                            if( Segment->GetClass() == FArianeSegmentCubic::StaticClass() )
                            {
                                FArianeSegmentCubic* CubicSegment = static_cast<FArianeSegmentCubic*>(Segment);
                                FArianeHandleSegment* Handle = CubicSegment->GetHandle( Vertex );

                                PickedHandleArray.Add( Handle );
                            }
                        }
                    }
                }
                break;

                case EArianePathEditToolEditionMode::SegmentHandle :
                    GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.edit-handles","Edit Handles"));
                break;

                default:
                break;
            }
        }

        // remember vertex position at mouse down
        PickedVertexDisplacementArray.Reserve( PickedVertexArray.Num() );
        PickedVertexRadiusArray.Reserve( PickedVertexArray.Num() );
        for( FArianeVertex* Vertex : PickedVertexArray )
        {
            const FTransform& OwnerTransform = Vertex->GetOwner()->GetTransform();

            PickedVertexDisplacementArray.Emplace( Vertex, View->GetViewDirection(), OwnerTransform, RayOrigin, RayDirection );
            PickedVertexRadiusArray.Emplace( Vertex->GetRadius() );
        }

        // remember handle position at mouse down
        PickedHandleDisplacementArray.Reserve( PickedHandleArray.Num() );
        for( FArianeHandleSegment* Handle : PickedHandleArray )
        {
            const FTransform& OwnerTransform = Handle->GetOwnerSegment()->GetOwner()->GetTransform();

            PickedHandleDisplacementArray.Emplace( Handle, View->GetViewDirection(), OwnerTransform, RayOrigin, RayDirection );
        }

        DrawingLayer->Update( true );
    }
}

bool
UArianeEditorPathEditTool::OnMouseDown( FEditorViewportClient* ViewportClient
                                      , FSceneView* View
                                      , const FKey& Key
                                      , const FArianePointerState& PointerState
                                      , bool iRepeat )
{
    if( Key == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {
            //painting3DComponent->PrintPointers();
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(LayerStack->GetCurrentLayer());

            if( DrawingLayer )
            {
                //mPointInTextureAtDown = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

                //mPathEditHUD->SetCutLineP0( iPointInTexture.x, iPointInTexture.y );
                //mPathEditHUD->SetCutLineP1( iPointInTexture.x, iPointInTexture.y );

                DrawingLayer->Modify();

                switch( EditionMode )
                {
                    case EArianePathEditToolEditionMode::Alter :
                    // dealt with in OnMouseUpVector
                    break;

                    default:
                        OnMouseDownPickPoint( ViewportClient, View, Key, PointerState, iRepeat );
                    break;
                }
            }
        }

        return true;
    }

    return false;
}

void
UArianeEditorPathEditTool::OnMouseHover( FEditorViewportClient* ViewportClient
                                       , FSceneView* View
                                       , const FArianePointerState& State )
{

}

void
UArianeEditorPathEditTool::Render( IToolsContextRenderAPI* RenderAPI )
{

}

/*
void
UArianeEditorPathEditTool::DragVertexHandle( FArianeVertex *Vertex
                                           , double OriginalRadius
                                           , const ::ULIS::FVec2D& iPointInTexture
                                           , bool iWidenAllAlong )
{
    FOdysseyVectorPath* path = iVertex->GetOwnerAsPath();
    ::ULIS::FVec2D localMouse = FOdysseyVector::MapPoint( path->GetInverseWorldMatrix()
                                                        , iPointInTexture );
    double ratio = ( ::ULIS::FVec2D( iVertex->GetX() - localMouse.x
                                   , iVertex->GetY() - localMouse.y ).Distance() ) / iVertex->GetRadius();

    if( iWidenAllAlong )
    {
        for( int i = 0; i < mSelectedPathArray.size(); i++ )
        {
            mSelectedPathArray[i]->AlterRadius( ratio  );
        }
    }
    else
    {
        iVertex->SetRadius( iVertex->GetRadius() * ratio );
    }
}
*/

void
UArianeEditorPathEditTool::DisplacePoint( FArianePoint* Point
                                        , const FPointDisplacement& PointDisplacment
                                        , const FTransform& Transform
                                        , const FVector& RayOrigin
                                        , const FVector& RayDirection )
{
    FVector IntersectAt;

    if( FArianeCore::IntersectPlane( PointDisplacment.WorldPlane, RayOrigin, RayDirection, IntersectAt ) > 0.0f )
    {
        FVector WorldDelta = IntersectAt - PointDisplacment.WorldRayPositionAtDown;

        Point->SetPosition( PointDisplacment.LocalPosition + Transform.InverseTransformVector( WorldDelta ) );
    }
}

bool
UArianeEditorPathEditTool::OnMouseDrag( FEditorViewportClient* ViewportClient
                                      , FSceneView* View
                                      , const FKey& iKey
                                      , const FArianePointerState& PointerState )
{
    FVector RayOrigin;
    FVector RayDirection;

    View->DeprojectFVector2D( FVector2D( PointerState.ViewportX, PointerState.ViewportY )
                            , RayOrigin
                            , RayDirection );

    if( iKey == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {
            //TArray<FArianePoint*> SnappedPointArray;

            //mPathEditHUD->SetCutLineP1( iPointInTexture.x, iPointInTexture.y );
            //mPathEditHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

            /*
            // snapping
            mPathEditHUD->PickPoints( iPointInTexture.x
                                    , iPointInTexture.y
                                    , PickingRadius
                                    , snappedPointArray );
            if( snappedPointArray.size() )
            {
                if( snappedPointArray[0]->GetClass() == FOdysseyVectorVertex::StaticClass() )
                {
                    FOdysseyVectorVertex* snappedVertex = static_cast<FOdysseyVectorVertex*>(snappedPointArray[0]);
                    ::ULIS::FVec2D snappedVertexWorldCoords = snappedVertex->GetWorldCoords();

                    pointInTextureX = snappedVertexWorldCoords.x;
                    pointInTextureY = snappedVertexWorldCoords.y;
                }
            }
            */

            if( EditionMode == EArianePathEditToolEditionMode::VertexHandle  )
            {
                for( int i = 0; i < PickedVertexArray.Num(); i++ )
                {
                    FArianeVertex *Vertex = PickedVertexArray[i];

                    ////DragVertexHandle( Vertex
                    ////                , PickedVertexRadiusArray[i]
                    ////                , ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y )
                    ////                , WidenAllAlong && ( PickedVertexArray.Num() == 1 ) );
                }
            }

            if( EditionMode == EArianePathEditToolEditionMode::Vertex )
            {
                for( int i = 0; i < PickedVertexArray.Num(); i++ )
                {
                    FArianeVertex *Vertex = PickedVertexArray[i];
                    FPointDisplacement& PointDisplacement = PickedVertexDisplacementArray[i];

                    DisplacePoint( Vertex
                                 , PointDisplacement
                                 , Vertex->GetOwner()->GetTransform()
                                 , RayOrigin
                                 , RayDirection );
                }
            }

            if( ( EditionMode == EArianePathEditToolEditionMode::SegmentHandle )
            ||  ( EditionMode == EArianePathEditToolEditionMode::Vertex        ) )
            {
                for( int i = 0; i < PickedHandleArray.Num(); i++ )
                {
                    FArianeHandleSegment *Handle = PickedHandleArray[i];
                    FPointDisplacement& PointDisplacement = PickedHandleDisplacementArray[i];

                    DisplacePoint( Handle
                                 , PointDisplacement
                                 , Handle->GetOwnerSegment()->GetOwner()->GetTransform()
                                 , RayOrigin
                                 , RayDirection );
                }
            }

            // adjust handle length to keep the same ratio as before the editing
            if( EditionMode == EArianePathEditToolEditionMode::Vertex )
            {
                for( FSegmentAdjustment& SegmentAdjustment : SegmentAdjustmentArray )
                {
                    SegmentAdjustment.Adjust();
                }
            }

            // Update. Note: as the seen is invalidated, it will request a redraw
            Painting3DComponent->Update( true );
        }
    }

    return false;
}

bool
UArianeEditorPathEditTool::OnMouseUp( FEditorViewportClient* ViewportClient
                                    , FSceneView* View
                                    , const FKey& iKey
                                    , const FArianePointerState& PointerState )
{

    if( iKey == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        if( Painting3DComponent )
        {
            switch( EditionMode )
            {
                case EArianePathEditToolEditionMode::Alter :
                {
                    ////TArray<FArianePoint*>& HoveredPointArray = PathEditHUD->GetHoveredPointArray();

                    // This populates mPathEditHUD::mHoveredPointArray
                    //mPathEditHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

                    ////if( HoveredPointArray.Num() )
                    ////{
                        ////OnMouseUpDeletePoint( iScene, hoveredPointArray );
                    ////}
                    ////else
                    ////{
                        ////TArray<FArianeSegment*> PickedSegmentArray;

                        // check if we picked any segment
                        ////PickSegments( iScene
                        ////            , iPointInTexture.x
                        ////            , iPointInTexture.y
                        ////            , PickingRadius
                        ////            , true
                        ////            , false
                        ////            , pickedSegmentArray );

                        ////if( pickedSegmentArray.size() )
                        ////{
                            ////OnMouseUpAddPoint( iScene
                            ////                 , iPointInTexture
                            ////                 , pickedSegmentArray );
                        ////}
                        ////else
                        ////{
                            ////OnMouseUpCutPaths( iScene, iPointInTexture );
                        ////}
                    ////}
                }
                break;

                default:
                {
                    // If nothing was selected, we pick an object
                    if( ( PickedVertexArray.Num() == 0 ) && ( PickedHandleArray.Num() == 0 ) )
                    {
                        ////PickObjects( iScene, iPointInTexture.x, iPointInTexture.y );
                    }
                }
                break;
            }

            Painting3DComponent->Update( false );
        }

        GetToolManager()->EndUndoTransaction();

        return true;
    }

    if( iKey == EKeys::RightMouseButton )
    {

        return false;
    }

    return false;
}

UArianeEditorTool::FPickingFlags
UArianeEditorPathEditTool::EditonModeToPickingFlags()
{
    FPickingFlags PickingFlags;

    switch( EditionMode )
    {
        case EArianePathEditToolEditionMode::Alter :
        case EArianePathEditToolEditionMode::Vertex :
        {
            PickingFlags.SetPathVertex();
        }
        break;

        case EArianePathEditToolEditionMode::SegmentHandle :
        {
            PickingFlags.SetPathSegmentHandle()
                        .SetPathVertex(); // for vertex handle alignment
        }
        break;

        case EArianePathEditToolEditionMode::VertexHandle :
        {
            PickingFlags.SetPathVertexHandle();
        }
        break;

        default :
        {
        }
        break;
    }

    return PickingFlags;
}

FArianeEditorHUD::FDrawingFlags
UArianeEditorPathEditTool::EditonModeToHUDDrawingFlags()
{
    FArianeEditorHUD::FDrawingFlags HUDDrawingFlags = Editor->GetHUDDrawingFlags();

    switch( EditionMode )
    {
        case EArianePathEditToolEditionMode::Alter :
        case EArianePathEditToolEditionMode::Vertex :
        {
            HUDDrawingFlags.SetPathVertex()
                           .SetPathSegment();
        }
        break;

        case EArianePathEditToolEditionMode::SegmentHandle :
        {
            HUDDrawingFlags.SetPathVertex()
                           .SetPathSegment()
                           .SetPathSegmentHandle();
        }
        break;

        case EArianePathEditToolEditionMode::VertexHandle :
        {
            HUDDrawingFlags.SetPathVertex()
                           .SetPathVertexHandle()
                           .SetPathSegment();
        }
        break;

        default :
        {
        }
        break;
    }

    return HUDDrawingFlags;
}

void
UArianeEditorPathEditTool::DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                            , ViewportClient->GetScene()
                                                                            , ViewportClient->EngineShowFlags ) );
    // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
    FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );
    FLinearColor FgColor = GetForegroundColor();
    FLinearColor BgColor = GetBackgroundColor();
    FLinearColor HcColor = GetHighlightColor();
    FArianeEditorHUD::FDrawingFlags HUDDrawingFlags = EditonModeToHUDDrawingFlags();

     // picking circle
    DrawCircleHUD( Canvas
                 , ViewportClient
                 , View
                 , FVector2D( ViewportClient->GetCachedMouseX()
                            , ViewportClient->GetCachedMouseY() )
                 , PickingRadius
                 , HcColor
                 , 1.0f );

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
        {
            DrawingLayer->GetRootObject()->Traverse( [ this
                                                     , Canvas
                                                     , ViewportClient
                                                     , View
                                                     , FgColor
                                                     , BgColor
                                                     , HcColor
                                                     , HUDDrawingFlags ]( FArianeObject* Object ) -> FArianeObject::TraversalReturnValue
            {
                if( Object->GetClass() == FArianePath::StaticClass() )
                {
                    FArianePath* Path = static_cast<FArianePath*>( Object );

                    DrawPathHUD( Canvas
                               , ViewportClient
                               , View
                               , Path
                               , FgColor
                               , BgColor
                               , HcColor
                               , HUDDrawingFlags );
                }

                return FArianeObject::TraversalReturnValue::Continue;
            } );
        }
    }
}

bool
UArianeEditorPathEditTool::SupportsColorType( EOdysseyPainterEditorColorType ColorType )
{
    return true;
}

EArianePathEditToolEditionMode
UArianeEditorPathEditTool::GetEditionMode()
{
    return EditionMode;
}

void
UArianeEditorPathEditTool::SetEditionMode( EArianePathEditToolEditionMode iMode )
{
    EditionMode = iMode;
}

const FSlateBrush*
UArianeEditorPathEditTool::GetBackgroundBrush( EArianePathEditToolEditionMode iMode ) const
{
    static FSlateColorBrush selected = FSlateColorBrush( FStyleColors::Select );

    return ( iMode == EditionMode ) ? &selected : nullptr;
}

TSharedRef<SWidget>
UArianeEditorPathEditTool::CreateModifierSegmentControl()
{
    return SNew(SSegmentedControl<EArianePathEditToolEditionMode>)
           .Value_Lambda( [this]{ return EditionMode; } )
           .SupportsEmptySelection( false )
           .SupportsMultiSelection( false )
           .IsEnabled( false ) // currently not clickable - Info only
           .UniformPadding( FMargin( 2, 0, 2, 0 ) )
           .OnValueChanged( SSegmentedControl<EArianePathEditToolEditionMode>::FOnValueChanged::CreateUObject( this, &UArianeEditorPathEditTool::SetEditionMode ) )
           // DEFAULT
           + SSegmentedControl<EArianePathEditToolEditionMode>::Slot( EArianePathEditToolEditionMode::Vertex )
           .ToolTip( LOCTEXT("ariane-path-edit-tool.edition-mode.default.name", "Default") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UArianeEditorPathEditTool::GetBackgroundBrush, EArianePathEditToolEditionMode::Vertex  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsShortcuts.PathEditMoveVertex20") )
               ]
           ]
           // CTRL
           + SSegmentedControl<EArianePathEditToolEditionMode>::Slot( EArianePathEditToolEditionMode::SegmentHandle )
#if PLATFORM_WINDOWS
           .ToolTip( LOCTEXT("ariane-path-edit-tool.edition-mode.ctrl.name", "Deform Segment (CTRL)") )
#endif
#if PLATFORM_MAC
           .ToolTip( LOCTEXT("ariane-path-edit-tool.edition-mode.cmd.name", "Deform Segment (CMD)") )
#endif
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UArianeEditorPathEditTool::GetBackgroundBrush, EArianePathEditToolEditionMode::SegmentHandle  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsShortcuts.PathEditDeformSegment20") )
               ]
           ]
           // SHIFT
           + SSegmentedControl<EArianePathEditToolEditionMode>::Slot( EArianePathEditToolEditionMode::VertexHandle )
           .ToolTip( LOCTEXT("ariane-path-edit-tool.edition-mode.shift.name", "Widen Vertex (SHIFT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UArianeEditorPathEditTool::GetBackgroundBrush, EArianePathEditToolEditionMode::VertexHandle  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsShortcuts.PathEditWidenVertex20") )
               ]
           ]
           // ALT
           + SSegmentedControl<EArianePathEditToolEditionMode>::Slot( EArianePathEditToolEditionMode::Alter )
           .ToolTip( LOCTEXT("ariane-path-edit-tool.edition-mode.alt.name", "Add/Remove Vertex (ALT)") )
           [
               SNew(SBorder)
               .BorderImage_UObject( this, &UArianeEditorPathEditTool::GetBackgroundBrush, EArianePathEditToolEditionMode::Alter  )
               [
                   SNew(SImage)
                   .Image( FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsShortcuts.PathEditAddRemoveVertex20") )
               ]
           ];
}

void
UArianeEditorPathEditTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            CreateModifierSegmentControl(),
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UArianeEditorPathEditTool, PickingRadius ), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UArianeEditorPathEditTool, WidenAllAlong ), FSinglePropertyParams())
                .InnerPadding(10.f)
            ],
            FText()
        )
    );
}

void
UArianeEditorPathEditTool::ExtendContextMenu( FMenuBuilder& menu )
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
