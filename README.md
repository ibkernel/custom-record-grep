# CRGREP

A customizable ranking search tool.

**Currently under development**

## Features

- Multi-pattern searching
- Default tokenizer
- Fuzzy search
- Ranking by analyzing contextual sequence

## Basic Usage

**Must compile with `-std=c++11` option**

compile
```
g++ -std=c++11 main.cpp record.cpp ranking.cpp book_formatter.cpp utils.cpp search.cpp result.cpp -I./
```

run crgrep
```
./a.out "query" "path_to_file_or_dir"
```

## To-do

- Restrict fuzzy search on mandarin data
- Finish Class: result
- Rewrite loading record (remove newline & improve record format support)

## Roadmap

- [x] Ranking tree traversal
- [ ] Customizable score ranking
- [ ] Customizable tokenizer
- [ ] Must have/ Must not have search (advanced query parser)
- [ ] More search algorithm
- [ ] Chinese fuzzy search
- [ ] Support large data

## License

MIT
