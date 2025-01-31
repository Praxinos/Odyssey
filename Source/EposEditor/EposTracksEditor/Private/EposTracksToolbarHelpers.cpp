// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposTracksToolbarHelpers.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "IDetailsView.h"
#include "IStructureDetailsView.h"
#include "PropertyEditorModule.h"
#include "Widgets/Input/SEditableTextBox.h"

#include "Settings/EposTracksEditorSettings.h"
#include "Widgets/SOdysseyAnimationConfigureWindow.h"

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
EposTracksToolbarHelpers::MakeAnimationSettingsEntries( FMenuBuilder& iMenuBuilder )
{
    static FOdysseyAnimationConfiguration configuration; //PATCH: must be a USettings like in MakeTextureSettingsEntries() above, to be able to easily access to GetMutableDefault<>()

    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "animation-settings.section-title", "Default Animation Settings" ) );
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );

        FStructureDetailsViewArgs structureDetailsViewArgs;
        FDetailsViewArgs detailsViewArgs;
        detailsViewArgs.bAllowSearch = false;
        detailsViewArgs.bShowScrollBar = false;
        detailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
        detailsViewArgs.ColumnWidth = .5f;

        TSharedRef<FStructOnScope> structOnScope = MakeShared<FStructOnScope>( FOdysseyAnimationConfiguration::StaticStruct(), reinterpret_cast<uint8*>( &configuration ) );
        TSharedPtr<IStructureDetailsView> configurationDetailsView = PropertyModule.CreateStructureDetailView( detailsViewArgs, structureDetailsViewArgs, structOnScope );      //TODO: must remove the width parameter to auto compute it OR make a custom USettings ???

        iMenuBuilder.AddWidget( configurationDetailsView->GetWidget().ToSharedRef(), FText(), true );

        // Create a detail view
        //FDetailsViewArgs Args;
        //Args.bAllowSearch = false;
        //Args.NameAreaSettings = FDetailsViewArgs::HideNameArea;
        //Args.ColumnWidth = .5f;
        //TSharedRef<IDetailsView> DetailView = PropertyModule.CreateDetailView( Args );

        // Filter properties to only get CameraSettings ones
        //auto visible_property = []( const FPropertyAndParent& iPropertyChain )
        //{
        //    FName root_name = iPropertyChain.ParentProperties.Num() ? iPropertyChain.ParentProperties.Last()->GetFName() : iPropertyChain.Property.GetFName();
        //    return root_name == GET_MEMBER_NAME_CHECKED( FOdysseyAnimationConfiguration, TextureSettings );
        //};
        //DetailView->SetIsPropertyVisibleDelegate( FIsPropertyVisible::CreateLambda( visible_property ) );
        // Set the object to view
        //DetailView->SetObject( GetMutableDefault<FOdysseyAnimationConfiguration>() );

        //iMenuBuilder.AddWidget( DetailView, FText(), true );
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

namespace
{
static
void
TextCommited( const FText& iNewText, ETextCommit::Type iType, TSharedRef<FString> ioCameraName, FSimpleDelegate iOnTextCommit )
{
    if( iType == ETextCommit::OnCleared )
        return;

    *ioCameraName = iNewText.ToString();

    if( iType == ETextCommit::OnEnter )
        iOnTextCommit.Execute();
}
}

//static
void
EposTracksToolbarHelpers::MakeCameraEntries( FMenuBuilder& iMenuBuilder, TSharedRef<FString> ioCameraName, FSimpleDelegate iOnTextCommit, bool iFocus )
{
    TSharedPtr<SEditableTextBox> text_widget;

    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "camera-options.section-title", "Camera" ) );
    {
        //iMenuBuilder.AddEditableText( ... ); // This won't display the section ... so use the classic widget ...

        iMenuBuilder.AddWidget( SNew( SBox )
                                .Padding( FAppStyle::Get().GetMargin( "Menu.Block.IndentedPadding" ) ) // If no label, the widget will be at the full menu width without margin
                                [
                                    SNew( SHorizontalBox )
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
                                        SAssignNew( text_widget, SEditableTextBox )
                                        .Text( FText::FromString( *ioCameraName ) )
                                        .ToolTipText( LOCTEXT( "camera-set-name-tooltip", "Set the camera name" ) )
                                        .OnTextCommitted( FOnTextCommitted::CreateStatic( TextCommited, ioCameraName, iOnTextCommit ) )
                                        .SelectAllTextWhenFocused( true )
                                    ]
                               ],
                               FText::GetEmpty() );
    }
    iMenuBuilder.EndSection();

    if( iFocus )
    {
        // Same as in D:\Epic Games\UE_4.27\Engine\Source\Editor\ContentBrowser\Private\SAssetPicker.cpp
        text_widget->RegisterActiveTimer( 0.f, FWidgetActiveTimerDelegate::CreateLambda( [=]( double InCurrentTime, float InDeltaTime ) -> EActiveTimerReturnType
                                                                                         {
                                                                                             if( text_widget.IsValid() )
                                                                                             {
                                                                                                 FWidgetPath WidgetToFocusPath;
                                                                                                 FSlateApplication::Get().GeneratePathToWidgetUnchecked( text_widget.ToSharedRef(), WidgetToFocusPath );
                                                                                                 FSlateApplication::Get().SetKeyboardFocus( WidgetToFocusPath, EFocusCause::SetDirectly );
                                                                                                 WidgetToFocusPath.GetWindow()->SetWidgetToFocusOnActivate( text_widget );

                                                                                                 return EActiveTimerReturnType::Stop;
                                                                                             }

                                                                                             return EActiveTimerReturnType::Continue;
                                                                                         } ) );
    }
}

//static
void
EposTracksToolbarHelpers::MakePlaneEntries( FMenuBuilder& iMenuBuilder, TSharedRef<FString> ioPlaneName, FSimpleDelegate iOnTextCommit, bool iFocus )
{
    TSharedPtr<SEditableTextBox> text_widget;

    iMenuBuilder.BeginSection( NAME_None, LOCTEXT( "plane-options.section-title", "Plane" ) );
    {
        //MenuBuilder.AddEditableText( ... ); // This won't display the section ... so use the classic widget ...

        iMenuBuilder.AddWidget( SNew( SBox )
                                .Padding( FAppStyle::Get().GetMargin( "Menu.Block.IndentedPadding" ) ) // If no label, the widget will be at the full menu width without margin
                                [
                                    SNew( SHorizontalBox )
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
                                        SAssignNew( text_widget, SEditableTextBox )
                                        .Text( FText::FromString( *ioPlaneName ) )
                                        .ToolTipText( LOCTEXT( "plane-set-name-tooltip", "Set the plane name" ) )
                                        .OnTextCommitted( FOnTextCommitted::CreateStatic( TextCommited, ioPlaneName, iOnTextCommit ) )
                                        .SelectAllTextWhenFocused( true )
                                    ]
                               ],
                               FText::GetEmpty() );
    }
    iMenuBuilder.EndSection();

    if( iFocus )
    {
        // Same as in D:\Epic Games\UE_4.27\Engine\Source\Editor\ContentBrowser\Private\SAssetPicker.cpp
        text_widget->RegisterActiveTimer( 0.f, FWidgetActiveTimerDelegate::CreateLambda( [=]( double InCurrentTime, float InDeltaTime ) -> EActiveTimerReturnType
                                                                                         {
                                                                                             if( text_widget.IsValid() )
                                                                                             {
                                                                                                 FWidgetPath WidgetToFocusPath;
                                                                                                 FSlateApplication::Get().GeneratePathToWidgetUnchecked( text_widget.ToSharedRef(), WidgetToFocusPath );
                                                                                                 FSlateApplication::Get().SetKeyboardFocus( WidgetToFocusPath, EFocusCause::SetDirectly );
                                                                                                 WidgetToFocusPath.GetWindow()->SetWidgetToFocusOnActivate( text_widget );

                                                                                                 return EActiveTimerReturnType::Stop;
                                                                                             }

                                                                                             return EActiveTimerReturnType::Continue;
                                                                                         } ) );
    }
}

#undef LOCTEXT_NAMESPACE
