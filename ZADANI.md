# IPK Project 1: IPKCP Host #

> *copied from [NESFIT](https://github.com/nesfit)[/IPK-Projekty](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Project%201/Protocol.md) on [GITFit](https://git.fit.vutbr.cz)*

Your task is to implement a client for the IPK Calculator Protocol (IPKCPC).

The client is started using: `ipkcpc -h <host> -p <port> -m <mode>` where `host` is the IPv4 address of the server, `port` the server port, and `mode` either `tcp` or `udp` (e.g., `ipkcpc -h 1.2.3.4 -p 2023 -m udp`).

The client will use the appropriate protocol variant depending on the selected mode (binary for UDP, textual for TCP). It will read a list of commands from the standard input, one per line, and print the received responses on the standard output. Responses should be formatted according to the examples below. Any (client) errors should be printed to the standard **error output**.

The client should react to the *Interrupt* signal (<kbd>CTRL+C</kbd>) by gracefully exiting, cleanly terminating the connection as appropriate for the selected mode.

## Submission ##

The submission should follow all the requirements described in the general [README](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Project%201/README.md). The project should build a single executable called `ipkcpc` in the project root using the default `make` target. This executable need not be self-contained, and can be implemented as a wrapper/launcher, but it should conform to the CLI interface described above.

Upload the project structure as **one** archive to [StudIS](https://www.vut.cz/studis/student.phtml?sn=zadani_odevzdani&registrace_zadani_id=929666&apid=231006) until 21.03.2023 23:59.

## IPK Calculator Protocol ##

IPKCP is an application protocol for communicating with an *Arithmetic as a Service* (AaaS) provider. Clients send an expression to be evaluated, and, assuming the expression is well-formed, receive a response containing the resulting value. IPKCP uses a prefix notation with generalized n-ary operators.

Using [ABNF notation](https://www.rfc-editor.org/info/std68), a well-formed value is formally defined as:

```text
operator = "+" / "-" / "*" / "/"
expr = "(" operator 2*(SP expr) ")" / 1*DIGIT
query = "(" operator 2*(SP expr) ")"
```

A server implementation MAY define additional custom operators, but it MUST support at least these four.

IPKCP has a binary and a textual variant, described below.

### Binary Variant ###

The binary variant is stateless and operates over UDP. Every message begins with an opcode specifying the type of the message and contains the payload length in bytes. The payload data is encoded as an ASCII string.

The request data contains a well-formed expression as defined above. A response with the OK status contains the computed numerical value; Error responses contain an error message.

#### OP-Codes ####

| Opcode   | Value |
|:---------|:------|
| Request  | 0     |
| Response | 1     |

#### Request Message Format ####

```text
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +---------------+---------------+-------------------------------+
 |     Opcode    |Payload Length |          Payload Data         |
 |      (8)      |      (8)      |                               |
 +---------------+---------------+ - - - - - - - - - - - - - - - +
 :                     Payload Data continued ...                :
 + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 |                     Payload Data continued ...                |
 +---------------------------------------------------------------+
```

#### Status-Codes ####

| Status | Value |
|:-------|:------|
| OK     | 0     |
| Error  | 1     |

#### Response Message Format ####

```text
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +---------------+---------------+---------------+---------------+
 |     Opcode    |  Status Code  |Payload Length | Payload Data  |
 |      (8)      |      (8)      |      (8)      |               |
 +---------------+---------------+---------------+ - - - - - - - +
 :                     Payload Data continued ...                :
 + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 |                     Payload Data continued ...                |
 +---------------------------------------------------------------+
```

### Textual Variant ###

The textual variant is stateful and operates over TCP. Communication begins in the init state with a hello greeting from the client, to which the server responds with *HELLO* if accepting requests, transitioning to the established state. Afterwards, the client can send queries with the solve command and should receive the computed result. Any number of solve queries can be sent in this state, including none. When the client wishes to terminate the connection, it can send a *BYE* command. The server will reply with *BYE* before terminating its part of the connection.

In case of a malformed, illegal, or otherwise unexpected message, the server will respond with *BYE* and close the connection.

#### Textual Message Format ####

```text
hello = "HELLO" LF
solve = "SOLVE" SP query LF
result = "RESULT" SP 1*DIGIT LF
bye = "BYE" LF
```

## I/O Examples ##

### Binary ###

Input:

```elm
(+ 1 2)
(a b)
```

Output:

```elm
OK:3
ERR:<error message>
```

### Textual ###

Input:

```elm
HELLO
SOLVE (+ 1 2)
BYE
```

Output:

```elm
HELLO
RESULT 3
BYE
```
