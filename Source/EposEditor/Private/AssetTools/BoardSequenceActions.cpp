// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 
// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTools/BoardSequenceActions.h"

#include "EngineGlobals.h"
#include "Engine/Engine.h"

#include "Board/BoardSequence.h"
#include "BoardSequenceEditorToolkit.h"
#include "EposEditorModule.h"

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
    return EAssetTypeCategories::Animation | FEposEditorModule::GetAssetCategory();
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
    return FColor( 200, 80, 128 );
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
        UBoardSequence* BoardSequence = Cast<UBoardSequence>( *ObjIt );

        if( BoardSequence != nullptr )
        {
            TSharedRef<FBoardSequenceEditorToolkit> Toolkit = MakeShareable( new FBoardSequenceEditorToolkit( mStyle ) );
            Toolkit->Initialize( Mode, iEditWithinLevelEditor, BoardSequence );
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
    return false;
}

#undef LOCTEXT_NAMESPACE
