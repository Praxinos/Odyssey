// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/UObjectGlobals.h"
#include "InputCoreTypes.h"
#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyPaintEngine.h"

#include <functional>

#include "OdysseyPainterEditorRasterLiquifyTool.generated.h"

class FOdysseyPaintEngine;
class FOdysseyPainterEditorRasterLiquifyToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterLiquifyTool : public UOdysseyPainterEditorRasterBaseTool
{
    GENERATED_BODY()

    public:
        struct FDistortion
        {
            FDistortion()
            {
                mapped = false;
                coveredDistance = 0.0f;
            }

            bool mapped;
            ::ULIS::FVec2I coords;
            ::ULIS::FVec2I newCoords;
            double distanceToCenter;
            float angle;
            double coveredDistance;
        };

        struct FFlow
        {
            FFlow()
            {
                coveredDistance = 0.0f;
            }

            uint32 fromPathIndex;
            double coveredDistance;
            ::ULIS::FVec2I delta;
            ::ULIS::FVec2I debug_distortionCoords;
        };

        struct FPath
        {
            FPath( const ::ULIS::FVec2D& iPoint0, const ::ULIS::FVec2D& iPoint1, double iCoveredDistance )
            {
                ::ULIS::FVec2D vec = ( iPoint1 - iPoint0 );

                point[0] = iPoint0;
                point[1] = iPoint1;
                distance = vec.Distance();
                vector = vec.Normalized();
                coveredDistance = iCoveredDistance;
            }

            ::ULIS::FVec2D point[2];
            ::ULIS::FVec2D vector;
            double distance;
            double coveredDistance;
        };

        typedef std::function<void(FDistortion&,::ULIS::FVec2D&,::ULIS::FVec2D&)> FLiquifyFunction;

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
        void MakeDistortionMap();
        void MakeFlowMap();
        void Twirl( int32 iSrcCenterX
                  , int32 iSrcCenterY
                  , int32 iDstCenterX
                  , int32 iDstCenterY
                  , double iAngleInRadians );
        void Push( int32 iSrcCenterX
                 , int32 iSrcCenterY
                 , int32 iDstCenterX
                 , int32 iDstCenterY );
        ::ULIS::FVec2D GetRelativeCoords( const FFlow& iFlow );
        void Liquify( int32 iSrcCenterX
                    , int32 iSrcCenterY
                    , int32 iDstCenterX
                    , int32 iDstCenterY );
        void Flow( int32 iCenterX
                 , int32 iCenterY
                 , double iLength );
        inline void _Liquify_Push( int32 iSrcCenterX
                                 , int32 iSrcCenterY
                                 , int32 iDstCenterX
                                 , int32 iDstCenterY
                                 , FDistortion& distortion
                                 , ::ULIS::FVec2D& oSrcCoords
                                 , ::ULIS::FVec2D& oDstCoords );

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
                 , meta = ( ClampMin = "1"
                          , UIMin = "1"
                          , ClampMax = "2000"
                          , UIMax = "2000"
                          , LinearDeltaSensitivity = "5"
                          , Delta = "1"
                          , Multiple = "1" ) )
        int Radius;

    private:
        TSharedPtr<FOdysseyRasterBlock> GetRasterBlockFromEditor(bool iCreate) const;

        void OnRasterSelectionChanged();

    public:
        UPROPERTY(EditInstanceOnly, Category="Blending", meta=(ShowOnlyInnerProperties))
        FOdysseyBlendParameters BlendParameters;

    protected:
        // protected Data Members
        TSharedPtr<FOdysseyPainterEditorRasterLiquifyToolHUD> mLiquifyHUD;
        //Resources
        FOdysseyPaintEngine                 mPaintEngine;
        //TSharedPtr<IOdysseyInterpolation>   mInterpolator;
        float mBaseSize; //Size on which the tool is based to compute its size from a percentage

        TSharedPtr<FScopedTransaction> mTransaction;

        TArray<FDistortion> mDistortionMap;
        TArray<FFlow> mFlowMap;
        TArray<FPath> mPathBuffer;
        double mPathCoveredDistance;
        FVector2D mMouseAtDown;
        bool bIsMouseDown = false;
        FVector2D mPreviousPointInTexture;
        TArray<uint32> mSourcePixels;
};
