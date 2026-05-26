// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Tools/SOdysseyPainterEditorToolOptions.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ISinglePropertyView.h"
#include "Modules/ModuleManager.h"
#include "ObjectEditorUtils.h"
#include "PropertyEditorModule.h"

#include "Tools/OdysseyPainterEditorTool.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorToolOptions
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyPainterEditorToolOptions::Construct( const FArguments& InArgs )
{
    mTool = InArgs._Tool;
    mDisplayedTool = mTool.Get();

    mOptionsExtender = CreateOptionsExtender();

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    // Create a details view
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Automatic;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = true;
    DetailsViewArgs.bShowOptions = true;
    DetailsViewArgs.bShowModifiedPropertiesOption = true;
    DetailsViewArgs.bShowPropertyMatrixButton = false;
    DetailsViewArgs.bShowDifferingPropertiesOption = false;
    DetailsViewArgs.bShowHiddenPropertiesWhilePlayingOption = false;
    DetailsViewArgs.bShowKeyablePropertiesOption = false;
    DetailsViewArgs.bShowAnimatedPropertiesOption = false;
    DetailsViewArgs.bShowSectionSelector = false;
    DetailsViewArgs.OptionsExtender = mOptionsExtender;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

    mDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    mDetailsView->SetObject(mDisplayedTool);

    this->ChildSlot
    [
        mDetailsView.ToSharedRef()
    ];
}

void
SOdysseyPainterEditorToolOptions::ResetAllToolParameters()
{
    UOdysseyPainterEditorTool* tool = mTool.Get();
    if (!tool)
        return;

    tool->Reset();

    mDetailsView->InvalidateCachedState(); //Makes the "ResetToDefault" buttons disappear
}

TSharedRef<FExtender>
SOdysseyPainterEditorToolOptions::CreateOptionsExtender()
{
    TSharedRef<FExtender> extender = MakeShared<FExtender>();

    extender->AddMenuExtension(
        "DetailsViewCategories",
        EExtensionHook::After,
        MakeShared<FUICommandList>(),
        FMenuExtensionDelegate::CreateLambda(
            [this](FMenuBuilder& iMenuBuilder)
            {
                iMenuBuilder.BeginSection("Tool", LOCTEXT("tool-options.options-menu.tool-section.name", "Tool"));
                {
                    iMenuBuilder.AddMenuEntry(
                        LOCTEXT("tool-options.options-menu.reset-all-parameters.name", "Reset all Parameters"),
                        LOCTEXT("tool-options.options-menu.reset-all-parameters.tooltip", "Resets all the current tools parameters to match their default value."),
                        FSlateIcon(),
                        FUIAction(
                            FExecuteAction::CreateRaw( this, &SOdysseyPainterEditorToolOptions::ResetAllToolParameters )
                        )
                    );
                }
                iMenuBuilder.EndSection();
            }
        )
    );

    return extender;
}

void
SOdysseyPainterEditorToolOptions::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    UOdysseyPainterEditorTool* tool = mTool.Get();
    if (tool != mDisplayedTool)
    {
        mDisplayedTool = tool;
        mDetailsView->SetObject(mDisplayedTool);
    }
}

#undef LOCTEXT_NAMESPACE
