// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "PathDrawingTool/ArianeEditorPathDrawingTool.h"
#include "ArianeEditor.h"
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeVertex.h"
#include "ArianeSegmentCubic.h"
// Unreal headers
#include "Subsystems/EditorActorSubsystem.h"
#include "SceneView.h"
#include "Math/UnrealMathUtility.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPathDrawingTool::~UArianeEditorPathDrawingTool()
{
}

UArianeEditorPathDrawingTool::UArianeEditorPathDrawingTool()
    : EditedPath( nullptr )
{
    bHasContextMenu = true;
}

bool
UArianeEditorPathDrawingTool::OnMouseDown( FEditorViewportClient* iViewportClient
                                         , double iViewportX
                                         , double iViewportY
                                         , const FKey& iKey
                                         , bool iRepeat )
{
    if( iKey == EKeys::LeftMouseButton )
    {
        UEditorActorSubsystem* editorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

        for( AActor* actor : editorActorSubsystem->GetSelectedLevelActors() )
        {
            UArianePainting3DComponent* painting3DComponent = Cast<UArianePainting3DComponent>(actor->GetComponentByClass( UArianePainting3DComponent::StaticClass() ));

            if( painting3DComponent )
            {
                EditedPath = new FArianePath();

                painting3DComponent->AddPath( EditedPath );

                PlotVertex( iViewportClient, iViewportX, iViewportY );
            }
        }

        return true;
    }

    return false;
}

void
UArianeEditorPathDrawingTool::OnMouseHover( FEditorViewportClient* iViewportClient
                                          , double iViewportX
                                          , double iViewportY )
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

// insipired from gluProject
FVector Project( FEditorViewportClient* iViewportClient
               , FSceneView* iSceneView
               , const FVector& iWorldPosition )
{
    const FMatrix& viewProjectionMatrix = iSceneView->ViewMatrices.GetProjectionMatrix();
    FIntPoint viewportSize = iViewportClient->Viewport->GetSizeXY();
    FVector winPosition;

    FVector4 screenPosition = viewProjectionMatrix.TransformFVector4( FVector4( iWorldPosition, 1.0f ) );

    winPosition.X = 0.0f + ( ( viewportSize.X * screenPosition.X ) + 1.0f ) * 0.5f;
    winPosition.Y = 0.0f + ( ( viewportSize.Y * screenPosition.Y ) + 1.0f ) * 0.5f;
    winPosition.Z = ( screenPosition.Z + 1.0f ) * 0.5f;

    return winPosition;
}

void
UArianeEditorPathDrawingTool::PlotVertex( FEditorViewportClient* iViewportClient
                                        , double iViewportX
                                        , double iViewportY )
{
    UEditorActorSubsystem* editorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
    FSceneView* View = GetSceneView( iViewportClient );

    for( AActor* actor : editorActorSubsystem->GetSelectedLevelActors() )
    {
        UArianePainting3DComponent* painting3DComponent = Cast<UArianePainting3DComponent>(actor->GetComponentByClass( UArianePainting3DComponent::StaticClass() ));

        if( painting3DComponent )
        {
            const FTransform& actorWorldTransform = actor->GetRootComponent()->GetComponentTransform();
            FVector actorWorldPosition = actorWorldTransform.TransformPosition( FVector( 0, 0, 0 ) );
            FVector rayOrigin, rayDirection;
            FVector4 actorWorldPlane = actorWorldTransform.TransformVector( FVector( 0, 1.0f, 0.0f ) );
            FVector intersectAt;

            FVector cameraCoords = iViewportClient->GetViewLocation();

            FVector planeVector = cameraCoords - actorWorldPosition;

            planeVector.Normalize();

            actorWorldPlane = planeVector;

            actorWorldPlane.W = - ( ( actorWorldPlane.X * actorWorldPosition.X )
                                    + ( actorWorldPlane.Y * actorWorldPosition.Y )
                                    + ( actorWorldPlane.Z * actorWorldPosition.Z ) );

            View->DeprojectFVector2D( FVector2D( iViewportX, iViewportY ), rayOrigin, rayDirection );

            if( Intersect( actorWorldPlane, rayOrigin, rayDirection, intersectAt  ) > 0.0f )
            {
                FVector localCoords = actorWorldTransform.Inverse().TransformFVector4( intersectAt );
                FArianeVertex *vertex0 = EditedPath->GetVertices().Num() ? EditedPath->GetVertices().Last()
                                                                         : nullptr;
                FArianeVertex *vertex1 = new FArianeVertex( localCoords );

                EditedPath->AddVertex( vertex1 );

                if( vertex0 )
                {
                    FArianeSegment *segment = new FArianeSegment( vertex0, vertex1 );

                    EditedPath->AddSegment( segment );
                }
            }

            break;
        }
    }
}

bool
UArianeEditorPathDrawingTool::OnMouseDrag( FEditorViewportClient* iViewportClient
                                         , double iViewportX
                                         , double iViewportY )
{
    if( iViewportClient->Viewport->KeyState( EKeys::LeftMouseButton ) )
    {
        PlotVertex( iViewportClient, iViewportX, iViewportY );
    }

    return false;
}

bool
UArianeEditorPathDrawingTool::OnMouseUp( FEditorViewportClient* iViewportClient
                                       , double iViewportX
                                       , double iViewportY
                                       , const FKey& iKey )
{
    if( iKey == EKeys::LeftMouseButton )
    {



        return true;
    }

    if( iKey == EKeys::RightMouseButton )
    {

        return false;
    }

    return false;
}

void
UArianeEditorPathDrawingTool::ExtendContextMenu( FMenuBuilder& menu )
{
    UEditorActorSubsystem* editorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

    menu.AddMenuEntry(
          LOCTEXT("ariane-path-drawing-tool.context-menu.add-painting3D-component.name", "Add Painting3D component")
        , LOCTEXT("ariane-path-drawing-tool.context-menu.add-painting3D-component.tooltip", "Add Painting3D component")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateRaw( mEditor
                                              , &FArianeEditor::AddPainting3DComponent
                                              , editorActorSubsystem->GetSelectedLevelActors() ) ) );
}

#undef LOCTEXT_NAMESPACE
