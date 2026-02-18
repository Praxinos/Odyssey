// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once
// Unreal headers
#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"
#include "IDetailsView.h"

class UArianeEditorTool;
class FArianeEditor;

/////////////////////////////////////////////////////
// SArianeEditorToolOptions
class ARIANEEDITOR_API SArianeEditorToolOptions
    : public SCompoundWidget
{
    public:
        // Construction / Destruction
        SLATE_BEGIN_ARGS( SArianeEditorToolOptions )
            {}
        SLATE_END_ARGS()

        void Construct( const  FArguments&  InArgs, FArianeEditor* iEditor );

    protected:
        void OnPostChangeCurrentTool();

    protected:
        TSharedPtr<IDetailsView> mDetailsView;
        FArianeEditor* mEditor;
};
