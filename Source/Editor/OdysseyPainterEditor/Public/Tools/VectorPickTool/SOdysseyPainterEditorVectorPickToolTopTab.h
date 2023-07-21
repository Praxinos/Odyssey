// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickTool.h"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorVectorPickToolTopTab
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorVectorPickToolTopTab : public SCompoundWidget
{
    typedef SCompoundWidget             tSuperClass;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyPainterEditorVectorPickToolTopTab )
        {}
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs, UOdysseyPainterEditorVectorPickTool* iTool);
    TSharedPtr<SWidget> CreatePropertyWidget(TSharedPtr<class IPropertyHandle> iPropertyHandle, const TSharedPtr<ISinglePropertyView> iView);

private:
    UOdysseyPainterEditorVectorPickTool* mTool;
};
