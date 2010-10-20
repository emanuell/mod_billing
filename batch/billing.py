import sys, os, time


class Billing(object):
	"""docstring for Billing"""
	
	def __init__(self):
		pass
	
	def watch(self, log):
		"""docstring for watch"""
		
		urls = {}
		
		log.seek(0, log.SEEK_END)
		
		while(True):
			line = log.readline()
			
			empty = True if line == '' else False
			found = True if line.find('Billing') != -1 else False
			
			if (found and not empty):
				billing_index = line.find('Billing')
				start = (billing_index + len('Billing '))
				end = line.find(']', start)
				
				url = line[start:end]
				
				line_as_array = line.split()
				value = line_as_array[len(line_as_array) - 1]
				
				if (urls.has_key(url)):
					urls[url] = round(urls[url] + float(value), 3)
				else:
					urls[url] = round(float(value), 3)
			
			time.sleep(5)
	
	
	
	def collect(self, log):
		"""docstring for collect"""
		
		urls = {}

		log.seek(0)

		for line in log:
			empty = True if line == '' else False
			found = True if line.find('Billing') != -1 else False

			if (found and not empty):
				billing_index = line.find('Billing')
				start = (billing_index + len('Billing '))
				end = line.find(']', start)

				url = line[start:end]

				line_as_array = line.split()
				value = line_as_array[len(line_as_array) - 1]

				if (urls.has_key(url)):
					urls[url] = round(urls[url] + float(value), 3)
				else:
					urls[url] = round(float(value), 3)
		
		return urls


def main():
	
	urls = {}
		
	file = open('/var/log/apache2/error.log', 'r')
	file.seek(0)
	
	while(True):
		line = file.readline()
		
		empty = True if line == '' else False
		found = True if line.find('Billing') != -1 else False
		
		if (found and not empty):
			billing_index = line.find('Billing')
			start = (billing_index + len('Billing '))
			end = line.find(']', start)
			
			url = line[start:end]
			
			line_as_array = line.split()
			value = line_as_array[len(line_as_array) - 1]
			
			if (urls.has_key(url)):
				urls[url] = urls[url] + float(value)
			else:
				urls[url] = float(value)
		print urls
		time.sleep(1);
	
	file.close()



if __name__ == '__main__':
	main()
