&ensp;&ensp;&ensp;&ensp;一个跨平台的lua游戏服务器开发框架, 该框架代码采用C++编写, 开发者只需要调用SDK提供的接口函数和绑定对应的回调函数即可。框架在逻辑层表现为单线程的开发模式, 使开发者更加易使用, 易调试, 易维护, 易扩展, 同时确保有快速的响应能力。

&ensp;&ensp;&ensp;&ensp;框架使用面向对象的开发模式, 去除协程, RPC远程调用, 更贴近于传统的开发模式, 适合C++, Java程序员在短时间内转为lua开发。开发者可以利用现成代码模板, 快速搭建类似bigworld引擎的多进程MMORPG集群架构, 也可以轻松来搭建其他游戏类型的集群架构。

作者:	galen  
&ensp;QQ: 88104725  
	
# **一、构建**
	
## 1. 在windows 10上的构建: 
		
	VS.net2019 打开Octopus_win32.sln, 按F7编译即可
		
## 2. 在centos7.9上的构建:
		
2.1 安装依赖库

	yum install wget libevent-devel ncurses-devel python readline-devel
			
	yum install zlib-devel openssl-devel cyrus-sasl-devel


2.2 编译程序
		
	在centos上用makefile脚本生成

	cd sh
	chmod -R 750 *
	./automake
	./makeDebug.sh
	./makeRelease.sh	

# **二、安装数据库软件**
	
## 1. MySQL 5.7.28

## 2. mongoDB 4.0.0
	  
		
# **三、执行第一个Octopus程序**

## 1. windows 10

	cd test
	hello_world.bat
			
## 2. centos 7.6

	cd test
	./hello_world.sh


# **四、重点文档:**

	1. doc\组件层次图.vsd
	2. doc\Octopus SDK.docx
	3. doc\源码目录结构.txt
	4. doc\开发环境编译与布署\*.txt

# **五、AOI测试图:**

![演示.png](https://www.cppbuild.cn:8001/i/2025/02/24/67bbcfb5c4284.png)