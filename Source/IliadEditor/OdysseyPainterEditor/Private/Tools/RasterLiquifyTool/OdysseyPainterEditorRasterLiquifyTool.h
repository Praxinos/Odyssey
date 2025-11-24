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
#include "OdysseyLiquifyMode.h"

#include "OdysseyPainterEditorRasterLiquifyTool.generated.h"

class FOdysseyPaintEngine;
class FOdysseyPainterEditorRasterLiquifyToolHUD;

UENUM()
enum class EOdysseyLiquifyTwirlDirection : uint8
{
    Clockwise,
    Counterclockwise
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

            FFlow currToPrev; // temporarily store flow
        };

        struct FFlowMap
        {
            void Empty()
            {
                currToPrevBuffer.Empty();
            }

            void SetSize( uint32 iWidth, uint32 iHeight )
            {
                width = iWidth;
                height = iHeight;

                currToPrevBuffer.SetNum( width * height );
            }

            // use floats instead of doubles to save some space
            TArray<FFlow> currToPrevBuffer;
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
                 , Category = "Liquify Tool" )
        FOdysseyLiquifyMode Mode;

        // hidden property for use with EditCondition
        UPROPERTY( EditDefaultsOnly
                 , Category = Hidden )
        EOdysseyLiquifyMode mHiddenModeAsEnum;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( ToolTip = "Use Stylus Pressure"
                          , EditCondition = "(mHiddenModeAsEnum != EOdysseyLiquifyMode::Adjust)"
                          , EditConditionHides ) )
        bool UseStylusPressure;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( ToolTip = "Stylus Pressure Options"
                          , EditCondition = "(UseStylusPressure == true) && (mHiddenModeAsEnum != EOdysseyLiquifyMode::Adjust)"
                          , EditConditionHides ) )
        FStylusPressureOptions StylusPressureOptions;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( ToolTip = "Radius"
                          , EditCondition = "(mHiddenModeAsEnum != EOdysseyLiquifyMode::Adjust)"
                          , EditConditionHides
                          , ClampMin = "1"
                          , UIMin = "1"
                          , ClampMax = "2000"
                          , UIMax = "2000"
                          , LinearDeltaSensitivity = "5"
                          , Delta = "1"
                          , Multiple = "1" ) )
        int Radius;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( ToolTip = "Strength"
                          , EditCondition = "(mHiddenModeAsEnum != EOdysseyLiquifyMode::Adjust)"
                          , EditConditionHides
                          , Units = "Percent"
                          , ClampMin = "0"
                          , UIMin = "0"
                          , ClampMax = "100"
                          , UIMax = "100"
                          , Delta = "1"
                          , Multiple = "1" ) )
        int32 Strength;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( ToolTip = "Hardness"
                          , EditCondition = "(mHiddenModeAsEnum != EOdysseyLiquifyMode::Adjust)"
                          , EditConditionHides
                          , Units = "Percent"
                          , ClampMin = "0"
                          , UIMin = "0"
                          , ClampMax = "100"
                          , UIMax = "100"
                          , Delta = "1"
                          , Multiple = "1" ) )
        int32 Hardness;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( DisplayName = "Strength"
                          , ToolTip = "Increase or decrease displacement"
                          , EditCondition = "(mHiddenModeAsEnum == EOdysseyLiquifyMode::Adjust)"
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
                 , meta = ( ToolTip = "Has Angle"
                          , InlineEditConditionToggle ) )
        bool bHasAngle;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( ToolTip = "Angle"
                          , EditCondition = "bHasAngle && (mHiddenModeAsEnum == EOdysseyLiquifyMode::Push)"
                          , EditConditionHides
                          , Units = "Degrees"
                          , ClampMin = "-180"
                          , UIMin = "-180"
                          , ClampMax = "180"
                          , UIMax = "180"
                          , Delta = "1"
                          , Multiple = "1"  ) )
        int32 Angle;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( ToolTip = "Direction"
                          , EditCondition = "(mHiddenModeAsEnum == EOdysseyLiquifyMode::Twirl)"
                          , EditConditionHides ) )
        EOdysseyLiquifyTwirlDirection TwirlDirection;

        UPROPERTY( EditAnywhere
                 , Category = "Liquify Tool"
                 , meta = ( ToolTip = "OnlyReferToEditngArea"
                          , EditCondition = "(mHiddenModeAsEnum != EOdysseyLiquifyMode::Adjust)"
                          , EditConditionHides ) )
        bool OnlyReferToEditngArea;

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
        FFlowMap mFlowMap;
        // this flow map will be used for undos. It allows us to save only the portion that has changed
        FFlowMap mFlowMapBackup;
        double mPathCoveredDistance;
        FVector2D mMouseAtDown;
        FVector2D mMousePosition;
        bool bIsMouseLeftButtonDown = false;
        FVector2D mPreviousPointInTexture;
        ::ULIS::FRectI mEditingArea;
        double mPressure;
};
