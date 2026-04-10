// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "PathDrawingTool/ArianeEditorPathDrawingTool.h"
#include "ArianeEditor.h"
#include "ArianePainting3DComponent.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerFolder.h"
#include "ArianePath.h"
#include "ArianeVertex.h"
#include "ArianeLayerStack.h"
#include "ArianeSegmentCubic.h"
// Odyssey
#include "OdysseyStyle.h"
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
    : Size( 5.0f )
    , bPressureSensitivity( false )
    , EditedPath( nullptr )
    //, LineType ( EArianePainting3DGeometryMode::Flat )
{
    Icon = FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathDrawing64");

    bHasContextMenu = true;
}

bool
UArianeEditorPathDrawingTool::OnMouseDown( FEditorViewportClient* iViewportClient
                                         , const FKey& iKey
                                         , const FArianePointerState& PointerState
                                         , bool iRepeat )
{
    if( iKey == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
        ::ULIS::FColor color = Editor->GetPaintColor();
        ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
        FColor ueColor = FColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );

        GetToolManager()->BeginUndoTransaction(FText::FromString("Draw Path"));

        if( Painting3DComponent )
        {
            //painting3DComponent->PrintPointers();
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerDrawing* DrawingLayer = LayerStack->GetFirstSelectedDrawingLayer();

            if( DrawingLayer )
            {
                //Painting3DComponent->Modify();
                //LayerStack->Modify();
                //LayerStack->GetRootFolder()->Modify();
                DrawingLayer->Modify();

                EditedPath = DrawingLayer->AllocPath();

                DrawingLayer->GetRootObject()->AppendChild( EditedPath );

                EditedPath->SetColor( ueColor );
                EditedPath->SetLineType( LineType );

                //GEditor->UndoTransaction();
                //painting3DComponent->PrintPointers();

                PlotVertex( iViewportClient, PointerState, true );
                //PlotVertex( iViewportClient, iViewportX + 100, iViewportY );
                //PlotVertex( iViewportClient, iViewportX + 200, iViewportY );
                /*PlotVertex( iViewportClient, iViewportX + 250, iViewportY + 100 );
                PlotVertex( iViewportClient, iViewportX + 400, iViewportY + 60 );*/
            }
        }

        return true;
    }

    return false;
}

void
UArianeEditorPathDrawingTool::OnMouseHover( FEditorViewportClient* iViewportClient
                                          , const FArianePointerState& State )
{

}

float Intersect ( const FVector4& iPlane
                , const FVector&  iOrigin
                , const FVector&  iDirection
               ,  FVector& oOut )
{
    float vo = ( iPlane.X * iOrigin.X ) +
               ( iPlane.Y * iOrigin.Y ) +
               ( iPlane.Z * iOrigin.Z ) + iPlane.W,
          vd = ( iPlane.X * iDirection.X ) +
               ( iPlane.Y * iDirection.Y ) +
               ( iPlane.Z * iDirection.Z );
    float t;

    if ( vd == 0.0f ) return 0.0f;

    t = - ( vo / vd );

    if ( t > 0.0f ) {
        oOut.X = iOrigin.X + ( iDirection.X * t );
        oOut.Y = iOrigin.Y + ( iDirection.Y * t );
        oOut.Z = iOrigin.Z + ( iDirection.Z * t );

        return t;
    }

    return 0.0f;
}


FVector4
UArianeEditorPathDrawingTool::GetDrawingPlane( UArianeLayerDrawing* DrawingLayer
                                             , const FVector& CameraCoords )
{
    const FTransform& LayerWorldTransform = DrawingLayer->GetComponentTransform();
    FVector LayerWorldPosition = LayerWorldTransform.TransformPosition( FVector( 0, 0, 0 ) );
    FVector4 DrawingPlane = FVector4( 0.0f, 0.0f, 0.0f, 0.0f );

    switch( DrawingLayer->GetDrawingOrientation() )
    {
        case EArianeLayerDrawingOrientation::LayerXY :
            DrawingPlane = LayerWorldTransform.TransformVector( FVector( 0.0f, 0.0f, 1.0f ) );
        break;

        case EArianeLayerDrawingOrientation::LayerYZ :
            DrawingPlane = LayerWorldTransform.TransformVector( FVector( 1.0f, 0.0f, 0.0f ) );
        break;

        case EArianeLayerDrawingOrientation::LayerZX :
            DrawingPlane = LayerWorldTransform.TransformVector( FVector( 0.0f, 1.0f, 0.0f ) );
        break;

        default : // EArianeLayerDrawingOrientation::View
        {
            FVector LayerToCamera = CameraCoords - LayerWorldPosition;

            if( LayerToCamera.IsNearlyZero() == false )
            {
                LayerToCamera.Normalize();

                DrawingPlane = LayerToCamera;
            }
        }
        break;
    }

/* Useless as the plane passes through the Layer's origin
    DrawingPlane.W = - ( ( DrawingPlane.X * LayerWorldPosition.X )
                       + ( DrawingPlane.Y * LayerWorldPosition.Y )
                       + ( DrawingPlane.Z * LayerWorldPosition.Z ) );
*/

    return DrawingPlane;
}

void
UArianeEditorPathDrawingTool::PlotVertex( FEditorViewportClient* iViewportClient
                                        , const FArianePointerState& PointerState
                                        , bool bInteractive )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    FSceneView* View = GetSceneView( iViewportClient );
    double Radius = bPressureSensitivity ? ( Size * 0.5f * PointerState.Pressure )
                                         : ( Size * 0.5f );

    if( Painting3DComponent )
    {
        //painting3DComponent->PrintPointers();
        UArianeLayerDrawing* DrawingLayer = Painting3DComponent->GetLayerStack()->GetFirstSelectedDrawingLayer();

        if( DrawingLayer )
        {
            const FTransform& LayerWorldTransform = DrawingLayer->GetComponentTransform();
            FVector CameraCoords = iViewportClient->GetViewLocation();
            // note: we could do that at MouseDown
            FVector4 DrawingPlane = GetDrawingPlane( DrawingLayer, CameraCoords );
            FVector rayOrigin, rayDirection;
            FVector intersectAt;

            View->DeprojectFVector2D( FVector2D( PointerState.ViewportX
                                               , PointerState.ViewportY )
                                    , rayOrigin
                                    , rayDirection );

            if( Intersect( DrawingPlane, rayOrigin, rayDirection, intersectAt  ) > 0.0f )
            {
                FVector localCoords = LayerWorldTransform.Inverse().TransformFVector4( intersectAt );
                FVector localNormal = LayerWorldTransform.Inverse().TransformVector( FVector( DrawingPlane ) );
                FArianeVertex *Vertex0 = EditedPath->GetVertices().Num() ? EditedPath->GetVertices().Last().GetVertex()
                                                                         : nullptr;

                FArianeVertex *Vertex1 = EditedPath->AllocVertex( localCoords, localNormal, Radius );

                EditedPath->AddVertex( Vertex1 );

                if( Vertex0 )
                {
                    FArianeSegment *Segment = EditedPath->AllocSegment( Vertex0, Vertex1 );

                    EditedPath->AddSegment( Segment );
                }
            }

            Painting3DComponent->Update( bInteractive );
        }
    }
}

bool
UArianeEditorPathDrawingTool::OnMouseDrag( FEditorViewportClient* iViewportClient
                                         , const FKey& iKey
                                         , const FArianePointerState& PointerState )
{
    if( iKey == EKeys::LeftMouseButton )
    {
        PlotVertex( iViewportClient, PointerState, true );
    }

    return false;
}

bool
UArianeEditorPathDrawingTool::OnMouseUp( FEditorViewportClient* iViewportClient
                                       , const FKey& iKey
                                       , const FArianePointerState& PointerState )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( iKey == EKeys::LeftMouseButton )
    {
        if( Painting3DComponent )
        {
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
