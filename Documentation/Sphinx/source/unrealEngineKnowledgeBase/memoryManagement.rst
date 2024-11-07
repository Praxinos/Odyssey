=================
Memory Management
=================

Memory Management in Unreal Engine is done differently if you manage regular classes or UObject classes.

Regular Memory Management
=========================

.. warning:: Regular Memory Management does not work with UObjects, please see :ref:`memoryManagement UObject Memory Management`

For regular classes, memory management can be done through `Unreal Engine Smart Pointer Library <https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/SmartPointerLibrary/>`_ using a bunch of classes like :

- ``TSharedPtr``
- ``TSharedRef``
- ``TWeakPtr``
- ``TUniquePtr``

| Using those classes allows you to control when memory is released, so that any object relying on this memory is guaranted to access an unreleased memory.
| It is basically the same system as the `std smart pointer library <https://en.cppreference.com/book/intro/smart_pointers>`_

-----------------------
TSharedPtr / TSharedRef
-----------------------

Some words to understand here :

- retain a pointer : means you care about the memory the pointer is pointing to to stay valid, you do not allow anyone to destroy it
- release a pointer : means you don't care about the memory the pointer is pointing to anymore, you don't care if anyone destroys it

| ``TSharedPtr`` and ``TSharedRef`` retain a pointer until they are destroyed, after what the pointer is released.
| When no one is retaining the pointer anymore, the memory the pointer points to is destroyed.

.. note:: Using ``TSharedPtr`` and ``TSharedRef`` describes the intent of "owning" the pointer, and controling its lifetime

The difference between both class is that ``TSharedPtr`` underlying pointer can be nullptr, whereas ``TSharedRef`` ones cannot.

Create a ``TSharedPtr`` or ``TSharedRef``, you can :

- Instantiate an object of a class and retrieve directly a ``TSharedPtr`` or ``TSharedRef`` from it using ``MakeShared()``.
- Retrieve ``TSharedPtr`` or ``TSharedRef`` from an existing classic pointer using ``MakeShareable()``.
- Retrieve ``TSharedPtr`` or ``TSharedRef`` from an object of a class inheriting from ``TSharedFromThis``, using ``AsShared()`` or ``SharedThis()``.

.. code-block:: cpp
    :caption: Using MakeShared()

    // a simple class with several constructors for example
    class MyClass {
    public:
        //default constructor
        MyClass();

        //a custom constructor
        MyClass(int a);
    };

    // Create a new object of class MyClass, and retrieve directly a SharedPtr retaining it
    // Using MyClass default constructor
    TSharedPtr<MyClass> sharedPtr = MakeShared<MyClass>();

    // Using MyClass custom constructor
    TSharedPtr<MyClass> sharedPtr = MakeShared<MyClass>(10);

.. code-block:: cpp
    :caption: Using MakeShareable()

    // a simple class
    class MyClass {
        ...
    };

    // Create the classic pointer
    MyClass* ptr = new MyClass();

    // Retrieve a SharedPtr from ptr.
    // You don't have to call 'delete' manually to destroy the ptr memory anymore
    // The destruction sharedPtr will take care of it automatically.
    TSharedPtr<MyClass> sharedPtr = MakeShareable(ptr);

.. code-block:: cpp
    :caption: Using AsShared() or SharedThis()

    // a base class inheriting TSharedFromThis
    class MyBaseClass : public TSharedFromThis<MyBaseClass> {
        ...
    };

    // a class inheriting MyBaseClass
    class MyClass : public MyBaseClass {
        ...
    };

    // Create the classic pointer
    MyClass* ptr = new MyClass();

    // Calling AsShared() or SharedThis() immediately will cause a CRASH
    // AsShared() or SharedThis() need the ptr to already be shared by a TSharedPtr or a TSharedRef
    TSharedPtr<MyClass> sharedPtr = ptr->AsShared(); //CRASH
    TSharedPtr<MyClass> sharedPtr = ptr->SharedThis<MyClass>(ptr); //CRASH

    //----

    // Make a TSharedPtr or TSharedRef so that ptr is considered as shared.
    TSharedPtr<MyClass> sharedPtr = MakeShareable(ptr);

    // Retrieve a TSharedPtr of MyClass from ptr
    TSharedPtr<MyClass> sharedPtr = ptr->AsShared();
    TSharedPtr<MyClass> sharedPtr = ptr->SharedThis<MyClass>(ptr);

    // SharedThis allows you to define the class to retrieve, so you can retrieve base classes
    TSharedPtr<MyBaseClass> sharedPtr = ptr->SharedThis<MyBaseClass>(ptr);

.. code-block:: cpp
    :caption: Retain / Release Behaviour

    // a simple class
    class MyClass {
        ...
    };

    {
        TSharedPtr<MyClass> sharedPtr1 = MakeShared<MyClass>(); // Create the sharedPtr, the pointer is retained 1 time

        {
            TSharedPtr<MyClass> sharedPtr2 = sharedPtr1; // Retain the same sharedPtr, the pointer is retained 2 times

            {
                TSharedPtr<MyClass> sharedPtr3 = sharedPtr2; // Retain the same sharedPtr, the pointer is retained 3 times
            } // sharedPtr3 is destroyed, the pointer is released once, the pointer is retained 2 times
        } // sharedPtr2 is destroyed, the pointer is released once, the pointer is retained 1 time
    } // sharedPtr1 is destroyed, the pointer is released once, the pointer is retained 0 time, the pointer memory is destroyed as no one is retaining it anymore

--------
TWeakPtr
--------

Sometimes you need to store a shared pointer but you don't actually need to retain it.
This can happen to avoid memory leaks when 2 objects store each other pointers using ``TSharedPtr`` or ``TSharedRef``.
Each object will retain the other and no one will ever be destroyed.
In those kind of cases, using a ``TWeakPtr`` can help.

.. note:: Using ``TWeakPtr`` and ``TSharedRef`` describes the intent of "observing" the pointer, without "owning" it or controling its lifetime

Let's take the example of a Car Owner and a Car Observer to illustrate teh concept.

.. code-block:: cpp
    :caption: Car Owner / Car Observer

    // A class representing a Car
    class Car 
    {
    };

    // A class representing a Car Owner
    class CarOwner
    {
    public:
        TSharedPtr<Car> mCar; //The owner owns the car and controls its lifetime
    };

    // A class representing a Car Observer, someone who will just be able to look at the car, but does not own it
    class CarObserver
    {
    public:
        TWeakPtr<Car> mCar; //The observer can observe the car, but does not own it, and does not control its lifetime
    };

    CarOwner owner;
    CarObserver observer;

    owner.mCar = MakeShared<Car>();
    observer.mCar = owner.mCar;

The car owner can do whatever it wants with the car anytime he wants, as he owns it.
On the other hand, the car observer is just able to observe the car, if the car still exists.
Here how the car observer would implement a function to observe the car.

.. code-block:: cpp
    :caption: CarObserver::ObserveCar()

    // A class representing a Car
    class Car 
    {
    };

    // A class representing a Car Observer, someone who will just be able to look at the car, but does not own it
    class CarObserver
    {
    public:
        void ObserveCar()
        {
            // When the observer needs to observe the car, it needs to make sure the car will not be destroyed while he is observeing it
            // To do that, he will retain the car using a TSharedPtr before observing it and will keep it during the whole observation process.
            // This is done using the TWeak::Pin() method
            TSharedPtr<Car> car = mCar.Pin();

            //At this point, the observer needs to know if the car he retained still exist or has been destroyed before he could retain it.
            if (!car)
            {
                //The car has been destroyed, there is nothing to observe
                return;
            }

            // The car exists and the observer can observe it here
            // Once the observation is done, the TSharedPtr is destroyed and the car ptr is released
        }

    public:
        TWeakPtr<Car> mCar; //The observer can observe the car, but does not own it, and does not control its lifetime
    };


----------
TUniquePtr
----------

.. todo:: Write the documentation for TUniquePtr

.. _memoryManagement UObject Memory Management:

UObject Memory Management
=========================

| There is not so much easy to find informations online about UObject Memory Management.
| Though you can find some information in the Garbage `Collection Documentation <https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/Objects/Optimizations/>`_, or in the some `community documentation <https://unrealcommunity.wiki/memory-management-6rlf3v4i>`_.
| Anyway, let's describe how UObject Memory Management is handled by Unreal Engine.

| UObjects are special objects in Unreal Engine, as they are designed to be used ingame.
| In order to manage memory in game in an easy and reliable way, the UObject implements a Garbage Collector.
| The Unreal Garbage Collector is executed once each few minutes (configurable in Unreal Engine Preferences), and will destroy each UObject which is not retained by something.

.. attention:: When an UObject in not retained by anyone anymore, it's memory is not immediately destroyed, it must wait for the Garbage Collector to destroy it. This can lead to High Memory Usage if you create a lot of UObjects at once.

But here comes the question : If UObject does not work with ``TSharedPtr``, ``TSharedRef`` or ``TUniquePtr``, how can anyone retain an UObject to prevent its destruction ?

UObjects can be retained in several ways:

- using UPROPERTY
- using TStrongObjectPtr
- using FGCObject
- using TArray or other Unreal ENgine container class

---------------
Using UProperty
---------------

| UProperty are special properties of UObjects and UStructs (see :doc:`UObjects <uObject>`).
| When a UObject is contained in a UProperty, the Uproperty retains it.

.. code-block:: cpp
    :caption: Using UProperty

    //The Main class
    UCLASS()
    class UMyClass :
        public UObject
    {
        GENERATE_BODY()
    };

    //The Second Class
    UCLASS()
    class UMySecondClass :
        public UObject
    {
        GENERATE_BODY()

        public:
        UPROPERTY()
        UMyClass* Object; 
    };

    UMyClass* object = NewObject<UMyClass>();
    UMySecondClass* secondObject = NewObject<UMySecondClass>();

    //Assign object to secondObject.Object
    //this will retain object
    FObjectEditorUtils::SetPropertyValue(secondObject, "Object", object);

----------------------
Using TStrongObjectPtr
----------------------

| If you need to prevent an UObject to be destroyed, but you cannot rely on UProperty, you can use ``TStrongObjectPtr``.
| ``TStrongObjectPtr`` works the same way ``TSharedPtr`` does for regular objects.

.. code-block:: cpp
    :caption: Using TStrongObjectPtr

    //The Main class
    UCLASS()
    class UMyClass :
        public UObject
    {
        GENERATE_BODY()
    };

    UMyClass* object = NewObject<UMyClass>();

    {
        TStrongObjectPtr<MyClass> strongPtr1 = object; // Retain object, the pointer is retained 1 time

        {
            TStrongObjectPtr<MyClass> strongPtr2 = object; // Retain object, the pointer is retained 2 times

            {
                TStrongObjectPtr<MyClass> strongPtr3 = object; // Retain object, the pointer is retained 3 times
            } // strongPtr3 is destroyed, the pointer is released once, the pointer is retained 2 times
        } // strongPtr2 is destroyed, the pointer is released once, the pointer is retained 1 time
    } // strongPtr1 is destroyed, the pointer is released once, the pointer is retained 0 time, object can be garbage collected

------------
Using TArray
------------

While a UObject is contained in a TArray or some other Unreal Engine container, it is prevented to be garbage collected.

.. code-block:: cpp
    :caption: Using TArray

    //The Main class
    UCLASS()
    class UMyClass :
        public UObject
    {
        GENERATE_BODY()
    };

    TArray<UMyClass*> array;
    UMyClass* object = NewObject<UMyClass>();

    //this will retain object
    array.Add(object);

---------------
Using FGCObject
---------------

As an alternative, you can create a class chich inherits ``FGCObject`` to communicate to the Garbage Collector which UObjects should not be garbage collected.

.. code-block:: cpp
    :caption: Using FGCObject
    
    //The Main class
    UCLASS()
    class UMyClass :
        public UObject
    {
        GENERATE_BODY()
    };

    class ObjectRetainer :
        public FGCObject
    {
    public:
        // FGCObject API
        virtual void AddReferencedObjects( FReferenceCollector& ioCollector ) override
        {
            //Prevents mObject to be garbage collected
            if (mObject)
                ioCollector.AddReferencedObject( mObject );
        }

        MyClass* mObject;
    };

    UMyClass* object = NewObject<UMyClass>();
    ObjectRetainer retainer;

    //Set object in retainer.mObject, which will then be provided to the garbage collector through AddReferencedObjects()
    retainer.mObject = object;
