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
        static const uint32 CHUNK_OBJECTS_DECLARE = 0;
        static const uint32 CHUNK_OBJECTS_DEFINE = 0;
        static const uint32 CHUNK_END = 0xFFFFFFFF;

        static const uint32 VECTOROBJECTTYPE = 0;
        static const uint32 VECTORPATHTYPE = 1;

        UOdysseyVectorRoot* mScene;
        FOdysseyVectorEngine* mVEngine;
        ::ULIS::FBlock* mBlock;

        static void OnInvalidBlock( const ::ULIS::FBlock* iBlock
                                  , const ::ULIS::FRectI* iRects
                                  , const uint32 iNumRects
                                  , void* iInfo );

        static void OnCleanupData( uint8* iData, void* iInfo );
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
        UOdysseyVectorRoot* GetScene();

        /**
         * @brief Renders an image in the given Block
         * Takes into account the size / format of the given block
         * 
         */
        virtual TArray<::ULIS::FEvent> RenderImage(::ULIS::FBlock* ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) override;
 
        void Serialize(FArchive& Ar);
};
