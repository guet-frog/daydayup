
#if chapter4
	Git版本库不仅提供版本库中所有文件的完整副本，还提供版本库本身的副本
	
	Git维护的数据结构：对象库(object store)和索引(index)		// 对象库、索引
	对象库：包含原始数据文件和所有日志信息、作者信息、日期，已经其他重建项目任意版本或分支的信息
			对象类型：块(blob)、目录树(tree)、提交(commit)、标签(tag)
	
	blob：文件的每一个版本表示为一个blob。blob保存一个文件的数据，不包含该文件元数据(文件名)
	tree：一个目录树对象代表一层目录信息，可以递归引用其他目录树或子树对象，从而建立一个包含文件和子目录的完整层次结构
	commit：每一个提交对象指向一个目录树对象，该tree捕获提交时版本库的状态
	tag：标签对象分配名字给 特定的 对象(通常为一个提交对象)
	
	对象库中的每个对象都有一个唯一名称，对象内容使用SHA1得到的SHA1散列值
	SHA1值改变，文件的新版本被单独编入索引
	
	目录树可以建立一个包含文件和子目录的完整层次结构
	每一个提交对象指向一个目录树对象，目录树对象捕获 提交时 版本库的状态
									  索引捕获项目在某个时刻整体结构的一个版本
	一个提交对象指向一个特定的树对象，并且这个树对象是由提交对象引入版本库
											
	索引是暂时的信息，对版本库来说是私有的
	索引捕获项目在某个时刻的整体结构的一个版本
	项目的状态可以用一个 提交 和 一颗目录树表示，可以是项目历史任意时刻，或正在开发的未来状态
	通过git命令在索引中暂存(stage)变更，索引记录和保存变更直到准备提交
	
	git通过tree来跟踪文件路径名，git add时会将添加的每个文件 的内容 创建一个对象(不会为subdir创建对象)，并不会为tree立刻创建对象
	此时索引更新.git/index，// 跟踪文件的路径名和相应的blob -- 需要覆盖所有文件
	索引要记录所有的文件，相同的对象也会有一条索引 -- 树对象不在索引中，文件的路径名和相应的blob
	
	// 索引 和 顶层提交树 的区别
	$ git ls-files -s
	100644 3b18e512dba79e4c8300dd08aeb37f8e728b8dad 0       hello.txt
	100644 28858638e7af9c85f07a4e0340ea0556dc4ef993 0       log.txt
	100644 3b18e512dba79e4c8300dd08aeb37f8e728b8dad 0       subdir/hello.txt
	100644 3b18e512dba79e4c8300dd08aeb37f8e728b8dad 0       subdir/hello1.txt
	100644 3b18e512dba79e4c8300dd08aeb37f8e728b8dad 0       subdir/hello2.txt
	
	$ git cat-file -p 226b7
	100644 blob 3b18e512dba79e4c8300dd08aeb37f8e728b8dad    hello.txt
	100644 blob 28858638e7af9c85f07a4e0340ea0556dc4ef993    log.txt
	040000 tree fa691c8b83dd1174490c3724d809543f4f492b4c    subdir
	
	// 子树对象
	$ git cat-file -p fa691
	100644 blob 3b18e512dba79e4c8300dd08aeb37f8e728b8dad    hello.txt
	100644 blob 3b18e512dba79e4c8300dd08aeb37f8e728b8dad    hello1.txt
	100644 blob 3b18e512dba79e4c8300dd08aeb37f8e728b8dad    hello2.txt

	
	可寻址内容名称
	对象库中的每个对象都有一个唯一的名称，SHA1		// 基于对象内容 -- 数据 not 文件名
	git使用一个 文件的全部内容 的散列值作为文件名	// git为hello.txt创建一个对象的时候，只关心文件内容
	
	git需要维护一个明确的 文件列表 来组成版本库内容，但是并不基于 文件名
	git把 文件名 视为一段区别于 文件内容 的数据 --> git把 索引 从传统数据库的 数据 中分离出来
	
	新提交对象添加了一个关联的树对象来表示目录和文件结构的总状态	// 只添加新文件，只改变文件内容，树的SHA1是否变化 -- 索引和树应该都会变
	
	git ls-files -s 		// 查看索引
	git cat-file -p 68aba6	// 查看blob
	
	git write-tree 生成子树 和 顶层树		// 有新树，则创建对象
	
	每次对相同的索引计算一个树对象，得到的散列值仍是完全一样
	查看索引 只有文件内容列表 没有目录信息
	
	hello.txt		// 两个对象
	hello.txt dir1/hello.txt	// 3个对象  3b-hello.txt 68-tree被复用
	
	git tag -m "Tag version 1.0" V1.0 3ede462
#endif

#if chapter5
	git status 		// git ls-files -s
	
	git将所有文件分成3类：tracked、ignored、untracked
	
	git commit --all		-- 递归遍历整个 版本库(已追踪的文件)，暂存所有修改文件，然后提交
	
	git可以从 索引 或者 同时 从索引和工作目录 中删除一个文件
	
	git rm --cached tmp.txt		-- 文件由已暂存转化成未暂存的(删除索引内容？？？) // 修改索引，warning
	删除文件前，会检查工作目录文件的版本与当前分支中的最新版本是匹配的，防止文件的修改意外丢失
	
	添加到索引没有提交的文件，git rm --cached后，是否可以找回 -- 此时版本库已有该blob
	
	git rm 			-- 同时删除 索引 和 工作目录文件
	
	如何管理 索引 和 文件
	
	文件已经暂存了，文件存储在对象库中，索引只是指向它而已
	
	从工作目录和索引中删除一个文件，并不会删除该文件在版本库中的历史记录
	文件的任何版本，只要提交到版本库的历史记录的一部分，就会留在对象库里并保存历史记录 -- git自动清理机制
	
	(1) mv file newfile
	(2) git rm file				--> git mv file newfile
	(3) git add newfile
	
	git log file
	git log --follow file 		// 查看文件的全部历史记录
	
	.gitignore文件				// 子目录.gitignore !driver.o
	.git/info/exclude
	
	工作目录和对象库各有一份拷贝
	树和索引都指向文件的拷贝
	
	// - file1文件从编辑到在索引中暂存，最终提交
		(1) 工作目录、索引以及对象库都是同步、一致的
			
		(2) 编辑file1，工作目录时是脏的，索引和对象库没有变化
			
		(3) git add file1，重新计算file1的SHA1，①更新对象库 ②更新索引(file1路径名对应的索引值)
			工作目录和索引是一致的
			参考HEAD，索引是脏的，索引的树 与 master分支的HEAD提交的树 在 对象库 中不一致
			
		(4) 当所有变更都暂存到索引，一个提交将它们应用的版本库中
			a，虚拟树对象(索引) 转换成一个真实的树对象，计算SHA1，加入对象库中
			b，用日志消息创建一个 新的提交对象（指向树对象以及父提交）
			c，master分支的引用 从最近一次提交移动到新创建的提交对象，成为新的master HEAD
			
		(5) 工作目录、索引和对象库(由master分支的HEAD表示)再次同步
#endif

#if chapter6
	引用(ref)是一个SHA1散列值，一般指向提交对象
	
	本地特性分支名称		refs/heads/ref
	远程跟踪分支名称		refs/remotes/ref
	标签名					refs/tags/ref			//  标签对象
	
	// "特性"仅指每个分支在版本库中有特定的目的
	
	HEAD始终指向 当前分支 的 最近提交
	
	git log -5 --pretty=oneline HEAD
	git log -5 --pretty=oneline master
	git log -5 --pretty=oneline master^
	
	git log -1 -p
	git show -1
	
	某一个提交存在多个父提交，是由合并操作产生
	
	通过 引用 与 ^和~ 组合，可以从引用的提交历史图中选出任意提交
	master
	master^1		// 多个父提交的第一个
	master~2		// 祖父提交
	master~3
	
	git show-branch --more=10 | tail=3
	
	git rev-parse master		// 将 任何形式 的提交名转化为散列ID
	
	git Y		// 显示所有
	git ^X Y	// 区间显示
	git log master~7..master~5
	用Y之前的所有提交减去X之前的所有提交且包括X
	从概念和数据流方面来讲，可达提交集就是流入或贡献给定开始提交的祖先提交的集合
#endif

#if chapter7
/* - 7.1 使用分支的原因 */

/* - 7.4 创建分支 */
	git branch xxx
						// 命令只是把分支名引进版本库。并没有改变工作目录去使用新的分支
	git branch V1.0		// 指定特定版本
	
/* - 7.6 查看分支 */
	git branch默认值列出版本库中特性分支	// -r只显示远程追踪分支，-a显示所有分支
	
	git show-branch
	git show-branch master
	git show-branch prs/ *
	
/* - 7.7 检出分支 */
	git checkout		// 工作目录一次只能反映一个分支
	
	git show dev:new_stuff		// 查看其他分支文件
	
	假如你希望变更作用于新的dev分支上		-- stash					// 在错误的分支上开发
	git checkout -m dev		// 本地修改 和 目标分支 进行一次合并操作
	
	detached HEAD		// 分离的HEAD
	在该点用新的提交保留，首先创建一个新分支
	git checkout -b new_branch		// 创建并切换
	
	git branch -d bug/pr-3
	
	分支只是简单的名称或指向有实际内容的提交的指针
	Git防止不合并到当前分支的分支被删除，不小心丢失内容
	
	git reflog 恢复意外删除的分支
#endif

#if chapter8	/* --- */
/* - 8.1 git diff命令格式 */
	git diff					// 工作目录 和 索引
	git diff commit				// 工作目录 和 提交 -- 默认HEAD
	git diff --cached commit 	// 索引的变更 和 提交 -- 默认HEAD
	git diff commit1 commit2	// 任意两个提交
	
	// git diff, git diff HEAD, git diff --cached
	
	-w	// 忽略空白字符
	
/* - 8.3 git diff和提交范围 */
	git diff master ..pr-1
	git log master ..pr-1
	
/* - 8.4 git diff的路径限制 */
	git diff master~5 master xxx.txt --stat
	
	git diff -S"AAA" master~5		// 给定字符串在哪里引入或删除的

	SVN如何避免update时发送文件的全部内容
	SVN版本库中有大量diff文件
	
	Git每一个修订版本有一个tree，可以直接操作两个版本完整状态的快照，可以检索和生成任意两个版本之间的差异
	// Git如何 存储 和 恢复 每个版本的快照
#endif

#if chapter9
/* - 9.1 合并的例子 */
	一般情况下，一次合并只结合两个分支
	合并必须发生在一个版本库中			// 引用其他版本库，下载远程分支到当前工作目录
	
	Git把冲突的修改 在索引中 标记为为合并(unmerged)，解决所有冲突后做一次最终提交
	合并冲突时首先git diff
	
	cat > file
	
	git checkout master
	git merge other_branch		// merge made by the 'recursive' strategy
	
	git log --graph --pretty=oneline --abbrev-commit

/* - 9.2 处理合并冲突 */
	git ls-files -u
	
	git diff HEAD				// git diff --ours
	git diff MERGE_HEAD			// git diff --theirs
	
	git diff --base
	
	git log --merge --left-right -p hello.txt
#endif


