var openssl = require('../../../index.js');
var assert = require('assert');
var cipher;

describe('aes128-ctr', function(){
  before(function(){
    cipher = openssl();
  });

  it('should encrypt a buffer successfully', function(){
    var actual_buffer = cipher.aes128_ctr_encrypt(new Buffer('thiskeyisverybad', 'ascii'), new Buffer("thisblockverybadthisclockbery", 'ascii'));
    var expected_buffer = new Buffer('c5648533a4ac43d457d169615f30b98a79ea83b51fd59e11813b484ebf', 'hex');
    assert.deepEqual(actual_buffer, expected_buffer, "The buffer ciphertext did not match the expected ciphertext");
  });

  it('should decrypt a buffer successfully', function(){
    
  });
});
