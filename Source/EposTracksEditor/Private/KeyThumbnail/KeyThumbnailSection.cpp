// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "KeyThumbnail/KeyThumbnailSection.h"

#include "Rendering/DrawElements.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleManager.h"
#include "Application/ThrottleManager.h"
#include "Widgets/Layout/SBox.h"
#include "SequencerSectionPainter.h"
#include "EditorStyleSet.h"
#include "LevelEditorViewport.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "IVREditorModule.h"
#include "MovieScene.h"
#include "MovieSceneTimeHelpers.h"

#define LOCTEXT_NAMESPACE "FKeyThumbnailSection"


/* FKeyThumbnailSection structors
 *****************************************************************************/

FKeyThumbnailSection::FKeyThumbnailSection( TSharedPtr<ISequencer> InSequencer, TSharedPtr<FTrackEditorThumbnailPool> InThumbnailPool, UMovieSceneSection& InSection )
    : FViewportThumbnailSection( InSequencer, InThumbnailPool, InSection )
    , KeyThumbnailCache( InThumbnailPool, this )
{
    GetMutableDefault<UMovieSceneUserThumbnailSettings>()->OnForceRedraw().Remove( RedrawThumbnailDelegateHandle );
    RedrawThumbnailDelegateHandle = GetMutableDefault<UMovieSceneUserThumbnailSettings>()->OnForceRedraw().AddRaw( this, &FKeyThumbnailSection::RedrawThumbnails ); // Removed inside parent destructor

    mRebuildKeysDelegateHandle = SequencerPtr.Pin()->OnMovieSceneDataChanged().AddRaw( this, &FKeyThumbnailSection::RebuildKeys );
}


FKeyThumbnailSection::~FKeyThumbnailSection()
{
    if( SequencerPtr.IsValid() )
        SequencerPtr.Pin()->OnMovieSceneDataChanged().Remove( mRebuildKeysDelegateHandle );
}


void FKeyThumbnailSection::RedrawThumbnails()
{
    KeyThumbnailCache.ForceRedraw();
}

void FKeyThumbnailSection::RebuildKeys( EMovieSceneDataChangeType iType )
{
    BuildKeys();
}

void FKeyThumbnailSection::BuildKeys()
{
    BuildThumbnailKeys();
}

/* ISequencerSection interface
 *****************************************************************************/


void FKeyThumbnailSection::BuildSectionContextMenu( FMenuBuilder& MenuBuilder, const FGuid& ObjectBinding )
{
    MenuBuilder.BeginSection( NAME_None, LOCTEXT( "ViewMenuText", "View" ) );
    {
        MenuBuilder.AddSubMenu(
            LOCTEXT( "ThumbnailsMenu", "Thumbnails" ),
            FText(),
            FNewMenuDelegate::CreateLambda( [=]( FMenuBuilder& InMenuBuilder )
            {
                TSharedPtr<ISequencer> Sequencer = SequencerPtr.Pin();

                FText CurrentTime = FText::FromString( Sequencer->GetNumericTypeInterface()->ToString( Sequencer->GetLocalTime().Time.GetFrame().Value ) );

                InMenuBuilder.BeginSection( NAME_None, LOCTEXT( "ThisSectionText", "This Section" ) );
                {
                    InMenuBuilder.AddMenuEntry(
                        LOCTEXT( "RefreshText", "Refresh" ),
                        LOCTEXT( "RefreshTooltip", "Refresh this section's thumbnails" ),
                        FSlateIcon(),
                        FUIAction( FExecuteAction::CreateRaw( this, &FKeyThumbnailSection::RedrawThumbnails ) )
                    );
                    InMenuBuilder.AddMenuEntry(
                        FText::Format( LOCTEXT( "SetSingleTime", "Set Thumbnail Time To {0}" ), CurrentTime ),
                        LOCTEXT( "SetSingleTimeTooltip", "Defines the time at which this section should draw its single thumbnail to the current cursor position" ),
                        FSlateIcon(),
                        FUIAction(
                            FExecuteAction::CreateLambda( [=]
                    {
                        SetSingleTime( Sequencer->GetLocalTime().AsSeconds() );
                        GetMutableDefault<UMovieSceneUserThumbnailSettings>()->bDrawSingleThumbnails = true;
                        GetMutableDefault<UMovieSceneUserThumbnailSettings>()->SaveConfig();
                    } )
                        )
                    );
                }
                InMenuBuilder.EndSection();

                InMenuBuilder.BeginSection( NAME_None, LOCTEXT( "GlobalSettingsText", "Global Settings" ) );
                {
                    InMenuBuilder.AddMenuEntry(
                        LOCTEXT( "RefreshAllText", "Refresh All" ),
                        LOCTEXT( "RefreshAllTooltip", "Refresh all sections' thumbnails" ),
                        FSlateIcon(),
                        FUIAction( FExecuteAction::CreateLambda( []
                    {
                        GetDefault<UMovieSceneUserThumbnailSettings>()->BroadcastRedrawThumbnails();
                    } ) )
                    );

                    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );

                    FDetailsViewArgs Args;
                    Args.bAllowSearch = false;
                    Args.NameAreaSettings = FDetailsViewArgs::HideNameArea;

                    TSharedRef<IDetailsView> DetailView = PropertyModule.CreateDetailView( Args );
                    DetailView->SetObject( GetMutableDefault<UMovieSceneUserThumbnailSettings>() );
                    InMenuBuilder.AddWidget( DetailView, FText(), true );
                }
                InMenuBuilder.EndSection();
            } )
        );
    }
    MenuBuilder.EndSection();
}


int32 FKeyThumbnailSection::OnPaintSection( FSequencerSectionPainter& InPainter ) const
{
    if( !GetDefault<UMovieSceneUserThumbnailSettings>()->bDrawThumbnails )
    {
        return InPainter.LayerId;
    }

    static const float SectionThumbnailPadding = 4.f;

    ESlateDrawEffect DrawEffects = InPainter.bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

    int32 LayerId = InPainter.LayerId;

    const FGeometry& SectionGeometry = InPainter.SectionGeometry;

    // @todo Sequencer: Need a way to visualize the key here

    const TRange<double> VisibleRange = GetVisibleRange();
    const TRange<double> GenerationRange = GetTotalRange();

    const float TimePerPx = GenerationRange.Size<double>() / InPainter.SectionGeometry.GetLocalSize().X;

    const FFrameRate TickResolution = Section->GetTypedOuter<UMovieScene>()->GetTickResolution();
    const double SectionEaseInDuration = TickResolution.AsSeconds( Section->Easing.GetEaseInDuration() ) / TimePerPx;
    const double SectionEaseOutDuration = TickResolution.AsSeconds( Section->Easing.GetEaseOutDuration() ) / TimePerPx;

    const FSlateRect ThumbnailClipRect = SectionGeometry.GetLayoutBoundingRect()
        .InsetBy( FMargin( SectionThumbnailPadding, 0.f ) )
        .InsetBy( FMargin( SectionEaseInDuration, 0.f, SectionEaseOutDuration, 0.f ) )
        .IntersectionWith( InPainter.SectionClippingRect );

    for( const TSharedPtr<FTrackEditorThumbnail>& Thumbnail : KeyThumbnailCache.GetThumbnails() )
    {
        const float Fade = Thumbnail->bHasFinishedDrawing ? Thumbnail->GetFadeInCurve() : 1.f;
        if( Fade >= 1.f )
        {
            continue;
        }

        FIntPoint ThumbnailRTSize = Thumbnail->GetSize();
        FIntPoint ThumbnailCropSize = Thumbnail->GetDesiredSize();

        const float ThumbnailScale = float( ThumbnailCropSize.Y ) / ThumbnailRTSize.Y;
        const float HorizontalCropOffset = ( ThumbnailRTSize.X*ThumbnailScale - ThumbnailCropSize.X ) * 0.5f;

        // Calculate the paint geometry for this thumbnail
        TOptional<double> SingleReferenceFrame = KeyThumbnailCache.GetSingleReferenceFrame();

        // Single thumbnails are always drawn at the start of the section, clamped to the visible range
        // Thumbnail sequences draw relative to their actual position in the sequence/section
        const float PositionX = SingleReferenceFrame.IsSet()
            ? FMath::Max( float( VisibleRange.GetLowerBoundValue() - GenerationRange.GetLowerBoundValue() ) / TimePerPx, 0.f ) + SectionThumbnailPadding
            : ( Thumbnail->GetTimeRange().GetLowerBoundValue() - GenerationRange.GetLowerBoundValue() ) / TimePerPx;

        const float PositionY = ( SectionGeometry.GetLocalSize().Y - ThumbnailCropSize.Y )*.5f;

        FPaintGeometry PaintGeometry = SectionGeometry.ToPaintGeometry(
            ThumbnailRTSize,
            FSlateLayoutTransform( ThumbnailScale, FVector2D( PositionX - HorizontalCropOffset, PositionY ) )
        );

        if( IVREditorModule::Get().IsVREditorModeActive() )
        {
            // In VR editor every widget is in the world and gamma corrected by the scene renderer.  Thumbnails will have already been gamma
            // corrected and so they need to be reversed
            DrawEffects |= ESlateDrawEffect::ReverseGamma;
        }
        else
        {
            DrawEffects |= ESlateDrawEffect::NoGamma;
        }

        if( Thumbnail->bIgnoreAlpha )
        {
            DrawEffects |= ESlateDrawEffect::IgnoreTextureAlpha;
        }

        FGeometry ClipGeometry = SectionGeometry.MakeChild(
            ThumbnailCropSize,
            FSlateLayoutTransform(
                FVector2D( PositionX, PositionY )
            )
        );

        FSlateRect ThisThumbnailClipRect = ThumbnailClipRect.IntersectionWith( ClipGeometry.GetLayoutBoundingRect() );

        FSlateClippingZone ClippingZone( ThisThumbnailClipRect );
        InPainter.DrawElements.PushClip( ClippingZone );

        FSlateDrawElement::MakeViewport(
            InPainter.DrawElements,
            LayerId,
            PaintGeometry,
            Thumbnail,
            DrawEffects | AdditionalDrawEffect,
            FLinearColor( 1.f, 1.f, 1.f, 1.f - Fade )
        );

        InPainter.DrawElements.PopClip();
    }

    return LayerId + 2;
}

void FKeyThumbnailSection::Tick( const FGeometry& AllottedGeometry, const FGeometry& ParentGeometry, const double InCurrentTime, const float InDeltaTime )
{
    if( FSlateThrottleManager::Get().IsAllowingExpensiveTasks() && GetDefault<UMovieSceneUserThumbnailSettings>()->bDrawThumbnails )
    {
        const UMovieSceneUserThumbnailSettings* Settings = GetDefault<UMovieSceneUserThumbnailSettings>();

        FIntPoint AllocatedSize = AllottedGeometry.GetLocalSize().IntPoint();
        AllocatedSize.X = FMath::Max( AllocatedSize.X, 1 );

        KeyThumbnailCache.Update( GetTotalRange(), GetVisibleRange(), GetThumbnailKeys(), AllocatedSize, Settings->ThumbnailSize, Settings->Quality, InCurrentTime );
    }
}

#undef LOCTEXT_NAMESPACE
