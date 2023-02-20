###############################################################################
#
# IPK1 Makefile
# @author Onegen Something <xonege99@vutbr.cz>
# @date 2023-02-20
#
# Usage:
#   * `make` to build the client (task)
#   * `make all` to build the client and the testing server
#   * `make clean` to remove built binaries
#   * `make zip` to create a .zip archive of the source code
#   * `make help` to show this makefile's usage
#
###############################################################################

CLIENT                 = ipkcpc
SERVER                 = ipkcps
ZIPNAME                = xonege99

CPP                    = g++
CPPFLAGS               = -Wall -Werror -pedantic
EXTRA_FLAGS            = -fdata-sections -ffunction-sections -O3
RM                     = rm -f

C_SRCS                := client.cpp
S_SRCS                := server.cpp

###############################################################################

.PHONY: help clean zip format

client: 
	${CPP} ${CPPFLAGS} ${EXTRA_FLAGS} ${C_SRCS} -o ${CLIENT}
	@echo "ipkcpc compiled!"
	@echo "Run client with: ipkcpc -h <host> -p <port> -m <mode>"

server: 
	${CPP} ${CPPFLAGS} ${EXTRA_FLAGS} ${S_SRCS} -o ${SERVER}
	@echo "ipkcps compiled!"
	@echo "Run server with: ipkcps -p <port>"

help:
	@echo "IPK-Project 1 (IPK Calculator Protocol client)"
	@echo "@author Onegen Something <xonege99@vutbr.cz>"
	@echo ""
	@echo "Usage: make [TARGET]"
	@echo "TARGETs:"
	@echo "  client  compile and link IPKCP client (default)"
	@echo "  server  compile and link IPKCP server"
	@echo "  all     compile and link IPKCP client and server"
	@echo "  clean   clean objects and executables"
	@echo "  zip     create a .zip archive with source files"
	@echo "  help    print this message"
	@echo "  format  run clang-format"


all:
	${CPP} ${CPPFLAGS} ${EXTRA_FLAGS} ${C_SRCS} -o ${CLIENT}
	${CPP} ${CPPFLAGS} ${EXTRA_FLAGS} ${S_SRCS} -o ${SERVER}
	@echo "ipkcpc & ipkcps compiled!"
	@echo "Run client with: ipkcpc -h <host> -p <port> -m <mode>"
	@echo "Run server with: ipkcps -p <port>"

clean:
	${RM} ipkcpc ipkcps
	@echo "Cleaned!"

zip:
	zip -q -r ${ZIPNAME}.zip *.cpp *.hpp Makefile
	@echo "Archive created!"

format:
	clang-format -i --sort-includes *.cpp *.hpp
	@echo "Files formatted!"
