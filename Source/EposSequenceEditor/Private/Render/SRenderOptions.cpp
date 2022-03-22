// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Render/SRenderOptions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Interfaces/IPluginManager.h"
#include "MoviePipelineMasterConfig.h"
#include "MovieSceneSequence.h"
//#include "Widgets/Layout/SUniformGridPanel.h"

#define LOCTEXT_NAMESPACE "SRenderOptions"

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
        //AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP( this, &SRenderOptions::OnMasterConfigSelected );
        AssetPickerConfig.OnAssetDoubleClicked = FOnAssetDoubleClicked::CreateSP( this, &SRenderOptions::OnMasterConfigDoubleClicked );
        AssetPickerConfig.OnAssetEnterPressed = FOnAssetEnterPressed::CreateSP( this, &SRenderOptions::OnMasterConfigEnterPressed );
    }

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

//void
//SRenderOptions::OnMasterConfigSelected( const FAssetData& iAssetData )
//{
//    mUserDlgResponse = true;
//
//    mMasterConfig = CastChecked<UMoviePipelineMasterConfig>( iAssetData.GetAsset() );
//}

void
SRenderOptions::OnMasterConfigDoubleClicked( const FAssetData& iAssetData )
{
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

    mUserDlgResponse = true;

    mMasterConfig = CastChecked<UMoviePipelineMasterConfig>( iAssetData[0].GetAsset() );

    if( mParentWindow.IsValid() )
        mParentWindow.Pin()->RequestDestroyWindow();
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
