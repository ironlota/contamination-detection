{ stdenv
, lib
, pkgs
, pkgconfig
, cmake
, ninja
, boost171
, opencv4
, gtkmm3
, spdlog
, fmt
, clang-tools
}:
  
let
  stdenvClang              = pkgs.clangStdenv;
in
  stdenvClang.mkDerivation {
    name                   = "mycelium-contamination-detection";
    version                = "1.0";
    src                    = builtins.filterSource (p: t: lib.cleanSourceFilter p t && baseNameOf p != "build") ./.;

    nativeBuildInputs      = [
      stdenvClang
      cmake
      ninja
      pkgconfig
      clang-tools
    ];

    propagatedBuildInputs  = [

    ];
  
    buildInputs            = [
      boost171
      opencv4
      gtkmm3
      spdlog
      fmt
    ];

    cmakeFlags             = [
      "-DCMAKE_BUILD_TYPE=Release"
      "-GNinja"
    ];

    preConfigure           = ''
      cmakeFlags="$cmakeFlags"
    '';

    preBuild               = ''
      ninja clean
    '';

    buildPhase             = ''
      ninja
    '';

    makeTarget             = "mycelium-contamination";
    enableParallelBuilding = true;

    doCheck                = false;
    checkTarget            = "test";

    installPhase           = ''
      mkdir -p $out/bin $out/bin/logs $out/bin/external $out/nix-support;
    '';
  }
