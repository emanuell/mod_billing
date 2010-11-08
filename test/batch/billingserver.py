from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler

class Handler(BaseHTTPRequestHandler):
	
	def do_GET(self):
		print 'GET' + self.path
		
		self.do_POST()
		return
		
	
	def do_POST(self):
		self.send_response(200)
		
		self.send_header('Content-type', 'text/html')	
		self.end_headers()
		
		try:
			params = self.get_params(self.rfile, int(self.headers['Content-Length']))
			
			app_name = params['app']
			billing_value = params['value']
			btype = params['btype']
			
		except KeyError, e:
			params = {}
			app_name = billing_value = btype = ""
		
		self.wfile.write('guid:app:%s:value:%s:btype:%s:aisodasdlldkf' % (app_name, billing_value, btype))
		
		return
	
	def get_params(self, inputstream, length):
		param_as_stream = inputstream.read(length)
		params_as_array = param_as_stream.split('&')
		
		params = {}
		for p in params_as_array:
			key = p.split('=')[0]
			value = p.split('=')[1]
			
			params[key] = value
		
		return params
	


def main():
	server = HTTPServer(('localhost', 7777), Handler)
	print 'listening...'
	server.serve_forever()

if __name__ == '__main__':
	main()