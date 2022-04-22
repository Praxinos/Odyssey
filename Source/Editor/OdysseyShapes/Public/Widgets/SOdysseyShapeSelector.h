// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

/////////////////////////////////////////////////////
// SOdysseyShapeSelector
class ODYSSEYSHAPES_API SOdysseyShapeSelector
    : public SCompoundWidget
{
    typedef SCompoundWidget         tSuperClass;
    typedef SOdysseyShapeSelector   tSelf;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyShapeSelector )
        {}
        SLATE_ARGUMENT(UObject*, Tool)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

private:
    // Private data members
    UObject*                       mTool;

    TSharedPtr<IDetailsView>            DetailsView;
    //TSharedPtr< IStructureDetailsView > DetailsView;
    TSharedPtr< FStructOnScope >        StructToDisplay;
};

