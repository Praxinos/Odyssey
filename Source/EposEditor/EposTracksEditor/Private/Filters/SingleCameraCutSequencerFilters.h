// IDDN.FR.000.000000.000.S.X.0000.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#pragma once

#include "Filters/SequencerTrackFilterExtension.h"

#include "SingleCameraCutSequencerFilters.generated.h"

UCLASS()
class USingleCameraCutSequencerTrackFilter
    : public USequencerTrackFilterExtension
{
public:
    GENERATED_BODY()

    //~ Begin USequencerTrackFilterExtension
    virtual void AddTrackFilterExtensions( ISequencerTrackFilters& InFilterInterface
                                           , const TSharedRef<FFilterCategory>& InPreferredCategory
                                           , TArray<TSharedRef<FSequencerTrackFilter>>& InOutFilterList ) const override;
    //~ End USequencerTrackFilterExtension
};
