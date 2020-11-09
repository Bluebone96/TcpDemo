编译demo 

sudo ./build_all.sh

可执行文件在 ./Bin目录

demo 仍然存在bug，client 发送数据后， server epoll_wait 返回，但是一直无法读到数据

testsocket 测试 发现 TcpSocket 类 中 封装 read 函数的 recvdatabuf 有问题，但找不到
read 返回 0， 但 errno == 11，即 EAGAIN，Resource temporarily unavailable，应该是返回-1 才对


 