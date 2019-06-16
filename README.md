# simulated_mysql
incomplete mysql rearranged in c plus plus for course design

实现的单函数：
int char_length(char)
char lower(char)
char upper(char)
char space(int)

num abs(num)
double sin(double)
double cos(double)
double tan(double)
double exp(double)

date adddate(d,n)
time addtime(d,n)

实现的聚合函数：
double avg(double)
int count(exp)
auto max(exp)
auto min(exp)


支持运算符/逻辑符（从上到下优先级递增）：
|| or xor
&& and
not !（只表示逻辑非）
= > < != LIKE
+ -
* / DIV % MOD


create database oop;
use oop;
create table info(id int not null, d date, primary key(id));
insert into info(id,d) values(1,"2000-10-19");
select * from info where id<2 xor d>"2000-11-19";
select * from info where id<2 xor d>"2000-10-18";

已经实现：复杂表达式，涵盖逻辑算术运算符函数，可以嵌套，可以加上as来指定别名
如：
select (ABS(-1) - 5) /2 as AaA, (sin( 1) - cos (2))/exp(1) as BBb,char_length("  aaa") + sin(Abs (-1)) as C, adddate("2000-10-19", 0), 3 Mod 0, 2 div 5;
输出结果：
AaA     BBb     C       adddate("2000-10-19", 0)        3 Mod 0 2 div 5
-2.0000 0.4627  5.8415  2000-10-19      NULL    0.4000

支持根据多个关键字排序，按照出现的先后顺序权重递减
为了输出count(*)，把all（忽略大小写）作为保留关键字供内部转换使用，不能用作别名、字段名

group和order过了样例，oj上还没提交

一个稍复杂的联合语句：
CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(stu_id INT NOT NULL, PRIMARY KEY(stu_id), stu_name CHAR, grade int);
INSERT INTO oop_info(stu_id, stu_name, grade) VALUES (2018011243, "aaaaa", -1);
INSERT INTO oop_info(stu_id, stu_name, grade) VALUES (2018011043, "abb", 2);
INSERT INTO oop_info(stu_id, stu_name, grade) VALUES (2018010243, "ab", 3);
INSERT INTO oop_info(stu_id, stu_name, grade) VALUES (201801024, "acccaaa", -3);
INSERT INTO oop_info(stu_id, stu_name, grade) VALUES (2018010243, "a", 3);
INSERT INTO oop_info(stu_id, stu_name, grade) VALUES (2018001344, "bbbbbb", -2);
INSERT INTO oop_info(stu_id, stu_name, grade) VALUES (2018001394, "bbb", 1);
INSERT INTO oop_info(stu_id, stu_name, grade) VALUES (2018011445, "c", 4);
SELECT grade*grade,1 - COUNt(*), Avg(grade) as avggrade, min(stu_id) from oop_info GROUP BY grade*grade ORDER BY COUNT(*),-min(stu_id) where char_length(stu_name)+grade = 4;
DROP DATABASE OOP;
输出结果
grade*grade     1 - COUNt(*)    avggrade        min(stu_id)
4       0       -2.0000 2018001344
1       -1      0.0000  2018001394
9       -1      0.0000  201801024

多表联合where测试：
create database oop;
use oop;
create table t1(id int, name char, primary key(id));
create table t2(id int, name char, primary key(id));
insert into t1(id, name) values(1, "1a");
insert into t1(id, name) values(2, "1b");
insert into t1(id, name) values(3, "1c");
insert into t2(id, name) values(2, "2a");
insert into t2(id, name) values(3, "2b");
insert into t2(id, name) values(1, "2c");
insert into t2(id, name) values(4, "2d");
select t1.name,t2.name from t1,t2 where t1.id = t2.id;
drop database oop;
输出结果
t1.name t2.name
1a      2c
1b      2a
1c      2b
联合处理时使用复杂表达式、group、order，三表即以上联合还没测，是潜在的特性
需要更多测试
like语句和其他的联合没测；where里面的复杂逻辑式没测
可以处理带有空格的字符串了，如select " a ";

完成了union和join
union中order by可根据多个关键字排序
join中select支持as指定别名

union测例：
create database oop;
use oop;
create table test1 (
id int,
 name char, primary key(id)
);

create table test2 (
id int,
 name char, primary key(id)
);
INSERT INTO test1 (id ,name) values(1,'test11');

INSERT INTO test1 (id ,name) values(2,'test12');

INSERT INTO test1 (id ,name) values(3,'test13');

INSERT INTO test1 (id ,name) values(4,'test14');

INSERT INTO test1 (id ,name) values(5,'test15');


INSERT INTO test2 (id ,name) values(1,'test11');
INSERT INTO test2 (id ,name) values(3,'test11');

INSERT INTO test2 (id ,name) values(4,'test11');

INSERT INTO test2 (id ,name) values(5,'test11');

INSERT INTO test2 (id ,name) values(6,'test11');

INSERT INTO test2 (id ,name) values(7,'test11');

INSERT INTO test2 (id ,name) values(8,'test11');
select * from test1 union select * from test2 order by name;
select * from test1 union all select * from test2 order by id;

join测例：
create database oop;
use oop;
create table test1 (
id int,
 name char, primary key(id)
);

create table test2 (
id int,
 name char, primary key(id)
);
INSERT INTO test1 (id ,name) values(1,'test11');

INSERT INTO test1 (id ,name) values(2,'test12');

INSERT INTO test1 (id ,name) values(3,'test13');

INSERT INTO test1 (id ,name) values(4,'test14');

INSERT INTO test1 (id ,name) values(5,'test15');


INSERT INTO test2 (id ,name) values(3,'test11');

INSERT INTO test2 (id ,name) values(4,'test11');

INSERT INTO test2 (id ,name) values(5,'test11');

INSERT INTO test2 (id ,name) values(6,'test11');

INSERT INTO test2 (id ,name) values(7,'test11');

INSERT INTO test2 (id ,name) values(8,'test11');
SELECT * FROM test1 t1 LEFT JOIN  test2 t2 on t1.id=t2.id;
SELECT * FROM test1 t1 right JOIN  test2 t2 on t1.id=t2.id;
SELECT * FROM test1 t1  inner join  test2 t2 on t1.id=t2.id;
输出结果：
SELECT * FROM test1 t1 LEFT JOIN  test2 t2 on t1.id=t2.id;
t1.id   t1.name t2.id   t2.name
1       test11  NULL    NULL
2       test12  NULL    NULL
3       test13  3       test11
4       test14  4       test11
5       test15  5       test11
SELECT * FROM test1 t1 right JOIN  test2 t2 on t1.id=t2.id;
t1.id   t1.name t2.id   t2.name
3       test13  3       test11
4       test14  4       test11
5       test15  5       test11
NULL    NULL    6       test11
NULL    NULL    7       test11
NULL    NULL    8       test11
SELECT * FROM test1 t1  inner join  test2 t2 on t1.id=t2.id;
t1.id   t1.name t2.id   t2.name
3       test13  3       test11
4       test14  4       test11
5       test15  5       test11


plan:
存档(10 points)（尝试看一下B+树,10 points）
