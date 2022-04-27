// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
#include "SEnumCombo.h"
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
    virtual void CustomizeDetails( IDetailLayoutBuilder& ioDetailBuilder );

private:
    void ExecutablePathChanged();

    void OnVideoCodecChanged( TSharedPtr<FString> iEntry, ESelectInfo::Type iType );
    void OnAudioCodecChanged( TSharedPtr<FString> iEntry, ESelectInfo::Type iType );

    TSharedRef<SWidget> OnGenerateVideoCodecEntryWidget( TSharedPtr<FString> iEntry );
    TSharedRef<SWidget> OnGenerateAudioCodecEntryWidget( TSharedPtr<FString> iEntry );

    int32 GetSelectedCodecPreset() const;
    void OnCodecPresetChanged( int32 iCodecPreset, ESelectInfo::Type iType );

private:
    bool IsExecutablePathValid() const;
    bool IsVideoCodecValid() const;
    bool IsAudioCodecValid() const;

    void UpdateCodecs();

    void CleanArguments();

private:
    bool* mIsExecutablePathValid { nullptr };

    TWeakObjectPtr<UMoviePipelineCommandLineEncoderSettings> mEncoderSettings;

    TSharedPtr<IPropertyHandle> mVideoCodecPropertyHandle;
    TSharedPtr<IPropertyHandle> mAudioCodecPropertyHandle;
    TSharedPtr<IPropertyHandle> mExtensionPropertyHandle;

    TSharedPtr<SComboBox<TSharedPtr<FString>>> mVideoCodecsListWidget;
    TSharedPtr<SComboBox<TSharedPtr<FString>>> mAudioCodecsListWidget;

    TArray<TSharedPtr<FString>> mVideoCodecs;
    TArray<FString>             mVideoCodecTooltips;
    TArray<TSharedPtr<FString>> mAudioCodecs;
    TArray<FString>             mAudioCodecTooltips;
};

//---

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

//---

void
FEncoderSettingsDetailsCustomization::ExecutablePathChanged()
{
    *mIsExecutablePathValid = IsExecutablePathValid();

    UpdateCodecs();
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
FEncoderSettingsDetailsCustomization::OnGenerateVideoCodecEntryWidget( TSharedPtr<FString> iEntry )
{
    check( mVideoCodecs.Num() == mVideoCodecTooltips.Num() );

    // Get the tooltip corresponding to the current entry
    FString codec = *iEntry;
    FString tooltip;
    int32 codec_index = mVideoCodecs.IndexOfByPredicate( [codec]( TSharedPtr<FString> iEntry ) { return *iEntry.Get() == codec; } );
    if( codec_index != INDEX_NONE )
        tooltip = mVideoCodecTooltips[codec_index];

    return
        SNew( SBox )
        .WidthOverride( 150.0f )
        [
            SNew( STextBlock )
            .Text( FText::FromString( *iEntry ) )
            .ToolTipText( FText::FromString( tooltip ) )
            .Font( IDetailLayoutBuilder::GetDetailFont() )
        ];
}

TSharedRef<SWidget>
FEncoderSettingsDetailsCustomization::OnGenerateAudioCodecEntryWidget( TSharedPtr<FString> iEntry )
{
    check( mAudioCodecs.Num() == mAudioCodecTooltips.Num() );

    // Get the tooltip corresponding to the current entry
    FString codec = *iEntry;
    FString tooltip;
    int32 codec_index = mAudioCodecs.IndexOfByPredicate( [codec]( TSharedPtr<FString> iEntry ) { return *iEntry.Get() == codec; } );
    if( codec_index != INDEX_NONE )
        tooltip = mAudioCodecTooltips[codec_index];

    return
        SNew( SBox )
        .WidthOverride( 150.0f )
        [
            SNew( STextBlock )
            .Text( FText::FromString( *iEntry ) )
            .ToolTipText( FText::FromString( tooltip ) )
            .Font( IDetailLayoutBuilder::GetDetailFont() )
        ];
}

int32
FEncoderSettingsDetailsCustomization::GetSelectedCodecPreset() const
{
    return static_cast<int32>( ECodecPresets::kSelect );
}

void
FEncoderSettingsDetailsCustomization::OnCodecPresetChanged( int32 iCodecPreset, ESelectInfo::Type iType )
{
    ECodecPresets codec_preset = static_cast<ECodecPresets>( iCodecPreset );

    FString video_codec;
    FString audio_codec;
    FString extension;

    switch( codec_preset )
    {
        case ECodecPresets::kMP4:
            video_codec = TEXT( "libx264" );
            // This condition is certainly due to lgpl license
            if( !mVideoCodecs.ContainsByPredicate( [video_codec]( TSharedPtr<FString> iEntry ) { return *iEntry.Get() == video_codec; } ) )
                video_codec = TEXT( "libopenh264" );
            audio_codec = TEXT( "flac" );
            extension = TEXT( "mp4" );
            break;
        case ECodecPresets::kMOV:
            video_codec = TEXT( "prores" );
            audio_codec = TEXT( "alac" );
            extension = TEXT( "mov" );
            break;
    }

    mVideoCodecPropertyHandle->SetValue( video_codec );
    mAudioCodecPropertyHandle->SetValue( audio_codec );
    mExtensionPropertyHandle->SetValue( extension );
}

//---

bool
FEncoderSettingsDetailsCustomization::IsExecutablePathValid() const
{
    if( mEncoderSettings->ExecutablePath.Len() == 0 )
        return false;

    int32 error_code;
    FString stdout_string;
    FString error_string;
    bool success = FPlatformProcess::ExecProcess( *mEncoderSettings->ExecutablePath, TEXT( "" ), &error_code, &stdout_string, &error_string );
    if( !success /*|| error_code != 0*/ ) // error_code == 2 when error but error_code == 1 when succeded
        return false;

    return true;
}

void
FEncoderSettingsDetailsCustomization::UpdateCodecs()
{
    mVideoCodecs.Empty();
    mVideoCodecTooltips.Empty();
    mAudioCodecs.Empty();
    mAudioCodecTooltips.Empty();

    // Greatly inspired by MoviePipelineCommandLineEncoderSettings.cpp:PrintAvailableCodecs()

    FString command_line = TEXT( "-encoders" );
    int32 error_code;
    FString stdout_string;
    FString error_string;
    bool success = FPlatformProcess::ExecProcess( *mEncoderSettings->ExecutablePath, *command_line, &error_code, &stdout_string, &error_string );
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

        TStringBuilder<64> description;
        for( int32 Index = 2; Index < line_contents.Num(); Index++ )
        {
            description.Append( *line_contents[Index] );
            description.Append( TEXT( " " ) );
        }

        if( line_contents[0].StartsWith( TEXT( "V" ) ) )
        {
            mVideoCodecs.Add( MakeShareable( new FString( line_contents[1] ) ) );
            mVideoCodecTooltips.Add( description.ToString() );
        }
        else if( line_contents[0].StartsWith( TEXT( "A" ) ) )
        {
            mAudioCodecs.Add( MakeShareable( new FString( line_contents[1] ) ) );
            mAudioCodecTooltips.Add( description.ToString() );
        }
    }

    if( mVideoCodecsListWidget )
        mVideoCodecsListWidget->RefreshOptions();
    if( mAudioCodecsListWidget )
        mAudioCodecsListWidget->RefreshOptions();
}

bool
FEncoderSettingsDetailsCustomization::IsVideoCodecValid() const
{
    return mVideoCodecs.ContainsByPredicate( [this]( TSharedPtr<FString> iEntry ) { return *iEntry.Get() == mEncoderSettings->VideoCodec; } );
}

bool
FEncoderSettingsDetailsCustomization::IsAudioCodecValid() const
{
    return mAudioCodecs.ContainsByPredicate( [this]( TSharedPtr<FString> iEntry ) { return *iEntry.Get() == mEncoderSettings->AudioCodec; } );
}

void
FEncoderSettingsDetailsCustomization::CleanArguments()
{
    // Add quotes around InputFile (if missing)
    if( !mEncoderSettings->CommandLineFormat.Contains( TEXT( "\"{OutputPath}\"" ) ) )
        mEncoderSettings->CommandLineFormat.ReplaceInline( TEXT( "{OutputPath}" ), TEXT( "\"{OutputPath}\"" ) );

    // Add quotes around InputFile (if missing)
    if( !mEncoderSettings->VideoInputStringFormat.Contains( TEXT( "\"{InputFile}\"" ) ) )
        mEncoderSettings->VideoInputStringFormat.ReplaceInline( TEXT( "{InputFile}" ), TEXT( "\"{InputFile}\"" ) );

    // Add quotes around InputFile (if missing)
    if( !mEncoderSettings->AudioInputStringFormat.Contains( TEXT( "\"{InputFile}\"" ) ) )
        mEncoderSettings->AudioInputStringFormat.ReplaceInline( TEXT( "{InputFile}" ), TEXT( "\"{InputFile}\"" ) );

    // Move FrameRate before InputFile (arguments order are important for ffmpeg)
    int32 input_index = mEncoderSettings->VideoInputStringFormat.Contains( TEXT( "-i { InputFile }" ) );
    int32 framerate_index = mEncoderSettings->VideoInputStringFormat.Contains( TEXT( "-r {FrameRate}" ) );
    if( input_index != INDEX_NONE
        && framerate_index != INDEX_NONE
        && input_index < framerate_index )
    {
        mEncoderSettings->VideoInputStringFormat.ReplaceInline( TEXT( "-r {FrameRate}" ), TEXT( "" ) );
        mEncoderSettings->VideoInputStringFormat.TrimStartAndEndInline();
        mEncoderSettings->VideoInputStringFormat.InsertAt( input_index, TEXT( "-r {FrameRate} " ) );
    }
}

void
FEncoderSettingsDetailsCustomization::CustomizeDetails( IDetailLayoutBuilder& ioDetailBuilder )
{
    const TArray< TWeakObjectPtr<UObject> >& objects = ioDetailBuilder.GetSelectedObjects();
    check( objects.Num() == 1 && objects[0].IsValid() );
    mEncoderSettings = Cast<UMoviePipelineCommandLineEncoderSettings>( objects[0].Get() );

    ExecutablePathChanged();

    CleanArguments();

    //--- Customize properties inside "Command Line Encoder" category of UMoviePipelineCommandLineEncoderSettings

    IDetailCategoryBuilder& encoderCategory = ioDetailBuilder.EditCategory( "Command Line Encoder" );

    //--- Customize the ExecutablePath property

    TSharedPtr<IPropertyHandle> executablePathPropertyHandle = ioDetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED( UMoviePipelineCommandLineEncoderSettings, ExecutablePath ) );

    executablePathPropertyHandle->SetOnPropertyValueChanged( FSimpleDelegate::CreateRaw( this, &FEncoderSettingsDetailsCustomization::ExecutablePathChanged ) );

    IDetailPropertyRow& executable_path_row = encoderCategory.AddProperty( executablePathPropertyHandle );

    executable_path_row.CustomWidget()
        .NameContent()
        [
            executablePathPropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .HAlign( HAlign_Fill )
        [
            SNew( SHorizontalBox )
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
                .Image( FAppStyle::Get().GetBrush( "Icons.Warning" ) )
                .Visibility_Lambda( [this]() { return *mIsExecutablePathValid ? EVisibility::Hidden : EVisibility::Visible; } )
            ]

            + SHorizontalBox::Slot()
            [
                SNew( SSpacer )
            ]
        ];

    //--- Remove the CodecHelpText property

    TSharedPtr<IPropertyHandle> helpPropertyHandle = ioDetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED( UMoviePipelineCommandLineEncoderSettings, CodecHelpText ) );
    helpPropertyHandle->MarkHiddenByCustomization();

    //--- Add some presets codecs

    const UEnum* codecPresetsEnum = FindObject<UEnum>( ANY_PACKAGE, TEXT( "ECodecPresets" ) );

    encoderCategory.AddCustomRow( LOCTEXT( "CodecPresets", "Codec Presets" ) )
        .NameContent()
        [
            SNew( STextBlock )
            .Text( LOCTEXT( "CodecPresets", "Codec Presets" ) )
        ]
        .ValueContent()
        [
            SNew( SEnumComboBox, codecPresetsEnum )
            .CurrentValue( this, &FEncoderSettingsDetailsCustomization::GetSelectedCodecPreset )
            .OnEnumSelectionChanged( this, &FEncoderSettingsDetailsCustomization::OnCodecPresetChanged )
        ];

    //--- Customize the VideoCodec property

    mVideoCodecPropertyHandle = ioDetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED( UMoviePipelineCommandLineEncoderSettings, VideoCodec ) );

    IDetailPropertyRow& video_codec_row = encoderCategory.AddProperty( mVideoCodecPropertyHandle );

    TSharedPtr<FString>* selected_video_codec_ptr = mVideoCodecs.FindByPredicate( [this]( TSharedPtr<FString> iEntry ) { return *iEntry.Get() == mEncoderSettings->VideoCodec; } );
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
                .Image( FAppStyle::Get().GetBrush( "Icons.Warning" ) )
                .Visibility_Lambda( [this]() { return IsVideoCodecValid() ? EVisibility::Hidden : EVisibility::Visible; } )
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding( 5, 0 )
            [
                SAssignNew( mVideoCodecsListWidget, SComboBox<TSharedPtr<FString>> )
                .OptionsSource( &mVideoCodecs )
                .OnGenerateWidget( this, &FEncoderSettingsDetailsCustomization::OnGenerateVideoCodecEntryWidget )
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

    mAudioCodecPropertyHandle = ioDetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED( UMoviePipelineCommandLineEncoderSettings, AudioCodec ) );

    IDetailPropertyRow& audio_codec_row = encoderCategory.AddProperty( mAudioCodecPropertyHandle );

    TSharedPtr<FString>* selected_audio_codec_ptr = mAudioCodecs.FindByPredicate( [this]( TSharedPtr<FString> iEntry ) { return *iEntry.Get() == mEncoderSettings->AudioCodec; } );
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
                .Image( FAppStyle::Get().GetBrush( "Icons.Warning" ) )
                .Visibility_Lambda( [this]() { return IsAudioCodecValid() ? EVisibility::Hidden : EVisibility::Visible; } )
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding( 5, 0 )
            [
                SAssignNew( mAudioCodecsListWidget, SComboBox<TSharedPtr<FString>> )
                .OptionsSource( &mAudioCodecs )
                .OnGenerateWidget( this, &FEncoderSettingsDetailsCustomization::OnGenerateAudioCodecEntryWidget )
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

    //---

    mExtensionPropertyHandle = ioDetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED( UMoviePipelineCommandLineEncoderSettings, OutputFileExtension ) );

    encoderCategory.AddProperty( mExtensionPropertyHandle );
}

//---
//---
//---

void
SRenderOptions::Construct( const FArguments& iArgs )
{
    mParentWindow = iArgs._ParentWindow;

    FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );

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
    DetailsViewArgs.NotifyHook = this;

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
        //    .MinDesiredSlotHeight( FAppStyle::Get().GetFloat( "StandardDialog.MinDesiredSlotHeight" ) )
        //    .MinDesiredSlotWidth( FAppStyle::Get().GetFloat( "StandardDialog.MinDesiredSlotWidth" ) )
        //    .SlotPadding( FAppStyle::Get().GetMargin( "StandardDialog.SlotPadding" ) )

        //    + SUniformGridPanel::Slot( 0, 0 )
        //    [
        //        SNew(SButton)
        //        .Text(LOCTEXT("Ok", "Ok"))
        //        .ContentPadding( FAppStyle::Get().GetMargin( "StandardDialog.ContentPadding" ) )
        //        .HAlign( HAlign_Center )
        //        .OnClicked( this, &SRenderOptions::OnAccept )
        //        .IsEnabled( this, &SRenderOptions::CanAccept )
        //    ]

        //    + SUniformGridPanel::Slot( 1, 0 )
        //        [
        //        SNew(SButton)
        //        .Text(LOCTEXT("Cancel", "Cancel"))
        //        .ContentPadding( FAppStyle::Get().GetMargin( "StandardDialog.ContentPadding" ) )
        //        .HAlign( HAlign_Center )
        //        .OnClicked( this, &SRenderOptions::OnCancel )
        //    ]
        //]
    ];
}

// Inspired by ...\Engine\Source\Developer\SettingsEditor\Private\Widgets\SSettingsEditor.cpp#157 -> SSettingsEditor::NotifyPostChange()
void
SRenderOptions::NotifyPostChange( const FPropertyChangedEvent& iPropertyChangedEvent, FEditPropertyChain* iPropertyThatChanged )
{
    if( iPropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive )
        return;

    // Note while there could be multiple objects in the details panel, only one is ever edited at once.
    // There could be zero objects being edited in the FStructOnScope case.
    if( iPropertyChangedEvent.GetNumObjectsBeingEdited() == 0 )
        return;

    UObject* objectBeingEdited = (UObject*)iPropertyChangedEvent.GetObjectBeingEdited( 0 );

    check( objectBeingEdited->GetClass()->HasAnyClassFlags( CLASS_DefaultConfig ) );
    // At this time, there are only string properties in UMoviePipelineCommandLineEncoderSettings
    check( iPropertyThatChanged->GetActiveMemberNode()->GetValue()->IsA( FStrProperty::StaticClass() ) );

    //TODO: find a way to get the section
    //if( Section->NotifySectionOnPropertyModified() )
    {
        objectBeingEdited->UpdateSinglePropertyInConfigFile( iPropertyThatChanged->GetActiveMemberNode()->GetValue(), objectBeingEdited->GetDefaultConfigFilename() );
    }
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
    mMasterConfig = Cast<UMoviePipelineMasterConfig>( iAssetData.GetAsset() );
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
