// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneDetailsView.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVector.h"
#include "OdysseyVectorCell.h"
#include "HUD/OdysseyVectorHUD.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "OdysseyPainterEditorVectorObjectView.h"
#include "OdysseyPainterEditorVectorPathView.h"
#include "OdysseyPainterEditorVectorGroupView.h"
#include "OdysseyPainterEditorVectorGroupPaintView.h"
#include "OdysseyPainterEditorVectorTagInbetweenerView.h"
#include "OdysseyMediaVector.h"
#include "OdysseyTextureLayerImageVector.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimationCurrentFrameMutator.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorVectorSceneTreeViewTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_VectorSceneTreeView"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorVectorSceneTreeViewTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorVectorSceneTreeViewTab::~FOdysseyPainterEditorVectorSceneTreeViewTab()
{

}

FOdysseyPainterEditorVectorSceneTreeViewTab::FOdysseyPainterEditorVectorSceneTreeViewTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab( LOCTEXT( "vector-scene-tree-view-tab.name", "Vector Scene Tree View" ),
                         FSlateIcon( "OdysseyStyle", "PainterEditor.VectorSceneTreeView.MenuIcon" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorVectorSceneTreeViewTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorVectorSceneTreeViewTab::CreateWidget()
{
    return SNew(SWidgetSwitcher)
        .WidgetIndex(this, &FOdysseyPainterEditorVectorSceneTreeViewTab::WidgetIndex)
        +SWidgetSwitcher::Slot()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("vector-scene-tree-view-tab.no-scene-text", "This tab is only available when editing a vector scene."))
            .AutoWrapText(true)
        ]
        +SWidgetSwitcher::Slot()
        [
            SNew(SSplitter)
            .Orientation( EOrientation::Orient_Vertical )
            +SSplitter::Slot()
            [
                SNew( SOdysseyPainterEditorVectorSceneTreeView )
                .Editor(mEditor)
                .Scene(this, &FOdysseyPainterEditorVectorSceneTreeViewTab::GetScene)
            ]
            +SSplitter::Slot()
            [
                SNew( SOdysseyPainterEditorVectorSceneDetailsView, mEditor )
                .Scene(this, &FOdysseyPainterEditorVectorSceneTreeViewTab::GetScene)
            ]
        ];

}

void
FOdysseyPainterEditorVectorSceneTreeViewTab::OnTransactCurrentFrame(TOptional<int> iFrame) const
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;

    int frame = iFrame.Get(player->GetCurrentFrame().FrameNumber.Value);

    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(player);
    currentFrameMutator.Set(frame);
    currentFrameMutator.Commit();
}

uint64
FOdysseyPainterEditorVectorSceneTreeViewTab::GetVectorHUDFlags() const
{
    return mEditor->GetVectorHUDFlags();
}

FOdysseyVectorGroupPaint*
FOdysseyPainterEditorVectorSceneTreeViewTab::GetScene() const
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();

    // for some reason when Unreal loads, the layerstack is NULL. But the medias exist. So in that case we use
    // the media provider.
    if( !layerStack )
    {
        if ( !mEditor->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>() )
            return nullptr;

        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mEditor->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.IsEmpty() )
            return nullptr;

        return mediaVectors[0]->GetScene();
    }

    // layerStack might be NULL when closing the program
    UOdysseyTextureLayerImageVector* currentTextureVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    UOdysseyAnimationLayerImageVector* currentAnimationVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());

    if(  currentAnimationVectorLayer )
    {
        UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
        if (!player)
            return nullptr;

        int frame = player->GetCurrentFrame().FrameNumber.Value;
        UOdysseyAnimationCell* cell = currentAnimationVectorLayer->GetCellAtFrame(frame);
        if (!cell || !cell->IsA<UOdysseyAnimationCellImageVector>())
            return nullptr;

        UOdysseyAnimationCellImageVector* cellVector = Cast<UOdysseyAnimationCellImageVector>(cell);

        // Note: iScene is ignored. We update the widget according to the current scene if any.
        return cellVector->GetVectorCell()->GetScene();
    }
    else if (currentTextureVectorLayer)
    {
        return currentTextureVectorLayer->GetVectorCell()->GetScene();
    }

    return nullptr;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

int
FOdysseyPainterEditorVectorSceneTreeViewTab::WidgetIndex() const
{
    if (GetScene())
        return 1;
    return 0;
}

#undef LOCTEXT_NAMESPACE
