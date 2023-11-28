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

class FOdysseyVectorBlock;

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
        FOdysseyVectorEngine* mEngine;
        TSharedPtr<FOdysseyVectorBlock> mVectorBlock; //A automatically cached block containing the render of mEngine

        void Init( uint32 iWidth, uint32 iHeight );

    public:
        ~UOdysseyTextureLayerImageVector();
        UOdysseyTextureLayerImageVector();

    protected:
        UPROPERTY()
        uint32 Width;

        UPROPERTY()
        uint32 Height;

        UPROPERTY()
        FGuid mVectorBlockId;

    public:
        //UOdysseyLayer overrides
        virtual void OnCreated_Implementation() override;
        virtual void PostInitProperties() override;
        virtual void PostLoad() override;

        FOdysseyVectorEngine* GetEngine();
 
        void OpacityChanged();
        void BlendModeChanged();
        void IsWireframeChanged();
        void IsColoredChanged();
        void Serialize(FArchive& Ar);
        virtual void PropertyChanged(const FName& iPropertyName) override;

        virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

    public:
        //FOdysseyImageRenderingAbility overrides
        virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
        virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType) const override;
        virtual ::ULIS::eBlendMode GetImageRenderingBlendMode() const override;
        virtual float GetImageRenderingOpacity() const override;

    private:
        void OnVectorBlockInvalidated(bool iIsInteractive);

    private:
        //Import/Export
        friend class FOdysseyTextureLayerImageVectorExport;
        friend class FOdysseyTextureLayerImageVectorImport;

    public:
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Texture | LayerStack")
        bool IsWireframe = false;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Texture | LayerStack")
        bool IsColored = true;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture | LayerStack")
	    EOdysseyBlendingMode BlendMode = EOdysseyBlendingMode::kNormal;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture | LayerStack")
        float Opacity = 1.0f;
};
