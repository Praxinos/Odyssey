// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Board/BoardSequenceActions.h"

//#include "ContentBrowserModule.h"
//#include "IContentBrowserSingleton.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
//#include "MoviePipelineMasterConfig.h"
#include "Toolkits/ToolkitManager.h"

#include "Board/BoardHelpers.h"
#include "Board/BoardSequence.h"
#include "EposSequenceEditorToolkit.h"
#include "EposSequenceEditorModule.h"
//#include "Render/EposSequenceRenderHelpers.h"

#define LOCTEXT_NAMESPACE "BoardAssetTypeActions"


/* IAssetTypeActions interface
 *****************************************************************************/

uint32
FBoardSequenceActions::GetCategories()
{
    return EAssetTypeCategories::Animation | FEposSequenceEditorModule::GetAssetCategory();
}

FText
FBoardSequenceActions::GetName() const
{
    return NSLOCTEXT( "AssetTypeActions", "AssetTypeActions_BoardSequence", "Board Sequence" );
}

UClass*
FBoardSequenceActions::GetSupportedClass() const
{
    return UBoardSequence::StaticClass();
}

FColor
FBoardSequenceActions::GetTypeColor() const
{
    return FColor( 240, 100, 153 );
}

void
FBoardSequenceActions::OpenAssetEditor( const TArray<UObject*>& iObjects, TSharedPtr<IToolkitHost> iEditWithinLevelEditor )
{
    UWorld* WorldContext = nullptr;
    for( const FWorldContext& Context : GEngine->GetWorldContexts() )
    {
        if( Context.WorldType == EWorldType::Editor )
        {
            WorldContext = Context.World();
            break;
        }
    }

    if( !ensure( WorldContext ) )
    {
        return;
    }

    EToolkitMode::Type Mode = iEditWithinLevelEditor.IsValid()
        ? EToolkitMode::WorldCentric
        : EToolkitMode::Standalone;

    for( auto ObjIt = iObjects.CreateConstIterator(); ObjIt; ++ObjIt )
    {
        UBoardSequence* board_sequence = Cast<UBoardSequence>( *ObjIt );
        if( !board_sequence )
            continue;

        TArray< UEposMovieSceneSequence* > board_sequences = BoardHelpers::FindParents( board_sequence );
        if( !board_sequences.Num() )
            continue;

        TSharedPtr< IToolkit > toolkit = FToolkitManager::Get().FindEditorForAsset( board_sequences[0] );
        TSharedPtr<FEposSequenceEditorToolkit> existing_toolkit = StaticCastSharedPtr< FEposSequenceEditorToolkit >( toolkit );
        if( existing_toolkit )
        {
            existing_toolkit->GoToFocusedSequence( board_sequences ); // board_sequences >= 2, as when double-clicking on the 'root' asset (which is already opened), this method is not called at all (this is managed/checked above, inside UAssetEditorSubsystem::OpenEditorForAsset(...))
            existing_toolkit->BringToolkitToFront();
        }
        else
        {
            TSharedRef<FEposSequenceEditorToolkit> new_toolkit = MakeShareable( new FEposSequenceEditorToolkit() );
            new_toolkit->Initialize( Mode, iEditWithinLevelEditor, board_sequences );
        }
    }
}

bool
FBoardSequenceActions::ShouldForceWorldCentric()
{
    // @todo sequencer: Hack to force world-centric mode for Sequencer
    return true;
}

bool
FBoardSequenceActions::CanLocalize() const
{
    return false;
}

//bool
//FBoardSequenceActions::HasActions( const TArray<UObject*>& iObjects ) const
//{
//    return true;
//}
//
//namespace
//{
//static
//void
//OnMasterConfigSelected( const FAssetData& iAssetData, TArray<UEposMovieSceneSequence*> iSequences )
//{
//    EposSequenceRenderHelpers::RenderMovie( iSequences, CastChecked<UMoviePipelineMasterConfig>( iAssetData.GetAsset() ) );
//}
//
//static
//void
//MakeRenderMovieSubMenu( FMenuBuilder& ioMenuBuilder, TArray<UEposMovieSceneSequence*> iSequences )
//{
//    FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>( TEXT( "ContentBrowser" ) );
//
//    // Configure filter for asset picker
//    // Same as in Engine\Plugins\MovieScene\MovieRenderPipeline\Source\MovieRenderPipelineEditor\Private\Widgets\SMoviePipelineQueueEditor.cpp # 313
//    FAssetPickerConfig AssetPickerConfig;
//    {
//        AssetPickerConfig.SelectionMode = ESelectionMode::Single;
//        AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
//        AssetPickerConfig.bFocusSearchBoxWhenOpened = true;
//        AssetPickerConfig.bAllowNullSelection = false;
//        AssetPickerConfig.bShowBottomToolbar = true;
//        AssetPickerConfig.bAutohideSearchBar = false;
//        AssetPickerConfig.bAllowDragging = false;
//        AssetPickerConfig.bCanShowClasses = false;
//        AssetPickerConfig.bShowPathInColumnView = true;
//        AssetPickerConfig.bShowTypeInColumnView = false;
//        AssetPickerConfig.bSortByPathInColumnView = false;
//        AssetPickerConfig.ThumbnailScale = 0.1f;
//        AssetPickerConfig.SaveSettingsName = TEXT( "MoviePipelineConfigAsset" ); // Use the same as in MovieRenderQueue menu
//
//        AssetPickerConfig.AssetShowWarningText = LOCTEXT( "NoConfigs_Warning", "No Master Configurations Found" );
//        AssetPickerConfig.Filter.ClassNames.Add( UMoviePipelineMasterConfig::StaticClass()->GetFName() );
//        AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateStatic( &OnMasterConfigSelected, iSequences );
//    }
//
//    ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "section-render.browse-label", "Browse" ) );
//    {
//        ioMenuBuilder.AddWidget( SNew( SBox )
//                                 .WidthOverride( 300.f )
//                                 .HeightOverride( 300.f )
//                                 [
//                                     ContentBrowserModule.Get().CreateAssetPicker( AssetPickerConfig )
//                                 ],
//                                 FText::GetEmpty(),
//                                 true,
//                                 false );
//    }
//    ioMenuBuilder.EndSection();
//}
//}
//
//void
//FBoardSequenceActions::GetActions( const TArray<UObject*>& InObjects, FMenuBuilder& ioMenuBuilder )
//{
//    TArray<UEposMovieSceneSequence*> objects;
//    for( int i = 0; i < InObjects.Num(); i++ )
//    {
//        UEposMovieSceneSequence* sequence = Cast<UEposMovieSceneSequence>( InObjects[i] );
//        if( sequence )
//            objects.Add( sequence );
//    }
//
//    ioMenuBuilder.AddSubMenu(
//        LOCTEXT( "CB_Extension_BoardSequence_EposActions_RenderSequence", "Render Movie..." ),
//        LOCTEXT( "CB_Extension_BoardSequence_EposActions_RenderSequence_ToolTip", "Render a Movie for each selected Board Asset" ),
//        FNewMenuDelegate::CreateStatic( &MakeRenderMovieSubMenu, objects ) );
//}

#undef LOCTEXT_NAMESPACE
