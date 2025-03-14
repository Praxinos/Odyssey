// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "ISinglePropertyView.h"

/////////////////////////////////////////////////////
// SOdysseySinglePropertyView
class ODYSSEYWIDGETS_API SOdysseySinglePropertyView : public SCompoundWidget
{
    typedef SCompoundWidget             tSuperClass;

    DECLARE_DELEGATE_RetVal_OneParam(TSharedPtr<class IPropertyHandle>, FOnOverridePropertyHandle, TSharedPtr<class IPropertyHandle>)

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseySinglePropertyView )
        : _InnerPadding(0.f)
        {}
        SLATE_ARGUMENT(float, InnerPadding)
        SLATE_ATTRIBUTE(FOptionalSize, ValueWidthOverride)
        SLATE_EVENT(FOnOverridePropertyHandle, OnOverridePropertyHandle)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs, UObject* iObject, const FName& iPropertyName, const FSinglePropertyParams& iParams);

private:
    TSharedPtr<ISinglePropertyView> mPropertyView;
};
