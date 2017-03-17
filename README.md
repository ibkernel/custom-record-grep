# CRGREP

A customizable ranking search tool.

## Features

- Multi-pattern searching
- Customizable tokenizer 
- Fuzzy search
- Ranking by analyzing contextual sequence

## Input file format

#### Data file

In each file, every record must have 3 prefix `@id:`, `@title:`, `@content:` following with the coresponding data, and with a ending newline indicating end of such data.

*DATA MUST NOT CONTAIN OTHER NEWLINE '/n' INSIDE, OR IT WILL TREAT IT AS ANOTHER RECORD*

```
@id:[your data id]\n
@title:[your data title]\n
@content:[your data content]\n
...
```

When crgrep is loading the data, it will search the corresponding indexing file alongside ( same file name, but with the extension 'tags' ). If no indexing file is found, the default scoring mechanism will be used.

**If there are more than one record in a single file, the indexing file is consider corresponding only to the first record, the rest remains default**

#### The `.tags` file

In the `.tags` file, each line has three special variable: `tag_name` `open_location`  `close_location`. Having all of these tags' starting and ending locations, crgrep will then build a 4-level tree. After setting the data all up, we can seek out the enclosing tags of the found location rapidly, therefor, calculating the result score more accurately than the default way: scoring by the appearance time.

An example of the preprocessed index file:

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

TODO TREE SIMULATION IMAGE

Although, crgrep works only if the input data were all well formatted, crgrep can took care all of the preprocessing stuff for us. All crgrep need is just some arguments . See instruction below.

## Usage

### Step 0:

naviagate to ./src

### Step 1:

build the cld library

```
make build-libcld
```

### Step 2:

compile crgrep

```
make compile
```

*Read the makefile for compiler details*

### Commands:

```
./crgrep -q "search pattern" -p -i -d -f
```

TODO

## Used library

- [tclap](http://tclap.sourceforge.net/ "clap")
- [cld](https://github.com/mzsanford/cld 'CLD')

## To-do

- [ ] Customizable score ranking
- [ ] Customizable tokenizer
- [ ] Rewrite loading record (remove newline & improve record format support)

## Roadmap

- [ ] Ranking tree traversal
- [ ] Customizable score ranking
- [ ] Customizable tokenizer
- [ ] Must have/ Must not have search (advanced query parser)
- [ ] More search algorithm
- [ ] Chinese fuzzy search
- [ ] Support large data

## License

MIT