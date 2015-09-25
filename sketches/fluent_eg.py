# test fluent style interface

class Floozy:

    def __init__(self):
        self._val = 0

    def val(self, val = None):
        if val == None:
            return self._val
        else:
            self._val = val
            return self

if __name__ == "__main__":
    f1 = Floozy()
    print f1.val(33)            # setter
    print f1.val()              # getter
