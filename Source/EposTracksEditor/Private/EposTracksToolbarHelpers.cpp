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
EposTracksToolbarHelpers::MakePlaneSettingsEntries( FMenuBuilder& iMenuBuilder )
{
    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "plane-settings.section-title", "Default Plane Settings" ) );
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
            return root_name == GET_MEMBER_NAME_CHECKED( UEposTracksEditorSettings, PlaneSettings );
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
EposTracksToolbarHelpers::MakeTextureSettingsEntries( FMenuBuilder& iMenuBuilder )
{
    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "texture-settings.section-title", "Default Texture Settings" ) );
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
    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "camera-settings.section-title", "Default Camera Settings" ) );
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

//static
void
EposTracksToolbarHelpers::MakeCameraEntries( FMenuBuilder& iMenuBuilder, TSharedRef<FString> ioCameraName )
{
    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "camera-options.section-title", "Camera" ) );
    {
        auto TextCommited = [=]( const FText& iNewText, ETextCommit::Type iType )
        {
            if( !iType == ETextCommit::OnEnter )
                return;

            *ioCameraName = iNewText.ToString();
        };

        //MenuBuilder.AddEditableText( ... ); // This won't display the section ... so use the classic widget ...

        iMenuBuilder.AddWidget( SNew( SHorizontalBox )
                               + SHorizontalBox::Slot()
                               .FillWidth( 1.f )
                               [
                                   SNew( STextBlock )
                                   .Text( LOCTEXT( "camera-set-name-label", "Name" ) )
                                   .ToolTipText( LOCTEXT( "camera-set-name-tooltip", "Set the camera name" ) )
                               ]
                               + SHorizontalBox::Slot()
                               .FillWidth( 5.f )
                               [
                                   SNew( SEditableTextBox )
                                   .Text( FText::FromString( *ioCameraName ) )
                                   .ToolTipText( LOCTEXT( "camera-set-name-tooltip", "Set the camera name" ) )
                                   .OnTextCommitted( FOnTextCommitted::CreateLambda( TextCommited ) )
                               ],
                               FText::GetEmpty() );
    }
    iMenuBuilder.EndSection();
}

//static
void
EposTracksToolbarHelpers::MakePlaneEntries( FMenuBuilder& iMenuBuilder, TSharedRef<FString> ioPlaneName )
{
    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "plane-options.section-title", "Plane" ) );
    {
        auto TextCommited = [=]( const FText& iNewText, ETextCommit::Type iType )
        {
            if( !iType == ETextCommit::OnEnter )
                return;

            *ioPlaneName = iNewText.ToString();
        };

        //MenuBuilder.AddEditableText( ... ); // This won't display the section ... so use the classic widget ...

        iMenuBuilder.AddWidget( SNew( SHorizontalBox )
                               + SHorizontalBox::Slot()
                               .FillWidth( 1.f )
                               [
                                   SNew( STextBlock )
                                   .Text( LOCTEXT( "plane-set-name-label", "Name" ) )
                                   .ToolTipText( LOCTEXT( "plane-set-name-tooltip", "Set the plane name" ) )
                               ]
                               + SHorizontalBox::Slot()
                               .FillWidth( 5.f )
                               [
                                   SNew( SEditableTextBox )
                                   .Text( FText::FromString( *ioPlaneName ) )
                                   .ToolTipText( LOCTEXT( "plane-set-name-tooltip", "Set the plane name" ) )
                                   .OnTextCommitted( FOnTextCommitted::CreateLambda( TextCommited ) )
                               ],
                               FText::GetEmpty() );
    }
    iMenuBuilder.EndSection();
}

#undef LOCTEXT_NAMESPACE
