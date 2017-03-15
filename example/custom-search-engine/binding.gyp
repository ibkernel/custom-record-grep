{
  "targets": [
    {
      "target_name": "crgrep",
      'conditions': [
        [ 'OS=="mac"', {

          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11','-stdlib=libc++'],
            'OTHER_LDFLAGS': ['-stdlib=libc++'],
            },

        }],
      ],
      "sources": [ "src/crgrep.cc", "src/ranking.cc", "src/search.cc", "src/record.cc"
      ,"src/search_worker.cc", "src/utils.cc"],
      "include_dirs": ["<!(node -e \"require('nan')\")"]
    }
  ]
}