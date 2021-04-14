# ue4-chest2d-sample

![UE4](https://img.shields.io/badge/UE4-4.25+-blue)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/Nauja/ue4-jetpack-sample/master/LICENSE)

Sample of an interactable 2D chest done with Paper2D.

![Preview](https://github.com/Nauja/ue4-chest2d-sample/raw/master/docs/preview.gif)

This project is an example of how to write an interactable chest in a Paper2D game, with the constraint of
being fully replicated over network.

Prerequisites:
  * [Pixel Perfect 2D Sample](https://github.com/Nauja/ue4-pixelperfect2d-sample)

Features:
  * RPC to replicate **Interact** action
  * Detect possible interaction on server and client
  * Replicate the state of our chest

### RPC to replicate Interact action

Pressing the **Interact** button client-side simply calls an RPC server-side to handle the action.
In a multiplayer game, the client may be totally desynchronized and may not see the correct state
of the game. This step is necessary to let the server validate whether or not the client can interact
with something and to let the server perform the action.

Here is the definition in **ASampleCharacter.h**:

```cpp
UFUNCTION()
void Interact();

UFUNCTION(reliable, Server, WithValidation)
void Server_Interact();
```

Here is the implementation in **ASampleCharacter.cpp**:

```cpp
void ASampleCharacter::Interact()
{
    if (GetLocalRole() < ROLE_Authority) {
        Server_Interact();
    }
    else
    {
        TSet<AActor*> Actors;
        GetOverlappingActors(Actors, TSubclassOf<ASampleInteractableActor>());
        for (auto Actor : Actors)
        { 
            static_cast<ASampleInteractableActor*>(Actor)->Interact(this);
        }
    }
}

bool ASampleCharacter::Server_Interact_Validate()
{
    return true;
}

void ASampleCharacter::Server_Interact_Implementation()
{
    Interact();
}
```

Please note that this RPC has to be called from an Actor owned by the client for it to works.
Per consequence it would not be possible to move it directly into the class of our
interactable Actor. Missing this point can give you a hard time trying to figure out why
the RPC is not called on server.

### Detect possible interaction on server and client

To visually show when the player can use the **Interact** action, we have to detect
when the character is overlapping the chest. To do this, simply use the two **NotifyActorBeginOverlap**
and **NotifyActorEndOverlap** to detect and track overlaps with Actors.
This step is necessary to let the client compute itself whether or not it can interact
with something and give and feedback to the player.

Here is the definition in **SampleInteractableActor.h**:

```cpp
void NotifyActorBeginOverlap(class AActor* Other) override;
void NotifyActorEndOverlap(class AActor* Other) override;

/** Called when at least one actor can interact with this interactable **/
UPROPERTY(BlueprintAssignable, Category="Sample")
FOnBeginInteractableDelegate OnBeginInteractable;

/** Called when no more actors can interact with this interactable **/
UPROPERTY(BlueprintAssignable, Category="Sample")
FOnEndInteractableDelegate OnEndInteractable;

/** Called when at least one actor can interact with this interactable **/
UFUNCTION(BlueprintNativeEvent, Category = "Sample")
void NotifyBeginInteractable();

/** Called when no more actors can interact with this interactable **/
UFUNCTION(BlueprintNativeEvent, Category = "Sample")
void NotifyEndInteractable();
```

Note that the custom **NotifyBeginInteractable** and **NotifyEndInteractable** functions are used to react to the
overlap directly from C++, while **OnBeginInteractable** and **OnEndInteractable** are used to react
from Blueprint.

Here is the implementation in **SampleInteractableActor.cpp**:

```cpp
void ASampleInteractableActor::NotifyActorBeginOverlap(class AActor* Other)
{
    Super::NotifyActorBeginOverlap(Other);

    if (IsValid(Other) && !IsPendingKill() && bIsEnabled)
    {
        if (OverlappingActors.Num() == 0)
        {
            NotifyBeginInteractable();
        }
        OverlappingActors.Add(Other);
    }
}

void ASampleInteractableActor::NotifyActorEndOverlap(class AActor* Other)
{
    Super::NotifyActorEndOverlap(Other);

    if (IsValid(Other) && !IsPendingKill() && bIsEnabled)
    {
        OverlappingActors.Remove(Other);
        if (OverlappingActors.Num() == 0)
        {
            NotifyEndInteractable();
        }
    }
}

void ASampleInteractableActor::NotifyBeginInteractable_Implementation()
{
    OnBeginInteractable.Broadcast();
}

void ASampleInteractableActor::NotifyEndInteractable_Implementation()
{
   OnEndInteractable.Broadcast();
}
```

In this sample you can see two buttons indicating when you can interact with the chest:

![LevelInteractButtons](https://github.com/Nauja/ue4-chest2d-sample/raw/master/docs/editor-levelinteractbutton.png)

They are bound to the chest from Blueprint using **OnBeginInteractable** and **OnEndInteractable** delegates:

![InteractButtonsBP](https://github.com/Nauja/ue4-chest2d-sample/raw/master/docs/editor-interactbutton.png)

### Replicate the state of our chest

In this sample, we use a boolean **bIsEnabled** to tell whether the chest can be interacted with or not.
Once someone interacted with the chest, this boolean becomes **false** and is later resetted to **true** to make
the interaction available once again. This step is necessary to synchronize the client with the server.

Here is the definition in **SampleInteractableActor.h**:

```cpp
/** [Server] Set if the interaction is enabled or not */
UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Sample")
void SetIsEnabled(bool State);
 
/** [Client] Called when bIsEnabled get replicated */
UFUNCTION()
virtual void OnRep_IsEnabled();

/** Indicate if the interaction is enabled **/
UPROPERTY(replicated, ReplicatedUsing = OnRep_IsEnabled)
bool bIsEnabled;
```

And the implementation in **SampleInteractableActor.cpp**:

```cpp
void ASampleInteractableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASampleInteractableActor, bIsEnabled);
}

void ASampleInteractableActor::SetIsEnabled_Implementation(bool State)
{
    bIsEnabled = State;

    // If re-enabled, check currently overlapping actors
    if (bIsEnabled)
    {
        OverlappingActors.Empty();
        GetOverlappingActors(OverlappingActors, TSubclassOf<ASampleCharacter>());
        if (OverlappingActors.Num() == 0)
        {
            NotifyEndInteractable();
        }
        else
        {
            NotifyBeginInteractable();
        }
    }
    else
    {
        NotifyEndInteractable();
    }
}

void ASampleInteractableActor::OnRep_IsEnabled()
{
    SetIsEnabled(bIsEnabled);
}
```

The key is to use the **ReplicatedUsing** attribute to call a function when **bIsEnabled** get replicated.
When the function is called, we simply detect overlapping Actors again.

Now to also replicate the visual state of our chest (closed or opened), we mark its components as replicated:

```cpp
ASampleChestActor::ASampleChestActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , bIsOpened(false)
    , ChestConfig(nullptr)
{
    ClosedComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, ClosedSpriteComponentName);
    ClosedComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ClosedComponent->SetIsReplicated(true);
    ClosedComponent->SetupAttachment(RootComponent);

    OpenedComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, OpenedSpriteComponentName);
    OpenedComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    OpenedComponent->SetIsReplicated(true);
    OpenedComponent->SetupAttachment(RootComponent);
    bReplicates = true;
}
```

### Credits

Sprites are coming from [The Spriters Resource](https://www.spriters-resource.com/).

Font from [FontSpace](https://www.fontspace.com/atlantis-international-font-f31357).

## License

Licensed under the [MIT](LICENSE) License.
