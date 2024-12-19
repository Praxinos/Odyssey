// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/LighttableTools.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "EditorSupportDelegates.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "MaterialEditingLibrary.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieSceneSequence.h"
#include "MovieSceneSequenceVisitor.h"
#include "ISequencer.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "Sections/MovieSceneSubSection.h"

#include "EposMovieSceneSequence.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "LighttableTools"

//---

//static
void
LighttableTools::Activate( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    Activate( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iPlaneBinding );
}

//static
void
LighttableTools::Deactivate( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    Deactivate( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iPlaneBinding );
}

//static
int8
LighttableTools::GetState( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iPlaneBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return -1;

    return GetState( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iPlaneBinding );
}

//---

//static
void
LighttableTools::Deactivate( ISequencer* iSequencer, FGuid iPlaneBinding )
{
    Deactivate( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iPlaneBinding );
}

//static
void
LighttableTools::Activate( ISequencer* iSequencer, FGuid iPlaneBinding )
{
    Activate( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iPlaneBinding );
}

//static
int8
LighttableTools::GetState( ISequencer* iSequencer, FGuid iPlaneBinding )
{
    return GetState( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iPlaneBinding );
}

//---

//static
void
LighttableTools::Deactivate( ISequencer* iSequencer )
{
    if( !iSequencer )
        return;

    struct FBindingVisitor
        : UE::MovieScene::ISequenceVisitor
    {
        virtual void VisitObjectBinding( const FMovieSceneBinding& iBinding, const UE::MovieScene::FSubSequenceSpace& iLocalSpace ) override
        {
            const FMovieSceneSequenceHierarchy* Hierarchy = mSequencer->GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( mSequencer->GetEvaluationTemplate().GetCompiledDataID() );
            UMovieSceneSequence* subsequence = Hierarchy->FindSubSequence( iLocalSpace.SequenceID );

            if( iLocalSpace.SequenceID == MovieSceneSequenceID::Root )
                subsequence = mSequencer->GetRootMovieSceneSequence();

            LighttableTools::Deactivate( *mSequencer, subsequence, iLocalSpace.SequenceID, iBinding.GetObjectGuid() );
        }

        ISequencer* mSequencer;
    };

    //---

    UE::MovieScene::FSequenceVisitParams params;
    params.bVisitRootTracks = true;
    params.bVisitSubSequences = true;
    params.bVisitObjectBindings = true;

    FBindingVisitor visitor;
    visitor.mSequencer = iSequencer;

    // Visit all notes
    VisitSequence( iSequencer->GetRootMovieSceneSequence(), params, visitor );
}

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
            UMaterialInstanceConstant* current_material = Cast<UMaterialInstanceConstant>( values[i].Get() );
            if( !current_material )
                continue;

            UTexture* previous_material_texture = nullptr;
            if( i > 0 )
            {
                UMaterialInstanceConstant* previous_material = Cast<UMaterialInstanceConstant>( values[i - 1].Get() );
                if( previous_material )
                    previous_material->GetTextureParameterValue( TEXT( "DrawingTexture" ), previous_material_texture );
            }

            UTexture* current_material_previous_texture = nullptr;
            current_material->GetTextureParameterValue( TEXT( "PreviousDrawingTexture" ), current_material_previous_texture );
            UTexture* current_material_next_texture = nullptr;
            current_material->GetTextureParameterValue( TEXT( "NextDrawingTexture" ), current_material_next_texture );

            UTexture* next_material_texture = nullptr;
            if( i < values.Num() - 1 )
            {
                UMaterialInstanceConstant* next_material = Cast<UMaterialInstanceConstant>( values[i + 1].Get() );
                if( next_material )
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

    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequence );
    if( !epos_sequence )
    {
        checkf( false, TEXT( "iSequence is certainly a LevelSequence, manage it" ) );
        return;
    }

    FString texture_transparent_path;
    FString texture_transparent_name;
    FString texture_transparent_pathname = NamingConvention::GetMasterTexturePathName( iSequencer, *epos_sequence, iSequenceID, texture_transparent_path, texture_transparent_name );

    bool modified = false;

    for( auto section : result.mSections )
    {
        TArrayView<FMovieSceneObjectPathChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
        check( channels.Num() == 1 );
        FMovieSceneObjectPathChannel* channel = channels[0];

        TArrayView<FMovieSceneObjectPathChannelKeyValue> values = channel->GetData().GetValues();
        for( int i = 0; i < values.Num(); i++ )
        {
            UMaterialInstanceConstant* current_material = Cast<UMaterialInstanceConstant>( values[i].Get() );
            if( !current_material )
                continue;

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
                UTexture2D* texture_transparent = MasterAssetTools::GetMasterTexture2D( iSequencer, iSequence, iSequenceID ); // Slow operation

                current_material->SetTextureParameterValueEditorOnly( TEXT( "PreviousDrawingTexture" ), texture_transparent );
                modified |= true;
            }
            if( current_material_next_texture
                && current_material_next_texture->GetPackage()->GetPathName() != texture_transparent_pathname )
            {
                UTexture2D* texture_transparent = MasterAssetTools::GetMasterTexture2D( iSequencer, iSequence, iSequenceID ); // Slow operation

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
int8
LighttableTools::GetState( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding )
{
    ShotSequenceHelpers::FFindOrCreateMaterialDrawingResult result = ShotSequenceHelpers::FindMaterialDrawingTrackAndSections( iSequencer, iSequence, iSequenceID, iPlaneBinding );

    int32 total_drawing = 0;
    int32 total_drawing_on = 0;
    int32 total_drawing_off = 0;

    for( auto section : result.mSections )
    {
        TArrayView<FMovieSceneObjectPathChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();
        check( channels.Num() == 1 );
        FMovieSceneObjectPathChannel* channel = channels[0];

        TArrayView<FMovieSceneObjectPathChannelKeyValue> values = channel->GetData().GetValues();
        for( int i = 0; i < values.Num(); i++ )
        {
            UMaterialInstanceConstant* current_material = Cast<UMaterialInstanceConstant>( values[i].Get() );
            if( !current_material )
                continue;

            total_drawing++;

            float use_lighttable = 0.f;
            current_material->GetScalarParameterValue( TEXT( "UseLighttable" ), use_lighttable );

            if( use_lighttable >= .5f )
                total_drawing_on++;
            else
                total_drawing_off++;
        }
    }

    if( !total_drawing )
        return -1;

    if( total_drawing == total_drawing_on )
        return 1;
    else if( total_drawing == total_drawing_off )
        return 0;
    else
        return -1;
}

//TODO: certainly remove IsOn/IsOff and replace them by GetState in the code

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
        for( int i = 0; i < values.Num(); i++ )
        {
            UMaterialInstanceConstant* current_material = Cast<UMaterialInstanceConstant>( values[i].Get() );
            if( !current_material )
                continue;

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
