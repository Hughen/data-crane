import myclient

class crane():
    def __init__(self):
        self.client = myclient.crane_new()
    
    def hello(self):
        print("python3 function hello", myclient.hello(self.client))
    
    def __delete__(self):
        myclient.close()
