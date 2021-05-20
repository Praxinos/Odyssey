// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "LighttableTools.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "MaterialEditingLibrary.h"
#include "Materials/MaterialInstanceConstant.h"
#include "ISequencer.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"

#include "Board/BoardSequenceHelpers.h"
#include "MaterialPlaneAssetTools.h"
#include "Shot/ShotSequenceHelpers.h"

#define LOCTEXT_NAMESPACE "LighttableTools"

//---

//static
void
LighttableTools::Activate( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding )
{
    TArray<UMovieScenePrimitiveMaterialSection*> plane_material_sections = ShotSequenceHelpers::GetPlaneMaterialSections( iSequencer, iSequence, iSequenceID, iPlaneBinding );

    for( auto section : plane_material_sections )
    {
        UMovieScenePrimitiveMaterialSection* section_material = Cast<UMovieScenePrimitiveMaterialSection>( section );
        if( !section_material )
            continue;

        TArrayView<FMovieSceneObjectPathChannel*> channels = section_material->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
        check( channels.Num() == 1 );
        FMovieSceneObjectPathChannel* channel = channels[0];

        TArrayView<FMovieSceneObjectPathChannelKeyValue> values = channel->GetData().GetValues();
        for( int i = 0; i < values.Num(); i++ )
        {
            UMaterialInstanceConstant* previous_material = nullptr;
            UTexture* previous_texture = nullptr;
            if( i > 0 )
            {
                previous_material = Cast<UMaterialInstanceConstant>( values[i - 1].Get() );
                previous_material->GetTextureParameterValue( TEXT( "DrawingTexture" ), previous_texture );
            }

            UMaterialInstanceConstant* current_material = Cast<UMaterialInstanceConstant>( values[i].Get() );

            UMaterialInstanceConstant* next_material = nullptr;
            UTexture* next_texture = nullptr;
            if( i < values.Num() - 1 )
            {
                next_material = Cast<UMaterialInstanceConstant>( values[i + 1].Get() );
                next_material->GetTextureParameterValue( TEXT( "DrawingTexture" ), next_texture );
            }

            if( previous_texture )
            {
                current_material->SetTextureParameterValueEditorOnly( TEXT( "PreviousDrawingTexture" ), previous_texture );
            }
            if( next_texture )
            {
                current_material->SetTextureParameterValueEditorOnly( TEXT( "NextDrawingTexture" ), next_texture );
            }

            UMaterialEditingLibrary::UpdateMaterialInstance( current_material );
        }
    }
}

//static
void
LighttableTools::Deactivate( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding )
{
    TArray<UMovieScenePrimitiveMaterialSection*> plane_material_sections = ShotSequenceHelpers::GetPlaneMaterialSections( iSequencer, iSequence, iSequenceID, iPlaneBinding );

    for( auto section : plane_material_sections )
    {
        UMovieScenePrimitiveMaterialSection* section_material = Cast<UMovieScenePrimitiveMaterialSection>( section );
        if( !section_material )
            continue;

        TArrayView<FMovieSceneObjectPathChannel*> channels = section_material->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
        check( channels.Num() == 1 );
        FMovieSceneObjectPathChannel* channel = channels[0];

        TArrayView<FMovieSceneObjectPathChannelKeyValue> values = channel->GetData().GetValues();
        if( values.Num() <= 1 )
            continue;

        for( int i = 0; i < values.Num(); i++ )
        {
            UMaterialInstanceConstant* current_material = Cast<UMaterialInstanceConstant>( values[i].Get() );

            UTexture2D* texture_transparent = MaterialPlaneAssetTools::GetMasterTexture2D( iSequencer.GetRootMovieSceneSequence() );

            current_material->SetTextureParameterValueEditorOnly( TEXT( "PreviousDrawingTexture" ), texture_transparent );
            current_material->SetTextureParameterValueEditorOnly( TEXT( "NextDrawingTexture" ), texture_transparent );

            UMaterialEditingLibrary::UpdateMaterialInstance( current_material );
        }
    }
}

#undef LOCTEXT_NAMESPACE
