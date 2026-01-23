// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "PathDrawingTool/ArianeEditorPathDrawingTool.h"
#include "ArianeEditor.h"
#include "ArianePainting3DComponent.h"
// Unreal headers
#include "Subsystems/EditorActorSubsystem.h"
#include "SceneView.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPathDrawingTool::~UArianeEditorPathDrawingTool()
{
}

UArianeEditorPathDrawingTool::UArianeEditorPathDrawingTool()
{
    bHasContextMenu = true;
}

bool
UArianeEditorPathDrawingTool::OnMouseDown( FEditorViewportClient* iviewportClient
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
                painting3DComponent->mVertices.Empty();
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

bool
UArianeEditorPathDrawingTool::OnMouseDrag( FEditorViewportClient* iViewportClient
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
            FVector4 screenCoords = View->WorldToScreen ( FVector( 0.0f, 0.0f, 0.0f ) );
            FVector4 worldCoords = View->ScreenToWorld ( FVector( iViewportX, iViewportY, screenCoords.Z ) );

            const FTransform& rootTransform = actor->GetRootComponent()->GetComponentTransform();

            painting3DComponent->mVertices.Push( rootTransform.Inverse().TransformPosition( worldCoords ) );
        }
    }

    return true;
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
