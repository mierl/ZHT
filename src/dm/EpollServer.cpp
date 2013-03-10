/*
 * Copyright (C) 2010-2020 DatasysLab@iit.edu(http://datasys.cs.iit.edu/index.html).
 *      Director: Ioan Raicu(iraicu@cs.iit.edu)
 *	 
 * This file is part of ZHT library(http://datasys.cs.iit.edu/projects/ZHT/index.html).
 *      Ioan Raicu(iraicu@cs.iit.edu),
 *      Tonglin Li(tli13@hawk.iit.edu) with nickname tony,
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname xiaobingo.
 * 
 * The ZHT Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The ZHT Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the ZHT Library; if not, write to the 
 * Data-Intensive Distributed Systems Laboratory, 10 W. 31st Street,
 * Stuart Building, Room 003B, Chicago, IL 60616 USA.
 *
 * EpollServer.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: tony, xiaobingo
 */

#include "EpollServer.h"

#include "Const-impl.h"

#include "../net_util.h"
#include "../zht_util.h"

#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/epoll.h>

#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

EpollData::EpollData(const int& fd, const sockaddr * const sender) :
		_fd(fd), _sender(sender) {

}

EpollData::~EpollData() {

	if (_sender != NULL)
		delete _sender;
}

const bool EpollServer::TCP = true;
const int EpollServer::MAX_EVENTS = 1024;
const uint EpollServer::MAX_MSG_SIZE = Env::MAX_MSG_SIZE; //transferd string maximum size

EpollServer::EpollServer(ZProcessor *processor) :
		_ZProcessor(processor) {

}

EpollServer::EpollServer() {

}

EpollServer::~EpollServer() {

}

int EpollData::fd() const {
	return _fd;
}

const sockaddr * const EpollData::sender() const {
	return _sender;
}

int EpollServer::create_and_bind(const char *port) {
	return create_and_bind(NULL, port);
}

int EpollServer::create_and_bind(const char *host, const char *port) {

	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, sfd;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; /* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
	hints.ai_flags = AI_PASSIVE; /* All interfaces */

	s = getaddrinfo(host, port, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {

		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0) {
			/* We managed to bind successfully! */
			break;
		}

		close(sfd);
	}

	if (rp == NULL) {
		fprintf(stderr, "Could not bind\n");
		return -1;
	}

	freeaddrinfo(result);

	return sfd;
}

int EpollServer::make_socket_non_blocking(const int& sfd) {

	int flags, s;

	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl(sfd, F_SETFL, flags);
	if (s == -1) {
		perror("fcntl");
		return -1;
	}

	return 0;
}

int EpollServer::serve(const char *port) {

	int sfd, s;
	int efd;
	struct epoll_event event;
	struct epoll_event *events;

	sfd = makeSvrSocket(atoi(port), true);
	if (sfd == -1)
		abort();

	s = make_socket_non_blocking(sfd);
	if (s == -1)
		abort();

	s = listen(sfd, SOMAXCONN);
	if (s == -1) {
		perror("listen");
		abort();
	}

	reuseSock(sfd);

	efd = epoll_create(1);
	if (efd == -1) {
		perror("epoll_create");
		abort();
	}

	event.data.ptr = new EpollData(sfd, NULL);
	event.events = EPOLLIN | EPOLLET;
	s = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event);
	if (s == -1) {
		perror("epoll_ctl");
		abort();
	}

	/* Buffer where events are returned */
	events = (epoll_event *) calloc(MAX_EVENTS, sizeof event);

	/* The event loop */
	while (1) {

		int n, i;

		n = epoll_wait(efd, events, MAX_EVENTS, -1);

		for (i = 0; i < n; i++) {

			EpollData *edata = (EpollData*) events[i].data.ptr;

			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)
					|| (!(events[i].events & EPOLLIN))) {

				/* An error has occured on this fd, or the socket is not
				 ready for reading (why were we notified then?) */
				fprintf(stderr, "epoll error\n");
				close(edata->fd());
				delete edata;
				continue;
			} else if (sfd == edata->fd()) {

				if (TCP == true) {
					/* We have a notification on the listening socket, which
					 means one or more incoming connections. */
					while (1) {

						int infd;
						socklen_t in_len = sizeof(struct sockaddr);
						sockaddr *in_addr = (sockaddr *) calloc(1,
								sizeof(struct sockaddr));

						infd = accept(sfd, in_addr, &in_len);
						if (infd == -1) {

							free(in_addr);

							if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {

								/* We have processed all incoming
								 connections. */
								break;
							} else {
								perror("accept");
								break;
							}
						}

						/*				fprintf(stdout,
						 "sin_family[%hu], sin_zero[%s], sin_addr.s_addr[%u], sin_port[%hu]\n",
						 in_addr.sin_family, in_addr.sin_zero,
						 in_addr.sin_addr.s_addr, in_addr.sin_port);
						 */

						char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
						getnameinfo(in_addr, in_len, hbuf, sizeof hbuf, sbuf,
								sizeof sbuf, 0);

						if (s == 0) {
							printf("Accepted connection on descriptor %d "
									"(host=%s, port=%s)\n", infd, hbuf, sbuf);
						}

						/* Make the incoming socket non-blocking and add it to the
						 list of fds to monitor. */
						s = make_socket_non_blocking(infd);
						if (s == -1) {
							free(in_addr);
							abort();
						}

						reuseSock(infd);

						event.data.ptr = new EpollData(infd, in_addr);
						event.events = EPOLLIN | EPOLLET;
						s = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &event);
						if (s == -1) {
							free(in_addr);
							perror("epoll_ctl");
							abort();
						}
					}
					continue;
				} else {

					ssize_t count;
					char buf[MAX_MSG_SIZE];
					memset(buf, 0, sizeof(buf));

					sockaddr sender;
					socklen_t sender_len = sizeof(struct sockaddr);

					count = recvfrom(edata->fd(), buf, sizeof buf, 0, &sender,
							&sender_len);

					string str(buf);
					_ZProcessor->process(edata->fd(), buf, sender,
							Const::PROTO_UGRADM);

					memset(buf, 0, sizeof(buf));
				}

			} else {

				if (TCP == true) {
					/* We have data on the fd waiting to be read. Read and
					 display it. We must read whatever data is available
					 completely, as we are running in edge-triggered mode
					 and won't get a notification again for the same
					 data. */
					int done = 0;

					while (1) {

						ssize_t count;
						char buf[MAX_MSG_SIZE];
						memset(buf, 0, sizeof(buf));

						count = generalReveiveTCP(edata->fd(), buf, sizeof buf,
								0);

						if (count == -1) {

							/* If errno == EAGAIN, that means we have read all
							 data. So go back to the main loop. */
							if (errno != EAGAIN) {
								perror("read");
								done = 1;
							} /*else {

							 printf(
							 "Closed connection on descriptor %d, -1<--recv\n",
							 edata->fd());

							 close(edata->fd());
							 delete edata;
							 }*/
							break;
						} else if (count == 0) {

							/* End of file. The remote has closed the
							 connection. */
							done = 1;
							break;
						}

						_ZProcessor->process(edata->fd(), buf, *edata->sender(),
								Const::PROTO_STREAM);

						memset(buf, 0, sizeof(buf));

					}

					if (done) {

						printf("Closed connection on descriptor %d, done.\n",
								edata->fd());

						/* Closing the descriptor will make epoll remove it
						 from the set of descriptors which are monitored. */
						close(edata->fd());
						delete edata;
					}
				} //if TCP == true
			}
		}
	}

	free(events);

	close(sfd);

	EpollData *gedata = (EpollData*) event.data.ptr;
	delete gedata;

	return EXIT_SUCCESS;
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
