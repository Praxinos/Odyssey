// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/LighttableTools.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "EditorSupportDelegates.h"
#include "MaterialEditingLibrary.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieSceneSequence.h"
#include "ISequencer.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"

#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "LighttableTools"

//---

//static
void
LighttableTools::Activate( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding )
{
    ShotSequenceHelpers::FFindOrCreateMaterialDrawingResult result = ShotSequenceHelpers::FindMaterialDrawingTrackAndSections( iSequencer, iSequence, iSequenceID, iPlaneBinding );

    bool modified = false;

    for( auto section : result.mSections )
    {
        TArrayView<FMovieSceneObjectPathChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
        check( channels.Num() == 1 );
        FMovieSceneObjectPathChannel* channel = channels[0];

        TArrayView<FMovieSceneObjectPathChannelKeyValue> values = channel->GetData().GetValues();
        for( int i = 0; i < values.Num(); i++ )
        {
            UTexture* previous_material_texture = nullptr;
            if( i > 0 )
            {
                UMaterialInstanceConstant* previous_material = Cast<UMaterialInstanceConstant>( values[i - 1].Get() );
                previous_material->GetTextureParameterValue( TEXT( "DrawingTexture" ), previous_material_texture );
            }

            UMaterialInstanceConstant* current_material = Cast<UMaterialInstanceConstant>( values[i].Get() );
            UTexture* current_material_previous_texture = nullptr;
            current_material->GetTextureParameterValue( TEXT( "PreviousDrawingTexture" ), current_material_previous_texture );
            UTexture* current_material_next_texture = nullptr;
            current_material->GetTextureParameterValue( TEXT( "NextDrawingTexture" ), current_material_next_texture );

            UTexture* next_material_texture = nullptr;
            if( i < values.Num() - 1 )
            {
                UMaterialInstanceConstant* next_material = Cast<UMaterialInstanceConstant>( values[i + 1].Get() );
                next_material->GetTextureParameterValue( TEXT( "DrawingTexture" ), next_material_texture );
            }

            float use_lighttable = 0.f;
            current_material->GetScalarParameterValue( TEXT( "UseLighttable" ), use_lighttable );

            //---

            if( use_lighttable < .5f )
                current_material->SetScalarParameterValueEditorOnly( TEXT( "UseLighttable" ), 1.f );

            if( previous_material_texture
                && current_material_previous_texture
                && previous_material_texture->GetPathName() != current_material_previous_texture->GetPathName() )
            {
                current_material->SetTextureParameterValueEditorOnly( TEXT( "PreviousDrawingTexture" ), previous_material_texture );
                modified |= true;
            }
            if( next_material_texture
                && current_material_next_texture
                && next_material_texture->GetPathName() != current_material_next_texture->GetPathName() )
            {
                current_material->SetTextureParameterValueEditorOnly( TEXT( "NextDrawingTexture" ), next_material_texture );
                modified |= true;
            }

            if( modified )
            {
                //UMaterialEditingLibrary::UpdateMaterialInstance( current_material );
                current_material->MarkPackageDirty();
                current_material->PreEditChange( nullptr );
                current_material->PostEditChange();

                // This is commented because it is slow, and certainly not needed in our case, because no static switch inside and only values are changed above
                //current_material->UpdateStaticPermutation();
                //current_material->UpdateParameterNames(); // protected
            }
        }
    }

    if( modified )
    {
        // This is used inside UMaterialEditingLibrary::UpdateMaterialInstance();
        // but as it is not material related, do it outside the loop
        FEditorDelegates::RefreshEditor.Broadcast();
        FEditorSupportDelegates::RedrawAllViewports.Broadcast();
    }
}

//static
void
LighttableTools::Deactivate( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding )
{
    ShotSequenceHelpers::FFindOrCreateMaterialDrawingResult result = ShotSequenceHelpers::FindMaterialDrawingTrackAndSections( iSequencer, iSequence, iSequenceID, iPlaneBinding );

    FString texture_transparent_path;
    FString texture_transparent_name;
    FString texture_transparent_pathname = NamingConvention::GetMasterTexturePathName( iSequencer, iSequencer.GetRootMovieSceneSequence(), texture_transparent_path, texture_transparent_name );

    bool modified = false;

    for( auto section : result.mSections )
    {
        TArrayView<FMovieSceneObjectPathChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
        check( channels.Num() == 1 );
        FMovieSceneObjectPathChannel* channel = channels[0];

        TArrayView<FMovieSceneObjectPathChannelKeyValue> values = channel->GetData().GetValues();
        if( values.Num() <= 1 )
            continue;

        for( int i = 0; i < values.Num(); i++ )
        {
            UMaterialInstanceConstant* current_material = Cast<UMaterialInstanceConstant>( values[i].Get() );

            UTexture* current_material_previous_texture = nullptr;
            current_material->GetTextureParameterValue( TEXT( "PreviousDrawingTexture" ), current_material_previous_texture );
            UTexture* current_material_next_texture = nullptr;
            current_material->GetTextureParameterValue( TEXT( "NextDrawingTexture" ), current_material_next_texture );

            float use_lighttable = 0.f;
            current_material->GetScalarParameterValue( TEXT( "UseLighttable" ), use_lighttable );

            //---

            if( use_lighttable >= .5f )
                current_material->SetScalarParameterValueEditorOnly( TEXT( "UseLighttable" ), 0.f );

            if( current_material_previous_texture
                && current_material_previous_texture->GetPackage()->GetPathName() != texture_transparent_pathname )
            {
                UTexture2D* texture_transparent = MasterAssetTools::GetMasterTexture2D( iSequencer, iSequencer.GetRootMovieSceneSequence() ); // Slow operation

                current_material->SetTextureParameterValueEditorOnly( TEXT( "PreviousDrawingTexture" ), texture_transparent );
                modified |= true;
            }
            if( current_material_next_texture
                && current_material_next_texture->GetPackage()->GetPathName() != texture_transparent_pathname )
            {
                UTexture2D* texture_transparent = MasterAssetTools::GetMasterTexture2D( iSequencer, iSequencer.GetRootMovieSceneSequence() ); // Slow operation

                current_material->SetTextureParameterValueEditorOnly( TEXT( "NextDrawingTexture" ), texture_transparent );
                modified |= true;
            }

            if( modified )
            {
                //UMaterialEditingLibrary::UpdateMaterialInstance( current_material );
                current_material->MarkPackageDirty();
                current_material->PreEditChange( nullptr );
                current_material->PostEditChange();

                // This is commented because it is slow, and certainly not needed in our case, because no static switch inside and only values are changed above
                //current_material->UpdateStaticPermutation();
                //current_material->UpdateParameterNames(); // protected
            }
        }
    }

    if( modified )
    {
        // This is used inside UMaterialEditingLibrary::UpdateMaterialInstance();
        // but as it is not material related, do it outside the loop
        FEditorDelegates::RefreshEditor.Broadcast();
        FEditorSupportDelegates::RedrawAllViewports.Broadcast();
    }
}

//static
void
LighttableTools::Update( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding )
{
    if( IsOn( iSequencer, iSequence, iSequenceID, iPlaneBinding ) )
        Activate( iSequencer, iSequence, iSequenceID, iPlaneBinding );
    else
        Deactivate( iSequencer, iSequence, iSequenceID, iPlaneBinding );
}

//---

//static
bool
LighttableTools::IsOn( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding )
{
    ShotSequenceHelpers::FFindOrCreateMaterialDrawingResult result = ShotSequenceHelpers::FindMaterialDrawingTrackAndSections( iSequencer, iSequence, iSequenceID, iPlaneBinding );

    for( auto section : result.mSections )
    {
        TArrayView<FMovieSceneObjectPathChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
        check( channels.Num() == 1 );
        FMovieSceneObjectPathChannel* channel = channels[0];

        TArrayView<FMovieSceneObjectPathChannelKeyValue> values = channel->GetData().GetValues();
        if( values.Num() <= 1 )
            continue;

        for( int i = 0; i < values.Num(); i++ )
        {
            UMaterialInstanceConstant* current_material = Cast<UMaterialInstanceConstant>( values[i].Get() );

            float use_lighttable = 0.f;
            current_material->GetScalarParameterValue( TEXT( "UseLighttable" ), use_lighttable );

            if( use_lighttable >= .5f )
                return true;
        }
    }

    return false;
}

//static
bool
LighttableTools::IsOff( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding )
{
    return !IsOn( iSequencer, iSequence, iSequenceID, iPlaneBinding );
}

#undef LOCTEXT_NAMESPACE
