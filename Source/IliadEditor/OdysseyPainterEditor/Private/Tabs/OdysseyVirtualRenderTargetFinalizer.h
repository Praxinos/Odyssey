// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "VirtualTexturing.h"

/**
 * A Finalizer is an object that does the final work of filling the physical textures.
 * The work for all finalizers is scheduled at a specific point in the frame where we can write to the physical texture without hazards, and
 * where the virtual texture page tables are also updated.
 * The finalizer work may be split into two parts.
 * RenderFinalize() does not write to the physical texture, but can read from virtual textures. This is a phase that allows any
 * page rendering which may need to sample virtual textures. Runtime virtual textures and material systems require this.
 * Finalise() must write to the physical textures, but cannot sample from them.
 * All finalizers need to implement Finalize() but only ones that need to sample virtual textures need to implement RenderFinalize().
 */

class FOdysseyVirtualRenderTargetFinalizer
    : public IVirtualTextureFinalizer //This is actually the base class for a Producer
{
public:
    virtual ~FOdysseyVirtualRenderTargetFinalizer();
    FOdysseyVirtualRenderTargetFinalizer(const FVTProducerDescription& InProducerDesc);

public:
    // IVirtualTextureFinalizer interface
    /** Finalize work that has read only access to the virtual texture physical pools. */
    virtual void RenderFinalize(FRDGBuilder& GraphBuilder, ISceneRenderer* SceneRenderingContext) override;
    /** Finalize work that has write only access to the virtual texture physical pools. */
    virtual void Finalize(FRDGBuilder& GraphBuilder) override;
    // End IVirtualTextureFinalizer interface

    /** A description for a single tile to render. */
    struct FTileEntry
    {
        FVTProduceTargetLayer Target;
        uint64 vAddress = 0;
        uint8 vLevel = 0;
    };

    void AddTile(FTileEntry const& InEntry)
    {
        TilesToRender.Add(InEntry);
    }

private:
    /** Array of tiles in the queue to finalize. */
    TArray<FTileEntry> TilesToRender;

    FVTProducerDescription ProducerDesc;
};
