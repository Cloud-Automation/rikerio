/*
 * Copyright (c) 2017 Stefan Pöter.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file eventloop.h
 * @author Stefan Poeter
 * @date 2017-07-03
 * @brief This file contains the header information for the internal eventloop.
 *
 * The eventloop module uses the epoll function to handle different types of linux related
 * signals. It is basically a epoll wrapper with a slightly simpler API.
 *
 */

#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <memory>
#include <sys/epoll.h>

/**
 * @brief Size of the event list.
 */
#define EVENTLOOP_MAX_EVENTS 100

using namespace std;

class EventLoop {

  public:
    EventLoop();
    ~EventLoop();

    void registerEvent(shared_ptr<Event>);
    void unregisterEvent(shared_ptr<Event>);

    void run();
    void stop();

  private:

    int fd;
    multimap<int, shared_ptr<Event>> eventMap;


};

#endif
