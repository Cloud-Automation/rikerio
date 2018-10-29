#include "eventloop.h"
#include <sys/epoll.h>

EventLoop::EventLoop() {

    // epoll init
    fd = epoll_create1( 0 );
    if ( fd < 0 ) {
        throw runtime_error("epoll_create1");
    }

}
