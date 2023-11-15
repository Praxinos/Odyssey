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
   .OnCheckStateChanged(FOnCheckStateChanged::CreateSP(this, &SOdysseyPainterEditorVectorEditionMode::SetVectorEditionFlags, FOdysseyVectorHUD::HUD_MODE_OBJECT ))
   .IsChecked( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT ? ECheckBoxState::Checked
                                                                                 : ECheckBoxState::Unchecked )
    [
        SNew(SImage)
       .Image(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeObject32"))
    ];

    mVertexModeCheckbox =
    SNew(SCheckBox)
   .Type(ESlateCheckBoxType::ToggleButton)
   .Style(FAppStyle::Get(),TEXT("ToggleButtonCheckBox"))
   .OnCheckStateChanged(FOnCheckStateChanged::CreateSP(this, &SOdysseyPainterEditorVectorEditionMode::SetVectorEditionFlags, FOdysseyVectorHUD::HUD_MODE_VERTEX ))
   .IsChecked( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX ? ECheckBoxState::Checked
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
SOdysseyPainterEditorVectorEditionMode::SetVectorEditionFlags( ECheckBoxState iNewState, uint64 iViewMode )
{
    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mEditor->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

    mEditor->SetVectorHUDFlags( ( mEditor->GetVectorHUDFlags() & (~FOdysseyVectorHUD::HUD_MODE_ALL) ) | iViewMode );

    if( iViewMode & FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        mObjectModeCheckbox.Get()->SetIsChecked( ECheckBoxState::Checked );
        mVertexModeCheckbox.Get()->SetIsChecked( ECheckBoxState::Unchecked );
    }

    if( iViewMode & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        mObjectModeCheckbox.Get()->SetIsChecked(ECheckBoxState::Unchecked);
        mVertexModeCheckbox.Get()->SetIsChecked( ECheckBoxState::Checked );
    }

    if( mediaVectors.Num() )
    {
        FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
        FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

        vectorEngine->ResetHUD();

        vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
    }
}

#undef LOCTEXT_NAMESPACE

