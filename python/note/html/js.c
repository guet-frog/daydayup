var a

模板字符串
var message = "hello, ${name}"
字符串的常见操作

数组的常见操作方法

具有iterable类型的集合通过for of循环来遍历		// Array Map Set
通过iterable内置的forEach()

抽象是数学中常见概念，借助抽象，我们不关心底层的具体计算过程，而直接在更高的层次上思考问题
函数是最基本的一种代码抽象方式

定义函数方式1
function abs(x)
{
	return x;			// 没有return语句，函数执行完毕返回undefined
}

JS函数也是一个对象，定义的abs()函数实际上是一个函数的对象，而函数名abs可以视为指向该函数的变量

var abs = function (x)		// 匿名函数
{
	return x;
};	// 表示赋值语句结束

关键字arguments，只在函数内部起作用，指向当前函数的调用者传入的所有参数，arguments类似Array
常用与判断参数的个数

在函数内部首先申明所有变量

不在任何函数内定义的变量具有全局作用域
JS默认有一个全局对象window，全局作用域的变量 被 绑定为 window的一个 属性

alert(tmp)
alert(window.tmp)

var func = function() {}	// 定义函数实际上也是一个全局变量
func()
window.func()

alert()函数其实也是window的一个变量
JS实际只有一个全局的作用域，任何变量(函数也视为变量)，
如果没有在当前函数作用域中找到，就会继续往上查找，最后如果在全局作用域中也没有找到，则报ReferenceError错误

全局变量会绑定到window，不同的JS文件相同的全局变量、相同名字的顶层函数，造成命名冲突
减少冲突 -- 把所有的变量和函数全部绑定到一个全局变量中，唯一的命名空间	// JS库 jQuery

let替代var可以申明一个块级作用域	// JS的变量作用域实际是函数内部
for (let i=0; i<100; i++)

解构赋值
var {hostname:domain, pathname:path} = location;	// 快速获取当前页面的域名和路径

JS的函数内部调用this
person.age();			// this指向被调用对象
getAge();				// this指向全局对象，window

getAge.apply(person, [])	// 控制this的指向

Math.max.apply(null, [3, 4, 5])		// 对普通函数调用，通常把this绑定为null
Math.max.call(null, 3, 4, 5)

// 统计函数的调用次数，动态改变函数的行为
var count = 0;
var oldParseInt = parseInt;

window.parseInt = function() {
	count += 1;
	return oldParseInt.apply(null, arguments);
}

高阶函数，函数的参数可以接收别的函数
使用filter()这个高阶函数，关键在于正确实现一个"筛选"函数
var r = array.filter(function (s)){
	return s && s.trim();
}

高阶函数，可以把函数作为结果值返回
function sum(arr){
	return arr.reduce(function (x, y){
		return x + y;
	});
}

如果不需要立即求和，而是返回求和的函数
function lazy_sum(arr){
	var sum = function(){}
}

闭包：函数作为返回值




