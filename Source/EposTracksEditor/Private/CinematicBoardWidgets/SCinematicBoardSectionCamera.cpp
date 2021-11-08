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
    mBoardSection = iBoardSection; //TODO: maybe find a way to set it inside the parent ?

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
    auto DeleteKey = [=]( TSharedPtr<FMetaChannel> iKeysUnderMouse )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        const FScopedTransaction transaction( LOCTEXT( "DeleteCameraKeys", "Delete camera keys" ) );

        for( auto pair : iKeysUnderMouse->GetMetaKeys() )
        {
            for( const auto& subkey : pair.Value.mSubKeys )
            {
                BoardSequenceTools::DeleteCameraKey( sequencer, *subsection_object, subkey.mSection.Get(), subkey.mChannelHandle, subkey.mKeyHandle );
            }
        }
    };

    auto CanDeleteKey = [=]( TSharedPtr<FMetaChannel> iKeysUnderMouse ) -> bool
    {
        return true;
    };

    //-

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    ISequencer* sequencer = board_section->GetSequencer().Get();
    ACineCameraActor* camera = BoardSequenceTools::GetCamera( sequencer, sequencer->GetLocalTime().Time.FrameNumber );

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
