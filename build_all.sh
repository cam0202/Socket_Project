mkdir -p build

# UDP
gcc -o build/server_udp -O0 -g common.c server_main.c server_udp.c
gcc -o build/client_udp -O0 -g common.c client_main.c client_udp.c

# TCP
gcc -o build/server_tcp -O0 -g common.c server_main.c server_tcp.c
gcc -o build/client_tcp -O0 -g common.c client_main.c client_tcp.c
