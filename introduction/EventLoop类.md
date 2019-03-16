# EventLoop类
1. EventLoop类有几个重要的成员
  - isInLoopThread()  
  用来确保调用EventLoop成员函数的线程，与EventLoop的对象所在线程是同一个线程。
  - loop()  
  业务核心。主要干：(a)等待epoll_wait返回，并处理相应事件；(b)处理别的线程传入当前线程的事件。
  - unInLoop()和queueInLoop()  
  跨线程调用核心。如果EventLoop的对象所在线程与当前线程一致，则runInLoop()会直接执行传入的函数；若不一致，则该传入的函数会被加入任务队列（调用queueInLoop()）。
  - wakeupFd_和pwakeupChannel_  
  因为loop中会阻塞在epoll_wait，所以需要在queueInLoop()中将函数加入任务队列后使loop从epoll_wait中返回。方法就是将wakeupFd_加入到epoll的监听（监听可读事件），当需要唤醒时往wakeupFd_写入64位无符号整型。关于eventfd的使用可以见这篇[文章](https://www.jianshu.com/p/4f87c4792580)。
2. EventLoopThread类  
EventLoopThread的作用是开启一个线程并使得loop跑起来（此时loop会阻塞在epoll_wait直到监听fd事件发生）。
3. EventLoopThreadPool类  
EventLoopThreadPool的作用是构造多个EventLoopThread，并调用EventLoopThread::startLoop()。即是开启多个线程，并让多个loop在相应的线程上跑起来，然后等待Server分配的acceptfd。
