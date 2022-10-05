// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditor.h"

/**
 * Base class of any Odyssey Toolkit.
 * The toolkit is the main entry point for the  Editor
 */
template<typename T>
class TOdysseyToolkit
    : public T
{
public:
    // Construction / Destruction
    virtual ~TOdysseyToolkit();
    TOdysseyToolkit(const FName& iAppIdentifier, TSharedPtr<FOdysseyEditor> iEditor);

public:
    virtual void Initialize();
    virtual void Finalize();

protected:
    // FAssetEditorToolkit interface
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager) override;
    virtual void ExtendMenu() = 0;

protected:
    FName mAppIdentifier;
    TSharedPtr<FOdysseyEditor> mEditor;
};

/////////////////////////////////////////////////////
// TOdysseyToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

template<typename T>
TOdysseyToolkit<T>::~TOdysseyToolkit()
{
}

template<typename T>
TOdysseyToolkit<T>::TOdysseyToolkit(const FName& iAppIdentifier, TSharedPtr<FOdysseyEditor> iEditor) :
    mAppIdentifier(iAppIdentifier),
    mEditor(iEditor)
{
}

template<typename T>
void
TOdysseyToolkit<T>::Initialize()
{
    mEditor->OnToolkitInitialized(this);
    mEditor->BindShortcuts(this);
}


template<typename T>
void TOdysseyToolkit<T>::Finalize()
{
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface

template<typename T>
FLinearColor
TOdysseyToolkit<T>::GetWorldCentricTabColorScale() const
{
    return FLinearColor( 0.3f, 0.2f, 0.5f, 0.5f );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

template<typename T>
void
TOdysseyToolkit<T>::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    T::RegisterTabSpawners(iTabManager);
    T::WorkspaceMenuCategory = mEditor->RegisterTabSpawners(iTabManager);
}

template<typename T>
void
TOdysseyToolkit<T>::UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    T::UnregisterTabSpawners(iTabManager);
	mEditor->UnregisterTabSpawners(iTabManager);
}

// #include "OdysseyToolkit.inl"
