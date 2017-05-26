# Copyright (c) 2016 Rick van Miltenburg, NHTV Breda University of Applied Sciences
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
# associated documentation files (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute,
# sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all copies or
# substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

'''
	An automated watchdog service watching the asset folders. Upon any modification of any of the
	assets, the asset will automatically be recompiled, allowing error reporting to occur
	while the asset is still being edited.

	Note that only the assets explicitly setup to be compiled will be compiled. Refer to the
	"config" block.
	
	Also note that an asset compile will exclusively trigger when the asset itself is edited,
	not when any of its dependencies (eg textures) are.


	EXTERNAL DEPENDENCIES:
		- Python (created for Python 3.5.2, but newer versions SHOULD?!? work)
		- watchdog (cmd -> "pip install watchdog")
'''


####################################################################################################
####################################################################################################
# Config

watchdog_path = "vktut/assets"
assets = {
	"vk" : {
		"vktut/assets/shaders/shadow_v.glsl" : [
			{ 'stage': 'vert', 'out': 'bin/assets/shaders/shadow_v.spv' },
		],
		"vktut/assets/shaders/forward_v.glsl" : [
			{ 'stage': 'vert', 'out': 'bin/assets/shaders/forward_v.spv' },
		],
		"vktut/assets/shaders/forward_f.glsl" : [
			{ 'stage': 'frag', 'out': 'bin/assets/shaders/forward_f.spv' },
		],
		"vktut/assets/shaders/post_v.glsl" : [
			{ 'stage': 'vert', 'out': 'bin/assets/shaders/post_v.spv' },
		],
		"vktut/assets/shaders/post_f.glsl" : [
			{ 'stage': 'frag', 'out': 'bin/assets/shaders/post_f.spv' },
		],
	},
	"models" : {
		"vktut/assets/models/cube.obj" : [
			{ 'out': 'bin/assets/models/cube.bobj' },
		],
		"vktut/assets/models/texcube.obj" : [
			{ 'out': 'bin/assets/models/texcube.bobj' },
		],
		"vktut/assets/models/cylinder.obj" : [
			{ 'out': 'bin/assets/models/cylinder.bobj' },
		],
		"vktut/assets/models/sponza.obj" : [
			{ 'out': 'bin/assets/models/sponza.bobj' },
		],
	}
}

####################################################################################################
####################################################################################################
## http://stackoverflow.com/a/37505496

import msvcrt
import atexit
import ctypes
from ctypes import wintypes

kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)

# input flags
ENABLE_PROCESSED_INPUT = 0x0001
ENABLE_LINE_INPUT      = 0x0002
ENABLE_ECHO_INPUT      = 0x0004
ENABLE_WINDOW_INPUT    = 0x0008
ENABLE_MOUSE_INPUT     = 0x0010
ENABLE_INSERT_MODE     = 0x0020
ENABLE_QUICK_EDIT_MODE = 0x0040

# output flags
ENABLE_PROCESSED_OUTPUT   = 0x0001
ENABLE_WRAP_AT_EOL_OUTPUT = 0x0002
ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004 # VT100 (Win 10)

def check_zero(result, func, args):
	if not result:
		err = ctypes.get_last_error()
		if err:
			raise ctypes.WinError(err)
	return args

if not hasattr(wintypes, 'LPDWORD'): # PY2
	wintypes.LPDWORD = ctypes.POINTER(wintypes.DWORD)

kernel32.GetConsoleMode.errcheck= check_zero
kernel32.GetConsoleMode.argtypes = (
	wintypes.HANDLE,   # _In_  hConsoleHandle
	wintypes.LPDWORD,) # _Out_ lpMode

kernel32.SetConsoleMode.errcheck= check_zero
kernel32.SetConsoleMode.argtypes = (
	wintypes.HANDLE, # _In_  hConsoleHandle
	wintypes.DWORD,) # _Out_ lpMode

####################################################################################################
####################################################################################################
## http://stackoverflow.com/a/37505496 (cont)

def get_console_mode(output=False):
	'''Get the mode of the active console input or output
	   buffer. Note that if the process isn't attached to a
	   console, this function raises an EBADF IOError.
	'''
	device = r'\\.\CONOUT$' if output else r'\\.\CONIN$'
	with open(device, 'r+') as con:
		mode = wintypes.DWORD()
		hCon = msvcrt.get_osfhandle(con.fileno())
		kernel32.GetConsoleMode(hCon, ctypes.byref(mode))
		return mode.value

def set_console_mode(mode, output=False):
	'''Set the mode of the active console input or output
	   buffer. Note that if the process isn't attached to a
	   console, this function raises an EBADF IOError.
	'''
	device = r'\\.\CONOUT$' if output else r'\\.\CONIN$'
	with open(device, 'r+') as con:
		hCon = msvcrt.get_osfhandle(con.fileno())
		kernel32.SetConsoleMode(hCon, mode)

####################################################################################################
####################################################################################################
# http://stackoverflow.com/a/287944 (Slightly modified)

class bcolors:
	HEADER = '\033[95m'
	OKBLUE = '\033[94m'
	OKGREEN = '\033[92m'
	WARNING = '\033[93m'
	FAIL = '\033[41m'
	ENDC = '\033[0m'
	BOLD = '\033[1m'
	UNDERLINE = '\033[4m'

####################################################################################################
####################################################################################################

import sys
import os
import time
import logging
import subprocess
import datetime
import hashlib
from watchdog.observers import Observer
from watchdog.events import LoggingEventHandler
from watchdog.events import FileModifiedEvent

set_console_mode(ENABLE_VIRTUAL_TERMINAL_PROCESSING|0x8|0x1,True)

def LogLine(str):
	with open("asset_watchdog.log", "a") as log_file:
		log_str = ("[" + datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S') + "]:\n" + str).replace("\n", "\n\t").replace(os.path.dirname(os.path.realpath(__file__)) + "\\", "")
		log_file.write(log_str + "\n")
		print(log_str)

def ProcessFile(file):
	file = file.replace("\\", "/")
	valueVK     = assets["vk"].get(file)
	valueModels = assets["models"].get(file)
	if valueVK is not None:
		for shader in valueVK:
			prefix = "shader:\n\tfile   : " + file + "\n\tstage  : " + shader["stage"] + "\n\tout    : " + shader["out"] + "\n"

			if "defines" in shader:
				prefix += "\tdefines:\n"
				keyLen  = 1
				for key, value in shader["defines"].items():
					keyLen = max(keyLen, len(key))

				for key, value in shader["defines"].items():
					prefix += "\t\t- " + key
					for x in range(len(key),keyLen):
						prefix += " "
					prefix += " = " + value + "\n"

			with open(file,"rb") as f:
				h = hashlib.md5 ( )
				content = f.read()
				h.update ( content )
				hash = h.hexdigest()
				if "hash" in shader and hash == shader["hash"]:
					continue;
				shader["hash"] = hash

			try:
				params = ["glslangValidator", "-V", "-S", shader["stage"], "-o", shader["out"], file]
				out = subprocess.check_output(params, stderr=subprocess.STDOUT, shell=True)
				LogLine(prefix + "\n")##+ out.decode("UTF-8"))
				## Log output for success case disabled to make errors more clear
			except subprocess.CalledProcessError as e:
				LogLine(bcolors.FAIL + "\a"+prefix + "\n" + e.output.decode("UTF-8") + bcolors.ENDC)
	elif valueModels is not None:
		for outModel in valueModels:
			prefix = "model:\n\tfile: " + file + "\n\tout : " + outModel["out"] + "\n"

			with open(file,"rb") as f:
				h = hashlib.md5 ( )
				content = f.read()
				h.update ( content )
				hash = h.hexdigest()
				if "hash" in outModel and hash == outModel["hash"]:
					continue;
				outModel["hash"] = hash

			try:
				params = ["tools\\mconv.exe", file, outModel["out"]]
				out = subprocess.check_output(params, stderr=subprocess.STDOUT, shell=True)
				LogLine(prefix + "\n" + out.decode("UTF-8"))
			except subprocess.CalledProcessError as e:
				LogLine(bcolors.FAIL + "\a"+prefix + "\n" + e.output.decode("UTF-8") + bcolors.ENDC)
	else:
		LogLine ( "File " + file + " does not have any compile entries" )

class FileChangeEventHandler (LoggingEventHandler):
	def on_modified(self, event):
		if type(event) is FileModifiedEvent:
			ProcessFile(event.src_path)

if __name__ == "__main__":
	for key, value in assets["vk"].items():
		ProcessFile(key)
	for key, value in assets["models"].items():
		ProcessFile(key)
	logging.basicConfig(level=logging.INFO,
						format='%(asctime)s - %(message)s',
						datefmt='%Y-%m-%d %H:%M:%S')
	event_handler = FileChangeEventHandler()
	observer = Observer()
	observer.schedule(event_handler, watchdog_path, recursive=True)
	observer.start()
	try:
		while True:
			time.sleep(100)
	except KeyboardInterrupt:
		observer.stop()
	observer.join()
