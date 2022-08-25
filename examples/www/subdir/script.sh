#!/bin/bash

var1=`env`
var2=`cat`
var3="$var1$var2"


echo "HTTP/1.1 200 OK\nContent-type: text/plain,\nContent-length: ${#var3}\n\n$var3"