#!/bin/bash
echo "hola caracola" > mount-point/prueb.txt
echo "caracola" > mount-point/p.txt
echo "wlµgpwmetpwtwngwqbgkjbwghola caracola" > mount-point/b.txt
ln mount-point/prueb.txt mount-point/prueb2.txt
ln mount-point/prueb.txt mount-point/prueb3.txt
ln mount-point/prueb.txt mount-point/prueb4.txt
ln mount-point/b.txt mount-point/b2.txt
ls -la mount-point
