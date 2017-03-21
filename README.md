# CRGREP: A ranking search tool based on contextual relationship analysis

Since the invention of www, data have grown so immensely that even one night of crawling, the amount of data would be tremendous and hard to classify. As a curious programmer who want to search huge data, determining the priority of the search result has become an important and challenging task. This project provides a simple tool to help us search keywords with ranking priority by match location analysis.

## Features

- Multi-pattern searching 
- error toleranted search (western language only)
- Ranking by analyzing contextual relationship.
- Customizable data preprocessor


## Data Format Introduction

### Format of the data
Data must follow the format bellow.

```
@id:[your data id]\n
@title:[your data title]\n
@content:[your data content]\n
...
```
**Data must not contain other newline '/n' inside, or it will treat it as another record**

When crgrep is loading the data, it will search the corresponding indexing file alongside ( same file name, but with the extension 'tags' ). If no indexing file is found, the default scoring mechanism will be used.

### An example of a preprocessed index file

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
**If there is more than one record in a single file, the indexing file is consider corresponding to the first record only, the rest remains default**

Although, crgrep works only if the input data were all well formatted, **crgrep can took care all of the preprocessing stuff for us**. All crgrep need is just some arguments, see instruction below.

## Usage

### Compile crgrep:
*Read the makefile for compiler details*

```
cd ./src
make build-libcld
make compile
```

### Preprocess data

Generate index file for ranking and create the formatted data

```
./crgrep -f path_to_source_dir_or_file -f path_to_formatted_dir [-f path_to_stop_word_file]
```
**The optional argument `path_to_stop_word_file` is a file containing stop words you wish to remove when formatting.**
*Stop Word File Format: a stop word per line (ending with a '\n')*


### Commands:

```
./crgrep -q "search pattern" -p -i -d
```
[TODO]

## Implement details

Go check out my [blog](https://cwayne.github.io "blog") for implementation details.


## Used library

- [tclap](http://tclap.sourceforge.net/ "clap")
- [cld](https://github.com/mzsanford/cld 'CLD')

## Roadmap

- [x] Customizable score ranking
- [x] Customizable tokenizer
- [x] Must have/ Must not have search
- [ ] Refactor Record, add a new Search class to implement the ranking logic
- [ ] More search algorithm
- [ ] Chinese error toleranted search
- [ ] Support large data (in-file search)

## License

MIT
