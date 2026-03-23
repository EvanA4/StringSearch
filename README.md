# String Search Tests

A collection of scripts for testing different string searching algorithms.

### Basic Testing

Make sure you've created a `bin` and `obj` directory first!

```
./bin/tgen <number of tests> <file length> <pattern length> <alphabet len>
```

This creates a `tests.sh` and `expected.txt` to test answers. Test files are placed in the `tests` directory.

```
./tests.sh <string search executable>
ex: ./tests.sh bin/bmh
```

This creates an `actual.txt` you can call `diff` on to ensure it matches `expected.txt`.

### Graph Generation

Simply generate the CSV file with this command:

```
./bin/times
```

Graph the CSV file with `graphs.py`.