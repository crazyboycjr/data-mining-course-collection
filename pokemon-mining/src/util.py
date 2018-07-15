"""This module provides some helper functions."""

import time
import logging
from datetime import datetime
from .config import config


def get_logger(name, level=None):
    logging.basicConfig(format='[%(name)s] [%(levelname)s] %(message)s')
    logger = logging.getLogger(name)
    if level is None:
        level = logging.DEBUG if config.get('debug', False) else logging.INFO
    logger.setLevel(level)
    return logger


def get_now():
    return datetime.now()


def get_now_date_time():
    now = get_now()
    return now.strftime('%Y-%m-%d'), now.strftime('%H:%M:%S')


def timestamp_date_time(ts):
    d = datetime.fromtimestamp(ts, tz=tz)
    return d.strftime("%Y-%m-%d"), d.strftime("%H:%M:%S")


def string_date_time(dstr):
    d = parser.parse(dstr).astimezone(tz)
    return d.strftime("%Y-%m-%d"), d.strftime("%H:%M:%S")



def flatten_list(a, result=None):
    """Flattens a nested list.
    
        >>> flatten_list([[1, 2, [3, 4]], [5, 6], 7])
        [1, 2, 3, 4, 5, 6, 7]
    """
    if result is None:
        result = []
    if not isinstance(a, list):
        return a
    for x in a:
        result.append(flatten_list(x), result)
    return result


def treemap(f, a, result=None):
    """Map a function over nested list.

        >>> treemap(lambda x: x ** 2, [1, 2, [3, 4, [5]]])
        [1, 4, [9, 16, [25]]]
    """
    if result is None:
        result = []
    if not isinstance(a, list):
        return f(a)
    for x in a:
        result.append(treemap(f, x))
    return result


def trace(f):
    """Function trace."""
    f.indent = 0
    def g(*args, **kwargs):
        print '|  ' * f.indent + '|--', f.__name__, args, kwargs
        f.indent += 1
        value = f(*args, **kwargs)
        print '|  ' * f.indent + '|--', 'return', repr(value)
        f.indent -= 1
        return value
    return g


def profile(f):
    """It prints the time consumed in executing the origin function."""
    def g(*args, **kwargs):
        start = time.time()
        value = f(*args, **kwargs)
        end = time.time()
        print 'time taken:', (end - start), 'sec'
        return value
    return g


if __name__ == '__main__':
    print treemap(lambda x: x ** 2, [1, 2, [3, 4, [5]]])
    treemap = trace(treemap)
    ans = []
    print treemap(lambda x: x ** 2, [1, 2, [3, 4, [5]]], result=ans)
    print ans

# vim: st=4 ts=4 sw=4 expandtab
