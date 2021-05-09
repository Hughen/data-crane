# mongo 2 es

## ES mapping

`DataBatch`，`RawData`mapping 结构：

```json
{
    "mappings": {
        "properties": {
            "type": {
                "type": "keyword"
            },
            "name": {
                "type": "text"
            },
            "projectID": {
                "type": "text",
                "fields": {
                    "keyword": {
                        "type": "keyword",
                        "ignore_above": 64
                    }
                }
            },
            "creator": {
                "type": "text",
                "fields": {
                    "keyword": {
                        "type": "keyword",
                        "ignore_above": 64
                    }
                }
            },
            "digest": {
                "type": "keyword"
            },
            "count": {
                "type": "integer_range"
            },
            "size": {
                "type": "long_range"
            },
            "customMeta": {
                "dynamic": "true",
                "type": "object"
            },
            "clientMeta": {
                "dynamic": "true",
                "type": "object"
            }
        }
    }
}
```

fs 文件 mapping 结构：

```json
{
    "mappings": {
        "properties": {
            "type": {
                "type": "keyword"
            },
            "name": {
                "type": "text"
            },
            "dataset": {
                "type": "text"
            },
            "contentType": {
                "type": "text",
                "fields": {
                    "keyword": {
                        "type": "keyword",
                        "ignore_above": 64
                    }
                }
            },
            "creator": {
                "type": "text",
                "fields": {
                    "keyword": {
                        "type": "keyword",
                        "ignore_above": 64
                    }
                }
            },
            "digest": {
                "type": "keyword"
            }
        }
    }
}
```


```shell
curl -XPUT "http://127.0.0.1:9200/index_1" -H "Content-Type: application/json" -d '
{
    "mappings": {
        "properties": {
            "city": {
                "type": "text",
                "fields": {
                    "raw": { 
                        "type": "keyword"
                    }
                }
            }
        }
    }
}
'

curl -XPUT "http://127.0.0.1:9200/index_1/_doc/1" -H "Content-Type: application/json" -d '
{
    "city": "Shen Zhen"
}
'
curl -XPUT "http://127.0.0.1:9200/index_1/_doc/2" -H "Content-Type: application/json" -d '
{
    "city": "Shen"
}
'

curl "http://127.0.0.1:9200/index_1/_search" -H "Content-Type: application/json" -d '
{
    "query": {
        "match": {
            "city": "shen"
        }
    },
    "sort": {
        "city.raw": "asc" 
    },
    "aggs": {
        "Cities": {
            "terms": {
                "field": "city.raw" 
            }
        }
    }
}
'
```
