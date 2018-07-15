#coding:utf-8

import numpy as np

def _empty(a):
    return a is None or a == '' or a == 'NULL'


class Pokemon:

    def __init__(self):
        self.attr_ = {}
    def attr(self, name):
        return self.attr_.get(name, None)


class Pokedex:

    def __init__(self):
        self.pokemons_ = 
