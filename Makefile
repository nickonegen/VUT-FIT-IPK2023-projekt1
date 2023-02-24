###############################################################################
#
# IPKCP Makefile
# @author Onegen Something <xonege99@vutbr.cz>
#
# Usage:
#   - `make` or `make all` or `make release` to build the project
#   - `make debug` to build the project with debug flags
#   - `make clean` to remove built binaries
#   - `make format` to format the source code
#   - `make lint` to lint the source code
#   - `make zip` to create a zip archive of the project
#   - `make help` to show Makefile usage
#
###############################################################################

TARGET                 = ipkcpc
ZIPNAME                = xonege99.zip

CPP                    = g++
CPPFLAGS               = -std=c++20
EXTRA_CPPFLAGS         = -Wall -Wextra -Werror -pedantic \
                    -fdata-sections -ffunction-sections
RELEASE_CPPFLAGS       = -DNDEBUG -O2 -march=native
DEBUG_CPPFLAGS         = -g -Og -fsanitize=undefined
LINT_FLAGS             = --format-style=file \
-checks="bugprone-*,google-*,performance-*,readability-*,hicpp-*,clang-analyzer-*"
RM                     = rm -f

SRCS                   = main.cpp client.cpp

###############################################################################

.PHONY: all release debug help clean zip lint format

all: release

release: EXTRA_CPPFLAGS += ${RELEASE_CPPFLAGS}
release: ${TARGET}

debug: EXTRA_CPPFLAGS += ${DEBUG_CPPFLAGS}
debug: ${TARGET}

${TARGET}: ${SRCS}
	${CPP} ${CPPFLAGS} ${EXTRA_CPPFLAGS} ${SRCS} -o ${TARGET}
	@echo "ipkcpc compiled!"
	@echo "Run with: ./ipkcpc -h <host> -p <port> -m <mode>"

help:
	@echo "IPKCP-Client Makefile"
	@echo "@author Onegen Something <xonege99@vutbr.cz>"
	@echo ""
	@echo "Usage: make [TARGET]"
	@echo "TARGETs:"
	@echo "  all     compile and link the project (default)"
	@echo "  debug   compile and link the project with debug flags"
	@echo "  clean   clean objects and executables"
	@echo "  format  run formatter"
	@echo "  lint    run linter"
	@echo "  zip     create a .zip archive with the source files"
	@echo "  help    print this message"

clean:
	${RM} ${TARGET}

zip:
	zip -q -r ${ZIPNAME}.zip *.cpp *.hpp Makefile

format:
	clang-format -i *.cpp *.hpp

lint:
	clang-tidy *.cpp *.hpp ${LINT_FLAGS} -- ${CPPFLAGS}
