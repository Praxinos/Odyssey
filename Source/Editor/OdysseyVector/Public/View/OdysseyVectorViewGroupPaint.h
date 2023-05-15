#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorGroupPaint.h"
#include "View/OdysseyVectorViewObject.h"

#include "OdysseyVectorViewGroupPaint.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorViewGroupPaint : public UOdysseyVectorViewObject
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyVectorViewGroupPaint();
        UOdysseyVectorViewGroupPaint();

    protected:
        virtual void ImportParam( FOdysseyVectorObject* iObject ) override;
        virtual void ExportParam( FOdysseyVectorObject* iObject ) override;
        virtual void PropertyChanged( const FName& iPropertyName, const FName& iCategory ) override;

    public:
        UPROPERTY(EditAnywhere, Category="Paint Group")
        FGroupPaintParam GroupPaintParam; // UStruct from OdysseyVectorGroupPaint.h
};
