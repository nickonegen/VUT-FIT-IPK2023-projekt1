# IPK Project 1: IPKCP Host #

> *copied from [NESFIT](https://github.com/nesfit)[/IPK-Projekty](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Project%201/Protocol.md) on [GITFit](https://git.fit.vutbr.cz)*

Your task is to implement a client for the IPK Calculator Protocol (IPKCPC) in C/C++, C# or Common Lisp.

The client is started using: `ipkcpc -h <host> -p <port> -m <mode>` where `host` is the IPv4 address of the server, `port` the server port, and `mode` either `tcp` or `udp` (e.g., `ipkcpc -h 1.2.3.4 -p 2023 -m udp`).

The client will use the appropriate protocol variant depending on the selected mode (binary for UDP, textual for TCP). It will read a list of commands from the standard input, one per line, and print the received responses on the standard output. Responses should be formatted according to the examples below. Any (client) errors should be printed to the standard **error output**.

The client should react to the *Interrupt* signal (<kbd>CTRL+C</kbd>) by gracefully exiting, cleanly terminating the connection as appropriate for the selected mode.

If you decide to implement the project in C++, let it be by the C++20 standard. If you decide to implement the project in C#, make your implementation is .NET 6+ compatible and out-of-the-box runnable on the reference virtual machine with the provided developer environment active. Common Lisp implementations will be tested using SBCL. *Makefile* is mandatory for all programming languages. Compilation of your projects will be done by calling make. For C/C++, compiling with Makefile is native, for C# you can call the standard `dotnet build` in the appropriate phony. For Common Lisp, you can call (`asdf:make`). The goal of using Makefile is to make the compilation of projects agnostic to the programming language used.

The project should be as portable and multi-platform as possible (e.g., try to show it at least once somewhere in your code that you are aware of specifics between Windows and Unix operating systems).

Project source codes should be usable and meaningful, adequately annotated and formatted, and broken down into functions and modules. The program should include help to inform the user about the program's operation and parameters. Any errors will be intuitively described to the user. In no case shall the application terminate with a `SEGMENTATION FAULT` or other violent system termination (e.g., division by zero).

An essential part of this project is demonstrating your software engineering skills. Please, use **private** [Git](https://git-scm.com/) repository on the faculty [Gitea portal](https://git.fit.vutbr.cz/) when versioning your project, to which you will give access to all [IPK instructors](https://www.fit.vut.cz/study/course/231006/.en). Your repository should have multiple commits (preferably using [semantic commit messages](https://www.conventionalcommits.org/en/v1.0.0/)) illustrating your development history.

Task deadline is Tuesday 21st March 2023 at 23:59:59.

## Submission ##

The project should build a single executable called `ipkcpc` in the project root using the default `make` target. This executable need not be self-contained, and can be implemented as a wrapper/launcher, but it should conform to the CLI interface described above.

The project archive (and repo as well) needs to contain at least the following mandatory files:
- all project source codes and stand-alone libraries;
- working *Makefile*, which will compile or build the executable binary (with the filename specified in the assignment) from the source codes;
- documentation written in Markdown in the file README.md;
- the license in the file *LICENSE* with the most restrictive (i.e., preserving maximum rights of you as the author) commonly recognized open-source license you can find;
- *CHANGELOG.md* will contain a brief description of implemented functionality and known limitations (a documented problem is better than an undocumented one).

Please, verify following steps for successful submission of your projects:
1. check whether your code successfully compiles/builds under the reference developer environment and check filename syntax of mandatory files;
2. upload ZIP (and only ZIP) archive with filename `xlogin99.zip` containing the snapshot of your Gitea repo with all mandatory files (see above);
3. give access to your private Gitea repo to logins #!SEE ORIGINAL REPO!#.

We will ignore any submission via email (or any other communication channel) after the deadline.

## Documentation Instructions ##

Good documentation should include the following:
- content structuring;
- executive summary of the theory necessary to understand the functionality of the implemented application;
- UML diagrams or at least a narrative about interesting source code sections;
- most importantly testing part, which should prove that you have done some validation and verification of implemented functionality;
- description of extra functionality or features beyond the standard outlined by the assignment;
- bibliography with the list of all used sources.

Documentation or code comments can be in the following languages: Czech, Slovak, or English.

The section of the documentation describing testing should be sound and reproducible (e.g., accurate description of network topology and involved hardware specification and software versions). Testing should include at least a few runs of your application testing proper functionality and various edge cases. Please, compare your project with another similar tool (if such exists). Testing proofs can have the form of textual program outputs, screenshots or screencasts.

When working with any bibliography or sources on the Internet, please cite according to [faculty guidelines](https://www.fit.vut.cz/study/theses/citations/.en). It is worth to read also [this web page](http://www.fit.vutbr.cz/~martinek/latex/citace.html) by David Martinek.

When adopting or inserting snippets of someone else's source code into your project, you need to use them according to the author's license terms. Moreover, you need to mark this source code and reference the author explicitly in your source files.

## Developer Environment and Virtualization Remarks ##

The individual assignment specifies a custom reference environment, located in ./Devenv. To not waste resources with language toolchains you don't need, the reference environment is separate from the provided virtual machine, and activated on-demand for a particular language.

To use the developer environment, you can use the virtual machines located at [FIT Nextcloud](https://nextcloud.fit.vutbr.cz/s/YTxbDiCxFjHL29o) (hosted by e.g., [VMWare Player](https://www.vmware.com/products/workstation-player.html) or [VirtualBox](https://www.virtualbox.org/) or [Hyper-V](https://learn.microsoft.com/en-us/virtualization/hyper-v-on-windows/about/)). The user login is #!SEE ORIGINAL REPO!# with password #!SEE ORIGINAL REPO!#. Alternatively, you can install [Nix](https://nixos.org/download.html#nix-install-linux) on your own machine.

There are three developer environments provided:
- `c`
- `clisp`
- `csharp`

To activate the environment, clone [this repository](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty) and run `nix develop ./Devenv#csharp` from the repository root (replace `csharp` with the environment of choice). You can edit the environment definitions (to, e.g., add JetBrains Rider, add `jetbrains.rider` to the packages list; to add a C library for testing, add it to the `buildInputs` list; search for packages [here](https://search.nixos.org/packages?channel=22.11)). However, your *Makefile* should work with the default, unedited environment.

Due to the nature of the projects, you can assume that:
- the project will be run and tested with root privileges,
- reference machine will be connected to the Internet.

## Evaluation ##

You can get up to 13 points for working applications in compliance with the assignment. You can get up to 7 points for documentation and demonstrating good programmer skills on the project.

You can earn extra points for extending features of your program. You cannot get more than 20 points from the project, but the points for extra effort can help you minimize any losses during the assignment evaluation of standard requirements.

Common criteria for the evaluation:
- unstructured, uncommented or spaghetti source code = up to -7 points
- non-functioning or missing Makefile = up to -4 points
- missing or poor documentation = up to -7 points
- wrong bibliography = up to -2 points
- missing testing of the project = up to -3 points
- wrong binary filename = up to -2 points
- temporary or irrelevant files inside the submitted archive = up to -2 points
- the project cannot be compiled, executed or tested = 0 points
- the project is not in compliance with the assignment = 0 points
- plagiarism = 0 points for all involved parties

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
