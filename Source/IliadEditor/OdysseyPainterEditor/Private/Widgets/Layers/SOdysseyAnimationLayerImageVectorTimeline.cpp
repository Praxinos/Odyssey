// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationLayerImageVectorTimeline.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Misc/MessageDialog.h"
#include "Widgets/Input/SButton.h"

#include "SOdysseyAnimationCellImageStagger.h"
#include "SOdysseyAnimationCellImageVector.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyLayerCellImageStagger.h"
#include "OdysseyLayerStackSelection.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorModule.h"
#include "OdysseyAnimation.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "HUD/OdysseyVectorHUD.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorMassModifierView.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerImageVectorTimeline::~SOdysseyAnimationLayerImageVectorTimeline()
{
}

SOdysseyAnimationLayerImageVectorTimeline::SOdysseyAnimationLayerImageVectorTimeline()
{
}

EVisibility
SOdysseyAnimationLayerImageVectorTimeline::GetRowVisibility(FName iRow) const
{
    if (iRow == "Inbetweening")
    {
        FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
        FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(mLayer->GetAnimation());
        if (!editor)
            return EVisibility::Collapsed;

        return ( editor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN ) ? EVisibility::Visible : EVisibility::Collapsed;
    }

    return SOdysseyAnimationLayerImageTimeline::GetRowVisibility(iRow);
}

EVisibility
SOdysseyAnimationLayerImageVectorTimeline::IsInbetweeningTimelineVisible() const
{
    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(mLayer->GetAnimation());
    if (!editor)
        return EVisibility::Collapsed;

    return ( editor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN ) ? EVisibility::Visible : EVisibility::Collapsed;
}

TSharedPtr<SOdysseyAnimationLayerImageVectorTimelineInbetweening>
SOdysseyAnimationLayerImageVectorTimeline::GetInbetweeningListView()
{
    return mInbetweeningListView;
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorTimeline::OnGenerateCellWidget(UOdysseyLayerCell* iCell)
{
    if (!iCell)
    {
        return SNew(SOdysseyAnimationCellImageVector, Cast<UOdysseyAnimationCellImageVector>(iCell))
            .Clipping(EWidgetClipping::ClipToBoundsAlways);
    }
    if (iCell->IsA<UOdysseyAnimationCellImageVector>())
    {
        return SNew(SOdysseyAnimationCellImageVector, Cast<UOdysseyAnimationCellImageVector>(iCell))
            .Clipping(EWidgetClipping::ClipToBoundsAlways);
    }
    else if (iCell->IsA<UOdysseyLayerCellImageStagger>())
    {
        return SNew(SOdysseyAnimationCellImageStagger, Cast<UOdysseyLayerCellImageStagger>(iCell))
            .TimelinePosition(mTimelinePosition);
    }

    return SNullWidget::NullWidget;
}

FReply
SOdysseyAnimationLayerImageVectorTimeline::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    UOdysseyLayerStack* layerStack = mLayer->GetLayerStack();
    if (!layerStack)
        return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    if (layerStack->GetCurrentLayer() == mLayer)
        return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    layerStack->SetCurrentLayer(mLayer);

    return SOdysseyAnimationLayerImageTimeline::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorTimeline::GenerateWidget( const FName& iRow, const FName& iColumn )
{
    ensure(iColumn == "Timeline");

    if (iRow == "Inbetweening")
    {
        return GenerateInbetweeningRowTimelineWidget();
    }

    return SOdysseyAnimationLayerImageTimeline::GenerateWidget( iRow, iColumn );
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorTimeline::GenerateInbetweeningRowTimelineWidget()
{
    return SAssignNew( mInbetweeningListView, SOdysseyAnimationLayerImageVectorTimelineInbetweening
                                , Cast<UOdysseyAnimationLayerImageVector>(mLayer) )
                            .TimelinePosition(mTimelinePosition)
                            .Visibility( this, &SOdysseyAnimationLayerImageVectorTimeline::IsInbetweeningTimelineVisible );
}

FReply
SOdysseyAnimationLayerImageVectorTimeline::MassModifierAcceptProperties( TSharedRef<SOdysseyPainterEditorVectorMassModifierView> iMassModifierView )
{
    TSharedPtr<SWindow> topWindow;

    iMassModifierView.Get().UndoPreview();
    iMassModifierView.Get().ValidateProperties();

    topWindow = FSlateApplicationBase::Get().GetActiveTopLevelWindow();

    FSlateApplicationBase::Get().RequestDestroyWindow( topWindow.ToSharedRef() );

    return FReply::Handled();
}

void
SOdysseyAnimationLayerImageVectorTimeline::MassModifier()
{
    UOdysseyAnimationLayerImageVector* layerImageVector = Cast<UOdysseyAnimationLayerImageVector>( mLayer->GetLayerStack()->GetCurrentLayer() );
    TArray<UOdysseyLayerCell*> selectedCells = OdysseyLayerStackSelection::GetSelectedCells(mLayer);
    TArray<FOdysseyVectorGroupPaint*> vectorSceneArray;
    FOdysseyVectorGroupPaint* previewScene;
    FOdysseyVectorGroupPaint* previewSceneCopy = nullptr;

    vectorSceneArray.Reserve( selectedCells.Num() );

    if( selectedCells.IsEmpty() )
        return;

    // prevent multiple instances of the mass modifier
    //if( bMassModifierWindowRunning == false )
    {
        for( UOdysseyLayerCell* cell : selectedCells )
        {
            UOdysseyAnimationCellImageVector* animationCell = Cast<UOdysseyAnimationCellImageVector>( cell );

            if( animationCell )
            {
                vectorSceneArray.Push( animationCell->GetVectorCell()->GetScene() );
            }
        }

        UOdysseyLayerCell* cell = mLayer->GetCellAtFrame( mCurrentFrame.Get() );
        UOdysseyAnimationCellImageVector* imageVectorCell = Cast<UOdysseyAnimationCellImageVector>( cell );

        previewScene = imageVectorCell ? imageVectorCell->GetVectorCell()->GetScene() : nullptr;

        TSharedRef<SOdysseyPainterEditorVectorMassModifierView> massModifierView = SNew( SOdysseyPainterEditorVectorMassModifierView )
            .VectorLayer( layerImageVector->GetVectorLayer() )
            .SceneArray( vectorSceneArray )
            .PreviewScene( previewScene );

        TSharedRef<SWindow> ObjectWindow = SNew( SWindow )
            .Title( LOCTEXT( "vector-mass-modifier-window.name", "Mass Modifier" ) )
            //.ClientSize(FVector2D(800, 400))
            .SizingRule( ESizingRule::Autosized )
            .SupportsMaximize( false )
            .SupportsMinimize( false )
            .IsTopmostWindow( true ) // kind-of mimic modal window because we need it to be non-modal for the preview.
            [
                SNew( SVerticalBox )
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign( HAlign_Center )
                    .VAlign( VAlign_Center )
                    [
                        massModifierView
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign( HAlign_Center )
                    .VAlign( VAlign_Center )
                    [
                        SNew( SButton )
                            .Text( LOCTEXT( "vector-mass-modifier-window-apply", "Apply" ) )
                            .OnClicked_Raw( this, &SOdysseyAnimationLayerImageVectorTimeline::MassModifierAcceptProperties, massModifierView )
                    ]
            ];

        // Ask whether or not to apply modified properties
        ObjectWindow.Get().SetOnWindowClosed( FOnWindowClosed::CreateSP( this
                                                                         , &SOdysseyAnimationLayerImageVectorTimeline::MassModifierWindowClosed
                                                                         , massModifierView ) );

        massModifierView.Get().GetOnPreviewPropertiesDelegate().AddLambda( []()
                                                                           {

                                                                           } );
        /*
                // We don't run a ModalWindow because we need the viewport to redraw for previewing.
                FSlateApplication::Get().AddWindow
                (
                    ObjectWindow,
                    true
                );
        */
        // We don't run a ModalWindow because we need the viewport to redraw for previewing.
        FSlateApplication::Get().AddModalWindow
        (
            ObjectWindow,
            FGlobalTabmanager::Get()->GetRootWindow(),
            false
        );
        /*
                //ObjectWindow.Get().ShowWindow();
        */
        //bMassModifierWindowRunning = true;
    }
}

void
SOdysseyAnimationLayerImageVectorTimeline::MassModifierWindowClosed( const TSharedRef<SWindow>& iWindow
                                                               , TSharedRef<SOdysseyPainterEditorVectorMassModifierView> iMassModifierView )
{
    UOdysseyAnimationLayerImageVector* layerImageVector = Cast<UOdysseyAnimationLayerImageVector>( mLayer->GetLayerStack()->GetCurrentLayer() );
    FText dialogText = LOCTEXT( "mass-modifier.apply-properties.title", "Apply Properties ?" );

    if( iMassModifierView.Get().HasAnyPropertyBit() )
    {
        iMassModifierView.Get().UndoPreview();

        if( FMessageDialog::Open( EAppMsgType::YesNo, dialogText ) == EAppReturnType::Yes )
        {
            MassModifierAcceptProperties( iMassModifierView );
        }
    }

    //bMassModifierWindowRunning = false;
}

void
SOdysseyAnimationLayerImageVectorTimeline::BuildContextMenu( TSharedRef<FUICommandList> CommandList, FMenuBuilder& MenuBuilder, FFrameNumber iClickedFrame, TSharedRef<FExtender> MenuExtender )
{
    MenuExtender->AddMenuExtension(
        "Cells",
        EExtensionHook::After,
        CommandList,
        FMenuExtensionDelegate::CreateSP( this, &SOdysseyAnimationLayerImageVectorTimeline::AddCellsMenuEntries ) );

    SOdysseyAnimationLayerImageTimeline::BuildContextMenu( CommandList, MenuBuilder, iClickedFrame, MenuExtender );
}

void
SOdysseyAnimationLayerImageVectorTimeline::AddCellsMenuEntries( FMenuBuilder& MenuBuilder )
{
    //MenuBuilder.BeginSection("More", LOCTEXT("timeline-cells.context-menu.mass-modifier.name", "Mass Modifier"));

    MenuBuilder.AddMenuEntry(
        LOCTEXT( "timeline-cells.context-menu.mass-modifier.name", "Mass Modifier" )
        , LOCTEXT( "timeline-cells.context-menu.mass-modifier.tooltip", "Mass Modifier" )
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateSP( this, &SOdysseyAnimationLayerImageVectorTimeline::MassModifier ) ) );

}

#undef LOCTEXT_NAMESPACE
