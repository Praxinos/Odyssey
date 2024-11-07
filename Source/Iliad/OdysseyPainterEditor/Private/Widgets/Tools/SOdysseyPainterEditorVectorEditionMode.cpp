// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tools/SOdysseyPainterEditorVectorEditionMode.h"
#include "ISinglePropertyView.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h" //ChildSlot

/////////////////////////////////////////////////////
// SOdysseyPainterEditorVectorEditionMode
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

#define LOCTEXT_NAMESPACE "PainterEditor"

ECheckBoxState
SOdysseyPainterEditorVectorEditionMode::GetObjectModeState() const
{
    return mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT ? ECheckBoxState::Checked
                                                                             : ECheckBoxState::Unchecked;
}

ECheckBoxState
SOdysseyPainterEditorVectorEditionMode::GetVertexModeState() const
{
    return mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX ? ECheckBoxState::Checked
                                                                             : ECheckBoxState::Unchecked;
}

void
SOdysseyPainterEditorVectorEditionMode::Construct( const FArguments& InArgs, FOdysseyPainterEditor* iEditor )
{
    TAttribute<ECheckBoxState> objectModeChecked( this, &SOdysseyPainterEditorVectorEditionMode::GetObjectModeState );
    TAttribute<ECheckBoxState> vertexModeChecked( this, &SOdysseyPainterEditorVectorEditionMode::GetVertexModeState ) ;

    mEditor = iEditor;

    mObjectModeCheckbox =
    SNew(SCheckBox)
   .Type(ESlateCheckBoxType::ToggleButton)
   .Style(FAppStyle::Get(),TEXT("ToggleButtonCheckBox"))
   .OnCheckStateChanged(FOnCheckStateChanged::CreateSP(this, &SOdysseyPainterEditorVectorEditionMode::SetVectorEditionFlags, static_cast<uint64>(FOdysseyVectorHUD::HUD_MODE_OBJECT) ))
   .IsChecked( objectModeChecked )
   .ToolTipText(LOCTEXT("vector-edition-mode.object-mode.tooltip", "Object Mode"))
    [
        SNew(SImage)
       .Image(FOdysseyStyle::GetBrush("PainterEditor.TopBar.VectorModeObject32"))
    ];

    mVertexModeCheckbox =
    SNew(SCheckBox)
   .Type(ESlateCheckBoxType::ToggleButton)
   .Style(FAppStyle::Get(),TEXT("ToggleButtonCheckBox"))
   .OnCheckStateChanged(FOnCheckStateChanged::CreateSP(this, &SOdysseyPainterEditorVectorEditionMode::SetVectorEditionFlags, static_cast<uint64>(FOdysseyVectorHUD::HUD_MODE_VERTEX) ))
   .IsChecked( vertexModeChecked )
   .ToolTipText(LOCTEXT("vector-edition-mode.vertex-mode.tooltip", "Vertex Mode"))
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

    if( mediaVectors.Num() )
    {
        FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
        FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

        vectorEngine->ResetHUD();

        vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
    }
}

#undef LOCTEXT_NAMESPACE
