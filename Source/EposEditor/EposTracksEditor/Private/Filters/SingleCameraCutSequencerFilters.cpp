// IDDN.FR.000.000000.000.S.X.0000.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SingleCameraCutSequencerFilters.h"

#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandInfo.h"

#include "Shot/ShotSequence.h"
#include "Styles/EposTracksEditorStyle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SingleCameraCutSequencerFilters)

#define LOCTEXT_NAMESPACE "SingleCameraCutSequencerTrackFilters"

class FSequencerTrackFilter_SingleCameraCutFilterCommands
    : public TCommands<FSequencerTrackFilter_SingleCameraCutFilterCommands>
{
public:
    FSequencerTrackFilter_SingleCameraCutFilterCommands()
        : TCommands<FSequencerTrackFilter_SingleCameraCutFilterCommands>(
            TEXT( "FSequencerTrackFilter_SingleCameraCut" ),
            LOCTEXT( "FSequencerTrackFilter_SingleCameraCut", "Single CameraCut Filters" ),
            NAME_None,
            FEposTracksEditorStyle::Get().GetStyleSetName() )
    {
    }

    TSharedPtr<FUICommandInfo> ToggleFilter_SingleCameraCut;

    virtual void RegisterCommands() override
    {
        UI_COMMAND( ToggleFilter_SingleCameraCut, "Toggle SingleCameraCut Filter", "Toggle the filter for Single CameraCut tracks", EUserInterfaceActionType::ToggleButton, FInputChord() );
    }
};

//////////////////////////////////////////////////////////////////////////
//

//static
FString
FSequencerTrackFilter_SingleCameraCut::StaticName()
{
    return TEXT( "SingleCameraCut" );
}

FSequencerTrackFilter_SingleCameraCut::FSequencerTrackFilter_SingleCameraCut( ISequencerTrackFilters& InFilterInterface, TSharedPtr<FFilterCategory> InCategory )
    : FSequencerTrackFilter_ClassType<UMovieSceneSingleCameraCutTrack>( InFilterInterface, InCategory )
{
    FSequencerTrackFilter_SingleCameraCutFilterCommands::Register();
}

FSequencerTrackFilter_SingleCameraCut::~FSequencerTrackFilter_SingleCameraCut()
{
    FSequencerTrackFilter_SingleCameraCutFilterCommands::Unregister();
}

FString
FSequencerTrackFilter_SingleCameraCut::GetName() const //override
{
    return StaticName();
}

FText
FSequencerTrackFilter_SingleCameraCut::GetDisplayName() const //override
{
    return LOCTEXT( "SequencerTrackFilter_SingleCameraCut", "Single CameraCut" );
}
FSlateIcon
FSequencerTrackFilter_SingleCameraCut::GetIcon() const //override
{
    return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), TEXT( "Sequencer.Tracks.SingleCameraCut" ) );
}

FText
FSequencerTrackFilter_SingleCameraCut::GetDefaultToolTipText() const //override
{
    return LOCTEXT( "SequencerTrackFilter_SingleCameraCutToolTip", "Show only Single CameraCut tracks" );
}

TSharedPtr<FUICommandInfo>
FSequencerTrackFilter_SingleCameraCut::GetToggleCommand() const //override
{
    return FSequencerTrackFilter_SingleCameraCutFilterCommands::Get().ToggleFilter_SingleCameraCut;
}

bool
FSequencerTrackFilter_SingleCameraCut::SupportsSequence( UMovieSceneSequence* const InSequence ) const //override
{
    // Here the class must be also checked
    // because board sequence internally supports SingleCameraCut track (see UBoardSequence::IsTrackSupportedImpl())
    // but it is not displayed in the Add menu
    return InSequence->IsA<UShotSequence>() && IsSequenceTrackSupported<UMovieSceneSingleCameraCutTrack>( InSequence );
}

//////////////////////////////////////////////////////////////////////////
//

void USingleCameraCutSequencerTrackFilter::AddTrackFilterExtensions( ISequencerTrackFilters& InFilterInterface, const TSharedRef<FFilterCategory>& InPreferredCategory, TArray<TSharedRef<FSequencerTrackFilter>>& InOutFilterList ) const
{
    InOutFilterList.Add( MakeShared<FSequencerTrackFilter_SingleCameraCut>( InFilterInterface, InPreferredCategory ) );
}

#undef LOCTEXT_NAMESPACE
