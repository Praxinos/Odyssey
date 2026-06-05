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
#include "ArianeVertex.h"
#include "ArianeLayerStack.h"
#include "ArianeSegmentCubic.h"

// Unreal headers
#include "Subsystems/EditorActorSubsystem.h"
#include "SceneView.h"
#include "Math/UnrealMathUtility.h"
#include "IStylusState.h"
#include "InteractiveToolManager.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorPathEditTool::~UArianeEditorPathEditTool()
{
}

UArianeEditorPathEditTool::UArianeEditorPathEditTool()
    : Size( 25.0f )
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
UArianeEditorPathEditTool::OnMouseDown( FEditorViewportClient* ViewportClient
                                      , const FKey& iKey
                                      , const FArianePointerState& PointerState
                                      , bool iRepeat )
{
    if( iKey == EKeys::LeftMouseButton )
    {
        UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

        GetToolManager()->BeginUndoTransaction(LOCTEXT("ariane-path-edit-tool.edit-handles","Edit Handles"));

        if( Painting3DComponent )
        {
            //painting3DComponent->PrintPointers();
            UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(LayerStack->GetCurrentLayer());

            if( DrawingLayer )
            {
                //DrawingLayer->Modify();
            }
        }

        return true;
    }

    return false;
}

void
UArianeEditorPathEditTool::OnMouseHover( FEditorViewportClient* ViewportClient
                                       , const FArianePointerState& State )
{

}

void
UArianeEditorPathEditTool::Render( IToolsContextRenderAPI* RenderAPI )
{

}

bool
UArianeEditorPathEditTool::OnMouseDrag( FEditorViewportClient* ViewportClient
                                      , const FKey& iKey
                                      , const FArianePointerState& PointerState )
{
    if( iKey == EKeys::LeftMouseButton )
    {
    }

    return false;
}

bool
UArianeEditorPathEditTool::OnMouseUp( FEditorViewportClient* ViewportClient
                                    , const FKey& iKey
                                    , const FArianePointerState& PointerState )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( ViewportClient->Viewport
                                                                            , ViewportClient->GetScene()
                                                                            , ViewportClient->EngineShowFlags ) );
    // Note: View is not allocated, it will be destroyed by Unreal at the end of the scope
    FSceneView* View = ViewportClient->CalcSceneView( &ViewFamily );

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
    FLinearColor FgColor = FLinearColor( FColor( 0, 169, 157, 255 ) ); // Odyssey's teal
    FLinearColor BgColor = FLinearColor::Black;
    FLinearColor HcColor = FLinearColor::Red;

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
                                                     , HcColor ]( FArianeObject* Object ) -> FArianeObject::TraversalReturnValue
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
                               , FArianeHUD::FDrawingFlags().SetPathSegment()
                                                            .SetPathSegmentHandle()
                                                            .SetPathVertex()
                                                            .SetPathVertexValence0()
                                                            .SetPathVertexValence1()
                                                            .SetPathVertexValence2() );
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
