# libghh

data structures and utilities I made and personally use in my C99 projects.

## what's here?

generalized implementations of...
- your obligatory linked list
- hashmap
  - can hash strings or fixed-length keys of any data type
  - open addressed
  - uses fnv-1a hash algorithm
- 2 types of dynamic arrays
  - an opaque object implementation
  - macro-heavy implementation applicable to any kind of pointer
- priority heap

utilities
- runtime memory allocation checking
- strings
- timers

## build

add to a premake project using `include "PATH_TO_LIBGHH_DIR"` and then `links { "ghh" }`
