# NetworkProgramming #
Linux网络编程视频教程


视频地址 https://www.bilibili.com/video/av33813135/


ISO/OSI参考模型(理想化)

+ 应用层             apdu   （HTTP协议，FTP协议）
+ 表示层             ppdu（加密，解密，压缩）
+ 会话层             spdu
+ 传输层             sement
+ 网络层             packet(包)     路由选择
+ 数据链路层     frame(帧)      交换机
+ 物理层             byte流

## TCP/IP四层模型(事实上)

+ 应用层：用户进程：常用协议（FTP，telent，QQ，自定义）
+ 传输层（TCP，UDP）
+ 网络层（ICMP，IP，IGMP）
+ 链路层（ARP，RARP）

## ICMP协议

ICMP协议用于传递差错信息，时间，回显，网络信息等控制数据，ping程序用到ICMP协议

## 数据在网络中的传输过程

1. 应用程序ping会判断发送的是主机名还是IP地址，调用函数gethostbyname()解析主机B，将主机名转换成一个32位的IP地址。这个过程叫做DNS域名解析
2. ping程序向目的IP地址发送一个ICMP的ECHO包
3. 将目标主机的IP地址转换成48位的硬件地址，在局域网内发送ARP请求广播，查找主机B的硬件地址
4. 主机B的ARP协议层会收到主机A的ARP请求后，将本机的硬件地址填充到应答包，发送ARP应答到主机A
5. 发送ICMP数据包到主机B
6. 主机B接收到主机A的ICMP包，发送相应包
7. 主机A收到主机B的ICMP包相应包

## TCP特点

+ 基于字节流

  segment段之间不做任何假定，无格式，发送和接受可能要多次，会出现粘包问题（多个数据包被连续存储在连续的缓存中，无法确定边界）

+ 面向连接

  先建立连接才能传输数据

+ 可靠传输

  保证传输可靠，例如通过端到端的校验和保证不会出错

+ 缓冲传输

  可能先将数据缓冲到一个缓冲区，然后选择一个最佳的时机来传输数据

+ 全双工

  发送时同时可以接受数据

+ 流量控制

  通过窗口机制

  ![image-20220630235610916](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220630235610916.png)

  
  
  ![image-20220629132745182](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220629132745182.png)
  

## TCP建立连接三次握手机制

![image-20220629203423533](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220629203423533.png)

全双工的通信，A端点发送一个数据到达B端点，但是A不知道发送的数据能否到达B，所以B需要给A一个确认，同样，从B到A的数据B并不知道能否到达，所以A需要给B一个确认，确认两次之后，A和B都能确认连接，因此可以建立连接

**细节：**

+ 首先A发送一个TCP段给B，序号为a，SYN为1(表示发起连接)
+ B收到之后，向A表示确认，发送一个TCP段给B，SYN为1，序号为b，且确认值ACK为1，确认号为a+1
+ 当A收到确认之后，发送一个TCP段给B，ACK为1，确认号为b + 1

## 连接终止的四次挥手

既可以由A发起 ，又可以由B发起

![image-20220629204927558](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220629204927558.png)

+ A发送一个TCP段给B，FIN，序号为x，ACK，确认号为y
+ B收到之后发送一个TCP段给A，ACK，确认号为x+1，关闭了A到B的连接
+ B发送一个TCP段给A，FIN，序号为y，ACK，确认号为x+1
+ A收到之后发送了一个TCP段给B，ACK，确认号为y + 1

## TCP如何保证可靠性

不可靠原因：

+ 差错   -->维护一个端到端的校验和
+ 丢包   -->超时重传 + 确认机制：在规定时间没有收到确认会启动重传机制
+ 失序   -->TCP头部有一个序号
+ 重复   -->TCP头部有一个序号

如何保持可靠：

+ 应用程序被分为TCP段传递给IP层
+ 当TCP发送一个段后，它启动一个定时器，等待接收端的确认报文段，如果不能及时收到确认，将会重发这个报文段
+ 当TCP收到发自TCP连接的另一端的数据，它将发送一个确认，这个确认不是立即发送，通常推迟一段时间
+ TCP保持首部和数据的校验和，这是一个端到端的校验和，用来检测数据在传输过程中的变化，如果收到的校验和有差错，将丢弃这个段并且不确认（导致对方超时重传）
+ TCP承载于IP数据报用于传输，而IP数据报可能失序，因此TCP设置了序号用来保证排序
+ IP数据报会发生重复，TCP接收端必须丢弃重复数据
+ TCP还提供流量控制，TCP连接的每一方都有一定大小的缓冲空间

## 滑动窗口协议（用于流量控制）

**引言：**

想象一下这个场景：主机 A 一直向主机 B 发送数据，不考虑主机 B 的接收能力，则可能导致主机 B 的接收缓冲区满了而无法再接收数据，从而导致大量的数据丢包，引发重传机制。而在重传的过程中，若主机 B 的接收缓冲区情况仍未好转，则会将大量的时间浪费在重传数据上，降低传送数据的效率。

所以引入了流量控制机制，主机 B 通过告诉主机 A 自己接收缓冲区的大小，来使主机 A 控制发送的数据量。**总结来说：所谓流量控制就是控制发送方发送速率，保证接收方来得及接收**。

**TCP 实现流量控制主要就是通过 滑动窗口协议**。

对于发送方来说，窗口大小就是指无需等待确认应答，可以连续发送数据的最大值。



窗口大小具体由谁来设定呢？

窗口大小和 TCP 报文首部中 16 位的 **窗口大小 Window** 字段有关：

![TCP 报文段首部](https://cs-wiki.oss-cn-shanghai.aliyuncs.com/img/20210110153454.png)

该字段的含义是**指自己接收缓冲区的剩余大小**，于是发送端就可以根据这个接收端的处理能力来发送数据，而不会导致接收端处理不过来。

所以，通常来说**窗口大小是由接收方来决定的**。

**滑动窗口详解**

站在发送方的角度，滑动窗口可以分为四个部分：

1. 已发送且已确认，这部分已经发送完毕，可以忽略； 
2. 已发送但未确认，这部分可能在网络中丢失，数据必须保留以便必要时重传； 
3. 未发送但可发送，这部分接收方缓冲区还有空间保存，可以发出去； 
4. 未发送且暂不可发送，这部分已超出接收方缓冲区存储空间，就算发出去也没意义； 

> 第 2 和第 3 部分加起来就刚好就是接收方窗口大小，它规定了当前发送方能发送的[最大数]()据量。

发送方在收到确认应答报文之前，必须在窗口中保留已发送的报文段（因为报文段可能在网络中丢失，所以必须把这些未确认的报文段保留这，以便必要时重传）；如果在规定时间间隔内收到接收方发来的确认应答报文，就可以将这些报文段从窗口中清除。

当发送方收到接收方发来的确认应答后，就将窗口中那些被确认的报文清除出去，然后窗口向右移动，如下图所示：

![img](https://cs-wiki.oss-cn-shanghai.aliyuncs.com/img/20220410232650.png)

随着双方通信的进行，窗口将不断向右移动，因此被形象地称为滑动窗口（Sliding Window）



对于 TCP 的接收方，窗口稍微简单点，分为三个部分：

1. 已接收 
2. 未接收准备接收 (也即接收窗口，再强调一遍，接收窗口的大小决定发送窗口的大小) 
3. 未接收并未准备接收 

> 由于 ACK 直接由 TCP 协议栈回复，默认无应用延迟，不存在 “已接收未回复 ACK”

![img](https://cs-wiki.oss-cn-shanghai.aliyuncs.com/img/20220410232840.png)



综上，举个例子，假设发送方需要发送的数据总长度为 400 字节，分成 4 个报文段，每个报文段长度是 100 字节:

1）三次握手连接建立时接收方告诉发送方，我的接收窗口大小(`rwnd`) 是 300 字节

此时的接收方滑动窗口如下：

![接收方滑动窗口](https://cs-wiki.oss-cn-shanghai.aliyuncs.com/img/20220410231731.png)

此时的发送方滑动窗口如下：

![发送方滑动窗口](https://cs-wiki.oss-cn-shanghai.aliyuncs.com/img/20220410233037.png)

2）发送方发送第一个报文段（序号 1 - 100），还能再发送 200 个字节

3）发送方发送第二个报文段（序号 101 - 200），还能再发送 100 个字节

4）发送方发送第三个报文段（序号 201 - 300），还能再发送 0 个字节

> 此时，发送方的窗口中存了三个报文段了

此时的发送方滑动窗口如下：

![发送方滑动窗口](https://cs-wiki.oss-cn-shanghai.aliyuncs.com/img/20220410231058.png)

5）接收方接收到了第一个报文段和第三个报文段，中间第二个报文段丢失。此时接收方返回一个报文段 ack = 101, rwnd = 200（假设这里发生流量控制，把窗口大小降到了 200，允许发送方继续发送起始序号为 101，长度为 200 的报文）

此时的接收方滑动窗口如下（本来窗口右端应该右移，但是这里发生了流量控制，接收方希望缩小窗口大小，所以正好，这里就不需要向右扩展了）：

![接收方滑动窗口](https://cs-wiki.oss-cn-shanghai.aliyuncs.com/img/20220410231803.png)

> 发送方收到了第一个报文段的确认，从窗口中移除掉第一个报文段

此时的发送方滑动窗口如下：

![发送方滑动窗口](https://cs-wiki.oss-cn-shanghai.aliyuncs.com/img/20220410231206.png)

6）发送方一直没有收到第二个报文段的确认应答，在等待超时后重传第二个报文段（序号 101 - 200）

7）接收方成功收到第二个报文段（窗口中有第二个和第三个报文段了），于是向发送方返回一个报文段 ack = 301, rwnd = 100（假设这里发生流量控制，把窗口大小降到了 100）

此时的接收方滑动窗口如下：（本来窗口右端应该右移，但是这里发生了流量控制，接收方希望缩小窗口大小，所以正好，这里就不需要向右扩展了）

![接收方滑动窗口](https://cs-wiki.oss-cn-shanghai.aliyuncs.com/img/20220410231858.png)

> 发送方收到了第二个和第三个报文段的确认，从窗口中移除掉这俩报文段

8）发送方发送第四个报文段（序号 301 - 400）

此时的发送方滑动窗口如下：

![发送方滑动窗口](https://cs-wiki.oss-cn-shanghai.aliyuncs.com/img/20220410231224.png)

**⭐ 窗口的本质**

说了半天，窗口好像只是一个虚无缥缈的概念，

实际上，由于 TCP 是内核维护的，所以窗口中的报文数据其实就是存放在**内核缓冲区**中

> 注意这里区分下内核缓冲区（buffer）和高速缓存的概念

内核缓冲区大小一般是不会发生改变的，缓冲区大小 > 窗口大小，且窗口大小根据缓冲区中空闲空间的大小在不断发生改变。

对于接收方来说：

- 接收方根据缓冲区空闲的空间大小，计算出后续能够接收多少字节的报文（即接收窗口的大小） 
- 当内核接收到报文时，将其存放在缓冲区中，这样缓冲区中空闲的空间就变小了，接收窗口也就随之变小了 
- 当进程调用 `read` 函数后（将数据从内核缓冲区复制到用户/进程缓冲区），报文数据被读入了用户空间，内核缓冲区就被清空，这意味着主机可以接收更多的报文，接收窗口就会变大 

对于发送方来说，进程在发送报文之前会调用 `write` 函数（将数据从用户/进程缓冲区写到内核缓冲区），这样，缓冲区中可用空间变小，窗口变小，可发送的数据就变少了，等收到这些发送出去的数据的确认应答后，再从缓冲区中清除掉，从而使得窗口变大。

**通俗的例子**

下面来更通俗地解释下滑动窗口，看下面这个场景，老师（发送方）说一段话，学生（接收方）来记

> 最原始的模式，发送方一股脑把所有的报文段全都发出去。

老师说 "危楼高百尺，手可摘星辰，不敢高声语，恐惊天上人"（咱把每个字看成一个报文段，总共 20 个报文段）

学生写道"危楼高百尺，手可......."

> 上面的模式过于简单粗暴，发送方发送速度太快，接收方跟不上，并且重传成本过高。
>
> 于是他们换了一种模式：每发送一个报文段就等待确认一个报文段，收到确认后才能发送下一个

老师说 "危"，学生说"确认"

老师说 "楼"，学生说"确认"

老师说 "高"，学生说"确认"

.........

> 上面的模式每发一个报文段，必须等到确认后才能再次发送，效率低下。
>
> 于是他们又换了一种模式：**累积确认**，既不是一股脑把所有的报文段全都发出去，也不是一次只发一个报文段，而是分组发送，每次发几个报文段。

老师说 "危楼高百尺" （5 个报文段），学生说 "确认"

老师说 "手可摘星辰"，学生说 "手可..."（3 个报文段丢失）

老师说 "不敢高声语"，学生说 "确认"

老师一直没有收到 "摘星辰" 的确认，于是重新说了一遍 "摘星辰"，学生说 "确认"

老师说 "恐惊天上人"，学生说 "确认"

> 上面的模式提高了效率，连续多个报文段一起进行发送, 但是到底该怎么决定多少个报文段一起发送呢呢？
>
> 于是他们在上面模式的基础上，做出了一些改进：**滑动窗口**，接收方认为状态好（窗口比较大）的时候, 让发送方每次多发一点；接收方认为状态不好（窗口比较小）的时候，让发送方每次少发送一点，起到一个流量控制的作用，限制发送方的速度。

学生告诉老师，我一次性可以接收 10 个报文段

老师说 "危楼高百尺，手可摘星辰"，学生说 "危楼高百尺，手可..."（3 个报文段丢失，返回 ”可" 的确认应答，一共确认了 7 个报文段，老师的可用窗口右移，窗口中现在还有 “摘星辰” 3 个报文段）

学生说，我状态不行，一次性现在只能接收 5 个报文段（流量控制，缩小窗口）

老师说 "不敢"（窗口中还有 “摘星辰” 3 个报文段，所以只能发送 2 个），学生说 "确认"

老师一直没有收到 "摘星辰" 的确认，于是重新说了一遍，学生说 "确认"

（可用窗口恢复为 5 个）老师说 "恐惊天上人"，......

## 什么是socket

+ socket可以看成是用户进程与内核网络协议栈的编程接口
+ socket不仅可以用于本机的进程通信，还可以用于网络上不同主机的进程通信，可以看成是两个进程之间数据传递的抽象

## listen函数

+ 功能：将套接字用于监听进入的连接

+ 原型：`int listen(int sockfd,int backlog)`

+ 参数：
  + sockfd：socket函数返回的套接字
  + backlog：规定内核为此套接字排队的最大连接个数

+ 返回值：成功返回0，失败返回-1

+ ```
  * 调用listen函数将会使得主动套接字变为被动套接字
  * 主动套接字用于发起连接 调用connect()函数
  * 被动套接字用于接受连接 调用accept()函数
  ```

+ listen函数应该在调用socket和bind()函数之后，调用accept函数之前
+ 对于给定的监听套接口，内核要维护两个队列：
  + 已经由用户发出并到达服务器，服务器正在等待完成相应的TCP三次握手过程
  + 已经完成连接的队列

## accept函数

+ 功能：从已完成连接队列返回第一个连接，如果已完成连接队列为空，则阻塞
+ 原型：`int accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen)`
+ 参数：
  + sockfd：服务器套接字
  + addr：将返回对等方的套接字地址
  + addrlen：返回对等方的套接字地址长度

+ 返回值：成功返回非负整数，失败返回-1 

## 解决粘包问题

TCP是基于字节流的传输。在传输层未解决粘包问题，需要放到应用层解决

+ 本质上就是要在应用层维护消息与消息的边界
  + 定长包
  + 包尾加`\r\n(ftp)`
  + 包头加上包体长度
  + 更复杂的应用层协议

## TCP的状态

![image-20220704170959707](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220704170959707.png)

## SIGPIPE信号  

+ 往一个已经接受FIN的套接中写是允许的, 会收到RST段，接收到FIN仅仅代表对方不再发送数据
+ 在收到RST段之后，如果再调用write就会产生SIGPIPE信号，默认终止当前进程，可以是以哦那个信号处理函数处理该信号

## 五种IO模型

+ **阻塞I/O**

  ![image-20220707130411182](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220707130411182.png)

  一旦套接口完成连接之后就可以接收数据，向系统提交一个recv请求接收数据，这个请求是阻塞的，直到对等方发送数据，填充了recv套接口对应的接收缓冲区，这时候才解除了阻塞，并且将接收缓冲区中的数据从内核空间复制到用户缓冲区buf当中，一旦拷贝完成，recv函数就返回了相应数据，就可以处理数据了。

+ **非阻塞I/O**

  ![image-20220707143752450](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220707143752450.png)

  `fcntl(fd,F_SETFL,flag|0_NUNBLOCK)`设置套接口为非阻塞模式，recv函数即使没有数据到来，也不会阻塞，会返回一个错误，返回值为-1，错误代码为`EWOULDBLOCK`, 此时需要再次提交请求，直到有数据到来，然后把数据从内核缓冲区复制到用户缓冲区，然后进一步处理数据。

  使用较少，因为在数据没有到来之前需要循环请求，处于忙等待状态，造成及CPU资源浪费

+ **IO复用（select和poll）**

  ![image-20220707151911366](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220707151911366.png)

  主要是通过`select`来管理多个文件描述符，一旦其中的一个文件描述符有数据到来，select就返回，这时候再调用recv函数，将数据从内核空间拷贝到用户空间，然后处理数据，`select`相当于一个管理器

+ **信号驱动I/O**

  ![image-20220707152651117](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220707152651117.png)

  用户空间程序可以建立一个`SIGIO`的信号处理程序，然后程序就可以做其他的事情，处理其他的任务。一旦有数据到来，就以信号的方式通知应用程序，然后就转到了信号处理程序，信号处理程序就可以调用recv来接收数据， 将数据从内核空间拷贝到用户空间。拷贝完成之后recv返回。因为数据到来之后以信号的方式通知才会调用recv，因此recv不会阻塞。

+ **异步I/O**

  ![image-20220707153938268](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220707153938268.png)

  效率最高，使用`aio_read()`函数实现，`aio_read()`提交一个请求，并且递交一个应用层的缓冲区buf，此时即使内核中没有数据到来，也会立即返回，一旦返回之后，应用进程就可以处理其他的事情，达到了一个异步处理的效果，当有数据到来之后，内核就会自动将数据从内核接收缓冲区拷贝到应用程的缓冲区buf，拷贝完成之后，会通过一个信号来通知应用进程的上层程序，上层程序直接去处理数据。在这种模型中，一旦得到了一个信号，此时数据已经从内核拷贝到用户空间了，无需再调用recv函数，数据直接从内核推送到用户进程的缓冲区当中.

## select函数

+ 功能：相当于管理者，管理多个I/O，一旦其中一个或者多个I/O检测到我们输入，select函数返回

  select既可以检测标准输入IO，又可以从检测网络IO，避免了阻塞在其中一个IO，而无法处理另一个IO

+ 原型： **int** **select(int** nfds**,** **fd_set** *****readfds**,** **fd_set** *****writefds**,**                                           

  ​                  **fd_set** *****exceptfds**,** **struct** **timeval** *****timeout**);**

  

+ 参数：

  + nfds：存放到三个集合中的最大的文件描述符 + 1
  + fd_set *readfds：数据可读的套接口集合
  + fd_set *writefds：数据可写的套接口集合
  + fd_set *execptfds：异常的套接口集合
  + struct timeval *timeout：超时时间，超时之后返回0

+ 返回值：检测到IO事件的个数，失败返回-1

常用的搭配函数：

+ `void FD_CLR(int fd, fd_set *set)`：将文件描述符fd从集合中移除
+ `int FD_ISSET(int fd,fd_set *set)`：判断文件描述符fd是否在集合中
+ `void FD_SET(int fd,fd_set *set)`：将文件描述符fd添加进set当中
+ `void FD_ZERO(fd_set *set)`：清空集合

## 读、写、异常时间发生条件

+ 可读
  + 套接口缓冲区有数据可读
  + 连接的读一半关闭，即接收到FIN段，读操作将返回0
  + 如果是监听端口，已完成连接队列不为空时
  + 套接口上发生了一个错误待处理，错误可以通过`getsockopt`指定`SO_ERROR`选项来获取

+ 可写
  + 套接口发送缓冲区有空间容纳数据
  + 连接的写一半关闭。即收到`RST`段后，再次调用`write`函数，会产生`SIGPIPIE`信号
  + 套接口上发生一个错误待处理，错误可以通过`getsockopt`指定`SO_ERROR`来获取

+ 异常
  + 套接口存在带外数据

## close与showdown的区别

+ close终止了数据传输的两个方向（不能向对等方发送数据，也不能从对等方接收数据）
+ showdown可以有选择的终止某个方向的数据传送或者终止数据传送的两个方向
+ showdown  how=1就可以保证对等方接收到一个EOF字符，而不管其他进程是否已经打开了套接字，而close不能保证，直到套接字引用计数减为0时才发送。也就是说直到所有的进程都关闭了套接字。

~~~c++
int conn;
conn = accept(sock,NULL,NULL);
pid_t pid = fork();

if(pid == -1)
    ERR_EXIT("fork");

if(pid == 0)
{
	close(sock);
    close(conn); //conn引用计数减为0时，才会向对等方发送FIN段
} else if(pid > 0){
    shutdown(conn,SHUT_WR);//不理会计数，直接向对方发送FIN段
    close(conn);  //将套接字的引用计数减一，引用计数减为零的时候才会向对等方发送
}
~~~

**使用情景：**

如果在客户端关闭套接口之后，服务端还必须接收到在管道之中残留的数据，此时必须使用showdown关闭单向连接，而不是使用close关闭双向连接。

## 套接字I/O超时设置方法

+ select实现超时（常用）

  + read_timeout函数封装

    ~~~c++
    int ret = 0;
    ret = read_timeout(fd,5);//检测是否超时
    if(ret == 0)
    {
        read(fd,...);
    } else if(ret == -1 && errno == ETIMEDOUT)
    {
        timeout...
    } else
    {
        ERR_EXIT("read_timeout");
    }
    
    /* select实现read_timeout
    	read_timeout -读超时检测函数，不含读操作
    	fd: 文件描述符
    	wait_seconds: 超时等待秒数，如果为0表示不检测超时
    	成功(未超时)返回0，失败返回-1，超时返回-1且errno = ETIMEDOUT
    */
    int read_timeout(int fd,unsigned int wait_seconds)
    {
        int ret = 0;
        if(wait_seconds > 0)
        {
            fd_set read_fdset;
    		struct timeval timeout;
            
            FD_ZERO(&read_fdset);
            FD_SET(fd,&read_fdset);
            
            timeout.tv_sec = wait_seconds;
            timeout.tv_usec = 0;
            do{
                ret = select(fd + 1,&read_fdset,NULL,NULL,&timeout);
            } while(ret < 0 && errno == EINTR);
            if(ret == 0)
            {
                ret = -1;
                errno = ETIMEDOUT;
            }
            else if (ret == 1)
    			ret = 0;        
        }
        return ret;
    }
    ~~~

  + write_timeout函数封装

    ~~~c++
    /* select实现write_timeout
    	write_timeout -写超时检测函数，不含写操作
    	fd: 文件描述符
    	wait_seconds: 超时等待秒数，如果为0表示不检测超时
    	成功(未超时)返回0，失败返回-1，超时返回-1且errno = ETIMEDOUT
    */
    int write_timeout(int fd,unsigned int wait_seconds)
    {
        int ret = 0;
        if(wait_seconds > 0)
        {
            fd_set write_fdset;
    		struct timeval timeout;
            
            FD_ZERO(&write_fdset);
            FD_SET(fd,&write_fdset);
            
            timeout.tv_sec = wait_seconds;
            timeout.tv_usec = 0;
            do{
                ret = select(fd + 1,NULL,&write_fdset,NULL,&timeout);
            } while(ret < 0 && errno == EINTR);
            if(ret == 0)
            {
                ret = -1;
                errno = ETIMEDOUT;
            }
            else if (ret == 1)
    			ret = 0;        
        }
        return ret;
    }
    ~~~

    

  + accept_timeout函数封装

    ~~~c++
    /* accept_timeout - 带超时的accept - 包含accept操作
    	fd: 套接字
    	addr: 输出参数，返回对方地址
    	wait_seconds：等待查实秒数，如果为0表示正常模式
    	成功(未超时)返回已连接套接字，超时返回-1且errno = ETIMEDOUT
    */
    int accept_timeout(int fd,struct sockaddr_in *addr,unsigned int wait_seconds)
    {
        int ret;
        socklen_t addrlen = sizeof(struct sockaddr_in);
        
        if(wait_seconds > 0){
            fd_set accept_fdset;
            struct timeval timeout;
            FD_ZERO(&accept_fdset);
        	FD_SET(fd,&accept_fdset);
            timeout.tv_sec = wait_seconds;
            timeout.tv_usec = 0;
            
           	do{
                ret = select(fd + 1,&accept_fdset,NULL,NULL,&timeout);
            }while(ret < 0 && errno == EINTR);
            if (ret == -1)
                return -1;
            else if(ret == 0)
            {
                errno = ETIMEDOUT;
                return -1;
            }
        }
        
        if (addr != NULL)
            ret = accept(fd,(struct sockaddr*)addr,&addrlen);
        else
            ret = accept(fd,NULL,NULL);
        if(ret == -1)
            ERR_EXIT("accept");
        return ret;
    } 
    ~~~

  + connect_timeout函数封装

    ~~~c++
    /* connect_timeout - connect
    	fd: 套接字
    	addr: 要连接的对方地址
    	wait_seconds：等待超时秒数，如果为0表示正常模式
    	成功(未超时)返回0，失败返回-1，超时返回-1且errno = ETIMEDOUT
    */
    
    void activate_nonblock(int fd)//设置文件描述符为非阻塞模式
    {
        int ret;
        int flags = fcntl(fd,F_GETFL);//先获取文件的标志
        if (flags == -1)
            ERR_EXIT("fcntl");
        flags |= 0_NONBLOCK;//添加非阻塞模式
        ret = fcntl(fd,FSETFL,flags);//设置
        if(ret == -1)
            ERR_EXIT("fcntl");
    }
    
    void deactivate_nonblock(int fd)//设置文件描述符为阻塞模式
    {
        int ret;
        int flags = fcntl(fd,F_GETFL);//先获取文件的标志
        if (flags == -1)
            ERR_EXIT("fcntl");
        flags &= 0_NONBLOCK;//添加非阻塞模式
        ret = fcntl(fd,FSETFL,flags);//设置
        if(ret == -1)
            ERR_EXIT("fcntl");
    }
    
    int connect_timeout(int fd,struct sockaddr_in *addr,unsigned int wait_seconds)
    {
        int ret;
        socklen_t addrlen = sizeof(struct sockaddr_in);
        
        if(wait_seconds > 0)
            activate_nonblock(fd);
    	
        ret = connect(fd,(struct sockaddr*)addr,addrlen);
        if (ret < 0 && errno == EINPROGRESS) //连接正在处理当中
        {
            fd_set connect_fdset;
            struct timeval timeout;
            FD_ZERO(&connect_fdset);
            FD_SET(fd,&connect_fdset);
            timeout.tv_sec = wait_seconds;
            timeout.tv_usec = 0;
            do{
               /* 一旦连接建立，套接字就可写 */
               ret = select(fd + 1,NULL,&connect_fdset,NULL,&timeout);
            }while(ret < 0 && errno == EINTR);
            if (ret == 0)
            {
                ret = -1;
                errno = ETIMEDOUT;
            }
            else if (ret < 0)
                return -1;
            else if (ret == 1)
            {
                /* ret返回为1 可能有两种情况，一种是连接建立成功，一种是套接字产生错误 */
                /* 此时错误信息不会保存至errno变量中，因此，需要调用getsickopt获取 */
            	int err;
                socklen_t socklen = sizeof(err);
                int sockoptret = getsockopt(fd,SOL_SOCKET,SO_ERROR,&err,&socklen);
                if (sockoptret == -1)
                {
                    return -1;
                }
                if (err == 0)
                    ret = 0;
                else
                {
                    errno = err;
                    ret = -1;
                }
            }
        }
        if (wait_seconds > 0)
        {
            deactivate_nonblock(fd);
        }
        return ret;
    }
    ~~~

+ alarm（基本不会使用）

  ~~~c++
  void handler(int sig)
  {
      return;
  }
  signal(SIGALRM,handler);
  alarm(5);//设置时间限制为5 若5秒钟没有返回数据，就会产生SIGALRM信号，打断read函数
  int ret = read(fd,buf,sizeof buf);//打断之后，此时就可以判断ret返回值
  if(ret == -1 && errno == EINTR)
  {
      errno = ETIMEDOUT;//设置错误信号为超时
  }
  else if(ret >=0 )
  {
      alarm(0);//关闭闹钟
  }
  ~~~

+ 套接字选项（少使用）

  + SO_SNDTIMEO 发送的超时时间
  + SO_RCVTIMEO  接受的超时时间

  ~~~c++
  setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,5);
  
  int ret = read(sock,buf,sizeof buf);
  if(ret == -1 && errno = EWOULDBLOCK)
  {
      errrno = ETIMEDOUT;
  }
  ~~~


## select限制与poll函数

**select函数**

select实现的并发服务器，能达到的并发数受两个方面的限制

+ 一个进程能打开的最大文件描述符限制，这可以通过调整内核参数
+ select中的fd_set集合容量的限制(FD_SETSIZE)，这需要重新编译内核

~~~c++
客户：
0 1 2
1021 个连接
创建第1022个套接字的时候失败了 sleep(4)
    
服务：
已完成连接队列中维护1021个条目
~~~

**poll函数**

只受到系统能打开的最大文件描述符的限制（跟内存有关系）

+ #include "poll.h"

+ int poll(struct pollfd *fds, nfds_t nfds, int timeout);

+ struct pollfd *fds   事件IO数组的指针

  ~~~c++
  struct pollfd{
      int fd;         /* file descriptor */
      short events;	/* requested events */
      short revents;	/* returned events*/
  }
  ~~~

+ nfds：IO的个数
+ timeout：超时时间
+ sudo ulimit -n number：调整打开文件描述符个数
+ cat  /proc/sys/fs/file-max

![image-20220711015054156](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220711015054156.png)

**共同点：**

内核要遍历所有的文件描述符，直到找到发生事件的文件描述符，并发数增长时，效率直线下降

## epoll函数的使用

+ #include <sys/epoll.h>
+ int epoll_create(int size);   //创建一个epoll实例
+ int epoll_create1(int flags);  

+ int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);  //将一个文件描述符添加到epoll来管理
+ int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) //等待事件

**epoll与select，poll的区别：**

+ 相比于select与poll，epoll最大的好处在于它不会随着监听fd数目的增加而降低效率
+ 内核中的select与poll是采用轮询来处理的，轮询的fd数目越多，自然耗时越多
+ epoll的实现是基于回调的，如果fd有期望的事件发生就通过回调函数，将其加入epoll就绪队列中，也就是说它只关心活跃的fd，与fd数目无关。
+ 内核/用户空间内存拷贝问题，如何让内核把fd消息通知给用户空间呢？在这个问题上，select/poll采用了内存拷贝的方法，而epoll采用了内存共享的方法
+ epoll不仅会告诉应用程序有I/O事件的到来，还会告诉应用程序相关的信息，这些信息是应用程序填充的，因此根据这些信息就能直接定位到事件，而不必遍历整个fd集合

**epoll模式：**

+ **EPOLLLT：**电平触发模式
  + 完全靠kernel epoll驱动，应用程序员只需要处理从epoll_wait返回的fds，这些fds我们认为他们处于就绪状态。
+ **EPOLLET：**边缘触发模式（通常情况下效率更高）
  + 此模式下，系统仅仅通知应用程序员哪些fds变成了就绪状态，一旦fd变成就绪状态，epoll将不再关注这个fd的任何状态信息（从epoll队列之中移除），直到应用程序通过读写操作触发EAGAIN状态，epoll认为这个fd又变为空闲状态，那么epoll又重新关注这个fd的状态变化（重新加入epoll队列）
  + 随着epoll_wait的返回，队列中的fds是在减少的，所以在大并发的系统中，EPOLLET更有优势。但是对程序员的要求也更高。

## UDP服务器

+ **UDP特点**
  + 无连接（没有维护端到端的连接状态）
  + 基于消息的信息传输服务（包之间有边界）（TCP是基于字节流）
  + 不可靠
  + 一般情况下UDP更加高效

+ **UDP客户/服务基本模型**

  ![image-20220712172519282](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220712172519282.png)

+ **UDP回射客户服务器**

  ![image-20220712172950533](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220712172950533.png)

+ **套接口（sock）属性**
  + 本地地址：getsockname()获取
  + 远程地址（对于已连接套接口）getpeername()获取

+ **UDP注意点**

  + UDP报文可能会丢失，重复

  + UDP报文可能会乱序

  + UDP缺乏流量控制

  + UDP协议数据报文截断（发送的数据大于接收缓冲区）

  + recvfrom返回0，不代表连接关闭，因为udp是无连接的

  + ICMP异步错误

    ```c++
    sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
    ```

    在服务端未启动时，调用sendto，会将sendbuf缓冲区中的内容拷贝到套接口缓冲区中，但是无法到达对等方，这时候会产生一个ICMP错误，或有一个ICMP的报文应答给客户端，这个应答会在recvfrom的时候才会返回给客户端，但是规定异步的错误不能返回给未连接的套接口，所以无法接收此错误。

  + UDP connect

    UDP也可以调用connect（并未真的连接，只是维护一些状态，指定远程地址），此时可以检测到异步错误，并且在调用recvfrom时不需要指定地址了。

  + UDP外出接口的确定

## UDP聊天室实现

+ 聊天室架构图

  ![image-20220712224346289](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220712224346289.png)

  

## UNIX域协议

+ UNIX域协议特点

  + UNIX域套接字与TCP套接字相比较，在同一台主机的传输速度前者是后者的两倍
  + UNIX域套接字可以在同一台主机上各进程之间传递描述符
  + UNIX域套接字与传统套接字的区别是用路径名来表示协议族的描述

+ UNIX域地址结构

  ~~~c++
  #define UNIX_PATH_MAX 108
  struct sockaddr_un {
      sa_family_t sun_family;   /* AF_UNIX */
      char sun_path[UNIX_PATH_MAX]; /* pathname */
  };
  ~~~

+ UNIX域字节流回射客户/服务

+ UXIN域套接字编程注意点

  + bind成功将会创建一个文件，权限为0777 & umask
  + sun path最好用一个绝对路径
  + UNIX域协议支持流式套接口与报式套接口
  + UNIX域协议套接字connect发现监听队列满时，会立即返回一个ECONNREFUSED，这和TCP不同，如果监听队列满，会忽略到来的SYN，这导致对方重传SYN.

+ socketpair函数

  + 功能：创建一个全双工的流管道（类似于匿名管道，父子亲缘进程间通信，因为共享了文件描述符）
  + 原型：int socketpair(int domain, int type, int protocol, int sv[2]);
  + 参数：
    + domain：协议家族
    + type：套接字类型
    + protocal：协议类型
    + sv：返回套接字对

## 通过UNIX域套接字传递描述符和sendmsg/recvmsg函数

UNIX域套接字可以在同一台主机上各进程之间传递文件描述符

首先先看两个函数：

~~~c++
#include <sys/tpyes.h>
#include <sys/socket.h>

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);

#它们与sendto函数和recvfrom函数相似，只不过可以传输更复杂的数据结构，不仅可以传输一般数据，还可以传输额外的数据，即文件描述符，下面来看结构体 msghdr:

struct msghdr {
               void         *msg_name;       /* optional address */
               socklen_t     msg_namelen;    /* size of address */
               struct iovec *msg_iov;        /* scatter/gather array */
               size_t        msg_iovlen;     /* # elements in msg_iov */
               void         *msg_control;    /* ancillary data, see below */
               size_t        msg_controllen; /* ancillary data buffer len */
               int           msg_flags;      /* flags on received message */
           };       
~~~

如下图所示：

![img](https://img-blog.csdn.net/20130612182041437)

1、msg_name ：即对等方的地址指针，不关心时设为NULL即可；

2、msg_namelen：地址长度，不关心时设置为0即可；

3、msg_iov：是结构体iovec 的指针。 

~~~c++
struct iovec {
        void *iov_base;  /* Starting address */
        size_t iov_len;   /* Number of bytes to transfer */
      };
~~~

成员iov_base 可以认为是传输正常数据时的buf，iov_len 是buf 的大小。

4、msg_iovlen：当有n个iovec 结构体时，此值为n；

5、msg_control：是一个指向cmsghdr 结构体的指针

 ~~~c++
 struct cmsghdr {
       socklen_t cmsg_len;  /* data byte count, including header */
       int    cmsg_level; /* originating protocol */
       int    cmsg_type;  /* protocol-specific type */
       /* followed by unsigned char cmsg_data[]; */
     };
 ~~~

6、msg_controllen ：参见下图，即cmsghdr 结构体可能不止一个；

7、flags ： 一般设置为0即可；

![img](https://img-blog.csdn.net/20130612183242312)

这里解释一下send_fd函数：

~~~c++
msg.msg_name = NULL;
msg.msg_namelen = 0;
msg.msg_iov = &vec;
msg.msg_iovlen = 1; //主要不是为了传输数据，故只传递1个字符
msg.msg_flags = 0;
vec.iov_base = &sendchar;
vec.iov_len = sizeof(sendchar);
~~~

这里目的是为了传输文件描述符，所以只定义了一个字节的char

现在我们有一个cmsghdr结构体，把需要传递的文件描述符send_fd长度，也就是需要传输的额外数据大小，当作参数传递给CMSG_SPACE宏，就可以得到整个结构体的大小，包括一些填充字节，如上图所示，也即是

~~~c++
char  cmsgbuf[CMSG(sizeof(send_fd))];
#也就可以进一步得出以下两行：
msg.msg_control = cmsgbuf;
msg.msg_controllen = sizeof(cmsgbuf);
~~~

接着，需要填充cmsghdr结构体，传入msghdr指针，CMSG_FIRSTHDR宏可以得到首个cmsghdr结构体的指针，即p_cmsg = CMSG_FIRSTHDR(&msg);

然后使用指针来填充各个字段，如下：

~~~c++
p_cmsg -> cmsg_level = SOL_SOCKET;
p_cmsg -> cmsg_type = SCM_RIGHTS;
p_cmsg -> cmsg_len = CMSG_LEN(sizeof(send_fd))
~~~

传入 send_fd 的大小，CMSG_LEN 宏可以得到cmsg_len 字段的大小

最后，传入结构体指针p_cmsg，宏CMSG_DATA可以得到准备存放send_fd的位置指针，将send_fd放进去，如下：

~~~c++
p_fds= (int*)CMSG_DATA(p_cmsg);
*p_fds = send_fd;   //通过传递辅助数据的方式传递文件描述符
~~~

## 进程之间的通信（一）

+ 进程同步与进程互斥
+ 进程间通信目的
  + 数据传输
  + 资源共享
  + 通知事件：通知另一个进程发生了某种事件
  + 进程控制：Debug进程，此时控制进程希望能拦截另一个进程的的所有陷入和异常（SIGTRAP信号）
+ 进程间通信发展
  + 管道
  + System V进程间通信
  + POSIX进程间通信
+ 进程间通信分类
  + 文件
  + 文件锁
  + 管道（pipe）和有名管理（FIFO）
  + 信号（signal）
  + 消息队列（System V & POSIX）
  + 共享内存（System V & POSIX）
  + 信号量（System V & POSIX）
  + 互斥量（POSIX）
  + 条件变量（POSIX）
  + 读写锁（POSIX）
  + 套接字（socket）
+ 进程间共享信息的三种方式
  + 文件系统
  + 内核当中共享信息
  + 共享内存区
+ IPC对象的持续性
  + 随进程持续：一直存在直到打开的最后一个进程结束（如pipe 和FIFO）
  + 随内核持续：一直存在直到内核自举或者显式删除（**如System V消息队列，共享内存，信号量**）
  + 随文件系统持续：一直存在直到显式删除，即使内核自举还存在（**POSIX消息队列，共享内存，信号量如果是使用映射文件来实现**）

## 进程之间的通信（二）

+ 死锁

  + 互斥条件
  + 请求和保持：资源一次性分配（破坏请求和保持条件）
  + 不可剥夺：可剥夺资源（破坏不可剥夺条件）
  + 环路等待：资源有序分配法（破坏循环等待条件）

+ 信号量与PV原语

  信号量：

  + 互斥：PV在同一个进程中
  + 同步：PV在不同的进程中

  信号量值含义：

  + S > 0：S表示可用资源的个数
  + S = 0：表示可用资源，无等待进程
  + S < 0：|S|表示等待队列中进程个数

  ~~~c++
  struct semaphore
  {
      int value;
      pointer_PCB queue;
  }
  
  void P(s) //原子性操作
  {
      s.value = s.value--;
      if (s.value < 0)
      {
          该进程进入等待状态
          将该进程的PCB插入相应的等待队列s.queue末尾
      }
  }
  void V(s) //原子性操作
  {
      s.value = s.value++;
      if (s.value <= 0)
      {
          唤醒相应等待队列s.queue中等待的一个进程
          改变其状态为就绪态
          并将其插入就绪队列
      }
  }
  ~~~

## System V消息队列（一）

+ 消息队列提供了一个从一个进程向另一个进程发送一块数据的方法（与管道不同，管道是基于字节流的，消息之间没有边界，且先进先出）
+ 每个数据块都被认为是有一个类型（消息，有边界），接收者进程接收的数据块可以有不同的类型值（不一定按照顺序接收）
+ 消息队列也有管道一样的不足，就是每个消息的最大长度是有上限的（MSGMAX），每个消息队列的总的字节数是有上限（MSGMNB）

+ IPC对象数据结构，内核为每个IPC对象（消息队列，共享内存，信号量）维护一个数据结构

  ~~~c++
  struct ipc_perm{
      key_t __key; //key supplied to xxxget(2)
      uid_t uid;   //effective UID of owner
      gid_t gid;   //effective GID of owner
      uid_t cuid;  //effective UID of creator
      gid_t cgid;  //effective GID of creator
  	unsigned short mode;    // permissions
      unsigned short __seq;   // sequence number
  }
  ~~~

+ 消息队列数据结构

  ~~~c++
  struct msqid_ds{
      struct ipc_perm msg_perm; //ownership and permissions
      time_t msg_stime; //time of last msgsnd
      time_t msg_rtime; //time of last msgrcv
      time_t msg_ctime; //time of last change
      unsigned long __msg_cbytes;//current number of bytes in queue
      
      msgqnum_t msg_qnum;  //current number of messages in queue
      msglen_t msg_qbytes; //maximin number of bytes allowed in queue = MSGMNB
      pid_t msg_lspid;	//PID of last msgsnd
      pid_t msg_lrpid;	//PID of last msgrcv
  }
  ~~~

+ 消息队列在内核中的表示

  ![image-20220719234309650](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220719234309650.png)

+ 消息队列函数

  ~~~c++
  #include <sys/types.h>
  #include <sys/ipc.h>
  #include <sys/msg.h>
  
  int msgget(key_t key, int msgflg); //创建或者打开消息队列
  int msgctl(int msqid, int cmd, struct msqid_ds *buf);//控制消息队列
  int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);//发送一条消息
  ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);//接收一条消息
  ~~~

  + **msgget函数 ：**创建或者返回一个消息队列

    + 原型：int msgget(key_t key, int msgflg);

    + 参数：
      + key：某个消息队列的名称
      + msgflg：由九个权限标志构成，它们的用法和创建文件时使用的mode模式标志是一样的

    + 返回值：成功返回一个非负整数，即该消息队列的标识码，失败返回-1

      ![image-20220720004543790](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220720004543790.png)

      

  + **msgctl函数**：消息队列的控制函数

    + 原型：int msgctl(int msqid, int cmd, struct msqid_ds *buf);

    + 参数：

      + msqid：由msgget函数返回的消息队列标识码

      + cmd：将要采取的动作（有3个可取值）

        |   cmd    |                         explanation                          |
        | :------: | :----------------------------------------------------------: |
        | IPC_STAT |        把msqid_ds结构中的数据设置为消息队列当前关联值        |
        | IPC_SET  | 在进程有足够权限的前提下，把消息队列的当前关联值设置为msqid_ds数据结构中给出的值 |
        | IPC_RMID |                         删除消息队列                         |

        

    + 返回值：成功返回0，失败返回-1

## System V消息队列（二）

+ **msgsnd函数**

  + 功能：把一条消息添加到消息队列中

  + 原型：int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);

  + 参数：

    + msgid：由msgget函数返回的消息队列标识码

    + msgp：是一个指针，指针指向准备发送的消息

    + msgsz：是msgp指向的消息长度，这个消息长度不含保存消息类型的那个long int长整型

    + msgflg：控制着当前消息队列满或到达系统上限时的操作，msgflg = IPC_NOWAIT表示队列满不等待，返回EAGAIN错误

    + 消息结构在两方面受到制约，首先，它必须小于系统规定的上限值；其次，它必须以一个long int长整数开始，接收者函数利用这个长整数确定消息的类型

    + 消息结构参考形势如下：

      ~~~c++
      struct msgbuf{
          long mtype;
          char mtext[1];
      }
      ~~~

+ **msgrev函数**：成功返回0；失败返回-1

  + 功能：是从一个消息队列接收消息
  + 原型：ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
  + 参数：
    + msgid：由msgget函数返回的消息队列标识码
    + msgp：是一个指针，指针指向准备接收的消息
    + msgsz：是msgp指向的消息长度，这个长度不包含保存消息类型的那个long int长整型
    + msgtype：它可以实现接收优先级的简单形式
    + msgflg：控制着消息队列中没有相应类型的消息可供接收时的操作
  + 返回值：成功返回实际放到接收缓冲区里去的字符个数，失败返回-1
  + megtype 与 msgflag接收类型 
    + msgtype = 0 返回队列第一条信息
    + msgtype > 0 返回队列第一条类型等于msgtype的消息
    + msgtype < 0 返回队列第一条类型小于等于msgtype绝对值的消息，且消息的类型值最小
    + msgflg = IPC_NOWAIT 队列没有可读消息不等待，返回ENOMSG错误
    + msgflg = IPC_NOERROR 消息大小超过msgsz时被截断
    + msgtype > 0 且 msgflg = MSC_EXCEPT 接收类型不等于msgtype的第一条消息

## 消息队列实现一个回射客户/服务器

架构如下：

![image-20220720152734421](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220720152734421.png)

可能会产生死锁，可以换一种方式实现：（通过私有队列回复）

![image-20220720171031175](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220720171031175.png)

## System V 共享内存（一）

+ 共享内存

  共享内存区是最快的IPC形式。一旦这样的内存映射到共享它的进程的地址空间，这些进程间数据传递不再涉及到内核，换句话说是进程不再通过执行进入内核的系统调用来传递彼此的数据。

+ 共享内存示意图

  ![image-20220720190556266](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220720190556266.png)

+ 管道，消息队列与共享内存传递数据对比

  管道或者消息队列传递数据（4次系统调用和内存拷贝）

  ![image-20220720190930209](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220720190930209.png)

  用共享内存传递数据

  ![image-20220720202405717](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220720202405717.png)

+ **mmap函数**

  + **功能：**将文件或者设备空间映射到共享内存区

  + **原型：**void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);

  + **参数：**

    + addr：要映射的起始地址，通常指定为NULL，

    + len：映射到进程地址空间的字节数

    + prot：映射去保护方式

    + flags：标志

    + fd：文件描述符

    + offset：从文件头开始的偏移量

      | prot       |     说明     |
      | ---------- | :----------: |
      | PROT_READ  |   页面可读   |
      | PROT_WRITE |   页面可写   |
      | PROC_EXEC  |  页面可执行  |
      | PROT_NONE  | 页面不可访问 |

      | flags         | 说明                       |
      | ------------- | -------------------------- |
      | MAP_SHARED    | 变动是共享的               |
      | MAP_PRIVATE   | 变动是私有的               |
      | MAP_FIXED     | 准确解释addr参数           |
      | MAP_ANONYMOUS | 建立匿名映射区，不涉及文件 |

  ![image-20220720211706496](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220720211706496.png)

  + 返回值：成功返回映射到的内存区的地址空间，失败返回-1

+ **munmap函数**

  + **功能：**取消mmap函数建立的映射
  + **原型：**int munmap(void *addr, size_t len);
  + **参数：**
    + addr：映射的内存起始地址
    + len：映射到进程地址空间的字节数
  + **返回值：**成功返回0；失败返回-1

+ **msync函数**

  + 功能：对映射的共享内存执行同步操作

  + 原型：int msync(void *addr, size_t len, int flags);

    |     flags     |         说明         |
    | :-----------: | :------------------: |
    |   MS_ASYNC    |      执行异步写      |
    |    MS_SYNC    |      执行同步写      |
    | MS_INVALIDATE | 使高速缓存的数据失效 |

  + 参数：

    + addr：内存起始地址
    + len：长度
    + flags：选项

  + 返回值：成功返回0；失败返回-1

+ map注意点
  + 映射不能改变文件的大小
  + 可用于进程间通信的有效地址空间不完全受限于被映射文件的大小
  + 文件一旦被映射后，所有对映射区域的访问实际上是堆内存区域的访问。映射区域内容写回文件时，所写内容不能超过文件的大小、

## System V共享内存（二）

+ 共享内存数据结构

  ~~~c++
  struct shmid_ds{
      struct ipc_perm shm_perm; //Ownership and permissions
      size_t shm_segsz;		  //size of segment (bytes)
  	time_t shm_atime;			//last attach time
      time_t shm_dtime;			//last detach time
      time_t shm_ctime;			//last change time
      pid_t  	shm_cpid;			//PID of creator
      pid_t	shm_lpid;			//PID of last shmat(2)
      shmatt_t shm_nattch;		//No of current attaches
      ...
  }
  ~~~

+ 共享内存函数

  ~~~c++
  #include <sys/ipc.h>
  #include <sys/shm.h>
  
  int shmget(key_t key, size_t size, int shmflg);
  void *shmat(int shmid, const void *shmaddr, int shmflg);
  int *shmdt(const void *shmaddr);
  int shmctl(int shmid, int cmd, struct shmid_ds *buf);
  ~~~

  + **shmget函数**

    + 功能：用来创建共享内存

    + 原型：

      int shmget(key_t key, size_t size, int shmflg);

    + 参数

      + key：这个共享内存段名字
      + size：共享内存大小
      + shmflg：由九个权限标志组成，它们的用法和创建文件时使用的mode模式标志是一样的

    + 返回值：成功返回一个非负整数，即该共享内存段的标识码，失败返回-1

  + **shmat函数**

    + 功能：将共享内存段连接到进程的地址空间

    + 原型：

      void *shmat(int shmid, const void *shmaddr, int shmflg);

    + 参数：

      + shmid：共享内存标识
      + shmaddr：指定连接的地址
      + shmflg：它的两个可能取值是SHM_RND和SHM_RDONLY

    + 返回值：成功返回一个指针，指向共享内存第一个字节；失败返回-1 

    + 注意点：

      + shmaddr通常情况下NULL，内核自动选择一个地址
      + shmaddr不为NULL，且shmflg无SHM_RND标志，则以shmaddr为连接地址
      + shmaddr不为NULL且shmflg设置了SHM_RND标记，则连接的地址会自动向下调整为SHMLBA的整数倍，公式：shmaddr - (shmaddr % SHMLBA)
      + shmflg = SHM_RDONLY, 表示连接操作用来只读共享内存

  + **shmdt**

    + 功能：将共享内存段与当前进程脱离

    + 原型：

      int shmdt(const void *shmaddr);

    + 参数：

      + shmaddr：由shmat所返回的指针

    + 返回值：成功返回0；失败返回-1

    + 注意：将共享内存段与当前进程脱离不等于删除共享内存段

  + **shmctl**

    + 功能：用来创建和访问一个消息队列

    + 原型：

      + shmid：由shmget返回的共享内存标识码

      + cmd：将要采取的动作（有三个可取值）

        |   cmd    |                          definition                          |
        | :------: | :----------------------------------------------------------: |
        | IPC_STAT |     把shmid_ds数据结构中的数据设置为共享内存的当前关联值     |
        | IPC_SET  | 在进程有足够权限的前提下，把共享内存的当前关联值设置为shmid_ds数据结构中给出的值 |
        | IPC_RMID |                        删除共享内存段                        |

        

      + buf：指向一个保存着共享内存的模式状态和访问权限的数据结构

    + 返回值：成功返回0；失败返回-1

  

## System V信号量（一）

+ 信号量

  + 信号量和PV原语由Dihkstra提出

  + 信号量

    + 互斥：P, V在同一个进程里
    + 同步：P,V在不同进程里

  + 信号量值含义

    + S > 0：S表示可用资源的个数
    + S = 0：表示无可用资源，无等待进程
    + S < 0：|S|表示等待队列中进程个数

    ~~~c++
    struct semaphore
    {
        int value;
        pointer_PCB queue;
    }
    P(s)
    {
        s.value--;
        if (s.value < 0)
        {
            该进程进入等待状态
            将该进程的PCB插入相应的等待队列s.queue的末尾
        }
    }
    V(s)
    {
        s.value++;
        if(s.value <= 0)
        {
            唤醒相应等待队列s.queue中等待的一个进程，并改变其状态为就绪态
            并将其插入就绪队列
        }
    }
    ~~~

+ 信号量集结构

  ~~~c++
  struct semid_ds
  {
  	struct ipc_perm sem_perm;   //Ownership and permissions
      time_t sem_otime;			//Last semop time
      time_t sem_ctime;			//Last change time
      unsigned short sem_nsems;	//No. of semaphores in set
  }
  ~~~

+ 信号量集函数

  ~~~c++
  #include <sys/types.h>
  #include <sys/ipc.h>
  #include <sys/sem.h>
  
  int semget(key_t key, int nsems, int semflg);
  int semctl(int semid, int semnum, int cmd,...);
  int semop(int semid, struct sembuf* *sops, unsigned nsops);//both of P,V
  ~~~

  + **semget函数**

    + 功能：用来创建和访问一个消息队列

    + 原型：

      int semget(key_t key, int nsems, int semflg);

    + 参数：

      + key：信号集的名称
      + nsems：信号集中信号量的个数
      + semflg：由九个权限标志构成，它们的用法和创建文件时使用的mode模式标志一样

    + 返回值：成功返回一个整数；失败返回-1；

  + **semctl函数**

    + 功能：用于控制信号量集

    + 原型：

      int semctl(int semid, int semnum, int cmd, ...);

    + 参数：

      + semid：由semget返回的信号集标识码

      + semnum：信号集中信号量的序号

      + cmd：将要采取的动作（有三个可取值）

        |   cmd    |                         explanation                          |
        | :------: | :----------------------------------------------------------: |
        |  SETVAL  |                设置信号量集中的信号量的计数值                |
        |  GETVAL  |                获取信号量集中的信号量的计数值                |
        | IPC_SATA |        把semid_ds结构中的数据设置为信号集的当前关联值        |
        | IPC_SET  | 在进程由足够权限的的前提下，把信号集的当前关联值设置为semid_ds数据结构中给出的值 |
        | IPC_RMID |                          删除信号集                          |

      + 最后一个参数根据命令不同而不同

    + 返回值：成功返回0，失败返回-1

  + **semop函数**

    + 功能：用来操作一个信号量（P, V操作）

    + 原型

      int semop(int semid, struct sembuf *sops, unsigned nsops);

    + 参数：

      + semid：是该信号量的标识码，也就是semget函数的返回值

      + sops：是个指向一个结构数值的指针

        ~~~c++
        struct sembuf
        {
            short sem_num;
            short sem_op;
            short sem_flg;
        };
        /*
        	sem_num是信号量的编号
        	sem_op是信号量一次PV操作时加减的数值，一般只会用到两个值，一个是"-1"，也就是P操作，等待信号量变得可用，另一个是"+1",也就是我们的V操作，发出信号量已经变得可用
        	sem_flag的两个取值是IPC_NOWAIP或SEM_UNDO 
        */
        ~~~

      + nsops：信号量的个数

    + 返回值：成功返回0；失败返回-1

## System V共享内存与信号量综合

+ 信号量解决生产者消费者问题
+ 实现shmfifo

## POSIX 消息队列

+ POSIX消息队列查看方式

  ~~~bash
# man 7 mq_overview
  消息队列存在于一个虚拟的文件系统当中，该文件系统需要挂载到某一个目录下才能用
# mkdir /dev/mqueue
  # mount -t mqueue none /dev/mqueue

  # 解除挂载
# sudo umount /dev/mqueue/
  ~~~

+ POSIX消息队列相关函数

  + **mq_open函数**

    + 功能：用来创建和访问一个消息队列

    + 原型：

      mqd_t mq_open(const char*name, int oflag); //用来打开一个消息队列

      mqd_t mq_open(const char*name, int oflag, mode_t mode, struct mq_attr *attr);

    + POSIX IPC名字限定规则

      + 必须以/打头，并且后续不能有其他/，形如/somename
      + 长度不能超过NAME_MAX
    
    + 参数

      + name：某个消息队列的名字
      + oflag：与open函数类似，可以是O_RDONLY, O_WRONLY, O_REWR, 还可以按位或上O_CREATE, O_EXCL, O_NONBLOCK等.
      + mode：如果oflag指定了O_CREAT, 还需要设置mode.
      + attr：指定消息队列的属性
    
    + 返回值：成功返回消息队列文件描述符；失败返回-1.
    
  + **mq_close函数**

    + 功能：关闭消息队列

    + 原型：

      mqd_t mq_close(mqd_t mqdes);

    + 参数

      + mqdes：消息队列描述符

    + 返回值：成功返回0；失败返回-1

  + **mq_unlink函数**

    + 功能：删除消息队列

    + 原型：

      mqd_t mq_unlink(const char *name);

    + 参数：

      + name：消息队列的名称

    + 返回值：成功返回0；失败返回-1；

  + **mq_getattr/mq_setattr**

    + 功能：获取/设置消息队列属性
    + 原型：
      + mqd_t mq_getattr(mqd_t mqdes, struct mq_attr *attr);
      + mqd_t mq_setattr(mqd_t mqdes, struct mq_addr *newattr, struct mq_attr *oldattr);
    + 参数：
      + mqdes：消息队列描述符
      + newattr：新的消息队列属性
      + oldattr：旧的消息队列属性
      + 如果参数 *oldattr* 不是NULL, 函数*mq_setattr*() 将会把旧的属性存储到*oldattr* 指向的内存
    + 返回值：成功返回0；失败返回-1；

  + **mq_send函数**

    + 功能：发送消息

    + 原型：

      mqd_t mq_send(mqd_t mqdes, const char* msg_ptr, size_t msg_len, unsigned msg_prio);

    + 参数：

      + mqdes：消息队列描述符
      + msg_ptr：指向消息的指针
      + msg_len：消息长度
      + msg_prio：消息优先级

    + 返回值：成功返回0；失败返回-1；

  + **mq_receive函数**

    + 功能：接收消息
    + 原型：
      + ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);
    + 参数：
      + mqdes：消息队列描述符
      + msg_ptr：返回可接收到的消息指针
      + msg_len：消息长度
      + msg_prio：返回接收到的消息优先级
    + 返回值：成功返回接收到的消息字节数；失败返回-1
    + 注意：返回指定消息队列中最高优先级的最早消息

  + **mq_notify函数**（有消息时，System V是无法获得通知的，而posix可以获得通知）

    + 功能：建立或者删除消息到达通知事件

    + 原型：

      mqd_t mq_notify(mqd_t mqdes, const struct sigevent *notification);

    + 参数

      + mqdes：消息队列描述符

      + notification：

        + 非空表示当消息到达且消息队列先前为空，那么将得到通知
        + NULL表示撤销已注册的通知

        ~~~c++
        union sigval  		//Data passed with notificaton
        {
        	int sival_int; 	//Interger value
            void *sival_prt;//Pointer value
        }
        
        struct sigevent
        {
            int sigev_notify;/* Notification method*/
            int sigev_signo;/* Notification signal*/
            union sigval sigev_value;/*Data passed with notification*/
            void (*sigev_notify_function)(union sigval);/*Function for thread notification*/
            void (*sigev_notify_attributes) /*Thread function attributes*/
        }
  ~~~

​        

    + 返回值：成功返回0；失败返回-1
    
    + 通知方式：
    
      + 产生一个信号；
      + 创建一个线程执行一个指定的函数
    
    + **mq_notify注意：**
    
      + 任何时刻只能有一个进程可以被注册为接收某个给定队列的通知
      + 当有一个消息到达某个先前为空的队列，而且已经有一个进程被注册为接收该队列的通知时，只有没有任何线程阻塞在该队列的mq_receive调用的前提下，通知才会发出。
      + 当通知被发送给它的注册进程时，其注册被撤销。进程必须再次调用mq_notify以重新注册，重新注册要放在消息队列读出消息之前而不是之后。

## POSIX共享内存

+ POSIX共享内存相关函数

  + **shm_open函数（所创建的共享内存为虚拟文件方式，已经挂载在/dev/shm/下）**

    + 功能：用来创建或者打开一个共享内存对象

    + 原型：

      int shm_open(const char*name, int oflag, mode_t mode);

    + 参数：

      + name：共享内存对象的名字
      + oflag：与open函数类似，可以是O_RDONLY, ORDWE, 还可以按位或上O_CREAT, O_EXCL, O_TRUNC等.
      + mode：此参数总是需要设置，如果oflag没有指定O_CREAT, 可以指定为0

    + 返回值：成功返回非负整数文件描述符，失败返回-1；

  + **ftruncate函数 -> 还可以修改文件大小**

    + 功能：修改共享内存大小；还可以用来修改文件大小；

    + 原型：

      int ftruncate(int fd, off_t length);

    + 参数

      + fd：文件描述符
      + length：长度

    + 返回值：成功返回0；失败返回-1；

  + **fstat函数 -> 还可以获取文件信息**

    + 功能：获取共享内存对象信息

    + 原型：

      int fstat(int fd, struct stat *buf);

    + 参数：

      + fd：文件描述符
      + buf：返回共享内存状态

    + 返回值：成功返回0；失败返回-1；

    + 注意：文件权限信息需扣除掩码（umask）

  + **shm_unlink函数**

    + 功能：删除一个共享内存对象

    + 原型：

      int shm_unlink(const char *name);

    + 参数：

      + name：共享内存对象的名字

    + 返回值：成功返回0；失败返回-1；

  + **mmap共享内存对象的映射函数**

    + 功能：将共享内存对象映射到进程地址空间

    + 原型：

      ~~~c++
      void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
      ~~~

    + 参数：

      + addr：要映射的起始地址，通常指定为NULL，让内核自动选择
      + len：映射到进程地址空间的字节数
      + prot：映射区保护方式
      + flags：标志
      + fd：文件描述符
      + offset：从文件头开始的偏移量

    + 返回值：成功返回映射到的内存区起始地址；失败返回-1；

## 线程介绍

+ 什么是线程

  + 在一个程序里的一个执行路线就叫做线程（thread）。更准确的定义是：线程是"一个进程内部的控制序列"。
  + 一切进程至少都有一个执行线程。
  + 进程是资源竞争的基本单位
  + 线程是程序执行的最小单位
  + 线程共享进程数据，但是也拥有自己的一部分数据（线程ID;一组寄存器;栈errno;信号状态;优先级）

+ 进程与线程

  fork和创建新线程的区别

  + 当一个进程执行一个fork调用的时候，会创建出进程的一个新拷贝，新进程将拥有它自己的变量和它自己的PID。这个新进程的运行时间是独立的，它在执行时几乎完全独立于创建它的进程。
  + 在进程里面创建一个新线程的时候，新的执行线程会拥有自己的堆栈（因此也就有自己的局部变量），但要与它的创建者共享全局变量，文件描述符，信号处理器和当前的工作目录状态

+ 线程优缺点

  **优点：**

  + 创建一个新线程的代价要比创建一个新进程小得多
  + 与进程之间的切换相比，线程之间的切换需要操作系统做的工作要少很多
  + 线程占有的资源要比进程少很多
  + 能充分利用多处理器的可并行数量
  + 在等待慢速*IO*操作结束的同时，程序可执行其他的计算任务
  + 计算密集型任务，为了能在多处理器系统上运行，将计算分解到多个线程中实现
  + I/O密集型应用，为了提高性能，将I/O操作重叠。线程可以同时等待不同的I/O操作

  **缺点：**

  + **性能损失**

    一个很少被外部事件阻塞的计算密集型线程往往无法与其他线程共享同一个处理器。如果计算密集型线程的数量比可用的处理器多，那么可能会有较大的性能损失，这里的性能损失指的是增加了额外的同步和调度开销，而可用的资源不变。

  + **健壮性降低**

    编写多线程需要更加全面更加深入的考虑，在一个多线程程序里，因时间分配上的细微偏差或者因共享了不该共享的变量而造成不良影响的可能性是很大的，换句话说线程之间是缺乏保护的.

  + **缺乏访问控制**

    进程是访问控制的基本粒度，在一个线程中调用某些OS函数会对整个进程造成影响。

  + **编程难度提高**

    编写与调试一个多线程程序比单线程程序困难得多.

+ 线程模型

  + N:1 用户线程模型
    + "线程实现" 建立在 "进程控制" 机制之上，由用户空间的程序库来管理，OS内核完全不知道线程信息，这些线程称为用户空间线程.
    + 这些线程都工作在"进程竞争范围".
    + 在**N:1**线程模型中，内核不干涉线程的任何生命活动，也不干涉同一进程中的线程环境切换
    + 在**N:1**线程模型中，一个进程中的多个线程只能调度到一个CPU，这种约束限制了可用的并行总量
    + 第二个缺点是如果某个线程执行了一个"阻塞式"操作（如read），那么，进程中的所有线程都会被阻塞，直到那个操作结束。为此，一些线程的实现是为这些阻塞式函数提供包装器，用非阻塞版本替换这些系统调用，以消除这种限制。
  + 1:1 核心线程模型
    + 在**1:1**核心线程模型中，应用程序创建的每一个线程都由一个内核线程直接管理
    + OS内核将每一个核心线程都调到系统CPU上，因此，所有线程都工作在"系统竞争范围".
    + 这种线程的创建与调度由内核完成，因为这种线程的系统开销比较大（但是比进程开销小）
  + N:M 混合线程模型
    + N:M混合线程模型提供了两级控制，将用户线程映射为系统的可调度体以实现并行，这个可调度体称为轻量级进程(LWP:lightweight process)，有可能多个用户进程映射到一个LWP上，LWP再一一映射到核心线程。

## POSIX线程（一）

+ 线程库介绍：

  + 与线程有关的函数构成了一个完整的系列，绝大多数函数的名字都是以`pthread_`打头的
  + 要使用这些函数库，要通过引入头文件`<pthread.h>`
  + 链接这些线程函数库时要使用编译器命令的`-lpthread`选项

+ 类比：

  |                            pid_t                             |                           thread_t                           |
  | :----------------------------------------------------------: | :----------------------------------------------------------: |
  |                             fork                             |                        pthread_create                        |
  |                           waitpid                            |                         pthread_join                         |
  |                exit ^ 在main函数中调用return                 |          pthread_exit ^ 在线程入口函数中调用return           |
  | 僵进程：子进程结束，父进程还未结束，子进程会保留一个状态，直到父进程调用waitpid，僵进程状态才被撤销 | 僵线程：新创建的线程结束，主线程未调用pthread_join，子线程处于僵线程状态；pthread_detach：脱离僵线程 |
  |                        kill：杀死进程                        |                 pthread_cancel：取消一个线程                 |

+ pthread线程库常见函数：

  + **pthread_create函数**

    + 功能：创建一个新的线程

    + 原型：

      ~~~c++
      int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg);
      ~~~

    + 参数

      + thread：返回线程ID
      + attr：设置线程属性，attr为NULL表示使用默认属性
      + start_routine：是个函数地址，线程启动后要执行的函数
      + arg：传给线程启动函数的参数

    + 返回值：成功返回0；失败返回错误码；
    
    + 错误检查
    
      + 传统的一些函数是，成功返回0，失败返回-1，并且对全局变量errno赋值以指示错误
      + `pthreads`函数出错时不会设置全局变量errno（大部分其他POSIX函数会这样做）。而是将错误代码通过返回值返回。
      + `pthreads`同样也提供了线程内的errno变量（线程库对每一个创建的线程，都提供了一个errno），以支持其它使用errno的代码。对于pthreads函数的错误，建议通过返回值判定，因为读取返回值要比读取线程内的errno变量的开销更小。  
  
  + **pthread_exit函数**
  
    + 功能：线程终止
  
    + 原型：
  
      ~~~c++
      void pathread_exit(void *value_ptr);
      ~~~
  
    + 参数：
  
      + vaule_ptr：value_ptr不要指向一个全局变量
  
    + 返回值：无返回值，跟进程一样，线程结束的时候无法返回到它的调用者
  
  + **pthread_self函数**
  
    + 功能：返回线程ID；
  
    + 原型：
  
      ~~~c++
      pthread_t pthread_self(void);
      ~~~
  
    + 返回值：成功返回0；
  
  + **pthread_cancel函数**
  
    + 功能：取消一个执行中的线程
  
    + 原型：
  
      ~~~c++
      int pthread_cancal(pthread_t thread);
      ~~~
  
    + 参数
  
      + thread：线程ID
  
    + 返回值：成功返回0；失败返回错误码
  
  + **pthread_detach函数**
  
    + 功能：将一个线程分离，避免僵尸线程
  
    + 原型：
  
      ~~~c++
      int pthread_detach(pthread_t thread);
      ~~~
  
    + 参数：
  
      + thread：线程ID
  
    + 返回值：成功返回0；失败返回错误码

## POSIX线程（二）

+ 线程属性

  + 初始化与销毁属性变量

    ~~~c++
    int pthread_attr_init(pthread_attr_t *attr);
    int pthread_attr_destroy(pthread_attr_t *attr);
    ~~~

  + 获取与设置分离属性，保存在detachstate中

    ~~~c++
    int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
    int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
    ~~~

  + 获取与设置栈大小

    ~~~c++
    int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
    int pthread_att_getstacksize(pthread_attr_t *attr, size_t *stacksize) ;
    ~~~

  + 设置栈溢出保护区的大小

    ~~~c++
    int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
    int pthread_attr_getguardsize(pthread_attr_t *attr, size_t *guardsize);
    ~~~

  + 获取与设置线程竞争范围

    ~~~c++
    int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope);
    int pthread_attr_setscope(const pthread_attr_t *attr, int contentionscope);
    ~~~

  + 获取与设置调度策略

    ~~~c++
    int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);
    int pthread_attr_setschedpolicy(const pthread_attr_t *attr, int policy);
    ~~~

  + 获取与设置继承的调度策略

    ~~~c++
    int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched);
    int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
    ~~~

  + 获取与设置调度参数

    ~~~c++
    int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param);
    int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
    ~~~

  + 获取与设置并发级别

    ~~~c++
    int pthread_setconcurrency(int new_level);
    int pthread_getconcurrency(void);
    ~~~

  + 仅在`N:M`模型中有效，设置并发级别，给内核一个提示，表示提供给定级别数量的核心线程来映射用户线程是高效的。

+ 线程特定数据

  + 在单线程程序中，我们经常要用到全局变量以实现多个函数间共享数据.

  + 在多线程环境下，由于数据空间是共享的，因此全局变量也为所有线程所共有.

  + 但是有时应用程序设计中有必要提供线程私有的全局变量，仅在某个线程中有效，但却可以跨多个函数访问.

  + POSIX线程库通过维护一定的数据结构来解决这个问题，这个数据结构称为（Thread-specific Data，或TSD

  + ![image-20220730205949376](C:\Users\chengzi\AppData\Roaming\Typora\typora-user-images\image-20220730205949376.png)

  + TSD相关函数

    ~~~c++
    int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
    int pthread_key_delete(pthread_key_t key);
    void *phread_getspecific(pthread_key_t key);
    int pthread_setspecific(pthread_key_t key, const void *value);
    int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));
    ~~~

## POSIX信号量与互斥锁

+ POSIX信号量相关函数

  + 有名信号量相关函数
    + sem_open()
    + sem_close()
    + sem_unlink()
  + 无名信号量
    + sem_init()
    + sem_destrop()
  + PV操作（既可用于有名，也可用于无名）
    + sem_wait()
    + sem_post()

+ POSIX互斥锁相关函数

  ~~~c++
  void pthread_mutex_init()
  void pthread_mutex_lock()
  void pthread_mutex_unlock()
  void pthread_mutex_destroy()
  ~~~

  

+ 自旋锁

  + 自旋锁类似于互斥锁，但是它的性能比互斥锁更高。

  + 自旋锁与互斥锁很重要的一个区别在于，线程在申请自旋锁的时候，线程不会被挂起，它处于忙等待的状态，CPU不停的申请资源。（适用于等待时间比较短的应用）

  + 相关函数

    ~~~c++
    void pthread_spin_init();
    void pthread_spin_destroy();
    void pthread_spin_lock();
    void pthread_spin_unlock();
    ~~~

+ 读写锁

  + 只要没有线程持有给定的读写锁用于写，那么任意数目的线程可以持有读写锁用于读

  + 仅当没有线程持有某个给定的读写锁用于读或用于写时，才能分配读写锁用于写

  + 读写锁用于读称为共享锁，读写锁用于写称为排他锁

  + 相关函数

    ~~~c++
    void pthread_rwlock_init();
    void pthread_rwlock_destroy();
    int pthread_rwlock_rdlock();
    int pthread_rwlock_wrlock();
    int pthread_rwlock_unlock();
    ~~~

## POSIX条件变量

+ 条件变量

  + 当一个线程互斥的访问某一个变量的时候，他可能发现在其他线程改变状态之前，它什么也做不了
  + 例如一个线程访问队列时，发现队列为空，它只能等待，直到其他线程将一个节点添加到队列中，这种情况下就需要用到条件变量。

+ 条件变量函数

  + **pthread_cond_init()**
  + **pthread_cond_desgroy()**
  + **pthread_cond_wait()**
    + 对mutex进行解锁
    + 等待条件，知道有线程向他发起通知
    + 重新对mutex进行加锁
  + **pthread_cond_signal()**
    + 向第一个等待线程发起通知，若无等待线程，则通知信号忽略
  + **pthread_cond_broadcast()**
    + 向所有等待线程发起通知

+ 条件变量使用规范

  + 等待条件代码

    ~~~c++
    pthread_mutex_lock(&mutex);
    while(条件为假)
        pthread_cond_wait(cond,mutex);
    修改条件;
    pthread_mutex_unlock(&mutex);
    ~~~

  + 给条件发送信号代码

    ~~~c++
    pthread_mutex_lock(&mutex);
    设置条件为真;
    pthread_cond_signal(cond);
    pthread_mutex_unlock(&mutex); 
    ~~~

## 线程池实现

+ 线程池介绍
  + 用于执行大量相对短暂的任务
    + 计算密集型任务：线程个数 = CPU个数
    +  IO密集型任务：线程个数 > CPU任务
  + 当任务增加的时候能够动态的增加线程池中线程的数量直到达到一个阀值
  + 当任务执行完毕的时候，能够动态的销毁线程池中的线程
  + 该线程池的实现本质上也是生产者与消费者模型的应用，生产者线程向任务队列中添加任务，一旦有任务到来，如果有等待线程，就唤醒来执行任务，如果没有等待线程并且线程数没有达到阀值，就创建新线程来执行任务。
