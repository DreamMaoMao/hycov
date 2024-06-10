{
  description = "Hyprland Plugins (Hycov)";

  inputs.hyprland.url = "git+https://github.com/hyprwm/Hyprland?submodules=1";

  outputs =
    { self
    , hyprland
    }:
    let
      inherit (hyprland.inputs) nixpkgs;
      inherit (nixpkgs) lib;
      systems = lib.attrNames hyprland.packages;
      withPkgsFor = fn: lib.genAttrs systems (system:
        let
          pkgs = import nixpkgs {
            localSystem.system = system;
            overlays = [
              hyprland.overlays.hyprland-packages
              self.overlays.default
            ];
          };
        in
          fn system pkgs);
    in
    {
      overlays = {
        default = self.overlays.hycov;
        hycov = final: prev: {
          hyprlandPlugins = prev.hyprlandPlugins or {} // {
            hycov = final.callPackage ./default.nix {
              stdenv = final.gcc13Stdenv;
            };
          };
        };
      };

      packages = withPkgsFor (system: pkgs: {
        default = self.packages.${system}.hycov;
        inherit (pkgs.hyprlandPlugins) hycov;
      });

      devShells = withPkgsFor (system: pkgs: {
        default = pkgs.mkShell.override { stdenv = pkgs.gcc13Stdenv; } {
          name = "hyprland-plugins";
          # buildInputs = [ pkgs.hyprland ];
          inputsFrom = [ pkgs.hycov ];
        };
      });
    };
}
