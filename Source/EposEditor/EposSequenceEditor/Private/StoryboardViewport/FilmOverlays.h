// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Framework/Commands/UICommandList.h"
#include "Math/Vector2D.h"
#include "Misc/Optional.h"
#include "UObject/NameTypes.h"
#include "Widgets/SCompoundWidget.h"
#include "IFilmOverlay.h"

class FPaintArgs;
class FSlateWindowElementList;
struct FSlateBrush;


/** A widget that sits on top of a viewport, and draws custom content */
class SFilmOverlay : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SFilmOverlay){}

        /** User provided array of overlays to draw */
        SLATE_ATTRIBUTE(TArray<IFilmOverlay*>, FilmOverlays)

    SLATE_END_ARGS()

    /** Construct this widget */
    void Construct(const FArguments& InArgs);

    /** Paint this widget */
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    /** Assigns film overlays to this film overlay widget*/
    void SetFilmOverlays( const TAttribute<TArray<IFilmOverlay*>>& InFilmOverlays )
    {
        FilmOverlays = InFilmOverlays;
    }

    /** Sets the current primary film overlay */
    void SetPrimaryFilmOverlay( const FName InFilmOverlay );

    /** Get the current primary film overlay */
    FName GetPrimaryFilmOverlay() const
    {
        return PrimaryFilmOverlay;
    }

    /** The tint this viewport applies to the primary film overlay it picks */
    const FLinearColor& GetPrimaryTint() const;

    /** Set the tint this viewport applies to its primary film overlay, now and whenever another is picked */
    void SetPrimaryTint( const FLinearColor& InTint );

private:
    /** Attribute used once per frame to retrieve the film overlays to paint */
    TAttribute<TArray<IFilmOverlay*>> FilmOverlays;

    /** Currently selected primary film overlay */
    FName PrimaryFilmOverlay;

    /** Tint this viewport applies to whichever primary film overlay it picks, which other viewports picking the same overlay replace */
    FLinearColor PrimaryTint = IFilmOverlay::DefaultTint;
};

/** A custom widget that comprises a combo box displaying all available overlay options */
class SFilmOverlayOptions : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SFilmOverlayOptions)
        : _IsComboButton( true )
        {}
        /** Set this to false in order to directly show the menu content, instead of showing a button to summon it */
        SLATE_ATTRIBUTE( bool, IsComboButton )
    SLATE_END_ARGS()

    /** Construct this widget */
    void Construct(const FArguments& InArgs, TSharedPtr<SFilmOverlay> InFilmOverlay);

    /** Retrieve the actual overlay widget that this widget controls. Can be positioned in any other widget hierarchy. */
    TSharedPtr<SFilmOverlay> GetFilmOverlayWidget() const;

    /** Bind commands for the overlays */
    void BindCommands( TSharedRef<FUICommandList> );

    /** Register the default primary and toggleable overlays into UFilmOverlayToolkit (idempotent). */
    static void RegisterDefaultOverlays();

    /** Generate menu content for the combo button */
    TSharedRef<SWidget> GetMenuContent();

private:

    /** Construct the part of the menu that defines the set of film overlays */
    TSharedRef<SWidget> ConstructPrimaryOverlaysMenu();

    /** Construct the part of the menu that defines the set of toggleable overlays (currently just safe-frames) */
    TSharedRef<SWidget> ConstructToggleableOverlaysMenu();

private:

    /** Get the thumbnail to be displayed on the combo box button */
    const FSlateBrush* GetCurrentThumbnail() const;

    /** Get the primary film overlay ptr (may be nullptr) */
    IFilmOverlay* GetPrimaryFilmOverlay() const;

    /** Get an array of all enabled film overlays */
    TArray<IFilmOverlay*> GetActiveFilmOverlays() const;

    /** Set the current primary overlay to the specified name */
    FReply SetPrimaryFilmOverlay(FName InName);

    /** Get/Set the tint the overlay widget applies to its primary overlay, which the menu's color control edits */
    FLinearColor GetPrimaryColorTint() const;
    void SetPrimaryColorTint( const FLinearColor& InTint );

    /** Toggle the film overlay enabled or disabled */
    FReply ToggleFilmOverlay( FName InName );

private:

    ///** Primary overlays (only one can be active at a time) */
    //TArray<TSharedPtr<IFilmOverlay>> PrimaryOverlays;

    ///** Toggleable overlays (any number can be active at a time) */
    //TArray<TSharedPtr<IFilmOverlay>> ToggleableOverlays;

    /** The overlay widget we control - externally owned */
    TWeakPtr<SFilmOverlay> WeakOverlayWidget;
};

/** Script access to the settings of the default frame overlays RegisterDefaultOverlays creates, which their settings widgets otherwise own */
namespace UE::EposSequenceEditor::FilmOverlays
{

    /** The name RegisterDefaultOverlays registers the letterbox under */
    inline const FLazyName LetterBoxName( TEXT( "LetterBox" ) );

    /** The smallest and largest value either term of the letterbox ratio can take, which its settings widget also enforces */
    inline constexpr float MinLetterboxRatioTerm = 0.1f;
    inline constexpr float MaxLetterboxRatioTerm = 35.f;

    /** The inset of the default safe frame registered under InName as a percentage of the viewport, or unset when no default safe frame is registered there */
    TOptional<int32> GetSafeFrameSize( const FName InName );

    /** Sets the inset of the default safe frame registered under InName as a percentage of the viewport, returning false when no default safe frame is registered there */
    bool SetSafeFrameSize( const FName InName, const int32 InSizePercentage );

    /** The width and height terms of the ratio the default letterbox masks the viewport to, as X and Y, or unset when it is not registered */
    TOptional<FVector2D> GetLetterboxAspectRatio();

    /** Sets the width and height terms of the ratio the default letterbox masks the viewport to, returning false when it is not registered */
    bool SetLetterboxAspectRatio( const float InWidth, const float InHeight );

} // namespace UE::EposSequenceEditor::FilmOverlays
