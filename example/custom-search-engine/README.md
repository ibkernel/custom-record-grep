# Custom search engine using crgrep
Thanks to node.js' NAN module, it is now simplier to build a web-based search engine. This is a demo using crgrep as the searching backend. 

*There are some slighlty code change in order to make it a native addon.*

**Currently under development**

## Requirement

- node-gyp

## Usage

build cld library
```
make build-libcld:
```

Build node native c++ addon
```
node-gyp rebuild
```
Run server
```
npm start
```

## Roadmap

- [x] update crgrep to-date
- [ ] Web page