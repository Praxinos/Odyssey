// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"

#include "OdysseyPainterEditorVectorTagInbetweenerView.generated.h"

class FOdysseyPainterEditor;
class FOdysseyVectorUndo;


UENUM()
enum class eInbetweenerMappingMode : uint8
{
    KeepTopology = 0 UMETA( ToolTip = "Keep topology" ),
    Polyline     = 1 UMETA( ToolTip = "As polyline"   ),
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorTagInbetweenerView : public UObject
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorTagInbetweenerView();
        UOdysseyPainterEditorVectorTagInbetweenerView();
        UOdysseyPainterEditorVectorTagInbetweenerView( FOdysseyPainterEditor* iEditor
                                                     , FOdysseyVectorGroupPaint* iScene );

        bool Update( FOdysseyPainterEditor* iEditor
                   , FOdysseyVectorGroupPaint* iScene );
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    protected:
        static uint64 GetSnapshotFlags( const FName& iPropertyName
                                      , const FName& iMemberPropertyName
                                      , const FName& iCategory );
        virtual void ImportParam();
        virtual void PropertyChanged( const FName& iPropertyName
                                    , const FName& iMemberPropertyName
                                    , const FName& iCategory );
        FOdysseyVectorUndo* MakeUndo( const FName& iPropertyName
                                    , const FName& iMemberPropertyName
                                    , const FName& iCategory );

    protected:
        FOdysseyPainterEditor* mEditor;
        FOdysseyVectorGroupPaint* mScene;
        std::vector<FOdysseyVectorTagInbetweener*> mSelectedInbetweenerTagArray;
        bool SelectionHasRoutes();

    public:

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Interpolation Type" ) )
        eInbetweenerInterpolationType InterpolationType;


        //UPROPERTY( EditAnywhere
        //         , Category = "Inbetweener"
        //         , meta = ( ToolTip  = "Grid Type" ) )
        eInbetweenerGridType GridType;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Divisions X"
                          , DisplayName = "Divisions X"
                          , ClampMin = "1"
                          , ClampMax = "64"
                          , UIMin    = "1"
                          , UIMax    = "64"
                          // the second condition  "|| ( DivisionX != DivisionY )" is for legacy support
                          , EditCondition = "( Square == false ) || ( DivisionX != DivisionY )"
                          , EditConditionHides ) )
        uint32 DivisionX;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Divisions Y"
                          , DisplayName = "Divisions Y"
                          , ClampMin = "1"
                          , ClampMax = "64"
                          , UIMin    = "1"
                          , UIMax    = "64"
                          // the second condition  "|| ( DivisionX != DivisionY )" is for legacy support
                          , EditCondition = "( Square == false ) || ( DivisionX != DivisionY )"
                          , EditConditionHides ) )
        uint32 DivisionY;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Divisions"
                          , DisplayName = "Divisions"
                          , ClampMin = "1"
                          , ClampMax = "64"
                          , UIMin    = "1"
                          , UIMax    = "64"
                          // the second condition  "&& ( DivisionX == DivisionY ) " is for legacy support
                          , EditCondition = "( Square == true ) && ( DivisionX == DivisionY )"
                          , EditConditionHides ) )
        uint32 Divisions;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Map As Polyline" ) )
        eInbetweenerMappingMode MappingMode;

        //UPROPERTY( EditAnywhere
        //         , Category = "Inbetweener"
        //         , meta = ( ToolTip  = "With Thickness" ) )
        bool WithThickness;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Constant Width" ) )
        bool ConstantWidth;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Square Shaped" ) )
        bool Square;

        UPROPERTY( EditAnywhere
                 , Category = "Coloring"
                 , meta = ( DisplayName = "Inbetween"
                          , ToolTip  = "Inbetween Color" ) )
        FColor InbetweenColor;

        UPROPERTY( EditAnywhere
                 , Category = "Coloring"
                 , meta = ( DisplayName = "Chart"
                          , ToolTip  = "Chart Color" ) )
        FColor ChartColor;

        UPROPERTY( EditAnywhere
                 , Category = "Coloring"
                 , meta = ( DisplayName = "Grid"
                          , ToolTip  = "Grid Color" ) )
        FColor GridColor;

        UPROPERTY( EditAnywhere
                 , Category = "Coloring"
                 , meta = ( DisplayName = "Trajectory"
                          , ToolTip  = "Trajectory Color" ) )
        FColor TrajectoryColor;
};
