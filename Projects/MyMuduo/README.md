# 重构Muduo网络库

使用C++11的相关语法，让Muduo网络库脱离boost库的依赖。

## 安装方式

`sudo ./autobuild.sh`运行安装脚本

## 相关类成员(从底层到顶层)

1. EventLoop相关
   1. Channel
      1. 成员：fd + event/revent + callbacks
   2. Poller/EPollPoller
      1. Poller成员：Map<fd, Channel*>
      2. EPollPoller成员：Vector`<revent>`
   3. EventLoop
      1. 成员：Vector`<Channel>` + Poller + Vector`<reventChannel>` + Vector`<callback>` + wakeup fd/Channel
2. 线程相关
   1. EventLoopThread：让EventLoop与一个Thread绑定
   2. EventLoopThreadPool
      1. Vector`<Thread>` + Vector`<EventLoop*>`
3. 调用类
   1. Acceptor
      1. Socket + Channel  只设置关注读事件，设置写方法，写方法里的newConnection上层给
   2. TcpConnection
      1. Socket + Channel  各种事件，各种方法
      2. 2个Buffer + 连接的建立和销毁
4. TcpServer
   1. Map<string,TcpConnection>
5. 工具类
   1. Socket
   2. Thread
   3. CurrentThread
   4. Logger
   5. Buffer
   6. InetAddress
   7. Timestamp

## 调用流程(从顶层到底层)

### 1.初始化

1. TcpServer构造 + 传入Loop
   1. Acceptor：注册新连接方法
      1. Sock
      2. Channel
         1. 设置sockfd参数
         2. 给Channel注册读关注/方法
   2. ThreadPool
      1. 设置subLoop数量
      2. (如果有)注册线程初始化方法
2. TcpServer.start()
   1. ThreadPool调用初始化方法
      1. 初始化EventLoopThread
      2. Loop.loop()
   2. Acceptor开启监听

### 2.新连接到来

1. Acceptor监听到新连接后，Channel通知给Loop，Loop调用Channel的读方法，Tcpserver->Acceptor->Channel，方法中，用连接到的fd创建TcpConnection，并注册各类事件的方法，轮询出一个subLoop，让该loop去执行连接的创建
2. 连接的创建里，首先用tie绑定对应的管道，再调用TcpServer注册给连接的连接方法
