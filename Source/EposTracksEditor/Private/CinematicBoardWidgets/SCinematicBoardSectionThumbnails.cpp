// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionThumbnails.h"

#include "Brushes/SlateColorBrush.h"
#include "MovieSceneToolsUserSettings.h"
#include "SequencerSectionPainter.h"
#include "Styles/EposTracksEditorStyle.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/CinematicBoardTrackHelpers.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionThumbnails"

//---

void
SCinematicBoardSectionThumbnails::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    ChildSlot
    [
        SNew( SOverlay )
        + SOverlay::Slot()
        [
            SNew( SBox )
        ]
        + SOverlay::Slot()
        .HAlign( HAlign_Fill )
        .VAlign( VAlign_Center )
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign( VAlign_Center )
            [
                SNew( SComboButton )
                .HasDownArrow( false )
                .ButtonStyle( FEditorStyle::Get(), "SimpleRoundButton" )
                .ButtonColorAndOpacity( FLinearColor( FColor( 72, 72, 72, 255 ) ) ) // like in ...\UE_4.26\Engine\Source\Editor\Sequencer\Private\SAnimationOutlinerTreeNode.cpp
                .Cursor( EMouseCursor::Default )
                .Visibility( InArgs._OptionalWidgetsVisibility )
                .OnGetMenuContent( FOnGetContent::CreateSP( this, &SCinematicBoardSectionThumbnails::HandleAddBoardBeforeComboButtonGetMenuContent ) )
                .ButtonContent()
                [
                    SNew( SImage )
                    .ColorAndOpacity( FLinearColor::White )
                    .Image( FEditorStyle::GetBrush( "Plus" ) )
                ]
            ]
            + SHorizontalBox::Slot()
            [
                SNew( SSpacer )
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign( VAlign_Center )
            [
                SNew( SComboButton )
                .HasDownArrow( false )
                .ButtonStyle( FEditorStyle::Get(), "SimpleRoundButton" )
                .ButtonColorAndOpacity( FLinearColor( FColor( 72, 72, 72, 255 ) ) ) // like in ...\UE_4.26\Engine\Source\Editor\Sequencer\Private\SAnimationOutlinerTreeNode.cpp
                .Cursor( EMouseCursor::Default )
                .Visibility( InArgs._OptionalWidgetsVisibility )
                .OnGetMenuContent( FOnGetContent::CreateSP( this, &SCinematicBoardSectionThumbnails::HandleAddBoardAfterComboButtonGetMenuContent ) )
                .ButtonContent()
                [
                    SNew( SImage )
                    .ColorAndOpacity( FLinearColor::White )
                    .Image( FEditorStyle::GetBrush( "Plus" ) )
                ]
            ]
        ]
    ];
}

TSharedRef<SWidget>
SCinematicBoardSectionThumbnails::HandleAddBoardBeforeComboButtonGetMenuContent()
{
    FMenuBuilder menuBuilder( true, nullptr );

    menuBuilder.AddMenuEntry( LOCTEXT( "create-board-before.label", "New Previous Board" ),
                              LOCTEXT( "create-board-before.description", "Create a new board inside a new section before this section" ),
                              FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.NewSectionWithBoardBeforeSection" ),
                              FUIAction( FExecuteAction::CreateLambda( [this]()
                                                                       {
                                                                           TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();
                                                                           UMovieSceneSection* sectionObject = section->GetSectionObject();
                                                                           CinematicBoardTrackHelpers::InsertBoard( section->GetSequencer().Get(), sectionObject->GetInclusiveStartFrame() );
                                                                       } ) ) );

    menuBuilder.AddMenuEntry( LOCTEXT( "create-shot-before.label", "New Previous Shot" ),
                              LOCTEXT( "create-shot-before.description", "Create a new shot inside a new section before this section" ),
                              FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.NewSectionWithShotBeforeSection" ),
                              FUIAction( FExecuteAction::CreateLambda( [this]()
                                                                       {
                                                                           TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();
                                                                           UMovieSceneSection* sectionObject = section->GetSectionObject();
                                                                           CinematicBoardTrackHelpers::InsertShot( section->GetSequencer().Get(), sectionObject->GetInclusiveStartFrame() );
                                                                       } ) ) );

    return menuBuilder.MakeWidget();
}

TSharedRef<SWidget>
SCinematicBoardSectionThumbnails::HandleAddBoardAfterComboButtonGetMenuContent()
{
    FMenuBuilder menuBuilder( true, nullptr );

    menuBuilder.AddMenuEntry( LOCTEXT( "create-board-after.label", "New Next Board" ),
                              LOCTEXT( "create-board-after.description", "Create a new board inside a new section after this section" ),
                              FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.NewSectionWithBoardAfterSection" ),
                              FUIAction( FExecuteAction::CreateLambda( [this]()
                                                                       {
                                                                           TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();
                                                                           UMovieSceneSection* sectionObject = section->GetSectionObject();
                                                                           CinematicBoardTrackHelpers::InsertBoard( section->GetSequencer().Get(), sectionObject->GetExclusiveEndFrame() - 1 );
                                                                       } ) ) );

    menuBuilder.AddMenuEntry( LOCTEXT( "create-shot-after.label", "New Next Shot" ),
                              LOCTEXT( "create-shot-after.description", "Create a new shot inside a new section after this section" ),
                              FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.NewSectionWithShotAfterSection" ),
                              FUIAction( FExecuteAction::CreateLambda( [this]()
                                                                       {
                                                                           TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();
                                                                           UMovieSceneSection* sectionObject = section->GetSectionObject();
                                                                           CinematicBoardTrackHelpers::InsertShot( section->GetSequencer().Get(), sectionObject->GetExclusiveEndFrame() - 1 );
                                                                       } ) ) );

    return menuBuilder.MakeWidget();
}

FVector2D
SCinematicBoardSectionThumbnails::ComputeDesiredSize( float ) const //override
{
    FVector2D size = GetDesiredSize();

    auto* Settings = GetDefault<UMovieSceneUserThumbnailSettings>();
    size.Y = Settings->bDrawThumbnails ? Settings->ThumbnailSize.Y : SequencerSectionConstants::DefaultSectionHeight;
    size.Y = size.Y + 2 * SequencerSectionConstants::DefaultSectionHeight /* top/bottom film-border */;

    return size;
}

//---

static
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
    if( !mBoardSection.IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    UMovieSceneSection* sectionObject = section->GetSectionObject();
    UMovieSceneSubSection* subsectionObject = Cast<UMovieSceneSubSection>( sectionObject );

    FSequencerSectionPainterImpl painter( *sectionObject, OutDrawElements, AllottedGeometry );
    //painter.KeyAreaElements = ;
    painter.SectionClippingRect = section->GetRootPainter( Args )->SectionClippingRect.IntersectionWith( MyCullingRect );
    painter.LayerId = LayerId;
    painter.bParentEnabled = section->GetRootPainter( Args )->bParentEnabled;
    painter.bIsHighlighted = section->GetRootPainter( Args )->bIsHighlighted;
    painter.bIsSelected = section->GetRootPainter( Args )->bIsSelected;

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

    painter.LayerId = section->FKeyThumbnailSection::OnPaintSection( painter );

    //---

    // Paint the sub-sequence information/looping boundaries/etc.

    FSubSectionPainterParams subSectionPainterParams( section->GetContentPadding() );
    subSectionPainterParams.bShowTrackNum = false;

    FSubSectionPainterUtil::PaintSection( section->GetSequencer(), *subsectionObject, painter, subSectionPainterParams );

    //---

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, painter.LayerId, InWidgetStyle, bParentEnabled );
}

#undef LOCTEXT_NAMESPACE
