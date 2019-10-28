// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "InputCoreTypes.h"
#include "UObject/GCObject.h"
#include "UnrealClient.h"

class FSceneViewport;
class SViewport;
class FOdysseyBrushTestAreaViewportClient;
class FOdysseyBrushEditor;
class FCanvas;
class SOdysseyBrushTestArea;
class UTexture2D;

//////////////////////////////////////////////////////////////////////////
// FOdysseyBrushTestAreaViewportClient

class FOdysseyBrushTestAreaViewportClient
    : public FViewportClient
    , public FGCObject
{
public:
    // Construction / Destruction
    FOdysseyBrushTestAreaViewportClient(TWeakPtr<FOdysseyBrushEditor> InEditor, TWeakPtr<SOdysseyBrushTestArea> InOdysseyBrushPreview);
    ~FOdysseyBrushTestAreaViewportClient();

public:
    // FViewportClient interface
    virtual void Draw(FViewport* Viewport, FCanvas* Canvas) override;
    virtual UWorld* GetWorld() const override { return nullptr; }

public:
    // FGCObject interface
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

public:
    // public interface
    void Resize( FVector2D iSize );

private:
    // Internal callbacks
    // Nothing ATM

private:
    // Private data members
    /** Pointer back to the editor tool that owns us */
    TWeakPtr<FOdysseyBrushEditor> EditorPtr;

    /** Pointer back to the widget control that owns us */
    TWeakPtr<SOdysseyBrushTestArea> OdysseyBrushTestAreaPtr;

    UTexture2D* tex;
};

//////////////////////////////////////////////////////////////////////////
// SOdysseyBrushTestArea

class SOdysseyBrushTestArea
    : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SOdysseyBrushTestArea) { }
    SLATE_END_ARGS()

public:
    // Widget construction
    void Construct( const FArguments& InArgs, const TSharedRef<FOdysseyBrushEditor>& InEditor );

public:
    // Widget viewports getters
    TSharedPtr<FSceneViewport> GetViewport( ) const;
    TSharedPtr<SViewport> GetViewportWidget( ) const;

public:
    // SWidget overrides
    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

public:
    // Drawing utility
    void Refresh();

private:
    // Internal callbacks
    void OnViewportResized( FVector2D NewSize );

private:
    // Private data members
    // Pointer back to the Texture editor tool that owns us.
    TWeakPtr<FOdysseyBrushEditor> EditorPtr;

    // Level viewport client.
    TSharedPtr<class FOdysseyBrushTestAreaViewportClient> ViewportClient;

    // Slate viewport for rendering and IO.
    TSharedPtr<FSceneViewport> Viewport;

    // Viewport widget.
    TSharedPtr<SViewport> ViewportWidget;
};
