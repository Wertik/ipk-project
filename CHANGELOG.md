# Changelog

## Functionality

- Sending messages thruogh IPKCP received through standard input
- UDP & TCP communication modes
- Graceful exit with ^C

## Limitations

- `recv` operations are blocking and have no timeout, the client can get easily stuck waiting for a response from the server
- Messages sent through `UDP` mode don't get resent when no response is received from the server
