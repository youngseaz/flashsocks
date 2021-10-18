# Linux网络优化



## 内核参数调优


修改 ```/etc/sysctl.conf```，做如下配置
```
#本地端口使用范围
net.ipv4.ip_local_port_range = 1024 65535
#内核网络接收缓存大小
net.core.rmem_max=16777216
#内核网络发送缓存大小
net.core.wmem_max=16777216
#TCP接收缓存,最小值,默认值,最大值
net.ipv4.tcp_rmem=4096 87380 16777216
#TCP发送缓存,最小值,默认值,最大值
net.ipv4.tcp_wmem=4096 65536 16777216
#对于本端断开的socket连接,TCP保持在FIN­WAIT­2状态的时间(秒)。对方可能会断开连接或一直不结束连接或不可预料的进程死亡。
net.ipv4.tcp_fin_timeout = 10
#能够更快地回收TIME­WAIT套接字。
net.ipv4.tcp_tw_recycle = 1
#TCP时间戳(会在TCP包头增加12个字节),以一种比重发超时更精确的方法(参考RFC 1323)来启用对RTT 的计算,为实现更好的性能应该启用这个选项。
net.ipv4.tcp_timestamps = 0
#启用RFC 1323定义的window scaling,要支持超过64KB的TCP窗口,必须启用该值(1表示启用),TCP窗口最大至1GB,TCP连接双方都启用时才生效。
net.ipv4.tcp_window_scaling = 0
#启用有选择的应答(1表示启用),通过有选择地应答乱序接收到的报文来提高性能,让发送者只发送丢失的报文段,(对于广域网通信来说)这个选项应该启用,但是会增加对CPU的占用。
net.ipv4.tcp_sack = 0
#在每个网络接口接收数据包的速率比内核处理这些包的速率快时,允许送到队列的数据包的最大数目。
net.core.netdev_max_backlog = 30000
net.ipv4.tcp_no_metrics_save=1
#定义了系统中每一个端口最大的监听队列的长度,这是个全局的参数。
net.core.somaxconn = 262144
#表示是否打开TCP同步标签(syncookie),内核必须打开了CONFIG_SYN_COOKIES项进行编译,同步标签可以防止一个套接字在有过多试图连接到达时引起过载。
net.ipv4.tcp_syncookies = 0
#系统所能处理不属于任何进程的 socket数量
net.ipv4.tcp_max_orphans = 262144
#对于还未获得对方确认的连接请求,可保存在队列中的最大数目。如果服务器经常出现过载,可以尝试增加这个数字。
net.ipv4.tcp_max_syn_backlog = 262144
#SNY/ACK重试次数
net.ipv4.tcp_synack_retries = 2
#SYN重试次数
net.ipv4.tcp_syn_retries = 2
```

[【linux】 网络内核参数优化](https://www.jianshu.com/p/e809731e0ff7)