
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
	div标签没有具体的语义(关键的地方用有语义的，大部分地方无语义用div -- div + css)，没有默认的样式，表示 块		// span -- 给一行某几个字添加样式
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
	
	样式 如何与 标签关联  -- 选择器
	
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

	在页面中调整 背景图的 left top值1
	
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

#if JS
// ----------------------------------------------
	js插入到页面中有三种方法
	
	行间事件：标签有 行间事件属性	-- 该方式嵌入页面，主要用于事件  -- 一般行间调用的事件提取到javascript中调用，结构与行为分离
	<input type="button" name="" onclick="alert("AAAA");">		-- 行间事件属性onclick每个标签都有
	
	// 内嵌式 -- 使用script标签实现
	<script type="text/javascript">
		// 浏览器 加载完成后，执行匿名函数
		window.onload = function(){
			// 使用script中内置的document对象 -- 通过id获取元素
			document.getElementById('div1').tittle = "haha";		// 修改标签属性，修改样式属性
			
			oDiv.onclick = myalert();	// 立即调用
			oDiv.onclick = myalert;		// 点击调用
			
			oBtn01.onclick = change01;
			function change01(){
				oDiv.className = 'sty01';	// class="sty01" -- 改变样式
			}
		}
	</script>
	
	<script type="text/javascript" src="js/index"></script>
	
// ----------------------------------------------
	基本数据类型：number、string、boolean、undefined、null
	复合数据类型：object
	
// ----------------------------------------------
	获取页面的元素，可以对元素进行读、写操作
	window.onload = function(){
		var oDiv = document.getElementById('div1');
	}
	
	html、js的属性写法一致
	"class"属性写成"className"
	"style"属性里面的属性，驼峰式 -- style.fontSize
	
	var tmp = 'color'
	oA.style.color = 'red'
	oA.style[tmp] = 'red'
	
	innerHTML -- 读取或写入标签包着的内容
	
// ----------------------------------------------
	oBtn.onclick = function(){		// 如果这个函数其他地方不调用 -- 匿名函数
		alert('ok')
	}

// ----------------------------------------------
	if(oDiv.style.display == 'block'){ 	// oDiv.style.display执行时 去读取 行间样式(不是去样式类中去读取)，开始div无style属性
		oDiv.style.display = 'none';	// 虽然div.style.display = block，但是 读出值 = ''
	}
	else 	
	{
		oDiv.style.display = 'block';
	}
	
	<div class="box" id="box"></div>	// 原始
	<div class="box" id="box" style="display:block"></div>	// 第一次点击
	<div class="box" id="box" style="display:none"></div>	// 第二次点击
	
	return阻止默认行为，如：阻止表单默认行为 
	-- 点击submit，默认按照action地址提交数据。某些时候不需要表单提交，通过ajax提交
	-- 修改默认右键菜单
	
	// JS修改CSS样式，实现变化效果 --  001复习封闭函数-闭包30′  -- 程序执行流程
	aBtn[i].onclick = function(){
	
		for (var i = 0; i < aBtn.length; i++)
		{
			aBtn.className = '';
		}
		
		this.className = 'cur';
	}
	
// ----------------------------------------------
	数组常用方法
	var sTr = aRr.join("-");	// 1-2-3-4
	var sTr2 = aRr.join("");	// 1234

	aRr.push(5);
	aRr.pop();
	
	aRr.unshift(0)	// 从第一个添加
	aRr.shift()		// 从第一个删除
	
	aRr.reverse()
	
	aRr.indexOf()	// 第一次出现的索引值
	
	aRr.splice(2, 1, 'a')	// 从第二个开始，删除一个元素，追加一个元素'a'
	
	var aLi = document.getElementsByTagName("li");		// 通过标签获取元素
	var aLi = oList.getElementsByTagName("li");			// 限定某个ul范围
	
// ----------------------------------------------
	DOM 文档对象模型 操作html和css的方法
	BOM 浏览器对象模型 操作浏览器的一些方法 -- 语言在浏览器环境运行，操作浏览器的一些方法
	window.onload = function(){} -- 浏览器加载完成后，产生onload事件属性。调用浏览器onload事件属性完成一些操作
	window加载完成后，执行匿名函数
	alert() -- 浏览器行为 -- 弹窗（如果想统一各种浏览器的弹框，需要自己做 -- 弹窗一般用定位实现）
	弹窗 -- 浏览器行为，但是可以放在JS代码中使用
	
// ----------------------------------------------
	a = 12  b = '34'  a + b -- 1234
	
	str.split('')	// 字符串切割成数组

	str.charAt(0)	// 如判断某个选择器是 id选择器 还是 类选择器 '#div' '.div'
	
	str.indexOf()	// 查找字符串中是否包含某个 字符串
	
	str.substring()
	
	str.toUpperCase()
	str.toLowerCase()
	
// ----------------------------------------------
	g = NaN			// not a number
	if (g == NaN)		// false
	alert(NaN == NaN)	// false
	
	isNaN(g)
	
	程序调试方法 -- alert、console.log、document.tittle
	
	在原有系统添加新功能，防止覆盖掉原有功能 -- 封闭函数(新功能用封闭函数包起来)
	(function(){
		var calc = function(){
			var docElement = document.documentElements;		// 获取页面html标签
			
			docElement.style.fontSize = docElement.clientwidth;
		}
		calc();		// 刚开始执行一次
		window.addEventListener('resize', calc);	// 绑定一个监听，当窗口改变尺寸时(resize)，执行calc
		
	})();	// 定义封闭函数，(); -- 立即执行
	
	定义变量前，全局搜索判断有无重名
	
// ----------------------------------------------
	闭包：函数嵌套，子函数可以调用父函数的局部变量，局部变量不会被回收
	一般闭包赋值给全局变量，全局变量不会被回收  -- 闭包内部使用的局部变量也不会被回收

	页面关闭之后，全局变量被清空
	
	var aLi = document.getElementsByTagName('li');
	
	alert(aLi.length);
	
	document.referrer -- 登录完又跳回首页
	var backurl = document.referrer;
	window.location.href = backurl;
	
// -----------------------------------------------
	<div>欢迎<span id="span01"></span>访问我的主页</div>
	var oSpan = document.getElementById('span01');
	oSpan.innerHTML = '张三'
	
	函数末尾规范加;
	
	JS没有对象
	函数是一个类，有自己的属性
	
	选项卡 -- 闭包实现，jQuery实现
	
// --------------------------------------------------
	实际开发中，用jQuery或公司封装的JS库  -- 原生JS和CSS有兼容性问题
	
	后台开发一般用jQuery，考虑到与其他厂家后台对接
	
	压缩 -- 去掉程序空格 换行 代码混淆(简单变量)
	
	window.onload = function(){		// 页面渲染完成开始执行
		var oDiv = document.getElementById('div');
		alert(oDiv.innerHTML);
	}
	
	$(document).ready(function(){		// 页面节点加载完成开始执行 -- 开发常用
		var $div = $('#div');
		alert($div.html());
	})
	
	可以对选择器进行修饰(类似于伪类)，$('#ul1 li:first')	// 选择id为ul1元素下的第一个li
	
	$('.list li') 不能回到父级
	$('.list').children()  可以通过end()回到父级
	$('.list').children().css({background:'gold', height:'20px'}).end().css({background:'green'})
	
	siblings() -- 方便实现选项卡  -- (选择集转移 选择器常用)
	
	alert($('.div1').css('fontSize'));	// 不设置值 -- 读
	
	alert(this) -- object HTMLInputElement -- input元素
	如果需要再jQuery中使用JS元素 -- $(this)
	$(function(){
		$('#btns input').click(function(){
			// this是原生对象 -- jQuery对象 $(this)
			$(this).addClass('cur').siblings().removeClass('cur')
			
			// 需要通过按钮索引关联的选项卡
			// -- jQuery完成工作： 1.存索引值 -- 不用通过闭包存 2.选取剩余元素 -- 不用通过循环
			$('#contents div').eq($(this).index()).addClass('active').siblings().removeClass('active')  // -- 链式调用
			
			$(this).index()		// 获取当前按钮所在层级范围的索引值 -- 原生JS this没有index()方法			
		})
	})
	
	开发环境， 生产环境 -- 放到线上
	
// ----------------------------------------------
	jQuery操作属性
	class属性由 addClass、removeClass、toggleClass操作
	style属性由 css({fontSize:'30px', color:'red'})操作
	
	jQuery样式操作 -- 操作行间样式、操作样式类名
	
	alert($('.box').html())	-- 可以读可以写，也可以写结构(加入标签结构)
	$('.box').html('AAAA')
	
	alert($('.box').attr('class'))		// 读
	$('.box').attr({title:'BBBB'})		// 写
	
	alert($('#check').prop('checked'))	// 读
	$('#check').prop({checked:true})
	
	常用html() attr() prop()
	
// ----------------------------------------------
	层级菜单 -- 先写结构 -- 再写样式 -- 再写JS
	
// ----------------------------------------------
	jQuery运动框架 -- jQuery动画和JS函数相似，设置元素样式变化
	
	<div id="div1" class="box"></div>		// 程序使用id，class给样式用 -- 前端维护样式，只修改class
	
	动画曲线 -- include jQuery UI
	jQuery特殊效果（淡入、淡出、向上卷起）是由动画实现
	
	$('.list li').each(function(a){		// 不管写什么变量，第一个字都是索引值 index
		$(this).html(a);
	})
	
	做效果前，先看结构（手风琴） -- li包img实现，浮动定位(一个定位到最左边，剩下定位到最右边，再用一个容器整体裁切)，右边超出部分拆切掉，设置left值可以实现展示效果
	
	$('#accordion li').click(function(){
		$(this).animate({left:$(this).index()*20});		// 第三个移动前两个也要相应移动
		
		$(this).prevAll().each(function(index){		// 前两个分别移动过去
			//alert(index);
			
			//$(this).animate({left:index*20});		// 动画执行慢，index提前被遍历
			
			$(this).animate({left:$(this).index()*20})
		});
	});
	
// ----------------------------------------------
	获取元素尺寸
	获取元素相对浏览器绝对位置  offset()   -- 左上角
	
	没有定位属性，left:'50%' top:0  -- 居中没有作用
	
	var pos = $('div').eq(2).offset();
	console.log(pos);
	console log返回一个对象 -- 类似与一个json格式
	
// ----------------------------------------------
	h5表单控件属性 autofocus -- 
	
	输入框相关：
	blur()		// 元素失去焦点
	focus()		// 元素获得焦点			-- 类似于H5表单控件属性 autofocus，不用点直接可以输入
	change()	// 表单元素值发生变化
	
	click()
	
	mouseenter()
	mouseleave()
	hover()
	
	keyup()		// input框快速交互 -- 判断用户名重名，判断输完通过ajax传到后台
	
	resize()
	
	submit()
	
// ----------------------------------------------
	绑定的事件触发时，系统会自动产生event事件对象，包含一些常用属性  -- console.log(event)
	
	事件委托 -- 提高性能
	利用事件冒泡，把事件加在父级，判断事件来源某个子集，执行相应操作
	减少事件绑定次数 -- 多个子元素
	让新加入的子元素也可以拥有相同操作
	
	// 对大量子元素进行操作使用事件委托
	$('.list').delegate('li', 'click', function(){
		alert($(this).html());
	})
	
// ----------------------------------------------
	创建节点 var $div = $('<div>')
	插入节点 $('#div1').append($span)
	删除节点 $p.remove()
	
// ----------------------------------------------
	整屏滚动
	画面切换时，移动的效果 -- CSS3动画实现
	底部切换按钮		   -- JS动画实现
	
	// 图片初始样式	
	<div class="main_con">
	<div class="left_img"><img src="./001.png"></div>
	</div>
	
	// 再塞一个moving进去 -- 通过js  -- 权重增加，盒子样式变了 -- 未修改的样式保持不变 3′
	<div class="moving">
		... 
	</div>
	
	// mousewheel 插件 -- 判断鼠标上滑还是下滑
	$(window).mousewheel(function(event, dat){})
	
	高频触发事件，使用函数截流 -- 定时器
	$(window).mousewheel(function(event, data){
		clearTimeout(timer);		// 200ms内连续触发，取消上一次，只保留最后一次
		timer = setTimeout(function(){		/* - setInterval 反复执行 */
			
		}, 200)
	})
	
	鼠标点击换页
	$points = $('.points li');			// 1. 选中li	
	
	$points.click(function(){			// 2. 绑定事件
										// 3. 首先 点 的样式改变$(this).addClass('active')
										// 4. 其他的 点 改变其他样式 $(this).siblings().removeClass('active')
		$(this).addClass('active').siblings().removeClass('active');
										// 5. 切换到当前屏 -- 当前屏 = li index
		$('.pages_con').animate({top:-$h*$(this).index}, 300)
										// 6. 相应动画
		$pages.eq($(this).index()).addClass('moving').siblings().removeClass('moving');
	})
// ----------------------------------------------
	var $sli = $('<li>')		// 创建  -- $sli = $('li')	选择
	
// ----------------------------------------------
	json(数据格式) 取代 比较冗余的xml
	
	JS需要与后台通信 --  js方便修改标签内容，向后台请求数据，塞到标签中 -- 实现页面局部刷新或无刷新
	没有ajax之前，数据通过后台语言打到页面上 -- 页面需要刷新数据，后台将页面打到前端，浏览器重新渲染 -- 全部刷新
	
	nodeJS类似Python，语法与js类似，跑在后端的js，属于前端的后台语言 -- server.js是由nodejs语言编写
	处理并发性能好
	node.js安装包，用node.js跑 server.js  -- 模拟服务器程序 -- 通过服务器程序跑页面
	
	$.ajax		// 方法绑定在 $ 上面
	
	让该文件夹，成为服务器目录
	
	一个接口返回一类数据，与后台约定
	
// ----------------------------------------------
	ajax 同域或父域到子域请求数据
	jsonp 跨域请求数据 -- 论坛使用qq登录 -- 数据共享方式
	跨域请求 -- <script type="text/javascript" src="../js/data.js"></script>	通过script标签引用其他js文件
	
	script、img等标签可以跨域
	
	www.bootcdn.com
	
	jsonp和ajax是两种东西
	ajax通过xml http对象请求数据，发送http请求
	jsonp通过script标签
	jQuery将两种功能都集成到一个函数 $.ajax
	
	jsonp -- 数据用括号包起来，加一个函数名，-- 接口要对上
	
	使用jsonp公开接口 -- 21′
	
	读公开jsonp接口数据，塞到页面中
	.done(function(data){
		for (var i = 0; i < data.s.length; i++)
		{
			var $li = $('<li>' + data.s[i] + '<li>');		/* - 创建节点，塞入元素 */
			
			$li.prependTo('.list');		/* - 插入节点 */
		}
	})
	
	jasonp接口动态获取 ajax传过去的函数名 然后把它塞到函数名旁边
	开发时有很多公共接口，使用公共接口数据，-- 使用jsonp原理
	

// ----------------------------------------------
	正则：匹配的规则 -- 针对字符串处理
	前端常用作表单验证
	
	var re2 = /a/ig;
	var str = 'abc'
	alert(re2.test(str))
	常用test()  作用字符串  -- 匹配到 第一个就退出  --  ^ & 以xx开头 以xx结尾
	
	
// ----------------------------------------------
	cookie在同源(同源)请求时携带，影响性能 -- 图片或css等静态资源，放在另一个域中，传输中不会携带cookie
	
	cookie插件依赖jQuery，先导入jQuery，再导入cookie JS文件
	
	检查 -> application -> storage		// cookie读写需要服务器环境下运行
	
	node server
	
	地址栏输入 localhost:8888 就开始找index文件 -- localhost:8888/001cookie.html -- 指定页面
	icon文件放在根目录下，浏览器自动放在tittle中
	
	// cookie读写
	$.cookie('mycookie', 'ok!', {expires:7, path:'/'});
	var val = $.cookie('mycookie');
	
	用途：如存用户名，自动填入到输入框中
		  点击我知道了，最近不再弹出
		  
	<div class="pop_con">
		<p class="hasknow">我知道了</p>
	</div>
	
	$(function(){
		
		var read = $.cookie('hasread');		// 读cookie
		
		if (read == undefined)
		{
			$(".pop_con").show();
		}
		
		$('.hasknow').click(function()		/* 通过样式给元素绑定一个事件 */	-- 一个样式对应多个元素???
		{
			$.cookie('hasread', 'ok', {expires:7, path:'/'});	// 保存cookie
			$('.pop_con').hide();	/* 通过样式找到元素，隐藏元素 */
		})
	})

	购物车商品存数据库中
	
	H5新增本地存储方式，不需要服务器环境，
	LocalStorage 最大存5M -- 类似cookie，但是没有日期限制
	sessionStorage 窗口(整体浏览器)关闭时清空 -- 安全
	
	cookie http传输时会携带传输，这两种不会
	
// ----------------------------------------------
	jQueryui
	$('.scroll_bar').draggable({		// 常用 drag
		axis:'x',
		containment:'parent',
		drag:function(ev, ui){
			console.log(ui.position.left);
			var nowleft = ui.position.left;
			$('.slide_count').val(parseInt(nowleft*100/570));
		}
	})
	
// ----------------------------------------------
	移动端将几种事件封装出三种操作 -- 点击、拖动、滑动（封装后的操作）
	点击 -- touchstart + touchend
	滑动 -- touchstart + touchmove
	
	zeptos -- jQuery太大了
	
#endif

#if django
#if 流程
	操作数据库 -- 数据库可能发生变更
	处理用户浏览器请求
	呈现处理结果 -- html模板(页面格式)
	
	django流程(MVT)：
	客户端请求 -> 视图View(本质是一个方法) -> 如果需要数据则取调用模型model
	-> 获取到数据，视图View做处理(逻辑判断) -> 处理完成后要呈现结果，去查找相应的模板
	-> 补全模板信息(html)，最终需要呈现的html内容准备完成 -> 由浏览器返回到客户端
	-> 客户端渲染html+css，执行JS
	
	模型当中不直接编写sql语句，可以编写但是设计目的是不直接使用sql语句
	模型中一个对象，对应表中的一行
	一个表对应一个对象的集合 -- python中列表实现表

	// ------------------------------------------
	创建（python开发，为了python开发）虚拟环境，以适应于不同版本开发	// ubuntu虚拟环境
	bin
	include
	lib			// 其他固定，
				// 默认安装到usr/local/dist-packages/ -- 第三方库安装到site-packages
	local
	share
	
// 查看当前环境安装的包
	pip list
	pip freeze 	// 省略 pip setuptools wheel
	
	往服务器部署时，需要将本地开发环境导出，通过pip freeze，然后直接安装
	
	pip install django==1.8.2		// 不确定包 -- 开发规定版本 或 pip.org查找 -- 通过pip安装
									// 不加版本号，默认安装最新版本
									// 非源码安装

	django-admin startproject test1		// 注意执行该命令时，需要有操作当前用户的权限 -- 注意目录权限
	ls test1	// 创建项目基本文件，如：配置文件
	-- manage.py
	 |
	 | -- test1	// 这个包不再添加新文件
		|
		| -- __init__.py	// 需要有这个文件，有无内容不重要，表示test1目录是一个包
			 setting.py		// 如：注册迁移时的APP; 单独配置数据库 -- sqlite3 -- 文档型数据库，物理上在当前项目创建一个文件路径即可
			 urls.py		// 配置用
			 wsgi.py		// 部署、发布用
			 
	 | -- 其他代码

// ----------------------------------------------
	django三个组成部分：模型、视图、模板
	模型 -- 通过模型类 完成和数据库交互
	
	图书表
	英雄表
		属性hbook -- 和图书表关系(图书-英雄 1对多)，直接存储图书对象 -- 数据库层面本质还是一个id
	
	设置环境：Project/settings/Project:projects/Project Interpreter
	
	python manage.py startapp booktest		// 开启应用booktest
	-- booktest
	 | -- admin.py		// 管理用
	 | -- __init__.py
	 | -- migrations		// 迁移，根据当前项目逻辑类生成数据库脚本，并将脚本映射到数据库中
	    | -- __init__.py
	 | -- models.py		// 用户定义模型类
	 | -- tests.py		// django自带的测试模块
	 | -- views.py		// 定义视图、函数
	
	// 管理模块admin、模型、视图，没有创建模板 -- 因为视图不确定，不能确定模板
	
	// models.py
	form django.db import models
	class Bookinfo(models.Mode):		// 非普通类，作为模型类，这个类创建的对象要操作数据库 -- 继承mode类，django中已经定义
		btittle=models.CharField(max_length=20)		// 定义类属性
	
	定义模型类的作用：
		根据定义生成sql语句并创建表		// 根据类属性新建字段，类属性的值(类型) 对应 数据库的类型
		创建对象，进行其他操作 -- 可以映射到数据库(不用写特定sql语句)，执行insert update delete
		-- 在django中不需要面向数据库具体写sql语句 -- 通过模型类
	
	// 运行一个项目
	python manage.py runserver 8080
	
	迁移操作：
	(0) 向应用中注册，编辑setting.py文件 -- INSTALL_APPS = ()	// 添加所有需要安装的应用 -- 注册到项目  -- 添加第三方包也需要注册
	(1) 先生成迁移，根据模型类生成 所选择数据库 特定的 sql语句 -- python manage.py makemigrations
		-- booktest/migrations/0001_initial.py	-- 用于成sql语句
	(2) 执行迁移
		根据生成的迁移文件，去数据库中执行特定的sql语句，创建表 -- python manage.py migrate
		
	迁移的目的，根据当前 模型类 生成 数据库表
	如果数据库以有，不用迁移重新生成表，配置DATABASES={}参数	
	
	python manage.py shell 	// 很多操作依赖manage.py
	
	>>> from booktest.models import *
	
	BookInfo.objects.all()		// 默认调用对象的__str__方法 返回一个列表	-- object指向一个manage对象
	object是类属性 -- BookInfo类 调用 类 属性
	b = BookInfo()
	b.btittle = 'aaa'	// 操作的不是类属性，-- 操作的是实例属性
	b.save()			// 通过实例属性 去对应 类属性 -- 进行sql语句映射
	
	只修改模型类方法，数据库不需要迁移，数据库 表中存的是数据(字段)，方法是对数据的处理(数据库不存储)方法不需要迁移
	需要重新开启shell 导入模型类
	
	MVC MVP模型，开发面向对象，内部转化成sql语句 -- BookInfo.objects.all()并没有执行select，产生执行结果
	
	模型开发完成(数据库表建好)，下一步向数据库中添加数据 -- django管理站点
	(1) 创建超级用户 python manage.py createsuperuser
	(2) 运行项目（后登录） python manage.py runserver		-- 默认使用8000端口
	(3) 进入管理登录页面 127.0.0.1:8000/admin
	(4) 在管理页面的group中添加相应的模型类 -- 将模型类注册到管理组中，定义模型类在models.py，注册到admin.py
	
	admin使用	// 编辑维护模型类
	在admin.py文件中注册 admin.site.register(模型类, admin类)		// admin site类中的register方法
	编辑管理时 列表页显示样式和添加修改页的显示样式，通过admin类参数设置
	在admin.site.register()注册之前，定义一个类继承model.modelAdmin类，定义相关属性设置显示效果
	
// ----------------------------------------------
	处理客户端信息 -- 视图
	
	请求http信息方式，响应http信息放在另一个对象
	
	from django.http import request, response		// django封装request信息，response信息需要构造
	
	def index(request):						// 默认至少接收一个参数request
		return HttpResponse('hello world')	// 必须返回HttpResponse对象，或者这个类的子类
		
	def index(request):
		temp = loader.get_template('index.html')
		return HttpResponse(temp.render())
	
	def index(request):
		return render(request,'index.html')
		
	视图定义好，如何请求 -- 需要配置route -- 配置url
	
	urls.py		// 除了url配置，其他配置在settings.py
	urlpatterns = [
		url(r'^admin/', include(admin.site.urls)),		// 可以请求admin的原因
	]
	地址栏去匹配url正则表达式，使用匹配上的视图(方法)去处理 -- 视图被执行
	
	完成视图，配置其url。一个视图与一个正则唯一匹配（一个请求过来只能一个正则去匹配，调用某一个视图）
	项目中一般由多个应用，一般不在根级目录配置url。一般在自己的应用中配置url
	
// ----------------------------------------------
	在工程目录下，创建模板目录templates，创建与视图名相同的模板
	
	需要再setting.py中配置模板路径 TEMPLATES 'DIRS' -- os.path.join(BASE_DIR, 'templates')	// 拼接目录
	
// ----------------------------------------------
	首先定义model类(可以现在shell下测试)，生成迁移(只做一次，目的生成表)。
	启用管理站点，在创建好的表中，维护数据 -- 后台管理
	编写视图，配置url为了视图被调用。视图被调用后，使用模型获取数据  -- 开发中有哪些视图 提前设计规定好，url已配置好，正则表达式规则已确定
	获取数据后，呈现数据，使用模板中定义好的html					 -- 开发前有哪些模板也已经确定
	用视图加载模板，并传递上下文context，完成解析，得出最终html
	通过response返回，客户端浏览器渲染执行
	
	url(r'^(\d+)$', views.show)		// 通过正则表达式 传参 到 视图方法，默认传参request
	
// ----------------------------------------------
	(1) 创建虚拟环境
	(2) 安装django
	(3) 创建项目		// 配置数据库、配置应用、配置模板
	(4) 创建应用
	
	(5) 在models.py中定义模型类		// 循环5-8
	(6) 定义视图	// 视图的要求								
	(7) 配置url
	(8) 创建模板
	
	开发完成，运行测试这个项目
	在开发阶段，django提供一个轻量级的服务器，python manage.py runserver
#endif

#if 模型
	字段说明
	
	对于关系如何进行相互访问
	BookInfo	// 一本书对应多个英雄，一对多的关系，关系应该存在多的那一端
				// 表中每一行数据，就是一个BookInfo对象
	
	HeroInfo
		book = models.ForeignKey(BookInfo)	// 保存book对象，本质存的是id，访问id两种方法
											// hero.book.id
											// hero.book_id		// 表中有book_id字段
											
	book.heroinfo_set	// 通过书对象找到英雄 -- 书中没有英雄属性
	hero.book			// 通过英雄对象找到对应的书

	任何一个模型类都有一个管理器，用于和数据库交互(模型类不能直接和数据库交互)
	如果不适用默认管理器，可以自定义管理器，可以达到更改查询集的效果 -- 重写管理器的一些方法
	
	books1 = model.Manager()	// 仍使用默认管理器 -- 没有objects管理器
	books2 = BookInfoManager()	// 重写了默认管理器的方法
								// 自定义管理器作用
	
	管理器对象实现ORM功能（映射操作：将对象的操作转换成数据库语句。将数据库查询返回的数据转化成对象，如果有多个对象，则以列表形式返回）
	
	MVC框架中的ORM部分完成数据库映射 -- django中ORM为管理器
	管理器需要知道 哪个模型类 和 哪个表 映射 -- 将管理器作为模型类的一个属性
	管理器：将模型类的对象 和 数据库中的数据 映射起来
	模型类中如果不自定义管理器，默认有一个objects管理器
	
	管理器是模型类的一个数据，用于将对象与数据变映射
	
// ----------------------------------------------
	模型类完成，数据库迁移完成，向数据库中增加、修改、删除数据 -- admin
	
// ----------------------------------------------
	filter(键1=值1)
	get()				// 拿一个
	count()				// 返回总条数
	
	惰性执行：创建查询集不会带来任何数据库的访问，直到调用数据时，才会访问数据 -- 迭代、序列化、与if合用
	print([e.tittle for e in Entry.objects.all()])	// 以取数据
	
	querylist = Entry.objects.all()			// 未取数据
	print([e.tittle for e in querylist])	// 取数据
	printt([e.tittle for e in querylist])	// 使用本地缓存数据，不用重新拿数据
	
#endif

#if 视图
	用户浏览器输入网址请求
	django获取地址信息，除去域名和端口部分
	匹配正则，调用方法，方法：接收request及正则中获取的值，处理并返回response对象
	// request类型HTTPRequest -- 该对象由django接收报文完成后构造
	
	url配置：
	-- projects
	 | -- test1
	 | -- test2
	    | -- manage.py		// os.environ.setdefault('test2.setting')
		| -- test2
	       | -- settings.py	// ROOT_URLCONF = 'test2.urls'
		   | -- urls.py		// urlpatterns = [] -- 这里包含每个应用的urls.py文件，每个应用的url放在单独应用中
		| -- booktest
		   | -- urls.py		// 创建
		   | 
	
	// 通过url如何找到视图， url参数
	
	cur_path = os.path.join(BASE_DIR, 'aaa')

// ----------------------------------------------
	request对象：
	使用表单，且明确指定method=post时为POST，其他默认为GET方式
	
	// 在views.py，视图方法中处理request传参
	a1 = request.GET.get('a')
	a1 = request.GET['a']
	
	a2 = request.GET.getlist('a')	// 一个键多个值
	
	GET请求方式的键值：../?a=1&b=2&c=3
	POST请求方式的键值：input控件的name属性作为键，value属性作为值，通过POST提交
	
	接收通过键来接收，控件中没有写name属性，则不会提交
	<input type="text" name="uname">		// 无value属性，提交报文中存在uname信息
	<input type="submit" value="提交">		// 同样是input，提交报文中无
	
	// ?后面键值对信息，url方式不能获取
	www.baidu.com/2020?a=1
	url(r'(\d{4})')		// 值写在地址里面可以接收 -- get方式不能获取

// ----------------------------------------------
	response对象
	
	cookie：存储在 浏览器中 的一段文本信息
			cookie由服务器设置，填充到response中，浏览器接收后，以键值方式存储，
			再次访问该网站，则携带相应cookie，添加到request head中 -- 跨域名不能共享该cookie信息
			指定时间的两种方式 -- 默认不设定两周过期
			
	httpresponse有两个子类：HttpResponseDirect重定向用，JsonResponse
	重定向，登录页面实现

// ----------------------------------------------
	状态保持
	session的过期时间设置
	session如何存储，如何修改
	服务器如何区分session
	
	cookie中携带session id，服务 器通过id区分不同用户		// session依赖cookie
	
#endif

#if 模板
	变量					// {{ var }}
	标签 {% 代码块 %}		// 执行代码段
	过滤器
	注释					// 单行注释、多行注释

	浏览器不能解析DTL，在发给浏览器之前django先将模板语言解析，形成最终HTML字符串

	def index(request):
		tmp = loader.get_template('index.html')
		context = RequestContext(request, {})
		return HttpResponse(tmp.render(context))

// ----------------------------------------------
	定义模板
	def index(request):
		hero = HeroInfo.objects.get(pk=1)		// hero = HeroInfo(hname='abc')
		context = {'hero':hero}
		
		list = HeroInfo.objects.all()	// list = HeroInfo.objects.filter(idDelete = True)
		context = {'list':list}
		
		return render(request, 'index.html', context)
	
	{% for hero in list %}
		{% if forloop.counter|divisbleby:"2" %}		// 过滤器：基于前面值 做运算
													// 过滤器相当于方法调用 val|func() 变量调用方法，需要参数 :"xxx"
			<li style="color:red">{{ forloop.counter }}:{{ hero.showname }}</li>
		{% else %}
		
		{% endif %}
	
	{% endfor %}
	
// ----------------------------------------------
	反向解析
	配置视图的url时
	redirect
	模板标签中	
	
	// 链接地址不能硬编码，根据url配置规则生成，维护简单
	<a href="{% url 'booktest:show' %}"></a>

// ----------------------------------------------
	CSRF
	需要CSRF打开，且自己的网站内部请求能访问到页面
	只有post请求需要用到CSRF验证 -- get也需要(get请求不需要csrf验证)
	需要csrf防御时，添加token标签
	{% csrf_token %}
	
	localhost/a.html
	
// ----------------------------------------------
	验证码
	return HttpResponse(value, 'image/png')		// 视图统一返回结果，浏览器通过MIME type来执行 -- text/html text/css text/js
	
	/* 页面1：首先加载表单页面，文本框 + img标签 */
	/* - <img src="verifyCode"> 图片标签也需要配接口 */
	request.session['code'] = value 	// 加载表单页面时，对应的视图要生成验证且保存验证码到session
	
	/* 页面2：<form action="verifyTest2" method="post"> */
	需要视图接收验证码，校验验证码
	code1 = request.POST['code1']
	code2 = request.session['code']		// 存 ，提交，取session对比
	
// ----------------------------------------------
	浏览器请求index页面，服务器将'booktest/index.html'转化成字符串发送给浏览器
	浏览器解析到img标签，会再次向服务器发送请求
	
	类似于反向解析，任何一个地址都不使用硬编码 -- 静态文件。可以隐藏实际目录
	
#endif

#endif

#if selenium
// ----------------------------------------------课程总体介绍
	自动化测试常用selenium、QTP
	loaderrunner性能测试
	
	selenium基础
	自动化需求分析
	自动化用例设计
	
	脚本开发
	
	脚本封装重构
	
	基础函数封装
	参数化
	
	PageObject
	
	日志的收集
	报告的生成
	
	TestNG使用
	
	持续集成
	
	Jenkins -- 做持续集成。创建工程 -> 点击立即构建 -> console output
	
	自动化基础：自动化用例编写、selenium优势及原理、自动化环境搭建
	
	selenium基础：	-- selenium基础教程
	常见8大元素定位(表格)，
	常见元素处理
	下拉框元素处理
	不同窗口切换（常用于自动化测试）
	元素的进阶（如果当前页面没有出现该元素，如何才能出现）
	元素的等待
	
	登录流程简单，购物流程复杂
	
	读取配置文件，数据驱动（TestNG）
	
	JenKins无界面环境下，防止不同浏览器的原因造成case执行失败。
	case执行失败有log日志，同时可以设置截图信息
	
	需求不断更新的，不适合做自动化测试
	
	如果系统为windows，不用selenium
	
	文字化自动化测试用例 -- 编码实现
	
	手工用例
	用例ID，模块，前提条件，步骤，预期结果，实际结果
	
// ----------------------------------------------自动化测试用例编写
	检查元素，查看元素id信息
	
// ----------------------------------------------selenium工作原理
	脚本创建driver，然后启动server，将浏览器绑定在特定的端口上
	脚本创建session发送请求，服务器响应请求，回复信息
	
// ----------------------------------------------
//	selenium基础知识回顾
// ----------------------------------------------
	// --------------------------------
	// IDE环境配置
	// --------------------------------
	jdk：JAVA_HOME CLASSPATH PATH		// java -version
	计算机->属性->高级系统设置->高级->环境变量->系统变量
	创建JAVA_HOME系统变量
	创建CLASSPATH系统变量
	
	// ------------------------------------------
	// 创建工程
	// ------------------------------------------
	eclipse创建java工程 -> 选择next，添加jar包(selenium-java.jar)
	
	
	
	打开百度网实例
	package com.mushishi.selenium;
	import java.sql.Driver;

	// --------------------------------
	// 元素定位
	// --------------------------------
		public class selenium {
		WebDriver driver;	// 先定义
		
		System.setProperty("webdriver.firefox.marionette", "path_name");	// 配置环境变量
		driver = new FirefoxDriver();	// 初始化driver
		driver.findElement(By.tagName("input")).sendKeys("user");
		driver.findElement(By.name("password")).sendKeys("xxx");
		driver.findElement(By.id("auto-signin")).click();
		
		driver.findElement(By.linkText("立即注册")).click();
		driver.findElement(By.partialLinkText("直接")).click();
		
		drvier.manage().window().maximize();
		driver.get("http://www.imooc.com");
		driver.findElement(By.className("search-input")).click();
		driver.findElement(By.className("search-input")).sendKeys("xxx");
		
		WebElement element = driver.findElement(By.className("nav-item"));	// 元素对象类型WebElement
		List<WebElement> elements = element.findElement(By.tagName("li"));
		elements.get(3).click();
	}
	
	// --------------------------------
	// 常见元素处理
	// --------------------------------
	文本框：sendKeys、clear、getAttribute
	
	单选框：click、clear、isSelected
		driver.get("http://www.imooc.com/user/setprofile");		// 更快加载
		用xpath只能定位一个
	
	多选框：click、clear(不可使用，重复点击取消选中)、isSelected、isEnabled
	
	按钮：click、isEabled
	
	表单：
		检查元素界面 -> Network -> 观察手动提交表单做的操作 -> 对应logo接口 -> Form Data
	
		driver.findElement(By.id("signup-form")).submit();
		selenium以表单方式提交，只是把参数拼接到当前url后面，实际提交url不是这个地址
	
		driver.get("www.immoc.com");
		driver.findElement(By.id("xxx")).submit();	// 获取url，将input请求拼接起来，构成简单的接口测试
	
	上传文件：
		//按钮隐藏，修改style，通过JS定位 -- JS修改元素属性信息，让元素变的可操作
		在console测试
		document.getElementByClassName("js-avator-link")
		[a.js-avator-link]	// 数组，length = 1
		document.getElementByClassName("js-avator-link")[0].style.bottom = 0;
		
		driver转化成JS
		String jsString = "document.getElementByClassName("js-avator-link")[0].style.bottom = 0;";
		JavascriptExecutor js = (JavascriptExecutor)driver;
		js.executeScrpit(jsString);
		
		driver.findElement(By.id("upload")).sendKeys("C:\\user\\test.jpg");	// input框->sendKeys 注意路径转义字符
	
	下拉框定位
		选择对应的元素 Text、value、Index
		不选择对应的元素 deselectAll、deselectByValue、deseletByVisibleText
		获取选择项的值 getAllselectOptions、getFirstSelectedOption.getText
		
		Select list = new Select(locator);	// 通过selenium自带的类 -- 仅对select标签有效
		
		组合定位，只需要选取前级
		复合型，定位冲突时，通过层级定位
	
		悬停出现的页面 中的元素如何定位 -- ???
		
	元素进阶操作
		常用鼠标操作：鼠标左击、右击(contextClick)、双击(doubleClick)、悬停(moveToElement)
		左击:	Action action = new Actions(driver);
				action.click().perform()	// 需要提交 -- 没有perform不会执行
				
	特殊窗体
		窗体分类：iframe切换、弹窗处理、对话框
		switchTo()、getWindowHandlers()
		
		切换页面时，首先记录下当前页面handle	// curWindowsHandle = driver.getWindowHandler();
		切换后，拿到所有页面handle，判断新页面	// s.equals(windowHande){ continue; }
		
	三种等待分类
		强制等待	// Thread.sleep()
		显式等待	// 
		隐式等待	// 调试用
		
	// id class不唯一时，使用css、xpath定位，万能定位，web自动化中xpath常用。 -- 移动端自动化中xpath，效率低
		
// ----------------------------------------------
	登录自动化设计
	
	自动化用例设计，再将用例翻译成脚本
	并不是将所有的功能用例，翻译成自动化用例
	自动化测试将很多逻辑业务去掉，保留大部分冒泡测试用例(子功能、小模块的冒泡测试用例)
	更多是用来检验线上功能的正确性，或者监控线上功能
	
	fiddler监控接口
	
	Maven：构建、文档生成、报告、依赖		// 属于Apache
	添加到eclipse
	Maven创建工程，通过修改pom.xml文件添加相应的包 -- Maven repository -- // 不需要手动下载离线包，也不用再工程中build path中添加相应库
	
	读取配置文件：properties
		load			// 加载 常用
		setProperty
		getProperty		// 读取 常用
		
	testNG自动化测试框架 -- 配置Maven仓库时放在系统盘，没有权限更新仓库
						 -- 安装testNG的三种方式，不一定要 右键创建testNG类，以及 右键testNG运行
	
	PO模型 page object
	分层设计：元素层、操作层、业务层
	driver及基类的封装：
		handle
		business
		testcase
		DriverBase
		SelectDriver
		CaseBase
		page
		
	
	一个页面上的元素（如输入框）与其他页面只是名字不同，定位信息不同
	
	抽取功能测试部分测试用例，编写自动化用例 -- 冒烟测试的详细版
	
	使用fiddler配合下单流程的功能测试
	
	courseInfoText=tagName>span		// tagName定位 
	
	模态框的处理
		
#endif

#if 

 “<head>”标签和“<body>”标签是它的第一层子元素，“<head>”标签里面负责对网页进行一些设置以及定义标题，设置包括定义网页的编码格式，外链css样式文件和javascript文件等，设置的内容不会显示在网页上，标题的内容会显示在标题栏，“<body>”内编写网页上显示的内容
 
 搜索引擎会使用标题将网页的结构和内容编制索引，所以网页上使用标题是很重要的
 语义化的标签，就是在布局的时候多使用语义化的标签，搜索引擎在爬网的时候能认识这些标签，理解文档的结构，方便网站的收录
 
 <a href="http://www.itcast.cn/" title="跳转的传智播客网站">传智播客</a>  -- 悬停显示tittle
 
 <a href="#mao1">标题一</a>
 <h3 id="mao1">AAAA</h3>
 
 无序列表 <ul> <li>列表文字</li> </ul>
 
 <link rel="stylesheet" type="text/css" href="css/main.css">
 
 <style type="text/css">
	div{width:100px; height:100px}
 </style>
 
 <div style="width:100px; height:100px">AAAA</div>
 
 应用文本常用css设置
 css颜色表示法三种
 
 css选择器
 <div>...</div>						div{color:red}
 <div id="box">...</div>			#box{color:red}
 <div class="box big">...</div>		.box{color:red} .big{font-size:20px}
 
 常用的伪类选择器有hover，鼠标悬浮再元素上时的状态
 伪元素选择器before和after，可以通过样式再元素中插入内容
 
 content -> padding -> border -> margin
 
 这三种元素，可以通过display属性来相互转化，不过实际开发中，块元素用得比较多，所以我们经常把内联元素转化为块元素，少量转化为内联块，而要使用内联元素时，直接使用内联元素，而不用块元素转化了
 
 一般div有很多层时，尽量不要使用外层
 
 进入其他页面，再返回该页面，之前定位信息失效 -- selenium机制决定