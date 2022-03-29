// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UMoviePipelineMasterConfig;
class UMovieSceneSequence;
struct FAssetData;

class SRenderOptions
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SRenderOptions )
        {}
        SLATE_ARGUMENT( TSharedPtr<SWindow>, ParentWindow )
        SLATE_ARGUMENT( UMovieSceneSequence*, Sequence )
    SLATE_END_ARGS()

    void Construct( const FArguments& iArgs );

    bool IsOk();
    bool IsCanceled();

    UMoviePipelineMasterConfig* GetMasterConfig();

//private:
//    FReply  OnAccept();
//    bool    CanAccept() const;
//    FReply  OnCancel();

private:
    void OnMasterConfigSelected( const FAssetData& iAssetData );
    void OnMasterConfigDoubleClicked( const FAssetData& iAssetData );
    void OnMasterConfigEnterPressed( const TArray<FAssetData>& iAssetData );

    EVisibility EncoderSettingsVisibility() const;

private:
    TWeakPtr< SWindow >         mParentWindow;
    bool                        mUserDlgResponse { false }; // == Cancel

    UMoviePipelineMasterConfig* mMasterConfig { nullptr };
    bool                        mIsExecutablePathValid { false };
};
