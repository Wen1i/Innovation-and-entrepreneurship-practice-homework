# Project11-Implement-ECMH-scheme
ECMH:把哈希映射成椭圆曲线上的点，然后利用ECC进行运算。ECMH可以把多个数据的 hash 合并到一个 hash 中，但还可以支持删除。这样节点维护一个 UTXO 的根 hash 的成本就很低了，每次只需做增量修改。然后只需要把 UTXO 根 hash 记录到区块上，其他节点同步 UTXO 集合之后，就可以验证该集合是否被篡改了。但这个方案的缺点是只能做全量验证，没办法验证单独一个 UTXO 是否存在。
## 实现功能
* Add elements
~~~
def ADD(ecmh, msg):
    dot = msg_to_dot(msg)
    tmp = EC_add(ecmh, dot)
    return tmp
~~~

* Combine elements
~~~
def combine(msg_set):
    ans = single(msg_set[0])
    num = len(msg_set) - 1
    for i in range(num):
        ans = ADD(ans, msg_set[i+1])
    return ans
~~~

* Remove elements
~~~
def remove(ecmh, msg):
    dot = msg_to_dot(msg)
    tmp = EC_sub(ecmh, dot)
    return tmp
~~~

