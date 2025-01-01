#-- coding:utf-8 --

import os
import re

regex = r'/\* begin[\s\w]+\*/([\w\W]+?)}\s*/\* end[\s\w]+\*/'
refile = r'tolua_dobuffer\([^"]+"[\w\/:\s]+/([\w\-.]+.lua)"\);'

def parse_toluac(fpath):
	with open(fpath, 'r') as reader:
		content = reader.read()
		results = re.findall(regex, content, flags=re.M);
		for result in results:
			# 1、获取文件名数据
			search = re.search(refile, result, flags=re.M)
			if not search: continue
			file = search.group(1)
			newpath = os.path.join('lua', file)
			print(newpath)

			# 2、获取文件对应的ASCII内容
			recont = r'char B\[\] = {([\d\s,]+)}'
			search = re.search(recont, result, flags=re.M)
			if not search: continue
			curasc = search.group(1)

			# 3、将ASCII转换成string
			asc_arr = curasc.split(',')
			asc_arr = [int(s.strip()) for s in asc_arr]
			chars_arr = [chr(c) for c in asc_arr]
			with open(newpath, 'w') as wtf:
				wtf.write(''.join(chars_arr))


def parse_luaToasc(fpath):
	all_ascs = []
	with open(fpath, 'r') as reader:
		content = reader.read()
		chsize = len(content)
		for idx in range(0, chsize, 15):
			if idx + 15 > chsize:
				asc_line = [str(ord(c)) for c in content[idx:]]
				all_ascs.append('    '+', '.join(asc_line))
			else:
				asc_line = [str(ord(c)) for c in content[idx:idx+15]]
				all_ascs.append('    '+', '.join(asc_line))
	result = (', \n'.join(all_ascs))
	return ('\n%s\n   '%result)


def parse_ascToc(fpath):
	with open(fpath, 'r') as reader:
		content = reader.read()
		results = re.findall(regex, content, flags=re.M);
		for result in results:
			# 1、获取文件名数据
			search = re.search(refile, result, flags=re.M)
			if not search: continue
			file = search.group(1)
			newpath = os.path.join('lua', file)
			print(newpath)

			# 2、获取文件对应的ASCII内容
			recont = r'char B\[\] = {([\d\s,]+)}'
			search = re.search(recont, result, flags=re.M)
			if not search: continue
			curasc = search.group(1)

			# 3、将Lua文件转换成ASCII
			newasc = parse_luaToasc(newpath)
			content = content.replace(curasc, newasc)

		with open(fpath, 'w') as wtf:
			wtf.write(content)
			wtf.flush()


if __name__ == '__main__':
	#parse_tolua('toluabind.c')
	parse_ascToc('toluabind.c')