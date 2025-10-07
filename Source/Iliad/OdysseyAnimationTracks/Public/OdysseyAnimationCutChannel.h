// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Channels/MovieSceneChannel.h"
#include "Channels/MovieSceneChannelData.h"
#include "Channels/MovieSceneChannelEditorData.h"
#include "Channels/MovieSceneChannelTraits.h"
#include "Containers/Array.h"
#include "Containers/ArrayView.h"
#include "Containers/UnrealString.h"
#include "HAL/Platform.h"
#include "HAL/PlatformCrt.h"
#include "Math/Range.h"
#include "Misc/FrameNumber.h"
#include "Misc/FrameTime.h"
#include "Misc/Optional.h"
#include "MovieSceneClipboard.h"
#include "OdysseyAnimationCut.h"
#include "Serialization/StructuredArchive.h"
#include "Templates/UnrealTemplate.h"
#include "UObject/Class.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyAnimationCutChannel.generated.h"

class UOdysseyAnimationTimelineSection;
struct FFrameRate;
struct FKeyHandle;
struct FPropertyTag;

#if WITH_EDITOR
namespace MovieSceneClipboard
{
    template<> inline FName GetKeyTypeName<FOdysseyAnimationCutValue>()
    {
        return "FOdysseyAnimationCutValue";
    }
}
#endif

/** Defines data for keys in this emitter section. */
USTRUCT()
struct FOdysseyAnimationCutValue //TODO: really necessary ?!
{
    GENERATED_BODY()

    UPROPERTY( EditAnywhere, Category = "Value" )
    FAnimationCut Value;
};

USTRUCT()
struct FOdysseyAnimationCutChannel : public FMovieSceneChannel
{
    GENERATED_BODY()

    typedef FOdysseyAnimationCutValue CurveValueType;

    FOdysseyAnimationCutChannel();
    FOdysseyAnimationCutChannel( UOdysseyAnimationTimelineSection* iTimelineSection );

    void SetParentSection( UOdysseyAnimationTimelineSection* iTimelineSection );

    /**
     * Access a mutable interface for this channel's data
     *
     * @return An object that is able to manipulate this channel's data
     */
    FORCEINLINE TMovieSceneChannelData<FOdysseyAnimationCutValue> GetData()
    {
       return TMovieSceneChannelData<FOdysseyAnimationCutValue>(&Times, &Values, this, &KeyHandles);
    }

    /**
     * Access a constant interface for this channel's data
     *
     * @return An object that is able to interrogate this channel's data
     */
    FORCEINLINE TMovieSceneChannelData<const FOdysseyAnimationCutValue> GetData() const
    {
        return TMovieSceneChannelData<const FOdysseyAnimationCutValue>(&Times, &Values);
    }

    /**
     * Const access to this channel's times
     */
    FORCEINLINE TArrayView<const FFrameNumber> GetTimes() const
    {
        return Times;
    }

    /**
     * Const access to this channel's values
     */
    FORCEINLINE TArrayView<const FOdysseyAnimationCutValue> GetValues() const
    {
        return Values;
    }

    /**
     * Evaluate this channel
     *
     * @param InTime     The time to evaluate at
     * @return A pointer to the string, or nullptr
     */
    ODYSSEYANIMATIONTRACKS_API const FOdysseyAnimationCutValue* Evaluate(FFrameTime InTime) const;

    /**
     * Evaluate this channel
     *
     * @param InTime     The time to evaluate at
     * @param OutValue   A value to receive the result
     * @return true if the channel was evaluated successfully, false otherwise
     */
    ODYSSEYANIMATIONTRACKS_API bool Evaluate( FFrameTime InTime, FOdysseyAnimationCutValue& OutValue ) const;

    ODYSSEYANIMATIONTRACKS_API FKeyHandle FindPreviousKey( const TArray<FKeyHandle>& iKeyHandles );
    ODYSSEYANIMATIONTRACKS_API FKeyHandle FindNextKey( const TArray<FKeyHandle>& iKeyHandles );

protected:
    friend class UOdysseyAnimationTimelineSection;

    ODYSSEYANIMATIONTRACKS_API void Update( const TArray<FKeyHandle>& iKeyHandles, EPropertyChangeType::Type iChangeType );

public:
    // ~ FMovieSceneChannel Interface
    virtual void GetKeys(const TRange<FFrameNumber>& WithinRange, TArray<FFrameNumber>* OutKeyTimes, TArray<FKeyHandle>* OutKeyHandles) override;
    virtual void GetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<FFrameNumber> OutKeyTimes) override;
    virtual void SetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<const FFrameNumber> InKeyTimes) override;
    virtual void DuplicateKeys(TArrayView<const FKeyHandle> InHandles, TArrayView<FKeyHandle> OutNewHandles) override;
    virtual void DeleteKeys(TArrayView<const FKeyHandle> InHandles) override;
    virtual void DeleteKeysFrom(FFrameNumber InTime, bool bDeleteKeysBefore) override;
    virtual void RemapTimes(const UE::MovieScene::IRetimingInterface& Retimer) override;
    virtual TRange<FFrameNumber> ComputeEffectiveRange() const override;
    virtual int32 GetNumKeys() const override;
    virtual void Reset() override;
    virtual void Offset(FFrameNumber DeltaPosition) override;
    //virtual void Optimize(const FKeyDataOptimizationParams& InParameters) override;
    virtual FKeyHandle GetHandle(int32 Index) override;
    virtual int32 GetIndex(FKeyHandle Handle) override;

private:
    TWeakObjectPtr<UOdysseyAnimationTimelineSection> TimelineSection;

    UPROPERTY(meta=(KeyTimes))
    TArray<FFrameNumber> Times;

    /** Array of values that correspond to each key time */
    UPROPERTY(meta=(KeyValues))
    TArray<FOdysseyAnimationCutValue> Values;

    /** This needs to be a UPROPERTY so it gets saved into editor transactions but transient so it doesn't get saved into assets. */
    UPROPERTY(Transient)
    FMovieSceneKeyHandleMap KeyHandles;
};

template<>
struct TMovieSceneChannelTraits<FOdysseyAnimationCutChannel> : TMovieSceneChannelTraitsBase<FOdysseyAnimationCutChannel>
{
    enum { SupportsDefaults = false };
};

//template<>
//struct TStructOpsTypeTraits<FOdysseyAnimationCutChannel> : public TStructOpsTypeTraitsBase2<FOdysseyAnimationCutChannel>
//{
//  enum { WithStructuredSerializeFromMismatchedTag = true };
//};

//template<>
//struct TMovieSceneChannelTraits<FOdysseyAnimationCutChannel> : TMovieSceneChannelTraitsBase<FOdysseyAnimationCutChannel>
//{
//#if WITH_EDITOR
//
//  /** String channels can have external values (ie, they can get their values from external objects for UI purposes) */
//  typedef TMovieSceneExternalValue<FString> ExtendedEditorDataType;
//
//#endif
//};

//inline bool EvaluateChannel(const FOdysseyAnimationCutChannel* InChannel, FFrameTime InTime, FString& OutValue)
//{
//  if (const FString* Result = InChannel->Evaluate(InTime))
//  {
//      OutValue = *Result;
//      return true;
//  }
//  return false;
//}

//inline bool ValueExistsAtTime(const FOdysseyAnimationCutChannel* Channel, FFrameNumber InFrameNumber, const FString& Value)
//{
//  const FFrameTime FrameTime(InFrameNumber);
//
//  const FString* ExistingValue = Channel->Evaluate(FrameTime);
//  return ExistingValue && Value == *ExistingValue;
//}
