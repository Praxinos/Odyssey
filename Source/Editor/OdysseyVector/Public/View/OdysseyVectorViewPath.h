#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPath.h"
#include "View/OdysseyVectorViewObject.h"

#include "OdysseyVectorViewPath.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorViewPath : public UOdysseyVectorViewObject
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyVectorViewPath();
        UOdysseyVectorViewPath();

    protected:
        virtual void ImportParam( FOdysseyVectorObject* iObject ) override;
        virtual void ExportParam( FOdysseyVectorObject* iObject ) override;
        virtual void PropertyChanged( const FName& iPropertyName, const FName& iCategory ) override;

    public:
        UPROPERTY(EditAnywhere, Category="Path")
        FPathParam PathParam; // UStruct from OdysseyVectorPath.h
};
