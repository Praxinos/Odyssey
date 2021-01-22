// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionThumbnails.h"

#include "Brushes/SlateColorBrush.h"
#include "MovieSceneToolsUserSettings.h"
#include "SequencerSectionPainter.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionThumbnails"

//---

//static
float
SCinematicBoardSectionThumbnails::GetHeight( TSharedRef<const FCinematicBoardSection> iBoardSection )
{
    float height = FEditorStyle::GetFontStyle( "NormalFont" ).Size + 8.f;

    auto* Settings = GetDefault<UMovieSceneUserThumbnailSettings>();
    if( Settings->bDrawThumbnails )
        height = Settings->ThumbnailSize.Y;

    return height + 2 * 15.f /* top/bottom film-border */;
}

void
SCinematicBoardSectionThumbnails::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    ChildSlot
    [
        SNew( SBox )
        .HeightOverride( this, &SCinematicBoardSectionThumbnails::GetHeight )
    ];
}

FOptionalSize
SCinematicBoardSectionThumbnails::GetHeight() const
{
    return GetHeight( mBoardSection.ToSharedRef() );
}

//---

FTimeToPixel
ConstructTimeConverterForSection( const FGeometry& InSectionGeometry, const UMovieSceneSection& InSection )
{
    FFrameRate     TickResolution = InSection.GetTypedOuter<UMovieScene>()->GetTickResolution();
    double         LowerTime = InSection.GetInclusiveStartFrame() / TickResolution;
    double         UpperTime = InSection.GetExclusiveEndFrame() / TickResolution;

    return FTimeToPixel( InSectionGeometry, TRange<double>( LowerTime, UpperTime ), TickResolution );
}

struct FSequencerSectionPainterImpl
    : FSequencerSectionPainter
{
public:
    /** Constructor */
    FSequencerSectionPainterImpl( UMovieSceneSection& InSection, FSlateWindowElementList& _OutDrawElements, const FGeometry& InSectionGeometry );

    /** Virtual destructor */
    virtual ~FSequencerSectionPainterImpl();

    /** Paint the section background with the specified tint override */
    virtual int32 PaintSectionBackground( const FLinearColor& Tint );

public:
    /** Get a time-to-pixel converter for the section */
    virtual const FTimeToPixel& GetTimeConverter() const;

public:
    FTimeToPixel TimeToPixelConverter;
};

FSequencerSectionPainterImpl::FSequencerSectionPainterImpl( UMovieSceneSection& InSection, FSlateWindowElementList& _OutDrawElements, const FGeometry& InSectionGeometry )
    : FSequencerSectionPainter( _OutDrawElements, InSectionGeometry, InSection )
    , TimeToPixelConverter( ConstructTimeConverterForSection( SectionGeometry, InSection ) )
{
}

FSequencerSectionPainterImpl::~FSequencerSectionPainterImpl()
{
}

int32
FSequencerSectionPainterImpl::PaintSectionBackground( const FLinearColor& Tint )
{
    return 0;
}

const FTimeToPixel&
FSequencerSectionPainterImpl::GetTimeConverter() const
{
    return TimeToPixelConverter;
}

//---

int32
SCinematicBoardSectionThumbnails::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    if( !mBoardSection )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    UMovieSceneSection* sectionObject = mBoardSection->GetSectionObject();
    UMovieSceneSubSection* subsectionObject = Cast<UMovieSceneSubSection>( sectionObject );

    FSequencerSectionPainterImpl painter( *sectionObject, OutDrawElements, AllottedGeometry );
    //painter.KeyAreaElements = ;
    painter.SectionClippingRect = mBoardSection->GetRootPainter( Args )->SectionClippingRect.IntersectionWith( MyCullingRect );
    painter.LayerId = LayerId;
    painter.bParentEnabled = mBoardSection->GetRootPainter( Args )->bParentEnabled;
    painter.bIsHighlighted = mBoardSection->GetRootPainter( Args )->bIsHighlighted;
    painter.bIsSelected = mBoardSection->GetRootPainter( Args )->bIsSelected;

    //---

    static const FSlateBrush* filmBorder = FEditorStyle::GetBrush( "Sequencer.Section.FilmBorder" );

    FVector2D localSectionSize = painter.SectionGeometry.GetLocalSize();

    // Paint fancy-looking film border.
    FSlateDrawElement::MakeBox(
        painter.DrawElements,
        painter.LayerId++,
        painter.SectionGeometry.ToPaintGeometry( FVector2D( localSectionSize.X - 2.f, 7.f ), FSlateLayoutTransform( FVector2D( 1.f, 4.f ) ) ),
        filmBorder
    );

    FSlateDrawElement::MakeBox(
        painter.DrawElements,
        painter.LayerId++,
        painter.SectionGeometry.ToPaintGeometry( FVector2D( localSectionSize.X - 2.f, 7.f ), FSlateLayoutTransform( FVector2D( 1.f, localSectionSize.Y - 11.f ) ) ),
        filmBorder
    );

    //---

    mBoardSection->OnPaintSectionThumbnails( painter );

    //---

    // Paint the sub-sequence information/looping boundaries/etc.

    FSubSectionPainterParams subSectionPainterParams( mBoardSection->GetContentPadding() );
    subSectionPainterParams.bShowTrackNum = false;

    FSubSectionPainterUtil::PaintSection( mBoardSection->GetSequencer(), *subsectionObject, painter, subSectionPainterParams );

    //---

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, painter.LayerId, InWidgetStyle, bParentEnabled );
}

#undef LOCTEXT_NAMESPACE
