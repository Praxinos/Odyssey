#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorEllipse.h"
#include "View/OdysseyVectorViewPath.h"

#include "OdysseyVectorViewEllipse.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorViewEllipse : public UOdysseyVectorViewPath
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyVectorViewEllipse();
        UOdysseyVectorViewEllipse();

    protected:
        virtual void ImportParam( FOdysseyVectorObject* iObject ) override;
        virtual void ExportParam( FOdysseyVectorObject* iObject ) override;
        virtual void PropertyChanged( const FName& iPropertyName, const FName& iCategory ) override;

    public:
        UPROPERTY(EditAnywhere, Category="Geometry")
        FEllipseParam EllipseParam; // UStruct from OdysseyVectorEllipse.h
};
