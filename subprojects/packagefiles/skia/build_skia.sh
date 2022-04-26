#!/bin/sh
# dependencies fontconfig-dev

bin/gn gen out/Static
cp args.gn out/Static/
ninja -C out/Static