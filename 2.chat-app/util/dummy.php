<?php

$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);

if (!socket_set_option($socket, SOL_SOCKET, SO_BROADCAST, 1)) {
    throw new Exception("failed to configure");
}

while (true) {
    socket_sendto($socket, pack("C*", 72, 69, 76, 76, 79), 10, 0, "127.0.0.1", 9600);
    sleep(5);
}