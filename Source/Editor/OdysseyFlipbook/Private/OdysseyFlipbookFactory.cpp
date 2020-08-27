// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyFlipbookFactory.h"

#include "PaperFlipbook.h"
#include "OdysseyFlipbook.h"

//---

/////////////////////////////////////////////////////
// UOdysseyFlipbookFactory
UOdysseyFlipbookFactory::UOdysseyFlipbookFactory( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    SupportedClass = UOdysseyFlipbook::StaticClass();
}

UObject*
UOdysseyFlipbookFactory::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(UPaperFlipbookFactory::FactoryCreateNew( UPaperFlipbook::StaticClass(), iParent, iName, iFlags, iContext, iWarn ));
    FScopedFlipbookMutator mutator(flipbook);
	mutator.FramesPerSecond = 24.0f;
    return flipbook;
}
