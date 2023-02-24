# my_log
  
### 运行步骤：    
1.克隆仓库到本地:      
git clone git@github.com:zhengzhe2/my_log.git   
2.安装zlib依赖库：   
sudo apt-get install zlib1g-dev    
3.在xt_log目录下创建build目录:        
mkdir build        
4.切换到build目录下:      
cd build          
5.执行cmake:        
cmake ..       
6.执行make:      
make        
7.切换到xt_log目录下,执行setenv.sh脚本，拷贝共享库到/usr/lib/下,并执行测试程序：   
source setenv.sh

