// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SCompoundWidget.h"

//---

class FCinematicBoardSection;

class EPOSTRACKSEDITOR_API SCinematicBoardSectionThumbnails
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionThumbnails )
        {}
        SLATE_ATTRIBUTE( EVisibility, OptionalWidgetsVisibility )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct(const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection);

    // SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

private:
    TSharedRef<SWidget> HandleAddBoardBeforeComboButtonGetMenuContent();
    TSharedRef<SWidget> HandleAddBoardAfterComboButtonGetMenuContent();
    TSharedRef<SWidget> CreatePopupEntryNewSectionWithDurationWidget( FText iLabel, TSharedRef<int32> iNumberOfNewSequence );
    TSharedRef<SWidget> CreatePopupEntryNewSectionWithDurationText( FText iLabel, TSharedRef<int32> iNumberOfNewSequence );

    TSharedRef<SWidget> MakeCreateCameraMenu();
    TSharedRef<SWidget> MakeTakeMenu();
    void CreateCamera( TSharedRef<FString> iCameraName, TSharedRef<FString> iPlaneName );

protected:
    // SWidget overrides.
    virtual FVector2D ComputeDesiredSize( float ) const override;

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;

    TAttribute<EVisibility> mOptionalWidgetsVisibility;
};
