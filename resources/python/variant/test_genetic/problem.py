from variant import model

class Model(model.ModelBase):
    def create(self):
        print "create"
        self.a = self.parameters["a"]
        self.b = self.parameters["b"]
        self.c = self.parameters["c"]
        self.d = self.parameters["d"]
        self.e = self.parameters["e"]

    def solve(self):
        print "solve"
        self.solved = True

    def process(self):
        print "process"
        self.variables["Func1"] = self.a + self.b + self.c + self.d + self.e
        
        # why does the setter not work?
        #self.functional = self.variables["Func1"]
        
        self.variables["_functional"] = self.variables["Func1"]