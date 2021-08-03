# TLPI

code from *The Linux Programming Interface*

## How to build & run

```bash
./build.sh <input C file>

# can pass up to 4 args
./run.sh <arg1> <arg2> ...

# if you need to interact with linux container
docker run -it --entrypoint /bin/bash tlpi
```
