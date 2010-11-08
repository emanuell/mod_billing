import unittest
from mockito import *
from batch.billing import BillingMonitor, BillingServer


class BillingServerTest(unittest.TestCase):
	
	
	def test_should_send_post_request(self):
		
		server = BillingServer('localhost', '/cloudmonitor', 7777)
		
		assert server.host == 'localhost'
		assert server.port == 7777
		
		params = {'app' : 'test', 'value' : 123, 'btype' : 'cpu'}
		
		self.assertEquals(server.send('post', params), 'guid:app:test:value:123:btype:cpu:aisodasdlldkf')
		self.assertEquals(server.send('post', None), 'guid:app::value::btype::aisodasdlldkf')
		
	


class BillingTest(unittest.TestCase):
	
	def setUp(self):
		self.log = open('test/batch/log.txt', 'r')
	
	def tearDown(self):
		self.log.close()
	
	def test_should_read_log_and_return_billing(self):
		billing_server = mock()
		monitor = BillingMonitor(billing_server, 'FirstCloudApp')
		
		billing_urls = monitor.collect(self.log)
		
		assert billing_urls
		
		self.assertTrue(billing_urls.has_key('/teste1/'))
		self.assertTrue(billing_urls.has_key('/teste2/'))
		self.assertTrue(billing_urls.has_key('/teste4/'))
		
		self.assertEquals(billing_urls['/teste1/'], 18.902)
		self.assertEquals(billing_urls['/teste2/'], 9.001)
		self.assertEquals(billing_urls['/teste4/'], 2.123)
		
#	def test_should_watch_log_and_send_request(self):
#		billing_server = mock()
#		when(billing_server).send().thenReturn('ok')
#		
#		monitor  = BillingMonitor(billing_server)
#		monitor.watch()
#		
#		assert monitor.urls
	

if __name__ == '__main__':
	unittest.main()