// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Widgets/Input/SEditableTextBox.h"

//---

class IPropertyHandle;

DECLARE_DELEGATE_RetVal_OneParam( bool, FOnVerifyPattern, const FString& iPattern );

//---

/**
  * This manage a pattern text box
  * It will check the validity of keywords and display all the available keywords
  */
class EPOSPATTERN_API SPatternTextBox
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SPatternTextBox )
        {}
        SLATE_ARGUMENT( TArray<FString>, Keywords )
        SLATE_ARGUMENT( TArray<FText>, KeywordLabels )
        SLATE_ARGUMENT( TArray<FText>, KeywordHelps )
        SLATE_ATTRIBUTE( FText, MoreExplanation )
        SLATE_EVENT( FOnVerifyPattern, OnVerifyPattern )
    SLATE_END_ARGS()

    void Construct( const FArguments& iArgs, TSharedPtr<IPropertyHandle> iPatternHandle );

private:
    FText GetPatternText() const;

    void OnPatternTextCommited( const FText& iNewText, ETextCommit::Type iCommitInfo );
    void OnPatternTextChanged( const FText& iNewText );

    FReply OnClickExpanderButton();
    const FSlateBrush* GetExpanderIcon() const;
    EVisibility GetKeywordsVisibility() const;

    FReply OnClickKeyword( int iKeywordIndex );

    //void ContextMenuExtender( FMenuBuilder& iMenuBuilder );
    //void AddKeywordAtCursor( FString iKeyword );

private:
    bool IsKeywordUsed( int iKeywordIndex ) const;

private:
    TSharedPtr<IPropertyHandle> mPatternHandle;
    TArray<FString>             mKeywords;

    TSharedPtr<SEditableTextBox> mTextBoxWidget;

    bool mIsExpanded { false };

    /** Callback to verify pattern. */
    FOnVerifyPattern mOnVerifyPattern;
};
