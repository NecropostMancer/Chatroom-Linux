#!/bin/sh

hostip=$(ip route show | awk '/default/ {print $3}')

# username,passwd,mysqlhost

if [$# == 3]
then
    ./chatting-room "$hostip:33060" $1 $2
fi

if [$# == 4]
then
    ./chatting-room $3 $1 $2
fi

if [$# == 1]
then
    ./chatting-room "$hostip:33060" root 1234
fi
