// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UOdysseyPainterEditorRasterPaintBucketTool;

/////////////////////////////////////////////////////
// SOdysseyPainterEditorRasterPaintBucketToolTopTab
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorRasterPaintBucketToolTopTab : public SCompoundWidget
{
    typedef SCompoundWidget             tSuperClass;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyPainterEditorRasterPaintBucketToolTopTab )
        {}
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs, UOdysseyPainterEditorRasterPaintBucketTool* iTool);
    TSharedPtr<SWidget> CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle);

private:
    FReply OnEraserButtonClicked();
    FSlateColor GetEraserButtonColorAndOpacity() const;

private:
    UOdysseyPainterEditorRasterPaintBucketTool* mTool;
    TSharedPtr<ISinglePropertyView> mBlendParametersPropertyView;
};

