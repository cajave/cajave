#!/usr/bin/env python3
# -*- coding : utf-8 -*-


import time


class Context(object):
	def __init__(self):
		self.is_running = False
		self.logger = None
		self.times = 4
		self.interval = 10
		
		
class Executor(object):
	def __init__(self, ctx):
		self.ctx = ctx
		self.__handler = None
		
	def set_handler(self, handler):
		self.__handler = handler
		
	def exec(self):
		if self.ctx.times <= 0:
			while self.ctx.is_running:
				handler(self.ctx)
				time.sleep(self.ctx.interval)
		else:
			if self.ctx.times > 0:
				while self.ctx.is_running and self.ctx.times > 0:
					handler(self.ctx)
					self.ctx.times -= 1
					time.sleep(self.ctx.interval)

			
class Logger(object):
	def __init__(self):
		pass
		
	def __do_handle(self, msg):
		print(msg)
		
	def handle_error(self, error):
		self.__do_handle(error)
		
	def handle_warning(self, warning):
		self.__do_handle(warning)
		
	def handle_info(self, info):
		self.__do_handle(info)

		
def handler(ctx):
	ctx.logger.handle_error("handle_error")
	ctx.logger.handle_warning("handle_warning")
	ctx.logger.handle_info("handle_info")

		
def main():
	ctx = Context()
	ctx.is_running = True
	ctx.logger = Logger()
	exe = Executor(ctx)
	exe.set_handler(handler)
	exe.exec()


if __name__ == "__main__":
	main()