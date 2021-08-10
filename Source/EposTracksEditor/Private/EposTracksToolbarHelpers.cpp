// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposTracksToolbarHelpers.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"

#include "Settings/EposTracksEditorSettings.h"

#define LOCTEXT_NAMESPACE "EposTracksToolbarHelpers"

//---

//static
void
EposTracksToolbarHelpers::MakeTextureSettingsEntries( FMenuBuilder& iMenuBuilder )
{
    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "TextureSettingsTitle", "Default Texture Settings" ) );
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );

        // Create a detail view
        FDetailsViewArgs Args;
        Args.bAllowSearch = false;
        Args.NameAreaSettings = FDetailsViewArgs::HideNameArea;
        Args.ColumnWidth = .5f;
        TSharedRef<IDetailsView> DetailView = PropertyModule.CreateDetailView( Args );

        // Filter properties to only get CameraSettings ones
        auto visible_property = []( const FPropertyAndParent& iPropertyChain )
        {
            FName root_name = iPropertyChain.ParentProperties.Num() ? iPropertyChain.ParentProperties.Last()->GetFName() : iPropertyChain.Property.GetFName();
            return root_name == GET_MEMBER_NAME_CHECKED( UEposTracksEditorSettings, TextureSettings );
        };
        DetailView->SetIsPropertyVisibleDelegate( FIsPropertyVisible::CreateLambda( visible_property ) );
        // Set the object to view
        DetailView->SetObject( GetMutableDefault<UEposTracksEditorSettings>() );

        iMenuBuilder.AddWidget( DetailView, FText(), true );
    }
    iMenuBuilder.EndSection();
}

//static
void
EposTracksToolbarHelpers::MakeCameraSettingsEntries( FMenuBuilder& iMenuBuilder )
{
    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "CameraSettingsTitle", "Default Camera Settings" ) );
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );

        // Create a detail view
        FDetailsViewArgs Args;
        Args.bAllowSearch = false;
        Args.NameAreaSettings = FDetailsViewArgs::HideNameArea;
        Args.ColumnWidth = .5f;
        TSharedRef<IDetailsView> DetailView = PropertyModule.CreateDetailView( Args );

        // Filter properties to only get CameraSettings ones
        auto visible_property = []( const FPropertyAndParent& iPropertyChain )
        {
            FName root_name = iPropertyChain.ParentProperties.Num() ? iPropertyChain.ParentProperties.Last()->GetFName() : iPropertyChain.Property.GetFName();
            return root_name == GET_MEMBER_NAME_CHECKED( UEposTracksEditorSettings, CameraSettings );
        };
        DetailView->SetIsPropertyVisibleDelegate( FIsPropertyVisible::CreateLambda( visible_property ) );
        // Set the object to view
        DetailView->SetObject( GetMutableDefault<UEposTracksEditorSettings>() );

        iMenuBuilder.AddWidget( DetailView, FText(), true );
    }
    iMenuBuilder.EndSection();

}

#undef LOCTEXT_NAMESPACE
