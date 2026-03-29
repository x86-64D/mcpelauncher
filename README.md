# mcpelauncher on freebsd without linuxulator, kind of native with shim!

please dont curse on me, my net is slow so ill upload to other git platforms as mirror
u need to mount /proc as well as defined in freebsd handbook
also this is stripped build from macos so please dont hate on me

installation and build is easy

Deps:
```
pkg install cmake gmake git ca_root_nss pkgconf curl openssl png \
  libX11 libXi libXtst mesa-libs mesa-dri \
  qt5-core qt5-gui qt5-widgets qt5-buildtools qt5-qmake qt5-webengine \
  qt5-declarative qt5-svg qt5-quickcontrols qt5-quickcontrols2 \
  libudev-devd libevdev alsa-lib pulseaudio \
  libepoll-shim
```

Build and installation:
```

mkdir build && cd build

CC=clang CXX=clang++ cmake .. -Wno-dev \
  -DCMAKE_BUILD_TYPE=Release \
  -DGAMEWINDOW_SYSTEM=SDL3 \
  -DCMAKE_C_FLAGS="-I/usr/local/include/libepoll-shim" \
  -DCMAKE_CXX_FLAGS="-I/usr/local/include/libepoll-shim" \
  -DCMAKE_EXE_LINKER_FLAGS="-L/usr/local/lib -lepoll-shim -lexecinfo" \
  -DCMAKE_SHARED_LINKER_FLAGS="-L/usr/local/lib -lepoll-shim -lexecinfo"

make -j$(nproc)
sudo make install # or use your fav privilige escalation tool
```
Known issues:
- there is no xbox live login right now
- cannot load FMOD host lib (uses sdl3/pulseaudio as backend)
- no linux-gamepad support
