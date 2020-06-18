// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 
// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTools/ShotSequenceActions.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "Shot/ShotSequence.h"
#include "ShotSequenceEditorToolkit.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"


/* FShotSequenceActions constructors
 *****************************************************************************/

FShotSequenceActions::FShotSequenceActions( const TSharedRef<ISlateStyle>& iStyle )
    : mStyle( iStyle )
{
}

/* IAssetTypeActions interface
 *****************************************************************************/

uint32 FShotSequenceActions::GetCategories()
{
    return EAssetTypeCategories::Animation;
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
    return FColor( 200, 80, 128 );
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
        UShotSequence* ShotSequence = Cast<UShotSequence>( *ObjIt );

        if( ShotSequence != nullptr )
        {
            TSharedRef<FShotSequenceEditorToolkit> Toolkit = MakeShareable( new FShotSequenceEditorToolkit( mStyle ) );
            Toolkit->Initialize( Mode, iEditWithinLevelEditor, ShotSequence );
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

bool
FShotSequenceActions::HasActions( const TArray<UObject*>& iObjects ) const
{
    return false;
}

#undef LOCTEXT_NAMESPACE
