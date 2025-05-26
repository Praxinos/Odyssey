// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyFlipbookEditorSettings.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "OdysseyFlipbookEditorModule.h"

UOdysseyFlipbookEditorSettings::UOdysseyFlipbookEditorSettings( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
    , OdysseyDefaultEditorEnabled( true )
    /* , PreviousFrame( EKeys::Left )
    , NextFrame( EKeys::Right )
    , PreviousKeyFrame( EKeys::A )
    , NextKeyFrame( EKeys::B )
    , FirstFrame( EKeys::Home )
    , LastFrame( EKeys::End )
    , PlayForward( EKeys::SpaceBar )
    , PlayBackward( EKeys::Enter )
    , Pause( EKeys::SpaceBar )
    , Stop( EKeys::Enter )
    , ToggleLoopingMode( EKeys::L )
    , AddKeyFrameAtEnd( EKeys::Add )
    , AddKeyFrameBeforeCurrentKeyFrame( EKeys::Insert )
    , AddKeyFrameAfterCurrentKeyFrame( EKeys::C )
    , DuplicateCurrentKeyFrame( EKeys::D )
    , DeleteCurrentKeyFrame( EKeys::Delete ) */
{
}

//Static
UOdysseyFlipbookEditorSettings* UOdysseyFlipbookEditorSettings::Get()
{
    return CastChecked<UOdysseyFlipbookEditorSettings>(UOdysseyFlipbookEditorSettings::StaticClass()->GetDefaultObject());
}

void
UOdysseyFlipbookEditorSettings::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    const FName propertyName = PropertyChangedEvent.GetPropertyName();
    if( propertyName == GET_MEMBER_NAME_CHECKED( UOdysseyFlipbookEditorSettings, OdysseyDefaultEditorEnabled ) )
    {
        FOdysseyFlipbookEditorModule* odysseyFlipbookModule = &FModuleManager::LoadModuleChecked<FOdysseyFlipbookEditorModule>("OdysseyFlipbookEditor");
        odysseyFlipbookModule->UnregisterAssetTypeActions();
        odysseyFlipbookModule->RegisterAssetTypeActions();
    }
}
