#!/bin/bash

myvar=`cat`

echo "HTTP/1.1 200 OK\nContent-type: text/plain,\nContent-length: $CONTENT_LENGTH\n\n$myvar"