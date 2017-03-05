#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from urllib.request import urlopen
from urllib.request import HTTPError, URLError
from socket import timeout
import re, os, signal, sys
from bs4 import BeautifulSoup
from lxml import etree
from urllib.parse import urlparse
import atexit

''' Book downloader
		Given a URL, it should automatically download
		any not yet downloaded books.
		Book's chapters should be put in the same dir 
		(name the dir with the book's name)
		and be processed by another python file after.
		
		Need to print out downloading progress during execution
'''
class Downloader:
	path_to_downloaded_list = "./dwlist.txt"
	path_to_download_dir = "./downloadedData/"
	downloader_timeout = 10

	def __init__(self, web_url):
		self.web_url = web_url
		parsed_uri = urlparse(web_url)
		self.domain_name = "{uri.scheme}://{uri.netloc}/".format(uri=parsed_uri)
		# initiate already downloaded books list from file
		self.downloaded_list = {}
		self.current_downloaded_list = {}
		try:
			f_list = open(Downloader.path_to_downloaded_list, 'r')
			for line in f_list:
				downloaded_list = line.replace("\n", "").split("\t")
				self.downloaded_list[downloaded_list[0]] = downloaded_list[1]
				#self.last_download_book_title = downloaded_list[0]
				#self.last_download_book_link =  downloaded_list[1]
			#print(self.downloaded_list)
			#bookList = [ (line.split("\t")[0].decode('utf-8'), line.split("\t")[1]) for line in f_list]
			f_list.close()
		except:
			print("No book downloaded yet")
		signal.signal(signal.SIGINT, self.signal_handler)

	def getBookList(self):
		try:
			response = urlopen(self.web_url , timeout=Downloader.downloader_timeout)
			htmlparser = etree.HTMLParser()
			tree = etree.parse(response, htmlparser)
			newline_space_pattern = re.compile('[\s]+')
			try:
				tables = tree.xpath("//body/form/div[1]/div[3]//table[3]")
				book_links = []
				i = 0
				for element in tables[0]:
					if i%2 == 0:
						book_info = element.findall('td')[1].find('a')
						book_title = newline_space_pattern.sub("", book_info.text)
						book_link = "{}{}".format(self.domain_name,book_info.attrib['href'].replace('Book/','Book/Chapter/'))
						book_links.append((book_title, book_link))
						#print(book_title)
						i+=1
					else:
						i+=1
				#print(book_links)
			except:
				tables = tree.xpath("//body/form/div[3]/div[3]//table[3]")
				book_links = []
				i = 0
				for element in tables[0]:
					if i%2 == 0:
						book_info = element.findall('td')[1].find('a')
						book_title = newline_space_pattern.sub("", book_info.text) # unicode (.encode('utf8') -> str)
						book_link = "{}{}".format(self.domain_name,book_info.attrib['href'].replace('Book/','Book/Chapter/'))
						book_links.append((book_title, book_link))
						#print(u'{}'.format(book_title))
						i+=1
					else:
						i+=1
				i = 0
			for booktitle, booklink in book_links:
				# 因為book_links順序有可能變，所以還是要確認每一本書都下載完了沒 在else:增加downloadBookWithChecking
				if not booktitle in self.downloaded_list and not booktitle in self.current_downloaded_list:
					print("Current book: {}".format(booktitle))
					print("-----------------------------------")
					self.current_downloaded_list[booktitle] = booklink
					self.downloadBook(booklink, booktitle)
					print("-----------Downloaded--------------")
				# elif booktitle == self.last_download_book_title:
				# 	# check for previous abrupt interrupt
				# 	# if there are chapter not downloaded yet, finish it.
				# 	print("Resume download from last downloading book: {}".format(booktitle))
				# 	print("-----------------------------------")
				# 	self.downloadBookWithChecking(booklink, booktitle)
				# 	print("-----------Finished--------------")
				else:
					print("雖然此書已經下載過，然而book_links順序有可能變，所以還是要確認每一本書都下載完了沒")
					print("Checking book's completeness: {}".format(booktitle))
					print("-----------------------------------")
					self.downloadBookWithChecking(booklink, booktitle)
					print("--------Finished Checking----------")
		except timeout:
			print("Cannot read book list from website.")

		f_list = open(Downloader.path_to_downloaded_list, 'a')
		for booktitle, booklink in self.current_downloaded_list.items():
			f_list.write('{}\t{}\n'.format(booktitle, booklink))
		f_list.close()
		print('All books downloaded~')

	def downloadBookWithChecking(self, book_link, book_title):
		path_to_last_book_dir = Downloader.path_to_download_dir + book_title + '/'
		if not os.path.exists(path_to_last_book_dir):
			os.makedirs(path_to_last_book_dir)
		only_files = [f for f in os.listdir(path_to_last_book_dir) if os.path.isfile(path_to_last_book_dir+f)]
		last_chapter_downloaded = only_files[len(only_files)-1].split("-", 1)[1].replace('.txt', '').replace('_',' ')
		try:
			response = urlopen(book_link, timeout=Downloader.downloader_timeout)
			htmlparser = etree.HTMLParser()
			tree = etree.parse(response, htmlparser)
			chapter_lists = tree.xpath("//div[contains(@id, 'tbchapterlist')] // td // a")
			download_flag = False
			prefix_num = 0
			#print(chapter_lists)
			for link in chapter_lists:
				prefix_num += 1
				#print(link.text)
				if link.text == last_chapter_downloaded:
					download_flag = True
				if download_flag:
					#print(":::", link.text)
					self.download(self.domain_name+link.attrib['href'], link.text.replace(' ','_'), book_title, prefix_num)
		except timeout:
				print('socket timed out - URL %s', book_link)

	def downloadBook(self, book_link, book_title):
		# 1. get all chapter urls
		# 2. download all chapters into dir
		# 3. delete non sense sentences/words using regex
		# 4. add the book list into downloadedList.txt
		response = urlopen(book_link)
		htmlparser = etree.HTMLParser()
		tree = etree.parse(response, htmlparser)
		chapter_lists = tree.xpath("//div[contains(@id, 'tbchapterlist')] // td // a")
		prefix_num = 0
		for link in chapter_lists:
			prefix_num += 1
			self.download(self.domain_name+link.attrib['href'], link.text.replace(' ','_'), book_title, prefix_num)
		
	def download(self, chapter_link, chapter_title, book_title, prefix_num):
		try: 
			response = urlopen(chapter_link, timeout=Downloader.downloader_timeout)
			htmlparser = etree.HTMLParser()
			tree = etree.parse(response, htmlparser)
			directory_path = Downloader.path_to_download_dir+book_title+'/'
			print("Current chapter: {}".format(chapter_title))
			if not os.path.exists(directory_path):
				os.makedirs(directory_path)
			f_chapter = open(directory_path+str(prefix_num).zfill(4)+'-'+chapter_title+'.txt','w')
			try:
				raw_contents = tree.xpath("//div[contains(@id, 'AllySite')]/following-sibling::div[1]")
				for content in raw_contents:
					f_chapter.write("{}".format(("".join([x for x in content.itertext()]))))
			except:
				print(u"Error occurred:{}\t{}".format(chapter_title, chapter_link))
			f_chapter.close()
		except (HTTPError, URLError) as error:
				print('Content of %s not retrieved because %s\nURL: %s', chapter_title, error, chapter_link)
		except timeout:
				print('socket timed out - URL %s', chapter_link)
		else:
				print('Access successful.')


	def signal_handler(self, signum, frame):
		f_list = open(Downloader.path_to_downloaded_list, 'a')
		for booktitle, booklink in self.current_downloaded_list.items():
			f_list.write('{}\t{}\n'.format(booktitle, booklink))
		f_list.close()
		print('Signal {} caugth, exiting application'.format(signum))
		exit(0)

	def init_tester(self):
		pass


if __name__ == '__main__':
	print (sys.version)
	web_url = "http://tw.hjwzw.com/"
	downloader = Downloader(web_url)
	#atexit.register(downloader.exit_handler)
	#downloader.init_tester();
	#print('in second downloader')
	downloader.getBookList()
	#downloader.downloadBook()

