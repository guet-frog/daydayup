
// 本地Git仓库与GitHub仓库关联
(1) ssh-keygen -t rsa -C "xxx@mail.com"		// vi ~/.ssh/id_rsa.pub
(2) Setting -> SSH keys
(3) git clone git@github.com:clouder-frog/daydayup.git

// 虚拟机上网
(1) vi /etc/network/interfaces
  -- auto eth0
  -- iface eth0 inet dhcp
  
// https://jingyan.baidu.com/article/ce436649375b0a3773afd319.html

// vim设置注释颜色
(1) vim配置文件 /etc/vim/vimrc
(2) hi comment ctermfg=2

// source insight设置
1, x86 Assemble 			-- *.s;*.S
2, MAKE File 				-- *makefile
3, Add Type -> Kconfig		-- *kconfig;kconfig*

// include when adding to projects