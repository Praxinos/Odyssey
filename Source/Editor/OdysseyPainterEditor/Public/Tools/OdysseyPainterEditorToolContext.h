// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "TickableEditorObject.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyVectorEngine.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorToolContext
{
public:
    DECLARE_DELEGATE(FOnProvideRasterBlock)
    DECLARE_MULTICAST_DELEGATE(FOnChanged)

public:                         
    class ODYSSEYPAINTEREDITOR_API FParams
    {
    public:
        FParams();

    public:
        bool Set(const FParams& iParams);

    public:
        TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
        bool mCanProvideRasterBlockOnDemand;
        bool mIsRasterBlockReadOnly;
        FOdysseyVectorEngine* mVectorEngine;
        FOnProvideRasterBlock mOnProvideRasterBlockDelegate;
    };

public:
    // Destructor
    virtual ~FOdysseyPainterEditorToolContext();

    //Constructor
    FOdysseyPainterEditorToolContext(FOdysseyPainterEditor* iEditor);

public:
    void Set( const FParams& iParams );
    void Unset();

    FOdysseyPainterEditor* GetEditor() const;

    //Raster
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;

    /**
     * @brief Returns true if the RasterBlock can be created on demand by the user
     * 
     * example : in an animation raster layer, the block can be created on demand.
     * in that case, CanProvideRasterBlockOnDemand() returns true all the time,
     * even if the RasterBlock is not created when we ask for it (Auto Create Instance option turned off).
     * 
     * @return true 
     * @return false 
     */
    bool CanProvideRasterBlockOnDemand() const; 
    bool IsRasterBlockReadOnly() const; 

    //Vector
    FOdysseyVectorEngine* GetVectorEngine() const;

    //Other
    FOnProvideRasterBlock& OnProvideRasterBlockDelegate(); 
    FOnChanged& OnChanged();

private:
    //TSharedPtr<FOdysseyMediaSet> mMediaSet; 

    FOdysseyPainterEditor* mEditor;
    FParams mParams;
    FOnChanged mOnChanged;
};
