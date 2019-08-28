#!/usr/bin/env python3
# coding=utf-8


import sys
import getopt
from path_utils import *
import os


def walk_path(path):
    file_list = os.walk(path)
    for dir_path, dir_names, file_names in file_list:
        pass


def print_usage():
    print("""
    usage:
    -s, --source      : specify the source dir
    -d, --destination : specify the destination dir
    -h, --help        : print the help info
    """)


def compare_files(src_dir):
    for item in src_dir.child_list:
        if isinstance(item, DirPath):
            compare_files(item)
        elif isinstance(item, FilePath):
            pass


def main(argv):
    # 解析命令行参数
    try:
        opts, args = getopt.getopt(argv[1:], "s:d:h", ["source=", "destination=", "help"])
    except getopt.GetoptError as err:
        print(err)
        print_usage()
        sys.exit(2)

    src = str()
    des = str()

    # 根据各参数值做决策
    for key, value in opts:
        if key in ["-h", "--help"]:
            print_usage()
            return 0
        elif key in ["-s", "--source"]:
            src = value
        elif key in ["-d", "--destination"]:
            des = value

    driver = os.path.splitdrive(src)[0]
    if driver is None or driver == "":
        src = DirPath(src).abspath()

    # walk_path(src)

    try:
        if des is None or des == "":
            des = os.path.join(src, "DestinationDir")
        elif os.path.isabs(os.path.join(des, os.sep)) is False:
                des = os.path.join(src, des)

        if os.path.exists(des) is False:
            os.makedirs(des)
    except Exception as err:
        print(err)
        return 1

    # 列出所有文件名和文件路径
    src_dir = DirPath(src)
    src_dir.list_all_entries()
    print("main over, get the results!")

if __name__ == "__main__":
    main(sys.argv)