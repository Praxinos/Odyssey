// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseyShape.h"

/////////////////////////////////////////////////////
// SOdysseyShapeSelector

class ODYSSEYSHAPES_API SOdysseyShapeSelector
    : public SCompoundWidget
{
    typedef SCompoundWidget         tSuperClass;
    typedef SOdysseyShapeSelector   tSelf;

public:
    DECLARE_DELEGATE_OneParam(FOnShapeSelected, EOdysseyShape)

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyShapeSelector )
        {}
        SLATE_ATTRIBUTE(EOdysseyShape, SelectedShape)
        SLATE_EVENT(FOnShapeSelected, OnShapeSelected)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

private:
    void OnCheckStateChanged(ECheckBoxState iState, EOdysseyShape iShape);
    ECheckBoxState IsChecked(EOdysseyShape iShape) const;
    

private:
    // Private data members
    TAttribute<EOdysseyShape>           mSelectedShape;
    FOnShapeSelected                    mOnShapeSelected;
};

