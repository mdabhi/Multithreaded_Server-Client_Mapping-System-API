#!/bin/bash

EXEC=runmap
MAIN=multiserver
MAPPER=roadmap
CLIENT=clients
CC=gcc

rm -f $EXEC

$CC -o $EXEC $MAIN.c $MAPPER.c -lpthread
$CC -o $CLIENT $CLIENT.c -lpthread

if [[ $? -eq 0 ]]
then
	./$EXEC
fi
