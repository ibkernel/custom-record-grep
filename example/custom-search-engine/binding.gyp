{
  "targets": [
    {
      "target_name": "crgrep",
      "sources": [ "src/crgrep.cc", "src/load_data.cc", "src/search.cc"],
      "include_dirs": ["<!(node -e \"require('nan')\")"]
    }
  ]
}