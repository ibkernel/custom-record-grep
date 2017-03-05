#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import re, os, signal, sys
''' 這個檔案是用來合併下載完的章節，並加上額外delimeter(tags)好讓搜尋更為準確
		delimeter detail:
			<tag+tags num> 
			- <t數字>	章節名稱
			-	<l數字>	句分割
			- <p數字>	段分割
			- <c數字>	章節分割
			- <b數字>	書分割
		
		檔案剖析：
			章節名稱、一個句子(。|.|!|?）、一個段落（\n）、一個章節(a new file)、一本書(a new dir)
		
		相關程度剖析：
			（多重字串搜尋）
				-同一行
				-同一段
				-同一章
'''

'''
original text:
第一張_掃地小斯(file name with ".txt & prefix num" removed)

人群中有人面露不忍，輕聲道：“這么多人，有些過分了啊。”
當下便有人回道：“你覺得人多也可以走啊，沒人要你留下來。”
一句話便讓那人訕訕不言，大家都知道為什么聚集在這里，都知道為什么要盯著楊開，現在就是在等那一刻的到來。時限馬上就要到了，現在離去豈不是太可惜？若能搶了頭籌，今日就又是一筆收獲呀。
周旁的動靜楊開自然是知道的，只不過他的神色一直未曾變過。這樣的陣仗自己每五天就要經歷一次，一個月六次，實在沒什么好大驚小怪的，而且看眼前這人數也略少了一些，應該是還沒全到的緣故。
所以他一直在掃地，對周旁之人不管不問，一路走一路掃。
隨著時間的流逝，聚集在楊開身邊的人越來越多，粗略估計至少也有三四十人之多。

processed text:
<b1><c1><t1>第一張_掃地小斯</t1><p1><l1>人群中有人面露不忍，輕聲道：“這么多人，有些過分了啊</l1>”<p1>
當下便有人回道：“你覺得人多也可以走啊，沒人要你留下來<l2>”<p2>
一句話便讓那人訕訕不言，大家都知道為什么聚集在這里，都知道為什么要盯著楊開，現在就是在等那一刻的到來<l3>時限馬上就要到了，現在離去豈不是太可惜<l4>若能搶了頭籌，今日就又是一筆收獲呀<l5><p3>
周旁的動靜楊開自然是知道的，只不過他的神色一直未曾變過<l6>這樣的陣仗自己每五天就要經歷一次，一個月六次，實在沒什么好大驚小怪的，而且看眼前這人數也略少了一些，應該是還沒全到的緣故<l7><p4>
所以他一直在掃地，對周旁之人不管不問，一路走一路掃<l8><p5>
隨著時間的流逝，聚集在楊開身邊的人越來越多，粗略估計至少也有三四十人之多<l9><p6></c1></b1>

Re-format rules:
remove non-sense
replace all "\"" and " " with ""

in dir -> <b1>
in file -> <c1>
	-> first will always be <t>
|->	-> add <p> when detected /n or imediatly after t
|		|- -> add <l> immediately after <p> or after close <l>
|		|		-> close <l> after detect pattern
		|___|
|	-> close <l> then close <p> when new line 
|__|
 


Search method and ranking method:
建立memory location <-> tag name mapping (dict)
[1] -> <b1>
[5] -> <c1>
[9] -> <t1>
[20] -> </t1>
[30] -> <l1>
[33] -> <p1>
[43] -> <l2>
[52] -> <p3>
[67] -> <l3>
[74] -> <l4>
[86] -> <l5>
[89] -> <p3>
[100] -> <l6>
[113] -> <l7>
[115] -> <p4>
[123] -> <l8>
[132] -> <p5>
[156] -> <l9>
[159] -> <p6>
[160] -> </c1>
[163] -> </b1>

	Single pattern:
		search(pattern) -> find all location (assume 10) -> between 9, 20 it's a title -> add title score

	Multi pattern:
		search(p1,p2,p3) -> find all location 
												-> assume result p1: 10,23,42; p2: None; p3: 19,55 -> no score
												-> assume result p1:28,123; p2:42,55; p3:231,42 -> has score
																-> 
'''


class BookFormater:
	path_to_formatted_dir = "./formattedData/"
	path_to_download_dir = "./downloadedData/"
	path_to_merged_file = "./mergedlist.txt"
	path_to_downloaded_list = "./dwlist.txt"

	non_sense = ["請記住本站域名: 黃金屋", "\"", "”","“", " "]

	def __init__(self):
		pass

	def mergeChaptersIntoBooks(self):
		to_merge_titles = self.getNotMergeBookList()
		for book_title in to_merge_titles:
			self.merge(book_title)

	'''讀dwlist.txt，除了最後一本書，還有可能沒下載全，不進行合併'''
	def getNotMergeBookList(self):

		# TODO:read already_merged.txt

		# read dwlist.txt
		downloaded_book_title = []
		download_list_file = open(BookFormater.path_to_downloaded_list, 'r')
		for line in download_list_file:
			downloaded_book_title.append(line.split("\t")[0])
		download_list_file.close()

		return downloaded_book_title


	def merge(self, book_title):
	#def format(self, text): # Just for local testing
		#text = u"{}".format(text.decode('utf-8'))
		source_path = BookFormater.path_to_download_dir + book_title
		destination_path = BookFormater.path_to_formatted_dir + book_title + '.txt'
		formatted_text = ""
		b_num, c_num, t_num, p_num, l_num = 1,1,0,0,0

		to_merge_chapters_path = [source_path+'/'+f for f in os.listdir(source_path+'/') if os.path.isfile(source_path+'/'+f)]

		destination_file = open(destination_path, 'w')
		# initial setup for destination file
		destination_file.write('@id:{}\n'.format(book_title))
		destination_file.write('@title:{}\n'.format(book_title))
		destination_file.write('@content:')
		formatted_text += "<b"+str(b_num)+">"
		print('Merging {}'.format(book_title))
		for chapter_path in to_merge_chapters_path:
			chapter_file = open(chapter_path, 'r')
			print('Current chapter: {}'.format(chapter_file))
			text = ""
			for line in chapter_file:
				text += line
			for nonsense in BookFormater.non_sense:
				#text = text.replace(nonsense.decode('utf-8'), "")
				text = text.replace(nonsense, "")
			title_flag = True
			formatted_text += "<c"+str(c_num)+">"
			for line in text.split("\n"):
				if line:
					if not title_flag:
						p_num += 1
						formatted_text += ("<p_"+str(p_num)+">")
						line , l_num = self.lineFormatter(line, l_num)
						formatted_text += line
						formatted_text += ("</p_"+str(p_num)+">")
						#print("{}".format(line.encode('utf-8')))
					else:
						t_num += 1
						formatted_text += ("<t_"+str(t_num)+">")
						formatted_text += line 
						formatted_text += ("</t_"+str(t_num)+">")
						title_flag = False
			formatted_text += "</c"+str(c_num)+">"
			c_num += 1
			chapter_file.close()
			destination_file.write(formatted_text)
			formatted_text = ""
			
		formatted_text += "</b"+str(b_num)+">"
		destination_file.write('\n')
		destination_file.close()
		print('End merging {}'.format(book_title))
		#return formatted_text
		#print(formatted_text)

	def lineFormatter(self, line_content, l_num):
		# find ? 。 . ... !
		pattern = re.compile('[!?。？！。]|(\.+)')
		new_content_list = re.split(pattern, line_content)
		processed_content = ""
		for content in new_content_list:
			if content:
				l_num += 1
				processed_content += ("<l_"+str(l_num)+">")
				processed_content += content
				processed_content += ("</l_"+str(l_num)+">")

		return processed_content, l_num


print (sys.version)
formater = BookFormater()
formater.mergeChaptersIntoBooks()
















