// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class FOdysseyBrushEditor;
class IDetailsView;

//////////////////////////////////////////////////////////////////////////
// SOdysseyBrushInternalExposed

class SOdysseyBrushInternalExposed
    : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SOdysseyBrushInternalExposed) { }
    SLATE_END_ARGS()

public:
    // Widget construction
    void Construct( const FArguments& InArgs, const TSharedRef<FOdysseyBrushEditor>& InEditor );
    void Refresh();

private:
    // Private data members
    // Pointer back to the Texture editor tool that owns us.
    TWeakPtr<FOdysseyBrushEditor> EditorPtr;

    /** Property View */
    TSharedPtr<class IDetailsView> DetailsView;
};
