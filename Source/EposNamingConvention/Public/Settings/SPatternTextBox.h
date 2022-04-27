// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Widgets/Input/SEditableTextBox.h"

//---

class IPropertyHandle;

//---

/**
  * This manage a pattern text box
  * It will check the validity of keywords and display all the available keywords
  *
  * (this is maybe not the best place (Naming module) as it is also used by the SequenceEditor module)
  */
class EPOSNAMINGCONVENTION_API SPatternTextBox
    : public SCompoundWidget
    //: public SEditableTextBox
{
public:
    SLATE_BEGIN_ARGS( SPatternTextBox )
        {}
        SLATE_ARGUMENT( TArray<FString>, Keywords )
        SLATE_ARGUMENT( TArray<FText>, KeywordLabels )
        SLATE_ARGUMENT( TArray<FText>, KeywordHelps )
        SLATE_ATTRIBUTE( FText, MoreExplanation )
    SLATE_END_ARGS()

    void Construct( const FArguments& iArgs, TSharedPtr<IPropertyHandle> iPatternHandle );

private:
    FText GetPatternText() const;

    void OnPatternTextCommited( const FText& iNewText, ETextCommit::Type iCommitInfo );
    void OnPatternTextChanged( const FText& iNewText );

    bool CheckPatternValidity( const FString& iPattern );

private:
    TSharedPtr<IPropertyHandle> mPatternHandle;

    TSharedPtr<SEditableTextBox> mTextBoxWidget;

    TArray<FString> mValidKeywords;
};
