// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UOdysseyPainterEditorRasterTransformTool;

/////////////////////////////////////////////////////
// SOdysseyPainterEditorRasterTransformToolTopTab
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorRasterTransformToolTopTab : public SCompoundWidget
{
    typedef SCompoundWidget             tSuperClass;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyPainterEditorRasterTransformToolTopTab )
        {}
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs, UOdysseyPainterEditorRasterTransformTool* iTool);
    TSharedPtr<SWidget> CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle);

private:
    FReply OnFlipHorizontalClicked();
    FReply OnFlipVerticalClicked();

private:
    UOdysseyPainterEditorRasterTransformTool* mTool;
};

