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
   .Type(ESlateCheckBoxType::ToggleButton)
   .Style(FAppStyle::Get(),TEXT("ToggleButtonCheckBox"))
   .OnCheckStateChanged(FOnCheckStateChanged::CreateSP(this, &SOdysseyPainterEditorVectorEditionMode::SetVectorEditionMode, eVectorEditionMode::Object))
   .IsChecked( mEditor->GetVectorEditionMode() == eVectorEditionMode::Object ? ECheckBoxState::Checked
                                                                             : ECheckBoxState::Unchecked )
    [
        SNew(SImage)
       .Image(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeObject32"))
    ];

    mVertexModeCheckbox =
    SNew(SCheckBox)
   .Type(ESlateCheckBoxType::ToggleButton)
   .Style(FAppStyle::Get(),TEXT("ToggleButtonCheckBox"))
   .OnCheckStateChanged(FOnCheckStateChanged::CreateSP(this, &SOdysseyPainterEditorVectorEditionMode::SetVectorEditionMode, eVectorEditionMode::Vertex))
   .IsChecked( mEditor->GetVectorEditionMode() == eVectorEditionMode::Vertex ? ECheckBoxState::Checked
                                                                             : ECheckBoxState::Unchecked )
    [
        SNew(SImage)
       .Image(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeVertex32"))
    ];


    ChildSlot
    .Padding(0)
    //.VAlign(VAlign_Fill)
    //.HAlign(HAlign_Fill)
    [
        SNew(SHorizontalBox)
        //.UseAllottedSize(true)
        //.HAlign(HAlign_Fill)
        + SHorizontalBox::Slot()
        .AutoWidth()
        //.HAlign(HAlign_Fill)
        [
            mObjectModeCheckbox.ToSharedRef()
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
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

        vectorEngine->ResetHUD();

        vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
    }
}

#undef LOCTEXT_NAMESPACE

