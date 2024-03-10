import multiprocessing as mp
import redis
from typing import Tuple
from abc import abstractmethod
import os
import time


class Tailf(mp.Process):
    def __init__(self, log_path: str, output_queue: mp.Queue):
        super().__init__()

        self.log_path = log_path
        self._output_queue = output_queue

        self._new_redis_client()

    def run(self):
        try:
            prev_inode, prev_size, prev_ctime = self._get_log_info_from_cache()
            cur_inode, cur_size, cur_ctime = self._get_log_info()
            seek_pos = self._get_seek_pos_from_cache()
            if prev_inode != cur_inode:
                seek_pos = 0

            fd = open(self.log_path, "r")
            fd.seek(seek_pos)
            while True:
                line = fd.readline()
                line = line.strip()
                pos = fd.tell()
                if not line:
                    if pos != seek_pos:
                        seek_pos = pos
                        self._save_log_info(seek_pos=seek_pos)
                        continue
                    seek_pos = pos
                    self._save_log_info(seek_pos=0)
                    fd.close()
                    fd = None
                    # 暂停一段时间，让内核回收掉可能已经发生 truncated 的 handler
                    time.sleep(30)
                    inode, size, ctime = self._get_log_info()
                    self._save_log_info(inode=inode, size=size, ctime=ctime)
                    fd = open(self.log_path, "r")
                    # 没有发生异常，则需要将读取位置切到上次 close 的地方
                    fd.seek(pos)
                    self._save_log_info(seek_pos=pos)
                    continue
                if not self.filter(line):
                    continue
                self.queue.put(line)
                self._save_log_info(seek_pos=pos)
        except FileNotFoundError:
            # TODO: 打印 not found
            self._redis.delete(self.log_path)

    @property
    def queue(self) -> mp.Queue:
        return self._output_queue

    @abstractmethod
    def filter(self, log: str) -> bool:
        pass

    def _new_redis_client(self):
        pool = redis.ConnectionPool(connection_class=redis.UnixDomainSocketConnection, path="/var/run/redis/redis.sock")
        self._redis = redis.Redis(connection_pool=pool)

    def _get_log_info_from_cache(self) -> Tuple[int, int, float]:
        inode = int(self._redis.hget(self.log_path, "inode"))
        size = int(self._redis.hget(self.log_path, "size"))
        ctime = int(self._redis.hget(self.log_path, "ctime"))
        if inode:
            return inode, size, ctime
        return 0, 0, 0

    def _get_log_info(self) -> Tuple[int, int, float]:
        stats = os.stat(self.log_path)
        return stats.st_ino, stats.st_size, stats.st_ctime

    def _save_log_info(self, inode: int = 0, size: int = -1, ctime: float = 0, seek_pos: int = -1):
        if inode:
            self._redis.hset(self.log_path, "inode", str(inode))
        if size >= 0:
            self._redis.hset(self.log_path, "size", str(size))
        if ctime:
            self._redis.hset(self.log_path, "ctime", str(ctime))
        if seek_pos >= 0:
            self._redis.hset(self.log_path, "seek_pos", str(seek_pos))
        # ttl=5days
        self._redis.expire(self.log_path, time=432000)

    def _get_seek_pos_from_cache(self) -> int:
        return int(self._redis.hget(self.log_path, "seek_pos"))
