// IDDN.FR.000.000000.000.S.X.0000.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#include "SingleCameraCutSequencerFilters.h"

#include "Filters/SequencerTrackFilterBase.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandInfo.h"

#include "Shot/ShotSequence.h"
#include "Styles/EposTracksEditorStyle.h"

#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"

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

class FSequencerTrackFilter_SingleCameraCut: public FSequencerTrackFilter_ClassType<UMovieSceneSingleCameraCutTrack>
{
public:
    FSequencerTrackFilter_SingleCameraCut( ISequencerTrackFilters& InFilterInterface, TSharedPtr<FFilterCategory> InCategory = nullptr )
        : FSequencerTrackFilter_ClassType<UMovieSceneSingleCameraCutTrack>( InFilterInterface, InCategory )
    {
        FSequencerTrackFilter_SingleCameraCutFilterCommands::Register();
    }

    virtual ~FSequencerTrackFilter_SingleCameraCut() override
    {
        FSequencerTrackFilter_SingleCameraCutFilterCommands::Unregister();
    }

    //~ Begin IFilter
    virtual FString GetName() const override
    {
        return TEXT( "SingleCameraCut" );
    }
    //~ End IFilter

    //~ Begin FFilterBase
    virtual FText GetDisplayName() const override
    {
        return LOCTEXT( "SequencerTrackFilter_SingleCameraCut", "Single CameraCut" );
    }
    virtual FSlateIcon GetIcon() const override
    {
        return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), TEXT( "Sequencer.Tracks.SingleCameraCut" ) );
    }
    //~ End FFilterBase

    //~ Begin FSequencerTrackFilter

    virtual FText GetDefaultToolTipText() const override
    {
        return LOCTEXT( "SequencerTrackFilter_SingleCameraCutToolTip", "Show only Single CameraCut tracks" );
    }

    virtual TSharedPtr<FUICommandInfo> GetToggleCommand() const override
    {
        return FSequencerTrackFilter_SingleCameraCutFilterCommands::Get().ToggleFilter_SingleCameraCut;
    }

    virtual bool SupportsSequence( UMovieSceneSequence* const InSequence ) const override
    {
        // Here the class must be also checked
        // because board sequence internally supports SingleCameraCut track (see UBoardSequence::IsTrackSupportedImpl())
        // but it is not displayed in the Add menu
        return InSequence->IsA<UShotSequence>() && IsSequenceTrackSupported<UMovieSceneSingleCameraCutTrack>( InSequence );
    }

    //~ End FSequencerTrackFilter
};

//////////////////////////////////////////////////////////////////////////
//

void USingleCameraCutSequencerTrackFilter::AddTrackFilterExtensions( ISequencerTrackFilters& InFilterInterface, const TSharedRef<FFilterCategory>& InPreferredCategory, TArray<TSharedRef<FSequencerTrackFilter>>& InOutFilterList ) const
{
    InOutFilterList.Add( MakeShared<FSequencerTrackFilter_SingleCameraCut>( InFilterInterface, InPreferredCategory ) );
}

#undef LOCTEXT_NAMESPACE
