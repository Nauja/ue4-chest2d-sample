# ue4-chest2d-sample

![UE4](https://img.shields.io/badge/UE4-4.25+-blue)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/Nauja/ue4-jetpack-sample/master/LICENSE)

Sample of a 2D chest done with Paper2D.

![Preview](https://github.com/Nauja/ue4-chest2d-sample/raw/master/docs/preview.gif)

This project is an example of how to write an interactable chest in a Paper2D game, with the constraint of
being fully replicated over network.

Features:
  * Pixel perfect camera/sprites
  * True sprite colors
  * Custom **Interact** action (E)
  * C++ RPC to replicate **Interact** action
  * C++ interactable chest

### Pixel perfect sprites

wip

### True sprite colors

One common problem is to keep the colors of your sprites intact.
By default, many post processing effects are enabled and are causing sprites
not to be rendered correctly. In this sample, all post processing effects are disabled
with this single command:

```cpp
APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
check(Controller);
Controller->ConsoleCommand(TEXT("showflag.postprocessing 0"));
```

Also, in **Project Settings > Engine > Rendering**, make sure to uncheck the **Mobile HDR** option and all post processing options such as **Bloom**, **Auto Exposure**, **Anti-Aliasing**, etc.

### Custom Interact action

wip

### RPC to replicate Interact action

wip

### Interactable chest

wip
