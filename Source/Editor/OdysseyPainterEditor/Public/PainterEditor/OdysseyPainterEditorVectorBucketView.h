#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorBucket.h"

#include "OdysseyPainterEditorVectorBucketView.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorBucketView : public UObject
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorBucketView();
        UOdysseyPainterEditorVectorBucketView();
        UOdysseyPainterEditorVectorBucketView( FOdysseyVectorBucket* iBucket );

        void Update( FOdysseyVectorBucket* iBucket );
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    protected:
        virtual void ImportParam();
        virtual void PropertyChanged( const FName& iPropertyName, const FName& iCategory );

    protected:
        FOdysseyVectorBucket* mBucket;

    public:
        UPROPERTY(EditAnywhere, Category="Bucket")
        FBucketParam BucketParam; // UStruct from OdysseyVectorBucket.h
};
