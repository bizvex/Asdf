# Channel类
1. Channel类似于对fd的轻微包裹，程序中大部分对fd的处理都要经过Channel。但是Channel不是fd的唯一拥有者，不会在析构时关闭这个fd。Channel自始至终都属于一个EventLoop，主要负责对fd的IO事件分发。
2. Channel中有成员变量events_和revents_，前者是想要监听的事件，由用户设置；后者是目前活动的事件，由Epoll类返回，为events_的子集。
3. Channel提供各类函数回调，当事件发生时调用相应的回调函数。
4. Channel在构造时获得fd，并在随后需要的地方调用setEvents()函数设置events_。Epoll类通过Channel获得想要监听的fd和想要监听的事件，并在Epoll::poll()中设置并返回实际发生的事件revents_。
5. Channel的核心是handleEvents()，它由EventLoop::loop()调用，它根据revents_的值调用不同的回调函数。
