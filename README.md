## ft_ping

ft_ping is a simple implementation of the core features of the ping utility written in C. It allows a machine to ping anyhost on the network, to determine its status and latency.

### Usage

```
Usage: ft_ping [OPTIONS...] <HOST> ...

Send ICMP ECHO_REQUEST packets to network hosts.

Options:
  -c, --count <NUMBER>        stop after sending NUMBER packets
  -t, --timeout <SECONDS>     stop after SECONDS seconds
  -i, --interval <SECONDS>    send a packet every SECONDS seconds
  -s, --size <BYTES>          set packet payload size to BYTES bytes(if too small, no timestamp will fit)
  -p, --pattern <HEX>         set the payload repeating pattern to HEX (must be a hexadecimal string)
  -q, --quiet                 do not show packet reception output
  -v, --verbose               verbose output
  -?, --help                  give this help list`
```

### Installation

To build ft_ping from source, clone the repository and make the binary :

> Warning: since ft_ping requires permission to create raw sockets, compiling the binary requires root access
```
git clone https://github.com/Samael1254/ft_ping
cd ft_ping
make
```
