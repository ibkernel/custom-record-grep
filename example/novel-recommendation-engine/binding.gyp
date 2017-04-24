{
  "targets": [
    {
      "target_name": "crgrep",
      'conditions': [
        [ 'OS=="mac"', {
          #"include_dirs": ['./cld/'],
          'cflags': [
            '-DCLD_WINDOWS'
            '-std=c++11',
            '-stdlib=libc++'
          ],
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS' : [
              '-DCLD_WINDOWS',
              '-std=c++11',
              '-stdlib=libc++'
              ],
            'OTHER_LDFLAGS': ['-stdlib=libc++'],
            },

        }],
        [ 'OS=="linux"', {
          'cflags': [
            '-DCLD_WINDOWS'
            '-std=c++11',
          ],

        }],
      ],
      'include_dirs': [
        "<!(node -e \"require('nan')\")",
        "src/cld/"
      ],
      'libraries': [
        '-L../src/','-lcld', '-lstdc++'
      ],
      "sources": [ "src/crgrep.cc",
                   "src/ranking.cc", 
                   "src/search.cc",
                   "src/record.cc",
                   "src/search_worker.cc",
                   "src/utils.cc",
                    "src/result.cc"
                  ]
    }
  ]
}
