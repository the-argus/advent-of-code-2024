{
  description = "C Environment for advent of code";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    nixpkgs,
    flake-utils,
    ...
  }: let
    supportedSystems = let
      inherit (flake-utils.lib) system;
    in [
      system.aarch64-linux
      system.x86_64-linux
    ];
  in
    flake-utils.lib.eachSystem supportedSystems (system: let
      pkgs = import nixpkgs {inherit system;};
    in {
      devShell =
        pkgs.mkShell
        {
          packages =
            (with pkgs; [
              gdb
              valgrind
              zig_0_13
              (pkgs.writeScriptBin "build" "zig build install_day$1")
              (pkgs.writeScriptBin "run" "gdb --args ./zig-out/bin/day$1 ./day$1/input")
            ]);
        };

      formatter = pkgs.alejandra;
    });
}
