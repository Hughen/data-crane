# MLDB SDK

## 编译

### 预备条件

- cmake version >= 3.13
- python3, 推荐 3.7 及其以上

> 可以执行`install-dep.sh`，来检查和安装 SDK 所依赖的所有相关项目。

```bash
cmake .

# 如果是使用 c++ 接口，include sdk.h 头文件即可
# 编译过程参考：
make -j
```

对于 python 类型的环境，直接运行`make py_setup_install`，程序将自动为python编译安装 MLDB SDK 扩展。

## mldb uri 定义

```text
mldb://resource-type/r-path-1/r-path-2/r-path-3?parameter-a=1&parameter-b=value
\__/   \__________/  \_______________________/  \____________________________/
  |         |                    |                            |
协议     资源类型              资源路径                       查询串
```

## python sdk 使用举例

```python
from mldb import io, dispatcher, prefetcher

# 预取
prf = prefetcher.Prefetcher()
prf.append(list={"mldb://batch/coco2017/object/coco_det-validation.tfrecord-00005-of-00008", "mldb://batch/coco2017/object/coco_det-validation.tfrecord-00005-of-00008.idxs"})
# 向服务端发起预取开始信号
num = prf.start()
print("it will prefetch {} objects".format(num))

# 读取
with io.open("mldb://batch/coco2017/object/coco_det-validation.tfrecord-00005-of-00008") as f:
  content = f.read()
  print("the file content length: {}".format(len(content)))
```

## SDK 调试

对于 SDK 部分的代码，如果需要调试，只需要将 log level 提高到 6 即可，也就是将环境变量设置为：`MLDB_SDK_LOG=6`

```cpp
// log 级别
enum {
    Info  = 2,
    Warn  = 3,
    Error = 4,
    Fatal = 5,
    Debug = 6
}
```
