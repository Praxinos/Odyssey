// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SCompoundWidget.h"

//---

class FNoteSection;
class SInlineEditableTextBlock;

class EPOSTRACKSEDITOR_API SNoteSectionContent
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SNoteSectionContent )
        {}
    SLATE_END_ARGS()

    // Construct the widget
    void Construct(const FArguments& InArgs, TSharedRef<FNoteSection> iNoteSection);

    virtual void OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent );
    virtual void OnMouseLeave( const FPointerEvent& MouseEvent );
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

    virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override;

protected:
    FText           GetNoteText() const;
    void            OnNoteTextCommited( const FText& iText, ETextCommit::Type CommitType );

public:
    EVisibility OptionalWidgetsVisibility() const;

private:
    TWeakPtr<FNoteSection> mNoteSection;

    TSharedPtr<SInlineEditableTextBlock> mNoteTextWidget;

    EVisibility mOptionalWidgetsVisibility = EVisibility::Collapsed; // By default Collapsed, it's ok as we manage only 1 toolbar and that's we want
};
