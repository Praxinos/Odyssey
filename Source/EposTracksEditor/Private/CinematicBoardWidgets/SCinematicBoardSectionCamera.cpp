// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionCamera.h"

#include "Brushes/SlateColorBrush.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "CineCameraActor.h"
#include "KeyDrawParams.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "SequencerSettings.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/MetaChannelProxy.h"
#include "EposSequenceHelpers.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionCamera"

//---

void
SCinematicBoardSectionCamera::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    SMetaKeysArea::Construct( SMetaKeysArea::FArguments(), iBoardSection );

    ChildSlot
    [
        SNew( SBox )
        //[
        //    SNew( STextBlock )
        //    .Text( GetBindingName() ) )
        //]
    ];
}

//---

TSharedPtr<FMetaChannel>
SCinematicBoardSectionCamera::GetMetaChannel() //override
{
    return mBoardSection.Pin()->GetCameraTransformMetaChannel();
}

TSharedPtr<const FMetaChannel>
SCinematicBoardSectionCamera::GetMetaChannel() const //override
{
    return mBoardSection.Pin()->GetCameraTransformMetaChannel();
}

void
SCinematicBoardSectionCamera::RebuildMetaChannel() //override
{
    mBoardSection.Pin()->ReBuildCameraTransformMetaChannel();
}

//---

bool
SCinematicBoardSectionCamera::BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys ) //override
{
    auto DeleteKey = [=]( TSharedPtr<FMetaChannel> iKeys )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        TArray<TWeakObjectPtr<UMovieSceneSection>> sections;
        TArray<FMovieSceneChannelHandle> channelHandles;
        TArray<FKeyHandle> keyHandles;

        for( auto pair : iKeys->GetMetaKeys() )
        {
            for( const auto& subkey : pair.Value.mSubKeys )
            {
                sections.Add( subkey.mSection );
                channelHandles.Add( subkey.mChannelHandle );
                keyHandles.Add( subkey.mKeyHandle );
            }
        }

        BoardSequenceTools::DeleteCameraKey( sequencer, *subsection_object, sections, channelHandles, keyHandles );
    };

    auto CanDeleteKey = [=]( TSharedPtr<FMetaChannel> iKeys ) -> bool
    {
        return true;
    };

    //-

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    UMovieSceneSubSection& subsection = board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();
    ACineCameraActor* camera = BoardSequenceTools::GetCamera( sequencer, subsection );

    FText camera_name = FText::FromString( camera->GetActorLabel() ); //TODO: or maybe get the binding (aka track) name like planes ?

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "camera-section-label", "Camera: {0}" ), camera_name ) );

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "delete-camera-key-label", "Delete" ), //TODO: find a way to know the number of "symbolic" keys deleted, 1 symbolic key should represent a key at the same time for the 9 (maybe more or less) channels
                                LOCTEXT( "delete-camera-key-tooltip", "Delete the current key" ),
                                FSlateIcon( FCoreStyle::Get().GetStyleSetName(), "GenericCommands.Delete" ),
                                FUIAction( FExecuteAction::CreateLambda( DeleteKey, iKeys ),
                                           FCanExecuteAction::CreateLambda( CanDeleteKey, iKeys ) ) );

    ioMenuBuilder.EndSection();

    return true;
}

FText
SCinematicBoardSectionCamera::GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const //override
{
    return GetAreaTooltipText();
}

FText
SCinematicBoardSectionCamera::GetAreaTooltipText() const //override
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    UMovieSceneSubSection& subsection = board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    FGuid camera_binding;
    ACineCameraActor* camera = BoardSequenceTools::GetCamera( sequencer, subsection, &camera_binding );

    UMovieSceneSequence* inner_sequence = subsection.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;
    FMovieScenePossessable* possessable = inner_moviescene ? inner_moviescene->FindPossessable( camera_binding ) : nullptr;

    //-

    FString camera_track_name = possessable ? possessable->GetName() : FString();
    FString camera_actor_name = camera ? camera->GetActorLabel() : FString();

    FText camera_track_text = FText::Format( LOCTEXT( "tooltip-camera-area-track", "Camera: {0}" ), FText::FromString( camera_track_name ) );
    FText camera_actor_text = FText::Format( LOCTEXT( "tooltip-camera-area-actor", "Actor: {0}" ), FText::FromString( camera_actor_name ) );
    FText num_keys_text = FText::Format( LOCTEXT( "tooltip-camera-area-num-keys", "Keys: {0}" ), GetMetaChannel()->NumMetaKeys() );

    if( !camera_track_name.Equals( camera_actor_name ) )
        return FText::Join( FText::FromString( TEXT( "\n" ) ), camera_track_text, camera_actor_text, num_keys_text );

    if( camera_track_name.IsEmpty() )
        return LOCTEXT( "tooltip-camera-no", "No camera" );

    return FText::Join( FText::FromString( TEXT( "\n" ) ), camera_track_text, num_keys_text );
}

//---

FCursorReply
SCinematicBoardSectionCamera::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return SMetaKeysArea::OnCursorQuery( MyGeometry, CursorEvent );
}

FReply
SCinematicBoardSectionCamera::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionCamera::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonUp( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionCamera::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseMove( MyGeometry, MouseEvent );
}

const FSlateBrush*
SCinematicBoardSectionCamera::GetBackgroundBrush() const //override
{
    static FSlateColorBrush background_brush = FSlateColorBrush( FLinearColor( .15f, .06f, .14f ) );

    return &background_brush;
}

int32
SCinematicBoardSectionCamera::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    return SMetaKeysArea::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

#undef LOCTEXT_NAMESPACE
