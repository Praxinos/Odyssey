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
/*
        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Drawing Count"
                          , ClampMin = "2"
                          , ClampMax = "16"
                          , UIMin    = "2"
                          , UIMax    = "16" ) )
        uint32 DrawingCount;
*/
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
                          , ClampMin = "1"
                          , ClampMax = "32"
                          , UIMin    = "1"
                          , UIMax    = "32" ) )
        uint32 DivisionX;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Divisions Y"
                          , DisplayName = "Divisions Y"
                          , ClampMin = "1"
                          , ClampMax = "32"
                          , UIMin    = "1"
                          , UIMax    = "32" ) )
        uint32 DivisionY;
/*
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
*/
        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Map As Polyline" ) )
        bool MapAsPolyline;

        UPROPERTY( EditAnywhere
                 , Category = "Inbetweener"
                 , meta = ( ToolTip  = "Color" ) )
        FColor Color;
};
