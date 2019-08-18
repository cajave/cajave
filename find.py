#!/usr/bin python3
# -*-coding:UTF-8-*-
import os
import sys
import getopt
import codecs


class IHandler:
    def __init__(self):
        self.__handlers = []

    def add_handler(self, handler):
        self.__handlers.append(handler)

    def remove_handler(self, handler):
        self.__handlers.remove(handler)

    @property
    def handlers(self):
        return self.__handlers

    def handle(self, content):
        return ""


class HasHandler(IHandler):
    def __init__(self, std):
        super(HasHandler, self).__init__()
        self.__std = std

    def handle(self, content):
        if self.__std in content:
            return content
        else:
            return str()


#
# 选项处理器
#
class OptsHandler(IHandler):
    """ 选项处理器 """

    def __init__(self, dir):
        super(OptsHandler, self).__init__()
        self.__dir = dir
        self.__name_handler = IHandler()
        self.__key_handler = IHandler()

    def handle(self, dir):
        self.__dir = dir
        for cur_dir, sub_dirs, names in os.walk(self.__dir):
            for name in names:
                file_path = os.path.join(cur_dir, name)
                if not self.__handle_name(file_path):
                    continue
                result = self.__handle_key(file_path)
                if result:
                    print(result)

    def add_name_handler(self, handler):
        self.__name_handler.add_handler(handler)

    def add_key_handler(self, handler):
        self.__key_handler.add_handler(handler)

    def __handle_name(self, path):
        if not self.__name_handler.handlers:
            return True
        for handler in self.__name_handler.handlers:
            if not handler.handle(path):
                return False
        return True

    def __handle_key(self, path):
        if not self.__key_handler.handlers:
            return path
        file = codecs.open(path, 'r', 'UTF-8')
        if file.readable():
            content = file.read()
            for handler in self.__key_handler.handlers:
                if not handler.handle(content):
                    return str()
            return path
        else:
            return str()


#
# 命令行选项解析器
#
class CmdOptsParser:
    """ 命令行选项解析器 """

    def __init__(self):
        self.__cmdline = []
        self.__parsed = ""
        self.__app = ""
        self.__source = "."
        self.__key = ""
        self.__name = ""
        self.__result = ""
        self.__case = False
        
    def __str__(self):
        return "parsed input :\n{}\n".format(self.__parsed)

    def parse_options(self, cmdline):
        self.__cmdline = cmdline
        self.__app = self.__cmdline[0]
        self.__parsed = self.__app
        try:
            opts, args = getopt.getopt(self.__cmdline[1:], "HS:K:N:R:C",
                                       ["help", "source=", "key=", "name=", "result=", "case"])
        except getopt.GetoptError as err:
            print("{}\n".format(err))
            self.print_usage()
            sys.exit(2)

        if len(opts) == 0:
            self.print_usage()
            sys.exit(2)

        for opt, arg in opts:
            if opt in ("-H", "--help"):
                self.__parsed += " --help"
                self.print_usage()
                sys.exit(0)
            elif opt in ("-S", "--source"):
                self.__source = arg
                option = ' --source="{}"' if " " in self.__source else ' --source={}'
            elif opt in ("-K", "--key"):
                self.__key = arg
                option = ' --key="{}"' if " " in self.__key else ' --key={}'
            elif opt in ("-N", "--name"):
                self.__name = arg
                option = ' --name="{}"' if " " in self.__name else ' --name={}'
            elif opt in ("-R", "--result"):
                self.__result = arg
                option = ' --result="{}"' if " " in self.__result else ' --result={}'
            elif opt in ("-C", "--case"):
                self.__case = True
                option = "{}"
            else:
                print("invalid options!\n")
                self.print_usage()
                sys.exit(2)
            self.__parsed += option.format(arg)
        else:
            print(self)

    def add_handlers(self, opts_handler):
        if self.name:
            opts_handler.add_name_handler(HasHandler(self.name))
        if self.key:
            opts_handler.add_key_handler(HasHandler(self.key))

    @property
    def source(self):
        return self.__source

    @property
    def key(self):
        return self.__key

    @property
    def name(self):
        return self.__name

    @property
    def result(self):
        return self.__result

    @property
    def case(self):
        return self.__case

    @staticmethod
    def print_usage():
        print("""
    Usage:
        find [Options]
    Options:
        -H, --help     : show this help message.
        -S, --source   : source dir.
        -K, --key      : key word to find.
        -N, --name     : file name to find.
        -R, --result   : the file to output result.
        -C, --case     : case.
        """)


def main():
    cmd_opts = CmdOptsParser()
    cmd_opts.parse_options(sys.argv)
    opts_handler = OptsHandler(cmd_opts.source)
    cmd_opts.add_handlers(opts_handler)
    opts_handler.handle(cmd_opts.source)


if __name__ == "__main__":
    main()
