# CRGREP: A ranking search tool based on contextual relationship analysis

Since the invention of www, data have grown so immensely that even one night of crawling, the amount of data would be tremendous and hard to classify. As a curious programmer who want to search huge data, determining the priority of the search result has become an important and challenging task. This project provides a simple lightweight tool to help us search keywords with ranking priority by proximity matching.

## Features

- Multi-pattern searching 
- error toleranted search (western language only)
- Ranking by proximity relationship.
- Customizable data preprocessor

## Data Format

### Format of the data

Data must follow the format bellow.

```
@id:[your data id]\n
@title:[your data title]\n
@content:[your data content]\n
...
```

**Data must not contain other newline '/n' inside, or it will treat it as another record**

When crgrep is loading the data, it will search the corresponding indexing file alongside (same file name, but with the extension 'tags'). If no indexing file is found, the default scoring mechanism will be used.

### An example of a preprocessed index file

```
c_1 0 1281
t_1 0 94
p_1 94  151
s_1 94  151
p_2 151 177
s_2 151 177
p_3 177 1281
s_3 177 1281
```

**If there are more than one record in a single file, the indexing file is consider corresponding to the first record only, the rest remains default**

Although, crgrep works only if the input data were all well formatted, **crgrep can took care all of the preprocessing stuff for us**. All crgrep need is just some arguments, [see instruction below](#preprocess-data).

## Usage

### Compile crgrep:

*Read the makefile for compiler details*

```
cd ./src
make build-libcld
make compile
./crgrep -h
```

### Preprocess data

Generate the formatted data and the corresponding index file ([see further explanation](https://cwayne.github.io/2017/03/23/crgrep/#How-do-the-preprocessor-work))

```
./crgrep -f path_to_source_dir_or_file -f path_to_formatted_dir [-s path_to_stop_word_file]
```

**The optional argument `path_to_stop_word_file` is a file containing stop words you wish to remove when formatting.**

*Stop Word File Format: a stop word per line (ending with a '\n')*

#### Preprocess rule

##### if the provided path is a file

Crgrep will treat it as a single record, making the first line of the file the title, leaving the rest of the lines the content. On the remaining lines, index's paragraphs are separated by newline, sentense are separated by ending punctuations `. 。 ? ! `
##### if the provided path is a directory

Crgrep will look through every directory and file on the first level, files will be treated just like above, however, **the remaining directory are different, every directory will be considered a single record, all the files in it are being concatenated, others sub-directory ignored.**

### Commands:

| argument | description                              | example                                  |
| :------- | ---------------------------------------- | ---------------------------------------- |
| -p       | source path to file or directory         | `./crgrep -q pattern1 -p path_to_formatted_dir_or_file` |
| -q       | search query, one query at a time        | `./crgrep -q +must_have_pattern -q -must_not_pattern -q "pattern with spaces" -p path` |
| -a       | ascending output order                   | `./crgrep -q pattern1 -p path -a`        |
| -d       | error distance tolerance                 | `./crgrep -q pattern1 -p path -d 3`      |
| -i       | interactive mode                         | `./crgrep -i -p path`                    |
| -o       | output size                              | `./crgrep -q pattern1 -p path -o 3`      |
| -f       | path to raw file or directory to format and path to format destination directory | `./crgrep -f path_to_file_or_dir -f path_to_dest_dir` |
| -s       | path to stop word file                   | `./crgrep -f path_to_file_or_dir -f path_to_dest_dir -s path_to_stop_word_file` |
| -h       | show description message                 | `./crgrep -h`                            |



## Implementation detail

Go check out my [blog](https://cwayne.github.io/2017/03/23/crgrep/ "blog") for implementation details and the lesson I learned on the journey.

## Credit

Thanks to:

- [tclap](http://tclap.sourceforge.net/ "clap") for giving me a simple solution for cmd argument parsing
- [cld](https://github.com/mzsanford/cld 'CLD') for delivering a robust language detection library

## TODO

- More search algorithm !

## Roadmap

- [ ] Support large data (in-file search)
- [ ] Chinese error toleranted search

## License

MIT