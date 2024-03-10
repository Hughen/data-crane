import multiprocessing as mp
import docker
from logwatcher.tailf import Tailf


update_ds_time_script = """
local ds_id = KEYS[1]
local ds_type = KEYS[2]
local timestamp = ARGV[1]
local has_src = ARGV[2]

local key = ds_id .. ds_type

if has_src == 1
then
    local active_last_time = tonumber(redis.call("hget", key, "active_last_time"))
    if active_last_time < timestamp
    then
        redis.call("hset", key, "active_last_time", timestamp)
    end
end

local training_last_time = tonumber(redis.call("hget", key, "training_last_time"))
if training_last_time < timestamp
then
    redis.call("hset", key, "training_last_time", timestamp)
end

redis.call("expire", key, 180000)
"""

refresh_ds_liveness = """
local function is_same_day(t1, t2)
    local dt1 = os.date("*t", t1)
    local dt2 = os.date("*t", t2)
    return dt1.year == dt2.year and dt1.month == dt2.month and dt1.day == dt2.day
end

local ds_id = KEYS[1]
local ds_type = KEYS[2]
local training_last_time = tonumber(ARGV[1])
local active_last_time = tonumber(ARGV[2])

local key = ds_id .. ds_type
local expired = false

if training_last_time >= 0
then
    local last_time = tonumber(redis.call("hget", key, "training_last_time"))
    if last_time < training_last_time
    then
        if is_same_day(last_time, training_last_time) == false
        then
            redis.call("hincrby", key, "training_count", 1)
        end
        redis.call("hset", key, "training_last_time", training_last_time)
        redis.call("expire", key, 93600)
        expired = true
    end
end

if active_last_time >= 0
then
    local last_time = tonumber(redis.call("hget", key, "active_last_time"))
    if last_time < active_last_time
    then
        if is_same_day(last_time, active_last_time) == false
        then
            redis.call("hincrby", key, "active_count", 1)
        end
        redis.call("hset", key, "active_last_time", active_last_time)
        if expired == false
        then
            redis.call("expire", key, 93600)
        end
    end
end
"""


def stats():
    crane_queue = mp.Queue(maxsize=20000)
    datahub_queue = mp.Queue(maxsize=10000)
