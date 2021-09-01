// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Board/BoardSequenceActions.h"

#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "Toolkits/ToolkitManager.h"

#include "Board/BoardHelpers.h"
#include "Board/BoardSequence.h"
#include "EposSequenceEditorToolkit.h"
#include "EposSequenceEditorModule.h"
#include "EposSequenceRenderHelpers.h"

#define LOCTEXT_NAMESPACE "BoardAssetTypeActions"


/* FBoardSequenceActions constructors
 *****************************************************************************/

FBoardSequenceActions::FBoardSequenceActions( const TSharedRef<ISlateStyle>& iStyle )
    : mStyle( iStyle )
{
}

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
            TSharedRef<FEposSequenceEditorToolkit> new_toolkit = MakeShareable( new FEposSequenceEditorToolkit( mStyle ) );
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

bool
FBoardSequenceActions::HasActions( const TArray<UObject*>& iObjects ) const
{
    return true;
}

void
FBoardSequenceActions::GetActions( const TArray<UObject*>& InObjects, FMenuBuilder& ioMenuBuilder )
{
    TArray<UEposMovieSceneSequence*> objects;
    for( int i = 0; i < InObjects.Num(); i++ )
    {
        UEposMovieSceneSequence* sequence = Cast<UEposMovieSceneSequence>( InObjects[i] );
        if( sequence )
            objects.Add( sequence );
    }

    ioMenuBuilder.AddMenuEntry(
        LOCTEXT( "CB_Extension_BoardSequence_EposActions_RenderSequence", "Render Movie" ),
        LOCTEXT( "CB_Extension_BoardSequence_EposActions_RenderSequence_ToolTip", "Render a Movie for each selected Board Asset" ),
        FSlateIcon( "EditorStyle", "Sequencer.RenderMovie.Small" ),
        FUIAction( FExecuteAction::CreateStatic( &EposSequenceRenderHelpers::RenderMovie, objects ) )
    );
}

#undef LOCTEXT_NAMESPACE
