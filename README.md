# 多线程静态Web服务器
## 简介
用C++11编写的多线程Web服务器，并实现异步日志。主体架构参考muduo。
## 服务器工作流程
1. 开启一个主线程作为mainReactor。Server类将listenfd加入到mainReactor的监听。Reactor模式的介绍可以看这篇[文章](https://www.jianshu.com/p/698c984327a8)。
2. 开启线程池，线程池中的线程作为subReactor。subReactor启动后会阻塞在epoll_wait返回。
3. 当有客户到来时，listenfd就绪，导致mainReactor从epoll_wait返回。然后accept得到acceptfd，并将这个acceptfd以round robin的方式分配给步骤2中的subReactor。具体做法是，使用跨线程调用函数queueInLoop()把能将acceptfd加入到epoll监听的函数送入到目标线程的任务队列。
4. 一个subReactor会同时监听若干个不同的acceptfd。一旦某个acceptfd就绪，会使得epoll_wait返回。返回后首先执行注册好的函数回调，接着是任务队列中的函数，最后是处理过期的acceptfd。
## 重要的类
- [EventLoop类](https://github.com/bizvex/Asdf/blob/master/introduction/EventLoop%E7%B1%BB.md)
- [Server类](https://github.com/bizvex/Asdf/blob/master/introduction/Server%E7%B1%BB.md)
- [Channel类](https://github.com/bizvex/Asdf/blob/master/introduction/Channel%E7%B1%BB.md)
- [异步日志](https://github.com/bizvex/Asdf/blob/master/introduction/%E5%BC%82%E6%AD%A5%E6%97%A5%E5%BF%97.md)
- [定时器](https://github.com/bizvex/Asdf/blob/master/introduction/%E5%AE%9A%E6%97%B6%E5%99%A8.md)
- [关闭连接](https://github.com/bizvex/Asdf/blob/master/introduction/%E5%85%B3%E9%97%AD%E8%BF%9E%E6%8E%A5.md)
## 环境
OS: ubuntu16.04  
Complier: g++ 4.8
## 测试结果  
- 主线程1个，线程池线程4个  
- QPS  
  短连接: 4万4    
  长连接: 10万  
- webbench结果  
  ![短连接](https://github.com/bizvex/Asdf/blob/master/introduction/%E7%9F%AD%E8%BF%9E%E6%8E%A5.png "短连接")  
  ![长连接](https://github.com/bizvex/Asdf/blob/master/introduction/%E9%95%BF%E8%BF%9E%E6%8E%A5.png "长连接")  
- 负载  
  长连接与短连接负载的区别在于主线程。短连接的主线程需要不断地accept，所以负载较高；长连接的主线程则不用，所以负载较低。
  ![空闲负载](https://github.com/bizvex/Asdf/blob/master/introduction/%E7%A9%BA%E9%97%B2%E8%B4%9F%E8%BD%BD.png "空闲负载")
  ![短连接负载](https://github.com/bizvex/Asdf/blob/master/introduction/%E7%9F%AD%E8%BF%9E%E6%8E%A5%E8%B4%9F%E8%BD%BD.png "短连接负载")
  ![长连接负载](https://github.com/bizvex/Asdf/blob/master/introduction/%E9%95%BF%E8%BF%9E%E6%8E%A5%E8%B4%9F%E8%BD%BD.png "长连接负载")
