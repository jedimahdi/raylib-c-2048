{ pkgs ? import <nixpkgs> { } }:
let
  buildInputs = with pkgs; [
    raylib
    glfw
    wayland
  ];
in
pkgs.mkShell {
  inherit buildInputs;
  nativeBuildInputs = [ pkgs.pkg-config ];
  LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath buildInputs;
}
