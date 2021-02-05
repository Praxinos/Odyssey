// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

class FOdysseyPainterEditorToolkit;

/**
 * Interface for a Painting Editor
 */
class ODYSSEYPAINTEREDITOR_API IOdysseyPainterEditor
{
public:
    // Construction / Destruction
    virtual ~IOdysseyPainterEditor();

public:
    virtual void Init() = 0;

public:
    virtual UObject* GetEditedObject() = 0;
    virtual TArray<UObject*> GetAllEditedObjects() = 0;

    virtual TSharedPtr<FOdysseyPainterEditorToolkit> GetToolkit() = 0;
    virtual void SetToolkit(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit) = 0;
    virtual void OnToolkitInitialized() = 0;

    virtual bool OnCloseRequested() = 0;
    virtual const TSharedRef<FTabManager::FLayout>& CreateLayout() const = 0;
    virtual const TArray<TSharedPtr<FExtender>>& CreateMenuExtenders() const = 0;
    
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) = 0;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) = 0;
};

// template<typename T> class TOdysseyPainterEditor<T>;

/**
 * Base template class for a Painting Editor
 */
template<typename T>
class TOdysseyPainterEditor :
    public IOdysseyPainterEditor
{
public:
    // Construction / Destruction
    ~TOdysseyPainterEditor();
    TOdysseyPainterEditor();

public:
    virtual void Init() override;

public:
    void EditObject(T* iObject);

    virtual TSharedPtr<FOdysseyPainterEditorToolkit> GetToolkit() override;
    virtual void SetToolkit(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit) override;
    virtual void OnToolkitInitialized() override;

    virtual UObject* GetEditedObject() override;
    virtual TArray<UObject*> GetAllEditedObjects() override;

protected:
    T* mEditedObject;

private:
    TWeakPtr<FOdysseyPainterEditorToolkit> mToolkit;
};

template<typename T>
TOdysseyPainterEditor<T>::~TOdysseyPainterEditor<T>()
{

}

template<typename T>
TOdysseyPainterEditor<T>::TOdysseyPainterEditor() :
    mEditedObject(nullptr)
{

}

template<typename T>
void
TOdysseyPainterEditor<T>::Init()
{

}

template<typename T>
void
TOdysseyPainterEditor<T>::EditObject(T* iObject)
{
    mEditedObject = iObject;
}

template<typename T>
TSharedPtr<FOdysseyPainterEditorToolkit>
TOdysseyPainterEditor<T>::GetToolkit()
{
    return mToolkit.Pin();
}

template<typename T>
void
TOdysseyPainterEditor<T>::SetToolkit(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit)
{
    mToolkit = iToolkit;
}

template<typename T>
void
TOdysseyPainterEditor<T>::OnToolkitInitialized()
{

}

template<typename T>
UObject*
TOdysseyPainterEditor<T>::GetEditedObject()
{
    return mEditedObject;
}

template<typename T>
TArray<UObject*>
TOdysseyPainterEditor<T>::GetAllEditedObjects()
{
    TArray<UObject*> objects;
    objects.Add(mEditedObject);
    return objects;
}
