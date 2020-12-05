# mydocker
docker cover

工程说明：
visual studio 工程
X86配置使用wsl中的armgcc，部署到实际嵌入式ARM环境
X64配置使用wsl中的gcc，方便调试

项目目的：
记录其他进程对docker的调用，便于分析其他进程的行为

过程：
mydocker重命名为docker替换原docker程序，原docker程序改名docker.cover
mydocker调用docker.cover，传递变参，并记录日志
