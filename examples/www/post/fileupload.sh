#!/bin/bash

var1=`env`
var2=`cat`
var3="<html><head><title>CGI Script</title></head><body><h1>Headers and Env</h1><pre><code>$var1</code></pre><h1>Body</h1><pre><textarea rows='40' cols='120' placeholder='No body' readonly>$var2</textarea></pre></body></html>"


echo "HTTP/1.1 200 OK\nContent-type: text/html\nContent-length: ${#var3}\n\n$var3"
