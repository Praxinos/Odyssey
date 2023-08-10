// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STreeView.h"

class FOdysseyVectorObject;

/**
 * Implements the Scene Tree View Widget
 */
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorVectorSceneTreeView
    : public STreeView<TSharedPtr<FOdysseyVectorObject>>
{
    public:
        SLATE_BEGIN_ARGS(SOdysseyPainterEditorVectorSceneTreeView)
            {}
        SLATE_END_ARGS()

    public:
        // Construction / Destruction
        ~SOdysseyPainterEditorVectorSceneTreeView();
        SOdysseyPainterEditorVectorSceneTreeView();
    
        void Construct(const FArguments& InArgs);

    protected:
        /**
         * @brief Called when the treeview asks for the children of a specific item
         * 
         * @param iParent 
         * @param oChildren 
         */
        void OnGetChildren(TSharedPtr<FOdysseyVectorObject> iParent, TArray<TSharedPtr<FOdysseyVectorObject>>& oChildren) const;
};
