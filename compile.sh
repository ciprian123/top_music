#/bin/bash

rm server client
gcc server.c -pthread -lsqlite3 -o server
gcc client.c -pthread -lsqlite3 -o client