// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorSelectedVectorObjectTab.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorSelectedVectorObjectTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorSelectedVectorObjectTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorSelectedVectorObjectTab::~FOdysseyPainterEditorSelectedVectorObjectTab()
{
}

FOdysseyPainterEditorSelectedVectorObjectTab::FOdysseyPainterEditorSelectedVectorObjectTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(TEXT("OdysseyPainterEditor_SelectedVectorObject"),
                            LOCTEXT( "OdysseyPainterEditorSelectedVectorObjectTab", "Vector Selection" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ))
    , mEditor(iEditor)
{
    mEditor->GetVectorObjectPickTool()->mSelectionChanged.AddRaw(this, &FOdysseyPainterEditorSelectedVectorObjectTab::OnSelectionChanged);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorSelectedVectorObjectTab::CreateWidget()
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    
    mDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

    mDetailsView->SetObject(nullptr);

    return mDetailsView;
}

void
FOdysseyPainterEditorSelectedVectorObjectTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorSelectedVectorObjectTab::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners
void
FOdysseyPainterEditorSelectedVectorObjectTab::OnSelectionChanged()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer )
    {
        if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
        {
            UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

            mDetailsView->SetObject(currentVectorLayer->GetScene()->GetLastSelected());
        }
    }
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

#undef LOCTEXT_NAMESPACE
