// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Shot/AssetDefinition_ShotSequence.h"

#include "Toolkits/ToolkitManager.h"

#include "Board/BoardHelpers.h"
#include "Shot/ShotSequence.h"
#include "EposSequenceEditorToolkit.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText
UAssetDefinition_ShotSequence::GetAssetDisplayName() const //override
{
    // Done in the (normally) same namespace as all other AssetDefinition
    return NSLOCTEXT( "AssetTypeActions", "AssetTypeActions_ShotSequence", "Shot Sequence" );
}

FLinearColor
UAssetDefinition_ShotSequence::GetAssetColor() const //override
{
    return FLinearColor( FColor( 240, 100, 153 ) );
}

TSoftClassPtr<UObject>
UAssetDefinition_ShotSequence::GetAssetClass() const //override
{
    return UShotSequence::StaticClass();
}

TConstArrayView<FAssetCategoryPath>
UAssetDefinition_ShotSequence::GetAssetCategories() const //override
{
    static const auto Categories = { EAssetCategoryPaths::Cinematics, FAssetCategoryPath( LOCTEXT( "EPOS", "EPOS" ) ) };
    return Categories;
}

FAssetOpenSupport
UAssetDefinition_ShotSequence::GetAssetOpenSupport(const FAssetOpenSupportArgs& OpenSupportArgs) const //override
{
    return FAssetOpenSupport(OpenSupportArgs.OpenMethod,OpenSupportArgs.OpenMethod == EAssetOpenMethod::Edit, EToolkitMode::WorldCentric);
}

EAssetCommandResult
UAssetDefinition_ShotSequence::OpenAssets(const FAssetOpenArgs& OpenArgs) const //override
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

    for( UShotSequence* shot_sequence : OpenArgs.LoadObjects<UShotSequence>() )
    {
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
            new_toolkit->Initialize( OpenArgs.GetToolkitMode(), OpenArgs.ToolkitHost, shot_sequences );
        }
    }

    return EAssetCommandResult::Handled;
}

#undef LOCTEXT_NAMESPACE
