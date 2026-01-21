// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STileView.h"

#include "Tools/ArianeEditorTool.h"

class UArianeEditorTool;
class FArianeEditor;

class ARIANEEDITOR_API SArianeEditorToolSelector
    : public SCompoundWidget
{
    public:
        DECLARE_DELEGATE_OneParam( FOnToolSelected, UArianeEditorTool* );

    public:
        SLATE_BEGIN_ARGS(SArianeEditorToolSelector)
            {}
            SLATE_EVENT( FOnToolSelected, OnToolSelected )
        SLATE_END_ARGS()

    public:
        ~SArianeEditorToolSelector();
        SArianeEditorToolSelector();

        void Construct( const FArguments& InArgs, FArianeEditor* iEditor );

    private:
        void OnToolCheckStateChanged(ECheckBoxState InValue, UArianeEditorTool* iTool);
        EVisibility ToolVisibility(UArianeEditorTool* iTool) const;
        ECheckBoxState IsToolChecked(UArianeEditorTool* iTool) const;
        FText ToolTooltip(UArianeEditorTool* iTool) const;

    private:
        FOnToolSelected mOnToolSelected;
        FArianeEditor* mEditor;
};
