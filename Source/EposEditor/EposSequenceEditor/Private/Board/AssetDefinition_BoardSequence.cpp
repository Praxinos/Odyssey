// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Board/AssetDefinition_BoardSequence.h"

#include "Toolkits/ToolkitManager.h"

#include "Board/BoardHelpers.h"
#include "Board/BoardSequence.h"
#include "EposSequenceEditorToolkit.h"

#define LOCTEXT_NAMESPACE "BoardAssetTypeActions"

FText
UAssetDefinition_BoardSequence::GetAssetDisplayName() const //override
{
    // Done in the (normally) same namespace as all other AssetDefinition
    return NSLOCTEXT( "AssetTypeActions", "AssetTypeActions_BoardSequence", "Board Sequence" );
}

FLinearColor
UAssetDefinition_BoardSequence::GetAssetColor() const //override
{
    return FLinearColor( FColor( 240, 100, 153 ) );
}

TSoftClassPtr<UObject>
UAssetDefinition_BoardSequence::GetAssetClass() const //override
{
    return UBoardSequence::StaticClass();
}

TConstArrayView<FAssetCategoryPath>
UAssetDefinition_BoardSequence::GetAssetCategories() const //override
{
    static const auto Categories = { EAssetCategoryPaths::Cinematics, FAssetCategoryPath( LOCTEXT( "EPOS", "EPOS" ) ) };
    return Categories;
}

FAssetOpenSupport
UAssetDefinition_BoardSequence::GetAssetOpenSupport(const FAssetOpenSupportArgs& OpenSupportArgs) const //override
{
    return FAssetOpenSupport(OpenSupportArgs.OpenMethod,OpenSupportArgs.OpenMethod == EAssetOpenMethod::Edit, EToolkitMode::WorldCentric);
}

EAssetCommandResult
UAssetDefinition_BoardSequence::OpenAssets(const FAssetOpenArgs& OpenArgs) const //override
{
    UWorld* WorldContext = nullptr;
    for (const FWorldContext& Context : GEngine->GetWorldContexts())
    {
        if (Context.WorldType == EWorldType::Editor)
        {
            WorldContext = Context.World();
            break;
        }
    }

    if (!ensure(WorldContext))
    {
        return EAssetCommandResult::Handled;
    }

    for( UBoardSequence* board_sequence : OpenArgs.LoadObjects<UBoardSequence>() )
    {
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
            new_toolkit->Initialize( OpenArgs.GetToolkitMode(), OpenArgs.ToolkitHost, board_sequences );
        }
    }

    return EAssetCommandResult::Handled;
}

#undef LOCTEXT_NAMESPACE
