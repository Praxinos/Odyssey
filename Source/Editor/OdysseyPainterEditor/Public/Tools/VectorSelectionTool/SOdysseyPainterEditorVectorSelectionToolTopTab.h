// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorVectorSelectionToolTopTab
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorVectorSelectionToolTopTab : public SCompoundWidget
{
    typedef SCompoundWidget             tSuperClass;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyPainterEditorVectorSelectionToolTopTab )
        {}
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs, UOdysseyPainterEditorVectorSelectionTool* iTool);
    TSharedPtr<SWidget> CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle, const TSharedPtr<ISinglePropertyView> iView);

private:
    UOdysseyPainterEditorVectorSelectionTool* mTool;
};
