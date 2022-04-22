// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/DrawingTool/Widgets/SOdysseyDrawingToolOptions.h"

#include "ObjectEditorUtils.h"
#include "PropertyEditorModule.h"
#include "ISinglePropertyView.h"
#include "Widgets/SOdysseyShapeSelector.h"
#include "Widgets/SOdysseyShape.h"

#define LOCTEXT_NAMESPACE "SOdysseyDrawingToolOptions"

/////////////////////////////////////////////////////
// SOdysseyDrawingToolOptions
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyDrawingToolOptions::Construct( const FArguments& InArgs )
{
    mTool = InArgs._Tool;

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    this->ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyShapeSelector)
            .SelectedShape(this, &SOdysseyDrawingToolOptions::GetSelectedShape)
            .OnShapeSelected(this, &SOdysseyDrawingToolOptions::OnShapeSelected)
        ]
        + SVerticalBox::Slot()//Step
        .AutoHeight()
        [
            PropertyEditorModule.CreateSingleProperty(mTool->GetBrushOptions(), "Step", FSinglePropertyParams()).ToSharedRef()

            /* SNew( SHorizontalBox )

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 3.f, 3.f )
            .MaxWidth( 45.f )
            [
                SNew( STextBlock )
                .Text( LOCTEXT( "Size", "Size :" ) )
            ]

            +SHorizontalBox::Slot()
            .VAlign( VAlign_Center )
            .Padding( 3.f, 3.f, 13.f, 3.f )
            [
                SAssignNew( mSizeSpinBox, SSpinBox< int >)
                .Value( this, &SOdysseyPaintModifiers::OnGetSize )
                .OnValueCommitted( this, &SOdysseyPaintModifiers::HandleSizeSpinBoxChanged )
                .OnValueChanged( this, &SOdysseyPaintModifiers::SetSize )
                .ShiftMouseMovePixelPerDelta( 15 )
                .Delta( 1 )
                .SliderExponent( 0.8f ) // Can't work properly if the following options are in use :  LinearDeltaSensitivity MinValue MaxValue
                .SliderExponentNeutralValue( 100 )
                .MinDesiredWidth( 100.0f ) // Depends on the size of the text in the previous slot
            ] */
        ]
        + SVerticalBox::Slot()//SizeAdaptative
        .AutoHeight()
        [
            PropertyEditorModule.CreateSingleProperty(mTool->GetBrushOptions(), "SizeAdaptative", FSinglePropertyParams()).ToSharedRef()
        ]
        + SVerticalBox::Slot()
        [
            SNew(SOdysseyShape)
            .Shape(this, &SOdysseyDrawingToolOptions::GetSelectedShapeInstance)
        ]
    ];
}

void
SOdysseyDrawingToolOptions::OnShapeSelected(EOdysseyShape iSelectedShape)
{
    FObjectEditorUtils::SetPropertyValue(mTool, "SelectedShape", iSelectedShape);
}

EOdysseyShape
SOdysseyDrawingToolOptions::GetSelectedShape() const
{
    return mTool->GetSelectedShape();
}

UOdysseyShape*
SOdysseyDrawingToolOptions::GetSelectedShapeInstance() const
{
    return mTool->GetSelectedShapeInstance();
}

#undef LOCTEXT_NAMESPACE

