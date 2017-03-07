#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import re, os, signal, sys
from collections import deque
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

	non_sense = ["請記住本站域名: 黃金屋", "\"", "”","“", " ", " "]

	def __init__(self):
		self.already_merged = []
		pass

	def mergeChaptersIntoBooks(self):
		to_merge_titles = self.getNotMergeBookList()
		merged_list_file = open(BookFormater.path_to_merged_file, 'a')
		for book_title in to_merge_titles:
			self.mergeWithoutTags(book_title)
			merged_list_file.write("{}\n".format(book_title))
		merged_list_file.close()

	'''讀dwlist.txt，除了最後一本書，還有可能沒下載全，不進行合併'''
	def getNotMergeBookList(self):

		merged_list_file = open(BookFormater.path_to_merged_file, 'r')
		for line in merged_list_file:
			self.already_merged.append(line.strip("\n"))
		merged_list_file.close()
		# read dwlist.txt
		downloaded_book_title = []
		download_list_file = open(BookFormater.path_to_downloaded_list, 'r')
		for line in download_list_file:
			book_title = line.split("\t")[0]
			if not book_title in self.already_merged:
				downloaded_book_title.append(line.split("\t")[0])
		download_list_file.close()

		return downloaded_book_title

#NOTE: Obey clean code rule as much as possible
	def mergeWithoutTags(self, book_title):
		to_merge_chapters_path = self.getToMergeChaptersPath(book_title)
		if not to_merge_chapters_path:
			return
		self.writeRecordHeaderToFile(book_title)
		self.writeChapterListToFile(book_title, to_merge_chapters_path)

	def writeChapterListToFile(self, book_title, to_merge_chapters_path):
		char_count, chapter_num, title_num, paragraph_num, sentense_num = 0, 0, 0, 0, 0
		for chapter_path in to_merge_chapters_path:
			char_count, chapter_num, title_num, paragraph_num, sentense_num = self.writeChapterToFile(\
				char_count, chapter_path, chapter_num, title_num, paragraph_num, sentense_num, book_title)
		print('End merging {}'.format(book_title))


	def writeChapterToFile(self, char_count, chapter_path, chapter_num, title_num, paragraph_num, sentense_num, book_title):
		print('Current processing chapter: {}'.format(chapter_path))
		chapter_file = open(chapter_path, 'r')
		text = ""
		for line in chapter_file:
			text += line
		text = self.removeNonsense(text)

		destination_path = BookFormater.path_to_formatted_dir + book_title + '.txt'
		destination_file = open(destination_path, 'a')
		title_flag, tag_queue = True, deque()
		tag_queue.append(("c_"+str(chapter_num), char_count))
		for line in text.split("\n"):
			if line:
				if not title_flag:
					paragraph_num += 1
					tag_queue.append(("p_"+str(paragraph_num), char_count))
					line , sentense_num, char_count, tag_queue = self.lineFormatterWithoutTag(line, sentense_num, char_count, tag_queue)
					tag_queue.append(("p_"+str(paragraph_num), char_count))
				else:
					title_num += 1
					tag_queue.append(("t_"+str(title_num), char_count))
					char_count += len(line)*3
					tag_queue.append(("t_"+str(title_num), char_count))
					title_flag = False
				destination_file.write(line)
		tag_queue.append(("c_"+str(chapter_num), char_count))
		chapter_num += 1

		chapter_file.close()
		destination_file.close()
		self.writeTagInfoToFile2(tag_queue, book_title)

		return char_count, chapter_num, title_num, paragraph_num, sentense_num

	def writeTagInfoToFile2(self, tag_queue, book_title):
		tag_info_path = BookFormater.path_to_formatted_dir + book_title + '.info'
		tag_file = open(tag_info_path, 'a')

		open_chapter_tag, open_tag_location = tag_queue.popleft()
		close_chapter_tag, close_tag_location  = tag_queue.pop()
		tag_file.write("{}\t{}\t{}\n".format(open_chapter_tag, open_tag_location, close_tag_location))
		open_title_tag, open_tag_location = tag_queue.popleft()
		close_title_tag, close_tag_location  = tag_queue.popleft()
		tag_file.write("{}\t{}\t{}\n".format(open_title_tag, open_tag_location, close_tag_location))


		other_tag_queue, queue_count = deque(), 0
		for tag, tag_location in tag_queue:
			if not other_tag_queue:
				other_tag_queue.append((tag, tag_location))
			elif tag in other_tag_queue[0]:
				_ , location = other_tag_queue.popleft()
				tag_file.write("{}\t{}\t{}\n".format(tag, location, tag_location))
				for _ in range(len(other_tag_queue)//2):
					open_tag, open_tag_location = other_tag_queue.popleft()
					close_tag, close_tag_location = other_tag_queue.popleft()
					tag_file.write("{}\t{}\t{}\n".format(open_tag, open_tag_location, close_tag_location))
			else:
				other_tag_queue.append((tag, tag_location))
		tag_file.close()

	def removeNonsense(self, text):
		for nonsense in BookFormater.non_sense:
			text = text.replace(nonsense, "")
		return text

	def lineFormatterWithoutTag(self, line_content, sentense_num, char_count, tag_queue):
		# find ? 。 . ... !
		pattern = re.compile('[!?。？！。]|(\.+)')
		new_content_list = re.split(pattern, line_content)
		refined_text = ""
		for content in new_content_list:
			if content:
				sentense_num += 1
				tag_queue.append(("s_"+str(sentense_num), char_count))
				char_count += len(content)*3
				tag_queue.append(("s_"+str(sentense_num), char_count))
				refined_text += content

		return refined_text, sentense_num, char_count, tag_queue


	def writeRecordHeaderToFile(self, book_title):
		destination_path = BookFormater.path_to_formatted_dir + book_title + '.txt'
		destination_file = open(destination_path, 'w')
		id_meta_content = "@id:{}\n".format(book_title)
		title_meta_content = "@title:{}\n".format(book_title)
		destination_file.write('{}'.format(id_meta_content))
		destination_file.write('{}'.format(title_meta_content))
		destination_file.write('@content:')


	def getToMergeChaptersPath(self, book_title):
		book_path = BookFormater.path_to_download_dir + book_title
		return [book_path+'/'+f for f in os.listdir(book_path+'/') if os.path.isfile(book_path+'/'+f)]

	''' Merge implementation with tags, No longer used

	def lineFormatter(self, line_content, s_num, char_count, tag_queue):
		# find ? 。 . ... !
		pattern = re.compile('[!?。？！。]|(\.+)')
		new_content_list = re.split(pattern, line_content)
		processed_content = ""
		for content in new_content_list:
			if content:
				s_num += 1
				tag_queue.append(("s_"+str(s_num), char_count+1))
				char_count += len("<s_"+str(s_num)+">")
				processed_content += ("<s_"+str(s_num)+">")

				char_count += len(content)
				processed_content += content

				tag_queue.append(("s_"+str(s_num), char_count+1))
				char_count += len("</s_"+str(s_num)+">")
				processed_content += ("</s_"+str(s_num)+">")

		return processed_content, s_num, char_count, tag_queue

	def mergeWithTags(self, book_title):
		#text = u"{}".format(text.decode('utf-8'))
		source_path = BookFormater.path_to_download_dir + book_title
		destination_path = BookFormater.path_to_formatted_dir + book_title + '.txt'
		formatted_text = ""
		b_num, c_num, t_num, p_num, s_num = 1,1,0,0,0
		char_count = 0;

		to_merge_chapters_path = [source_path+'/'+f for f in os.listdir(source_path+'/') if os.path.isfile(source_path+'/'+f)]

		destination_file = open(destination_path, 'w')
		
		id_meta_content = "@id:{}\n".format(book_title)
		title_meta_content = "@title:{}\n".format(book_title)
		char_count += len(id_meta_content)
		char_count += len(title_meta_content)
		char_count += len("@content:")
		destination_file.write('{}'.format(id_meta_content))
		destination_file.write('{}'.format(title_meta_content))
		destination_file.write('@content:')

		formatted_text += "<b"+str(b_num)+">"
		char_count += len(formatted_text)
		print('Merging {}'.format(book_title))
		for chapter_path in to_merge_chapters_path:
			chapter_file = open(chapter_path, 'r')
			print('Current chapter: {}'.format(chapter_file))

			tag_queue = deque()

			text = ""
			for line in chapter_file:
				text += line
			for nonsense in BookFormater.non_sense:
				#text = text.replace(nonsense.decode('utf-8'), "")
				text = text.replace(nonsense, "")

			tag_queue.append(("c_"+str(c_num), char_count+1))
			formatted_text += "<c"+str(c_num)+">"
			char_count += len("<c"+str(c_num)+">")
			title_flag = True
			for line in text.split("\n"):
				if line:
					if not title_flag:
						p_num += 1
						tag_queue.append(("p_"+str(p_num), char_count+1))
						char_count += len("<p_"+str(p_num)+">")
						formatted_text += ("<p_"+str(p_num)+">")

						line , s_num, char_count, tag_queue = self.lineFormatter(line, s_num, char_count, tag_queue)
						formatted_text += line

						tag_queue.append(("p_"+str(p_num), char_count+1))
						char_count += len("</p_"+str(p_num)+">")
						formatted_text += ("</p_"+str(p_num)+">")
						#print("{}".format(line.encode('utf-8')))
					else:
						t_num += 1
						tag_queue.append(("t_"+str(t_num), char_count+1))
						char_count += len("<t_"+str(t_num)+">")
						formatted_text += ("<t_"+str(t_num)+">")
						char_count += len(line)
						formatted_text += line
						tag_queue.append(("t_"+str(t_num), char_count+1))
						char_count += len("</t_"+str(t_num)+">")
						formatted_text += ("</t_"+str(t_num)+">")
						title_flag = False
			tag_queue.append(("c_"+str(c_num), char_count+1))
			char_count += len("</c"+str(c_num)+">")
			formatted_text += "</c"+str(c_num)+">"
			c_num += 1
			chapter_file.close()
			destination_file.write(formatted_text)
			formatted_text = ""
			self.writeTagInfoToFile(tag_queue, book_title)
			
		formatted_text += "</b"+str(b_num)+">"
		destination_file.write(formatted_text)
		destination_file.write('\n')
		destination_file.close()
		print('End merging {}'.format(book_title))
		#return formatted_text
		#print(formatted_text)

	def writeTagInfoToFile(self, tag_queue, book_title):
		tag_info_path = BookFormater.path_to_formatted_dir + book_title + '.info'
		tag_file = open(tag_info_path, 'a')


		open_chapter_tag, open_tag_location = tag_queue.popleft()
		close_chapter_tag, close_tag_location  = tag_queue.pop()
		tag_file.write("{}\t{}\t{}\n".format(open_chapter_tag, open_tag_location, close_tag_location))
		open_title_tag, open_tag_location = tag_queue.popleft()
		close_title_tag, close_tag_location  = tag_queue.popleft()
		tag_file.write("{}\t{}\t{}\n".format(open_title_tag, open_tag_location, close_tag_location))


		other_tag_queue, queue_count = deque(), 0
		for tag, tag_location in tag_queue:
			if not other_tag_queue:
				other_tag_queue.append((tag, tag_location))
			elif tag in other_tag_queue[0]:
				_ , location = other_tag_queue.popleft()
				tag_file.write("{}\t{}\t{}\n".format(tag, location, tag_location))
				for _ in range(len(other_tag_queue)//2):
					open_tag, open_tag_location = other_tag_queue.popleft()
					close_tag, close_tag_location = other_tag_queue.popleft()
					tag_file.write("{}\t{}\t{}\n".format(open_tag, open_tag_location, close_tag_location))
			else:
				other_tag_queue.append((tag, tag_location))
		tag_file.close()
'''

if __name__ == '__main__':
	print (sys.version)
	formater = BookFormater()
	formater.mergeChaptersIntoBooks()
















