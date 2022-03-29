// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Render/SRenderOptions.h"

#include "ContentBrowserModule.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IContentBrowserSingleton.h"
#include "IDetailCustomization.h"
#include "Interfaces/IPluginManager.h"
#include "MoviePipelineCommandLineEncoder.h"
#include "MoviePipelineCommandLineEncoderSettings.h"
#include "MoviePipelineMasterConfig.h"
#include "MovieSceneSequence.h"
//#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/STextComboBox.h"

#define LOCTEXT_NAMESPACE "SRenderOptions"

//---

/**
 *  Customizes FMovieSceneSection to expose the section bounds to the UI and allow changing their bounded states.
 */
class FEncoderSettingsDetailsCustomization
    : public IDetailCustomization
{
public:
    static TSharedRef<IDetailCustomization> MakeInstance( bool* ioIsExecutablePathValid );

    FEncoderSettingsDetailsCustomization( bool* ioIsExecutablePathValid );

    /** IDetailCustomization interface */
    virtual void CustomizeDetails( IDetailLayoutBuilder& DetailBuilder );

private:
    void OnVideoCodecChanged( TSharedPtr<FString> iEntry, ESelectInfo::Type iType );
    void OnAudioCodecChanged( TSharedPtr<FString> iEntry, ESelectInfo::Type iType );

    TSharedRef<SWidget> OnGenerateCodecEntryWidget( TSharedPtr<FString> iEntry );

private:
    void ExecutablePathChanged( const UMoviePipelineCommandLineEncoderSettings* iEncoderSettings );

    bool IsExecutablePathValid( const UMoviePipelineCommandLineEncoderSettings* iEncoderSettings ) const;
    bool IsVideoCodecValid( const UMoviePipelineCommandLineEncoderSettings* iEncoderSettings ) const;
    bool IsAudioCodecValid( const UMoviePipelineCommandLineEncoderSettings* iEncoderSettings ) const;

    void UpdateCodecs( const UMoviePipelineCommandLineEncoderSettings* iEncoderSettings );

private:
    bool* mIsExecutablePathValid { nullptr };

    TSharedPtr<IPropertyHandle> mVideoCodecPropertyHandle;
    TSharedPtr<IPropertyHandle> mAudioCodecPropertyHandle;

    TArray<TSharedPtr<FString>> mVideoCodecs;
    TArray<TSharedPtr<FString>> mAudioCodecs;
};

//static
TSharedRef<IDetailCustomization>
FEncoderSettingsDetailsCustomization::MakeInstance( bool* ioIsExecutablePathValid )
{
    return MakeShared<FEncoderSettingsDetailsCustomization>( ioIsExecutablePathValid );
}

FEncoderSettingsDetailsCustomization::FEncoderSettingsDetailsCustomization( bool* ioIsExecutablePathValid )
    : mIsExecutablePathValid( ioIsExecutablePathValid )
{
    check( mIsExecutablePathValid );
}

void
FEncoderSettingsDetailsCustomization::OnVideoCodecChanged( TSharedPtr<FString> iEntry, ESelectInfo::Type iType )
{
    if( !iEntry.IsValid() )
        return;

    mVideoCodecPropertyHandle->SetValue( *iEntry.Get() );
}

void
FEncoderSettingsDetailsCustomization::OnAudioCodecChanged( TSharedPtr<FString> iEntry, ESelectInfo::Type iType )
{
    if( !iEntry.IsValid() )
        return;

    mAudioCodecPropertyHandle->SetValue( *iEntry.Get() );
}

TSharedRef<SWidget>
FEncoderSettingsDetailsCustomization::OnGenerateCodecEntryWidget( TSharedPtr<FString> iEntry )
{
    return
        SNew( SBox )
        .WidthOverride( 150.0f )
        [
            SNew( STextBlock )
            .Text( FText::FromString( *iEntry ) )
            .Font( IDetailLayoutBuilder::GetDetailFont() )
        ];
}

bool
FEncoderSettingsDetailsCustomization::IsExecutablePathValid( const UMoviePipelineCommandLineEncoderSettings* iEncoderSettings ) const
{
    if( iEncoderSettings->ExecutablePath.Len() == 0 )
        return false;

    int32 error_code;
    FString stdout_string;
    FString error_string;
    bool success = FPlatformProcess::ExecProcess( *iEncoderSettings->ExecutablePath, TEXT( "" ), &error_code, &stdout_string, &error_string );
    if( !success /*|| error_code != 0*/ ) // error_code == 2 when error and error_code == 1 when succeded
        return false;

    return true;
}

static
void GetCodecs( FString iExecutablePath, TArray<TSharedPtr<FString>>* oVideoCodecs, TArray<TSharedPtr<FString>>* oAudioCodecs )
{
}

void
FEncoderSettingsDetailsCustomization::UpdateCodecs( const UMoviePipelineCommandLineEncoderSettings* iEncoderSettings )
{
    mVideoCodecs.Empty();
    mAudioCodecs.Empty();

    FString command_line = TEXT( "-encoders" );
    int32 error_code;
    FString stdout_string;
    FString error_string;
    bool success = FPlatformProcess::ExecProcess( *iEncoderSettings->ExecutablePath, *command_line, &error_code, &stdout_string, &error_string );
    if( !success )
        return;

    // Each (interesting) line has this format:
    //  VF.... zlib                 LCL (LossLess Codec Library) ZLIB
    //  V....D zmbv                 Zip Motion Blocks Video

    TArray<FString> lines;
    stdout_string.ParseIntoArray( lines, LINE_TERMINATOR, true );

    TArray<FString> line_contents;
    for( const FString& line : lines )
    {
        line_contents.Reset();
        line.ParseIntoArray( line_contents, TEXT( " " ), true );

        if( line_contents.Num() < 3 )
            continue;

        if( line_contents[1] == TEXT( "=" ) )
            continue;

        if( line_contents[0].StartsWith( TEXT( "V" ) ) )
        {
            mVideoCodecs.Add( MakeShareable( new FString( line_contents[1] ) ) );
        }
        else if( line_contents[0].StartsWith( TEXT( "A" ) ) )
        {
            mAudioCodecs.Add( MakeShareable( new FString( line_contents[1] ) ) );
        }
    }
}

bool
FEncoderSettingsDetailsCustomization::IsVideoCodecValid( const UMoviePipelineCommandLineEncoderSettings* iEncoderSettings ) const
{
    return mVideoCodecs.ContainsByPredicate( [iEncoderSettings]( TSharedPtr<FString> iEntry ) { return *iEntry.Get() == iEncoderSettings->VideoCodec; } );
}

bool
FEncoderSettingsDetailsCustomization::IsAudioCodecValid( const UMoviePipelineCommandLineEncoderSettings* iEncoderSettings ) const
{
    return mAudioCodecs.ContainsByPredicate( [iEncoderSettings]( TSharedPtr<FString> iEntry ) { return *iEntry.Get() == iEncoderSettings->AudioCodec; } );
}

void
FEncoderSettingsDetailsCustomization::ExecutablePathChanged( const UMoviePipelineCommandLineEncoderSettings* iEncoderSettings )
{
    *mIsExecutablePathValid = IsExecutablePathValid( iEncoderSettings );

    UpdateCodecs( iEncoderSettings );
}

void
FEncoderSettingsDetailsCustomization::CustomizeDetails( IDetailLayoutBuilder& DetailBuilder )
{
    const TArray< TWeakObjectPtr<UObject> >& objects = DetailBuilder.GetSelectedObjects();
    check( objects.Num() == 1 && objects[0].IsValid() );
    const UMoviePipelineCommandLineEncoderSettings* encoder_settings = Cast<UMoviePipelineCommandLineEncoderSettings>( objects[0].Get() );

    ExecutablePathChanged( encoder_settings );

    //--- Customize properties inside "Command Line Encoder" category of UMoviePipelineCommandLineEncoderSettings

    IDetailCategoryBuilder& encoderCategory = DetailBuilder.EditCategory( "Command Line Encoder" );

    //--- Customize the ExecutablePath property

    TSharedPtr<IPropertyHandle> executablePathPropertyHandle = DetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED( UMoviePipelineCommandLineEncoderSettings, ExecutablePath ) );

    executablePathPropertyHandle->SetOnPropertyValueChanged( FSimpleDelegate::CreateRaw( this, &FEncoderSettingsDetailsCustomization::ExecutablePathChanged, encoder_settings ) );

    IDetailPropertyRow& executable_path_row = encoderCategory.AddProperty( executablePathPropertyHandle );

    executable_path_row.CustomWidget()
        .NameContent()
        [
            executablePathPropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .HAlign( HAlign_Fill )
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth( 5 )
            [
                executablePathPropertyHandle->CreatePropertyValueWidget()
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign( VAlign_Center )
            .Padding( 5, 0 )
            [
                SNew( SImage )
                .Image( FEditorStyle::GetBrush( "Icons.Warning" ) )
                .Visibility_Lambda( [this]() { return *mIsExecutablePathValid ? EVisibility::Hidden : EVisibility::Visible; } )
            ]

            + SHorizontalBox::Slot()
            [
                SNew( SSpacer )
            ]
        ];

    //--- Remove the CodecHelpText property

    TSharedPtr<IPropertyHandle> helpPropertyHandle = DetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED( UMoviePipelineCommandLineEncoderSettings, CodecHelpText ) );
    helpPropertyHandle->MarkHiddenByCustomization();

    //--- Customize the VideoCodec property

    mVideoCodecPropertyHandle = DetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED( UMoviePipelineCommandLineEncoderSettings, VideoCodec ) );

    IDetailPropertyRow& video_codec_row = encoderCategory.AddProperty( mVideoCodecPropertyHandle );

    TSharedPtr<FString>* selected_video_codec_ptr = mVideoCodecs.FindByPredicate( [encoder_settings]( TSharedPtr<FString> iEntry ) { return *iEntry.Get() == encoder_settings->VideoCodec; } );
    TSharedPtr<FString> selected_video_codec = selected_video_codec_ptr ? *selected_video_codec_ptr : nullptr;

    video_codec_row.CustomWidget()
        .NameContent()
        [
            mVideoCodecPropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .HAlign( HAlign_Fill )
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth( .4f )
            [
                mVideoCodecPropertyHandle->CreatePropertyValueWidget()
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign( VAlign_Center )
            .Padding( 5, 0 )
            [
                SNew( SImage )
                .Image( FEditorStyle::GetBrush( "Icons.Warning" ) )
                .Visibility_Lambda( [this, encoder_settings]() { return IsVideoCodecValid( encoder_settings ) ? EVisibility::Hidden : EVisibility::Visible; } )
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding( 5, 0 )
            [
                SNew( SComboBox<TSharedPtr<FString>> )
                .OptionsSource( &mVideoCodecs )
                .OnGenerateWidget( this, &FEncoderSettingsDetailsCustomization::OnGenerateCodecEntryWidget )
                .OnSelectionChanged( this, &FEncoderSettingsDetailsCustomization::OnVideoCodecChanged )
                .InitiallySelectedItem( selected_video_codec )
                [
                    // combo box button intentionally blank to avoid displaying entry name, as the textbox is already generated by the handle
                    SNew( SSpacer )
                ]
            ]

            + SHorizontalBox::Slot()
            [
                SNew( SSpacer )
            ]
        ];

    //--- Customize the AudioCodec property

    mAudioCodecPropertyHandle = DetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED( UMoviePipelineCommandLineEncoderSettings, AudioCodec ) );

    IDetailPropertyRow& audio_codec_row = encoderCategory.AddProperty( mAudioCodecPropertyHandle );

    TSharedPtr<FString>* selected_audio_codec_ptr = mAudioCodecs.FindByPredicate( [encoder_settings]( TSharedPtr<FString> iEntry ) { return *iEntry.Get() == encoder_settings->AudioCodec; } );
    TSharedPtr<FString> selected_audio_codec = selected_audio_codec_ptr ? *selected_audio_codec_ptr : nullptr;

    audio_codec_row.CustomWidget()
        .NameContent()
        [
            mAudioCodecPropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .HAlign( HAlign_Fill )
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth( .4f )
            [
                mAudioCodecPropertyHandle->CreatePropertyValueWidget()
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign( VAlign_Center )
            .Padding( 5, 0 )
            [
                SNew( SImage )
                .Image( FEditorStyle::GetBrush( "Icons.Warning" ) )
                .Visibility_Lambda( [this, encoder_settings]() { return IsAudioCodecValid( encoder_settings ) ? EVisibility::Hidden : EVisibility::Visible; } )
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding( 5, 0 )
            [
                SNew( SComboBox<TSharedPtr<FString>> )
                .OptionsSource( &mAudioCodecs )
                .OnGenerateWidget( this, &FEncoderSettingsDetailsCustomization::OnGenerateCodecEntryWidget )
                .OnSelectionChanged( this, &FEncoderSettingsDetailsCustomization::OnAudioCodecChanged )
                .InitiallySelectedItem( selected_audio_codec )
                [
                    // combo box button intentionally blank to avoid displaying entry name, as the textbox is already generated by the handle
                    SNew( SSpacer )
                ]
            ]

            + SHorizontalBox::Slot()
            [
                SNew( SSpacer )
            ]
        ];
}

//---
//---
//---

void
SRenderOptions::Construct( const FArguments& iArgs )
{
    mParentWindow = iArgs._ParentWindow;

    FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );

    FARFilter filter;
    filter.bRecursivePaths = true;
    FString epos_root_path = IPluginManager::Get().FindPlugin( "Epos" )->GetMountedAssetPath();
    epos_root_path.RemoveFromEnd( TEXT( "/" ) ); // Remove the last '/' otherwise the asset picker won't get the asset directly inside the root
    filter.PackagePaths.Add( FName( epos_root_path ) );
    filter.PackagePaths.Add( FName( TEXT( "/Game" ) ) );

    // Configure filter for asset picker
    // Same as in Engine\Plugins\MovieScene\MovieRenderPipeline\Source\MovieRenderPipelineEditor\Private\Widgets\SMoviePipelineQueueEditor.cpp # 313
    FAssetPickerConfig AssetPickerConfig;
    {
        AssetPickerConfig.SelectionMode = ESelectionMode::Single;
        AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
        AssetPickerConfig.bFocusSearchBoxWhenOpened = true;
        AssetPickerConfig.bAllowNullSelection = false;
        AssetPickerConfig.bShowBottomToolbar = true;
        AssetPickerConfig.bAutohideSearchBar = false;
        AssetPickerConfig.bAllowDragging = false;
        AssetPickerConfig.bCanShowClasses = false;
        AssetPickerConfig.bShowPathInColumnView = true;
        AssetPickerConfig.bShowTypeInColumnView = false;
        AssetPickerConfig.bSortByPathInColumnView = false;
        AssetPickerConfig.ThumbnailScale = 0.25f;
        //AssetPickerConfig.SaveSettingsName = TEXT( "MoviePipelineConfigAsset" ); // Use the same as in MovieRenderQueue menu ... no ... to not share the same ThumbnailScale value
        AssetPickerConfig.Filter = filter;

        AssetPickerConfig.AssetShowWarningText = LOCTEXT( "NoConfigs_Warning", "No Master Configurations Found" );
        AssetPickerConfig.Filter.ClassNames.Add( UMoviePipelineMasterConfig::StaticClass()->GetFName() );
        AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP( this, &SRenderOptions::OnMasterConfigSelected );
        AssetPickerConfig.OnAssetDoubleClicked = FOnAssetDoubleClicked::CreateSP( this, &SRenderOptions::OnMasterConfigDoubleClicked );
        AssetPickerConfig.OnAssetEnterPressed = FOnAssetEnterPressed::CreateSP( this, &SRenderOptions::OnMasterConfigEnterPressed );
    }

    //---

    FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );

    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.bShowOptions = false;
    DetailsViewArgs.bAllowFavoriteSystem = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    //DetailsViewArgs.ViewIdentifier = "NewStoryboardSettings";

    TSharedPtr<IDetailsView> detailsViewCLIEncoderSettings = PropertyEditor.CreateDetailView( DetailsViewArgs );
    detailsViewCLIEncoderSettings->RegisterInstancedCustomPropertyLayout( UMoviePipelineCommandLineEncoderSettings::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic( &FEncoderSettingsDetailsCustomization::MakeInstance, &mIsExecutablePathValid ) );
    detailsViewCLIEncoderSettings->SetVisibility( MakeAttributeSP( this, &SRenderOptions::EncoderSettingsVisibility ) );

    UMoviePipelineCommandLineEncoderSettings* cli_encoder_settings = GetMutableDefault<UMoviePipelineCommandLineEncoderSettings>();
    detailsViewCLIEncoderSettings->SetObject( cli_encoder_settings );

    //---

    ChildSlot
    [
        SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SBox )
            .WidthOverride( 400.f )
            .HeightOverride( 500.f )
            [
                ContentBrowserModule.Get().CreateAssetPicker( AssetPickerConfig )
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 4, 4, 4, 4 )
        [
            detailsViewCLIEncoderSettings.ToSharedRef()
        ]

        //// Apply/Apply to All/Cancel
        //+SVerticalBox::Slot()
        //.AutoHeight()
        //.HAlign( HAlign_Center )
        //.VAlign( VAlign_Center )
        //[
        //    SNew( SUniformGridPanel )
        //    .MinDesiredSlotHeight( FCoreStyle::Get().GetFloat( "StandardDialog.MinDesiredSlotHeight" ) )
        //    .MinDesiredSlotWidth( FCoreStyle::Get().GetFloat( "StandardDialog.MinDesiredSlotWidth" ) )
        //    .SlotPadding( FCoreStyle::Get().GetMargin( "StandardDialog.SlotPadding" ) )

        //    + SUniformGridPanel::Slot( 0, 0 )
        //    [
        //        SNew(SButton)
        //        .Text(LOCTEXT("Ok", "Ok"))
        //        .ContentPadding( FCoreStyle::Get().GetMargin( "StandardDialog.ContentPadding" ) )
        //        .HAlign( HAlign_Center )
        //        .OnClicked( this, &SRenderOptions::OnAccept )
        //        .IsEnabled( this, &SRenderOptions::CanAccept )
        //    ]

        //    + SUniformGridPanel::Slot( 1, 0 )
        //        [
        //        SNew(SButton)
        //        .Text(LOCTEXT("Cancel", "Cancel"))
        //        .ContentPadding( FCoreStyle::Get().GetMargin( "StandardDialog.ContentPadding" ) )
        //        .HAlign( HAlign_Center )
        //        .OnClicked( this, &SRenderOptions::OnCancel )
        //    ]
        //]
    ];
}

bool
SRenderOptions::IsOk()
{
    return mUserDlgResponse;
}

bool
SRenderOptions::IsCanceled()
{
    return !mUserDlgResponse;
}

UMoviePipelineMasterConfig*
SRenderOptions::GetMasterConfig()
{
    return mMasterConfig;
}

void
SRenderOptions::OnMasterConfigSelected( const FAssetData& iAssetData )
{
    mMasterConfig = CastChecked<UMoviePipelineMasterConfig>( iAssetData.GetAsset() );
}

void
SRenderOptions::OnMasterConfigDoubleClicked( const FAssetData& iAssetData )
{
    if( EncoderSettingsVisibility().IsVisible() && !mIsExecutablePathValid )
        return;

    mUserDlgResponse = true;

    mMasterConfig = CastChecked<UMoviePipelineMasterConfig>( iAssetData.GetAsset() );

    if( mParentWindow.IsValid() )
        mParentWindow.Pin()->RequestDestroyWindow();
}

void
SRenderOptions::OnMasterConfigEnterPressed( const TArray<FAssetData>& iAssetData )
{
    if( !iAssetData.Num() )
        return;

    if( EncoderSettingsVisibility().IsVisible() && !mIsExecutablePathValid )
        return;

    mUserDlgResponse = true;

    mMasterConfig = CastChecked<UMoviePipelineMasterConfig>( iAssetData[0].GetAsset() );

    if( mParentWindow.IsValid() )
        mParentWindow.Pin()->RequestDestroyWindow();
}

EVisibility
SRenderOptions::EncoderSettingsVisibility() const
{
    if( !mMasterConfig )
        return EVisibility::Collapsed;

    UMoviePipelineCommandLineEncoder* setting = mMasterConfig->FindSetting<UMoviePipelineCommandLineEncoder>( true /* bIncludeDisabledSettings */ );
    if( !setting )
        return EVisibility::Collapsed;

    return EVisibility::Visible;
}

//FReply
//SRenderOptions::OnAccept()
//{
//    mUserDlgResponse = true;
//
//    if( mParentWindow.IsValid() )
//        mParentWindow.Pin()->RequestDestroyWindow();
//
//    return FReply::Handled();
//}
//
//bool
//SRenderOptions::CanAccept() const
//{
//    return !!mMasterConfig;
//}
//
//FReply
//SRenderOptions::OnCancel()
//{
//    mUserDlgResponse = false;
//
//    if( mParentWindow.IsValid() )
//        mParentWindow.Pin()->RequestDestroyWindow();
//
//    return FReply::Handled();
//}

//---

#undef LOCTEXT_NAMESPACE
