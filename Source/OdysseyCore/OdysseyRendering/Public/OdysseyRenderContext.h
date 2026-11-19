// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "CoreMinimal.h"

#include "OdysseyTextureRenderingAbility.h"

struct FOdysseyRenderOverride;

/** Module-owned render settings. Implementations are copied with the request. */
struct ODYSSEYRENDERING_API IOdysseyRenderContextExtension
{
    virtual ~IOdysseyRenderContextExtension() = default;
    virtual TUniquePtr<IOdysseyRenderContextExtension> Clone() const = 0;
};

/** Per-node render callbacks. Module-specific settings live in extensions. */
struct ODYSSEYRENDERING_API FOdysseyRenderOverride
{
    using FRenderFunction = IOdysseyTextureRenderingAbility::FRenderFunction;
    using FDecorator = TFunction<FRenderFunction(FRenderFunction)>;

    FRenderFunction RenderReplacement;
    FDecorator RenderDecorator;
    FRenderFunction ContentReplacement;
    FDecorator ContentDecorator;
};

/** Per-request state, resolved on the game thread. Never store on an asset. */
struct ODYSSEYRENDERING_API FOdysseyRenderContext
{
    using FFilter = TFunction<bool(const IOdysseyTextureRenderingAbility&, const TArray<const IOdysseyTextureRenderingAbility*>&)>;

    FOdysseyRenderContext() = default;
    FOdysseyRenderContext(const FOdysseyRenderContext& Other);
    FOdysseyRenderContext& operator=(const FOdysseyRenderContext& Other);
    FOdysseyRenderContext(FOdysseyRenderContext&&) = default;
    FOdysseyRenderContext& operator=(FOdysseyRenderContext&&) = default;

    template <typename T> T& FindOrAddExtension()
    {
        TUniquePtr<IOdysseyRenderContextExtension>& Entry = Extensions.FindOrAdd(T::TypeName());
        if (!Entry)
        {
            Entry = MakeUnique<T>();
        }
        return static_cast<T&>(*Entry);
    }
    template <typename T> const T* GetExtension() const
    {
        const TUniquePtr<IOdysseyRenderContextExtension>* Entry = Extensions.Find(T::TypeName());
        return Entry ? static_cast<const T*>(Entry->Get()) : nullptr;
    }
    bool ShouldRender(const IOdysseyTextureRenderingAbility& Node, const TArray<const IOdysseyTextureRenderingAbility*>& Parents) const;
    const FOdysseyRenderOverride* Find(const IOdysseyTextureRenderingAbility& Node) const;

    TMap<FGuid, FOdysseyRenderOverride> Overrides;
    FFilter Filter;
    /** Rendering flags shared by every node in this request. */
    uint64 Type = EOdysseyRenderingType::Render;

private:
    TMap<FName, TUniquePtr<IOdysseyRenderContextExtension>> Extensions;
};

/** View-local preview; callbacks must retain resources by value until GPU work completes. */
class ODYSSEYRENDERING_API FOdysseyRenderPreview
{
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnInvalidated, const TArray<FIntRect>&);
    const FOdysseyRenderContext& GetContext() const;
    const FGuid& GetId() const;
    uint64 GetRevision() const;
    FOnInvalidated& OnInvalidated();
    /**
     * @param InContext The context of the next renders.
     * @param bInvalidate False when the new context renders the same pixels as the previous one, e.g. a replacement
     *                    showing the current content, so that nothing needs to be rendered again.
     */
    void SetContext(FOdysseyRenderContext InContext, bool bInvalidate = true);
    /** Empty rectangles invalidate everything; otherwise use destination-space bounds. */
    void Invalidate(const TArray<FIntRect>& Rects);
    void Reset();
private:
    FGuid Id = FGuid::NewGuid();
    uint64 Revision = 0;
    FOdysseyRenderContext Context;
    FOnInvalidated Invalidated;
};
