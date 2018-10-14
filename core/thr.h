#pragma once

namespace wjp{

/*
    std::thread有几个缺陷：
        1. 默认id不是数值类型，没有办法用诸如-1的值代表不存在
        2. init即执行，没办法先定义，以后再run——除非用指针先指向nullptr，std::function设定好，far from graceful
        3. 虽然跨平台，但不可拓展，需要用tbb作底层，那就需要新的封装了。

    因此重造了轮子，tbb的封装在未来计划中，目前先用pthread迅速实现。
    
    thread和convar的复杂度低，没必要专门在platform层封装，因此直接在core层调用底层代码（pthread或tbb），仍然是现代linux优先。

    另外，命名上虽然有namespace保护，却还是避讳一下，免得误解。
*/

class convar{

};

class thr{
public:
    bool        is_current_thread()
    {

    }

private:
};


}