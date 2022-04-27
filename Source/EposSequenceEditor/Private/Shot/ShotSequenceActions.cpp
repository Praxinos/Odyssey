// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shot/ShotSequenceActions.h"

#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "Toolkits/ToolkitManager.h"

#include "Board/BoardHelpers.h"
#include "Shot/ShotSequence.h"
#include "EposSequenceEditorToolkit.h"
#include "EposSequenceEditorModule.h"
//#include "Render/EposSequenceRenderHelpers.h"

#define LOCTEXT_NAMESPACE "ShotAssetTypeActions"


/* IAssetTypeActions interface
 *****************************************************************************/

uint32
FShotSequenceActions::GetCategories()
{
    return EAssetTypeCategories::Animation | FEposSequenceEditorModule::GetAssetCategory();
}

FText
FShotSequenceActions::GetName() const
{
    return NSLOCTEXT( "AssetTypeActions", "AssetTypeActions_ShotSequence", "Shot Sequence" );
}

UClass*
FShotSequenceActions::GetSupportedClass() const
{
    return UShotSequence::StaticClass();
}

FColor
FShotSequenceActions::GetTypeColor() const
{
    return FColor( 240, 100, 153 );
}

void
FShotSequenceActions::OpenAssetEditor( const TArray<UObject*>& iObjects, TSharedPtr<IToolkitHost> iEditWithinLevelEditor )
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
        UShotSequence* shot_sequence = Cast<UShotSequence>( *ObjIt );
        if( !shot_sequence )
            continue;

        TArray< UEposMovieSceneSequence* > shot_sequences = BoardHelpers::FindParents( shot_sequence );
        if( !shot_sequences.Num() )
            continue;

        TSharedPtr< IToolkit > toolkit = FToolkitManager::Get().FindEditorForAsset( shot_sequences[0] );
        TSharedPtr<FEposSequenceEditorToolkit> existing_toolkit = StaticCastSharedPtr< FEposSequenceEditorToolkit >( toolkit );
        if( existing_toolkit )
        {
            existing_toolkit->GoToFocusedSequence( shot_sequences );
            existing_toolkit->BringToolkitToFront();
        }
        else
        {
            TSharedRef<FEposSequenceEditorToolkit> new_toolkit = MakeShareable( new FEposSequenceEditorToolkit() );
            new_toolkit->Initialize( Mode, iEditWithinLevelEditor, shot_sequences );
        }
    }
}

bool
FShotSequenceActions::ShouldForceWorldCentric()
{
    // @todo sequencer: Hack to force world-centric mode for Sequencer
    return true;
}

bool
FShotSequenceActions::CanLocalize() const
{
    return false;
}

//bool
//FShotSequenceActions::HasActions(const TArray<UObject*>& iObjects) const
//{
//    return true;
//}
//
//void
//FShotSequenceActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& ioMenuBuilder)
//{
//    TArray<UEposMovieSceneSequence*> objects;
//    for( int i = 0; i < InObjects.Num(); i++ )
//    {
//        UEposMovieSceneSequence* sequence = Cast<UEposMovieSceneSequence>( InObjects[i] );
//        if( sequence )
//            objects.Add( sequence );
//    }
//
//    ioMenuBuilder.AddMenuEntry(
//        LOCTEXT( "CB_Extension_ShotSequence_EposActions_RenderSequence", "Render Movie" ),
//        LOCTEXT( "CB_Extension_ShotSequence_EposActions_RenderSequence_ToolTip", "Render a Movie for each selected Shot Asset" ),
//        FSlateIcon( "EditorStyle", "Sequencer.RenderMovie.Small" ),
//        FUIAction( FExecuteAction::CreateStatic( &EposSequenceRenderHelpers::RenderMovie, objects ) )
//    );
//}

#undef LOCTEXT_NAMESPACE
