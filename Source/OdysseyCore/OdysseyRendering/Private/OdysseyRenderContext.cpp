// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#include "OdysseyRenderContext.h"

FOdysseyRenderContext::FOdysseyRenderContext(const FOdysseyRenderContext& Other)
    : Overrides(Other.Overrides), Filter(Other.Filter)
{
    for (const TPair<FName, TUniquePtr<IOdysseyRenderContextExtension>>& Entry : Other.Extensions)
    {
        Extensions.Add(Entry.Key, Entry.Value->Clone());
    }
}

FOdysseyRenderContext& FOdysseyRenderContext::operator=(const FOdysseyRenderContext& Other)
{
    if (this != &Other)
    {
        Overrides = Other.Overrides;
        Filter = Other.Filter;
        Extensions.Empty();
        for (const TPair<FName, TUniquePtr<IOdysseyRenderContextExtension>>& Entry : Other.Extensions)
        {
            Extensions.Add(Entry.Key, Entry.Value->Clone());
        }
    }
    return *this;
}

bool FOdysseyRenderContext::ShouldRender(const IOdysseyTextureRenderingAbility& Node, const TArray<const IOdysseyTextureRenderingAbility*>& Parents) const
{
    return !Filter || Filter(Node, Parents);
}

const FOdysseyRenderOverride* FOdysseyRenderContext::Find(const IOdysseyTextureRenderingAbility& Node) const
{
    return Overrides.Find(Node.GetRenderingId());
}

const FOdysseyRenderContext& FOdysseyRenderPreview::GetContext() const
{
    return Context;
}

const FGuid& FOdysseyRenderPreview::GetId() const
{
    return Id;
}

uint64 FOdysseyRenderPreview::GetRevision() const
{
    return Revision;
}

FOdysseyRenderPreview::FOnInvalidated& FOdysseyRenderPreview::OnInvalidated()
{
    return Invalidated;
}

void FOdysseyRenderPreview::SetContext(FOdysseyRenderContext InContext)
{
    Context = MoveTemp(InContext);
    Invalidate({});
}

void FOdysseyRenderPreview::Invalidate(const TArray<FIntRect>& Rects)
{
    ++Revision;
    Invalidated.Broadcast(Rects);
}

void FOdysseyRenderPreview::Reset()
{
    SetContext(FOdysseyRenderContext());
}
