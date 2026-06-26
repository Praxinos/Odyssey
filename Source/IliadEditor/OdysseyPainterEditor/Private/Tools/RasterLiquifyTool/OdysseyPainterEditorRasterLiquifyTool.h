// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/UObjectGlobals.h"

#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "OdysseyRasterBlockMutator.h"
#include <ULIS>

#include "OdysseyPainterEditorRasterLiquifyTool.generated.h"

class FOdysseyPainterEditorRasterLiquifyToolHUD;
class FOdysseyRasterBlock;
class FTransaction;
class UTransBuffer;
class FOdysseyRasterBlockMutator;

UENUM()
enum class EOdysseyLiquifyTwirlDirection : uint8
{
    Clockwise,
    Counterclockwise
};

UENUM()
enum class EOdysseyLiquifyPushDirection : uint8
{
    Front,
    Left,
    Right
};

UENUM()
enum class EOdysseyLiquifyBorderPolicy : uint8
{
    None,
    Clamp
    // commented-out, this mode would be complicated to implmeent due to the lack of vector information
    // outside the canvas.
    // Repeat
};

UENUM()
enum class EOdysseyLiquifyMode : uint8
{
    Push,
    Twirl,
    Pinch,
    Expand,
    Crystals,
    Edge,
    Reconstruct,
    Adjust,
    Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EOdysseyLiquifyMode, EOdysseyLiquifyMode::Count)

USTRUCT(BlueprintType)
struct FStylusPressureOptions
{
    GENERATED_BODY()

    public:
        FStylusPressureOptions()
            : UseSize( false )
            , UseStrength( false )
            , UseHardness( false )
        {
        }

        /** Stylus pressure linked to size. */
        UPROPERTY( EditAnywhere, Category="Options",meta = ( DisplayName = "Size", EditCondition = "(UseStylusPressure == true)", EditConditionHides ) )
        bool UseSize;

        /** Stylus pressure linked to strength. */
        UPROPERTY( EditAnywhere, Category="Options", meta = ( DisplayName = "Strength", EditCondition = "(UseStylusPressure == true)", EditConditionHides ) )
        bool UseStrength;

        /** Stylus pressure linked to hardness. */
        UPROPERTY( EditAnywhere, Category="Options", meta = ( DisplayName = "Hardness", EditCondition = "(UseStylusPressure == true)", EditConditionHides ) )
        bool UseHardness;
};

UCLASS( HideCategories = "Hidden" )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterLiquifyTool : public UOdysseyPainterEditorRasterBaseTool
{
    GENERATED_BODY()

    public:
        // use floats instead of doubles to save some space. We may want to use double some day so we create
        // a type that we can change easily.
        typedef FVector2f FFlow;

        struct FAlteredImage
        {
            ~FAlteredImage();
            FAlteredImage( TSharedPtr<FOdysseyRasterBlock> iSourceRasterBlock
                         , TSharedPtr<::ULIS::FBlock> iMaskBlock );

            TSharedPtr<FOdysseyRasterBlock> sourceRasterBlock;
            TSharedPtr<::ULIS::FBlock> sourceBlock; // to prevent garbagde collection of FOdysseyRasterBlock::GetBlock()
            TSharedPtr<::ULIS::FBlock> sourceBlockCopy;
            TSharedPtr<::ULIS::FBlock> destinationBlock;
            TSharedPtr<::ULIS::FBlock> maskBlock;
            FOdysseyRasterBlockMutator mutator;
            ::ULIS::FContext& context;
        };

        struct FFlowMap
        {
            void Zero()
            {
                memset( &toTargetBuffer[0], 0, width * height );
            }

            void Empty()
            {
                toTargetBuffer.Empty();
            }

            void SetSize( uint32 iWidth, uint32 iHeight )
            {
                width = iWidth;
                height = iHeight;

                toTargetBuffer.Init( FFlow(0.f, 0.f), width * height );
            }

            // use floats instead of doubles to save some space
            TArray<FFlow> toTargetBuffer;
            uint32 width;
            uint32 height;
        };

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorRasterLiquifyTool();

        //Constructor
        UOdysseyPainterEditorRasterLiquifyTool();

    public:
        //OdysseyPainterEditorTool overrides
        virtual void Activate() override;
        virtual bool IsActivable() const override;
        virtual void Load() override;
        virtual void Unload() override;

        virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
        virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
        virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
        virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
        virtual bool OnKeyDown(const FKey& iKey) override;
        virtual bool OnKeyUp(const FKey& iKey) override;

        virtual void Tick(float iDeltaTime) override;

        virtual void Flush() override;
        virtual void Commit() override;

        virtual void BindShortcuts(TSharedPtr<FUICommandList> iCommandList) override;
        virtual void ExtendMenu( TSharedRef<FExtender> iExtender) override;
        virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;

        virtual FText GetTooltip() const override;
        virtual bool HasRadius() const override;
        virtual float GetRadius() const override;
        virtual void SetRadius(float iRadius)override;

        static void RegisterDetailCustomization();
        static void UnregisterDetailCustomization();
        FFlowMap& GetFlowMap();

    protected:
        virtual void GetMouseCursorImpl() const override;

    protected:
        double GetStrength();
        double GetHardness();
        void FetchSourceImages();
        void MakeDistortionMap(); // instant map
        void MakeFlowMap(); // cumulative map
        void ApplyAdjustment();
        void ApplyFlow( FAlteredImage& iAlteredImage
                      , const ::ULIS::FRectI& iRegionOfInterest
                      , bool iBilinearFiltered );

        void Flow( const FVector2D& iPrevCenter
                 , const FVector2D& iCurrCenter
                 , double iStrength
                 , double iHardness );
        void UpdateAlteredImage( FAlteredImage& iAlteredImage
                               , const ::ULIS::FRectI& iSanitizedRegionOfInterest
                               , bool iCommit );

    public:
        virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive) override;
        virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive) override;

        ::ULIS::FRectI GetEditingArea();
        void Init();

    public:
        /** The mode. */
        UPROPERTY( EditAnywhere, Category = "Liquify Tool" )
        EOdysseyLiquifyMode Mode;
        EOdysseyLiquifyMode mPreviousMode;

        /** Use Stylus Pressure. */
        UPROPERTY( EditAnywhere, Category = "Liquify Tool", meta = ( EditCondition = "(Mode != EOdysseyLiquifyMode::Adjust)", EditConditionHides ) )
        bool UseStylusPressure;

        /** Stylus Pressure Options. */
        UPROPERTY( EditAnywhere, Category = "Liquify Tool", meta = ( EditCondition = "(UseStylusPressure == true) && (Mode != EOdysseyLiquifyMode::Adjust)", EditConditionHides ) )
        FStylusPressureOptions StylusPressureOptions;

        /** The size. */
        UPROPERTY( EditAnywhere, Category = "Liquify Tool", meta = ( EditCondition = "(Mode != EOdysseyLiquifyMode::Adjust)", EditConditionHides, ClampMin = "0", UIMin = "0", ClampMax = "2000", UIMax = "2000" ) )
        int Size;

        /** The strength. */
        UPROPERTY( EditAnywhere, Category = "Liquify Tool", meta = ( EditCondition = "(Mode != EOdysseyLiquifyMode::Adjust)", EditConditionHides, ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100", Units = "Percent" ) )
        int32 Strength;

        /** The hardness. */
        UPROPERTY( EditAnywhere, Category = "Liquify Tool", meta = ( EditCondition = "(Mode != EOdysseyLiquifyMode::Adjust)", EditConditionHides, ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100", Units = "Percent" ) )
        int32 Hardness;

        /** Increase or decrease displacement. */
        UPROPERTY( EditAnywhere, Category = "Liquify Tool", meta = ( DisplayName = "Strength", EditCondition = "(Mode == EOdysseyLiquifyMode::Adjust)", EditConditionHides, ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100", Units = "Percent" ) )
        int32 AdjustmentStrength;

        /** The direction. */
        UPROPERTY( EditAnywhere, Category = "Liquify Tool", meta = ( EditCondition = "(Mode == EOdysseyLiquifyMode::Push)", EditConditionHides ) )
        EOdysseyLiquifyPushDirection PushDirection;

        /** The direction. */
        UPROPERTY( EditAnywhere, Category = "Liquify Tool", meta = ( EditCondition = "(Mode == EOdysseyLiquifyMode::Twirl)", EditConditionHides ) )
        EOdysseyLiquifyTwirlDirection TwirlDirection;

        /** The border policy. */
        UPROPERTY( EditAnywhere, Category = "Liquify Tool" )
        EOdysseyLiquifyBorderPolicy BorderPolicy;

        void Reset();

    private:
        TSharedPtr<FOdysseyRasterBlock> GetRasterBlockFromEditor(bool iCreate) const;

        void OnRasterSelectionChanged();
        uint32 GetThreadCount();

    protected:
        TSharedPtr<FOdysseyPainterEditorRasterLiquifyToolHUD> mLiquifyHUD;
        // We copy the source image and stores it into an array of source image
        // the array will be used when this tool will be made multi-layer compatible
        TArray<FAlteredImage> mAlteredImageBuffer;
        // Distortion Map temporarily stores the displacement vectors
        FFlowMap mDistortionMap;
        FFlowMap mFlowMap;
        // this flow map will be used for undos. It allows us to save only the portion that has changed
        FFlowMap mFlowMapBackup;
        FVector2D mMouseAtDown;
        FVector2D mMousePosition;
        bool bIsMouseLeftButtonDown = false;
        FVector2D mPreviousPointInTexture;
        ::ULIS::FRectI mEditingArea;
        ::ULIS::FRectI mActionArea;
        double mPressure;
        uint64 mPreviousTime;
};
