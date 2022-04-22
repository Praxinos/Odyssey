// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/Widgets/SOdysseyToolOptions.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"


#define LOCTEXT_NAMESPACE "SOdysseyToolOptions"

/////////////////////////////////////////////////////
// SOdysseyToolOptions
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyToolOptions::Construct( const FArguments& InArgs )
{
    mTool = InArgs._Tool;
    mCurrentTool = nullptr;
    this->ChildSlot
    [
        SAssignNew(mToolSlot, SBorder)
        [
            SNullWidget::NullWidget
        ]
    ];
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides

void
SOdysseyToolOptions::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    UOdysseyTool* tool = mTool.Get();
    if (tool != mCurrentTool)
    {
        mCurrentTool = tool;
        mToolSlot->SetContent(tool->GetWidget().ToSharedRef());
    }
}

#undef LOCTEXT_NAMESPACE

