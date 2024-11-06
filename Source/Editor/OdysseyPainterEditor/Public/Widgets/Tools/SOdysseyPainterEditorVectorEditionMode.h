// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Tools/OdysseyPainterEditorTool.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateTypes.h" //ECheckBoxState
#include "Widgets/SCompoundWidget.h"

class SCheckBox;

class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorVectorEditionMode : public SCompoundWidget
{
    public:
        // Construction / Destruction
        SLATE_BEGIN_ARGS( SOdysseyPainterEditorVectorEditionMode )
            {}
        SLATE_ARGUMENT(FOdysseyPainterEditor*,Editor)
        SLATE_END_ARGS()

        void  Construct( const  FArguments&  InArgs, FOdysseyPainterEditor* iEditor );

    private:
        void SetVectorEditionFlags( ECheckBoxState iNewState, uint64 iViewMode );
        ECheckBoxState GetObjectModeState() const;
        ECheckBoxState GetVertexModeState() const;
        ECheckBoxState GetInbetweenModeState() const;
        EVisibility GetVisibility( uint64 iEditionMode ) const;

    private:
        FOdysseyPainterEditor* mEditor;
        TSharedPtr<SCheckBox> mObjectModeCheckbox;
        TSharedPtr<SCheckBox> mVertexModeCheckbox;
        TSharedPtr<SCheckBox> mInbetweenModeCheckbox;
};
