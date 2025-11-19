// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <ULIS>
#include "CoreMinimal.h"
#include "UObject/UObjectGlobals.h"
#include "InputCoreTypes.h"
#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyPaintEngine.h"
#include "IPropertyTypeCustomization.h"

#include "OdysseyPainterEditorRasterLiquifyTool.generated.h"

class FOdysseyPaintEngine;
class FOdysseyPainterEditorRasterLiquifyToolHUD;

UENUM()
enum class ELiquifyToolMode : uint8
{
    Push,
    Twirl,
    Pinch,
    Expand,
    Crystals,
    Edge,
    Reconstruct,
    Adjust
};

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

        UPROPERTY( EditAnywhere
                 , meta = ( DisplayName = "Size"
                          , ToolTip = "Stylus pressure influences size"
                          , EditCondition = "(UseStylusPressure == true)"
                          , EditConditionHides ) )
        bool UseSize;

        UPROPERTY( EditAnywhere
                 , meta = ( DisplayName = "Strength"
                          , ToolTip = "Stylus pressure influences strength"
                          , EditCondition = "(UseStylusPressure == true)"
                          , EditConditionHides ) )
        bool UseStrength;

        UPROPERTY( EditAnywhere
                 , meta = ( DisplayName = "Strength"
                          , ToolTip = "Stylus pressure influences hardness"
                          , EditCondition = "(UseStylusPressure == true)"
                          , EditConditionHides ) )
        bool UseHardness;
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterLiquifyTool : public UOdysseyPainterEditorRasterBaseTool
{
    GENERATED_BODY()

    public:
        struct FAlteredImage
        {
            ~FAlteredImage();
            FAlteredImage( TSharedPtr<FOdysseyRasterBlock> iSourceRasterBlock );

            TSharedPtr<::ULIS::FBlock> copiedSourceBlock;
            TSharedPtr<::ULIS::FBlock> destinationBlock;
            FOdysseyPaintEngine paintEngine;
            ::ULIS::FContext& context;
        };

        struct FDistortion
        {
            FDistortion()
            {
                mapped = false;
            }

            bool mapped;
            ::ULIS::FVec2I coords;
            double distanceToCenter;
            double angle;
            // TODO: try with floats instead of doubles
            FVector2D currToPrev; // temporarily store flow
        };

        struct FFlow
        {
            FFlow()
            : currToPrev ( 0.0f, 0.0f )
            {
            }

            // TODO: try with floats instead of doubles
            FVector2D currToPrev;
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

        virtual EMouseCursor::Type GetMouseCursor() const override;

        virtual FText GetTooltip() const override;

    protected:
        double GetStrength();
        double GetHardness();
        void FetchSourceImages();
        void MakeDistortionMap();
        void MakeFlowMap();
        void ApplyFlow( FAlteredImage& iAlteredImage
                      , const ::ULIS::FRectI& iRegionOfInterest
                      , bool iBilinearFiltered );

        void Flow( const FVector2D& iPrevCenter
                 , const FVector2D& iCurrCenter
                 , double iStrength
                 , double iHardness );

    public:
        virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive) override;
        virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive) override;

    public:
        // Paint Engine Stroke API

        //Begins a stroke at iPoint
        //Some value are computed from the last call to MoveTo(), like direction for example
        bool Begin(const FOdysseyPoint& iPoint);

        //Draws a Stroke from the last position to iPoint
        bool To(const FOdysseyPoint& iPoint);

        //Ends the stroke
        bool End();

    public:
        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( InlineEditConditionToggle ) )
        bool UseStylusPressure;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( ToolTip = "Increase or decrease displacement"
                          , EditCondition = "(UseStylusPressure == true)" ) )
        FStylusPressureOptions StylusPressureOptions;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( ClampMin = "1"
                          , UIMin = "1"
                          , ClampMax = "2000"
                          , UIMax = "2000"
                          , LinearDeltaSensitivity = "5"
                          , Delta = "1"
                          , Multiple = "1" ) )
        int Radius;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool" )
        ELiquifyToolMode Mode;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( Units = "Percent"
                          , ClampMin = "0"
                          , UIMin = "0"
                          , ClampMax = "100"
                          , UIMax = "100"
                          , Delta = "1"
                          , Multiple = "1" ) )
        int32 Strength;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( Units = "Percent"
                          , ClampMin = "0"
                          , UIMin = "0"
                          , ClampMax = "100"
                          , UIMax = "100"
                          , Delta = "1"
                          , Multiple = "1" ) )
        int32 Hardness;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool" )
        bool OnlyReferToEditngArea;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( DisplayName = "Strength"
                          , ToolTip = "Increase or decrease displacement"
                          , EditCondition = "(Mode == ELiquifyToolMode::Adjust)"
                          , EditConditionHides
                          , Units = "Percent"
                          , ClampMin = "0"
                          , UIMin = "0"
                          , ClampMax = "100"
                          , UIMax = "100"
                          , Delta = "1"
                          , Multiple = "1" ) )
        int32 AdjustmentStrength;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( InlineEditConditionToggle ) )
        bool bHasAngle;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( EditCondition = "bHasAngle"
                          , Units = "Degrees"
                          , ClampMin = "-180"
                          , UIMin = "-180"
                          , ClampMax = "180"
                          , UIMax = "180"
                          , Delta = "1"
                          , Multiple = "1"  ) )
        int32 Angle;

        UFUNCTION( BlueprintCallable
                 , Category = "Liquify Tool"
                 , CallInEditor )
        void Apply();

        UFUNCTION( BlueprintCallable
                 , Category = "Liquify Tool"
                 , CallInEditor )
        void Reset();

    private:
        TSharedPtr<FOdysseyRasterBlock> GetRasterBlockFromEditor(bool iCreate) const;

        void OnRasterSelectionChanged();

    protected:
        // protected Data Members
        TSharedPtr<FOdysseyPainterEditorRasterLiquifyToolHUD> mLiquifyHUD;
        //Resources


        TSharedPtr<FScopedTransaction> mTransaction;

        // We copy the source image and stores it into an array of source image
        // the array will be used when this tool will be made multi-layer compatible
        TArray<FAlteredImage> mAlteredImageArray;
        TArray<FDistortion> mDistortionMap;
        TArray<FFlow> mFlowMap;

        double mPathCoveredDistance;
        FVector2D mMouseAtDown;
        FVector2D mMousePosition;
        bool bIsMouseLeftButtonDown = false;
        FVector2D mPreviousPointInTexture;
        ::ULIS::FRectI mEditingArea;
        double mPressure;
};
