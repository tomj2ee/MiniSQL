# MiniSQL
A tiny relational database!!!

# Feature
表的建立/删除
索引的建立/删除
记录的插入/删除/查找

# Detail
## 数据类型
int, float, char(n) 1<=n<=255

## 表定义
最多32个属性
unique属性
单属性主键定义

## 索引的建立和删除
对主键自动建立B+树索引
对unique属性可以建立或者删除B+树索引
B+树索引均为单属性、单值

## 记录的查找
等值查询
范围查询
and连接的多条件查询

## 记录的插入和删除
单条记录的插入
单条或者多条记录的删除

## 支持标准的SQL语句
每条SQL语句以分号结尾
一条SQL语句可以一行或多行
关键字均为小写

支持：创建表语句、删除表语句、创建索引语句、删除索引语句、选择语句、插入记录语句、删除记录语句、退出系统语句、执行SQL脚本语句

样例：
create table student(
	sno char(8),
	sname char(16) unique,
	sage int,
	sgender char(1),
	primary key ( sno )
);

drop table student;

create index stunameidx on student ( sname );

select * from student where sage > 20 and sgender = 'F';

# Architecture
![minisql](https://github.com/travmygit/MiniSQL/blob/master/res/minisql.png)