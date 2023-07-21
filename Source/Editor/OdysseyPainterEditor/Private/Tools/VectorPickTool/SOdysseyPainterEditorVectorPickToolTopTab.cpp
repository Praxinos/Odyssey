// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPickTool/SOdysseyPainterEditorVectorPickToolTopTab.h"
#include "Widgets/Tools/SOdysseyPainterEditorVectorEditionMode.h"
#include "ISinglePropertyView.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorPickToolTopTab"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorVectorPickToolTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

void
SOdysseyPainterEditorVectorPickToolTopTab::Construct( const FArguments& InArgs, UOdysseyPainterEditorVectorPickTool* iTool )
{
    mTool = iTool;

    ChildSlot
    .VAlign(VAlign_Fill)
    .HAlign(HAlign_Fill)
    [
        SNew(SWrapBox)
        .InnerSlotPadding(FVector2D(10.f, 3.f))
        .UseAllottedSize(true)
        .HAlign(HAlign_Fill)
        + SWrapBox::Slot()
        .HAlign(HAlign_Fill)
        [
            SNew(SOdysseyPainterEditorVectorEditionMode, mTool->GetEditor() )
        ]
    ];
}

#undef LOCTEXT_NAMESPACE

