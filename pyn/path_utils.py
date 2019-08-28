#!/usr/bin/env python3
# coding=utf-8


import os
from enum import Enum


# 普通节点
class Node(object):
    def __init__(self):
        self.parent = None
        self.child_list = list()

    def add(self, child):
        child.parent = self
        child.init_attr()
        self.child_list.append(child)

    def remove(self, child):
        if len(self.child_list) > 0:
            self.child_list.remove(child)


# 路径节点的属性
class ItemPathAttr(object):
    # 枚举访问模式
    class MODE(Enum):
        READ = 1
        WRITE = 2
        EXEC = 4

    def __init__(self, access=MODE["READ"], size=0, visit=0, create=0, modify=0):
        self.access = access
        self.size = size
        self.visit = visit
        self.create = create
        self.modify = modify

    def read(self):
        return self.access & self.MODE.READ

    def write(self):
        return self.access & self.MODE.WRITE

    def exec(self):
        return self.access & self.MODE.EXEC


# 路径节点（继承Node的接口和实现）
class ItemPath(Node):
    def __init__(self, path):
        super(ItemPath, self).__init__()
        path = self.make_abspath(path)
        self.__path = path
        self.__name = os.path.basename(path)
        self.__attr = None
        self.init_attr()

    def init_attr(self):
        if self.__path is None or self.__path == "":
            return
        access = ItemPathAttr.MODE["READ"] and os.access(self.__path, os.R_OK)
        access = access | (ItemPathAttr.MODE["WRITE"] and os.access(self.__path, os.W_OK))
        access = access | (ItemPathAttr.MODE["EXEC"] and os.access(self.__path, os.X_OK))
        info = os.stat(self.__path)
        self.__attr = ItemPathAttr(access, info.st_size, info.st_atime, info.st_ctime, info.st_mtime)

    def list_all_entries(self):
        pass

    # 属性，外部引用时只读
    @property
    def abspath(self):
        return self.__path

    # 属性，外部引用时只读
    @property
    def basename(self):
        return self.__name

    # 属性，外部引用时只读
    @property
    def attribute(self):
        return self.__attr

    # 静态成员函数
    @staticmethod
    def make_abspath(path):
        if path is None or path == "":
            path = os.path.abspath(".")
        elif os.path.isabs(os.path.join(path, os.sep)) is False:
            path = os.path.abspath(path)
        return path


# 文件节点
class FilePath(ItemPath):
    def __init__(self, path):
        super(FilePath, self).__init__(path)

    def list_all_entries(self):
        return self.child_list


# 目录节点
class DirPath(ItemPath):
    def __init__(self, path=""):
        # 这步一漏就歇菜了
        super(DirPath, self).__init__(path)

    def list_all_entries(self):
        try:
            if os.path.exists(self.abspath) is False:
                return None
            items = os.listdir(self.abspath)
            for item in items:
                abs_item = os.path.join(self.abspath, item)
                if os.path.isdir(abs_item) is True:
                    if os.access(abs_item, os.R_OK) is True:
                        dir_path = DirPath(abs_item)
                        self.add(dir_path)
                        dir_path.list_all_entries()
                    else:
                        print(abs_item + " read failed!")
                else:
                    self.add(FilePath(abs_item))
        except Exception as err:
            print(err)
        return self.child_list
    # 最后留一个空行
