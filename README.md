# Screen Composer : a Wayland screen compositor

## Building and running

You can build SC with the [meson](https://mesonbuild.com/) build system. It
requires wayland, wlroots, and xkbcommon to be installed.
Simply execute the following steps to build SC:

```
$ meson build
$ ninja -C build
```

By default, this builds a debug build. To build a release build, use `meson
build --buildtype=release`.
