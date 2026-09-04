// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

#include "CoreMinimal.h"
#include "MovieSceneSection.h"
#include "Channels/MovieSceneEventChannel.h"

#include "SequencerChannelTraits.h"
#include "ScopedTransaction.h"
#include "Channels/MovieSceneChannelData.h"
#include "ArianeImage.h"

#if WITH_EDITOR
#include "MovieSceneClipboard.h"
#endif

#include "ArianeImageMovieSceneSection.generated.h"



class UArianeImage;

// La structure portée par CHAQUE losange
USTRUCT(BlueprintType)
struct FArianeImageKeyData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Instanced, Category = "Ariane")
    UArianeImage* Image = nullptr;

    //UPROPERTY()
    //FMovieSceneFloatChannel InternalOpacityChannel;
};

#if WITH_EDITOR
namespace MovieSceneClipboard
{
    template<>
    inline FName GetKeyTypeName<FArianeImageKeyData>()
    {
        return "ArianeImageKeyData";
    }
}
#endif



USTRUCT()
struct ARIANE_API FArianeImageMovieSceneChannel : public FMovieSceneChannel
{
    GENERATED_BODY()

    using CurveValueType = FArianeImageKeyData;

    TMovieSceneChannelData<FArianeImageKeyData> GetData()
    {
        return TMovieSceneChannelData<FArianeImageKeyData>(&Times, &Values, this, &KeyHandles);
    }

    TMovieSceneChannelData<const FArianeImageKeyData> GetData() const
    {
        return TMovieSceneChannelData<const FArianeImageKeyData>(&Times, &Values);
    }

    // Renvoie la valeur active (la clé la plus récente <= InTime).
    bool Evaluate(FFrameTime InTime, FArianeImageKeyData& OutValue) const;

    //~ Begin FMovieSceneChannel interface
    virtual void GetKeys(const TRange<FFrameNumber>& WithinRange, TArray<FFrameNumber>* OutKeyTimes, TArray<FKeyHandle>* OutKeyHandles) override;
    virtual void GetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<FFrameNumber> OutKeyTimes) override;
    virtual void SetKeyTimes(TArrayView<const FKeyHandle> InHandles, TArrayView<const FFrameNumber> InKeyTimes) override;
    virtual void DuplicateKeys(TArrayView<const FKeyHandle> InHandles, TArrayView<FKeyHandle> OutNewHandles) override;
    virtual void DeleteKeys(TArrayView<const FKeyHandle> InHandles) override;
    virtual void DeleteKeysFrom(FFrameNumber InTime, bool bDeleteKeysBefore) override;
    virtual void ChangeFrameResolution(FFrameRate SourceRate, FFrameRate DestinationRate) override;
    virtual TRange<FFrameNumber> ComputeEffectiveRange() const override;
    virtual int32 GetNumKeys() const override;
    virtual void Reset() override;
    virtual void Offset(FFrameNumber DeltaPosition) override;
    //~ End FMovieSceneChannel interface
    void SetDefault(FArianeImageKeyData InDefault) { DefaultValue = MoveTemp(InDefault); }
    TOptional<FArianeImageKeyData> GetDefault() const { return DefaultValue; }
    void RemoveDefault() { DefaultValue.Reset(); }
    bool HasDefault() const { return DefaultValue.IsSet(); }

private:
    UPROPERTY()
    TArray<FFrameNumber> Times;

    UPROPERTY()
    TArray<FArianeImageKeyData> Values;

    FMovieSceneKeyHandleMap KeyHandles;

private:
    TOptional<FArianeImageKeyData> DefaultValue;
};

UCLASS()
class ARIANE_API UArianeImageMovieSceneSection : public UMovieSceneSection
{
    GENERATED_BODY()

public:
    UArianeImageMovieSceneSection();

    UPROPERTY()
    FArianeImageMovieSceneChannel ImageChannel;
};


#if WITH_EDITOR
namespace Sequencer
{
/*
    // LA SPÉCIALISATION EXPLICITE BASÉE SUR VOTRE TRACE DE CRASH :
    template<>
    ARIANE_API FKeyHandle EvaluateAndAddKey(
        FArianeImageMovieSceneChannel* InChannel,
        const TMovieSceneChannelData<FArianeImageKeyData>& InChannelData,
        FFrameNumber InTime,
        ISequencer& InSequencer,
        FArianeImageKeyData InDefaultValue);
*/
    template<>
    ARIANE_API FKeyHandle AddOrUpdateKey(
        FArianeImageMovieSceneChannel* InChannel,
        UMovieSceneSection* InSectionToKey,
        FFrameNumber InTime,
        ISequencer& InSequencer,
        const FGuid& InObjectBindingID,
        FTrackInstancePropertyBindings* InPropertyBindings);
}
#endif
