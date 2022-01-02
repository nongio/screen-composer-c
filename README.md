# Screen Composer: a Wayland screen compositor
SC is a very basic wayland compositor

Implements the following protocols:
- xdg-shell
- wlr-layer-shell

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

## Configuration

SC can be configured using .ini files,
it uses the library [inih](https://github.com/benhoyt/inih)

```
sc -c config_file.ini
```

## Aknowledgements

The project uses on [wlroots](https://gitlab.freedesktop.org/wlroots/wlroots/)
much of the codebase is inspired by the following projects:
- [sway](https://github.com/swaywm/sway)
- [cage](https://github.com/Hjdskes/cage)
- [wayfire](https://github.com/WayfireWM/wayfire)


