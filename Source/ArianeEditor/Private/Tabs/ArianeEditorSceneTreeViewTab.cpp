// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor Headers
#include "ArianeEditorSceneTreeViewTab.h"
#include "SArianeEditorSceneTreeView.h"
#include "SArianeEditorCurrentObjectDetailsView.h"
//#include "ArianeEditorObjectView.h"
//#include "ArianeEditorPathView.h"
//#include "ArianeEditorGroupView.h"
#include "ArianeEditor.h"
// Ariane Headers
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
#include "ArianeImage.h"
#include "ArianePainting3DComponent.h"
// Unreal Headers
#include "Widgets/Layout/SWidgetSwitcher.h"


#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FArianeEditorSceneTreeViewTab::StaticId()
{
    static FName Id = TEXT("ArianeEditor_SceneTreeView");
    return Id;
}

/////////////////////////////////////////////////////
// FArianeEditorSceneTreeViewTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FArianeEditorSceneTreeViewTab::~FArianeEditorSceneTreeViewTab()
{

}

FArianeEditorSceneTreeViewTab::FArianeEditorSceneTreeViewTab( FArianeEditor* InEditor )
    : FArianeEditorTab( LOCTEXT( "ariane-scene-tree-view-tab.name", "Scene Tree View" )
                      , FSlateIcon( "ArianeEditorStyle", "ArianeEditor.SceneTreeView.MenuIcon" ) )
    , Editor( InEditor )
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FArianeEditorTab interface

const FName&
FArianeEditorSceneTreeViewTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FArianeEditorSceneTreeViewTab::CreateWidget()
{
    return SNew(SWidgetSwitcher)
        .WidgetIndex(this, &FArianeEditorSceneTreeViewTab::WidgetIndex)
        +SWidgetSwitcher::Slot()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("ariane-scene-tree-view-tab.no-scene-text", "This tab is only available when editing a vector scene."))
            .AutoWrapText(true)
        ]
        +SWidgetSwitcher::Slot()
        [
            SNew(SSplitter)
            .Orientation( EOrientation::Orient_Vertical )
            +SSplitter::Slot()
            [
                SNew( SArianeEditorSceneTreeView, Editor )
            ]
            +SSplitter::Slot()
            [
                SNew( SArianeEditorCurrentObjectDetailsView, Editor )
            ]
        ];

}

FArianeGroup*
FArianeEditorSceneTreeViewTab::GetRootGroup() const
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
        {
            return DrawingLayer->GetImage()->GetRootGroup();
        }
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

int
FArianeEditorSceneTreeViewTab::WidgetIndex() const
{
    if ( GetRootGroup() )
        return 1;

    return 0;
}

#undef LOCTEXT_NAMESPACE
