// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/OdysseyTextureLayer.h"
#include "Image/OdysseyBlendingMode.h"
#include "Misc/TransactionObjectEvent.h"
#include "Misc/ITransaction.h"
#include "Misc/ITransactionObjectAnnotation.h"

#include <ULIS>

#include "OdysseyVectorEngine.h"

#include "OdysseyTextureLayerImageVector.generated.h"

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayerImageVector
    : public UOdysseyTextureLayer
{
    GENERATED_BODY()

    private:
        UOdysseyVectorScene* mScene;
        FOdysseyVectorEngine* mVEngine;
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mBlock;

        void Init( uint32 iWidth, uint32 iHeight );

    public:
        ~UOdysseyTextureLayerImageVector();
        UOdysseyTextureLayerImageVector();

    protected:
        UPROPERTY()
        uint32 Width;

        UPROPERTY()
        uint32 Height;

    public:
        //UOdysseyLayer overrides
        virtual void OnCreated_Implementation() override;

        FOdysseyVectorEngine* GetEngine();
        UOdysseyVectorScene* GetScene();

        /**
         * @brief Renders an image in the given Block
         * Takes into account the size / format of the given block
         * 
         */
        virtual TArray<::ULIS::FEvent> RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) override;
 
        void Serialize(FArchive& Ar);
};
