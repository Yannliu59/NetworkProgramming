cmake_minimum_required(VERSION 3.22)
project(NetworkProgramming)

set(CMAKE_CXX_STANDARD 14)

add_executable(echosrv socket/echosrv.cpp)
add_executable(echosrv2 socket/echosrv2.cpp)
add_executable(echocli_1 socket/echocli.cpp)
add_executable(echocli_2 socket/echocli.cpp)
add_executable(p2pcli socket/p2pcli.cpp)
add_executable(p2psrv socket/p2psrv.cpp)
add_executable(echosev3 socket/echosrv3.cpp)
add_executable(getiplist socket/getiplist.cpp)

add_executable(echocli4 socket/echocli4.cpp)
add_executable(echosrv4 socket/echosrv4.cpp)

add_executable(echosrv5 socket/echosrv5.cpp)
add_executable(echocli5 socket/echocli5.cpp)

add_executable(echocli6 socket/echocli6.cpp)
add_executable(echosrv6 socket/echosrv6.cpp)

add_executable(echocli7 socket/echocli7.cpp)
add_executable(echosrv7 socket/echosrv7.cpp)

add_executable(pollcli socket/pollcli.cpp)
add_executable(pollsrv socket/pollsrv.cpp)

add_executable(conntest socket/conntest.cpp)
add_executable(epollsrv socket/epollsrv.cpp)
add_executable(epollcli socket/epollcli.cpp)

add_executable(u_echosrv socket/u_echosrv.cpp)
add_executable(u_echocli socket/u_echocli.cpp)

add_executable(UdpChatCli udpchat/UdpChatCli.cpp)
add_executable(UdpChatSrv udpchat/UdpChatSrv.cpp)

add_executable(test udpchat/test.cpp)

add_executable(unixsrv unix/unixsrv.cpp)
add_executable(unixcli unix/unixcli.cpp)

add_executable(sockpair unix/sockpair.cpp)
add_executable(sockpair02 unix/sockpair02.cpp)

add_executable(msg_stat msgque/msg_stat.cpp)
add_executable(msg_send msgque/msg_send.cpp)
add_executable(msg_get msgque/msg_get.cpp)
add_executable(msg_rcv msgque/msg_rcv.cpp)
add_executable(msecho_cli msgque/msecho_cli.cpp)
add_executable(msecho_srv msgque/msecho_srv.cpp)

add_executable(mmap_write mm/mmap_write.cpp)
add_executable(mmap_read mm/mmap_read.cpp)
add_executable(shmwrite mm/shmwrite.cpp)
add_executable(shmread mm/shmread.cpp)

add_executable(sem semaphore/sem.cpp)
add_executable(semtool semaphore/semtool.cpp)
add_executable(print semaphore/print.cpp)
add_executable(dinner semaphore/dinner.cpp)

set(shmfifo_src
        shmfifo/shmfifo.cpp
        shmfifo/shmfifo.h)
add_library(shmfifo ${shmfifo_src})

add_executable(shmfifo_recv shmfifo/shmfifo_recv.cpp)
add_executable(shmfifo_send shmfifo/shmfifo_send.cpp)
add_executable(shmfifo_destroy shmfifo/shmfifo_destroy.cpp)
target_link_libraries(shmfifo_recv shmfifo)
target_link_libraries(shmfifo_send shmfifo)
target_link_libraries(shmfifo_destroy shmfifo)

add_executable(mq_open posix_mq/mq_open.cpp)
add_executable(mq_unlink posix_mq/mq_unlink.cpp)
add_executable(mq_setattr posix_mq/mq_setattr.cpp)
add_executable(mq_getattr posix_mq/mq_getattr.cpp)
add_executable(mq_send posix_mq/mq_send.cpp)
add_executable(mq_recv posix_mq/mq_recv.cpp)
add_executable(mq_notify posix_mq/mq_notify.cpp)

add_executable(shm_open posix_shm/shm_open.cpp)
add_executable(shm_unlink posix_shm/shm_unlink.cpp)
add_executable(shm_write posix_shm/shm_write.cpp)
add_executable(shm_read posix_shm/shm_read.cpp)

add_executable(01thread pthread/01thread.cpp)
add_executable(02thread pthread/02thread.cpp)
add_executable(p_echosrv pthread/p_echosrv.cpp)
add_executable(p_echocli pthread/p_echocli.cpp)
add_executable(tsd pthread/tsd.c)

add_executable(01test posix_sem/01test.cpp)
add_executable(02test posix_sem/02test.cpp)

add_executable(condition threadpool/condition.cpp)
add_executable(threadpool threadpool/threadpool.cpp)
add_executable(threadtest threadpool/threadtest.cpp)