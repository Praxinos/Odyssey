// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Channels/MovieSceneChannelHandle.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Widgets/SCompoundWidget.h"

//---

class FCinematicBoardSection;

class EPOSTRACKSEDITOR_API SCinematicBoardSectionCamera
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionCamera )
        {}
    SLATE_END_ARGS()

    // Construct the widget
    void Construct(const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection);

    // SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

    virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual void OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual void OnMouseLeave( const FPointerEvent& MouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

protected:
    // SWidget overrides.
    virtual FVector2D ComputeDesiredSize( float ) const override;

private:
    TSharedPtr<FMetaFloatChannel> GetKeysUnderMouse( const FPointerEvent& MouseEvent ) const;

    void BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder );

    /** Start a transaction at mouse down */
    void BeginTransaction( const FText& iTransactionDesc );
    /** End the transaction at mouse up */
    void EndTransaction();

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;

    /** Scoped transaction for this drag operation */
    TUniquePtr<FScopedTransaction>      mTransaction;

    TSharedPtr<FMetaFloatChannel>       mKeysUnderMouse;
};
