// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "PathDrawingTool/ArianeEditorPathDrawingTool.h"
#include "ArianeEditor.h"
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
    , LineType ( EArianePathLineType::Tube )
    , SegmentType ( EArianeEditorPathDrawingToolSegmentType::Polyline )
    , bShowGrid ( true )
    , MaterialInterface ( nullptr )
    , EditedPath(nullptr)
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

                DrawingLayer->Modify();

                EditedPath = DrawingLayer->AllocPath( MaterialInterface, "Path" );

                ParentGroup->AppendChild( EditedPath );

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
            const FTransform& LayerWorldTransform = DrawingLayer->GetComponentTransform();
            // note: we could do that at MouseDown
            FVector4 DrawingPlane = GetDrawingPlane( ViewportClient, DrawingLayer );
            FVector RayOrigin, RayDirection;
            FVector IntersectAt;
            FVector2D ViewportPosition = FVector2D( PointerState.ViewportX
                                                  , PointerState.ViewportY );

            View->DeprojectFVector2D( ViewportPosition
                                    , RayOrigin
                                    , RayDirection );

            if( FArianeCore::IntersectPlane( DrawingPlane, RayOrigin, RayDirection, IntersectAt  ) > 0.0f )
            {
                FVector localCoords = LayerWorldTransform.Inverse().TransformFVector4( IntersectAt );
                FVector localNormal = LayerWorldTransform.Inverse().TransformVector( FVector( DrawingPlane ) );
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
                    Vertex0 = EditedPath->AllocVertex( localCoords, localNormal, Radius );

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
                            FArianeVertex *Vertex1 = EditedPath->AllocVertex( localCoords, localNormal, Radius );

                            EditedPath->AddVertex( Vertex1 );

                            if( Vertex0 )
                            {
                                FArianeSegment *Segment = EditedPath->AllocSegment( Vertex0, Vertex1 );

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
