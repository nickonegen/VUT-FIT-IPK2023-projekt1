# IPKCP Client #

*Simple and straight–forward CLI client for the AaaS IPKCP (IPK Calculator Protocol) written in C++*

## Usage ##

```bash
./ipkcpc -h <host> -p <port> -m <mode>
```

for Linux, or for Windows:

```bash
./ipkcpc.exe -h <host> -p <port> -m <mode>
```

- `<host>` — IPv4 address of a IPKCP server
- `<port>` — server port to use
- `<mode>` — either "tcp" (for text variant) or "udp" (for binary variant)

All options are required. After connecting, the client will forward valid IPKCP requests from standard input to the connected host, and print valid responses to standard output.
Input is internally parsed, so invalid string not conforming to IPKCP message standard will not be sent.

To terminate connection, send a *bye* command (textual variant) or press <kbd>CTRL+C</kbd> (both variants)

## Building ##

To build the `ipkcpc` binary, simply clone this repo and use `make` or `make release` in the project root. This will build an optimalised binary for common use; alternatively, you may use `make debug` to build a debugging-optimalised binary. For more options, use `make help`.

## Known Problems ##

- In the main loop, interruption signal is handled between messages, but invoking it while waiting for a response will exit incorrectly.

## Code Structure for developers ##

> *For details about the IPKCP protocol itself, see [its documentation](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Project%201/Protocol.md#ipk-calculator-protocol)*

The entry point of the client is `main.cpp` — it showcases the usage of the `IPKCPClient` class, providing simple communication over standard input, but the client itself is implemented in `client.cpp`. For full details about each function, take a look at `client.cpp` doxygen comments. The constructor creates a socket and prepares it for connection, which is initiated via `IPKCPClient::connect()` (required for both UDP and TCP, despite UDP not having to connect — this is for properly adjust the client state).

User interaction is handled via `IPKCPClient::send()` and `IPKCPClient::recv()` methods, which forward the argument to respective `_tcp()` or `_udp()` methods, that encode the message to/from the respective format. Communication is not case–sensitive.

<div align="center" width="100%">
  <img src="https://user-images.githubusercontent.com/84882649/221854586-6d0bb559-eff7-4031-8cfb-ad635d55d89b.png" alt="class flow diagram" width="40%" />
  <br />
  <img src="https://user-images.githubusercontent.com/84882649/221855309-7667150c-d851-4c26-8cc8-03857cb5284c.png" alt="tcp-udp flow diagram" width="40%" />
</div>

`IPKCPClient::send()` will return length of sent message, usable as a control value, following `unistd.h` I/O method standard — positive value indicates a successful write, negative value indicates an error, zero means the transfer was terminated. `IPKCPClient::recv()` will return the recieved message or an empty string, indicating an error or connection termination.

The client class maintains a private `state` variable (see `IPKCPCState` enum in `client.hpp`), kept primarily for graceful exit and error detection.

- `INIT` (0) — connection not formed, initial state
- `READY` (1) — socket created
- `UP` (2) — connected
- `EXPECT_BYE` (3) — (text. only) sent bye command, expecting "BYE"
- `DOWN` (4) — connection ended successfully
- `ERROR` (5) — connection errored

**TCP State–Diagram:**

<img alt="tcp-states" width="55%" src="https://user-images.githubusercontent.com/84882649/221844288-6b526065-9eb9-4008-9748-c071da6ccd94.png" />
  
**UDP State–Diagram:**

<img alt="udp-states" width="35%" src="https://user-images.githubusercontent.com/84882649/221847898-0587b945-e11a-4db8-b93b-ec417c480618.png" />

<br />

The client will only allow sending messages in the `UP` state. Since the textual variant ends the connection with "BYE" on both a proper exit and error, the `EXPECT_BYE` state is set when sending a "BYE" message — thereby if "BYE" is recieved in `EXPECT_BYE`, it indicates a proper termination, otherwise an error occured (most likely a malformed expression). Proper and inproper exits cause different return codes and ending message in `main.cpp`. The `~IPKCPClient()` destructor will attempt a graceful exit if the instance is in the `UP` state, sending a "BYE" message, expecting one back before ending. In `main.cpp`, interruption signal (<kbd>CTRL+C</kbd>) is handled by "calling" the destructor.

## Author ##

- Onegen Something — xonege99@vutbr.cz

Anyone is free to submit a PR, just please make sure you lint (`make lint`, with no functional warnings) and format (`make format`) the code before submission.
