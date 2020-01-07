#ifndef __RIKERIO_MONITOR_H__
#define __RIKERIO_MONITOR_H__

#include "common/ring-buffer.h"
#include "memory"

namespace RikerIO {

class Monitor {

  public:

    class RequestInterface {
      public:
        virtual unsigned int get_size() const;
        virtual uint8_t* get_data() const;
        virtual bool is_pending() const;
    };

  private:

    class Request {
      public:
        Request(unsigned int offset, unsigned int size, unsigned int semaphore);

        unsigned int get_offset() const;
        unsigned int get_size() const;
        int get_semaphore() const;

        uint8_t* get_data() const;

        bool is_pending() const;
        bool is_ready() const;

        void set_ready() {
            state = true;
        }

      private:

        const unsigned int offset;
        const unsigned int size;
        const int semaphore;

        uint8_t* data;

        std::atomic<bool> state;
    };

  public:

    std::shared_ptr<RequestInterface> create_request(unsigned int offset, unsigned int size);

  private:

    RingBuffer<std::shared_ptr<Request>> requests;

};

}

#endif
