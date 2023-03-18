# simple_sql
## summary

一个简单的sql，类似于splite

1、表的格式固定

（id，username，email）

2、command交互

3、简单游标

4、b树索引，目前只支持=查

`select where id=?`

4、`.indextree`  metaCommand查看indextree

```
db > .indextree
---------------------------------------------------------------------------------------------------
[|{unique_id=0,pos=294c2792bb0}|{unique_id=1,pos=294c2792cd3}|{unique_id=2,pos=294c2792df6}|]
---------------------------------------------------------------------------------------------------
```



## todo
1、索引文件持久化

2、支持简单范围查

`select where id<?`

## future

。。。



## use

![image-20230318182617581](C:\Users\26685\AppData\Roaming\Typora\typora-user-images\image-20230318182617581.png)

