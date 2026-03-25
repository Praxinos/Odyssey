// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "Widgets/SArianeEditorToolOptions.h"
#include "ArianeEditor.h"
// Unreal headers
#include "ObjectEditorUtils.h"
#include "PropertyEditorModule.h"
#include "ISinglePropertyView.h"
#include "Tools/ArianeEditorTool.h"

/////////////////////////////////////////////////////
// SArianeEditorToolOptions
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SArianeEditorToolOptions::Construct( const FArguments& InArgs, FArianeEditor* iEditor )
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    Editor = iEditor;

    // Create a details view
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Automatic;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

    DetailsView = PropertyEditorModule.CreateDetailView( DetailsViewArgs );
    DetailsView->SetObject( Editor->GetCurrentTool() );

    this->ChildSlot
    [
        DetailsView.ToSharedRef()
    ];

    Editor->OnPostCurrentToolChangedDelegate().AddSP( this, &SArianeEditorToolOptions::OnPostCurrentToolChanged );
}

void
SArianeEditorToolOptions::OnPostCurrentToolChanged()
{
    DetailsView->SetObject( Editor->GetCurrentTool() );
}
