# Server类
1. 线程类型  
多线程服务器中的线程可分为以下几类：
  - IO线程(负责网络IO)
  - 计算线程(负责复杂计算)
  - 第三方库所用线程  

 由于Asdf是一个静态web服务器，所以没有复杂的计算，因此除了日志线程属于第三类外，其它线程均属于IO线程。

2. Server类的实例所在线程可以看作一个主线程，它主要作用是：
  - 在程序开始时创建固定数量的线程，并在各个线程中开启loop循环

  - 持有listenFd_，并响应client的连接请求

  - 当有连接到达时，以round robin的方式，将请求分配给各个线程。由于线程里面只有一个EventLoop，所以实际上是以queueInLoop()跨线程调用的方式把处理请求的函数给EventLoop。处理请求的函数是将该Channel（acceptFd_与监听事件的集合体）添加到epoll监听。最后就可以在loop循环中根据epoll_wait的返回对fd及其对应事件进行处理（调用Channel::handleEvents()）  
3. 当主线程把新连接分配给了某个SubReactor，该线程此时可能正阻塞在多路选择器(epoll)的等待中，怎么得知新连接的到来呢？这里使用了eventfd进行异步唤醒，线程会从epoll_wait中醒来，得到活跃事件，进行处理。
