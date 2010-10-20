import sys, os, time, urllib

class BillingServer(object):
	
	def __init__(self, host, uri, port=None):
		self.host = host
		self.uri = uri
		
		if not port:
			self.port = 80
		else:
			self.port = port
	
	def send(self, method, params=None):
		if params:
			p = urllib.urlencode(params)
		
		url = 'http://' + self.host + ':' + str(self.port) + '/' + self.uri
		if method == 'post':
			f = urllib.urlopen(url, p)
		else:
			f = urllib.urlopen(url + '?%s' % p)
		
		return f.read()


class BillingMonitor(object):
	
	def __init__(self, server, appname):
		self.server = server
		self.app = appname
	
	def watch(self, log):
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
				
				changed = False
				if (urls.has_key(url)):		
					total = round(urls[url] + float(value), 3)
					if total > urls[url]:
						changed = True
										
					urls[url] = total
				else:
					urls[url] = round(float(value), 3)
					changed = True
				
				if changed:
					server.send('post', {'app' : self.app, 'value' : urls[url]})
			
			
		
	
	
	def collect(self, log):
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
	billing_server = BillingServer('127.0.0.1', '/cloudmonitor', 80)
	monitor = BillingMonitor(billing_server, 'FirstApp')
	
	monitor.watch(open('/var/log/apache2/error.log', 'r'))




if __name__ == '__main__':
	main()
