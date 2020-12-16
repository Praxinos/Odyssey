// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    // mutator.GetSourceFlipbook()->DefaultMaterial = ;

    UClass* flipbookClass = flipbook->StaticClass();
    FObjectProperty* defaultMaterialProperty = FindFProperty<FObjectProperty>(flipbookClass, "DefaultMaterial");
    defaultMaterialProperty->SetObjectPropertyValue(defaultMaterialProperty->ContainerPtrToValuePtr<UPaperFlipbook>(flipbook), LoadObject<UMaterialInterface>(nullptr, TEXT("/Iliad/Animation2D/DefaultFlipbookMaterialInstance.DefaultFlipbookMaterialInstance")));
    return flipbook;
}
