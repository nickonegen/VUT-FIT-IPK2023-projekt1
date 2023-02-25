# IPKCP Client #

> ⚠️ **HEAVILY WORK IN PROGRESS** ⚠️
>
> This project is very early stages of development, lacking important parts of the program and this should not be used by end users as of yet!

*Simple and straight–forward CLI client for the AaaS IPKCP (IPK Calculator Protocol) written in C++*

## Usage ##

```bash
./ipkcpc -h <host> -p <port> -m <mode>
```

for Linux, or for Windows:

```bash
./ipkcpc.exe -h <host> -p <port> -m <mode>
```


* `<host>` — IPv4 address of a IPKCP server
* `<port>` — server port to use
* `<mode>` — either "tcp" (for text variant) or "udp" (for binary variant)

All options are required. After connecting, the client will forward valid IPKCP requests from standard input to the connected host, and print valid responses to standard output.
Input is internally parsed, so invalid string not conforming to IPKCP message standard will not be sent.

To terminate connection, send a *bye* command (textual variant) or press <kbd>CTRL+C</kbd> (both variants)

## Building ##

To build the `ipkcpc` binary, simply clone this repo and use `make` or `make release` in the project root. This will build an optimalised binary for common use; alternatively, you may use `make debug` to build a debugging-optimalised binary. For more options, use `make help`.

## Known Problems ##

> To be written...

## Code Structure for developers ##

> *For details about the IPKCP protocol itself, see [its documentation](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Project%201/Protocol.md#ipk-calculator-protocol)*

`main.cpp` is the entry point of the program, handling user interaction. The main function creates an instance of `IPKCPClient` class, implemented in `client.cpp`. For full details about each function, take a look at `client.cpp` doxygen comments. The constructor creates a socket and prepares it for connection, which is initiated via `IPKCPClient::connect()` (function forms a TCP connection; in case of datagram communication, this function sends a testing message, validating the connection).

User interaction is handled via `IPKCPClient::send()` and `IPKCPClient::recv()` methods, which forward the argument to respective `_tcp()` or `_udp()` methods. These methods also validate each message and parse it to expected IPKCP format.

> Some program flow diagram here...

Interaction methods return length of recieved or sent message, which also serves as a control value, following `unistd.h` I/O methods — positive value indicates a successful read/write, negative value indicates an error, zero means the transfer ended.

Given that the textual variant is stateful, the client class keeps a private `state` variable, kept primarily for graceful exit.

* `CSTATE_INIT` (0) — connection not formed, initial state
* `CSTATE_NEW` (1) — socket created
* `CSTATE_UP` (2) — (text.) connected; (bin.) verified
* `CSTATE_END` (3) — (text. only) connection ended

Connection can be terminated using SIGINT (<kbd>CTRL+C</kbd>) or the *bye* command in textual variant, both events triggering the use of client's destructor `~IPKCPClient()`. SIGINT thereby sends the *bye* command and awaits a reply in the same manner as normally using the *bye* command. The destructor then closes the socked file descriptor and ends.

## Author ##

* Onegen Something — xonege99@vutbr.cz

Anyone is free to submit a PR, just please make sure you lint (`make lint`, with no functional warnings) and format (`make format`) the code before submission.