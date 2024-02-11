# hycov
A client overview plugin for Hyprland. 

### What can it do?
- Hycov can tile all of your windows in a single workspace via grid layout.

- After quitting the overview mode, hycov can perfectly recover a window's previous state (fullscreen, floating, size, positon, etc.)

- Hycov supports a variety of trigger methods, such as touch pad gestures, hot corners, and keyboard shortcuts.

- Supports multiple monitors.

- You can change the way that Hycov focuses a window, whether directional or cyclical. (single-shortcut)

https://github.com/DreamMaoMao/hycov/assets/30348075/59121362-21a8-4143-be95-72ce79ee8e95


Anyone is welcome to fork. If you end up improving the plugin, please let me know, and I'll be happy to use your fork.

### Manual Installation

##### NOTE:
1. After Hycov is installed, you will need to logout, then log back in. This may happen automatically, but do not worry. This behaviour is normal.

2. Only supports hyprland source code after 2023-10-22, because the plugin requires this [commit](https://github.com/hyprwm/Hyprland/commit/a61eb7694df25a75f45502ed64b1536fda370c1d) in [hyprland](https://github.com/hyprwm/Hyprland).

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
# When entering overview mode, you can use left mouse button to jump, right mouse button to kill or use keybind

# If you are installing hycov with hyprpm you should not use this line.
# If you are using the NixOS flake you should not use this line.
plugin = /usr/lib/libhycov.so

# If you manually compiled hycov you should not use this line.
exec-once = hyprpm reload

# If you wish to bind a different key than ALT make sure you change the key using the `alt_replace_key` aswell as changing the binding.

# Toggle overview with all clients from all monitors.
bind = ALT,tab,hycov:toggleoverview

# Toggle overview with all clients from all monitors on their respective monitors. 
# Using this mode will invalidate the usage of the `only_active_workspace` or `only_active_monitor` options.
bind = ALT,grave,hycov:toggleoverview,forceall #grave key is the '~' key

# Toggle overview on the active monitor with all clients from all monitors. 
# Using this mode will invalidate the usage of the `only_active_workspace` or `only_active_monitor` options.
bind = ALT,g,hycov:toggleoverview,forceallinone 

# The key binding for directional switch mode.
# Calculate the window closest to the direction to switch focus.
# This keybind is applicable not only to the overview, but also to the general layout.
bind = ALT, left, hycov:movefocus, l
bind = ALT, right, hycov:movefocus, r
bind = ALT, up, hycov:movefocus, u
bind = ALT, down, hycov:movefocus, d

plugin {
    hycov {
        overview_gappo = 60 # Gaps between the outside of clients and the edge of the screen
        overview_gappi = 24 # Gaps between clients
        # Hotarea
        enable_hotarea = 1 # Enable "hotarea". When you cursor enters the hotarea it will trigger overview mode. (Not reccomended if you play games as it can still trigger whilst the game is open)
        hotarea_monitor = all # Monitor name which hotarea is in, default is all
        hotarea_pos = 1 # position of hotarea (1: bottom left, 2: bottom right, 3: top left, 4: top right)
        hotarea_size = 10 # hotarea size, 10x10
        # Trackpad gestures
        swipe_fingers = 4 # finger number of gesture,move any directory
        move_focus_distance = 100 # distance for movefocus,only can use 3 finger to move 
        enable_gesture = 0 # enable gesture

        disable_workspace_change = 0 # disable workspace change when in overview mode
        disable_spawn = 0 # disable bind exec when in overview mode
        auto_exit = 1 # Don't open overview if there are no clients to be opened.
        auto_fullscreen = 0 # Maximize selected window from overview mode after exit.
        only_active_workspace = 0 # Only overview windows from the active workspace
        only_active_monitor = 0 # Only overview windows from the active monitor
        enable_alt_release_exit = 0 # If `1` overview mode will automatically exit once you release the assigned alt key.
        alt_replace_key = Alt_L # Change the modifier key for overview mode, options are [ Alt_L, Alt_R, Super_L, Super_R, Control_L, Control_R, Shift_L, Shift_R ] or a key code e.g. code:133
        alt_toggle_auto_next = 0 # Automatically focus the next client in the overview mode, useful in combination with `enable_alt_release_exit` to quickly toggle between clients.

        click_in_cursor = 1 # when click to jump,the target windwo is find by cursor, not the current foucus window.
        hight_of_titlebar = 0 # height deviation of title bar hight

    }
}

```

# Suggested additional configuration
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


- If you use the `hyprland/workspaces` module in waybar, you should change field {id} to {name}. That way when you are in overview mode it will display as "OVERVIEW".
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

    hycov = {
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
              plugins = [
                hycov.packages.${pkgs.system}.hycov
              ];
              extraConfig = ''
                # your options
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
- Configuration options are not working?
```
If you just installed hycov for the first time please logout and back in again.
```

- The numbers on the waybar are confused

```
1.Please pull the latest waybar source code compilation,
this issue has been fixed in the waybar project, fix date (2023-10-27)

2.Change the {id} field in hyprland/workspace field to {name}
```

- Compilation failure
```
Please pull the latest hyprland source code to compile and install. The plugin relies on a hyprland pr,pr submission date (2023-10-21)
```

- Unable to load
```
Check whether hyprland has been updated, and if so, please recompile hyprcov
```
