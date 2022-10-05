// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectMacros.h"


#include "OdysseyTextureImportFactory.generated.h"

UCLASS()
class UOdysseyTextureImportFactory 
    : public UFactory
{
    GENERATED_UCLASS_BODY()

private:
    virtual bool ConfigureProperties() override;

    virtual UObject* FactoryCreateBinary(UClass* Class,UObject* InParent,FName Name,EObjectFlags Flags,UObject* Context,const TCHAR* Type,const uint8*& Buffer,const uint8* BufferEnd,FFeedbackContext* Warn) override;
    
    virtual bool FactoryCanImport(const FString& Filename) override;
};
