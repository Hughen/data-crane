from ac2.shmloader.dataset import MLDBBaseDataset, _getitem
from typing import Dict, Any, Callable
import mldb
import ray


class DatasetBatch(MLDBBaseDataset):
    def __init__(
        self,
        name: str,
        version: int,
        world_rank: int,
        world_size: int,
        object_refs,
        object_storage,
        convert_func: Callable[[Dict[str, Any]], Dict[str, Any]] = None,
    ) -> None:
        super().__init__(world_rank, world_size, None, object_refs, object_storage)

        # 解析 databatch
        self.batch = []
        self.convert_func = convert_func

    def __getitem__(self, idx):
        try:
            ref_id = self.object_refs[idx]
        except KeyError:
            data = self._read_data(idx)
            if callable(self.convert_func):
                data = self.convert_func(data)
            if self.storage is not None and idx not in self.storage:
                ref_id = ray.put(data)
                self.storage[idx] = ref_id
            return data
        return _getitem(ref_id)

    def _read_data(self, idx) -> Dict[str, Any]:
        # TODO: 将 self.batch 每个元素变成一个具备标准读取接口的类型
        item = self.batch[idx]
        data = {}
        for k, vd in item.items():
            v = vd.get("v", None)
            if v:
                data[k] = v
                continue
            content = mldb.Resource(vd["uri"]).read()
            data[k] = content
        return data
