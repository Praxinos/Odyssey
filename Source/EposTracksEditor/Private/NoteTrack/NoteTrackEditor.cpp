// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "NoteTrack/NoteTrackEditor.h"

#include "Rendering/DrawElements.h"
#include "SequencerSectionPainter.h"
#include "EditorStyleSet.h"
#include "ISequencerSection.h"
#include "CommonMovieSceneTools.h"

#include "NoteTrack/MovieSceneNoteTrack.h"
#include "NoteTrack/MovieSceneNoteSection.h"

#define LOCTEXT_NAMESPACE "FNoteTrackEditor"


TSharedRef<ISequencerTrackEditor> FPatchStringPropertyTrackEditor::CreateTrackEditor( TSharedRef<ISequencer> OwningSequencer )
{
    return MakeShareable( new FPatchStringPropertyTrackEditor( OwningSequencer ) );
}


void FPatchStringPropertyTrackEditor::GenerateKeysFromPropertyChanged( const FPropertyChangedParams& PropertyChangedParams, UMovieSceneSection* SectionToKey, FGeneratedTrackKeys& OutGeneratedKeys )
{
    void* CurrentObject = PropertyChangedParams.ObjectsThatChanged[0];
    void* PropertyValue = nullptr;
    for( int32 i = 0; i < PropertyChangedParams.PropertyPath.GetNumProperties(); i++ )
    {
        if( FProperty* Property = PropertyChangedParams.PropertyPath.GetPropertyInfo( i ).Property.Get() )
        {
            CurrentObject = Property->ContainerPtrToValuePtr<FString>( CurrentObject, 0 );
        }
    }

    const FStrProperty* StrProperty = CastField<const FStrProperty>( PropertyChangedParams.PropertyPath.GetLeafMostProperty().Property.Get() );
    if( StrProperty )
    {
        FString StrPropertyValue = StrProperty->GetPropertyValue( CurrentObject );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneStringChannel>( 0, MoveTemp( StrPropertyValue ), true ) );
    }
}

//---

/**
 * Class for note sections
 */
class FNoteSection
    : public FSequencerSection
{
public:

    /** Constructor. */
    FNoteSection(UMovieSceneSection& InSectionObject) : FSequencerSection(InSectionObject) {}

public:

    virtual int32 OnPaintSection( FSequencerSectionPainter& Painter ) const override
    {
        int32 LayerId = Painter.PaintSectionBackground();

        //const ESlateDrawEffect DrawEffects = Painter.bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

        //FVector2D GradientSize = FVector2D( Painter.SectionGeometry.Size.X - 2.f, Painter.SectionGeometry.Size.Y - 3.0f );
        //FPaintGeometry PaintGeometry = Painter.SectionGeometry.ToPaintGeometry( FVector2D( 1.f, 3.f ), GradientSize );

        //const UMovieSceneFadeSection* FadeSection = Cast<const UMovieSceneFadeSection>( WeakSection.Get() );

        //FTimeToPixel TimeConverter    = Painter.GetTimeConverter();
        //FFrameRate   TickResolution   = TimeConverter.GetTickResolution();

        //const double StartTimeSeconds = TimeConverter.PixelToSeconds(1.f);
        //const double EndTimeSeconds   = TimeConverter.PixelToSeconds(Painter.SectionGeometry.GetLocalSize().X-2.f);
        //const double TimeThreshold    = FMath::Max(0.0001, TimeConverter.PixelToSeconds(5) - TimeConverter.PixelToSeconds(0));
        //const double DurationSeconds  = EndTimeSeconds - StartTimeSeconds;

        //TArray<TTuple<double, double>> CurvePoints;
        //FadeSection->FloatCurve.PopulateCurvePoints(StartTimeSeconds, EndTimeSeconds, TimeThreshold, 0.1f, TickResolution, CurvePoints);

        //TArray<FSlateGradientStop> GradientStops;
        //for (TTuple<double, double> Vector : CurvePoints)
        //{
        //  GradientStops.Add( FSlateGradientStop(
        //      FVector2D( (Vector.Get<0>() - StartTimeSeconds) / DurationSeconds * Painter.SectionGeometry.Size.X, 0 ),
        //      FadeSection->FadeColor.CopyWithNewOpacity(Vector.Get<1>()) )
        //  );
        //}

        //if ( GradientStops.Num() > 0 )
        //{
        //  FSlateDrawElement::MakeGradient(
        //      Painter.DrawElements,
        //      Painter.LayerId + 1,
        //      PaintGeometry,
        //      GradientStops,
        //      Orient_Vertical,
        //      DrawEffects
        //      );
        //}

        return LayerId + 1;
    }
};


/* FNoteTrackEditor static functions
 *****************************************************************************/

TSharedRef<ISequencerTrackEditor> FNoteTrackEditor::CreateTrackEditor(TSharedRef<ISequencer> InSequencer)
{
    return MakeShareable(new FNoteTrackEditor(InSequencer));
}


/* FNoteTrackEditor structors
 *****************************************************************************/

FNoteTrackEditor::FNoteTrackEditor(TSharedRef<ISequencer> InSequencer)
    : FPatchStringPropertyTrackEditor(InSequencer)
{ }

/* ISequencerTrackEditor interface
 *****************************************************************************/

TSharedRef<ISequencerSection> FNoteTrackEditor::MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding)
{
    return MakeShareable(new FNoteSection(SectionObject));
}

void FNoteTrackEditor::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
    MenuBuilder.AddMenuEntry(
        LOCTEXT("AddNoteTrack", "Note Track"),
        LOCTEXT("AddNoteTrackTooltip", "Adds a new track that controls note of the sequence."),
        FSlateIcon(FEditorStyle::GetStyleSetName(), "Sequencer.Tracks.Note"),
        FUIAction(
            FExecuteAction::CreateRaw(this, &FNoteTrackEditor::HandleAddNoteTrackMenuEntryExecute),
            FCanExecuteAction::CreateRaw(this, &FNoteTrackEditor::HandleAddNoteTrackMenuEntryCanExecute)
        )
    );
}

bool FNoteTrackEditor::SupportsSequence(UMovieSceneSequence* InSequence) const
{
    ETrackSupport TrackSupported = InSequence ? InSequence->IsTrackSupported(UMovieSceneNoteTrack::StaticClass()) : ETrackSupport::NotSupported;
    return TrackSupported == ETrackSupport::Supported;
}

bool FNoteTrackEditor::SupportsType(TSubclassOf<UMovieSceneTrack> Type) const
{
    return (Type == UMovieSceneNoteTrack::StaticClass());
}

const FSlateBrush* FNoteTrackEditor::GetIconBrush() const
{
    return FEditorStyle::GetBrush("Sequencer.Tracks.Note");
}


/* FNoteTrackEditor callbacks
 *****************************************************************************/

void FNoteTrackEditor::HandleAddNoteTrackMenuEntryExecute()
{
    UMovieScene* MovieScene = GetFocusedMovieScene();

    if (MovieScene == nullptr)
    {
        return;
    }

    if (MovieScene->IsReadOnly())
    {
        return;
    }

    UMovieSceneTrack* NoteTrack = MovieScene->FindMasterTrack<UMovieSceneNoteTrack>();

    //if (NoteTrack != nullptr)
    //{
    //  return;
    //}

    const FScopedTransaction Transaction(LOCTEXT("AddNoteTrack_Transaction", "Add Note Track"));

    MovieScene->Modify();

    NoteTrack = FindOrCreateMasterTrack<UMovieSceneNoteTrack>().Track;
    check(NoteTrack);

    UMovieSceneSection* NewSection = NoteTrack->CreateNewSection();
    check(NewSection);

    NoteTrack->AddSection(*NewSection);
    if (GetSequencer().IsValid())
    {
        GetSequencer()->OnAddTrack(NoteTrack, FGuid());
    }
}

bool FNoteTrackEditor::HandleAddNoteTrackMenuEntryCanExecute() const
{
    return true;
    //UMovieScene* FocusedMovieScene = GetFocusedMovieScene();
    //
    //return ((FocusedMovieScene != nullptr) && (FocusedMovieScene->FindMasterTrack<UMovieSceneNoteTrack>() == nullptr));
}

#undef LOCTEXT_NAMESPACE
