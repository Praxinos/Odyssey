#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"

#include "OdysseyPainterEditorVectorTagInbetweenerView.generated.h"

class FOdysseyPainterEditor;

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

        void Update( FOdysseyPainterEditor* iEditor
                   , FOdysseyVectorGroupPaint* iScene
                   , const std::list<FOdysseyVectorTagInbetweener*>& iSelectedInbetweenerTagList );
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    protected:
        static uint64 GetSnapshotFlags( const FName& iPropertyName
                                      , const FName& iMemberPropertyName
                                      , const FName& iCategory );
        virtual void ImportParam();
        virtual void PropertyChanged( const FName& iPropertyName
                                    , const FName& iMemberPropertyName
                                    , const FName& iCategory );

    protected:
        FOdysseyPainterEditor* mEditor;
        FOdysseyVectorGroupPaint* mScene;
        std::vector<FOdysseyVectorTagInbetweener*> mSelectedInbetweenerTagArray;

    public:
        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Inbetween Count"
                          , ClampMin = "0"
                          , ClampMax = "16"
                          , UIMin    = "0"
                          , UIMax    = "16" ) )
        uint32 InbetweenCount;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Interpolation Type" ) )
        eInbetweenerInterpolationType InterpolationType;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Grid Type" ) )
        eInbetweenerGridType GridType;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Divisions X"
                          , DisplayName = "Divisions X"
                          , EditCondition = "( GridType == eInbetweenerGridType::FFD )"
                          , EditConditionHides
                          , ClampMin = "1"
                          , ClampMax = "8"
                          , UIMin    = "1"
                          , UIMax    = "8" ) )
        uint32 FFDDivisionX;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Divisions Y"
                          , DisplayName = "Divisions Y"
                          , EditCondition = "( GridType == eInbetweenerGridType::FFD )"
                          , EditConditionHides
                          , ClampMin = "1"
                          , ClampMax = "8"
                          , UIMin    = "1"
                          , UIMax    = "8" ) )
        uint32 FFDDivisionY;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Divisions X"
                          , DisplayName = "Divisions X"
                          , EditCondition = "( GridType == eInbetweenerGridType::ARAP )"
                          , EditConditionHides
                          , ClampMin = "1"
                          , ClampMax = "16"
                          , UIMin    = "1"
                          , UIMax    = "16" ) )
        uint32 ARAPDivisionX;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Divisions Y"
                          , DisplayName = "Divisions Y"
                          , EditCondition = "( GridType == eInbetweenerGridType::ARAP )"
                          , EditConditionHides
                          , ClampMin = "1"
                          , ClampMax = "16"
                          , UIMin    = "1"
                          , UIMax    = "16" ) )
        uint32 ARAPDivisionY;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Rigidity"
                          , EditCondition = "( GridType == eInbetweenerGridType::ARAP )"
                          , EditConditionHides
                          , ClampMin = "1"
                          , ClampMax = "100"
                          , UIMin    = "1"
                          , UIMax    = "100" ) )
        uint32 Rigidity;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Map Paths As Polyline" ) )
        bool MapPathsAsPolyline;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Color" ) )
        FColor Color;
};
