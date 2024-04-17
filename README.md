# hycov
A Hyprland overview mode plugin, a new tiling WM workflow.  

> [!NOTE]
> This repository only maintains the hyprland version corresponding to each hycov release, please do not bring up the issue of non-release versions, because I do not have enough time to track every commit.

### What can it do?
- Hycov can tile all of your windows in a single workspace via grid layout.

- After quitting the overview mode, hycov can perfectly recover a window's previous state (fullscreen, floating, size, positon, etc.)

- Hycov supports a variety of trigger methods, such as touch pad gestures, hot corners, and keyboard shortcuts.

- Supports multiple monitors.

- You can change the way that Hycov focuses a window, whether directional or cyclical. (single-shortcut)

https://github.com/DreamMaoMao/hycov/assets/30348075/59121362-21a8-4143-be95-72ce79ee8e95


Anyone is welcome to fork. If you end up improving the plugin, please let me know, and I'll be happy to use your fork.

### Manual Installation

> [!NOTE]
> 1. After Hycov is installed, you will need to logout, then log back in. This may happen automatically, but do not worry. This behaviour is normal.
> 2. Only supports hyprland source code after 2023-10-22, because the plugin requires this [commit](https://github.com/hyprwm/Hyprland/commit/a61eb7694df25a75f45502ed64b1536fda370c1d) in [hyprland](https://github.com/hyprwm/Hyprland).
> 3. Each release of hycov corresponds to each release of hyprland. If you are using a release version of hycov, but you are using the latest hyprland-git, this may not be available. 

##### Using meson and ninja:

```shell
git clone https://github.com/DreamMaoMao/hycov.git
cd hycov
sudo meson setup build --prefix=/usr
sudo ninja -C build
sudo ninja -C build install # `libhycov.so` path: /usr/lib/libhycov.so
```

##### Using CMake:

```shell
git clone https://github.com/DreamMaoMao/hycov.git
cd hycov
bash install.sh # `libhycov.so` path: /usr/lib/libhycov.so
```

##### Using hyprpm:

```shell
hyprpm update
hyprpm add https://github.com/DreamMaoMao/hycov
hyprpm enable hycov
```

### Usage (hyprland.conf)

```conf
# When entering overview mode, you can use left-button to jump, right-button to kill or use keybind

#  If you are installing hycov with hyprpm, you should comment out this 
plugin = /usr/lib/libhycov.so

# If you are installing hycov by manually compile , you should comment out this 
exec-once = hyprpm reload

# bind key to toggle overview (normal)
bind = ALT,tab,hycov:toggleoverview

# bind key to toggle overview (force mode, not affected by `only_active_workspace` or `only_active_monitor`)
bind = ALT,grave,hycov:toggleoverview,forceall #grave key is the '~' key

# bind key to toggle overview (shows all windows in one monitor, not affected by `only_active_workspace` or `only_active_monitor`)
bind = ALT,g,hycov:toggleoverview,forceallinone 

# The key binding for directional switch mode.
# Calculate the window closest to the direction to switch focus.
# This keybind is applicable not only to the overview, but also to the general layout.
bind=ALT,left,hycov:movefocus,l
bind=ALT,right,hycov:movefocus,r
bind=ALT,up,hycov:movefocus,u
bind=ALT,down,hycov:movefocus,d

# if you want that focusmove can cross monitor, use this
bind=ALT,left,hycov:movefocus,leftcross
bind=ALT,right,hycov:movefocus,rightcross
bind=ALT,up,hycov:movefocus,upcross
bind=ALT,down,hycov:movefocus,downcross

plugin {
    hycov {
        overview_gappo = 60 # gaps width from screen edge
        overview_gappi = 24 # gaps width from clients
        enable_hotarea = 1 # enable mouse cursor hotarea, when cursor enter hotarea, it will toggle overview    
        hotarea_monitor = all # monitor name which hotarea is in, default is all
        hotarea_pos = 1 # position of hotarea (1: bottom left, 2: bottom right, 3: top left, 4: top right)
        hotarea_size = 10 # hotarea size, 10x10
        swipe_fingers = 4 # finger number of gesture,move any directory
        move_focus_distance = 100 # distance for movefocus,only can use 3 finger to move 
        enable_gesture = 0 # enable gesture
        auto_exit = 1 # enable auto exit when no client in overview
        auto_fullscreen = 0 # auto make active window maximize after exit overview
        only_active_workspace = 0 # only overview the active workspace
        only_active_monitor = 0 # only overview the active monitor
        enable_alt_release_exit = 0 # alt swith mode arg,see readme for detail
        alt_replace_key = Alt_L # alt swith mode arg,see readme for detail
        alt_toggle_auto_next = 0 # auto focus next window when toggle overview in alt swith mode
        click_in_cursor = 1 # when click to jump,the target windwo is find by cursor, not the current foucus window.
        hight_of_titlebar = 0 # height deviation of title bar height
        show_special = 0 # show windwos in special workspace in overview.

    }
}

```

# suggested additional configuration
- when `auto_fullscreen=1` is set, you can also set the border color to mark the maximize state and bind key to control fullscreen maximize state.
```
windowrulev2 = bordercolor rgb(158833),fullscreen:1 # set bordercolor to green if window is fullscreen maximize
# toggle fullscreen maximize
bind = ALT,a,fullscreen,1
```
<details>
<summary>detail video</summary>

https://github.com/DreamMaoMao/hycov/assets/30348075/15ba36c2-1782-4ae0-8ac1-d0ca98e01e0f

</details>


- if you use the `hyprland/workspaces` module in waybar,you should change field {id} to {name}. It will let you know you are in overview mode.
```
"hyprland/workspaces": {
    "format": "{name}",
    "on-click":"activate",
},
```

<details>
<summary>detail picture</summary>

![image](https://github.com/DreamMaoMao/hycov/assets/30348075/332f4025-20c1-4a44-853b-1b5264df986e)
![image](https://github.com/DreamMaoMao/hycov/assets/30348075/500d9fd7-299b-48bc-ab72-146f263044a5)

</details>


# Alt switch mode
```conf
enable_alt_release_exit = 1
alt_toggle_auto_next = 0 # auto focus next window when enter overview in alt mode
# alt_replace_key = Alt_L # If your MainKey of toggleoverview is ALt, you can ignore it
```
## operation
such as `alt + tab`:

- 1.`alt + tab` will enter overview when you not in overview(please hold alt,don't make it release)

- 2.`alt + tab` will switch window focus circularly when you in overview. (please hold alt,don't make it release)

- 3.when you release `alt` , it will auto exit overview.

<details>
<summary> If you don't want to use `alt` as MainKey in alt mode</summary>

such as use `super` to repalce `alt`
- 1.bind toggleoverview
```
bind = SUPER,tab,hycov:toggleoverview
```
- 2.use `alt_replace_key` to specify what is the detection key on release.
```
# use keyname
alt_replace_key = Super_L # Alt_L,Alt_R,Super_L,Super_R,Control_L,Control_R,Shift_L,Shift_R

# use keycode
alt_replace_key = code:133 # use `xev` command to get keycode
```

</details>

### NixOS with home—manager

```nix
# flake.nix

{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    home-manager = {
      url = "github:nix-community/home-manager";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    hyprland.url = "github:hyprwm/Hyprland";

    hycov={
      url = "github:DreamMaoMao/hycov";
      inputs.hyprland.follows = "hyprland";
    };
  };

  outputs = { nixpkgs, home-manager, hyprland, hycov, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      homeConfigurations."user@hostname" = home-manager.lib.homeManagerConfiguration {
        pkgs = nixpkgs.legacyPackages.x86_64-linux;

        modules = [
          hyprland.homeManagerModules.default
          {
            wayland.windowManager.hyprland = {
              enable = true;
              package = hyprland.packages."${pkgs.system}".hyprland;
              plugins = [
                hycov.packages.${pkgs.system}.hycov
              ];
              extraConfig = ''
                bind = ALT,tab,hycov:toggleoverview
                bind=ALT,left,hycov:movefocus,l
                bind=ALT,right,hycov:movefocus,r
                bind=ALT,up,hycov:movefocus,u
                bind=ALT,down,hycov:movefocus,d

                plugin {
                    hycov {
                      overview_gappo = 60 #gaps width from screen
                      overview_gappi = 24 #gaps width from clients
                	    hotarea_size = 10 #hotarea size in bottom left,10x10
                	    enable_hotarea = 1 # enable mouse cursor hotarea
                    }
                }
              '' + ''
                # your othor config
              '';
            };
          }
          # ...
        ];
      };
    };
}
```
## Frequently Asked Questions
- some config not work, or the plugin not work.
```
if you use install hycov at first time,please try logout and relogin again.
```

- The numbers on the waybar are confused

```
1.Please pull the latest waybar source code compilation,
this issue has been fixed in the waybar project, fix date (2023-10-27)

2.Change the {id} field in hyprland/workspace field to {name}
```

- Compilation failure
```
Please pull the latest hyprland source code to compile and install.
The plugin relies on a hyprland pr,pr submission date (2023-10-21)
```

- Unable to load
```
Check whether hyprland has been updated,
and if so, please recompile hyprcov
```

- build fail with message `No such file or directory #include <wlr/xxx>`
```
#step1
yay -R hyprland-git wlroots-git

#step2
sudo rm -rf /usr/include/hyprland
sudo rm -rf /usr/include/wlr
sudo rm -rf/usr/local/include/hyprland
sudo rm -rf /usr/local/include/wlr

#step3
yay -S  wlroots-git hyprland-git
```


## Some cool use cases
<details>
<summary> hycov + [hyprland-easymotion](https://github.com/zakk4223/hyprland-easymotion) </summary>


https://github.com/DreamMaoMao/hycov/assets/30348075/486b08f1-be0d-4647-90a3-2029961402cd


```conf
bind = ALT,tab, exec, ~/.config/hypr/scripts/hycov-easymotion.sh
```

```bash
#!/bin/bash

workspace_name=$(hyprctl -j activeworkspace | jq -r '.name')

if [ "$workspace_name" = "OVERVIEW" ]; then
    hyprctl dispatch hycov:toggleoverview
else
    hyprctl dispatch hycov:toggleoverview
    hyprctl dispatch 'easymotion action:hyprctl --batch "dispatch focuswindow address:{} ; dispatch hycov:toggleoverview"'
fi
```

</details>
