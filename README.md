# randfile
Generates a file index and randomly chooses a file for STDOUT.
The filename of the index is currently files.txt and cannot be changed.

# create index
```
./randfile ~/Downloads/Movies
```

# randomly get one file
```
./randfile
```
# pass one file to mpv
```
./randfile -p | xargs mpv
```
