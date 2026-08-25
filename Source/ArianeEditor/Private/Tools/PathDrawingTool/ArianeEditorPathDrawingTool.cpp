// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "PathDrawingTool/ArianeEditorPathDrawingTool.h"
#include "ArianeEditor.h"
#include "ArianeEditorSettings.h"
#include "ArianeEditorStyle.h"
// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerFolder.h"
#include "ArianePath.h"
#include "ArianeGroup.h"
#include "ArianeCore.h"
#include "ArianeVertex.h"
#include "ArianeLayerStack.h"
#include "ArianeSegmentCubic.h"

// Unreal headers
#include "Subsystems/EditorActorSubsystem.h"
#include "SceneView.h"
#include "Math/UnrealMathUtility.h"
#include "IStylusState.h"
#include "InteractiveToolManager.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPathDrawingTool::~UArianeEditorPathDrawingTool()
{
}

UArianeEditorPathDrawingTool::UArianeEditorPathDrawingTool()
    : Size( 25.0f )
    , bPressureSensitivity( false )
    , LineType ( EArianePathLineType::Flat )
    , SegmentType ( EArianeEditorPathDrawingToolSegmentType::Polyline )
    , bShowGrid ( true )
    , MaterialInterface ( nullptr )
    , EditedPath(nullptr)
    , Cursor( EMouseCursor::Type::Crosshairs )
{
    Icon = FArianeEditorStyle::Get().GetBrush( "ArianeEditor.ToolsTab.PathDrawing64");

    bHasContextMenu = true;
}

void
UArianeEditorPathDrawingTool::Activate()
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
UArianeEditorPathDrawingTool::Inactivate()
{
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();

    // ViewportClient can be nullptr when closing the editor
    if( ViewportClient )
    {
        // Hide outlining
        ViewportClient->EngineShowFlags.SetSelectionOutline(true);
        ViewportClient->Invalidate();

        GEditor->RedrawAllViewports();
    }

    Super::Inactivate();
}

FArianeGroup*
UArianeEditorPathDrawingTool::GetParentGroup( UArianeLayerDrawing* DrawingLayer )
{
    TArray<FArianeObject*>& SelectedObjects = DrawingLayer->GetSelectedObjects();

    if( SelectedObjects.Num() == 1 )
    {
       if( SelectedObjects[0]->GetClass() == FArianeGroup::StaticClass() )
       {
           return static_cast<FArianeGroup*>(SelectedObjects[0]);
       }
    }

    return DrawingLayer->GetRootGroup();
}

bool
UArianeEditorPathDrawingTool::OnMouseDown( FEditorViewportClient* ViewportClient
                                         , FSceneView* View
                                         , const FKey& iKey
                                         , const FArianePointerState& PointerState
                                         , bool iRepeat )
{
    const UArianeEditorSettings* Settings = GetDefault<UArianeEditorSettings>();

    PreviousSegment = CurrentSegment = nullptr;

    if( iKey == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
        ::ULIS::FColor color = Editor->GetPaintColor();
        ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
        FColor ueColor = FColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );

        GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-drawing-tool.draw-path","Draw Path"));

        if( Painting3DComponent )
        {
            //painting3DComponent->PrintPointers();
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(LayerStack->GetCurrentLayer());

            if( DrawingLayer )
            {
                // choose between the root group and the selected group if any
                FArianeGroup* ParentGroup = GetParentGroup( DrawingLayer );
                int PathNumber = DrawingLayer->GetInstancedObjects().Num();

                DrawingLayer->Modify();

                EditedPath = DrawingLayer->AllocPath( MaterialInterface ? MaterialInterface
                                                                        : Settings->GetDefaultPathDrawingMaterial()
                                                     , *(FString( "Path_" ) + FString::FromInt( PathNumber ))
                                                     , EArianeAllocationModel::InstancedStruct );

                ParentGroup->AppendChild( EditedPath );
                EditedPath->UpdateTransform();
                EditedPath->SetColor( ueColor );
                EditedPath->SetLineType( LineType );

                switch ( SegmentType )
                {
                    case EArianeEditorPathDrawingToolSegmentType::CubicBezier :
                        PathTracer.AttachPath( EditedPath );
                    break;

                    default :
                    break;
                }

                PlotVertex( ViewportClient, PointerState, true );
                //PlotVertex( ViewportClient, FArianePointerState( PointerState.ViewportX + 100, PointerState.ViewportY + 100 ), true );
            }
        }

        return true;
    }

    return false;
}

void
UArianeEditorPathDrawingTool::OnMouseHover( FEditorViewportClient* ViewportClient
                                          , FSceneView* View
                                          , const FArianePointerState& State )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
        {
            FPlane DrawingPlane = GetDrawingPlane( ViewportClient, DrawingLayer );
            FVector RayOrigin, RayDirection;
            FVector IntersectAt;
            FVector2D ViewportPosition = FVector2D( ViewportClient->Viewport->GetMouseX()
                                                  , ViewportClient->Viewport->GetMouseY() );

            View->DeprojectFVector2D( ViewportPosition
                                    , RayOrigin
                                    , RayDirection );

            Cursor = ( FArianeCore::IntersectPlane( DrawingPlane
                                                  , RayOrigin
                                                  , RayDirection
                                                  , IntersectAt ) > 0.0f ) ? EMouseCursor::Type::Crosshairs
                                                                           : EMouseCursor::Type::SlashedCircle;
        }
    }
}

bool
UArianeEditorPathDrawingTool::GetCursor( EMouseCursor::Type& OutCursor )
{
    OutCursor = Cursor;

    return true;
}

void
//UArianeEditorTool::OnTick(float DeltaTime)
UArianeEditorPathDrawingTool::Render(IToolsContextRenderAPI* RenderAPI)
{
    if  ( bShowGrid )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        //Super::OnTick( DeltaTime );

        if( Painting3DComponent )
        {
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>( LayerStack->GetCurrentLayer() );

            if( DrawingLayer )
            {
                DrawLayerOrientationGrid( RenderAPI, DrawingLayer );
            }
        }
    }
}

void
UArianeEditorPathDrawingTool::PlotVertex( FEditorViewportClient* ViewportClient
                                        , const FArianePointerState& PointerState
                                        , bool bInteractive )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                            , ViewportClient->GetScene()
                                                                            , ViewportClient->EngineShowFlags ) );
    // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
    FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );
    double Radius = bPressureSensitivity ? ( Size * 0.5f * PointerState.Pressure )
                                         : ( Size * 0.5f );

    if( Painting3DComponent )
    {
        //painting3DComponent->PrintPointers();
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
        {
            const FTransform& PathTransform = EditedPath->GetTransform();
            // note: we could do that at MouseDown
            FPlane DrawingPlane = GetDrawingPlane( ViewportClient, DrawingLayer );
            FVector RayOrigin, RayDirection;
            FVector IntersectAt;
            FVector2D ViewportPosition = FVector2D( PointerState.ViewportX
                                                  , PointerState.ViewportY );

            View->DeprojectFVector2D( ViewportPosition
                                    , RayOrigin
                                    , RayDirection );

            if( FArianeCore::IntersectPlane( DrawingPlane, RayOrigin, RayDirection, IntersectAt  ) > 0.0f )
            {
                FVector localCoords = PathTransform.Inverse().TransformFVector4( IntersectAt );
                FVector localNormal = PathTransform.Inverse().TransformVector( FVector( DrawingPlane ) );
                FArianeVertex *Vertex0 = EditedPath->GetVertices().Num() ? EditedPath->GetVertices().Last().GetVertex()
                                                                         : nullptr;

///////////////////////
/*
FArianeVertex* TestVertex0 = EditedPath->AllocVertex( localCoords, localNormal, Radius );
FArianeVertex* TestVertex1 = EditedPath->AllocVertex( localCoords + FVector(400,0,   0), localNormal, Radius );
FArianeVertex* TestVertex2 = EditedPath->AllocVertex( localCoords + FVector(200,0,-400), localNormal, Radius );

EditedPath->AddVertex( TestVertex0 );
EditedPath->AddVertex( TestVertex1 );
EditedPath->AddVertex( TestVertex2 );

EditedPath->AddSegment( EditedPath->AllocCubicSegment( TestVertex0
                                                     , TestVertex0->GetPosition().X
                                                     , TestVertex0->GetPosition().Y
                                                     , TestVertex0->GetPosition().Z + 200
                                                     , TestVertex1->GetPosition().X
                                                     , TestVertex1->GetPosition().Y
                                                     , TestVertex1->GetPosition().Z + 200
                                                     , TestVertex1 ) );

EditedPath->AddSegment( EditedPath->AllocCubicSegment( TestVertex1
                                                     , TestVertex1->GetPosition().X
                                                     , TestVertex1->GetPosition().Y
                                                     , TestVertex1->GetPosition().Z - 200
                                                     , TestVertex1->GetPosition().X
                                                     , TestVertex2->GetPosition().Y
                                                     , TestVertex2->GetPosition().Z
                                                     , TestVertex2 ) );
*/
///////////////////////
                if( Vertex0 == nullptr )
                {
                    Vertex0 = EditedPath->AllocVertex( localCoords
                                                      , localNormal
                                                      , Radius
                                                      , EArianeAllocationModel::InstancedStruct );

                    EditedPath->AddVertex( Vertex0 );
                }
                                            // if both points are at the same location, the segment will have length 0
                                            // which will result in a broken continuity (angled continuity). Quick-fix to prevent this
                if( localCoords != Vertex0->GetPosition() )
                {
                    switch ( SegmentType )
                    {
                        case EArianeEditorPathDrawingToolSegmentType::Polyline :
                        {
                            FArianeVertex *Vertex1 = EditedPath->AllocVertex( localCoords
                                                                            , localNormal
                                                                            , Radius
                                                                            , EArianeAllocationModel::InstancedStruct );

                            EditedPath->AddVertex( Vertex1 );

                            if( Vertex0 )
                            {
                                FArianeSegment *Segment = EditedPath->AllocSegment( Vertex0
                                                                                  , Vertex1
                                                                                  , EArianeAllocationModel::InstancedStruct );

                                EditedPath->AddSegment( Segment );
                            }
                        }
                        break;

                        case EArianeEditorPathDrawingToolSegmentType::CubicBezier :
                        {
                            if( Vertex0 )
                            {
                                FArianeSegment* NewSegment = PathTracer.Trace( View
                                                                             , PreviousSegment
                                                                             , CurrentSegment ? CurrentSegment->GetVertex(0)
                                                                                              : Vertex0
                                                                             , CurrentSegment
                                                                             , ViewportPosition
                                                                             , localCoords
                                                                             , localNormal
                                                                             , Radius );

                                if( NewSegment != CurrentSegment )
                                {
                                    PreviousSegment = CurrentSegment;
                                    CurrentSegment = NewSegment;
                                }
                            }
                        }
                        break;

                        default :
                        break;
                    }
                }
            }

            Painting3DComponent->Update( bInteractive );
        }
    }
}

bool
UArianeEditorPathDrawingTool::OnMouseDrag( FEditorViewportClient* ViewportClient
                                         , FSceneView* View
                                         , const FKey& iKey
                                         , const FArianePointerState& PointerState )
{
    if( iKey == EKeys::LeftMouseButton )
    {
        PlotVertex( ViewportClient, PointerState, true );
    }

    return false;
}

bool
UArianeEditorPathDrawingTool::OnMouseUp( FEditorViewportClient* ViewportClient
                                       , FSceneView* View
                                       , const FKey& iKey
                                       , const FArianePointerState& PointerState )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( iKey == EKeys::LeftMouseButton )
    {
        if( Painting3DComponent )
        {
            FArianeVertex *Vertex0 = EditedPath->GetVertices().Num() ? EditedPath->GetVertices().Last().GetVertex()
                                                                     : nullptr;
            FVector2D ViewportPosition = FVector2D( PointerState.ViewportX
                                                  , PointerState.ViewportY );
            switch ( SegmentType )
            {
                case EArianeEditorPathDrawingToolSegmentType::CubicBezier :
                {
                    PathTracer.Flush( View
                                    , Vertex0
                                    , nullptr );
                }
                break;

                default :
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

void
UArianeEditorPathDrawingTool::DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    FEditorViewportClient* ViewportClient = GetActiveViewportClient();
    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                            , ViewportClient->GetScene()
                                                                            , ViewportClient->EngineShowFlags ) );
    // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
    FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );
    //FLinearColor FgColor = GetForegroundColor();
    //FLinearColor BgColor = GetBackgroundColor();
    FLinearColor HcColor = GetHighlightColor();
    FVector2D MousePosition = FVector2D( ViewportClient->GetCachedMouseX()
                                       , ViewportClient->GetCachedMouseY() );

     // picking circle
/*
    DrawCircleHUD( Canvas
                 , ViewportClient
                 , View
                 , FVector2D( ViewportClient->GetCachedMouseX()
                            , ViewportClient->GetCachedMouseY() )
                 , Size * 0.5f
                 , HcColor
                 , 1.0f );
*/
}

bool
UArianeEditorPathDrawingTool::SupportsColorType( EOdysseyPainterEditorColorType ColorType )
{
    return true;
}

void
UArianeEditorPathDrawingTool::ExtendContextMenu( FMenuBuilder& menu )
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
