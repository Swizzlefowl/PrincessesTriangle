{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    { self
    , nixpkgs
    , flake-utils
    }:
    flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = nixpkgs.legacyPackages.${system};

      vulkan-raii = pkgs.clang13Stdenv.mkDerivation {
        name = "vulkan-raii";
        src = nixpkgs.lib.cleanSource ./.;

        nativeBuildInputs = with pkgs; [
          cmake
          python3
        ];

        buildInputs = with pkgs; [
          pkg-config
          xorg.libX11
          xorg.libXau
          xorg.libXdmcp
          xorg.libXrandr
          xorg.libXinerama
          xorg.libXcursor
          xorg.libXi
          xorg.libxcb
          shaderc
          doxygen
        ];
      };
    in
    {
      packages = {
        inherit vulkan-raii;
        default = vulkan-raii;
      };

      devShells.default = pkgs.mkShell.override { inherit (vulkan-raii) stdenv; } {
        inputsFrom = [ vulkan-raii ];

        packages = with pkgs; [
          vulkan-validation-layers
          gdb
        ];

        shellHook = ''
          export LD_LIBRARY_PATH="${pkgs.vulkan-loader}/lib":"$LD_LIBRARY_PATH"
        '';
      };
    });
}
