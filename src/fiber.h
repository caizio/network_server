#ifndef __CAIZI__FIBER_H__
#define __CAIZI__FIBER_H__

#include <memory>

namespace caizi{

class Fiber: public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State{
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };


};


}

#endif