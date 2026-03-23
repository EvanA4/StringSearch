# String Search Tests

A collection of scripts for testing different string searching algorithms.

### Basic Testing

Make sure you've created the `bin`, `obj`, and `tests` directories first:

```
mkdir bin obj tests
```

```
make
./bin/tgen <number of tests> <file length> <pattern length> <alphabet len>
```

This creates a `tests.sh` and `expected.txt` to test answers. Test files are placed in the `tests` directory.

```
./tests.sh <string search executable>
ex: ./tests.sh bin/bmh
```

This creates an `actual.txt` you can call `diff` on to ensure it matches `expected.txt`.

### Graph Generation

Make sure you've created the `bin` and `obj` directories first:

```
mkdir bin obj
```

Simply generate the CSV file with these commands:

```
make
./bin/times
```

Graph the CSV file with `graphs.py`.