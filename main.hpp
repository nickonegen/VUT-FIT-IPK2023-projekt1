// Main logic of IPKCP Client

#ifndef IPK1_HPP
#define IPK1_HPP

#include <array>
#include <atomic>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "client.hpp"

std::atomic<bool> quit(false);

void signal_handler(int signal) {
	(void)signal;
	quit.store(true);
}

#endif
