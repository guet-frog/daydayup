
#if 杂散

// (1) web测试时 网页打不开 ping一下 确定网络OK

// (2) 如何查询本机上网网卡的ip mac地址

// (3) 网站可以认为是多个网页(多个HTML文件)组成的一种服务

// (4) 网页由前端使用HTML语言编写的文件，包含文字、图片、超链接、声音、视频 \
		前端负责显示网页内容，呈现动画效果，后端数据交互
		
// (5) 一般浏览器不推荐使用IE; 火狐浏览器 -- 审查元素; 谷歌浏览器 -- 检查

// (6) 你会div+css吗？ -- 你会写网页吗？ -- 不准确，网页要使用到head标签

#endif


#if 标签分类与常见的标签

	1. HTML标签分为单标签和双标签
		单标签: <标签名 />			// <br />
		双标签: <标签名> </标签名>  // <h1></h1>

	2. 常见标签
	 (1) 标题标签: h1-h6
	 (2) 段落标签: p
	 (3) 换行标签: br		// 单标签			-- <br /> 单标签规范写法
	 (4) 空格    : &nbsp;	// 以字节为单位  -- 字符实体  -- 多个空格通过样式实现
	 (5) 按钮	 : button
	 
	 
	 (5) 图片标签: <img src=""/>			// -- <img src="" alt="图片描述" />	-- alt属性的作用 图片链接失效时，爬虫识别图片，读屏软件用
	 
	 (6) 超链接标签: <a href=""> </a>
		// 鼠标放在超链接上，浏览器左下角有网址显示
		// href="#"
		
		表单域
		
		method属性在做接口测试时需要
		
		CSS如何与HTML建立关系：直接把CSS代码写在HTML文件中，用来修饰HTML	// 设置标签样式
		
		// html元素默认类型
		// 类名选择器命名规则
		// id选择器要唯一
		
		// 图形验证码 后端生成
		
		
		// web前端需要将psd设计稿(图片)，转化成HTML格式文件
		// 网页类似于一个九宫格布局，由各个模块组成
		// 分模块来写，代码实现分模块的过程 -- 网页布局
		
		// 不要在段落标签中包裹标题
		
		// 元素展示类型 --  元素展示类型转换
		
		选择器权重 本质 不同选择器对同一个元素的样式的操作能力
		
		不管是内嵌还是外链 都是为了给元素设置样式  -- 核心思想 找到这个元素 添加相应属性
		
		谷歌浏览器  右键 检查 修改元素属性
		
		内嵌时 style是一个标签
		外链时 style不需要
		行内时 style是标签属性
		
		在CSS中写HTML有问题  <style> d --> 自动补全display not div<style>
		
		如果标签补全 不正常  很有可能标签写错位置了
		
		<style>
			<link>		// errors
		<style>
		
		<link rel="stylesheet" herf="./xxx.css">
#endif

#if html基础
	<body> 网页内容的容器 </body>
	
	<!-- 注释 -->
	网页上线时，通过自动化工具将注释，换行、空格去掉，方便网页传输
	
	学html即是学html标签用法
	
	<h1>标签带了一些样式(如：文字大小，样式，自动换行)
	head(标题) -- 搜索引擎
	
	&gt;	&lt;		// 实体字符
	
	html块
	div便签没有具体的语义(关键的地方用有语义的，大部分地方无语义用div -- div + css)，没有默认的样式，表示 块		// span -- 给一行某几个字添加样式
	// 作用给特定一块内容加样式
	
	搜索引擎爬网，尽量用语义化标签，网页规范，提高网站排名
	
	<a></a>		// 超链接 从一个文本跳转到另一个文本  -- 不写href a的样式不能出来
	用a标签将图片包起来
	
	<a href="#">新闻标题</a>		// # 链接到当前页面顶部
	新闻列表，每一条新闻消息后面对应一个新闻页面，由后端动态配置详细页
	
	<a href="">
	<a href="javascript:;">缺省值</a>		// 有a的样式，但是不做任何操作
	
	页面内跳转，使用id 或 name(会出现兼容性问题)
	<h1 id="标题0">标题一</h1>
	<a href="#标题0">标题一</a>
	
	<a href="www.baidu.com" target="_blank">百度一下</a>		// target="self"
	
	一般不用有序列表，用无序列表最多
	一般用于页面中做数据表格。新的页面布局不用列表，用盒子模型
	
	前端三大块
	html：负责页面的结构
	css：负责页面的表现
	JavaScript：负责页面的行为
	
	<table></table> 表格
	
	实际开发中，UI设计师将页面设计成一张图片，前端根据效果图制作
	
	UI设计师不标注尺寸，用Photoshop测量
	
	先删除默认样式，再添加自定义样式
	
	表单 <form></form>
	<input type="radio" name="gender"> 男		// -- 表单控件
	<input type="radio" name="gender" id="male"> <label for="male">男</label>	// 文字可以选中
	
	<input type="hidden" name="hid" value="1000">	// 在页面上存储临时值 -- 从数据库读出值，暂存到hid中，最后一起提交到数据库
	
	<iframe src="http://www.baidu.com"></iframe>	// html内嵌框架 -- 页面无刷新提交 -- javaQuery库封装
	
	CSS：cascading style sheets 层叠样式表
	css出现之前，有些html 标签是带有样式 -- html 标签只负责网页结构 内容，样式由css完成
	实际开发中，将标签中的样式重置，再设置新的样式
	
	样式 如何与 段落关联  -- 选择器
	
	标签选择器(通用选择器) -- div 匹配页面上所有的div
	
	外链标签 <link></link>
	
	内嵌式(样式)通常用在首页，首页加载更快。
	其他页面使用外链式
	
	内联式 -- JS写效果的时候
	
	标签选择器 -- 相同类型标签 统一设置 -- 一般结合层级选择器使用  .box span{color:red}
	星号选择器 -- 把所有标签的间距设置为0 *{margin:0;padding:0}  -- 对性能有影响，实际产品一般不使用
	
	id选择器 -- 实际产品少用(页面中元素(标签)的id不允许重复，使用面太小) -- 权重更大
	元素的id是唯一的(一个页面)，否则通过id找元素出问题
	id选择器一般给程序用，让程序抓取元素
	
	类选择器 -- 开发常用
	一个类选择器可以作用多个元素(标签)
	一个元素使用多个类选择器 -- 类与类之间用空格隔开
	
	层级选择器 -- 一般不要超过4层 -- 解析越慢
	
	组选择器
	
	伪类选择器hover -- 低版本IE浏览器支持a标签使用hover
	a.hover = {color:red}
	
	伪元素选择器 -- css兼容用，通过样式添加内容
	
	.clearfix:before{		// 解决margin-top塌陷问题 -- margin两个坑
		content:"";
		display:table;		// 类似于给盒子加一个边框
	}
	把这个伪类放在公共的样式里，有塌陷的地方使用
	
	div -- 标签 元素 盒子
	padding属于盒子里面的东西 -- 可以撑开元素的宽和高
	
	margin上下会自动合并 -- w3c设定，上下段落之间的间距固定
	margin-top塌陷 -- 里面margin-top影响到外面盒子的margin-top	// 其他方向没有
	
	margin:50px auto 0;		// 0可以不写单位px
	margin是盒子外东西，padding是盒子里面东西
	
	块元素(不能在一行)
	内联元素(不支持宽高) -- 写内联元素，先写一个容器(用一个div来写 -- body也是一个容器)
	
	浮动 -- 转换成行列块
	<div style="clear:both"></div>		// html决定网页结构，放一个空的div，增加无用的结构
	.clearfix:after{
		content:"";
		display:table;
		clear:both;
	}
	通过after在盒子的最后插入空字符串，然后设置为clear:both
	原理相同，只是通过样式塞进去，不需要写结构
	
	特征布局：翻页
	简单方法使用a便签+span标签 -- su会认识这是一个标签 -- ul li		// 做su优化
	最终文字是从数据库中读出
	
	块元素居中 -- margin:0px auto
	文字左右居中 -- 父级加 text-align:center  -- 多个元素水平居中
	文件上下居中 -- 文字没有加padding时 line-height:40px   文字加了padding 使用margin 或 padding撑开
	
	设置浮动，比较方便调居中，通过margin
	
	图片有两种：装饰性 -- 背景来实现  内容 -- 插入图片来实现		-- 爬网解析
	
	定位 -- 设置父级为relative 子级为absolute 通过开发者工具调整left top值
	
	
	开发者工具 -- 左边是元素，右边 标签对应的样式(点击右边样式文件 -- 样式可能被压缩成一行 -- 点击{})
#endif

#if php

	php压缩图片的两种方式 -- 另存为，另存为web
	
	图片放大，本来2个像素，第三个像素由前两个相加求平均值得出
	
	flash主要用于做网页的动画特效 -- javascript
	性能不足
	浏览器需要额外插件
	flash不支持搜索引擎 -- flash里面可以放图片、文字(但是打包成一个整体)，但是搜索引擎不能搜到里面
	
	php cs6版本
	
	72 像素/英寸
	
	RGB三种光 -- 光 -- 增色原理，颜色越多越亮
	CMYK颜色 -- 打印机四种墨盒颜色
	
	把多张图(装饰性图)合成一张图，css设定使用图片不同的位置 -- 节省http开销
	公司logo图 属于 内容
	
	切图 -> 做页面

	首页 -- index.html  --  常用html1.0
	
	reset.css		// 将一些元素的默认样式给取消 -- 淘宝版 Facebook版 (没有用到的标签，影响到性能)
	index.css or main.css
	
	登录成功 -- 通过JS 动态添加 行间样式 <style="display=block"> -- JS从后台数据库读取用户是否登录
	用margin将logo撑下来，没有产生塌陷 -- logo设置了左浮动

	在页面中调整 背景图的 left top值
	
	搜索框如果需要用到form提交 -- 有刷新的提交(整个页面刷新) -- 用ajax提交
	
	线可以用border-bottom画
	
	先写结构，再实现结构中使用的样式 -- 大的尺寸到细节 -- {width:1200px; height:340px; margin:18px auto 0; 只关心距离上面的margin，水平居中，下面margin 0}
	先把容器画出来
	.center_con{
		width:1200px;
		height:270px;
		background-color:green;
		margin:0 auto;
	}
	再处理容器里面的li
	.center_con li{
		width:198px;
		height:44px;
		border:1px solid #eee;
		background:url(images/icons.png) 166px -385px no-repeat;	// 图片偏移
	}
	再处理li里面的字(链接)
	.center_con li a{
		display:block;		// 设置为和框一样大
		width:198px;
		height:44px;
		font:14px/44px "microsoft Yahei"
		text-indent:71px		// 文字的偏移采用首行缩进
	}
#endif

#if H5 + CSS3
	
	用一种类型元素的开发中常用
	
	E:nth-child(n){ ... }	// 数的时候不认类型，匹配的时候认类型，类型不一致不起效果
	E:nth-of-type(n){}		// 数的时候认类型，匹配的时候也认类型

#endif




 ·