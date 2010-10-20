import unittest
from batch.billing import Billing

class BillingTest(unittest.TestCase):
	
	def setUp(self):
		self.log = open('test/batch/log.txt', 'r')
	
	def tearDown(self):
		self.log.close()
	
	def test_should_read_log_and_return_billing(self):
		monitor = Billing()
		
		billing_urls = monitor.collect(self.log)
		
		assert billing_urls
		
		self.assertTrue(billing_urls.has_key('/teste1/'))
		self.assertTrue(billing_urls.has_key('/teste2/'))
		self.assertTrue(billing_urls.has_key('/teste4/'))
		
		self.assertEquals(billing_urls['/teste1/'], 18.902)
		self.assertEquals(billing_urls['/teste2/'], 9.001)
		self.assertEquals(billing_urls['/teste4/'], 2.123)
		
	def test_should_watch_log_and_send_request(self):
		

if __name__ == '__main__':
	unittest.main()