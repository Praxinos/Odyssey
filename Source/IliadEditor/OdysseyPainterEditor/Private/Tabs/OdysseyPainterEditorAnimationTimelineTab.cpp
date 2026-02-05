// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorAnimationTimelineTab.h"

#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "SOdysseyAnimationLayerStack.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "Misc/ScopedSlowTask.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "DesktopPlatformModule.h"

#include "Toolkits/BaseToolkit.h"
#include "PaperFlipbook.h"
#include "OdysseyPixelFormat.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "Widgets/Animation/SOdysseyAnimationExportImageSequenceDialog.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationSource.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyPainterEditorAnimationFunctionLibrary.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Tools/OutOfPegsTool/OdysseyPainterEditorAnimationOutOfPegsTool.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

const FName&
FOdysseyPainterEditorAnimationTimelineTab::StaticId()
{
    static FName Id = TEXT("OdysseyAnimationEditor_LayerStack");  //Dont change, Old Id for retro compatibility
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorAnimationTimelineTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorAnimationTimelineTab::~FOdysseyPainterEditorAnimationTimelineTab()
{
}

FOdysseyPainterEditorAnimationTimelineTab::FOdysseyPainterEditorAnimationTimelineTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "timeline-tab.name", "Timeline" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Layers16" ))
    , mEditor(iEditor)
    , mEmptyTimelineTabWidget(CreateDefaultEmptyTimelineTabWidget())
    , mAnimationExportShortcuts(iEditor)
{
}

void
FOdysseyPainterEditorAnimationTimelineTab::SetEmptyTimelineWidget(TSharedRef<SWidget> iWidget)
{
    mEmptyTimelineTabWidget = iWidget;
}

const FName&
FOdysseyPainterEditorAnimationTimelineTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorAnimationTimelineTab::CreateWidget()
{

    return
        SNew(SWidgetSwitcher)
        .WidgetIndex_Lambda([this](){ return Animation() == nullptr ? 1 : 0; })
        +SWidgetSwitcher::Slot()
        [
            SNew(SOdysseyAnimationLayerStack)
            .Animation(this, &FOdysseyPainterEditorAnimationTimelineTab::Animation)
            .Player(this, &FOdysseyPainterEditorAnimationTimelineTab::Player)
            .TimelinePosition(this, &FOdysseyPainterEditorAnimationTimelineTab::GetTimelinePosition)
            .OnActivateOutOfPegs(this, &FOdysseyPainterEditorAnimationTimelineTab::OnActivateOutOfPegs)
            .OnInactivateOutOfPegs(this, &FOdysseyPainterEditorAnimationTimelineTab::OnInactivateOutOfPegs)
            .OnIsOutOfPegsChecked(this, &FOdysseyPainterEditorAnimationTimelineTab::OnIsOutOfPegsChecked)
            .CustomValidRange(this, &FOdysseyPainterEditorAnimationTimelineTab::GetAnimationValidRange)
            .OnScrubStart(this, &FOdysseyPainterEditorAnimationTimelineTab::OnScrubStart)
            .OnScrubEnd(this, &FOdysseyPainterEditorAnimationTimelineTab::OnScrubEnd)
        ]
        +SWidgetSwitcher::Slot()
        [
            //Display a PlaceHolder when no layerstack can be displayed
            mEmptyTimelineTabWidget.ToSharedRef()
        ];
}

TSharedPtr<SWidget>
FOdysseyPainterEditorAnimationTimelineTab::CreateDefaultEmptyTimelineTabWidget() const
{
    return SNew(STextBlock)
        .Text(LOCTEXT("timeline-tab.nothing-to-display", "No Timeline can be displayed"));
}

void
FOdysseyPainterEditorAnimationTimelineTab::ExtendMenu(TSharedRef<FExtender> iExtender)
{
    ExtendMenuFile(iExtender);
}

bool
FOdysseyPainterEditorAnimationTimelineTab::CanOpen() const
{
    //Always display this tab if we use the Odyssey Ed Mode
    if (!mEditor->GetToolkit()->IsAssetEditor())
        return true;

    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    return source && source->Id() == FOdysseyPainterEditorAnimationSource::StaticId();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyAnimation*
FOdysseyPainterEditorAnimationTimelineTab::Animation() const
{
    return mEditor->GetAnimation();
}

UOdysseyAnimationPlayer*
FOdysseyPainterEditorAnimationTimelineTab::Player() const
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorAnimationSource::StaticId())
        return nullptr;

    TSharedPtr<FOdysseyPainterEditorAnimationSource> animSource = StaticCastSharedPtr<FOdysseyPainterEditorAnimationSource>(source);
    return animSource->GetAnimationPlayer();
}

TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>
FOdysseyPainterEditorAnimationTimelineTab::GetTimelinePosition() const
{
    return mEditor->GetAnimationTimelinePosition();
}

void
FOdysseyPainterEditorAnimationTimelineTab::OnScrubStart()
{
    UOdysseyAnimationPlayer* player = Player();
    if (!player)
        return;

    player->BeginScrub();
}

void
FOdysseyPainterEditorAnimationTimelineTab::OnScrubEnd()
{
    UOdysseyAnimationPlayer* player = Player();
    if (!player)
        return;

    player->EndScrub();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyPainterEditorAnimationTimelineTab::ExtendMenuFile( TSharedRef<FExtender> iExtender )
{
    TSharedPtr<FUICommandList> commandList = MakeShared<FUICommandList>();
    mAnimationExportShortcuts.MapActionsToCommandList(commandList.ToSharedRef());

    iExtender->AddMenuExtension(
        "OdysseyFile",
        EExtensionHook::After,
        commandList,
        FMenuExtensionDelegate::CreateLambda(
            [this](FMenuBuilder& iBuilder)
            {
                TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
                if (!source || source->Id() != FOdysseyPainterEditorAnimationSource::StaticId())
                    return;

                iBuilder.BeginSection("OdysseyAnimation", LOCTEXT("timeline-tab.file-menu.animation-import-export-section.name", "Animation Import / Export"));
                {
                    iBuilder.AddSubMenu(
                        LOCTEXT("timeline-tab.file-menu.import-submenu.name", "Import"),
                        LOCTEXT("timeline-tab.file-menu.import-submenu.tooltip", "Contains Import actions"),
                        FNewMenuDelegate::CreateRaw(this, &FOdysseyPainterEditorAnimationTimelineTab::BuildImportMenu),
                        false,
                        FSlateIcon( "OdysseyStyle", "AnimationEditor.File-Menu.Import" )
                    );

                    iBuilder.AddSubMenu(
                        LOCTEXT("timeline-tab.file-menu.export-submenu.name", "Export"),
                        LOCTEXT("timeline-tab.file-menu.export-submenu.tooltip", "Contains Export actions"),
                        FNewMenuDelegate::CreateRaw(this, &FOdysseyPainterEditorAnimationTimelineTab::BuildExportMenu),
                        false,
                        FSlateIcon( "OdysseyStyle", "AnimationEditor.File-Menu.Export" )
                    );
                }
                iBuilder.EndSection();
            }
        )
    );
}

void
FOdysseyPainterEditorAnimationTimelineTab::BuildImportMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.AddMenuEntry(
        FOdysseyPainterEditorAnimationCommands::Get().ImportImageSequence,
        NAME_None,
        LOCTEXT("timeline-tab.file-menu.import-image-sequence.name", "Image Sequence...")
    );
    iMenuBuilder.AddMenuEntry(
        FOdysseyPainterEditorAnimationCommands::Get().ImportTextureSequence,
        NAME_None,
        LOCTEXT("timeline-tab.file-menu.import-texture-sequence.name", "Texture Sequence...")
    );
}

void
FOdysseyPainterEditorAnimationTimelineTab::BuildExportMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.AddMenuEntry(
        FOdysseyPainterEditorAnimationCommands::Get().ExportImageSequence,
        NAME_None,
        LOCTEXT("timeline-tab.file-menu.export-image-sequence.name", "Image Sequence...")
    );
    iMenuBuilder.AddMenuEntry(
        FOdysseyPainterEditorAnimationCommands::Get().ExportTextureSequence,
        NAME_None,
        LOCTEXT("timeline-tab.file-menu.export-texture-sequence.name", "Texture Sequence...")
    );
    iMenuBuilder.AddMenuEntry(
        FOdysseyPainterEditorAnimationCommands::Get().ExportAsFlipbook,
        NAME_None,
        LOCTEXT("timeline-tab.file-menu.export-as-flipbook.name", "Flipbook...")
    );
}

void
FOdysseyPainterEditorAnimationTimelineTab::OnActivateOutOfPegs(UOdysseyLayerCell* iCell)
{
    if (!iCell)
        return;

    mEditor->GetOutOfPegsTool()->SetCell(iCell);
    mEditor->ActivateTemporaryTool(mEditor->GetOutOfPegsTool());
}

void
FOdysseyPainterEditorAnimationTimelineTab::OnInactivateOutOfPegs()
{
    mEditor->InactivateTemporaryTool();
}

ECheckBoxState
FOdysseyPainterEditorAnimationTimelineTab::OnIsOutOfPegsChecked(UOdysseyLayerCell* iCell)
{
    if (!iCell)
        return ECheckBoxState::Unchecked;

    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return ECheckBoxState::Unchecked;

    bool isToolActive = tool->IsA(UOdysseyPainterEditorAnimationOutOfPegsTool::StaticClass());
    if (!isToolActive)
        return ECheckBoxState::Unchecked;

    UOdysseyPainterEditorAnimationOutOfPegsTool* outOfPegsTool = Cast<UOdysseyPainterEditorAnimationOutOfPegsTool>(tool);
    if (outOfPegsTool->GetCell() != iCell)
        return ECheckBoxState::Unchecked;

    return ECheckBoxState::Checked;
}

FInt32Range
FOdysseyPainterEditorAnimationTimelineTab::GetAnimationValidRange() const
{
    if (mAnimationValidRange.IsBound())
        return mAnimationValidRange.Get();

    return FInt32Range::Empty();
}

void
FOdysseyPainterEditorAnimationTimelineTab::SetAnimationValidRange(const TAttribute<FInt32Range>& iValue)
{
    mAnimationValidRange = iValue;
}

#undef LOCTEXT_NAMESPACE
