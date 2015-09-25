# test Configuration object

class Configuration:

    d1 = 10
    d2 = 20

class S1:

    def __init__(self):
        self.v = 3

    def asdf(self):
        return self.v * Configuration.d1

if __name__ == "__main__":
    s1 = S1()
    print s1.asdf()

