# randfile
Generates a file index and randomly chooses a file for STDOUT.

The filename of the index is by default ```files.txt```.
This can be changed by setting this environment variable:

```
export RANDFILE_FILENAME=newfilename.txt
```

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
