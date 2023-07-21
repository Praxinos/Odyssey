// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tools/SOdysseyPainterEditorVectorEditionMode.h"
#include "ISinglePropertyView.h"

#define LOCTEXT_NAMESPACE "SOdysseyPainterEditorVectorEditionMode"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorVectorEditionMode
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

void
SOdysseyPainterEditorVectorEditionMode::Construct( const FArguments& InArgs, FOdysseyPainterEditor* iEditor )
{
    mEditor = iEditor;

    mObjectModeCheckbox =
    SNew(SCheckBox)
   .CheckedHoveredImage(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeObject32"))
   .CheckedImage(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeObject32"))
   .CheckedPressedImage(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeObject32"))
   .UncheckedHoveredImage(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeObject32"))
   .UncheckedImage(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeObject32"))
   .UncheckedPressedImage(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeObject32"))
   .OnCheckStateChanged(FOnCheckStateChanged::CreateSP(this, &SOdysseyPainterEditorVectorEditionMode::SetVectorEditionMode, eVectorEditionMode::Object))
   .IsChecked( mEditor->GetVectorEditionMode() == eVectorEditionMode::Object ? ECheckBoxState::Checked
                                                                             : ECheckBoxState::Unchecked );

    mVertexModeCheckbox =
    SNew(SCheckBox)
   .CheckedHoveredImage(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeVertex32"))
   .CheckedImage(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeVertex32"))
   .CheckedPressedImage(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeVertex32"))
   .UncheckedHoveredImage(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeVertex32"))
   .UncheckedImage(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeVertex32"))
   .UncheckedPressedImage(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeVertex32"))
   .OnCheckStateChanged(FOnCheckStateChanged::CreateSP(this, &SOdysseyPainterEditorVectorEditionMode::SetVectorEditionMode, eVectorEditionMode::Vertex))
   .IsChecked( mEditor->GetVectorEditionMode() == eVectorEditionMode::Vertex ? ECheckBoxState::Checked
                                                                             : ECheckBoxState::Unchecked );


    ChildSlot
    .VAlign(VAlign_Fill)
    .HAlign(HAlign_Fill)
    [
        SNew(SWrapBox)
        .InnerSlotPadding(FVector2D(10.f, 3.f))
        //.UseAllottedSize(true)
        //.HAlign(HAlign_Fill)
        + SWrapBox::Slot()
        //.HAlign(HAlign_Fill)
        [
            mObjectModeCheckbox.ToSharedRef()
        ]
        + SWrapBox::Slot()
        //.HAlign(HAlign_Fill)
        [
            mVertexModeCheckbox.ToSharedRef()
        ]
    ];
}

void
SOdysseyPainterEditorVectorEditionMode::SetVectorEditionMode( ECheckBoxState iNewState, eVectorEditionMode iVectorEditionMode )
{
    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mEditor->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

    mEditor->SetVectorEditionMode( iVectorEditionMode );

    if( iVectorEditionMode == eVectorEditionMode::Object )
    {
        mVertexModeCheckbox.Get()->SetIsChecked(ECheckBoxState::Unchecked);
    }

    if( iVectorEditionMode == eVectorEditionMode::Vertex )
    {
        mObjectModeCheckbox.Get()->SetIsChecked(ECheckBoxState::Unchecked);
    }

    if( mediaVectors.Num() )
    {
        FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
        FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

        vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
        vectorEngine->ResetHUD();
    }
}

#undef LOCTEXT_NAMESPACE

