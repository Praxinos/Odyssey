// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/OdysseyTextureLayer.h"
#include "Image/OdysseyBlendingMode.h"
#include "Misc/TransactionObjectEvent.h"
#include "Misc/ITransaction.h"
#include "Misc/ITransactionObjectAnnotation.h"

#include <ULIS>

#include "OdysseyVector.h"

#include "OdysseyTextureLayerImageVector.generated.h"

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayerImageVector
    : public UOdysseyTextureLayer
{
    GENERATED_BODY()

    public:
        /**
         * @brief Delegate called when something changed the result of RenderImage()
         * 
         */
        DECLARE_MULTICAST_DELEGATE_OneParam(FOnBlendModeChanged, UOdysseyTextureLayerImageVector*)

        /**
         * @brief Delegate called when something changed the result of RenderImage()
         * 
         */
        DECLARE_MULTICAST_DELEGATE_OneParam(FOnOpacityChanged, UOdysseyTextureLayerImageVector*)

    public:
        static FOnBlendModeChanged& OnBlendModeChanged();
        static FOnOpacityChanged& OnOpacityChanged();


    private:
        // handle to a callback to refresh the layer when a property of an object's details view is changed
        //FDelegateHandle mOnRefreshHandle;
        //FOdysseyVectorScene* mScene;
        FOdysseyVectorEngine* mEngine;
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
        //FOdysseyVectorScene* GetScene();

        /**
         * @brief Renders an image in the given Block
         * Takes into account the size / format of the given block
         * 
         */
        virtual TArray<::ULIS::FEvent> RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) override;
 
        virtual void RenderImageChanged( const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive ) override;
        virtual void RenderImageChanged(bool iIsInteractive) override;
        void OpacityChanged();
        void BlendModeChanged();
        void IsColoredChanged();
        void Serialize(FArchive& Ar);
        virtual void PropertyChanged(const FName& iPropertyName) override;

        virtual FOdysseyMediaProvider GetMediaProvider() const override;

    public:
        // Event Listeners
        void OnRefresh(FOdysseyVectorScene* iScene);

    public:
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Texture | LayerStack")
        bool IsColored = true;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture | LayerStack")
	    EOdysseyBlendingMode BlendMode = EOdysseyBlendingMode::kNormal;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture | LayerStack")
        float Opacity = 1.0f;
};
