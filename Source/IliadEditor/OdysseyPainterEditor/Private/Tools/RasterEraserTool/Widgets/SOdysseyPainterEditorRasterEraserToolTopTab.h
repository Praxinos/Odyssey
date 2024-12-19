// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UOdysseyPainterEditorRasterEraserTool;
class ISinglePropertyView;

/////////////////////////////////////////////////////
// SOdysseyPainterEditorRasterEraserToolTopTab
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorRasterEraserToolTopTab : public SCompoundWidget
{
    typedef SCompoundWidget             tSuperClass;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyPainterEditorRasterEraserToolTopTab )
        {}
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs, UOdysseyPainterEditorRasterEraserTool* iTool);
    TSharedPtr<SWidget> CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle);

private:
    UOdysseyPainterEditorRasterEraserTool* mTool;
    TSharedPtr<ISinglePropertyView> mSizePropertyView;
    TSharedPtr<ISinglePropertyView> mFlowPropertyView;
    TSharedPtr<ISinglePropertyView> mOpacityPropertyView;
};
