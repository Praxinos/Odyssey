// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/NotifyHook.h"
#include "Widgets/SCompoundWidget.h"

#include "SRenderOptions.generated.h"

class UMoviePipelinePrimaryConfig;
class UMovieSceneSequence;
struct FAssetData;

UENUM()
enum class ECodecPresets : int32
{
    kSelect UMETA( DisplayName = "Select..." ),
    kMP4    UMETA( DisplayName = "MP4" ),
    kMOV    UMETA( DisplayName = "MOV" ),
};

class SRenderOptions
    : public SCompoundWidget
    , public FNotifyHook
{
public:
    SLATE_BEGIN_ARGS( SRenderOptions )
        {}
        SLATE_ARGUMENT( TSharedPtr<SWindow>, ParentWindow )
        SLATE_ARGUMENT( UMovieSceneSequence*, Sequence )
    SLATE_END_ARGS()

    void Construct( const FArguments& iArgs );

    // FNotifyHook interface
    virtual void NotifyPostChange( const FPropertyChangedEvent& iPropertyChangedEvent, FEditPropertyChain* iPropertyThatChanged ) override;
    //

    bool IsOk();
    bool IsCanceled();

    UMoviePipelinePrimaryConfig* GetPrimaryConfig();

private:
    virtual FReply OnKeyDown( const FGeometry& iMyGeometry, const FKeyEvent& iKeyEvent ) override;

//private:
//    FReply  OnAccept();
//    bool    CanAccept() const;
//    FReply  OnCancel();

private:
    void OnPrimaryConfigSelected( const FAssetData& iAssetData );
    void OnPrimaryConfigDoubleClicked( const FAssetData& iAssetData );
    void OnPrimaryConfigEnterPressed( const TArray<FAssetData>& iAssetData );

    EVisibility EncoderSettingsVisibility() const;

private:
    TWeakPtr< SWindow >             mParentWindow;
    bool                            mUserDlgResponse { false }; // == Cancel

    UMoviePipelinePrimaryConfig*    mPrimaryConfig { nullptr };
    bool                            mIsExecutablePathValid { false };
};
