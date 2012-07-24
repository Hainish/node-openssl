var openssl = require('../../../index.js');
var assert = require('assert');
var cipher;

describe('aes128-ctr', function(){
  var actual_ciphertext_buffer;
  var key = new Buffer('thiskeyisverybad', 'ascii');
  var pt = new Buffer("thisblockverybadthisclockbery", 'ascii');
  var expected_ciphertext_buffer = new Buffer('c5648533a4ac43d457d169615f30b98a79ea83b51fd59e11813b484ebf', 'hex');

  before(function(){
    cipher = openssl();
  });

  it('should encrypt a buffer successfully', function(){
    actual_ciphertext_buffer = cipher.aes128_ctr_encrypt(key, pt);
    assert.deepEqual(actual_ciphertext_buffer, expected_ciphertext_buffer, "The buffer ciphertext did not match the expected ciphertext");
  });

  it('should decrypt a buffer successfully', function(){
    var actual_plaintext_buffer = cipher.aes128_ctr_decrypt(key, expected_ciphertext_buffer);
    assert.deepEqual(pt, actual_plaintext_buffer, "The decrypted buffer did not match the original plaintext");
  });

  it('should throw an exception with the wrong argument types', function(){
    assert.throws(function(){
      cipher.aes128_ctr_decrypt(key);
    },
    TypeError);
    assert.throws(function(){
      cipher.aes128_ctr_decrypt("testing123", expected_ciphertext_buffer);
    },
    TypeError);
  });

  it('should throw an exception with keylength != 128 bits', function(){
    assert.throws(function(){
      cipher.aes128_ctr_encrypt(new Buffer('shortkey', ascii), pt);
    },
    Error);
  });
});
