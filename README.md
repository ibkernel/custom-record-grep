# CRGREP

A customizable ranking search tool.

**Currently under development**

## Features

- Multi-pattern searching
- Default tokenizer (Will create an index file)
- Fuzzy search
- Ranking by analyzing contextual sequence

## Usage

### Input file format
#### Data file
In each file, every record must have 3 prefix `@id:`, `@title:`, `@content:` following with the coresponding data, and with an ending newline indicating end of such data

*DATA MUST NOT CONTAIN OTHER NEWLINE INSIDE, OR IT WILL SKIP THAT RECORD*
```
@id:[your data id]\n
@title:[your data title]\n
@content:[your data content]\n
...
```
#### Index file
When crgrep found the location of the search pattern, it needs an processed index of the data to calculate the ranking score. 

Consider this an example of the format of the indexed file.
```
c_1	0	1281
t_1	0	94
p_1	94	151
s_1	94	151
p_2	151	177
s_2	151	177
p_3	177	1281
s_3	177	1281
```

Thankfully, crgrep can do the indexing and the formatting for us, all crgrep need is just two path `path_to_raw_dir` and `path_to_processed_dir`. See instruction below.


**must compile with `-std=c++11` option**

compile
```
make
```

run crgrep
```
./a.out "query" "path_to_file_or_dir"
```

## Used library

- tclap
- cld 

## To-do

- [ ] Customizable score ranking
- [ ] Customizable tokenizer
- [ ] Rewrite loading record (remove newline & improve record format support)

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
