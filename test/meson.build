
tests = [
    [
        'sc_read_file_test',
        [files('utils_file.c')],
        [files('example_file.txt')],
    ],
    [
        'output_utils_box_test',
        [files('output_utils_box.c')],
        [],
    ],
    [
        'output_utils_intersect_view',
        [files('output_utils_intersect_view.c')],
        [],
    ],
]

foreach t : tests
    exe = executable(
        t[0],
        t[1],
        include_directories: include_directories('../include'),
        dependencies: [
          server_protos,
          wayland_server,
          wlroots,
          xkbcommon,
          math,
          pixman,
          inih_dep,
          glesv2,
        ],

        link_with : sclib
    )
    test(t[0], exe, args : t[2], suite: 'unit')
endforeach
