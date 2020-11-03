// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Misc/NotifyHook.h"
#include "Framework/SlateDelegates.h"
#include "IStructureDetailsView.h"

#include "SOdysseyPerformanceOptions.generated.h"

/////////////////////////////////////////////////////
// FOdysseyPerformanceOptions
USTRUCT()
struct ODYSSEYWIDGETS_API FOdysseyPerformanceOptions
{
	GENERATED_BODY()

	virtual ~FOdysseyPerformanceOptions()
	{

	}

    FOdysseyPerformanceOptions()
        :  DrawBrushPreview     (   true    )
        ,  CacheEditLock        (   false   )
        ,  SuperSize            (   0       )
        ,  SuperCount           (   0       )
        ,  StateSize            (   0       )
        ,  StateCount           (   0       )
        ,  StrokeSize           (   0       )
        ,  StrokeCount          (   0       )
        ,  SubstrokeSize        (   0       )
        ,  SubstrokeCount       (   0       )
        ,  StepSize             (   0       )
        ,  StepCount            (   0       )
    {}
    
    /** Should  we draw a custom cursor representing the preview of the brush  */
    //UPROPERTY( EditAnywhere, Category = "Performances" )
    bool    DrawBrushPreview;
    
    /** EditLock. */
    UPROPERTY()
    bool    CacheEditLock;

    /** Super Cache size as Bytes. */
    UPROPERTY( VisibleAnywhere, Category = "Super", meta = ( EditCondition = CacheEditLock ) )
    int32   SuperSize;

    /** Super Cache object count. */
    UPROPERTY( VisibleAnywhere, Category = "Super", meta = ( EditCondition = CacheEditLock ) )
    int32   SuperCount;

    /** Super Cache size as Bytes. */
    UPROPERTY( VisibleAnywhere, Category = "State", meta = ( EditCondition = CacheEditLock ) )
    int32   StateSize;

    /** Super Cache object count. */
    UPROPERTY( VisibleAnywhere, Category = "State", meta = ( EditCondition = CacheEditLock ) )
    int32   StateCount;

    /** Super Cache size as Bytes. */
    UPROPERTY( VisibleAnywhere, Category = "Stroke", meta = ( EditCondition = CacheEditLock ) )
    int32   StrokeSize;

    /** Super Cache object count. */
    UPROPERTY( VisibleAnywhere, Category = "Stroke", meta = ( EditCondition = CacheEditLock ) )
    int32   StrokeCount;

    /** Super Cache size as Bytes. */
    UPROPERTY( VisibleAnywhere, Category = "Substroke", meta = ( EditCondition = CacheEditLock ) )
    int32   SubstrokeSize;

    /** Super Cache object count. */
    UPROPERTY( VisibleAnywhere, Category = "Substroke", meta = ( EditCondition = CacheEditLock ) )
    int32   SubstrokeCount;

    /** Super Cache size as Bytes. */
    UPROPERTY( VisibleAnywhere, Category = "Step", meta = ( EditCondition = CacheEditLock ) )
    int32   StepSize;

    /** Super Cache object count. */
    UPROPERTY( VisibleAnywhere, Category = "Step", meta = ( EditCondition = CacheEditLock ) )
    int32   StepCount;

	DECLARE_DELEGATE_OneParam(FOnDrawBrushPreviewChanged, bool)
	DECLARE_DELEGATE_OneParam(FOnAnyValueChanged, bool)

	FOnDrawBrushPreviewChanged mOnDrawBrushPreviewChanged;
	FOnAnyValueChanged mOnAnyValueChanged;

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged);
	
	virtual const UStruct* GetScriptStruct() {
		return FOdysseyPerformanceOptions::StaticStruct();
	}
};


/////////////////////////////////////////////////////
// SOdysseyPerformanceOptions
class ODYSSEYWIDGETS_API SOdysseyPerformanceOptions
    : public SCompoundWidget
    , public FNotifyHook
{
    typedef SCompoundWidget             tSuperClass;
    typedef SOdysseyPerformanceOptions  tSelf;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyPerformanceOptions )
        {}
        SLATE_ATTRIBUTE(FOdysseyPerformanceOptions*, PerformanceOptions)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

public:
    // FNotifyHook Interface
    virtual void NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

private:
    // Private data members
    TSharedPtr< IStructureDetailsView > PerformanceOptionsDetailsView;
    FOdysseyPerformanceOptions*         PerformanceOptionsStructData;
    TSharedPtr< FStructOnScope >        PerformanceOptionsStructToDisplay;
};

