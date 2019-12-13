// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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

    FOdysseyPerformanceOptions()
        :  LiveUpdate           (   true    )
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

    // Short member list:
    //  bool    LiveUpdate          ;
    //  bool    DelayedPaint        ;
    //  int32   Delay               ;
    //  bool    MultiThreadedPaint  ;

    /** Should the texture live-stream update in the 3D viewport ( can impact performances ). */
    UPROPERTY( EditAnywhere, Category = "Performances" )
    bool    LiveUpdate;

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
        SLATE_EVENT( FOnBooleanValueChanged ,   OnLiveUpdateChanged     )
        SLATE_EVENT( FOnBooleanValueChanged ,   OnAnyValueChanged       )
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

public:
    // Public Callbacks
    const  FOdysseyPerformanceOptions&  GetPerformanceOptions()  const;
    void  SetPerformanceOptions( const  FOdysseyPerformanceOptions& iValue );

    void  SetPerformanceOptionLiveUpdate        ( bool iValue );

    void  SetPeformanceCacheInfoSuperSize       ( int32 iValue );
    void  SetPeformanceCacheInfoSuperCount      ( int32 iValue );
    void  SetPeformanceCacheInfoStateSize       ( int32 iValue );
    void  SetPeformanceCacheInfoStateCount      ( int32 iValue );
    void  SetPeformanceCacheInfoStrokeSize      ( int32 iValue );
    void  SetPeformanceCacheInfoStrokeCount     ( int32 iValue );
    void  SetPeformanceCacheInfoSubstrokeSize   ( int32 iValue );
    void  SetPeformanceCacheInfoSubstrokeCount  ( int32 iValue );
    void  SetPeformanceCacheInfoStepSize        ( int32 iValue );
    void  SetPeformanceCacheInfoStepCount       ( int32 iValue );

public:
    // FNotifyHook Interface
    virtual void NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged) override;

private:
    // Private data members
    TSharedPtr< IStructureDetailsView > PerformanceOptionsDetailsView;
    FOdysseyPerformanceOptions          PerformanceOptionsStructData;
    TSharedPtr< FStructOnScope >        PerformanceOptionsStructToDisplay;

    FOnBooleanValueChanged  OnLiveUpdateChangedChangedCallback  ;
    FOnBooleanValueChanged  OnAnyValueChangedCallback           ;
};

