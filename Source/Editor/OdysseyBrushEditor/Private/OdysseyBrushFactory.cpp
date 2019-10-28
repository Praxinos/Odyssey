// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyBrushFactory.h"
#include "Modules/ModuleManager.h"
#include "Engine/Blueprint.h"
#include "OdysseyBrush.h"
#include "OdysseyBrushAssetBase.h"
#include "Kismet2/KismetEditorUtilities.h"

/////////////////////////////////////////////////////
// UOdysseyBrushFactory

UOdysseyBrushFactory::UOdysseyBrushFactory( const  FObjectInitializer&  ObjectInitializer )
    : Super( ObjectInitializer )
{
    UClass* DefaultParentClass = UOdysseyBrushAssetBase::StaticClass();

    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UOdysseyBrush::StaticClass();
    ParentClass = DefaultParentClass;
}


UObject*
UOdysseyBrushFactory::FactoryCreateNew( UClass* Class,
                                        UObject* InParent,
                                        FName Name,
                                        EObjectFlags Flags,
                                        UObject* Context,
                                        FFeedbackContext* Warn)
{
    // Make sure we are trying to factory a brush, then create and init one
    check( Class->IsChildOf( UOdysseyBrush::StaticClass() ) );
    return  FKismetEditorUtilities::CreateBlueprint(    ParentClass,
                                                        InParent,
                                                        Name,
                                                        BPTYPE_Normal,
                                                        UOdysseyBrush::StaticClass(),
                                                        UBlueprintGeneratedClass::StaticClass() );
}
