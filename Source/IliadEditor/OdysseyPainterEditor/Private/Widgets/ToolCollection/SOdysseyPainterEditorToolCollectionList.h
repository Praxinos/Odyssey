// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Implements the Tool collection widget list
 */
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorToolCollectionList
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorToolCollectionList)
    {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPainterEditorToolCollectionList();
    SOdysseyPainterEditorToolCollectionList();

    void Construct(const FArguments& InArgs);


};
